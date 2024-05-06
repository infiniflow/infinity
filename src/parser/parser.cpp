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
  YYSYMBOL_UNION = 20,                     /* UNION  */
  YYSYMBOL_ALL = 21,                       /* ALL  */
  YYSYMBOL_INTERSECT = 22,                 /* INTERSECT  */
  YYSYMBOL_COMPACT = 23,                   /* COMPACT  */
  YYSYMBOL_EXCEPT = 24,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 25,                     /* FLUSH  */
  YYSYMBOL_USE = 26,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 27,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 28,                /* PROPERTIES  */
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
  YYSYMBOL_BLOCKS = 49,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 50,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 51,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 52,                   /* INDEXES  */
  YYSYMBOL_GROUP = 53,                     /* GROUP  */
  YYSYMBOL_BY = 54,                        /* BY  */
  YYSYMBOL_HAVING = 55,                    /* HAVING  */
  YYSYMBOL_AS = 56,                        /* AS  */
  YYSYMBOL_NATURAL = 57,                   /* NATURAL  */
  YYSYMBOL_JOIN = 58,                      /* JOIN  */
  YYSYMBOL_LEFT = 59,                      /* LEFT  */
  YYSYMBOL_RIGHT = 60,                     /* RIGHT  */
  YYSYMBOL_OUTER = 61,                     /* OUTER  */
  YYSYMBOL_FULL = 62,                      /* FULL  */
  YYSYMBOL_ON = 63,                        /* ON  */
  YYSYMBOL_INNER = 64,                     /* INNER  */
  YYSYMBOL_CROSS = 65,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 66,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 67,                     /* WHERE  */
  YYSYMBOL_ORDER = 68,                     /* ORDER  */
  YYSYMBOL_LIMIT = 69,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 70,                    /* OFFSET  */
  YYSYMBOL_ASC = 71,                       /* ASC  */
  YYSYMBOL_DESC = 72,                      /* DESC  */
  YYSYMBOL_IF = 73,                        /* IF  */
  YYSYMBOL_NOT = 74,                       /* NOT  */
  YYSYMBOL_EXISTS = 75,                    /* EXISTS  */
  YYSYMBOL_IN = 76,                        /* IN  */
  YYSYMBOL_FROM = 77,                      /* FROM  */
  YYSYMBOL_TO = 78,                        /* TO  */
  YYSYMBOL_WITH = 79,                      /* WITH  */
  YYSYMBOL_DELIMITER = 80,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 81,                    /* FORMAT  */
  YYSYMBOL_HEADER = 82,                    /* HEADER  */
  YYSYMBOL_CAST = 83,                      /* CAST  */
  YYSYMBOL_END = 84,                       /* END  */
  YYSYMBOL_CASE = 85,                      /* CASE  */
  YYSYMBOL_ELSE = 86,                      /* ELSE  */
  YYSYMBOL_THEN = 87,                      /* THEN  */
  YYSYMBOL_WHEN = 88,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 89,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 90,                   /* INTEGER  */
  YYSYMBOL_INT = 91,                       /* INT  */
  YYSYMBOL_TINYINT = 92,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 93,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 94,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 95,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 96,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 97,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 98,                    /* DOUBLE  */
  YYSYMBOL_REAL = 99,                      /* REAL  */
  YYSYMBOL_DECIMAL = 100,                  /* DECIMAL  */
  YYSYMBOL_DATE = 101,                     /* DATE  */
  YYSYMBOL_TIME = 102,                     /* TIME  */
  YYSYMBOL_DATETIME = 103,                 /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 104,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 105,                     /* UUID  */
  YYSYMBOL_POINT = 106,                    /* POINT  */
  YYSYMBOL_LINE = 107,                     /* LINE  */
  YYSYMBOL_LSEG = 108,                     /* LSEG  */
  YYSYMBOL_BOX = 109,                      /* BOX  */
  YYSYMBOL_PATH = 110,                     /* PATH  */
  YYSYMBOL_POLYGON = 111,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 112,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 113,                     /* BLOB  */
  YYSYMBOL_BITMAP = 114,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 115,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 116,                   /* VECTOR  */
  YYSYMBOL_BIT = 117,                      /* BIT  */
  YYSYMBOL_PRIMARY = 118,                  /* PRIMARY  */
  YYSYMBOL_KEY = 119,                      /* KEY  */
  YYSYMBOL_UNIQUE = 120,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 121,                 /* NULLABLE  */
  YYSYMBOL_IS = 122,                       /* IS  */
  YYSYMBOL_DEFAULT = 123,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 124,                     /* TRUE  */
  YYSYMBOL_FALSE = 125,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 126,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 127,                   /* SECOND  */
  YYSYMBOL_SECONDS = 128,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 129,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 130,                  /* MINUTES  */
  YYSYMBOL_HOUR = 131,                     /* HOUR  */
  YYSYMBOL_HOURS = 132,                    /* HOURS  */
  YYSYMBOL_DAY = 133,                      /* DAY  */
  YYSYMBOL_DAYS = 134,                     /* DAYS  */
  YYSYMBOL_MONTH = 135,                    /* MONTH  */
  YYSYMBOL_MONTHS = 136,                   /* MONTHS  */
  YYSYMBOL_YEAR = 137,                     /* YEAR  */
  YYSYMBOL_YEARS = 138,                    /* YEARS  */
  YYSYMBOL_EQUAL = 139,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 140,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 141,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 142,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 143,                  /* BETWEEN  */
  YYSYMBOL_AND = 144,                      /* AND  */
  YYSYMBOL_OR = 145,                       /* OR  */
  YYSYMBOL_EXTRACT = 146,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 147,                     /* LIKE  */
  YYSYMBOL_DATA = 148,                     /* DATA  */
  YYSYMBOL_LOG = 149,                      /* LOG  */
  YYSYMBOL_BUFFER = 150,                   /* BUFFER  */
  YYSYMBOL_KNN = 151,                      /* KNN  */
  YYSYMBOL_USING = 152,                    /* USING  */
  YYSYMBOL_SESSION = 153,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 154,                   /* GLOBAL  */
  YYSYMBOL_OFF = 155,                      /* OFF  */
  YYSYMBOL_EXPORT = 156,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 157,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 158,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 159,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 160,                 /* PROFILES  */
  YYSYMBOL_STATUS = 161,                   /* STATUS  */
  YYSYMBOL_VARIABLES = 162,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 163,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 164,                   /* SEARCH  */
  YYSYMBOL_MATCH = 165,                    /* MATCH  */
  YYSYMBOL_QUERY = 166,                    /* QUERY  */
  YYSYMBOL_FUSION = 167,                   /* FUSION  */
  YYSYMBOL_NUMBER = 168,                   /* NUMBER  */
  YYSYMBOL_169_ = 169,                     /* '='  */
  YYSYMBOL_170_ = 170,                     /* '<'  */
  YYSYMBOL_171_ = 171,                     /* '>'  */
  YYSYMBOL_172_ = 172,                     /* '+'  */
  YYSYMBOL_173_ = 173,                     /* '-'  */
  YYSYMBOL_174_ = 174,                     /* '*'  */
  YYSYMBOL_175_ = 175,                     /* '/'  */
  YYSYMBOL_176_ = 176,                     /* '%'  */
  YYSYMBOL_177_ = 177,                     /* '['  */
  YYSYMBOL_178_ = 178,                     /* ']'  */
  YYSYMBOL_179_ = 179,                     /* '('  */
  YYSYMBOL_180_ = 180,                     /* ')'  */
  YYSYMBOL_181_ = 181,                     /* '.'  */
  YYSYMBOL_182_ = 182,                     /* ';'  */
  YYSYMBOL_183_ = 183,                     /* ','  */
  YYSYMBOL_YYACCEPT = 184,                 /* $accept  */
  YYSYMBOL_input_pattern = 185,            /* input_pattern  */
  YYSYMBOL_statement_list = 186,           /* statement_list  */
  YYSYMBOL_statement = 187,                /* statement  */
  YYSYMBOL_explainable_statement = 188,    /* explainable_statement  */
  YYSYMBOL_create_statement = 189,         /* create_statement  */
  YYSYMBOL_table_element_array = 190,      /* table_element_array  */
  YYSYMBOL_table_element = 191,            /* table_element  */
  YYSYMBOL_table_column = 192,             /* table_column  */
  YYSYMBOL_column_type = 193,              /* column_type  */
  YYSYMBOL_column_constraints = 194,       /* column_constraints  */
  YYSYMBOL_column_constraint = 195,        /* column_constraint  */
  YYSYMBOL_default_expr = 196,             /* default_expr  */
  YYSYMBOL_table_constraint = 197,         /* table_constraint  */
  YYSYMBOL_identifier_array = 198,         /* identifier_array  */
  YYSYMBOL_delete_statement = 199,         /* delete_statement  */
  YYSYMBOL_insert_statement = 200,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 201, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 202,        /* explain_statement  */
  YYSYMBOL_explain_type = 203,             /* explain_type  */
  YYSYMBOL_update_statement = 204,         /* update_statement  */
  YYSYMBOL_update_expr_array = 205,        /* update_expr_array  */
  YYSYMBOL_update_expr = 206,              /* update_expr  */
  YYSYMBOL_drop_statement = 207,           /* drop_statement  */
  YYSYMBOL_copy_statement = 208,           /* copy_statement  */
  YYSYMBOL_select_statement = 209,         /* select_statement  */
  YYSYMBOL_select_with_paren = 210,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 211,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 212, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 213, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 214, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 215,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 216,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 217,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 218,            /* order_by_type  */
  YYSYMBOL_limit_expr = 219,               /* limit_expr  */
  YYSYMBOL_offset_expr = 220,              /* offset_expr  */
  YYSYMBOL_distinct = 221,                 /* distinct  */
  YYSYMBOL_from_clause = 222,              /* from_clause  */
  YYSYMBOL_search_clause = 223,            /* search_clause  */
  YYSYMBOL_where_clause = 224,             /* where_clause  */
  YYSYMBOL_having_clause = 225,            /* having_clause  */
  YYSYMBOL_group_by_clause = 226,          /* group_by_clause  */
  YYSYMBOL_set_operator = 227,             /* set_operator  */
  YYSYMBOL_table_reference = 228,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 229,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 230,     /* table_reference_name  */
  YYSYMBOL_table_name = 231,               /* table_name  */
  YYSYMBOL_table_alias = 232,              /* table_alias  */
  YYSYMBOL_with_clause = 233,              /* with_clause  */
  YYSYMBOL_with_expr_list = 234,           /* with_expr_list  */
  YYSYMBOL_with_expr = 235,                /* with_expr  */
  YYSYMBOL_join_clause = 236,              /* join_clause  */
  YYSYMBOL_join_type = 237,                /* join_type  */
  YYSYMBOL_show_statement = 238,           /* show_statement  */
  YYSYMBOL_flush_statement = 239,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 240,       /* optimize_statement  */
  YYSYMBOL_command_statement = 241,        /* command_statement  */
  YYSYMBOL_expr_array = 242,               /* expr_array  */
  YYSYMBOL_expr_array_list = 243,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 244,               /* expr_alias  */
  YYSYMBOL_expr = 245,                     /* expr  */
  YYSYMBOL_operand = 246,                  /* operand  */
  YYSYMBOL_knn_expr = 247,                 /* knn_expr  */
  YYSYMBOL_match_expr = 248,               /* match_expr  */
  YYSYMBOL_query_expr = 249,               /* query_expr  */
  YYSYMBOL_fusion_expr = 250,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 251,         /* sub_search_array  */
  YYSYMBOL_function_expr = 252,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 253,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 254,             /* between_expr  */
  YYSYMBOL_in_expr = 255,                  /* in_expr  */
  YYSYMBOL_case_expr = 256,                /* case_expr  */
  YYSYMBOL_case_check_array = 257,         /* case_check_array  */
  YYSYMBOL_cast_expr = 258,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 259,            /* subquery_expr  */
  YYSYMBOL_column_expr = 260,              /* column_expr  */
  YYSYMBOL_constant_expr = 261,            /* constant_expr  */
  YYSYMBOL_array_expr = 262,               /* array_expr  */
  YYSYMBOL_long_array_expr = 263,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 264, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 265,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 266, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 267,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 268,         /* copy_option_list  */
  YYSYMBOL_copy_option = 269,              /* copy_option  */
  YYSYMBOL_file_path = 270,                /* file_path  */
  YYSYMBOL_if_exists = 271,                /* if_exists  */
  YYSYMBOL_if_not_exists = 272,            /* if_not_exists  */
  YYSYMBOL_semicolon = 273,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 274,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 275,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 276, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 277,         /* index_param_list  */
  YYSYMBOL_index_param = 278,              /* index_param  */
  YYSYMBOL_index_info_list = 279           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 95 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 408 "parser.cpp"

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
#define YYLAST   912

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  184
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  96
/* YYNRULES -- Number of rules.  */
#define YYNRULES  360
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  698

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   423


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
       2,     2,     2,     2,     2,     2,     2,   176,     2,     2,
     179,   180,   174,   172,   183,   173,   181,   175,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   182,
     170,   169,   171,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   177,     2,   178,     2,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   473,   473,   477,   483,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   520,   537,
     553,   582,   598,   616,   645,   649,   655,   658,   664,   701,
     737,   738,   739,   740,   741,   742,   743,   744,   745,   746,
     747,   748,   749,   750,   751,   752,   753,   754,   755,   758,
     760,   761,   762,   763,   766,   767,   768,   769,   770,   771,
     772,   773,   774,   775,   776,   777,   778,   779,   780,   781,
     800,   804,   814,   817,   820,   823,   827,   830,   835,   840,
     847,   853,   863,   879,   913,   926,   929,   936,   942,   945,
     948,   951,   954,   957,   960,   963,   970,   983,   987,   992,
    1005,  1018,  1033,  1048,  1063,  1086,  1127,  1172,  1175,  1178,
    1187,  1197,  1200,  1204,  1209,  1231,  1234,  1239,  1255,  1258,
    1262,  1266,  1271,  1277,  1280,  1283,  1287,  1291,  1293,  1297,
    1299,  1302,  1306,  1309,  1313,  1318,  1322,  1325,  1329,  1332,
    1336,  1339,  1343,  1346,  1349,  1352,  1360,  1363,  1378,  1378,
    1380,  1394,  1403,  1408,  1417,  1422,  1427,  1433,  1440,  1443,
    1447,  1450,  1455,  1467,  1474,  1488,  1491,  1494,  1497,  1500,
    1503,  1506,  1512,  1516,  1520,  1524,  1528,  1535,  1539,  1543,
    1547,  1551,  1555,  1561,  1567,  1573,  1584,  1595,  1606,  1618,
    1630,  1643,  1657,  1668,  1686,  1690,  1694,  1702,  1716,  1722,
    1727,  1733,  1739,  1747,  1753,  1759,  1765,  1771,  1779,  1785,
    1791,  1807,  1811,  1816,  1820,  1847,  1853,  1857,  1858,  1859,
    1860,  1861,  1863,  1866,  1872,  1875,  1876,  1877,  1878,  1879,
    1880,  1881,  1882,  1884,  2051,  2059,  2070,  2076,  2085,  2091,
    2101,  2105,  2109,  2113,  2117,  2121,  2125,  2129,  2134,  2142,
    2150,  2159,  2166,  2173,  2180,  2187,  2194,  2202,  2210,  2218,
    2226,  2234,  2242,  2250,  2258,  2266,  2274,  2282,  2290,  2320,
    2328,  2337,  2345,  2354,  2362,  2368,  2375,  2381,  2388,  2393,
    2400,  2407,  2415,  2439,  2445,  2451,  2458,  2466,  2473,  2480,
    2485,  2495,  2500,  2505,  2510,  2515,  2520,  2525,  2530,  2535,
    2540,  2543,  2546,  2549,  2553,  2556,  2560,  2564,  2569,  2574,
    2578,  2583,  2588,  2594,  2600,  2606,  2612,  2618,  2624,  2630,
    2636,  2642,  2648,  2654,  2665,  2669,  2674,  2696,  2706,  2712,
    2716,  2717,  2719,  2720,  2722,  2723,  2735,  2743,  2747,  2750,
    2754,  2757,  2761,  2765,  2770,  2775,  2783,  2790,  2801,  2849,
    2898
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
  "PREPARE", "UNION", "ALL", "INTERSECT", "COMPACT", "EXCEPT", "FLUSH",
  "USE", "OPTIMIZE", "PROPERTIES", "DATABASE", "TABLE", "COLLECTION",
  "TABLES", "INTO", "VALUES", "AST", "PIPELINE", "RAW", "LOGICAL",
  "PHYSICAL", "FRAGMENT", "VIEW", "INDEX", "ANALYZE", "VIEWS", "DATABASES",
  "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS", "COLUMN", "COLUMNS", "INDEXES",
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM",
  "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID", "POINT",
  "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE", "NULLABLE",
  "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS",
  "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS",
  "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN",
  "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "KNN", "USING",
  "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG",
  "PROFILES", "STATUS", "VARIABLES", "VARIABLE", "SEARCH", "MATCH",
  "QUERY", "FUSION", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "table_element",
  "table_column", "column_type", "column_constraints", "column_constraint",
  "default_expr", "table_constraint", "identifier_array",
  "delete_statement", "insert_statement", "optional_identifier_array",
  "explain_statement", "explain_type", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
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

#define YYPACT_NINF (-632)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-348)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     228,    25,     2,   253,    48,    -7,    48,   -90,   470,    55,
      42,   119,    85,    48,    89,   -55,   -58,   120,   -38,  -632,
    -632,  -632,  -632,  -632,  -632,  -632,  -632,   210,  -632,  -632,
     118,  -632,  -632,  -632,  -632,    62,    62,    62,    62,    -4,
      48,   143,   143,   143,   143,   143,   -19,   217,    48,   214,
     188,   233,  -632,  -632,  -632,  -632,  -632,  -632,  -632,   733,
     249,    48,  -632,  -632,  -632,   203,   251,  -632,   275,  -632,
      48,  -632,  -632,  -632,  -632,  -632,   224,   104,  -632,   317,
     165,   173,  -632,   171,  -632,   330,  -632,  -632,    -2,   301,
    -632,   304,   305,   384,    48,    48,    48,   390,   352,   245,
     354,   425,    48,    48,    48,   427,   436,   440,   377,   441,
     441,    38,    56,  -632,  -632,  -632,  -632,  -632,  -632,  -632,
     210,  -632,  -632,  -632,  -632,  -632,   286,  -632,  -632,   443,
    -632,  -632,   444,  -632,  -632,   273,    89,   441,  -632,  -632,
    -632,  -632,    -2,  -632,  -632,  -632,   445,   400,   386,   385,
    -632,   -49,  -632,   245,  -632,    48,   454,    -1,  -632,  -632,
    -632,  -632,  -632,   396,  -632,   299,   -43,  -632,   445,  -632,
    -632,   397,   398,  -632,  -632,  -632,  -632,  -632,  -632,  -632,
    -632,  -632,  -632,   472,   477,  -632,  -632,  -632,  -632,  -632,
     118,  -632,  -632,   298,   309,   311,  -632,  -632,   725,   530,
     312,   314,   307,   481,   490,   491,   493,  -632,  -632,   494,
     320,   322,   323,   324,   332,   562,   562,  -632,   303,   413,
     -59,  -632,   -29,   499,  -632,  -632,  -632,  -632,  -632,  -632,
    -632,  -632,  -632,  -632,  -632,   331,  -632,  -632,  -126,  -632,
      39,  -632,   445,   445,   448,  -632,   -58,     7,   469,   347,
    -632,    12,   348,  -632,    48,   445,   440,  -632,   185,   361,
     362,  -632,   353,   351,  -632,  -632,   196,  -632,  -632,  -632,
    -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,   562,
     363,   647,   453,   445,   445,   -50,   172,  -632,  -632,  -632,
    -632,   725,  -632,   539,   445,   540,   541,   546,   246,   246,
    -632,  -632,   380,   -99,     9,   445,   399,   558,   445,   445,
     -54,   393,   -46,   562,   562,   562,   562,   562,   562,   562,
     562,   562,   562,   562,   562,   562,   562,     5,  -632,   568,
    -632,   571,   394,  -632,    19,   185,   445,  -632,   210,   735,
     462,   403,    27,  -632,  -632,  -632,   -58,   454,   404,  -632,
     581,   445,   406,  -632,   185,  -632,   345,   345,   582,  -632,
    -632,   445,  -632,    46,   453,   446,   414,   -22,   -62,   177,
    -632,   445,   445,   518,  -115,   415,    93,   105,  -632,  -632,
     -58,   416,   405,  -632,    17,  -632,  -632,    53,   377,  -632,
    -632,   456,   418,   562,   413,   480,  -632,   635,   635,   184,
     184,   626,   635,   635,   184,   184,   246,   246,  -632,  -632,
    -632,  -632,  -632,  -632,  -632,   445,  -632,  -632,  -632,   185,
    -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,
    -632,   423,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,
    -632,  -632,   424,   428,   154,   429,   454,   578,     7,   210,
     106,   454,  -632,   113,   430,   610,   613,  -632,   122,  -632,
     134,   570,   135,  -632,   447,  -632,   735,   445,  -632,   445,
     -47,   -13,   562,   449,   619,  -632,   621,  -632,   624,    -5,
       9,   572,  -632,  -632,  -632,  -632,  -632,  -632,   577,  -632,
     634,  -632,  -632,  -632,  -632,  -632,   460,   576,   413,   635,
     464,   136,  -632,   562,  -632,   642,   306,   344,   528,   531,
    -632,  -632,    35,   154,  -632,  -632,   454,   151,   473,  -632,
    -632,   501,   156,  -632,   445,  -632,  -632,  -632,   345,  -632,
     645,  -632,  -632,   478,   185,   -35,  -632,   445,   382,   474,
    -632,  -632,   161,   479,   482,    17,   405,     9,     9,   488,
      53,   606,   622,   498,   163,  -632,  -632,   647,   169,   496,
     497,   500,   502,   506,   507,   508,   509,   510,   511,   515,
     516,   522,   523,   527,   529,  -632,  -632,  -632,  -632,  -632,
     194,  -632,   658,   679,   532,   200,  -632,  -632,  -632,  -632,
     185,  -632,   697,  -632,   707,  -632,  -632,  -632,  -632,   651,
     454,  -632,  -632,  -632,  -632,   445,   445,  -632,  -632,  -632,
    -632,   709,   710,   711,   712,   713,   714,   716,   717,   718,
     719,   720,   724,   726,   727,   731,   732,   744,  -632,   583,
     205,  -632,   652,   748,  -632,   579,   573,   445,   206,   580,
     185,   574,   575,   584,   591,   592,   598,   599,   600,   601,
     603,   604,   605,   611,   612,   623,   633,   665,   218,  -632,
     658,   614,  -632,   652,   757,  -632,   185,  -632,  -632,  -632,
    -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,
    -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,  -632,   658,
    -632,   607,   211,   808,  -632,   666,   652,  -632
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     169,     0,     0,     0,     0,     0,     0,     0,   105,     0,
       0,     0,     0,     0,     0,     0,   169,     0,   345,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   118,   117,
       0,     8,    14,    15,    16,   343,   343,   343,   343,   343,
       0,   341,   341,   341,   341,   341,   162,     0,     0,     0,
       0,     0,    99,   103,   100,   101,   102,   104,    98,   169,
       0,     0,   183,   184,   182,     0,     0,   185,     0,   187,
       0,   204,   205,   206,   208,   207,     0,   168,   170,     0,
       0,     0,     1,   169,     2,   152,   154,   155,     0,   141,
     123,   129,     0,     0,     0,     0,     0,     0,     0,    96,
       0,     0,     0,     0,     0,     0,     0,     0,   147,     0,
       0,     0,     0,    97,    17,    22,    24,    23,    18,    19,
      21,    20,    25,    26,    27,   194,   195,   188,   190,     0,
     189,   191,     0,   186,   220,     0,     0,     0,   122,   121,
       4,   153,     0,   119,   120,   140,     0,     0,   137,     0,
      28,     0,    29,    96,   346,     0,     0,   169,   340,   110,
     112,   111,   113,     0,   163,     0,   147,   107,     0,    92,
     339,     0,     0,   212,   214,   213,   210,   211,   217,   219,
     218,   215,   216,     0,     0,   197,   196,   202,   192,   193,
       0,   171,   209,     0,     0,   297,   301,   304,   305,     0,
       0,     0,     0,     0,     0,     0,     0,   302,   303,     0,
       0,     0,     0,     0,     0,     0,     0,   299,     0,   169,
     143,   221,   226,   227,   239,   240,   241,   242,   236,   231,
     230,   229,   237,   238,   228,   235,   234,   312,     0,   313,
       0,   311,     0,     0,   139,   342,   169,     0,     0,     0,
      90,     0,     0,    94,     0,     0,     0,   106,   146,     0,
       0,   203,   198,     0,   126,   125,     0,   323,   322,   325,
     324,   327,   326,   329,   328,   331,   330,   333,   332,     0,
       0,   263,   169,     0,     0,     0,     0,   306,   307,   308,
     309,     0,   310,     0,     0,     0,     0,     0,   265,   264,
     320,   317,     0,     0,     0,     0,   145,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   316,     0,
     319,     0,   128,   130,   135,   136,     0,   124,    31,     0,
       0,     0,     0,    34,    36,    37,   169,     0,    33,    95,
       0,     0,    93,   114,   109,   108,     0,     0,     0,   199,
     172,     0,   258,     0,   169,     0,     0,     0,     0,     0,
     288,     0,     0,     0,     0,     0,     0,     0,   233,   232,
     169,   142,   156,   158,   167,   159,   222,     0,   147,   225,
     281,   282,     0,     0,   169,     0,   262,   272,   273,   276,
     277,     0,   279,   271,   274,   275,   267,   266,   268,   269,
     270,   298,   300,   318,   321,     0,   133,   134,   132,   138,
      40,    43,    44,    41,    42,    45,    46,    60,    47,    49,
      48,    63,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    87,     0,     0,   351,     0,    32,
       0,     0,    91,     0,     0,     0,     0,   338,     0,   334,
       0,   200,     0,   259,     0,   293,     0,     0,   286,     0,
       0,     0,     0,     0,     0,   246,     0,   248,     0,     0,
       0,     0,   176,   177,   178,   179,   175,   180,     0,   165,
       0,   160,   250,   251,   252,   253,   144,   151,   169,   280,
       0,     0,   261,     0,   131,     0,     0,     0,     0,     0,
      83,    84,     0,    87,    80,    38,     0,     0,     0,    30,
      35,   360,     0,   223,     0,   337,   336,   116,     0,   115,
       0,   260,   294,     0,   290,     0,   289,     0,     0,     0,
     314,   315,     0,     0,     0,   167,   157,     0,     0,   164,
       0,     0,   149,     0,     0,   295,   284,   283,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    85,    82,    86,    81,    39,
       0,    89,     0,     0,     0,     0,   335,   201,   292,   287,
     291,   278,     0,   244,     0,   247,   249,   161,   173,     0,
       0,   254,   255,   256,   257,     0,     0,   127,   296,   285,
      62,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    88,   354,
       0,   352,   349,     0,   224,     0,     0,     0,     0,   150,
     148,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   350,
       0,     0,   358,   349,     0,   245,   174,   166,    61,    67,
      68,    65,    66,    69,    70,    71,    64,    75,    76,    73,
      74,    77,    78,    79,    72,   355,   357,   356,   353,     0,
     359,     0,     0,     0,   348,     0,   349,   243
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -632,  -632,  -632,   765,  -632,   756,  -632,   401,  -632,   402,
    -632,   356,   357,  -632,  -343,   805,   806,   721,  -632,  -632,
     807,  -632,   615,   813,   814,   -56,   851,   -16,   685,   734,
      -9,  -632,  -632,   463,  -632,  -632,  -632,  -632,  -632,  -632,
    -161,  -632,  -632,  -632,  -632,   407,  -142,    10,   334,  -632,
    -632,   741,  -632,  -632,   821,   822,   823,   824,  -265,  -632,
     585,  -166,  -168,  -378,  -376,  -374,  -361,  -632,  -632,  -632,
    -632,  -632,  -632,   608,  -632,  -632,  -632,   372,  -632,   412,
    -632,   419,  -632,   677,   534,   360,    -3,   124,   209,  -632,
    -632,  -631,  -632,   207,   234,  -632
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   113,    20,   342,   343,   344,   444,
     513,   514,   515,   345,   251,    21,    22,   157,    23,    59,
      24,   166,   167,    25,    26,    27,    28,    29,    90,   143,
      91,   148,   332,   333,   418,   244,   337,   146,   306,   388,
     169,   607,   552,    88,   381,   382,   383,   384,   491,    30,
      77,    78,   385,   488,    31,    32,    33,    34,   220,   352,
     221,   222,   223,   224,   225,   226,   227,   496,   228,   229,
     230,   231,   232,   286,   233,   234,   235,   236,   539,   237,
     238,   239,   240,   241,   458,   459,   171,   101,    93,    84,
      98,   662,   519,   630,   631,   348
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      81,   363,   258,   120,   450,   257,    89,   246,   411,   492,
     339,   493,    46,   494,    47,    85,    49,    86,   304,    87,
     489,    14,   392,    75,   168,   467,   495,   307,   395,   308,
     309,   281,   690,   252,   466,    40,   285,   536,   284,   196,
     197,   198,   173,   174,   175,   308,   309,   298,   299,   589,
      99,    46,   328,   303,    35,    36,    37,   329,   108,  -347,
     178,   179,   180,    50,    51,   697,    38,    39,   473,    92,
      48,   126,    70,   490,   537,   396,   334,   335,    14,   144,
     134,   379,   308,   309,    60,    61,   453,    62,    74,   354,
     416,   417,    76,   393,   308,   309,   462,   308,   309,    63,
      64,   176,    79,   517,   151,   152,   153,   172,   522,   308,
     309,   281,   160,   161,   162,   308,   309,   367,   368,   181,
      82,    16,   308,   309,   305,   340,    89,   341,   374,   501,
     247,   308,   309,   194,   192,    92,   203,   204,   205,   206,
     256,   253,   390,   391,    83,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   409,   410,   207,
     208,   209,   106,   308,   309,   249,   102,   103,   104,   105,
     419,  -344,   601,   580,   602,   545,   603,   142,     1,   412,
       2,     3,     4,     5,     6,     7,     8,     9,   380,   604,
     338,   111,   349,   177,    10,   350,    11,    12,    13,   195,
     196,   197,   198,   302,   211,   470,   471,   447,    65,    66,
     448,   182,   218,    67,    68,    69,   100,   330,   212,   213,
     214,   685,   331,   686,   687,   499,   463,   497,   508,   305,
      85,   107,    86,   554,    87,     1,   112,     2,     3,     4,
       5,     6,     7,     8,     9,    94,    95,    96,    97,   334,
      14,    10,   125,    11,    12,    13,   370,   638,   371,   585,
     372,   468,   361,   469,   353,   372,   366,    71,    72,    73,
     199,   200,   509,   475,   510,   511,   476,   512,   133,   201,
     135,   202,    41,    42,    43,   477,   521,   136,   478,   350,
     449,   109,   110,   523,    44,    45,   305,   203,   204,   205,
     206,   534,   527,   535,   538,   528,   312,    14,   300,   301,
     195,   196,   197,   198,   529,   531,   556,   528,   305,   305,
     207,   208,   209,   137,   479,  -348,  -348,    15,   183,   308,
     309,   581,   184,   185,   350,   557,   584,   186,   187,   350,
     639,   593,   210,   609,   594,   138,   305,   211,   464,   610,
      16,   141,   611,   139,  -348,  -348,   322,   323,   324,   325,
     326,   212,   213,   214,   127,   128,   129,   145,   215,   216,
     217,   590,   147,   218,   628,   219,   362,   350,   500,   149,
     634,   199,   200,   305,    15,   659,   667,   150,   660,   350,
     201,   694,   202,   154,   660,   284,   559,   560,   561,   562,
     563,   358,   359,   564,   565,   598,   599,    16,   203,   204,
     205,   206,   130,   131,   132,   155,   195,   196,   197,   198,
     324,   325,   326,   566,   156,   455,   456,   457,   159,   158,
     163,   207,   208,   209,   567,   568,   569,   570,   571,   164,
     640,   572,   573,   165,   168,   170,   188,   189,   195,   196,
     197,   198,   190,   210,   242,   243,   365,   250,   211,   254,
     245,   574,   481,  -181,   482,   483,   484,   485,   255,   486,
     487,   666,   212,   213,   214,   261,   259,   260,   264,   215,
     216,   217,   553,   262,   218,   287,   219,   199,   200,   265,
     266,   282,    14,   283,   288,   289,   201,   290,   202,   293,
     291,   294,   295,   296,   312,    52,    53,    54,    55,    56,
      57,   297,   327,    58,   203,   204,   205,   206,   336,   199,
     200,   313,   314,   315,   316,   346,   347,   351,   201,   318,
     202,   360,    14,   195,   196,   197,   198,   207,   208,   209,
     356,   357,   364,   373,   375,   376,   203,   204,   205,   206,
     377,   319,   320,   321,   322,   323,   324,   325,   326,   210,
     378,   389,   591,   387,   211,   195,   196,   197,   198,   207,
     208,   209,   394,   310,   413,   311,   414,   415,   212,   213,
     214,   445,   446,   451,   452,   215,   216,   217,   461,   454,
     218,   210,   219,   393,   465,   472,   211,   498,   474,   480,
     308,   502,   505,   506,   279,   280,   518,   507,   516,   524,
     212,   213,   214,   201,   525,   202,   526,   215,   216,   217,
     530,   312,   218,   542,   219,   543,   218,   532,   544,   551,
     547,   203,   204,   205,   206,   548,   279,   549,   313,   314,
     315,   316,   317,   550,   555,   201,   318,   202,   558,   575,
     576,   587,   582,   583,   207,   208,   209,   592,   588,   595,
     605,   629,   596,   203,   204,   205,   206,   600,   319,   320,
     321,   322,   323,   324,   325,   326,   210,   606,   608,   612,
     613,   211,   632,   614,   633,   615,   207,   208,   209,   616,
     617,   618,   619,   620,   621,   212,   213,   214,   622,   623,
     365,   635,   215,   216,   217,   624,   625,   218,   210,   219,
     626,   636,   627,   211,   637,   641,   642,   643,   644,   645,
     646,   365,   647,   648,   649,   650,   651,   212,   213,   214,
     652,   661,   653,   654,   215,   216,   217,   655,   656,   218,
       1,   219,     2,     3,     4,     5,     6,     7,   312,     9,
     657,   663,   658,   665,   668,   669,    10,   312,    11,    12,
      13,   691,   664,   305,   670,   313,   314,   315,   316,   312,
     503,   671,   672,   318,  -348,  -348,   315,   316,   673,   674,
     675,   676,  -348,   677,   678,   679,   313,   314,   315,   316,
     693,   680,   681,   689,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   682,  -348,   320,   321,   322,   323,   324,
     325,   326,    14,   683,   695,   114,   319,   320,   321,   322,
     323,   324,   325,   326,   420,   421,   422,   423,   424,   425,
     426,   427,   428,   429,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   439,   440,   684,   696,   441,   140,   520,
     442,   443,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   115,   116,   117,    80,   533,   578,
     579,   355,   118,   119,   248,   263,   193,   191,   504,   597,
     121,   122,   123,   124,   577,   540,   292,   546,   586,    15,
     386,   460,   541,   369,   688,     0,   692,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    16
};

static const yytype_int16 yycheck[] =
{
      16,   266,   168,    59,   347,   166,     8,    56,     3,   387,
       3,   387,     3,   387,     4,    20,     6,    22,    77,    24,
       3,    79,    76,    13,    67,    87,   387,    56,    74,   144,
     145,   199,   663,    34,    56,    33,   202,    84,    88,     4,
       5,     6,     4,     5,     6,   144,   145,   215,   216,    84,
      40,     3,   178,   219,    29,    30,    31,   183,    48,    63,
       4,     5,     6,   153,   154,   696,    41,    42,   183,    73,
      77,    61,    30,    56,    87,   121,   242,   243,    79,    88,
      70,   180,   144,   145,    29,    30,   351,    32,     3,   255,
      71,    72,     3,   147,   144,   145,   361,   144,   145,    44,
      45,    63,   157,   446,    94,    95,    96,   110,   451,   144,
     145,   279,   102,   103,   104,   144,   145,   283,   284,    63,
       0,   179,   144,   145,   183,   118,     8,   120,   294,   394,
     179,   144,   145,   142,   137,    73,   101,   102,   103,   104,
     183,   157,   308,   309,   182,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   124,
     125,   126,   181,   144,   145,   155,    42,    43,    44,    45,
     336,     0,   550,   516,   550,   180,   550,   179,     7,   174,
       9,    10,    11,    12,    13,    14,    15,    16,   179,   550,
     246,     3,   180,   155,    23,   183,    25,    26,    27,     3,
       4,     5,     6,   219,   151,   371,   372,   180,   153,   154,
     183,   155,   177,   158,   159,   160,    73,   178,   165,   166,
     167,     3,   183,     5,     6,   393,   180,   388,    74,   183,
      20,    14,    22,   498,    24,     7,     3,     9,    10,    11,
      12,    13,    14,    15,    16,    36,    37,    38,    39,   415,
      79,    23,     3,    25,    26,    27,    84,   600,    86,   524,
      88,    84,    66,    86,   254,    88,   282,   148,   149,   150,
      74,    75,   118,   180,   120,   121,   183,   123,     3,    83,
      56,    85,    29,    30,    31,   180,   180,   183,   183,   183,
     346,    77,    78,   180,    41,    42,   183,   101,   102,   103,
     104,   467,   180,   469,   472,   183,   122,    79,     5,     6,
       3,     4,     5,     6,   180,   180,   180,   183,   183,   183,
     124,   125,   126,     6,   380,   141,   142,   156,    42,   144,
     145,   180,    46,    47,   183,   503,   180,    51,    52,   183,
     605,   180,   146,   180,   183,   180,   183,   151,   364,   180,
     179,    21,   183,   180,   170,   171,   172,   173,   174,   175,
     176,   165,   166,   167,   161,   162,   163,    66,   172,   173,
     174,   537,    68,   177,   180,   179,   180,   183,   394,    74,
     180,    74,    75,   183,   156,   180,   180,     3,   183,   183,
      83,   180,    85,     3,   183,    88,    90,    91,    92,    93,
      94,    48,    49,    97,    98,   547,   548,   179,   101,   102,
     103,   104,   161,   162,   163,    63,     3,     4,     5,     6,
     174,   175,   176,   117,   179,    80,    81,    82,     3,    75,
       3,   124,   125,   126,    90,    91,    92,    93,    94,     3,
     606,    97,    98,     3,    67,     4,     3,     3,     3,     4,
       5,     6,   179,   146,    54,    69,    74,     3,   151,    63,
      75,   117,    57,    58,    59,    60,    61,    62,   169,    64,
      65,   637,   165,   166,   167,     3,    79,    79,   180,   172,
     173,   174,   498,     6,   177,     4,   179,    74,    75,   180,
     179,   179,    79,   179,     4,     4,    83,     4,    85,   179,
       6,   179,   179,   179,   122,    35,    36,    37,    38,    39,
      40,   179,   181,    43,   101,   102,   103,   104,    70,    74,
      75,   139,   140,   141,   142,    56,   179,   179,    83,   147,
      85,   180,    79,     3,     4,     5,     6,   124,   125,   126,
     179,   179,   179,     4,     4,     4,   101,   102,   103,   104,
       4,   169,   170,   171,   172,   173,   174,   175,   176,   146,
     180,     3,   180,   164,   151,     3,     4,     5,     6,   124,
     125,   126,   179,    74,     6,    76,     5,   183,   165,   166,
     167,   119,   179,   179,     3,   172,   173,   174,     6,   183,
     177,   146,   179,   147,   180,    77,   151,   179,   183,   183,
     144,   121,   179,   179,    74,    75,    28,   179,   179,   179,
     165,   166,   167,    83,     4,    85,     3,   172,   173,   174,
      50,   122,   177,     4,   179,     4,   177,   180,     4,    53,
      58,   101,   102,   103,   104,    58,    74,     3,   139,   140,
     141,   142,   143,   183,   180,    83,   147,    85,     6,   121,
     119,     6,   179,   152,   124,   125,   126,   183,   180,   180,
      54,     3,   180,   101,   102,   103,   104,   179,   169,   170,
     171,   172,   173,   174,   175,   176,   146,    55,   180,   183,
     183,   151,     3,   183,   152,   183,   124,   125,   126,   183,
     183,   183,   183,   183,   183,   165,   166,   167,   183,   183,
      74,     4,   172,   173,   174,   183,   183,   177,   146,   179,
     183,     4,   183,   151,    63,     6,     6,     6,     6,     6,
       6,    74,     6,     6,     6,     6,     6,   165,   166,   167,
       6,    79,     6,     6,   172,   173,   174,     6,     6,   177,
       7,   179,     9,    10,    11,    12,    13,    14,   122,    16,
       6,     3,   169,   180,   180,   180,    23,   122,    25,    26,
      27,     4,   183,   183,   180,   139,   140,   141,   142,   122,
     144,   180,   180,   147,   139,   140,   141,   142,   180,   180,
     180,   180,   147,   180,   180,   180,   139,   140,   141,   142,
     183,   180,   180,   179,   147,   169,   170,   171,   172,   173,
     174,   175,   176,   180,   169,   170,   171,   172,   173,   174,
     175,   176,    79,   180,     6,    59,   169,   170,   171,   172,
     173,   174,   175,   176,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   180,   180,   112,    83,   448,
     115,   116,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,    59,    59,    59,    16,   466,   513,
     513,   256,    59,    59,   153,   190,   142,   136,   415,   545,
      59,    59,    59,    59,   512,   473,   209,   480,   528,   156,
     305,   357,   473,   285,   660,    -1,   689,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   179
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   156,   179,   185,   186,   187,
     189,   199,   200,   202,   204,   207,   208,   209,   210,   211,
     233,   238,   239,   240,   241,    29,    30,    31,    41,    42,
      33,    29,    30,    31,    41,    42,     3,   231,    77,   231,
     153,   154,    35,    36,    37,    38,    39,    40,    43,   203,
      29,    30,    32,    44,    45,   153,   154,   158,   159,   160,
      30,   148,   149,   150,     3,   231,     3,   234,   235,   157,
     210,   211,     0,   182,   273,    20,    22,    24,   227,     8,
     212,   214,    73,   272,   272,   272,   272,   272,   274,   231,
      73,   271,   271,   271,   271,   271,   181,    14,   231,    77,
      78,     3,     3,   188,   189,   199,   200,   204,   207,   208,
     209,   238,   239,   240,   241,     3,   231,   161,   162,   163,
     161,   162,   163,     3,   231,    56,   183,     6,   180,   180,
     187,    21,   179,   213,   214,    66,   221,    68,   215,    74,
       3,   231,   231,   231,     3,    63,   179,   201,    75,     3,
     231,   231,   231,     3,     3,     3,   205,   206,    67,   224,
       4,   270,   270,     4,     5,     6,    63,   155,     4,     5,
       6,    63,   155,    42,    46,    47,    51,    52,     3,     3,
     179,   235,   270,   213,   214,     3,     4,     5,     6,    74,
      75,    83,    85,   101,   102,   103,   104,   124,   125,   126,
     146,   151,   165,   166,   167,   172,   173,   174,   177,   179,
     242,   244,   245,   246,   247,   248,   249,   250,   252,   253,
     254,   255,   256,   258,   259,   260,   261,   263,   264,   265,
     266,   267,    54,    69,   219,    75,    56,   179,   201,   231,
       3,   198,    34,   211,    63,   169,   183,   224,   245,    79,
      79,     3,     6,   212,   180,   180,   179,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,    74,
      75,   246,   179,   179,    88,   245,   257,     4,     4,     4,
       4,     6,   267,   179,   179,   179,   179,   179,   246,   246,
       5,     6,   211,   245,    77,   183,   222,    56,   144,   145,
      74,    76,   122,   139,   140,   141,   142,   143,   147,   169,
     170,   171,   172,   173,   174,   175,   176,   181,   178,   183,
     178,   183,   216,   217,   245,   245,    70,   220,   209,     3,
     118,   120,   190,   191,   192,   197,    56,   179,   279,   180,
     183,   179,   243,   231,   245,   206,   179,   179,    48,    49,
     180,    66,   180,   242,   179,    74,   211,   245,   245,   257,
      84,    86,    88,     4,   245,     4,     4,     4,   180,   180,
     179,   228,   229,   230,   231,   236,   244,   164,   223,     3,
     245,   245,    76,   147,   179,    74,   121,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     246,     3,   174,     6,     5,   183,    71,    72,   218,   245,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   112,   115,   116,   193,   119,   179,   180,   183,   209,
     198,   179,     3,   242,   183,    80,    81,    82,   268,   269,
     268,     6,   242,   180,   211,   180,    56,    87,    84,    86,
     245,   245,    77,   183,   183,   180,   183,   180,   183,   209,
     183,    57,    59,    60,    61,    62,    64,    65,   237,     3,
      56,   232,   247,   248,   249,   250,   251,   224,   179,   246,
     211,   242,   121,   144,   217,   179,   179,   179,    74,   118,
     120,   121,   123,   194,   195,   196,   179,   198,    28,   276,
     191,   180,   198,   180,   179,     4,     3,   180,   183,   180,
      50,   180,   180,   193,   245,   245,    84,    87,   246,   262,
     263,   265,     4,     4,     4,   180,   229,    58,    58,     3,
     183,    53,   226,   211,   242,   180,   180,   246,     6,    90,
      91,    92,    93,    94,    97,    98,   117,    90,    91,    92,
      93,    94,    97,    98,   117,   121,   119,   261,   195,   196,
     198,   180,   179,   152,   180,   242,   269,     6,   180,    84,
     245,   180,   183,   180,   183,   180,   180,   232,   230,   230,
     179,   247,   248,   249,   250,    54,    55,   225,   180,   180,
     180,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   180,     3,
     277,   278,     3,   152,   180,     4,     4,    63,   198,   242,
     245,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   169,   180,
     183,    79,   275,     3,   183,   180,   245,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,     3,     5,     6,   278,   179,
     275,     4,   277,   183,   180,     6,   180,   275
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   184,   185,   186,   186,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   189,   189,
     189,   189,   189,   189,   190,   190,   191,   191,   192,   192,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     194,   194,   195,   195,   195,   195,   196,   196,   197,   197,
     198,   198,   199,   200,   200,   201,   201,   202,   203,   203,
     203,   203,   203,   203,   203,   203,   204,   205,   205,   206,
     207,   207,   207,   207,   207,   208,   208,   209,   209,   209,
     209,   210,   210,   211,   212,   213,   213,   214,   215,   215,
     216,   216,   217,   218,   218,   218,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   223,   224,   224,   225,   225,
     226,   226,   227,   227,   227,   227,   228,   228,   229,   229,
     230,   230,   231,   231,   232,   232,   232,   232,   233,   233,
     234,   234,   235,   236,   236,   237,   237,   237,   237,   237,
     237,   237,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   239,   239,   239,   240,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   242,   242,   243,   243,   244,   244,   245,   245,   245,
     245,   245,   246,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   247,   248,   248,   249,   249,   250,   250,
     251,   251,   251,   251,   251,   251,   251,   251,   252,   252,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   253,   253,   254,   255,   255,   256,   256,   256,   256,
     257,   257,   258,   259,   259,   259,   259,   260,   260,   260,
     260,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   262,   262,   263,   264,   264,   265,
     266,   266,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   268,   268,   269,   269,   269,   270,
     271,   271,   272,   272,   273,   273,   274,   274,   275,   275,
     276,   276,   277,   277,   278,   278,   278,   278,   279,   279,
     279
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       8,     6,     7,     6,     1,     3,     1,     1,     3,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     1,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       1,     2,     2,     1,     1,     2,     2,     0,     5,     4,
       1,     3,     4,     6,     5,     3,     0,     3,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     1,     3,     3,
       4,     4,     4,     4,     6,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     8,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     1,     2,     1,     1,     1,     3,     1,     1,
       2,     4,     1,     3,     2,     1,     5,     0,     2,     0,
       1,     3,     5,     4,     6,     1,     1,     1,     1,     1,
       1,     0,     2,     2,     2,     2,     3,     2,     3,     3,
       3,     3,     4,     4,     3,     3,     4,     4,     5,     6,
       7,     9,     4,     5,     2,     2,     2,     2,     2,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       3,     1,     3,     3,     5,     3,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,    13,     6,     8,     4,     6,     4,     6,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     4,
       5,     4,     3,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       4,     3,     3,     5,     5,     6,     4,     6,     3,     5,
       4,     5,     6,     4,     5,     5,     6,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     2,     2,     3,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     1,
       2,     0,     3,     0,     1,     0,     2,     0,     4,     0,
       4,     0,     1,     3,     1,     3,     3,     3,     6,     7,
       3
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
#line 2036 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2044 "parser.cpp"
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
#line 2058 "parser.cpp"
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
#line 2072 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 298 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2083 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2091 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2100 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 309 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2109 "parser.cpp"
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
#line 2123 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 262 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2134 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2144 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2154 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2164 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2174 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2184 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 344 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2194 "parser.cpp"
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
#line 2208 "parser.cpp"
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
#line 2222 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2232 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2240 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2248 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2257 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2265 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2273 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2281 "parser.cpp"
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
#line 2295 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2304 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2313 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2322 "parser.cpp"
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
#line 2335 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 322 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2344 "parser.cpp"
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
#line 2358 "parser.cpp"
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
#line 2372 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 338 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2382 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2391 "parser.cpp"
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
#line 2405 "parser.cpp"
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
#line 2422 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2430 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2446 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2454 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2462 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2478 "parser.cpp"
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
#line 2492 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2500 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2508 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2524 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2532 "parser.cpp"
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
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 314 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2569 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2577 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2585 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2593 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2609 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2617 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2625 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 305 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2644 "parser.cpp"
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
#line 2658 "parser.cpp"
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
#line 2672 "parser.cpp"
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
#line 2686 "parser.cpp"
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

#line 2794 "parser.cpp"

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
#line 3009 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 477 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3020 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 483 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3031 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 490 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3037 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 491 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3043 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 492 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3049 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 493 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3055 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 494 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3061 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 495 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3067 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 496 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3073 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3079 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 498 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3085 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 499 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3091 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 500 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3097 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3103 "parser.cpp"
    break;

  case 17: /* explainable_statement: create_statement  */
#line 503 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3109 "parser.cpp"
    break;

  case 18: /* explainable_statement: drop_statement  */
#line 504 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3115 "parser.cpp"
    break;

  case 19: /* explainable_statement: copy_statement  */
#line 505 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3121 "parser.cpp"
    break;

  case 20: /* explainable_statement: show_statement  */
#line 506 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3127 "parser.cpp"
    break;

  case 21: /* explainable_statement: select_statement  */
#line 507 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3133 "parser.cpp"
    break;

  case 22: /* explainable_statement: delete_statement  */
#line 508 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3139 "parser.cpp"
    break;

  case 23: /* explainable_statement: update_statement  */
#line 509 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3145 "parser.cpp"
    break;

  case 24: /* explainable_statement: insert_statement  */
#line 510 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3151 "parser.cpp"
    break;

  case 25: /* explainable_statement: flush_statement  */
#line 511 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3157 "parser.cpp"
    break;

  case 26: /* explainable_statement: optimize_statement  */
#line 512 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3163 "parser.cpp"
    break;

  case 27: /* explainable_statement: command_statement  */
#line 513 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3169 "parser.cpp"
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
#line 3189 "parser.cpp"
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
#line 3207 "parser.cpp"
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
#line 3240 "parser.cpp"
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
#line 3260 "parser.cpp"
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
#line 3281 "parser.cpp"
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
#line 3314 "parser.cpp"
    break;

  case 34: /* table_element_array: table_element  */
#line 645 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3323 "parser.cpp"
    break;

  case 35: /* table_element_array: table_element_array ',' table_element  */
#line 649 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3332 "parser.cpp"
    break;

  case 36: /* table_element: table_column  */
#line 655 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3340 "parser.cpp"
    break;

  case 37: /* table_element: table_constraint  */
#line 658 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3348 "parser.cpp"
    break;

  case 38: /* table_column: IDENTIFIER column_type default_expr  */
#line 664 "parser.y"
                                    {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                delete (yyvsp[-2].str_value);
                YYERROR;
            }
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

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-1].column_type_t).logical_type_, type_info_ptr, std::move(default_expr));

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3390 "parser.cpp"
    break;

  case 39: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 701 "parser.y"
                                                         {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-2].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-2].column_type_t).precision, (yyvsp[-2].column_type_t).scale);
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
//            break;
//        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-2].column_type_t).embedding_type_, (yyvsp[-2].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, default_expr);

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    (yyval.table_column_t)->constraints_ = *(yyvsp[-1].column_constraints_t);
    delete (yyvsp[-1].column_constraints_t);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3429 "parser.cpp"
    break;

  case 40: /* column_type: BOOLEAN  */
#line 737 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3435 "parser.cpp"
    break;

  case 41: /* column_type: TINYINT  */
#line 738 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3441 "parser.cpp"
    break;

  case 42: /* column_type: SMALLINT  */
#line 739 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3447 "parser.cpp"
    break;

  case 43: /* column_type: INTEGER  */
#line 740 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3453 "parser.cpp"
    break;

  case 44: /* column_type: INT  */
#line 741 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3459 "parser.cpp"
    break;

  case 45: /* column_type: BIGINT  */
#line 742 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3465 "parser.cpp"
    break;

  case 46: /* column_type: HUGEINT  */
#line 743 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3471 "parser.cpp"
    break;

  case 47: /* column_type: FLOAT  */
#line 744 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3477 "parser.cpp"
    break;

  case 48: /* column_type: REAL  */
#line 745 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3483 "parser.cpp"
    break;

  case 49: /* column_type: DOUBLE  */
#line 746 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3489 "parser.cpp"
    break;

  case 50: /* column_type: DATE  */
#line 747 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3495 "parser.cpp"
    break;

  case 51: /* column_type: TIME  */
#line 748 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3501 "parser.cpp"
    break;

  case 52: /* column_type: DATETIME  */
#line 749 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3507 "parser.cpp"
    break;

  case 53: /* column_type: TIMESTAMP  */
#line 750 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3513 "parser.cpp"
    break;

  case 54: /* column_type: UUID  */
#line 751 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3519 "parser.cpp"
    break;

  case 55: /* column_type: POINT  */
#line 752 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3525 "parser.cpp"
    break;

  case 56: /* column_type: LINE  */
#line 753 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3531 "parser.cpp"
    break;

  case 57: /* column_type: LSEG  */
#line 754 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3537 "parser.cpp"
    break;

  case 58: /* column_type: BOX  */
#line 755 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3543 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 758 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3549 "parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 760 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3555 "parser.cpp"
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3561 "parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 762 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3567 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL  */
#line 763 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3573 "parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 766 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3579 "parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 767 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3585 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 768 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3591 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 769 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3597 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 770 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3603 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 771 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3609 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 772 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3615 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 773 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3621 "parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 774 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3627 "parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 775 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3633 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 776 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3639 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 777 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3645 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 778 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3651 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 779 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3657 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 780 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3663 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 781 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3669 "parser.cpp"
    break;

  case 80: /* column_constraints: column_constraint  */
#line 800 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3678 "parser.cpp"
    break;

  case 81: /* column_constraints: column_constraints column_constraint  */
#line 804 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3692 "parser.cpp"
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 814 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3700 "parser.cpp"
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 817 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3708 "parser.cpp"
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 820 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3716 "parser.cpp"
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 823 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3724 "parser.cpp"
    break;

  case 86: /* default_expr: DEFAULT constant_expr  */
#line 827 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 3732 "parser.cpp"
    break;

  case 87: /* default_expr: %empty  */
#line 830 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 3740 "parser.cpp"
    break;

  case 88: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 835 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3750 "parser.cpp"
    break;

  case 89: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 840 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3760 "parser.cpp"
    break;

  case 90: /* identifier_array: IDENTIFIER  */
#line 847 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3771 "parser.cpp"
    break;

  case 91: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 853 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3782 "parser.cpp"
    break;

  case 92: /* delete_statement: DELETE FROM table_name where_clause  */
#line 863 "parser.y"
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
#line 3799 "parser.cpp"
    break;

  case 93: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 879 "parser.y"
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
#line 3838 "parser.cpp"
    break;

  case 94: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 913 "parser.y"
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
#line 3855 "parser.cpp"
    break;

  case 95: /* optional_identifier_array: '(' identifier_array ')'  */
#line 926 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3863 "parser.cpp"
    break;

  case 96: /* optional_identifier_array: %empty  */
#line 929 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3871 "parser.cpp"
    break;

  case 97: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 936 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3881 "parser.cpp"
    break;

  case 98: /* explain_type: ANALYZE  */
#line 942 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3889 "parser.cpp"
    break;

  case 99: /* explain_type: AST  */
#line 945 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3897 "parser.cpp"
    break;

  case 100: /* explain_type: RAW  */
#line 948 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3905 "parser.cpp"
    break;

  case 101: /* explain_type: LOGICAL  */
#line 951 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3913 "parser.cpp"
    break;

  case 102: /* explain_type: PHYSICAL  */
#line 954 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3921 "parser.cpp"
    break;

  case 103: /* explain_type: PIPELINE  */
#line 957 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3929 "parser.cpp"
    break;

  case 104: /* explain_type: FRAGMENT  */
#line 960 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3937 "parser.cpp"
    break;

  case 105: /* explain_type: %empty  */
#line 963 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3945 "parser.cpp"
    break;

  case 106: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 970 "parser.y"
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
#line 3962 "parser.cpp"
    break;

  case 107: /* update_expr_array: update_expr  */
#line 983 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3971 "parser.cpp"
    break;

  case 108: /* update_expr_array: update_expr_array ',' update_expr  */
#line 987 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3980 "parser.cpp"
    break;

  case 109: /* update_expr: IDENTIFIER '=' expr  */
#line 992 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3992 "parser.cpp"
    break;

  case 110: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1005 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4008 "parser.cpp"
    break;

  case 111: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1018 "parser.y"
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
#line 4026 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1033 "parser.y"
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
#line 4044 "parser.cpp"
    break;

  case 113: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1048 "parser.y"
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
#line 4062 "parser.cpp"
    break;

  case 114: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1063 "parser.y"
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
#line 4085 "parser.cpp"
    break;

  case 115: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1086 "parser.y"
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
#line 4131 "parser.cpp"
    break;

  case 116: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1127 "parser.y"
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
#line 4177 "parser.cpp"
    break;

  case 117: /* select_statement: select_without_paren  */
#line 1172 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4185 "parser.cpp"
    break;

  case 118: /* select_statement: select_with_paren  */
#line 1175 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4193 "parser.cpp"
    break;

  case 119: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1178 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4207 "parser.cpp"
    break;

  case 120: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1187 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4221 "parser.cpp"
    break;

  case 121: /* select_with_paren: '(' select_without_paren ')'  */
#line 1197 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4229 "parser.cpp"
    break;

  case 122: /* select_with_paren: '(' select_with_paren ')'  */
#line 1200 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4237 "parser.cpp"
    break;

  case 123: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1204 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4246 "parser.cpp"
    break;

  case 124: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1209 "parser.y"
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
#line 4272 "parser.cpp"
    break;

  case 125: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1231 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4280 "parser.cpp"
    break;

  case 126: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1234 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4288 "parser.cpp"
    break;

  case 127: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1239 "parser.y"
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
#line 4308 "parser.cpp"
    break;

  case 128: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1255 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4316 "parser.cpp"
    break;

  case 129: /* order_by_clause: %empty  */
#line 1258 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4324 "parser.cpp"
    break;

  case 130: /* order_by_expr_list: order_by_expr  */
#line 1262 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4333 "parser.cpp"
    break;

  case 131: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1266 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4342 "parser.cpp"
    break;

  case 132: /* order_by_expr: expr order_by_type  */
#line 1271 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4352 "parser.cpp"
    break;

  case 133: /* order_by_type: ASC  */
#line 1277 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4360 "parser.cpp"
    break;

  case 134: /* order_by_type: DESC  */
#line 1280 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4368 "parser.cpp"
    break;

  case 135: /* order_by_type: %empty  */
#line 1283 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4376 "parser.cpp"
    break;

  case 136: /* limit_expr: LIMIT expr  */
#line 1287 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4384 "parser.cpp"
    break;

  case 137: /* limit_expr: %empty  */
#line 1291 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4390 "parser.cpp"
    break;

  case 138: /* offset_expr: OFFSET expr  */
#line 1293 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4398 "parser.cpp"
    break;

  case 139: /* offset_expr: %empty  */
#line 1297 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4404 "parser.cpp"
    break;

  case 140: /* distinct: DISTINCT  */
#line 1299 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4412 "parser.cpp"
    break;

  case 141: /* distinct: %empty  */
#line 1302 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4420 "parser.cpp"
    break;

  case 142: /* from_clause: FROM table_reference  */
#line 1306 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4428 "parser.cpp"
    break;

  case 143: /* from_clause: %empty  */
#line 1309 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4436 "parser.cpp"
    break;

  case 144: /* search_clause: SEARCH sub_search_array  */
#line 1313 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4446 "parser.cpp"
    break;

  case 145: /* search_clause: %empty  */
#line 1318 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4454 "parser.cpp"
    break;

  case 146: /* where_clause: WHERE expr  */
#line 1322 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4462 "parser.cpp"
    break;

  case 147: /* where_clause: %empty  */
#line 1325 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4470 "parser.cpp"
    break;

  case 148: /* having_clause: HAVING expr  */
#line 1329 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4478 "parser.cpp"
    break;

  case 149: /* having_clause: %empty  */
#line 1332 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4486 "parser.cpp"
    break;

  case 150: /* group_by_clause: GROUP BY expr_array  */
#line 1336 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4494 "parser.cpp"
    break;

  case 151: /* group_by_clause: %empty  */
#line 1339 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4502 "parser.cpp"
    break;

  case 152: /* set_operator: UNION  */
#line 1343 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4510 "parser.cpp"
    break;

  case 153: /* set_operator: UNION ALL  */
#line 1346 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4518 "parser.cpp"
    break;

  case 154: /* set_operator: INTERSECT  */
#line 1349 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4526 "parser.cpp"
    break;

  case 155: /* set_operator: EXCEPT  */
#line 1352 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4534 "parser.cpp"
    break;

  case 156: /* table_reference: table_reference_unit  */
#line 1360 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4542 "parser.cpp"
    break;

  case 157: /* table_reference: table_reference ',' table_reference_unit  */
#line 1363 "parser.y"
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
#line 4560 "parser.cpp"
    break;

  case 160: /* table_reference_name: table_name table_alias  */
#line 1380 "parser.y"
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
#line 4578 "parser.cpp"
    break;

  case 161: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1394 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4589 "parser.cpp"
    break;

  case 162: /* table_name: IDENTIFIER  */
#line 1403 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4599 "parser.cpp"
    break;

  case 163: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1408 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4611 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER  */
#line 1417 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4621 "parser.cpp"
    break;

  case 165: /* table_alias: IDENTIFIER  */
#line 1422 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4631 "parser.cpp"
    break;

  case 166: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1427 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4642 "parser.cpp"
    break;

  case 167: /* table_alias: %empty  */
#line 1433 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4650 "parser.cpp"
    break;

  case 168: /* with_clause: WITH with_expr_list  */
#line 1440 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4658 "parser.cpp"
    break;

  case 169: /* with_clause: %empty  */
#line 1443 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4666 "parser.cpp"
    break;

  case 170: /* with_expr_list: with_expr  */
#line 1447 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4675 "parser.cpp"
    break;

  case 171: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1450 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4684 "parser.cpp"
    break;

  case 172: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1455 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4696 "parser.cpp"
    break;

  case 173: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1467 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4708 "parser.cpp"
    break;

  case 174: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1474 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4721 "parser.cpp"
    break;

  case 175: /* join_type: INNER  */
#line 1488 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4729 "parser.cpp"
    break;

  case 176: /* join_type: LEFT  */
#line 1491 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4737 "parser.cpp"
    break;

  case 177: /* join_type: RIGHT  */
#line 1494 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4745 "parser.cpp"
    break;

  case 178: /* join_type: OUTER  */
#line 1497 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4753 "parser.cpp"
    break;

  case 179: /* join_type: FULL  */
#line 1500 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4761 "parser.cpp"
    break;

  case 180: /* join_type: CROSS  */
#line 1503 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4769 "parser.cpp"
    break;

  case 181: /* join_type: %empty  */
#line 1506 "parser.y"
                {
}
#line 4776 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW DATABASES  */
#line 1512 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4785 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW TABLES  */
#line 1516 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4794 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW VIEWS  */
#line 1520 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4803 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW CONFIGS  */
#line 1524 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4812 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1528 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4824 "parser.cpp"
    break;

  case 187: /* show_statement: SHOW PROFILES  */
#line 1535 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4833 "parser.cpp"
    break;

  case 188: /* show_statement: SHOW SESSION STATUS  */
#line 1539 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4842 "parser.cpp"
    break;

  case 189: /* show_statement: SHOW GLOBAL STATUS  */
#line 1543 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4851 "parser.cpp"
    break;

  case 190: /* show_statement: SHOW SESSION VARIABLES  */
#line 1547 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 4860 "parser.cpp"
    break;

  case 191: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1551 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 4869 "parser.cpp"
    break;

  case 192: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1555 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4880 "parser.cpp"
    break;

  case 193: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1561 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4891 "parser.cpp"
    break;

  case 194: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1567 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4902 "parser.cpp"
    break;

  case 195: /* show_statement: SHOW TABLE table_name  */
#line 1573 "parser.y"
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
#line 4918 "parser.cpp"
    break;

  case 196: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1584 "parser.y"
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
#line 4934 "parser.cpp"
    break;

  case 197: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1595 "parser.y"
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
#line 4950 "parser.cpp"
    break;

  case 198: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1606 "parser.y"
                                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegment;
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
    delete (yyvsp[-2].table_name_t);
}
#line 4967 "parser.cpp"
    break;

  case 199: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1618 "parser.y"
                                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlocks;
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[-1].long_value);
    delete (yyvsp[-3].table_name_t);
}
#line 4984 "parser.cpp"
    break;

  case 200: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1630 "parser.y"
                                                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlock;
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
#line 5002 "parser.cpp"
    break;

  case 201: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1643 "parser.y"
                                                                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBlockColumn;
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[-4].long_value);
    (yyval.show_stmt)->block_id_ = (yyvsp[-2].long_value);
    (yyval.show_stmt)->column_id_ = (yyvsp[0].long_value);
    delete (yyvsp[-6].table_name_t);
}
#line 5021 "parser.cpp"
    break;

  case 202: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1657 "parser.y"
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
#line 5037 "parser.cpp"
    break;

  case 203: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1668 "parser.y"
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
#line 5056 "parser.cpp"
    break;

  case 204: /* flush_statement: FLUSH DATA  */
#line 1686 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5065 "parser.cpp"
    break;

  case 205: /* flush_statement: FLUSH LOG  */
#line 1690 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5074 "parser.cpp"
    break;

  case 206: /* flush_statement: FLUSH BUFFER  */
#line 1694 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5083 "parser.cpp"
    break;

  case 207: /* optimize_statement: OPTIMIZE table_name  */
#line 1702 "parser.y"
                                        {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 5098 "parser.cpp"
    break;

  case 208: /* command_statement: USE IDENTIFIER  */
#line 1716 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5109 "parser.cpp"
    break;

  case 209: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1722 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5119 "parser.cpp"
    break;

  case 210: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1727 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5130 "parser.cpp"
    break;

  case 211: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1733 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5141 "parser.cpp"
    break;

  case 212: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1739 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5154 "parser.cpp"
    break;

  case 213: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1747 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5165 "parser.cpp"
    break;

  case 214: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1753 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5176 "parser.cpp"
    break;

  case 215: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1759 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5187 "parser.cpp"
    break;

  case 216: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1765 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5198 "parser.cpp"
    break;

  case 217: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1771 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5211 "parser.cpp"
    break;

  case 218: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1779 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5222 "parser.cpp"
    break;

  case 219: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1785 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5233 "parser.cpp"
    break;

  case 220: /* command_statement: COMPACT TABLE table_name  */
#line 1791 "parser.y"
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
#line 5249 "parser.cpp"
    break;

  case 221: /* expr_array: expr_alias  */
#line 1807 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5258 "parser.cpp"
    break;

  case 222: /* expr_array: expr_array ',' expr_alias  */
#line 1811 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5267 "parser.cpp"
    break;

  case 223: /* expr_array_list: '(' expr_array ')'  */
#line 1816 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5276 "parser.cpp"
    break;

  case 224: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1820 "parser.y"
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
#line 5296 "parser.cpp"
    break;

  case 225: /* expr_alias: expr AS IDENTIFIER  */
#line 1847 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5307 "parser.cpp"
    break;

  case 226: /* expr_alias: expr  */
#line 1853 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5315 "parser.cpp"
    break;

  case 232: /* operand: '(' expr ')'  */
#line 1863 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5323 "parser.cpp"
    break;

  case 233: /* operand: '(' select_without_paren ')'  */
#line 1866 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5334 "parser.cpp"
    break;

  case 234: /* operand: constant_expr  */
#line 1872 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5342 "parser.cpp"
    break;

  case 243: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1884 "parser.y"
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
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_];

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
            knn_expr->embedding_data_ptr_ = new char[embedding_size];

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
#line 5513 "parser.cpp"
    break;

  case 244: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 2051 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5526 "parser.cpp"
    break;

  case 245: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 2059 "parser.y"
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
#line 5541 "parser.cpp"
    break;

  case 246: /* query_expr: QUERY '(' STRING ')'  */
#line 2070 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5552 "parser.cpp"
    break;

  case 247: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2076 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5565 "parser.cpp"
    break;

  case 248: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2085 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5576 "parser.cpp"
    break;

  case 249: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2091 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5589 "parser.cpp"
    break;

  case 250: /* sub_search_array: knn_expr  */
#line 2101 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5598 "parser.cpp"
    break;

  case 251: /* sub_search_array: match_expr  */
#line 2105 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5607 "parser.cpp"
    break;

  case 252: /* sub_search_array: query_expr  */
#line 2109 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5616 "parser.cpp"
    break;

  case 253: /* sub_search_array: fusion_expr  */
#line 2113 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5625 "parser.cpp"
    break;

  case 254: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 2117 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5634 "parser.cpp"
    break;

  case 255: /* sub_search_array: sub_search_array ',' match_expr  */
#line 2121 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5643 "parser.cpp"
    break;

  case 256: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2125 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5652 "parser.cpp"
    break;

  case 257: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2129 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5661 "parser.cpp"
    break;

  case 258: /* function_expr: IDENTIFIER '(' ')'  */
#line 2134 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5674 "parser.cpp"
    break;

  case 259: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2142 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5687 "parser.cpp"
    break;

  case 260: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2150 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5701 "parser.cpp"
    break;

  case 261: /* function_expr: operand IS NOT NULLABLE  */
#line 2159 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5713 "parser.cpp"
    break;

  case 262: /* function_expr: operand IS NULLABLE  */
#line 2166 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5725 "parser.cpp"
    break;

  case 263: /* function_expr: NOT operand  */
#line 2173 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5737 "parser.cpp"
    break;

  case 264: /* function_expr: '-' operand  */
#line 2180 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5749 "parser.cpp"
    break;

  case 265: /* function_expr: '+' operand  */
#line 2187 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5761 "parser.cpp"
    break;

  case 266: /* function_expr: operand '-' operand  */
#line 2194 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5774 "parser.cpp"
    break;

  case 267: /* function_expr: operand '+' operand  */
#line 2202 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5787 "parser.cpp"
    break;

  case 268: /* function_expr: operand '*' operand  */
#line 2210 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5800 "parser.cpp"
    break;

  case 269: /* function_expr: operand '/' operand  */
#line 2218 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5813 "parser.cpp"
    break;

  case 270: /* function_expr: operand '%' operand  */
#line 2226 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5826 "parser.cpp"
    break;

  case 271: /* function_expr: operand '=' operand  */
#line 2234 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5839 "parser.cpp"
    break;

  case 272: /* function_expr: operand EQUAL operand  */
#line 2242 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5852 "parser.cpp"
    break;

  case 273: /* function_expr: operand NOT_EQ operand  */
#line 2250 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5865 "parser.cpp"
    break;

  case 274: /* function_expr: operand '<' operand  */
#line 2258 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5878 "parser.cpp"
    break;

  case 275: /* function_expr: operand '>' operand  */
#line 2266 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5891 "parser.cpp"
    break;

  case 276: /* function_expr: operand LESS_EQ operand  */
#line 2274 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5904 "parser.cpp"
    break;

  case 277: /* function_expr: operand GREATER_EQ operand  */
#line 2282 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5917 "parser.cpp"
    break;

  case 278: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2290 "parser.y"
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
#line 5952 "parser.cpp"
    break;

  case 279: /* function_expr: operand LIKE operand  */
#line 2320 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5965 "parser.cpp"
    break;

  case 280: /* function_expr: operand NOT LIKE operand  */
#line 2328 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5978 "parser.cpp"
    break;

  case 281: /* conjunction_expr: expr AND expr  */
#line 2337 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5991 "parser.cpp"
    break;

  case 282: /* conjunction_expr: expr OR expr  */
#line 2345 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6004 "parser.cpp"
    break;

  case 283: /* between_expr: operand BETWEEN operand AND operand  */
#line 2354 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6016 "parser.cpp"
    break;

  case 284: /* in_expr: operand IN '(' expr_array ')'  */
#line 2362 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6027 "parser.cpp"
    break;

  case 285: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2368 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6038 "parser.cpp"
    break;

  case 286: /* case_expr: CASE expr case_check_array END  */
#line 2375 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6049 "parser.cpp"
    break;

  case 287: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2381 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6061 "parser.cpp"
    break;

  case 288: /* case_expr: CASE case_check_array END  */
#line 2388 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6071 "parser.cpp"
    break;

  case 289: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2393 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6082 "parser.cpp"
    break;

  case 290: /* case_check_array: WHEN expr THEN expr  */
#line 2400 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6094 "parser.cpp"
    break;

  case 291: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2407 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6106 "parser.cpp"
    break;

  case 292: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2415 "parser.y"
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
#line 6134 "parser.cpp"
    break;

  case 293: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2439 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6145 "parser.cpp"
    break;

  case 294: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2445 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6156 "parser.cpp"
    break;

  case 295: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2451 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6168 "parser.cpp"
    break;

  case 296: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2458 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6180 "parser.cpp"
    break;

  case 297: /* column_expr: IDENTIFIER  */
#line 2466 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6192 "parser.cpp"
    break;

  case 298: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2473 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6204 "parser.cpp"
    break;

  case 299: /* column_expr: '*'  */
#line 2480 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6214 "parser.cpp"
    break;

  case 300: /* column_expr: column_expr '.' '*'  */
#line 2485 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6228 "parser.cpp"
    break;

  case 301: /* constant_expr: STRING  */
#line 2495 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6238 "parser.cpp"
    break;

  case 302: /* constant_expr: TRUE  */
#line 2500 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6248 "parser.cpp"
    break;

  case 303: /* constant_expr: FALSE  */
#line 2505 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6258 "parser.cpp"
    break;

  case 304: /* constant_expr: DOUBLE_VALUE  */
#line 2510 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6268 "parser.cpp"
    break;

  case 305: /* constant_expr: LONG_VALUE  */
#line 2515 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6278 "parser.cpp"
    break;

  case 306: /* constant_expr: DATE STRING  */
#line 2520 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6288 "parser.cpp"
    break;

  case 307: /* constant_expr: TIME STRING  */
#line 2525 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6298 "parser.cpp"
    break;

  case 308: /* constant_expr: DATETIME STRING  */
#line 2530 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6308 "parser.cpp"
    break;

  case 309: /* constant_expr: TIMESTAMP STRING  */
#line 2535 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6318 "parser.cpp"
    break;

  case 310: /* constant_expr: INTERVAL interval_expr  */
#line 2540 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6326 "parser.cpp"
    break;

  case 311: /* constant_expr: interval_expr  */
#line 2543 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6334 "parser.cpp"
    break;

  case 312: /* constant_expr: long_array_expr  */
#line 2546 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6342 "parser.cpp"
    break;

  case 313: /* constant_expr: double_array_expr  */
#line 2549 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6350 "parser.cpp"
    break;

  case 314: /* array_expr: long_array_expr  */
#line 2553 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6358 "parser.cpp"
    break;

  case 315: /* array_expr: double_array_expr  */
#line 2556 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6366 "parser.cpp"
    break;

  case 316: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2560 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6374 "parser.cpp"
    break;

  case 317: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2564 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6384 "parser.cpp"
    break;

  case 318: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2569 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6393 "parser.cpp"
    break;

  case 319: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2574 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6401 "parser.cpp"
    break;

  case 320: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2578 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6411 "parser.cpp"
    break;

  case 321: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2583 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6420 "parser.cpp"
    break;

  case 322: /* interval_expr: LONG_VALUE SECONDS  */
#line 2588 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6431 "parser.cpp"
    break;

  case 323: /* interval_expr: LONG_VALUE SECOND  */
#line 2594 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6442 "parser.cpp"
    break;

  case 324: /* interval_expr: LONG_VALUE MINUTES  */
#line 2600 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6453 "parser.cpp"
    break;

  case 325: /* interval_expr: LONG_VALUE MINUTE  */
#line 2606 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6464 "parser.cpp"
    break;

  case 326: /* interval_expr: LONG_VALUE HOURS  */
#line 2612 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6475 "parser.cpp"
    break;

  case 327: /* interval_expr: LONG_VALUE HOUR  */
#line 2618 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6486 "parser.cpp"
    break;

  case 328: /* interval_expr: LONG_VALUE DAYS  */
#line 2624 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6497 "parser.cpp"
    break;

  case 329: /* interval_expr: LONG_VALUE DAY  */
#line 2630 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6508 "parser.cpp"
    break;

  case 330: /* interval_expr: LONG_VALUE MONTHS  */
#line 2636 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6519 "parser.cpp"
    break;

  case 331: /* interval_expr: LONG_VALUE MONTH  */
#line 2642 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6530 "parser.cpp"
    break;

  case 332: /* interval_expr: LONG_VALUE YEARS  */
#line 2648 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6541 "parser.cpp"
    break;

  case 333: /* interval_expr: LONG_VALUE YEAR  */
#line 2654 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6552 "parser.cpp"
    break;

  case 334: /* copy_option_list: copy_option  */
#line 2665 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6561 "parser.cpp"
    break;

  case 335: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2669 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6570 "parser.cpp"
    break;

  case 336: /* copy_option: FORMAT IDENTIFIER  */
#line 2674 "parser.y"
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
#line 6597 "parser.cpp"
    break;

  case 337: /* copy_option: DELIMITER STRING  */
#line 2696 "parser.y"
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
#line 6612 "parser.cpp"
    break;

  case 338: /* copy_option: HEADER  */
#line 2706 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6622 "parser.cpp"
    break;

  case 339: /* file_path: STRING  */
#line 2712 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6630 "parser.cpp"
    break;

  case 340: /* if_exists: IF EXISTS  */
#line 2716 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6636 "parser.cpp"
    break;

  case 341: /* if_exists: %empty  */
#line 2717 "parser.y"
  { (yyval.bool_value) = false; }
#line 6642 "parser.cpp"
    break;

  case 342: /* if_not_exists: IF NOT EXISTS  */
#line 2719 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6648 "parser.cpp"
    break;

  case 343: /* if_not_exists: %empty  */
#line 2720 "parser.y"
  { (yyval.bool_value) = false; }
#line 6654 "parser.cpp"
    break;

  case 346: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2735 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6667 "parser.cpp"
    break;

  case 347: /* if_not_exists_info: %empty  */
#line 2743 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6675 "parser.cpp"
    break;

  case 348: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2747 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6683 "parser.cpp"
    break;

  case 349: /* with_index_param_list: %empty  */
#line 2750 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6691 "parser.cpp"
    break;

  case 350: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2754 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 6699 "parser.cpp"
    break;

  case 351: /* optional_table_properties_list: %empty  */
#line 2757 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 6707 "parser.cpp"
    break;

  case 352: /* index_param_list: index_param  */
#line 2761 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6716 "parser.cpp"
    break;

  case 353: /* index_param_list: index_param_list ',' index_param  */
#line 2765 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6725 "parser.cpp"
    break;

  case 354: /* index_param: IDENTIFIER  */
#line 2770 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6735 "parser.cpp"
    break;

  case 355: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2775 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6748 "parser.cpp"
    break;

  case 356: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2783 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6760 "parser.cpp"
    break;

  case 357: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2790 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6772 "parser.cpp"
    break;

  case 358: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2801 "parser.y"
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
#line 6825 "parser.cpp"
    break;

  case 359: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2849 "parser.y"
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
#line 6879 "parser.cpp"
    break;

  case 360: /* index_info_list: '(' identifier_array ')'  */
#line 2898 "parser.y"
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
#line 6897 "parser.cpp"
    break;


#line 6901 "parser.cpp"

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

#line 2912 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

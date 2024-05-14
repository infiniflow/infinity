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
  YYSYMBOL_TENSOR = 118,                   /* TENSOR  */
  YYSYMBOL_PRIMARY = 119,                  /* PRIMARY  */
  YYSYMBOL_KEY = 120,                      /* KEY  */
  YYSYMBOL_UNIQUE = 121,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 122,                 /* NULLABLE  */
  YYSYMBOL_IS = 123,                       /* IS  */
  YYSYMBOL_DEFAULT = 124,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 125,                     /* TRUE  */
  YYSYMBOL_FALSE = 126,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 127,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 128,                   /* SECOND  */
  YYSYMBOL_SECONDS = 129,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 130,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 131,                  /* MINUTES  */
  YYSYMBOL_HOUR = 132,                     /* HOUR  */
  YYSYMBOL_HOURS = 133,                    /* HOURS  */
  YYSYMBOL_DAY = 134,                      /* DAY  */
  YYSYMBOL_DAYS = 135,                     /* DAYS  */
  YYSYMBOL_MONTH = 136,                    /* MONTH  */
  YYSYMBOL_MONTHS = 137,                   /* MONTHS  */
  YYSYMBOL_YEAR = 138,                     /* YEAR  */
  YYSYMBOL_YEARS = 139,                    /* YEARS  */
  YYSYMBOL_EQUAL = 140,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 141,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 142,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 143,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 144,                  /* BETWEEN  */
  YYSYMBOL_AND = 145,                      /* AND  */
  YYSYMBOL_OR = 146,                       /* OR  */
  YYSYMBOL_EXTRACT = 147,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 148,                     /* LIKE  */
  YYSYMBOL_DATA = 149,                     /* DATA  */
  YYSYMBOL_LOG = 150,                      /* LOG  */
  YYSYMBOL_BUFFER = 151,                   /* BUFFER  */
  YYSYMBOL_KNN = 152,                      /* KNN  */
  YYSYMBOL_USING = 153,                    /* USING  */
  YYSYMBOL_SESSION = 154,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 155,                   /* GLOBAL  */
  YYSYMBOL_OFF = 156,                      /* OFF  */
  YYSYMBOL_EXPORT = 157,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 158,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 159,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 160,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 161,                 /* PROFILES  */
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
  YYSYMBOL_compact_statement = 242,        /* compact_statement  */
  YYSYMBOL_expr_array = 243,               /* expr_array  */
  YYSYMBOL_expr_array_list = 244,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 245,               /* expr_alias  */
  YYSYMBOL_expr = 246,                     /* expr  */
  YYSYMBOL_operand = 247,                  /* operand  */
  YYSYMBOL_knn_expr = 248,                 /* knn_expr  */
  YYSYMBOL_match_expr = 249,               /* match_expr  */
  YYSYMBOL_query_expr = 250,               /* query_expr  */
  YYSYMBOL_fusion_expr = 251,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 252,         /* sub_search_array  */
  YYSYMBOL_function_expr = 253,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 254,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 255,             /* between_expr  */
  YYSYMBOL_in_expr = 256,                  /* in_expr  */
  YYSYMBOL_case_expr = 257,                /* case_expr  */
  YYSYMBOL_case_check_array = 258,         /* case_check_array  */
  YYSYMBOL_cast_expr = 259,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 260,            /* subquery_expr  */
  YYSYMBOL_column_expr = 261,              /* column_expr  */
  YYSYMBOL_constant_expr = 262,            /* constant_expr  */
  YYSYMBOL_array_expr = 263,               /* array_expr  */
  YYSYMBOL_long_array_expr = 264,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 265, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 266,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 267, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 268,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 269,         /* copy_option_list  */
  YYSYMBOL_copy_option = 270,              /* copy_option  */
  YYSYMBOL_file_path = 271,                /* file_path  */
  YYSYMBOL_if_exists = 272,                /* if_exists  */
  YYSYMBOL_if_not_exists = 273,            /* if_not_exists  */
  YYSYMBOL_semicolon = 274,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 275,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 276,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 277, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 278,         /* index_param_list  */
  YYSYMBOL_index_param = 279,              /* index_param  */
  YYSYMBOL_index_info_list = 280           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 409 "parser.cpp"

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
#define YYFINAL  84
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   938

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  184
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  97
/* YYNRULES -- Number of rules.  */
#define YYNRULES  373
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  739

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
       0,   476,   476,   480,   486,   493,   494,   495,   496,   497,
     498,   499,   500,   501,   502,   503,   504,   505,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     525,   542,   558,   587,   603,   621,   650,   654,   660,   663,
     669,   707,   743,   744,   745,   746,   747,   748,   749,   750,
     751,   752,   753,   754,   755,   756,   757,   758,   759,   760,
     761,   764,   766,   767,   768,   769,   772,   773,   774,   775,
     776,   777,   778,   779,   780,   781,   782,   783,   784,   785,
     786,   787,   788,   789,   790,   791,   792,   793,   794,   795,
     814,   818,   828,   831,   834,   837,   841,   844,   849,   854,
     861,   867,   877,   893,   927,   940,   943,   950,   956,   959,
     962,   965,   968,   971,   974,   977,   984,   997,  1001,  1006,
    1019,  1032,  1047,  1062,  1077,  1100,  1141,  1186,  1189,  1192,
    1201,  1211,  1214,  1218,  1223,  1245,  1248,  1253,  1269,  1272,
    1276,  1280,  1285,  1291,  1294,  1297,  1301,  1305,  1307,  1311,
    1313,  1316,  1320,  1323,  1327,  1332,  1336,  1339,  1343,  1346,
    1350,  1353,  1357,  1360,  1363,  1366,  1374,  1377,  1392,  1392,
    1394,  1408,  1417,  1422,  1431,  1436,  1441,  1447,  1454,  1457,
    1461,  1464,  1469,  1481,  1488,  1502,  1505,  1508,  1511,  1514,
    1517,  1520,  1526,  1530,  1534,  1538,  1542,  1549,  1553,  1557,
    1561,  1567,  1573,  1579,  1590,  1601,  1612,  1624,  1636,  1649,
    1663,  1674,  1692,  1696,  1700,  1708,  1722,  1728,  1733,  1739,
    1745,  1753,  1759,  1765,  1771,  1777,  1785,  1791,  1797,  1803,
    1809,  1817,  1823,  1830,  1847,  1851,  1856,  1860,  1887,  1893,
    1897,  1898,  1899,  1900,  1901,  1903,  1906,  1912,  1915,  1916,
    1917,  1918,  1919,  1920,  1921,  1922,  1924,  2091,  2099,  2110,
    2116,  2125,  2131,  2141,  2145,  2149,  2153,  2157,  2161,  2165,
    2169,  2174,  2182,  2190,  2199,  2206,  2213,  2220,  2227,  2234,
    2242,  2250,  2258,  2266,  2274,  2282,  2290,  2298,  2306,  2314,
    2322,  2330,  2360,  2368,  2377,  2385,  2394,  2402,  2408,  2415,
    2421,  2428,  2433,  2440,  2447,  2455,  2479,  2485,  2491,  2498,
    2506,  2513,  2520,  2525,  2535,  2540,  2545,  2550,  2555,  2560,
    2565,  2570,  2575,  2580,  2583,  2586,  2589,  2593,  2596,  2600,
    2604,  2609,  2614,  2618,  2623,  2628,  2634,  2640,  2646,  2652,
    2658,  2664,  2670,  2676,  2682,  2688,  2694,  2705,  2709,  2714,
    2736,  2746,  2752,  2756,  2757,  2759,  2760,  2762,  2763,  2775,
    2783,  2787,  2790,  2794,  2797,  2801,  2805,  2810,  2815,  2823,
    2830,  2841,  2889,  2938
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
  "EMBEDDING", "VECTOR", "BIT", "TENSOR", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "KNN", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE",
  "CONFIGS", "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "SEARCH",
  "MATCH", "QUERY", "FUSION", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','",
  "$accept", "input_pattern", "statement_list", "statement",
  "explainable_statement", "create_statement", "table_element_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "default_expr", "table_constraint",
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
  "command_statement", "compact_statement", "expr_array",
  "expr_array_list", "expr_alias", "expr", "operand", "knn_expr",
  "match_expr", "query_expr", "fusion_expr", "sub_search_array",
  "function_expr", "conjunction_expr", "between_expr", "in_expr",
  "case_expr", "case_check_array", "cast_expr", "subquery_expr",
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

#define YYPACT_NINF (-644)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-361)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     630,    33,    28,   175,    73,    45,    73,    79,   287,   193,
      60,   234,   134,    73,   164,   -32,   -43,   190,    66,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,    35,  -644,  -644,
     184,  -644,  -644,  -644,  -644,  -644,   219,   219,   219,   219,
     -19,    73,   224,   224,   224,   224,   224,    83,   269,    73,
       6,   312,   328,   346,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,   259,   352,    73,  -644,  -644,  -644,    68,    81,  -644,
     354,  -644,    73,  -644,  -644,  -644,  -644,  -644,   246,   176,
    -644,   362,   180,   189,  -644,   171,  -644,   358,  -644,  -644,
      -2,   309,  -644,   323,   322,   406,    73,    73,    73,   410,
     359,   236,   353,   426,    73,    73,    73,   433,   439,   455,
     392,   461,   461,    10,    14,    43,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,    35,  -644,  -644,  -644,  -644,  -644,  -644,
     194,  -644,   468,  -644,   475,  -644,  -644,   304,   164,   461,
    -644,  -644,  -644,  -644,    -2,  -644,  -644,  -644,   415,   430,
     419,   422,  -644,   -46,  -644,   236,  -644,    73,   496,    24,
    -644,  -644,  -644,  -644,  -644,   441,  -644,   336,   -49,  -644,
     415,  -644,  -644,   434,   435,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
     512,   516,  -644,  -644,  -644,  -644,  -644,   184,  -644,  -644,
     343,   344,   349,  -644,  -644,   471,   446,   351,   356,   329,
     532,   534,   539,   540,  -644,  -644,   549,   366,   367,   386,
     387,   389,   548,   548,  -644,   282,   360,   -55,  -644,   -35,
     603,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,   388,  -644,  -644,    46,  -644,    64,  -644,   415,
     415,   500,  -644,   -43,     8,   518,   396,  -644,  -111,   397,
    -644,    73,   415,   455,  -644,   191,   398,   399,  -644,   296,
     403,  -644,  -644,   215,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,   548,   400,   680,   505,
     415,   415,   -50,   165,  -644,  -644,  -644,  -644,   471,  -644,
     581,   415,   582,   586,   587,   272,   272,  -644,  -644,   416,
     -33,     9,   415,   431,   594,   415,   415,   -62,   436,   -29,
     548,   548,   548,   548,   548,   548,   548,   548,   548,   548,
     548,   548,   548,   548,     5,  -644,   608,  -644,   611,   438,
    -644,   -44,   191,   415,  -644,    35,   805,   497,   445,   -48,
    -644,  -644,  -644,   -43,   496,   447,  -644,   624,   415,   449,
    -644,   191,  -644,   429,   429,   622,  -644,  -644,   415,  -644,
     116,   505,   481,   450,   -27,   -57,   177,  -644,   415,   415,
     557,   -75,   452,   121,   123,  -644,  -644,   -43,   464,   499,
    -644,    44,  -644,  -644,  -100,   392,  -644,  -644,   493,   469,
     548,   360,   536,  -644,   717,   717,    84,    84,   643,   717,
     717,    84,    84,   272,   272,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,   415,  -644,  -644,  -644,   191,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,   480,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,   482,
     483,   484,    91,   485,   496,   626,     8,    35,   129,   496,
    -644,   130,   486,   656,   663,  -644,   131,  -644,   163,   617,
     178,  -644,   488,  -644,   805,   415,  -644,   415,    -3,   -47,
     548,   492,   666,  -644,   667,  -644,   668,    13,     9,   618,
    -644,  -644,  -644,  -644,  -644,  -644,   620,  -644,   677,  -644,
    -644,  -644,  -644,  -644,   498,   629,   360,   717,   503,   210,
    -644,   548,  -644,   678,   280,   308,   376,   564,   567,  -644,
    -644,    37,    91,  -644,  -644,   496,   227,   509,  -644,  -644,
     537,   228,  -644,   415,  -644,  -644,  -644,   429,  -644,   683,
    -644,  -644,   511,   191,    23,  -644,   415,   562,   510,  -644,
    -644,   243,   514,   517,    44,   499,     9,     9,   513,  -100,
     642,   644,   521,   244,  -644,  -644,   680,   257,   515,   523,
     524,   525,   528,   529,   533,   535,   541,   545,   546,   547,
     556,   558,   565,   566,   569,   570,   572,   573,   574,   575,
     576,   577,  -644,  -644,  -644,  -644,  -644,   261,  -644,   716,
     720,   597,   277,  -644,  -644,  -644,  -644,   191,  -644,   736,
    -644,   757,  -644,  -644,  -644,  -644,   699,   496,  -644,  -644,
    -644,  -644,   415,   415,  -644,  -644,  -644,  -644,   758,   759,
     761,   762,   763,   764,   765,   774,   775,   776,   783,   784,
     786,   787,   788,   789,   790,   791,   792,   793,   794,   795,
     796,   798,   799,  -644,   637,   292,  -644,   684,   804,  -644,
     625,   631,   415,   297,   627,   191,   645,   646,   647,   649,
     650,   651,   652,   653,   654,   655,   657,   658,   659,   661,
     662,   664,   665,   681,   682,   686,   687,   688,   689,   690,
     691,   288,  -644,   716,   669,  -644,   684,   820,  -644,   191,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
    -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,  -644,
     716,  -644,   660,   299,   830,  -644,   692,   684,  -644
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     179,     0,     0,     0,     0,     0,     0,     0,   115,     0,
       0,     0,     0,     0,     0,     0,   179,     0,   358,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   128,   127,
       0,     8,    14,    15,    16,    17,   356,   356,   356,   356,
     356,     0,   354,   354,   354,   354,   354,   172,     0,     0,
       0,     0,     0,     0,   109,   113,   110,   111,   112,   114,
     108,   179,     0,     0,   193,   194,   192,     0,     0,   195,
       0,   197,     0,   212,   213,   214,   216,   215,     0,   178,
     180,     0,     0,     0,     1,   179,     2,   162,   164,   165,
       0,   151,   133,   139,     0,     0,     0,     0,     0,     0,
       0,   106,     0,     0,     0,     0,     0,     0,     0,     0,
     157,     0,     0,     0,     0,     0,   107,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   202,
     203,   198,     0,   199,     0,   196,   233,     0,     0,     0,
     132,   131,     4,   163,     0,   129,   130,   150,     0,     0,
     147,     0,    30,     0,    31,   106,   359,     0,     0,   179,
     353,   120,   122,   121,   123,     0,   173,     0,   157,   117,
       0,   102,   352,     0,     0,   220,   222,   221,   218,   219,
     225,   227,   226,   223,   224,   230,   232,   231,   228,   229,
       0,     0,   205,   204,   210,   200,   201,     0,   181,   217,
       0,     0,   310,   314,   317,   318,     0,     0,     0,     0,
       0,     0,     0,     0,   315,   316,     0,     0,     0,     0,
       0,     0,     0,     0,   312,     0,   179,   153,   234,   239,
     240,   252,   253,   254,   255,   249,   244,   243,   242,   250,
     251,   241,   248,   247,   325,     0,   326,     0,   324,     0,
       0,   149,   355,   179,     0,     0,     0,   100,     0,     0,
     104,     0,     0,     0,   116,   156,     0,     0,   211,   206,
       0,   136,   135,     0,   336,   335,   338,   337,   340,   339,
     342,   341,   344,   343,   346,   345,     0,     0,   276,   179,
       0,     0,     0,     0,   319,   320,   321,   322,     0,   323,
       0,     0,     0,     0,     0,   278,   277,   333,   330,     0,
       0,     0,     0,   155,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   329,     0,   332,     0,   138,
     140,   145,   146,     0,   134,    33,     0,     0,     0,     0,
      36,    38,    39,   179,     0,    35,   105,     0,     0,   103,
     124,   119,   118,     0,     0,     0,   207,   182,     0,   271,
       0,   179,     0,     0,     0,     0,     0,   301,     0,     0,
       0,     0,     0,     0,     0,   246,   245,   179,   152,   166,
     168,   177,   169,   235,     0,   157,   238,   294,   295,     0,
       0,   179,     0,   275,   285,   286,   289,   290,     0,   292,
     284,   287,   288,   280,   279,   281,   282,   283,   311,   313,
     331,   334,     0,   143,   144,   142,   148,    42,    45,    46,
      43,    44,    47,    48,    62,    49,    51,    50,    65,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
       0,     0,    97,     0,     0,   364,     0,    34,     0,     0,
     101,     0,     0,     0,     0,   351,     0,   347,     0,   208,
       0,   272,     0,   306,     0,     0,   299,     0,     0,     0,
       0,     0,     0,   259,     0,   261,     0,     0,     0,     0,
     186,   187,   188,   189,   185,   190,     0,   175,     0,   170,
     263,   264,   265,   266,   154,   161,   179,   293,     0,     0,
     274,     0,   141,     0,     0,     0,     0,     0,     0,    93,
      94,     0,    97,    90,    40,     0,     0,     0,    32,    37,
     373,     0,   236,     0,   350,   349,   126,     0,   125,     0,
     273,   307,     0,   303,     0,   302,     0,     0,     0,   327,
     328,     0,     0,     0,   177,   167,     0,     0,   174,     0,
       0,   159,     0,     0,   308,   297,   296,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    95,    92,    96,    91,    41,     0,    99,     0,
       0,     0,     0,   348,   209,   305,   300,   304,   291,     0,
     257,     0,   260,   262,   171,   183,     0,     0,   267,   268,
     269,   270,     0,     0,   137,   309,   298,    64,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    98,   367,     0,   365,   362,     0,   237,
       0,     0,     0,     0,   160,   158,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   363,     0,     0,   371,   362,     0,   258,   184,
     176,    63,    69,    70,    67,    68,    71,    72,    73,    66,
      85,    86,    83,    84,    87,    88,    89,    82,    77,    78,
      75,    76,    79,    80,    81,    74,   368,   370,   369,   366,
       0,   372,     0,     0,     0,   361,     0,   362,   256
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -644,  -644,  -644,   778,  -644,   785,  -644,   391,  -644,   390,
    -644,   355,   357,  -644,  -350,   812,   813,   721,  -644,  -644,
     814,  -644,   615,   819,   821,   -58,   865,   -15,   718,   739,
     -59,  -644,  -644,   462,  -644,  -644,  -644,  -644,  -644,  -644,
    -163,  -644,  -644,  -644,  -644,   428,  -236,    19,   331,  -644,
    -644,   780,  -644,  -644,   858,   861,   863,   864,   866,  -271,
    -644,   614,  -170,  -172,  -387,  -385,  -370,  -368,  -644,  -644,
    -644,  -644,  -644,  -644,   636,  -644,  -644,  -644,   408,  -644,
     451,  -644,   453,  -644,   714,   571,   394,   -34,   157,   239,
    -644,  -644,  -643,  -644,   203,   245,  -644
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   349,   350,   351,   452,
     522,   523,   524,   352,   258,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   339,   340,   425,   251,   344,   148,   313,   395,
     171,   624,   561,    90,   388,   389,   390,   391,   499,    30,
      79,    80,   392,   496,    31,    32,    33,    34,    35,   227,
     359,   228,   229,   230,   231,   232,   233,   234,   504,   235,
     236,   237,   238,   239,   293,   240,   241,   242,   243,   548,
     244,   245,   246,   247,   248,   466,   467,   173,   103,    95,
      86,   100,   695,   528,   655,   656,   355
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     265,    83,   370,   123,   458,   264,    91,   500,   418,   501,
     253,   346,    47,   175,   399,   176,   177,   180,   170,   181,
     182,   314,   311,    48,   502,    50,   503,   423,   424,   474,
     475,   146,    77,    87,   288,    88,    14,    89,   291,   292,
     546,   203,   204,   205,  -360,   402,   185,   497,   186,   187,
     305,   306,   218,   731,    94,    87,   310,    88,   259,    89,
     101,    41,    36,    37,    38,   219,   220,   221,   110,   356,
     315,   316,   357,   178,    39,    40,    47,   183,   174,   341,
     342,   545,   130,   111,   112,   201,   400,   461,   315,   316,
      72,   136,   361,   403,   738,   315,   316,   470,   315,   316,
     498,   315,   316,    14,   526,   199,   188,   606,   481,   531,
     315,   316,   315,   316,   288,   153,   154,   155,   315,   316,
     374,   375,    49,   162,   163,   164,    81,   347,   312,   348,
     509,   381,   455,   254,   263,   456,    16,    76,   210,   211,
     212,   213,   315,   316,   260,   397,   398,   386,   404,   405,
     406,   407,   408,   409,   410,   411,   412,   413,   414,   415,
     416,   417,   214,   215,   216,   517,   179,    78,   315,   316,
     184,  -357,   618,   426,   619,   597,   256,   144,     1,   419,
       2,     3,     4,     5,     6,     7,     8,     9,   387,   620,
      84,   621,    91,   554,    10,   345,    11,    12,    13,   189,
     104,   105,   106,   107,    42,    43,    44,   319,   478,   479,
     518,   309,   519,   520,   225,   521,    45,    46,   202,   203,
     204,   205,    62,    63,   335,    64,  -361,  -361,   507,   336,
     131,   132,   505,    51,    52,   563,   190,    65,    66,    53,
     191,   192,   337,   133,   134,   193,   194,   338,    85,   377,
      14,   378,   341,   379,  -361,  -361,   329,   330,   331,   332,
     333,   476,   602,   477,   108,   379,     1,   663,     2,     3,
       4,     5,     6,     7,   373,     9,    96,    97,    98,    99,
     360,   368,    10,   109,    11,    12,    13,   307,   308,   206,
     207,   726,    94,   727,   728,   457,   471,   102,   208,   312,
     209,   483,   137,   485,   484,   543,   486,   544,   547,   530,
     532,   536,   357,   312,   537,   113,   210,   211,   212,   213,
     615,   616,    54,    55,    56,    57,    58,    59,    15,   487,
      60,   114,   202,   203,   204,   205,   315,   316,    14,   566,
     214,   215,   216,   538,   365,   366,   537,    67,    68,   115,
      16,   664,    69,    70,    71,   129,   472,   135,   540,   138,
     140,   312,   217,   202,   203,   204,   205,   218,   139,   141,
     568,   569,   570,   571,   572,   147,   607,   573,   574,   143,
     219,   220,   221,    73,    74,    75,   508,   222,   223,   224,
     565,   149,   225,   312,   226,   369,   151,   575,   576,   577,
     578,   579,   580,   206,   207,   581,   582,   598,   601,   152,
     357,   357,   208,   156,   209,   158,    15,   291,   202,   203,
     204,   205,   157,   610,   626,   583,   611,   312,   160,   161,
     210,   211,   212,   213,   206,   207,   165,   627,    16,    14,
     628,   653,   166,   208,   357,   209,   331,   332,   333,   202,
     203,   204,   205,   665,   214,   215,   216,   659,   167,   170,
     312,   210,   211,   212,   213,   172,   584,   585,   586,   587,
     588,   195,   692,   589,   590,   693,   217,   700,   196,   735,
     357,   218,   693,   197,   249,   214,   215,   216,   250,   206,
     207,   562,   699,   591,   219,   220,   221,   252,   208,   257,
     209,   222,   223,   224,   261,   262,   225,   217,   226,   463,
     464,   465,   218,   266,   267,   268,   210,   211,   212,   213,
     286,   287,   269,   271,   272,   219,   220,   221,   273,   208,
     289,   209,   222,   223,   224,   290,   294,   225,   295,   226,
     214,   215,   216,   296,   297,   300,   301,   210,   211,   212,
     213,   202,   203,   204,   205,   298,   489,  -191,   490,   491,
     492,   493,   217,   494,   495,   302,   303,   218,   304,   334,
     343,   214,   215,   216,   353,   354,   358,   363,   364,   371,
     219,   220,   221,   367,    14,   380,   382,   222,   223,   224,
     383,   384,   225,   217,   226,   394,   385,   396,   218,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   219,   220,   221,   420,   401,   421,   453,   222,   223,
     224,   422,   286,   225,   454,   226,   459,   460,   469,   400,
     473,   208,   462,   209,   480,   482,   372,     1,   315,     2,
       3,     4,     5,     6,     7,     8,     9,   488,   506,   210,
     211,   212,   213,    10,   527,    11,    12,    13,   510,   513,
     534,   514,   515,   516,   525,   533,   535,   539,   541,   225,
     551,   552,   553,   214,   215,   216,   556,   317,   557,   318,
     558,   559,   560,   564,   567,   319,   592,   593,   599,   604,
     600,   605,   617,   609,   612,   217,   622,   613,   629,   623,
     218,   625,   320,   321,   322,   323,   630,   631,   632,    14,
     325,   633,   634,   219,   220,   221,   635,   372,   636,   654,
     222,   223,   224,   657,   637,   225,   319,   226,   638,   639,
     640,   326,   327,   328,   329,   330,   331,   332,   333,   641,
     660,   642,   608,   320,   321,   322,   323,   324,   643,   644,
     658,   325,   645,   646,   372,   647,   648,   649,   650,   651,
     652,   661,   662,   694,   666,   667,   319,   668,   669,   670,
     671,   672,   326,   327,   328,   329,   330,   331,   332,   333,
     673,   674,   675,   320,   321,   322,   323,    15,   511,   676,
     677,   325,   678,   679,   680,   681,   682,   683,   684,   685,
     686,   687,   688,   319,   689,   690,   691,   696,   697,    16,
     312,   698,   326,   327,   328,   329,   330,   331,   332,   333,
     320,   321,   322,   323,   732,   701,   702,   703,   325,   704,
     705,   706,   707,   708,   709,   710,   736,   711,   712,   713,
     319,   714,   715,   734,   716,   717,   117,   529,   730,   326,
     327,   328,   329,   330,   331,   332,   333,  -361,  -361,   322,
     323,   718,   719,   142,   542,  -361,   720,   721,   722,   723,
     724,   725,   737,   118,   119,   120,   255,   595,   362,   596,
     121,    82,   122,   200,   512,   614,  -361,   327,   328,   329,
     330,   331,   332,   333,   427,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,   445,   446,   447,   270,   555,   448,   198,   124,
     449,   450,   125,   451,   126,   127,   393,   128,   376,   594,
     299,   603,   549,   733,   550,   468,     0,     0,   729
};

static const yytype_int16 yycheck[] =
{
     170,    16,   273,    61,   354,   168,     8,   394,     3,   394,
      56,     3,     3,     3,    76,     5,     6,     3,    67,     5,
       6,    56,    77,     4,   394,     6,   394,    71,    72,    56,
      87,    90,    13,    20,   206,    22,    79,    24,    88,   209,
      87,     4,     5,     6,    63,    74,     3,     3,     5,     6,
     222,   223,   152,   696,    73,    20,   226,    22,    34,    24,
      41,    33,    29,    30,    31,   165,   166,   167,    49,   180,
     145,   146,   183,    63,    41,    42,     3,    63,   112,   249,
     250,    84,    63,    77,    78,   144,   148,   358,   145,   146,
      30,    72,   262,   122,   737,   145,   146,   368,   145,   146,
      56,   145,   146,    79,   454,   139,    63,    84,   183,   459,
     145,   146,   145,   146,   286,    96,    97,    98,   145,   146,
     290,   291,    77,   104,   105,   106,   158,   119,   183,   121,
     401,   301,   180,   179,   183,   183,   179,     3,   101,   102,
     103,   104,   145,   146,   159,   315,   316,   180,   320,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   125,   126,   127,    74,   156,     3,   145,   146,
     156,     0,   559,   343,   559,   525,   157,   179,     7,   174,
       9,    10,    11,    12,    13,    14,    15,    16,   179,   559,
       0,   559,     8,   180,    23,   253,    25,    26,    27,   156,
      43,    44,    45,    46,    29,    30,    31,   123,   378,   379,
     119,   226,   121,   122,   177,   124,    41,    42,     3,     4,
       5,     6,    29,    30,   178,    32,   142,   143,   400,   183,
     162,   163,   395,   154,   155,   506,    42,    44,    45,   160,
      46,    47,   178,   162,   163,    51,    52,   183,   182,    84,
      79,    86,   422,    88,   170,   171,   172,   173,   174,   175,
     176,    84,   533,    86,   181,    88,     7,   617,     9,    10,
      11,    12,    13,    14,   289,    16,    37,    38,    39,    40,
     261,    66,    23,    14,    25,    26,    27,     5,     6,    74,
      75,     3,    73,     5,     6,   353,   180,    73,    83,   183,
      85,   180,    56,   180,   183,   475,   183,   477,   480,   180,
     180,   180,   183,   183,   183,     3,   101,   102,   103,   104,
     556,   557,    35,    36,    37,    38,    39,    40,   157,   387,
      43,     3,     3,     4,     5,     6,   145,   146,    79,   511,
     125,   126,   127,   180,    48,    49,   183,   154,   155,     3,
     179,   622,   159,   160,   161,     3,   371,     3,   180,   183,
     180,   183,   147,     3,     4,     5,     6,   152,     6,   180,
      90,    91,    92,    93,    94,    66,   546,    97,    98,    21,
     165,   166,   167,   149,   150,   151,   401,   172,   173,   174,
     180,    68,   177,   183,   179,   180,    74,   117,    90,    91,
      92,    93,    94,    74,    75,    97,    98,   180,   180,     3,
     183,   183,    83,     3,    85,   179,   157,    88,     3,     4,
       5,     6,    63,   180,   180,   117,   183,   183,    75,     3,
     101,   102,   103,   104,    74,    75,     3,   180,   179,    79,
     183,   180,     3,    83,   183,    85,   174,   175,   176,     3,
       4,     5,     6,   623,   125,   126,   127,   180,     3,    67,
     183,   101,   102,   103,   104,     4,    90,    91,    92,    93,
      94,     3,   180,    97,    98,   183,   147,   180,     3,   180,
     183,   152,   183,   179,    54,   125,   126,   127,    69,    74,
      75,   506,   662,   117,   165,   166,   167,    75,    83,     3,
      85,   172,   173,   174,    63,   169,   177,   147,   179,    80,
      81,    82,   152,    79,    79,     3,   101,   102,   103,   104,
      74,    75,     6,   180,   180,   165,   166,   167,   179,    83,
     179,    85,   172,   173,   174,   179,     4,   177,     4,   179,
     125,   126,   127,     4,     4,   179,   179,   101,   102,   103,
     104,     3,     4,     5,     6,     6,    57,    58,    59,    60,
      61,    62,   147,    64,    65,   179,   179,   152,   179,   181,
      70,   125,   126,   127,    56,   179,   179,   179,   179,   179,
     165,   166,   167,   180,    79,     4,     4,   172,   173,   174,
       4,     4,   177,   147,   179,   164,   180,     3,   152,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   165,   166,   167,     6,   179,     5,   120,   172,   173,
     174,   183,    74,   177,   179,   179,   179,     3,     6,   148,
     180,    83,   183,    85,    77,   183,    74,     7,   145,     9,
      10,    11,    12,    13,    14,    15,    16,   183,   179,   101,
     102,   103,   104,    23,    28,    25,    26,    27,   122,   179,
       4,   179,   179,   179,   179,   179,     3,    50,   180,   177,
       4,     4,     4,   125,   126,   127,    58,    74,    58,    76,
       3,   183,    53,   180,     6,   123,   122,   120,   179,     6,
     153,   180,   179,   183,   180,   147,    54,   180,   183,    55,
     152,   180,   140,   141,   142,   143,   183,   183,   183,    79,
     148,   183,   183,   165,   166,   167,   183,    74,   183,     3,
     172,   173,   174,     3,   183,   177,   123,   179,   183,   183,
     183,   169,   170,   171,   172,   173,   174,   175,   176,   183,
       4,   183,   180,   140,   141,   142,   143,   144,   183,   183,
     153,   148,   183,   183,    74,   183,   183,   183,   183,   183,
     183,     4,    63,    79,     6,     6,   123,     6,     6,     6,
       6,     6,   169,   170,   171,   172,   173,   174,   175,   176,
       6,     6,     6,   140,   141,   142,   143,   157,   145,     6,
       6,   148,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   123,     6,     6,   169,     3,   183,   179,
     183,   180,   169,   170,   171,   172,   173,   174,   175,   176,
     140,   141,   142,   143,     4,   180,   180,   180,   148,   180,
     180,   180,   180,   180,   180,   180,     6,   180,   180,   180,
     123,   180,   180,   183,   180,   180,    61,   456,   179,   169,
     170,   171,   172,   173,   174,   175,   176,   140,   141,   142,
     143,   180,   180,    85,   474,   148,   180,   180,   180,   180,
     180,   180,   180,    61,    61,    61,   155,   522,   263,   522,
      61,    16,    61,   144,   422,   554,   169,   170,   171,   172,
     173,   174,   175,   176,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   197,   488,   112,   138,    61,
     115,   116,    61,   118,    61,    61,   312,    61,   292,   521,
     216,   537,   481,   730,   481,   364,    -1,    -1,   693
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   157,   179,   185,   186,   187,
     189,   199,   200,   202,   204,   207,   208,   209,   210,   211,
     233,   238,   239,   240,   241,   242,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   231,    77,
     231,   154,   155,   160,    35,    36,    37,    38,    39,    40,
      43,   203,    29,    30,    32,    44,    45,   154,   155,   159,
     160,   161,    30,   149,   150,   151,     3,   231,     3,   234,
     235,   158,   210,   211,     0,   182,   274,    20,    22,    24,
     227,     8,   212,   214,    73,   273,   273,   273,   273,   273,
     275,   231,    73,   272,   272,   272,   272,   272,   181,    14,
     231,    77,    78,     3,     3,     3,   188,   189,   199,   200,
     204,   207,   208,   209,   238,   239,   240,   241,   242,     3,
     231,   162,   163,   162,   163,     3,   231,    56,   183,     6,
     180,   180,   187,    21,   179,   213,   214,    66,   221,    68,
     215,    74,     3,   231,   231,   231,     3,    63,   179,   201,
      75,     3,   231,   231,   231,     3,     3,     3,   205,   206,
      67,   224,     4,   271,   271,     3,     5,     6,    63,   156,
       3,     5,     6,    63,   156,     3,     5,     6,    63,   156,
      42,    46,    47,    51,    52,     3,     3,   179,   235,   271,
     213,   214,     3,     4,     5,     6,    74,    75,    83,    85,
     101,   102,   103,   104,   125,   126,   127,   147,   152,   165,
     166,   167,   172,   173,   174,   177,   179,   243,   245,   246,
     247,   248,   249,   250,   251,   253,   254,   255,   256,   257,
     259,   260,   261,   262,   264,   265,   266,   267,   268,    54,
      69,   219,    75,    56,   179,   201,   231,     3,   198,    34,
     211,    63,   169,   183,   224,   246,    79,    79,     3,     6,
     212,   180,   180,   179,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,    74,    75,   247,   179,
     179,    88,   246,   258,     4,     4,     4,     4,     6,   268,
     179,   179,   179,   179,   179,   247,   247,     5,     6,   211,
     246,    77,   183,   222,    56,   145,   146,    74,    76,   123,
     140,   141,   142,   143,   144,   148,   169,   170,   171,   172,
     173,   174,   175,   176,   181,   178,   183,   178,   183,   216,
     217,   246,   246,    70,   220,   209,     3,   119,   121,   190,
     191,   192,   197,    56,   179,   280,   180,   183,   179,   244,
     231,   246,   206,   179,   179,    48,    49,   180,    66,   180,
     243,   179,    74,   211,   246,   246,   258,    84,    86,    88,
       4,   246,     4,     4,     4,   180,   180,   179,   228,   229,
     230,   231,   236,   245,   164,   223,     3,   246,   246,    76,
     148,   179,    74,   122,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,     3,   174,
       6,     5,   183,    71,    72,   218,   246,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   112,   115,
     116,   118,   193,   120,   179,   180,   183,   209,   198,   179,
       3,   243,   183,    80,    81,    82,   269,   270,   269,     6,
     243,   180,   211,   180,    56,    87,    84,    86,   246,   246,
      77,   183,   183,   180,   183,   180,   183,   209,   183,    57,
      59,    60,    61,    62,    64,    65,   237,     3,    56,   232,
     248,   249,   250,   251,   252,   224,   179,   247,   211,   243,
     122,   145,   217,   179,   179,   179,   179,    74,   119,   121,
     122,   124,   194,   195,   196,   179,   198,    28,   277,   191,
     180,   198,   180,   179,     4,     3,   180,   183,   180,    50,
     180,   180,   193,   246,   246,    84,    87,   247,   263,   264,
     266,     4,     4,     4,   180,   229,    58,    58,     3,   183,
      53,   226,   211,   243,   180,   180,   247,     6,    90,    91,
      92,    93,    94,    97,    98,   117,    90,    91,    92,    93,
      94,    97,    98,   117,    90,    91,    92,    93,    94,    97,
      98,   117,   122,   120,   262,   195,   196,   198,   180,   179,
     153,   180,   243,   270,     6,   180,    84,   246,   180,   183,
     180,   183,   180,   180,   232,   230,   230,   179,   248,   249,
     250,   251,    54,    55,   225,   180,   180,   180,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   180,     3,   278,   279,     3,   153,   180,
       4,     4,    63,   198,   243,   246,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   169,   180,   183,    79,   276,     3,   183,   180,   246,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,     3,     5,     6,   279,
     179,   276,     4,   278,   183,   180,     6,   180,   276
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   184,   185,   186,   186,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     189,   189,   189,   189,   189,   189,   190,   190,   191,   191,
     192,   192,   193,   193,   193,   193,   193,   193,   193,   193,
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
     238,   238,   239,   239,   239,   240,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   242,   243,   243,   244,   244,   245,   245,
     246,   246,   246,   246,   246,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   248,   249,   249,   250,
     250,   251,   251,   252,   252,   252,   252,   252,   252,   252,
     252,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   254,   254,   255,   256,   256,   257,
     257,   257,   257,   258,   258,   259,   260,   260,   260,   260,
     261,   261,   261,   261,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   263,   263,   264,
     265,   265,   266,   267,   267,   268,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   268,   269,   269,   270,
     270,   270,   271,   272,   272,   273,   273,   274,   274,   275,
     275,   276,   276,   277,   277,   278,   278,   279,   279,   279,
     279,   280,   280,   280
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     8,     6,     7,     6,     1,     3,     1,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     4,     1,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
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
       4,     4,     3,     3,     4,     4,     5,     6,     7,     9,
       4,     5,     2,     2,     2,     2,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     3,     1,     3,     3,     5,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,    13,     6,     8,     4,
       6,     4,     6,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     4,     5,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     3,     2,
       2,     1,     1,     2,     0,     3,     0,     1,     0,     2,
       0,     4,     0,     4,     0,     1,     3,     1,     3,     3,
       3,     6,     7,     3
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
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2057 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2065 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 221 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2079 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 211 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2093 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2104 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2112 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2121 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2130 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 271 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2144 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2155 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2165 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2175 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2185 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2195 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2205 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2215 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 254 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2229 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 254 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2243 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2253 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2261 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2269 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2278 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2286 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2294 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2302 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2316 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2325 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2334 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2343 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2356 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2365 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 281 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2379 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 281 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2393 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 340 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2403 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2412 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2426 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 241 "parser.y"
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
#line 2443 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2451 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2459 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2467 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2475 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2513 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2521 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2529 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2537 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2566 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2574 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2582 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2590 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2598 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2606 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2622 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2630 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2638 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2646 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2654 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2665 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2679 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2693 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 177 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 2707 "parser.cpp"
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
#line 86 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2815 "parser.cpp"

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
#line 476 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3030 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 480 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3041 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 486 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3052 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 493 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3058 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 494 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3064 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3070 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 496 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3076 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3082 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3088 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3094 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3100 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3106 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 502 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3112 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 503 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3118 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 504 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3124 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 505 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3130 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 507 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3136 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 508 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3142 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 509 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3148 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3154 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 511 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3160 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 512 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3166 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3172 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3178 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 515 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3184 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 516 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3190 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3196 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 518 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3202 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 525 "parser.y"
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
#line 3222 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 542 "parser.y"
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
#line 3240 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 558 "parser.y"
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
#line 3273 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 587 "parser.y"
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
#line 3293 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 603 "parser.y"
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
#line 3314 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 621 "parser.y"
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
#line 3347 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 650 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3356 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 654 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3365 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 660 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3373 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 663 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3381 "parser.cpp"
    break;

  case 40: /* table_column: IDENTIFIER column_type default_expr  */
#line 669 "parser.y"
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
        case infinity::LogicalType::kTensor:
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
#line 3424 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 707 "parser.y"
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
#line 3463 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 743 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3469 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 744 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3475 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 745 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3481 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 746 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3487 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 747 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3493 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 748 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3499 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 749 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3505 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 750 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3511 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 751 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3517 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 752 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3523 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 753 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3529 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 754 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3535 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 755 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3541 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 756 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3547 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 757 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3553 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 758 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3559 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 759 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3565 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 760 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3571 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 761 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3577 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 764 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3583 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 766 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3589 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 767 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3595 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 768 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3601 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 769 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3607 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 772 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3613 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 773 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3619 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 774 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3625 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 775 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3631 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 776 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3637 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 777 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3643 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 778 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3649 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 779 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3655 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 780 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3661 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 781 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3667 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 782 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3673 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 783 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3679 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 784 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3685 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 785 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3691 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 786 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3697 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 787 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3703 "parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 788 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3709 "parser.cpp"
    break;

  case 83: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 789 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3715 "parser.cpp"
    break;

  case 84: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 790 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3721 "parser.cpp"
    break;

  case 85: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 791 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3727 "parser.cpp"
    break;

  case 86: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 792 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3733 "parser.cpp"
    break;

  case 87: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3739 "parser.cpp"
    break;

  case 88: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 794 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3745 "parser.cpp"
    break;

  case 89: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 795 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3751 "parser.cpp"
    break;

  case 90: /* column_constraints: column_constraint  */
#line 814 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3760 "parser.cpp"
    break;

  case 91: /* column_constraints: column_constraints column_constraint  */
#line 818 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3774 "parser.cpp"
    break;

  case 92: /* column_constraint: PRIMARY KEY  */
#line 828 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3782 "parser.cpp"
    break;

  case 93: /* column_constraint: UNIQUE  */
#line 831 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3790 "parser.cpp"
    break;

  case 94: /* column_constraint: NULLABLE  */
#line 834 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3798 "parser.cpp"
    break;

  case 95: /* column_constraint: NOT NULLABLE  */
#line 837 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3806 "parser.cpp"
    break;

  case 96: /* default_expr: DEFAULT constant_expr  */
#line 841 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 3814 "parser.cpp"
    break;

  case 97: /* default_expr: %empty  */
#line 844 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 3822 "parser.cpp"
    break;

  case 98: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 849 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3832 "parser.cpp"
    break;

  case 99: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 854 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3842 "parser.cpp"
    break;

  case 100: /* identifier_array: IDENTIFIER  */
#line 861 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3853 "parser.cpp"
    break;

  case 101: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 867 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3864 "parser.cpp"
    break;

  case 102: /* delete_statement: DELETE FROM table_name where_clause  */
#line 877 "parser.y"
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
#line 3881 "parser.cpp"
    break;

  case 103: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 893 "parser.y"
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
#line 3920 "parser.cpp"
    break;

  case 104: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 927 "parser.y"
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
#line 3937 "parser.cpp"
    break;

  case 105: /* optional_identifier_array: '(' identifier_array ')'  */
#line 940 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3945 "parser.cpp"
    break;

  case 106: /* optional_identifier_array: %empty  */
#line 943 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3953 "parser.cpp"
    break;

  case 107: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 950 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3963 "parser.cpp"
    break;

  case 108: /* explain_type: ANALYZE  */
#line 956 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3971 "parser.cpp"
    break;

  case 109: /* explain_type: AST  */
#line 959 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3979 "parser.cpp"
    break;

  case 110: /* explain_type: RAW  */
#line 962 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3987 "parser.cpp"
    break;

  case 111: /* explain_type: LOGICAL  */
#line 965 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3995 "parser.cpp"
    break;

  case 112: /* explain_type: PHYSICAL  */
#line 968 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4003 "parser.cpp"
    break;

  case 113: /* explain_type: PIPELINE  */
#line 971 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4011 "parser.cpp"
    break;

  case 114: /* explain_type: FRAGMENT  */
#line 974 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4019 "parser.cpp"
    break;

  case 115: /* explain_type: %empty  */
#line 977 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4027 "parser.cpp"
    break;

  case 116: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 984 "parser.y"
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
#line 4044 "parser.cpp"
    break;

  case 117: /* update_expr_array: update_expr  */
#line 997 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4053 "parser.cpp"
    break;

  case 118: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1001 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4062 "parser.cpp"
    break;

  case 119: /* update_expr: IDENTIFIER '=' expr  */
#line 1006 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4074 "parser.cpp"
    break;

  case 120: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1019 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4090 "parser.cpp"
    break;

  case 121: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1032 "parser.y"
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
#line 4108 "parser.cpp"
    break;

  case 122: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1047 "parser.y"
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
#line 4126 "parser.cpp"
    break;

  case 123: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1062 "parser.y"
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
#line 4144 "parser.cpp"
    break;

  case 124: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1077 "parser.y"
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
#line 4167 "parser.cpp"
    break;

  case 125: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1100 "parser.y"
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
#line 4213 "parser.cpp"
    break;

  case 126: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1141 "parser.y"
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
#line 4259 "parser.cpp"
    break;

  case 127: /* select_statement: select_without_paren  */
#line 1186 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4267 "parser.cpp"
    break;

  case 128: /* select_statement: select_with_paren  */
#line 1189 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4275 "parser.cpp"
    break;

  case 129: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1192 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4289 "parser.cpp"
    break;

  case 130: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1201 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4303 "parser.cpp"
    break;

  case 131: /* select_with_paren: '(' select_without_paren ')'  */
#line 1211 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4311 "parser.cpp"
    break;

  case 132: /* select_with_paren: '(' select_with_paren ')'  */
#line 1214 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4319 "parser.cpp"
    break;

  case 133: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1218 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4328 "parser.cpp"
    break;

  case 134: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1223 "parser.y"
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
#line 4354 "parser.cpp"
    break;

  case 135: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1245 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4362 "parser.cpp"
    break;

  case 136: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1248 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4370 "parser.cpp"
    break;

  case 137: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1253 "parser.y"
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
#line 4390 "parser.cpp"
    break;

  case 138: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1269 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4398 "parser.cpp"
    break;

  case 139: /* order_by_clause: %empty  */
#line 1272 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4406 "parser.cpp"
    break;

  case 140: /* order_by_expr_list: order_by_expr  */
#line 1276 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4415 "parser.cpp"
    break;

  case 141: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1280 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4424 "parser.cpp"
    break;

  case 142: /* order_by_expr: expr order_by_type  */
#line 1285 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4434 "parser.cpp"
    break;

  case 143: /* order_by_type: ASC  */
#line 1291 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4442 "parser.cpp"
    break;

  case 144: /* order_by_type: DESC  */
#line 1294 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4450 "parser.cpp"
    break;

  case 145: /* order_by_type: %empty  */
#line 1297 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4458 "parser.cpp"
    break;

  case 146: /* limit_expr: LIMIT expr  */
#line 1301 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4466 "parser.cpp"
    break;

  case 147: /* limit_expr: %empty  */
#line 1305 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4472 "parser.cpp"
    break;

  case 148: /* offset_expr: OFFSET expr  */
#line 1307 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4480 "parser.cpp"
    break;

  case 149: /* offset_expr: %empty  */
#line 1311 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4486 "parser.cpp"
    break;

  case 150: /* distinct: DISTINCT  */
#line 1313 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4494 "parser.cpp"
    break;

  case 151: /* distinct: %empty  */
#line 1316 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4502 "parser.cpp"
    break;

  case 152: /* from_clause: FROM table_reference  */
#line 1320 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4510 "parser.cpp"
    break;

  case 153: /* from_clause: %empty  */
#line 1323 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4518 "parser.cpp"
    break;

  case 154: /* search_clause: SEARCH sub_search_array  */
#line 1327 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4528 "parser.cpp"
    break;

  case 155: /* search_clause: %empty  */
#line 1332 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4536 "parser.cpp"
    break;

  case 156: /* where_clause: WHERE expr  */
#line 1336 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4544 "parser.cpp"
    break;

  case 157: /* where_clause: %empty  */
#line 1339 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4552 "parser.cpp"
    break;

  case 158: /* having_clause: HAVING expr  */
#line 1343 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4560 "parser.cpp"
    break;

  case 159: /* having_clause: %empty  */
#line 1346 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4568 "parser.cpp"
    break;

  case 160: /* group_by_clause: GROUP BY expr_array  */
#line 1350 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4576 "parser.cpp"
    break;

  case 161: /* group_by_clause: %empty  */
#line 1353 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4584 "parser.cpp"
    break;

  case 162: /* set_operator: UNION  */
#line 1357 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4592 "parser.cpp"
    break;

  case 163: /* set_operator: UNION ALL  */
#line 1360 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4600 "parser.cpp"
    break;

  case 164: /* set_operator: INTERSECT  */
#line 1363 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4608 "parser.cpp"
    break;

  case 165: /* set_operator: EXCEPT  */
#line 1366 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4616 "parser.cpp"
    break;

  case 166: /* table_reference: table_reference_unit  */
#line 1374 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4624 "parser.cpp"
    break;

  case 167: /* table_reference: table_reference ',' table_reference_unit  */
#line 1377 "parser.y"
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
#line 4642 "parser.cpp"
    break;

  case 170: /* table_reference_name: table_name table_alias  */
#line 1394 "parser.y"
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
#line 4660 "parser.cpp"
    break;

  case 171: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1408 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4671 "parser.cpp"
    break;

  case 172: /* table_name: IDENTIFIER  */
#line 1417 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4681 "parser.cpp"
    break;

  case 173: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1422 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4693 "parser.cpp"
    break;

  case 174: /* table_alias: AS IDENTIFIER  */
#line 1431 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4703 "parser.cpp"
    break;

  case 175: /* table_alias: IDENTIFIER  */
#line 1436 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4713 "parser.cpp"
    break;

  case 176: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1441 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4724 "parser.cpp"
    break;

  case 177: /* table_alias: %empty  */
#line 1447 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4732 "parser.cpp"
    break;

  case 178: /* with_clause: WITH with_expr_list  */
#line 1454 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4740 "parser.cpp"
    break;

  case 179: /* with_clause: %empty  */
#line 1457 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4748 "parser.cpp"
    break;

  case 180: /* with_expr_list: with_expr  */
#line 1461 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4757 "parser.cpp"
    break;

  case 181: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1464 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4766 "parser.cpp"
    break;

  case 182: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1469 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4778 "parser.cpp"
    break;

  case 183: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1481 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4790 "parser.cpp"
    break;

  case 184: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1488 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4803 "parser.cpp"
    break;

  case 185: /* join_type: INNER  */
#line 1502 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4811 "parser.cpp"
    break;

  case 186: /* join_type: LEFT  */
#line 1505 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4819 "parser.cpp"
    break;

  case 187: /* join_type: RIGHT  */
#line 1508 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4827 "parser.cpp"
    break;

  case 188: /* join_type: OUTER  */
#line 1511 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4835 "parser.cpp"
    break;

  case 189: /* join_type: FULL  */
#line 1514 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4843 "parser.cpp"
    break;

  case 190: /* join_type: CROSS  */
#line 1517 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4851 "parser.cpp"
    break;

  case 191: /* join_type: %empty  */
#line 1520 "parser.y"
                {
}
#line 4858 "parser.cpp"
    break;

  case 192: /* show_statement: SHOW DATABASES  */
#line 1526 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4867 "parser.cpp"
    break;

  case 193: /* show_statement: SHOW TABLES  */
#line 1530 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4876 "parser.cpp"
    break;

  case 194: /* show_statement: SHOW VIEWS  */
#line 1534 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4885 "parser.cpp"
    break;

  case 195: /* show_statement: SHOW CONFIGS  */
#line 1538 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4894 "parser.cpp"
    break;

  case 196: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1542 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4906 "parser.cpp"
    break;

  case 197: /* show_statement: SHOW PROFILES  */
#line 1549 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4915 "parser.cpp"
    break;

  case 198: /* show_statement: SHOW SESSION VARIABLES  */
#line 1553 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 4924 "parser.cpp"
    break;

  case 199: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1557 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 4933 "parser.cpp"
    break;

  case 200: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1561 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4944 "parser.cpp"
    break;

  case 201: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1567 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4955 "parser.cpp"
    break;

  case 202: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1573 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4966 "parser.cpp"
    break;

  case 203: /* show_statement: SHOW TABLE table_name  */
#line 1579 "parser.y"
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
#line 4982 "parser.cpp"
    break;

  case 204: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1590 "parser.y"
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
#line 4998 "parser.cpp"
    break;

  case 205: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1601 "parser.y"
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
#line 5014 "parser.cpp"
    break;

  case 206: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1612 "parser.y"
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
#line 5031 "parser.cpp"
    break;

  case 207: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1624 "parser.y"
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
#line 5048 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1636 "parser.y"
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
#line 5066 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1649 "parser.y"
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
#line 5085 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1663 "parser.y"
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
#line 5101 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1674 "parser.y"
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
#line 5120 "parser.cpp"
    break;

  case 212: /* flush_statement: FLUSH DATA  */
#line 1692 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5129 "parser.cpp"
    break;

  case 213: /* flush_statement: FLUSH LOG  */
#line 1696 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5138 "parser.cpp"
    break;

  case 214: /* flush_statement: FLUSH BUFFER  */
#line 1700 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5147 "parser.cpp"
    break;

  case 215: /* optimize_statement: OPTIMIZE table_name  */
#line 1708 "parser.y"
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
#line 5162 "parser.cpp"
    break;

  case 216: /* command_statement: USE IDENTIFIER  */
#line 1722 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5173 "parser.cpp"
    break;

  case 217: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1728 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5183 "parser.cpp"
    break;

  case 218: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1733 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5194 "parser.cpp"
    break;

  case 219: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1739 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5205 "parser.cpp"
    break;

  case 220: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1745 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5218 "parser.cpp"
    break;

  case 221: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1753 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5229 "parser.cpp"
    break;

  case 222: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1759 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5240 "parser.cpp"
    break;

  case 223: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1765 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5251 "parser.cpp"
    break;

  case 224: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1771 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5262 "parser.cpp"
    break;

  case 225: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1777 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5275 "parser.cpp"
    break;

  case 226: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1785 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5286 "parser.cpp"
    break;

  case 227: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1791 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5297 "parser.cpp"
    break;

  case 228: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1797 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5308 "parser.cpp"
    break;

  case 229: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1803 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5319 "parser.cpp"
    break;

  case 230: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1809 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5332 "parser.cpp"
    break;

  case 231: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1817 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5343 "parser.cpp"
    break;

  case 232: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1823 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5354 "parser.cpp"
    break;

  case 233: /* compact_statement: COMPACT TABLE table_name  */
#line 1830 "parser.y"
                                            {
    std::string schema_name;
    if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        schema_name = std::string((yyvsp[0].table_name_t)->schema_name_ptr_);
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    std::string table_name = std::string((yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);

    (yyval.compact_stmt) = new infinity::ManualCompactStatement(std::move(schema_name), std::move(table_name));
    delete (yyvsp[0].table_name_t);
}
#line 5371 "parser.cpp"
    break;

  case 234: /* expr_array: expr_alias  */
#line 1847 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5380 "parser.cpp"
    break;

  case 235: /* expr_array: expr_array ',' expr_alias  */
#line 1851 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5389 "parser.cpp"
    break;

  case 236: /* expr_array_list: '(' expr_array ')'  */
#line 1856 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5398 "parser.cpp"
    break;

  case 237: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1860 "parser.y"
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
#line 5418 "parser.cpp"
    break;

  case 238: /* expr_alias: expr AS IDENTIFIER  */
#line 1887 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5429 "parser.cpp"
    break;

  case 239: /* expr_alias: expr  */
#line 1893 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5437 "parser.cpp"
    break;

  case 245: /* operand: '(' expr ')'  */
#line 1903 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5445 "parser.cpp"
    break;

  case 246: /* operand: '(' select_without_paren ')'  */
#line 1906 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5456 "parser.cpp"
    break;

  case 247: /* operand: constant_expr  */
#line 1912 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5464 "parser.cpp"
    break;

  case 256: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1924 "parser.y"
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
#line 5635 "parser.cpp"
    break;

  case 257: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 2091 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5648 "parser.cpp"
    break;

  case 258: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 2099 "parser.y"
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
#line 5663 "parser.cpp"
    break;

  case 259: /* query_expr: QUERY '(' STRING ')'  */
#line 2110 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5674 "parser.cpp"
    break;

  case 260: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2116 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5687 "parser.cpp"
    break;

  case 261: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2125 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5698 "parser.cpp"
    break;

  case 262: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2131 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5711 "parser.cpp"
    break;

  case 263: /* sub_search_array: knn_expr  */
#line 2141 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5720 "parser.cpp"
    break;

  case 264: /* sub_search_array: match_expr  */
#line 2145 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5729 "parser.cpp"
    break;

  case 265: /* sub_search_array: query_expr  */
#line 2149 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5738 "parser.cpp"
    break;

  case 266: /* sub_search_array: fusion_expr  */
#line 2153 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5747 "parser.cpp"
    break;

  case 267: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 2157 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5756 "parser.cpp"
    break;

  case 268: /* sub_search_array: sub_search_array ',' match_expr  */
#line 2161 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5765 "parser.cpp"
    break;

  case 269: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2165 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5774 "parser.cpp"
    break;

  case 270: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2169 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5783 "parser.cpp"
    break;

  case 271: /* function_expr: IDENTIFIER '(' ')'  */
#line 2174 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5796 "parser.cpp"
    break;

  case 272: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2182 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5809 "parser.cpp"
    break;

  case 273: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2190 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5823 "parser.cpp"
    break;

  case 274: /* function_expr: operand IS NOT NULLABLE  */
#line 2199 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5835 "parser.cpp"
    break;

  case 275: /* function_expr: operand IS NULLABLE  */
#line 2206 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5847 "parser.cpp"
    break;

  case 276: /* function_expr: NOT operand  */
#line 2213 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5859 "parser.cpp"
    break;

  case 277: /* function_expr: '-' operand  */
#line 2220 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5871 "parser.cpp"
    break;

  case 278: /* function_expr: '+' operand  */
#line 2227 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5883 "parser.cpp"
    break;

  case 279: /* function_expr: operand '-' operand  */
#line 2234 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5896 "parser.cpp"
    break;

  case 280: /* function_expr: operand '+' operand  */
#line 2242 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5909 "parser.cpp"
    break;

  case 281: /* function_expr: operand '*' operand  */
#line 2250 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5922 "parser.cpp"
    break;

  case 282: /* function_expr: operand '/' operand  */
#line 2258 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5935 "parser.cpp"
    break;

  case 283: /* function_expr: operand '%' operand  */
#line 2266 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5948 "parser.cpp"
    break;

  case 284: /* function_expr: operand '=' operand  */
#line 2274 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5961 "parser.cpp"
    break;

  case 285: /* function_expr: operand EQUAL operand  */
#line 2282 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5974 "parser.cpp"
    break;

  case 286: /* function_expr: operand NOT_EQ operand  */
#line 2290 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5987 "parser.cpp"
    break;

  case 287: /* function_expr: operand '<' operand  */
#line 2298 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6000 "parser.cpp"
    break;

  case 288: /* function_expr: operand '>' operand  */
#line 2306 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6013 "parser.cpp"
    break;

  case 289: /* function_expr: operand LESS_EQ operand  */
#line 2314 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6026 "parser.cpp"
    break;

  case 290: /* function_expr: operand GREATER_EQ operand  */
#line 2322 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6039 "parser.cpp"
    break;

  case 291: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2330 "parser.y"
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
#line 6074 "parser.cpp"
    break;

  case 292: /* function_expr: operand LIKE operand  */
#line 2360 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6087 "parser.cpp"
    break;

  case 293: /* function_expr: operand NOT LIKE operand  */
#line 2368 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6100 "parser.cpp"
    break;

  case 294: /* conjunction_expr: expr AND expr  */
#line 2377 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6113 "parser.cpp"
    break;

  case 295: /* conjunction_expr: expr OR expr  */
#line 2385 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6126 "parser.cpp"
    break;

  case 296: /* between_expr: operand BETWEEN operand AND operand  */
#line 2394 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6138 "parser.cpp"
    break;

  case 297: /* in_expr: operand IN '(' expr_array ')'  */
#line 2402 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6149 "parser.cpp"
    break;

  case 298: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2408 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6160 "parser.cpp"
    break;

  case 299: /* case_expr: CASE expr case_check_array END  */
#line 2415 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6171 "parser.cpp"
    break;

  case 300: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2421 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6183 "parser.cpp"
    break;

  case 301: /* case_expr: CASE case_check_array END  */
#line 2428 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6193 "parser.cpp"
    break;

  case 302: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2433 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6204 "parser.cpp"
    break;

  case 303: /* case_check_array: WHEN expr THEN expr  */
#line 2440 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6216 "parser.cpp"
    break;

  case 304: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2447 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6228 "parser.cpp"
    break;

  case 305: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2455 "parser.y"
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
#line 6256 "parser.cpp"
    break;

  case 306: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2479 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6267 "parser.cpp"
    break;

  case 307: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2485 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6278 "parser.cpp"
    break;

  case 308: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2491 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6290 "parser.cpp"
    break;

  case 309: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2498 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6302 "parser.cpp"
    break;

  case 310: /* column_expr: IDENTIFIER  */
#line 2506 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6314 "parser.cpp"
    break;

  case 311: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2513 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6326 "parser.cpp"
    break;

  case 312: /* column_expr: '*'  */
#line 2520 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6336 "parser.cpp"
    break;

  case 313: /* column_expr: column_expr '.' '*'  */
#line 2525 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6350 "parser.cpp"
    break;

  case 314: /* constant_expr: STRING  */
#line 2535 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6360 "parser.cpp"
    break;

  case 315: /* constant_expr: TRUE  */
#line 2540 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6370 "parser.cpp"
    break;

  case 316: /* constant_expr: FALSE  */
#line 2545 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6380 "parser.cpp"
    break;

  case 317: /* constant_expr: DOUBLE_VALUE  */
#line 2550 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6390 "parser.cpp"
    break;

  case 318: /* constant_expr: LONG_VALUE  */
#line 2555 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6400 "parser.cpp"
    break;

  case 319: /* constant_expr: DATE STRING  */
#line 2560 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6410 "parser.cpp"
    break;

  case 320: /* constant_expr: TIME STRING  */
#line 2565 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6420 "parser.cpp"
    break;

  case 321: /* constant_expr: DATETIME STRING  */
#line 2570 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6430 "parser.cpp"
    break;

  case 322: /* constant_expr: TIMESTAMP STRING  */
#line 2575 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6440 "parser.cpp"
    break;

  case 323: /* constant_expr: INTERVAL interval_expr  */
#line 2580 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6448 "parser.cpp"
    break;

  case 324: /* constant_expr: interval_expr  */
#line 2583 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6456 "parser.cpp"
    break;

  case 325: /* constant_expr: long_array_expr  */
#line 2586 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6464 "parser.cpp"
    break;

  case 326: /* constant_expr: double_array_expr  */
#line 2589 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6472 "parser.cpp"
    break;

  case 327: /* array_expr: long_array_expr  */
#line 2593 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6480 "parser.cpp"
    break;

  case 328: /* array_expr: double_array_expr  */
#line 2596 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6488 "parser.cpp"
    break;

  case 329: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2600 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6496 "parser.cpp"
    break;

  case 330: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2604 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6506 "parser.cpp"
    break;

  case 331: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2609 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6515 "parser.cpp"
    break;

  case 332: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2614 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6523 "parser.cpp"
    break;

  case 333: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2618 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6533 "parser.cpp"
    break;

  case 334: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2623 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6542 "parser.cpp"
    break;

  case 335: /* interval_expr: LONG_VALUE SECONDS  */
#line 2628 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6553 "parser.cpp"
    break;

  case 336: /* interval_expr: LONG_VALUE SECOND  */
#line 2634 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6564 "parser.cpp"
    break;

  case 337: /* interval_expr: LONG_VALUE MINUTES  */
#line 2640 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6575 "parser.cpp"
    break;

  case 338: /* interval_expr: LONG_VALUE MINUTE  */
#line 2646 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6586 "parser.cpp"
    break;

  case 339: /* interval_expr: LONG_VALUE HOURS  */
#line 2652 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6597 "parser.cpp"
    break;

  case 340: /* interval_expr: LONG_VALUE HOUR  */
#line 2658 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6608 "parser.cpp"
    break;

  case 341: /* interval_expr: LONG_VALUE DAYS  */
#line 2664 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6619 "parser.cpp"
    break;

  case 342: /* interval_expr: LONG_VALUE DAY  */
#line 2670 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6630 "parser.cpp"
    break;

  case 343: /* interval_expr: LONG_VALUE MONTHS  */
#line 2676 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6641 "parser.cpp"
    break;

  case 344: /* interval_expr: LONG_VALUE MONTH  */
#line 2682 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6652 "parser.cpp"
    break;

  case 345: /* interval_expr: LONG_VALUE YEARS  */
#line 2688 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6663 "parser.cpp"
    break;

  case 346: /* interval_expr: LONG_VALUE YEAR  */
#line 2694 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6674 "parser.cpp"
    break;

  case 347: /* copy_option_list: copy_option  */
#line 2705 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6683 "parser.cpp"
    break;

  case 348: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2709 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6692 "parser.cpp"
    break;

  case 349: /* copy_option: FORMAT IDENTIFIER  */
#line 2714 "parser.y"
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
#line 6719 "parser.cpp"
    break;

  case 350: /* copy_option: DELIMITER STRING  */
#line 2736 "parser.y"
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
#line 6734 "parser.cpp"
    break;

  case 351: /* copy_option: HEADER  */
#line 2746 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6744 "parser.cpp"
    break;

  case 352: /* file_path: STRING  */
#line 2752 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6752 "parser.cpp"
    break;

  case 353: /* if_exists: IF EXISTS  */
#line 2756 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6758 "parser.cpp"
    break;

  case 354: /* if_exists: %empty  */
#line 2757 "parser.y"
  { (yyval.bool_value) = false; }
#line 6764 "parser.cpp"
    break;

  case 355: /* if_not_exists: IF NOT EXISTS  */
#line 2759 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6770 "parser.cpp"
    break;

  case 356: /* if_not_exists: %empty  */
#line 2760 "parser.y"
  { (yyval.bool_value) = false; }
#line 6776 "parser.cpp"
    break;

  case 359: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2775 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6789 "parser.cpp"
    break;

  case 360: /* if_not_exists_info: %empty  */
#line 2783 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6797 "parser.cpp"
    break;

  case 361: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2787 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6805 "parser.cpp"
    break;

  case 362: /* with_index_param_list: %empty  */
#line 2790 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6813 "parser.cpp"
    break;

  case 363: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2794 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 6821 "parser.cpp"
    break;

  case 364: /* optional_table_properties_list: %empty  */
#line 2797 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 6829 "parser.cpp"
    break;

  case 365: /* index_param_list: index_param  */
#line 2801 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6838 "parser.cpp"
    break;

  case 366: /* index_param_list: index_param_list ',' index_param  */
#line 2805 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6847 "parser.cpp"
    break;

  case 367: /* index_param: IDENTIFIER  */
#line 2810 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6857 "parser.cpp"
    break;

  case 368: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2815 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6870 "parser.cpp"
    break;

  case 369: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2823 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6882 "parser.cpp"
    break;

  case 370: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2830 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6894 "parser.cpp"
    break;

  case 371: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2841 "parser.y"
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
#line 6947 "parser.cpp"
    break;

  case 372: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2889 "parser.y"
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
#line 7001 "parser.cpp"
    break;

  case 373: /* index_info_list: '(' identifier_array ')'  */
#line 2938 "parser.y"
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
#line 7019 "parser.cpp"
    break;


#line 7023 "parser.cpp"

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

#line 2952 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

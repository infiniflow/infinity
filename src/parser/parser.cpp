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
  YYSYMBOL_TEXT = 118,                     /* TEXT  */
  YYSYMBOL_TENSOR = 119,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 120,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 121,              /* TENSORARRAY  */
  YYSYMBOL_PRIMARY = 122,                  /* PRIMARY  */
  YYSYMBOL_KEY = 123,                      /* KEY  */
  YYSYMBOL_UNIQUE = 124,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 125,                 /* NULLABLE  */
  YYSYMBOL_IS = 126,                       /* IS  */
  YYSYMBOL_DEFAULT = 127,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 128,                     /* TRUE  */
  YYSYMBOL_FALSE = 129,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 130,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 131,                   /* SECOND  */
  YYSYMBOL_SECONDS = 132,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 133,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 134,                  /* MINUTES  */
  YYSYMBOL_HOUR = 135,                     /* HOUR  */
  YYSYMBOL_HOURS = 136,                    /* HOURS  */
  YYSYMBOL_DAY = 137,                      /* DAY  */
  YYSYMBOL_DAYS = 138,                     /* DAYS  */
  YYSYMBOL_MONTH = 139,                    /* MONTH  */
  YYSYMBOL_MONTHS = 140,                   /* MONTHS  */
  YYSYMBOL_YEAR = 141,                     /* YEAR  */
  YYSYMBOL_YEARS = 142,                    /* YEARS  */
  YYSYMBOL_EQUAL = 143,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 144,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 145,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 146,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 147,                  /* BETWEEN  */
  YYSYMBOL_AND = 148,                      /* AND  */
  YYSYMBOL_OR = 149,                       /* OR  */
  YYSYMBOL_EXTRACT = 150,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 151,                     /* LIKE  */
  YYSYMBOL_DATA = 152,                     /* DATA  */
  YYSYMBOL_LOG = 153,                      /* LOG  */
  YYSYMBOL_BUFFER = 154,                   /* BUFFER  */
  YYSYMBOL_KNN = 155,                      /* KNN  */
  YYSYMBOL_USING = 156,                    /* USING  */
  YYSYMBOL_SESSION = 157,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 158,                   /* GLOBAL  */
  YYSYMBOL_OFF = 159,                      /* OFF  */
  YYSYMBOL_EXPORT = 160,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 161,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 162,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 163,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 164,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 165,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 166,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 167,                   /* SEARCH  */
  YYSYMBOL_MATCH = 168,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 169,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 170,                    /* QUERY  */
  YYSYMBOL_FUSION = 171,                   /* FUSION  */
  YYSYMBOL_NUMBER = 172,                   /* NUMBER  */
  YYSYMBOL_173_ = 173,                     /* '='  */
  YYSYMBOL_174_ = 174,                     /* '<'  */
  YYSYMBOL_175_ = 175,                     /* '>'  */
  YYSYMBOL_176_ = 176,                     /* '+'  */
  YYSYMBOL_177_ = 177,                     /* '-'  */
  YYSYMBOL_178_ = 178,                     /* '*'  */
  YYSYMBOL_179_ = 179,                     /* '/'  */
  YYSYMBOL_180_ = 180,                     /* '%'  */
  YYSYMBOL_181_ = 181,                     /* '['  */
  YYSYMBOL_182_ = 182,                     /* ']'  */
  YYSYMBOL_183_ = 183,                     /* '('  */
  YYSYMBOL_184_ = 184,                     /* ')'  */
  YYSYMBOL_185_ = 185,                     /* '.'  */
  YYSYMBOL_186_ = 186,                     /* ';'  */
  YYSYMBOL_187_ = 187,                     /* ','  */
  YYSYMBOL_YYACCEPT = 188,                 /* $accept  */
  YYSYMBOL_input_pattern = 189,            /* input_pattern  */
  YYSYMBOL_statement_list = 190,           /* statement_list  */
  YYSYMBOL_statement = 191,                /* statement  */
  YYSYMBOL_explainable_statement = 192,    /* explainable_statement  */
  YYSYMBOL_create_statement = 193,         /* create_statement  */
  YYSYMBOL_table_element_array = 194,      /* table_element_array  */
  YYSYMBOL_table_element = 195,            /* table_element  */
  YYSYMBOL_table_column = 196,             /* table_column  */
  YYSYMBOL_column_type = 197,              /* column_type  */
  YYSYMBOL_column_constraints = 198,       /* column_constraints  */
  YYSYMBOL_column_constraint = 199,        /* column_constraint  */
  YYSYMBOL_default_expr = 200,             /* default_expr  */
  YYSYMBOL_table_constraint = 201,         /* table_constraint  */
  YYSYMBOL_identifier_array = 202,         /* identifier_array  */
  YYSYMBOL_delete_statement = 203,         /* delete_statement  */
  YYSYMBOL_insert_statement = 204,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 205, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 206,        /* explain_statement  */
  YYSYMBOL_explain_type = 207,             /* explain_type  */
  YYSYMBOL_update_statement = 208,         /* update_statement  */
  YYSYMBOL_update_expr_array = 209,        /* update_expr_array  */
  YYSYMBOL_update_expr = 210,              /* update_expr  */
  YYSYMBOL_drop_statement = 211,           /* drop_statement  */
  YYSYMBOL_copy_statement = 212,           /* copy_statement  */
  YYSYMBOL_select_statement = 213,         /* select_statement  */
  YYSYMBOL_select_with_paren = 214,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 215,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 216, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 217, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 218, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 219,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 220,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 221,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 222,            /* order_by_type  */
  YYSYMBOL_limit_expr = 223,               /* limit_expr  */
  YYSYMBOL_offset_expr = 224,              /* offset_expr  */
  YYSYMBOL_distinct = 225,                 /* distinct  */
  YYSYMBOL_from_clause = 226,              /* from_clause  */
  YYSYMBOL_search_clause = 227,            /* search_clause  */
  YYSYMBOL_where_clause = 228,             /* where_clause  */
  YYSYMBOL_having_clause = 229,            /* having_clause  */
  YYSYMBOL_group_by_clause = 230,          /* group_by_clause  */
  YYSYMBOL_set_operator = 231,             /* set_operator  */
  YYSYMBOL_table_reference = 232,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 233,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 234,     /* table_reference_name  */
  YYSYMBOL_table_name = 235,               /* table_name  */
  YYSYMBOL_table_alias = 236,              /* table_alias  */
  YYSYMBOL_with_clause = 237,              /* with_clause  */
  YYSYMBOL_with_expr_list = 238,           /* with_expr_list  */
  YYSYMBOL_with_expr = 239,                /* with_expr  */
  YYSYMBOL_join_clause = 240,              /* join_clause  */
  YYSYMBOL_join_type = 241,                /* join_type  */
  YYSYMBOL_show_statement = 242,           /* show_statement  */
  YYSYMBOL_flush_statement = 243,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 244,       /* optimize_statement  */
  YYSYMBOL_command_statement = 245,        /* command_statement  */
  YYSYMBOL_compact_statement = 246,        /* compact_statement  */
  YYSYMBOL_expr_array = 247,               /* expr_array  */
  YYSYMBOL_expr_array_list = 248,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 249,               /* expr_alias  */
  YYSYMBOL_expr = 250,                     /* expr  */
  YYSYMBOL_operand = 251,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 252, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 253,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 254,        /* match_vector_expr  */
  YYSYMBOL_match_text_expr = 255,          /* match_text_expr  */
  YYSYMBOL_query_expr = 256,               /* query_expr  */
  YYSYMBOL_fusion_expr = 257,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 258,         /* sub_search_array  */
  YYSYMBOL_function_expr = 259,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 260,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 261,             /* between_expr  */
  YYSYMBOL_in_expr = 262,                  /* in_expr  */
  YYSYMBOL_case_expr = 263,                /* case_expr  */
  YYSYMBOL_case_check_array = 264,         /* case_check_array  */
  YYSYMBOL_cast_expr = 265,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 266,            /* subquery_expr  */
  YYSYMBOL_column_expr = 267,              /* column_expr  */
  YYSYMBOL_constant_expr = 268,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 269,        /* common_array_expr  */
  YYSYMBOL_subarray_array_expr = 270,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 271, /* unclosed_subarray_array_expr  */
  YYSYMBOL_array_expr = 272,               /* array_expr  */
  YYSYMBOL_long_array_expr = 273,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 274, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 275,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 276, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 277,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 278,         /* copy_option_list  */
  YYSYMBOL_copy_option = 279,              /* copy_option  */
  YYSYMBOL_file_path = 280,                /* file_path  */
  YYSYMBOL_if_exists = 281,                /* if_exists  */
  YYSYMBOL_if_not_exists = 282,            /* if_not_exists  */
  YYSYMBOL_semicolon = 283,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 284,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 285,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 286, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 287,         /* index_param_list  */
  YYSYMBOL_index_param = 288,              /* index_param  */
  YYSYMBOL_index_info_list = 289           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 418 "parser.cpp"

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
#define YYLAST   1002

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  188
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  393
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  808

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   427


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
       2,     2,     2,     2,     2,     2,     2,   180,     2,     2,
     183,   184,   178,   176,   187,   177,   185,   179,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   186,
     174,   173,   175,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   181,     2,   182,     2,     2,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171,   172
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   477,   477,   481,   487,   494,   495,   496,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,   519,
     526,   543,   559,   588,   604,   622,   651,   655,   661,   664,
     670,   713,   749,   750,   751,   752,   753,   754,   755,   756,
     757,   758,   759,   760,   761,   762,   763,   764,   765,   766,
     767,   770,   772,   773,   774,   775,   778,   779,   780,   781,
     782,   783,   784,   785,   786,   787,   788,   789,   790,   791,
     792,   793,   794,   795,   796,   797,   798,   799,   800,   801,
     802,   803,   804,   805,   806,   807,   808,   809,   810,   811,
     830,   834,   844,   847,   850,   853,   857,   860,   865,   870,
     877,   883,   893,   909,   943,   956,   959,   966,   972,   975,
     978,   981,   984,   987,   990,   993,  1000,  1013,  1017,  1022,
    1035,  1048,  1063,  1078,  1093,  1116,  1157,  1202,  1205,  1208,
    1217,  1227,  1230,  1234,  1239,  1261,  1264,  1269,  1285,  1288,
    1292,  1296,  1301,  1307,  1310,  1313,  1317,  1321,  1323,  1327,
    1329,  1332,  1336,  1339,  1343,  1348,  1352,  1355,  1359,  1362,
    1366,  1369,  1373,  1376,  1379,  1382,  1390,  1393,  1408,  1408,
    1410,  1424,  1433,  1438,  1447,  1452,  1457,  1463,  1470,  1473,
    1477,  1480,  1485,  1497,  1504,  1518,  1521,  1524,  1527,  1530,
    1533,  1536,  1542,  1546,  1550,  1554,  1558,  1565,  1569,  1573,
    1577,  1583,  1589,  1595,  1606,  1617,  1628,  1640,  1652,  1665,
    1679,  1690,  1708,  1712,  1716,  1724,  1738,  1744,  1749,  1755,
    1761,  1769,  1775,  1781,  1787,  1793,  1801,  1807,  1813,  1819,
    1825,  1833,  1839,  1846,  1863,  1867,  1872,  1876,  1903,  1909,
    1913,  1914,  1915,  1916,  1917,  1919,  1922,  1928,  1931,  1932,
    1933,  1934,  1935,  1936,  1937,  1938,  1939,  1941,  1944,  1950,
    1972,  2138,  2146,  2157,  2163,  2172,  2178,  2188,  2192,  2196,
    2200,  2204,  2208,  2212,  2216,  2220,  2224,  2229,  2237,  2245,
    2254,  2261,  2268,  2275,  2282,  2289,  2297,  2305,  2313,  2321,
    2329,  2337,  2345,  2353,  2361,  2369,  2377,  2385,  2415,  2423,
    2432,  2440,  2449,  2457,  2463,  2470,  2476,  2483,  2488,  2495,
    2502,  2510,  2534,  2540,  2546,  2553,  2561,  2568,  2575,  2580,
    2590,  2595,  2600,  2605,  2610,  2615,  2620,  2625,  2630,  2635,
    2638,  2641,  2645,  2648,  2652,  2656,  2661,  2666,  2669,  2673,
    2677,  2682,  2687,  2691,  2696,  2701,  2707,  2713,  2719,  2725,
    2731,  2737,  2743,  2749,  2755,  2761,  2767,  2778,  2782,  2787,
    2809,  2819,  2825,  2829,  2830,  2832,  2833,  2835,  2836,  2848,
    2856,  2860,  2863,  2867,  2870,  2874,  2878,  2883,  2888,  2896,
    2903,  2914,  2962,  3011
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
  "EMBEDDING", "VECTOR", "BIT", "TEXT", "TENSOR", "SPARSE", "TENSORARRAY",
  "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE",
  "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS",
  "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ",
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE",
  "DATA", "LOG", "BUFFER", "KNN", "USING", "SESSION", "GLOBAL", "OFF",
  "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "SEARCH", "MATCH", "MAXSIM", "QUERY", "FUSION", "NUMBER",
  "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'",
  "'('", "')'", "'.'", "';'", "','", "$accept", "input_pattern",
  "statement_list", "statement", "explainable_statement",
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
  "compact_statement", "expr_array", "expr_array_list", "expr_alias",
  "expr", "operand", "extra_match_tensor_option", "match_tensor_expr",
  "match_vector_expr", "match_text_expr", "query_expr", "fusion_expr",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "common_array_expr",
  "subarray_array_expr", "unclosed_subarray_array_expr", "array_expr",
  "long_array_expr", "unclosed_long_array_expr", "double_array_expr",
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

#define YYPACT_NINF (-673)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-381)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     551,   197,    13,   235,    66,    -1,    66,    45,   482,   261,
      58,   119,    97,    66,   112,   -19,   -42,   146,    15,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,   209,  -673,  -673,
     169,  -673,  -673,  -673,  -673,  -673,   133,   133,   133,   133,
      -3,    66,   157,   157,   157,   157,   157,    49,   222,    66,
     207,   241,   258,   276,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,   609,   315,    66,  -673,  -673,  -673,   161,   243,  -673,
     332,  -673,    66,  -673,  -673,  -673,  -673,  -673,   236,   158,
    -673,   347,   175,   177,  -673,    52,  -673,   352,  -673,  -673,
       5,   325,  -673,   334,   348,   423,    66,    66,    66,   431,
     379,   267,   369,   449,    66,    66,    66,   458,   461,   468,
     407,   474,   474,    19,    24,    48,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,   209,  -673,  -673,  -673,  -673,  -673,  -673,
     256,  -673,   481,  -673,   486,  -673,  -673,   310,   112,   474,
    -673,  -673,  -673,  -673,     5,  -673,  -673,  -673,   366,   443,
     433,   428,  -673,   -38,  -673,   267,  -673,    66,   502,     1,
    -673,  -673,  -673,  -673,  -673,   446,  -673,   338,   -44,  -673,
     366,  -673,  -673,   444,   445,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
     528,   533,  -673,  -673,  -673,  -673,  -673,   169,  -673,  -673,
     356,   357,   362,  -673,  -673,   737,   425,   363,   365,   277,
     546,   547,   548,   552,  -673,  -673,   553,   374,   170,   385,
     386,   509,   509,  -673,     3,   309,   -60,  -673,   -16,   569,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,   387,  -673,  -673,  -673,   -97,  -673,  -673,   -14,
    -673,    22,  -673,   366,   366,   500,  -673,   -42,    25,   515,
     390,  -673,   -95,   396,  -673,    66,   366,   468,  -673,   219,
     397,   398,  -673,   367,   400,  -673,  -673,   166,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
     509,   399,   634,   506,   366,   366,   -32,   159,  -673,  -673,
    -673,  -673,   737,  -673,   582,   404,   405,   406,   586,   587,
     121,   121,  -673,  -673,  -673,   414,    31,     4,   366,   432,
     597,   366,   366,   -45,   421,    74,   509,   509,   509,   509,
     509,   509,   509,   509,   509,   509,   509,   509,   509,   509,
       8,  -673,   424,  -673,   601,  -673,   604,   427,  -673,   -29,
     219,   366,  -673,   209,   736,   492,   434,   -34,  -673,  -673,
    -673,   -42,   502,   441,  -673,   623,   366,   440,  -673,   219,
    -673,   229,   229,   622,  -673,  -673,   366,  -673,    27,   506,
     478,   447,    -9,   -55,   164,  -673,   366,   366,   556,   366,
     636,    12,    76,   179,  -673,  -673,   -42,   454,   339,  -673,
      30,  -673,  -673,   206,   407,  -673,  -673,   494,   463,   509,
     309,   519,  -673,   644,   644,    79,    79,   578,   644,   644,
      79,    79,   121,   121,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,   366,  -673,  -673,  -673,   219,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,   464,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,   465,
     467,   470,   477,   480,   -17,   487,   502,   637,    25,   209,
     198,   502,  -673,   202,   488,   647,   661,  -673,   203,  -673,
     230,   625,   248,  -673,   489,  -673,   736,   366,  -673,   366,
     -36,   -46,   509,    26,   485,  -673,    33,  -673,   672,  -673,
     678,    -8,     4,   620,  -673,  -673,  -673,  -673,  -673,  -673,
     626,  -673,   680,  -673,  -673,  -673,  -673,  -673,  -673,   504,
     640,   309,   644,   505,   249,  -673,   509,  -673,   688,   231,
     564,   674,   323,   768,   580,   583,  -673,  -673,    93,   -17,
    -673,  -673,   502,   259,   526,  -673,  -673,   554,   278,  -673,
     366,  -673,  -673,  -673,   229,  -673,   711,  -673,  -673,   534,
     219,   -26,  -673,   366,   523,   538,   721,   424,   543,   544,
      30,   339,     4,     4,   549,   206,   676,   681,   555,   279,
    -673,  -673,   634,   288,   550,   563,   572,   574,   575,   576,
     588,   589,   594,   595,   596,   599,   606,   607,   610,   611,
     612,   613,   614,   615,   616,   617,   618,   619,   628,   629,
     659,   660,   662,   663,   666,   667,   677,   693,  -673,  -673,
    -673,  -673,  -673,   289,  -673,   728,   730,   579,   304,  -673,
    -673,  -673,  -673,   219,  -673,   451,   694,   314,   695,  -673,
    -673,  -673,  -673,   675,   502,  -673,  -673,  -673,  -673,  -673,
     366,   366,  -673,  -673,  -673,  -673,   734,   735,   767,   778,
     790,   857,   861,   877,   878,   880,   881,   882,   883,   884,
     885,   886,   887,   888,   889,   890,   891,   892,   893,   894,
     895,   896,   897,   898,   899,   900,   901,   902,   903,   904,
     905,  -673,   739,   320,  -673,   834,   771,  -673,   910,  -673,
     911,   912,   366,   346,   731,   219,   733,   738,   740,   741,
     742,   743,   744,   745,   746,   747,   748,   749,   750,   751,
     752,   753,   754,   755,   756,   757,   758,   759,   760,   761,
     762,   763,   764,   765,   766,   769,   770,   772,   773,   774,
     775,   335,  -673,   728,   777,  -673,   834,   732,   779,   780,
     219,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,   728,  -673,   916,  -673,   917,   351,   781,   782,  -673,
     945,   919,   786,   787,  -673,  -673,   834,  -673
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     189,     0,     0,     0,     0,     0,     0,     0,   125,     0,
       0,     0,     0,     0,     0,     0,   189,     0,   378,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   138,   137,
       0,     8,    14,    15,    16,    17,   376,   376,   376,   376,
     376,     0,   374,   374,   374,   374,   374,   182,     0,     0,
       0,     0,     0,     0,   119,   123,   120,   121,   122,   124,
     118,   189,     0,     0,   203,   204,   202,     0,     0,   205,
       0,   207,     0,   222,   223,   224,   226,   225,     0,   188,
     190,     0,     0,     0,     1,   189,     2,   172,   174,   175,
       0,   161,   143,   149,     0,     0,     0,     0,     0,     0,
       0,   116,     0,     0,     0,     0,     0,     0,     0,     0,
     167,     0,     0,     0,     0,     0,   117,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   212,
     213,   208,     0,   209,     0,   206,   243,     0,     0,     0,
     142,   141,     4,   173,     0,   139,   140,   160,     0,     0,
     157,     0,    30,     0,    31,   116,   379,     0,     0,   189,
     373,   130,   132,   131,   133,     0,   183,     0,   167,   127,
       0,   112,   372,     0,     0,   230,   232,   231,   228,   229,
     235,   237,   236,   233,   234,   240,   242,   241,   238,   239,
       0,     0,   215,   214,   220,   210,   211,     0,   191,   227,
       0,     0,   326,   330,   333,   334,     0,     0,     0,     0,
       0,     0,     0,     0,   331,   332,     0,     0,     0,     0,
       0,     0,     0,   328,     0,   189,   163,   244,   249,   250,
     264,   262,   263,   265,   266,   259,   254,   253,   252,   260,
     261,   251,   258,   257,   341,   343,     0,   342,   347,     0,
     348,     0,   340,     0,     0,   159,   375,   189,     0,     0,
       0,   110,     0,     0,   114,     0,     0,     0,   126,   166,
       0,     0,   221,   216,     0,   146,   145,     0,   356,   355,
     358,   357,   360,   359,   362,   361,   364,   363,   366,   365,
       0,     0,   292,   189,     0,     0,     0,     0,   335,   336,
     337,   338,     0,   339,     0,     0,     0,     0,     0,     0,
     294,   293,   353,   350,   345,     0,     0,     0,     0,   165,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   344,     0,   349,     0,   352,     0,   148,   150,   155,
     156,     0,   144,    33,     0,     0,     0,     0,    36,    38,
      39,   189,     0,    35,   115,     0,     0,   113,   134,   129,
     128,     0,     0,     0,   217,   192,     0,   287,     0,   189,
       0,     0,     0,     0,     0,   317,     0,     0,     0,     0,
       0,     0,     0,     0,   256,   255,   189,   162,   176,   178,
     187,   179,   245,     0,   167,   248,   310,   311,     0,     0,
     189,     0,   291,   301,   302,   305,   306,     0,   308,   300,
     303,   304,   296,   295,   297,   298,   299,   327,   329,   346,
     351,   354,     0,   153,   154,   152,   158,    42,    45,    46,
      43,    44,    47,    48,    62,    49,    51,    50,    65,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
       0,     0,     0,     0,   107,     0,     0,   384,     0,    34,
       0,     0,   111,     0,     0,     0,     0,   371,     0,   367,
       0,   218,     0,   288,     0,   322,     0,     0,   315,     0,
       0,     0,     0,     0,     0,   326,     0,   273,     0,   275,
       0,     0,     0,     0,   196,   197,   198,   199,   195,   200,
       0,   185,     0,   180,   279,   277,   278,   280,   281,   164,
     171,   189,   309,     0,     0,   290,     0,   151,     0,     0,
       0,     0,     0,     0,     0,     0,   103,   104,     0,   107,
     100,    40,     0,     0,     0,    32,    37,   393,     0,   246,
       0,   370,   369,   136,     0,   135,     0,   289,   323,     0,
     319,     0,   318,     0,     0,     0,     0,     0,     0,     0,
     187,   177,     0,     0,   184,     0,     0,   169,     0,     0,
     324,   313,   312,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   105,   102,
     106,   101,    41,     0,   109,     0,     0,     0,     0,   368,
     219,   321,   316,   320,   307,     0,     0,     0,     0,   274,
     276,   181,   193,     0,     0,   284,   282,   283,   285,   286,
       0,     0,   147,   325,   314,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,   387,     0,   385,   382,     0,   247,     0,   271,
       0,     0,     0,     0,   170,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   383,     0,     0,   391,   382,     0,     0,     0,
     194,   186,    63,    69,    70,    67,    68,    71,    72,    73,
      66,    93,    94,    91,    92,    95,    96,    97,    90,    77,
      78,    75,    76,    79,    80,    81,    74,    98,    99,    85,
      86,    83,    84,    87,    88,    89,    82,   388,   390,   389,
     386,     0,   392,     0,   272,     0,     0,     0,   268,   381,
       0,     0,     0,     0,   267,   269,   382,   270
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -673,  -673,  -673,   867,  -673,   913,  -673,   493,  -673,   469,
    -673,   426,   436,  -673,  -357,   915,   918,   807,  -673,  -673,
     920,  -673,   697,   921,   922,   -57,   950,   -15,   776,   828,
     -18,  -673,  -673,   545,  -673,  -673,  -673,  -673,  -673,  -673,
    -162,  -673,  -673,  -673,  -673,   476,   -90,    32,   410,  -673,
    -673,   846,  -673,  -673,   924,   925,   926,   927,   928,  -275,
    -673,   673,  -170,  -172,  -673,  -393,  -384,  -383,  -382,  -377,
    -673,  -673,  -673,  -673,  -673,  -673,   696,  -673,  -673,   602,
     452,  -221,  -673,  -673,   429,  -673,  -673,  -673,  -673,   783,
     624,   448,   -68,   287,   318,  -673,  -673,  -672,  -673,   204,
     254,  -673
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   357,   358,   359,   464,
     539,   540,   541,   360,   262,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   347,   348,   435,   255,   352,   148,   319,   404,
     171,   652,   577,    90,   397,   398,   399,   400,   513,    30,
      79,    80,   401,   510,    31,    32,    33,    34,    35,   226,
     367,   227,   228,   229,   802,   230,   231,   232,   233,   234,
     519,   235,   236,   237,   238,   239,   297,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     478,   479,   173,   103,    95,    86,   100,   745,   545,   693,
     694,   363
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     269,    83,   378,   314,   123,   470,   268,    47,   312,   313,
     514,   427,    87,    91,    88,   495,    89,   317,   257,   515,
     516,   517,   175,   170,   176,   177,   518,   180,   354,   181,
     182,   408,   487,   511,   292,   263,    48,    14,    50,   296,
     320,   563,   433,   434,   174,    77,    41,   486,   562,   310,
     311,   185,  -377,   186,   187,   316,   295,   534,   632,     1,
    -380,     2,     3,     4,     5,     6,     7,     8,     9,    47,
      94,   199,   146,   101,   792,    10,    49,    11,    12,    13,
      14,   110,   178,   349,   350,   341,   512,   183,    72,   364,
     342,   473,   365,   321,   322,   130,   369,   203,   204,   205,
      76,   482,   321,   322,   136,   535,   409,   536,   537,   543,
     538,   188,   321,   322,   548,    78,   321,   322,   292,   321,
     322,   429,   321,   322,   382,   383,   201,   318,   153,   154,
     155,    14,   321,   322,   807,   524,   162,   163,   164,   321,
     322,    16,    81,   267,   264,   258,    84,   355,   411,   356,
     467,   406,   407,   468,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   425,   426,   343,   202,
     203,   204,   205,   344,   321,   322,   570,    91,   179,   321,
     322,   436,   645,   184,   224,   623,   428,   396,   144,   260,
     223,   646,   647,   648,   210,   211,   212,   213,   649,   412,
     353,    85,    51,    52,   345,   325,    94,   189,    53,   346,
     315,   483,    15,   565,   318,   395,   490,   491,   340,   493,
     567,   214,   215,   216,  -381,  -381,    36,    37,    38,    87,
     102,    88,   376,    89,   108,    16,   109,   522,    39,    40,
     206,   207,   520,   385,   113,   386,   579,   387,   488,   208,
     489,   209,   387,  -381,  -381,   335,   336,   337,   338,   339,
     497,   114,   349,   498,    42,    43,    44,   210,   211,   212,
     213,    73,    74,    75,   224,   628,    45,    46,   381,   115,
     202,   203,   204,   205,   111,   112,   305,   703,   306,   307,
      62,    63,   137,    64,   214,   215,   216,   368,   190,   337,
     338,   339,   191,   192,   469,    65,    66,   193,   194,   475,
     476,   477,   202,   203,   204,   205,   217,   560,   129,   561,
     564,   584,   585,   586,   587,   588,   131,   132,   589,   590,
     104,   105,   106,   107,   218,   135,   219,   220,   787,   501,
     788,   789,   221,   222,   223,   138,   638,   224,   591,   225,
     377,   206,   207,   139,   582,    96,    97,    98,    99,   140,
     208,   141,   209,   499,   484,   295,   500,   321,   322,   202,
     203,   204,   205,   143,   218,   704,   219,   220,   210,   211,
     212,   213,   547,   206,   207,   365,   549,   553,    14,   318,
     554,   147,   208,   633,   209,   523,   503,  -201,   504,   505,
     506,   507,   149,   508,   509,   214,   215,   216,   133,   134,
     210,   211,   212,   213,   555,   373,   374,   554,    67,    68,
     608,   609,   151,    69,    70,    71,   152,   217,   202,   203,
     204,   205,   557,   581,   156,   318,   318,   214,   215,   216,
     206,   207,   157,   624,   160,   218,   365,   219,   220,   208,
     158,   209,   161,   221,   222,   223,   312,   313,   224,   217,
     225,   165,   627,   654,   166,   365,   318,   210,   211,   212,
     213,   167,   655,   691,   170,   656,   365,   218,   172,   219,
     220,   705,   642,   643,   195,   221,   222,   223,   697,   196,
     224,   318,   225,   197,   214,   215,   216,   253,   699,   290,
     291,   700,   254,   256,   742,   261,   578,   743,   208,   265,
     209,   266,   202,   203,   204,   205,   217,    54,    55,    56,
      57,    58,    59,   270,   271,    60,   210,   211,   212,   213,
     751,   272,   750,   365,   218,   799,   219,   220,   743,   273,
     275,   276,   221,   222,   223,   277,   293,   224,   294,   225,
     298,   299,   300,   214,   215,   216,   301,   304,     1,   302,
       2,     3,     4,     5,     6,     7,     8,     9,   308,   309,
     351,   361,   340,   362,    10,   217,    11,    12,    13,   366,
     371,   372,   379,   290,   375,    14,   388,   389,   390,   391,
     392,   393,   208,   218,   209,   219,   220,   380,   394,   403,
     405,   221,   222,   223,   410,   224,   224,   430,   225,   431,
     210,   211,   212,   213,   432,   465,     1,   466,     2,     3,
       4,     5,     6,     7,   471,     9,   472,   474,   481,   409,
      14,   485,    10,   492,    11,    12,    13,   214,   215,   216,
     494,   502,   321,   323,   525,   324,   521,   528,   529,   325,
     530,   551,   380,   531,   592,   593,   594,   595,   596,   217,
     532,   597,   598,   533,   552,   544,   326,   327,   328,   329,
     542,   550,   566,   558,   331,   556,   568,   218,   572,   219,
     220,   599,   569,   574,   573,   221,   222,   223,    14,   580,
     224,   575,   225,   576,   583,   325,   332,   333,   334,   335,
     336,   337,   338,   339,   325,   618,   619,   634,   380,   625,
     626,    15,   326,   327,   328,   329,   330,   630,   631,   635,
     331,   326,   327,   328,   329,   637,   526,   639,   640,   331,
     650,   692,   644,   695,    16,   696,   651,   657,   702,   653,
     706,   707,   332,   333,   334,   335,   336,   337,   338,   339,
     658,   332,   333,   334,   335,   336,   337,   338,   339,   659,
     325,   660,   661,   662,   600,   601,   602,   603,   604,    15,
     325,   605,   606,   708,   746,   663,   664,   326,   327,   328,
     329,   665,   666,   667,   709,   331,   668,  -381,  -381,   328,
     329,   607,    16,   669,   670,  -381,   710,   671,   672,   673,
     674,   675,   676,   677,   678,   679,   680,   332,   333,   334,
     335,   336,   337,   338,   339,   681,   682,  -381,   333,   334,
     335,   336,   337,   338,   339,   437,   438,   439,   440,   441,
     442,   443,   444,   445,   446,   447,   448,   449,   450,   451,
     452,   453,   454,   455,   456,   457,   683,   684,   458,   685,
     686,   459,   460,   687,   688,   461,   462,   463,   610,   611,
     612,   613,   614,   711,   689,   615,   616,   712,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     690,   698,   701,   713,   714,   617,   715,   716,   717,   718,
     719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
     729,   730,   731,   732,   733,   734,   735,   736,   737,   738,
     739,   740,   741,   744,   747,   748,   749,   752,   318,   793,
     797,   798,   753,   804,   754,   755,   756,   757,   758,   759,
     760,   761,   762,   763,   764,   765,   766,   767,   768,   769,
     770,   771,   772,   773,   774,   775,   776,   777,   778,   779,
     780,   803,   142,   781,   782,   559,   783,   784,   785,   786,
     791,   546,   259,   794,   370,   621,    82,   795,   800,   801,
     805,   806,   200,   274,   117,   622,   118,   527,   571,   119,
     641,   120,   121,   122,   198,   124,   125,   126,   127,   128,
     620,   402,   384,   496,   636,   796,   480,   790,     0,   303,
       0,     0,   629
};

static const yytype_int16 yycheck[] =
{
     170,    16,   277,   224,    61,   362,   168,     3,     5,     6,
     403,     3,    20,     8,    22,     3,    24,    77,    56,   403,
     403,   403,     3,    67,     5,     6,   403,     3,     3,     5,
       6,    76,    87,     3,   206,    34,     4,    79,     6,   209,
      56,    87,    71,    72,   112,    13,    33,    56,    84,   221,
     222,     3,     0,     5,     6,   225,    88,    74,    84,     7,
      63,     9,    10,    11,    12,    13,    14,    15,    16,     3,
      73,   139,    90,    41,   746,    23,    77,    25,    26,    27,
      79,    49,    63,   253,   254,   182,    56,    63,    30,   184,
     187,   366,   187,   148,   149,    63,   266,     4,     5,     6,
       3,   376,   148,   149,    72,   122,   151,   124,   125,   466,
     127,    63,   148,   149,   471,     3,   148,   149,   290,   148,
     149,   342,   148,   149,   294,   295,   144,   187,    96,    97,
      98,    79,   148,   149,   806,   410,   104,   105,   106,   148,
     149,   183,   161,   187,   159,   183,     0,   122,    74,   124,
     184,   321,   322,   187,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   182,     3,
       4,     5,     6,   187,   148,   149,   184,     8,   159,   148,
     149,   351,   575,   159,   181,   542,   178,   183,   183,   157,
     178,   575,   575,   575,   101,   102,   103,   104,   575,   125,
     257,   186,   157,   158,   182,   126,    73,   159,   163,   187,
     225,   184,   160,   187,   187,   184,   386,   387,   185,   389,
     187,   128,   129,   130,   145,   146,    29,    30,    31,    20,
      73,    22,    66,    24,   185,   183,    14,   409,    41,    42,
      74,    75,   404,    84,     3,    86,   521,    88,    84,    83,
      86,    85,    88,   174,   175,   176,   177,   178,   179,   180,
     184,     3,   432,   187,    29,    30,    31,   101,   102,   103,
     104,   152,   153,   154,   181,   550,    41,    42,   293,     3,
       3,     4,     5,     6,    77,    78,   116,   644,   118,   119,
      29,    30,    56,    32,   128,   129,   130,   265,    42,   178,
     179,   180,    46,    47,   361,    44,    45,    51,    52,    80,
      81,    82,     3,     4,     5,     6,   150,   487,     3,   489,
     492,    90,    91,    92,    93,    94,   165,   166,    97,    98,
      43,    44,    45,    46,   168,     3,   170,   171,     3,   396,
       5,     6,   176,   177,   178,   187,   567,   181,   117,   183,
     184,    74,    75,     6,   526,    37,    38,    39,    40,   184,
      83,   184,    85,   184,   379,    88,   187,   148,   149,     3,
       4,     5,     6,    21,   168,   650,   170,   171,   101,   102,
     103,   104,   184,    74,    75,   187,   184,   184,    79,   187,
     187,    66,    83,   563,    85,   410,    57,    58,    59,    60,
      61,    62,    68,    64,    65,   128,   129,   130,   165,   166,
     101,   102,   103,   104,   184,    48,    49,   187,   157,   158,
      97,    98,    74,   162,   163,   164,     3,   150,     3,     4,
       5,     6,   184,   184,     3,   187,   187,   128,   129,   130,
      74,    75,    63,   184,    75,   168,   187,   170,   171,    83,
     183,    85,     3,   176,   177,   178,     5,     6,   181,   150,
     183,     3,   184,   184,     3,   187,   187,   101,   102,   103,
     104,     3,   184,   184,    67,   187,   187,   168,     4,   170,
     171,   651,   572,   573,     3,   176,   177,   178,   184,     3,
     181,   187,   183,   183,   128,   129,   130,    54,   184,    74,
      75,   187,    69,    75,   184,     3,   521,   187,    83,    63,
      85,   173,     3,     4,     5,     6,   150,    35,    36,    37,
      38,    39,    40,    79,    79,    43,   101,   102,   103,   104,
     184,     3,   702,   187,   168,   184,   170,   171,   187,     6,
     184,   184,   176,   177,   178,   183,   183,   181,   183,   183,
       4,     4,     4,   128,   129,   130,     4,   183,     7,     6,
       9,    10,    11,    12,    13,    14,    15,    16,   183,   183,
      70,    56,   185,   183,    23,   150,    25,    26,    27,   183,
     183,   183,   183,    74,   184,    79,     4,   183,   183,   183,
       4,     4,    83,   168,    85,   170,   171,    74,   184,   167,
       3,   176,   177,   178,   183,   181,   181,     6,   183,     5,
     101,   102,   103,   104,   187,   123,     7,   183,     9,    10,
      11,    12,    13,    14,   183,    16,     3,   187,     6,   151,
      79,   184,    23,    77,    25,    26,    27,   128,   129,   130,
       4,   187,   148,    74,   125,    76,   183,   183,   183,   126,
     183,     4,    74,   183,    90,    91,    92,    93,    94,   150,
     183,    97,    98,   183,     3,    28,   143,   144,   145,   146,
     183,   183,   187,   184,   151,    50,     4,   168,    58,   170,
     171,   117,     4,     3,    58,   176,   177,   178,    79,   184,
     181,   187,   183,    53,     6,   126,   173,   174,   175,   176,
     177,   178,   179,   180,   126,   125,   123,   184,    74,   183,
     156,   160,   143,   144,   145,   146,   147,     6,   184,   181,
     151,   143,   144,   145,   146,     4,   148,   184,   184,   151,
      54,     3,   183,     3,   183,   156,    55,   187,    63,   184,
       6,     6,   173,   174,   175,   176,   177,   178,   179,   180,
     187,   173,   174,   175,   176,   177,   178,   179,   180,   187,
     126,   187,   187,   187,    90,    91,    92,    93,    94,   160,
     126,    97,    98,     6,     3,   187,   187,   143,   144,   145,
     146,   187,   187,   187,     6,   151,   187,   143,   144,   145,
     146,   117,   183,   187,   187,   151,     6,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   173,   174,   175,
     176,   177,   178,   179,   180,   187,   187,   173,   174,   175,
     176,   177,   178,   179,   180,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   187,   187,   112,   187,
     187,   115,   116,   187,   187,   119,   120,   121,    90,    91,
      92,    93,    94,     6,   187,    97,    98,     6,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     187,   187,   187,     6,     6,   117,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   173,    79,     4,     4,     4,   184,   187,   187,
       4,     4,   184,     4,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,     6,    85,   184,   184,   486,   184,   184,   184,   184,
     183,   468,   155,   184,   267,   539,    16,   187,   187,   187,
     184,   184,   144,   197,    61,   539,    61,   432,   502,    61,
     570,    61,    61,    61,   138,    61,    61,    61,    61,    61,
     538,   318,   296,   391,   565,   791,   372,   743,    -1,   216,
      -1,    -1,   554
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   160,   183,   189,   190,   191,
     193,   203,   204,   206,   208,   211,   212,   213,   214,   215,
     237,   242,   243,   244,   245,   246,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   235,    77,
     235,   157,   158,   163,    35,    36,    37,    38,    39,    40,
      43,   207,    29,    30,    32,    44,    45,   157,   158,   162,
     163,   164,    30,   152,   153,   154,     3,   235,     3,   238,
     239,   161,   214,   215,     0,   186,   283,    20,    22,    24,
     231,     8,   216,   218,    73,   282,   282,   282,   282,   282,
     284,   235,    73,   281,   281,   281,   281,   281,   185,    14,
     235,    77,    78,     3,     3,     3,   192,   193,   203,   204,
     208,   211,   212,   213,   242,   243,   244,   245,   246,     3,
     235,   165,   166,   165,   166,     3,   235,    56,   187,     6,
     184,   184,   191,    21,   183,   217,   218,    66,   225,    68,
     219,    74,     3,   235,   235,   235,     3,    63,   183,   205,
      75,     3,   235,   235,   235,     3,     3,     3,   209,   210,
      67,   228,     4,   280,   280,     3,     5,     6,    63,   159,
       3,     5,     6,    63,   159,     3,     5,     6,    63,   159,
      42,    46,    47,    51,    52,     3,     3,   183,   239,   280,
     217,   218,     3,     4,     5,     6,    74,    75,    83,    85,
     101,   102,   103,   104,   128,   129,   130,   150,   168,   170,
     171,   176,   177,   178,   181,   183,   247,   249,   250,   251,
     253,   254,   255,   256,   257,   259,   260,   261,   262,   263,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,    54,    69,   223,    75,    56,   183,   205,
     235,     3,   202,    34,   215,    63,   173,   187,   228,   250,
      79,    79,     3,     6,   216,   184,   184,   183,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
      74,    75,   251,   183,   183,    88,   250,   264,     4,     4,
       4,     4,     6,   277,   183,   116,   118,   119,   183,   183,
     251,   251,     5,     6,   269,   215,   250,    77,   187,   226,
      56,   148,   149,    74,    76,   126,   143,   144,   145,   146,
     147,   151,   173,   174,   175,   176,   177,   178,   179,   180,
     185,   182,   187,   182,   187,   182,   187,   220,   221,   250,
     250,    70,   224,   213,     3,   122,   124,   194,   195,   196,
     201,    56,   183,   289,   184,   187,   183,   248,   235,   250,
     210,   183,   183,    48,    49,   184,    66,   184,   247,   183,
      74,   215,   250,   250,   264,    84,    86,    88,     4,   183,
     183,   183,     4,     4,   184,   184,   183,   232,   233,   234,
     235,   240,   249,   167,   227,     3,   250,   250,    76,   151,
     183,    74,   125,   251,   251,   251,   251,   251,   251,   251,
     251,   251,   251,   251,   251,   251,   251,     3,   178,   269,
       6,     5,   187,    71,    72,   222,   250,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   112,   115,
     116,   119,   120,   121,   197,   123,   183,   184,   187,   213,
     202,   183,     3,   247,   187,    80,    81,    82,   278,   279,
     278,     6,   247,   184,   215,   184,    56,    87,    84,    86,
     250,   250,    77,   250,     4,     3,   267,   184,   187,   184,
     187,   213,   187,    57,    59,    60,    61,    62,    64,    65,
     241,     3,    56,   236,   253,   254,   255,   256,   257,   258,
     228,   183,   251,   215,   247,   125,   148,   221,   183,   183,
     183,   183,   183,   183,    74,   122,   124,   125,   127,   198,
     199,   200,   183,   202,    28,   286,   195,   184,   202,   184,
     183,     4,     3,   184,   187,   184,    50,   184,   184,   197,
     250,   250,    84,    87,   251,   187,   187,   187,     4,     4,
     184,   233,    58,    58,     3,   187,    53,   230,   215,   247,
     184,   184,   251,     6,    90,    91,    92,    93,    94,    97,
      98,   117,    90,    91,    92,    93,    94,    97,    98,   117,
      90,    91,    92,    93,    94,    97,    98,   117,    97,    98,
      90,    91,    92,    93,    94,    97,    98,   117,   125,   123,
     268,   199,   200,   202,   184,   183,   156,   184,   247,   279,
       6,   184,    84,   250,   184,   181,   272,     4,   269,   184,
     184,   236,   234,   234,   183,   253,   254,   255,   256,   257,
      54,    55,   229,   184,   184,   184,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   184,     3,   287,   288,     3,   156,   184,   187,   184,
     187,   187,    63,   202,   247,   250,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   173,   184,   187,    79,   285,     3,     4,     4,     4,
     250,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,     3,     5,     6,
     288,   183,   285,   187,   184,   187,   287,     4,     4,   184,
     187,   187,   252,     6,     4,   184,   184,   285
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   188,   189,   190,   190,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     193,   193,   193,   193,   193,   193,   194,   194,   195,   195,
     196,   196,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     198,   198,   199,   199,   199,   199,   200,   200,   201,   201,
     202,   202,   203,   204,   204,   205,   205,   206,   207,   207,
     207,   207,   207,   207,   207,   207,   208,   209,   209,   210,
     211,   211,   211,   211,   211,   212,   212,   213,   213,   213,
     213,   214,   214,   215,   216,   217,   217,   218,   219,   219,
     220,   220,   221,   222,   222,   222,   223,   223,   224,   224,
     225,   225,   226,   226,   227,   227,   228,   228,   229,   229,
     230,   230,   231,   231,   231,   231,   232,   232,   233,   233,
     234,   234,   235,   235,   236,   236,   236,   236,   237,   237,
     238,   238,   239,   240,   240,   241,   241,   241,   241,   241,
     241,   241,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   242,   243,   243,   243,   244,   245,   245,   245,   245,
     245,   245,   245,   245,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   246,   247,   247,   248,   248,   249,   249,
     250,   250,   250,   250,   250,   251,   251,   251,   251,   251,
     251,   251,   251,   251,   251,   251,   251,   252,   252,   253,
     254,   255,   255,   256,   256,   257,   257,   258,   258,   258,
     258,   258,   258,   258,   258,   258,   258,   259,   259,   259,
     259,   259,   259,   259,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   259,   259,   259,   259,   259,   259,
     260,   260,   261,   262,   262,   263,   263,   263,   263,   264,
     264,   265,   266,   266,   266,   266,   267,   267,   267,   267,
     268,   268,   268,   268,   268,   268,   268,   268,   268,   268,
     268,   268,   269,   269,   270,   271,   271,   272,   272,   273,
     274,   274,   275,   276,   276,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   278,   278,   279,
     279,   279,   280,   281,   281,   282,   282,   283,   283,   284,
     284,   285,   285,   286,   286,   287,   287,   288,   288,   288,
     288,   289,   289,   289
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
       1,     1,     1,     1,     1,     1,     1,     2,     0,    12,
      14,     7,     9,     4,     6,     4,     6,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
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
#line 2111 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2119 "parser.cpp"
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
#line 2133 "parser.cpp"
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
#line 2147 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2158 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2166 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2175 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2184 "parser.cpp"
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
#line 2198 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2209 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2219 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2229 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2239 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2249 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2259 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2269 "parser.cpp"
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
#line 2283 "parser.cpp"
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
#line 2297 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2307 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2315 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2323 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2332 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2340 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2348 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2356 "parser.cpp"
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
#line 2370 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2379 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2388 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2397 "parser.cpp"
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
#line 2410 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2419 "parser.cpp"
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
#line 2433 "parser.cpp"
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
#line 2447 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 340 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2457 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2466 "parser.cpp"
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
#line 2480 "parser.cpp"
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
#line 2497 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2505 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2513 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2521 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2529 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2537 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2569 "parser.cpp"
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
#line 2583 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2599 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2607 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2615 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2623 "parser.cpp"
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
#line 2636 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2644 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2660 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2668 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2684 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2692 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2700 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2708 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2716 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2724 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2732 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2748 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2759 "parser.cpp"
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
#line 2773 "parser.cpp"
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
#line 2787 "parser.cpp"
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
#line 2801 "parser.cpp"
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

#line 2909 "parser.cpp"

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
#line 477 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3124 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 481 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3135 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 487 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3146 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 494 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3152 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3158 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 496 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3164 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 497 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3170 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3176 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3182 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3188 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 501 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3194 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 502 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3200 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 503 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3206 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 504 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3212 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 505 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3218 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 506 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3224 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 508 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3230 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 509 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3236 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3242 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3248 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 512 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3254 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3260 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3266 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3272 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 516 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3278 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 517 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3284 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 518 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3290 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3296 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 526 "parser.y"
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
#line 3316 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 543 "parser.y"
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
#line 3334 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 559 "parser.y"
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
#line 3367 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 588 "parser.y"
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
#line 3387 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 604 "parser.y"
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
#line 3408 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 622 "parser.y"
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
#line 3441 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 651 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3450 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 655 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3459 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 661 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3467 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 664 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3475 "parser.cpp"
    break;

  case 40: /* table_column: IDENTIFIER column_type default_expr  */
#line 670 "parser.y"
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
        case infinity::LogicalType::kTensorArray:
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kSparse: {
            type_info_ptr = infinity::SparseInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
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
#line 3523 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 713 "parser.y"
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
#line 3562 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 749 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3568 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 750 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3574 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 751 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3580 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 752 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3586 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 753 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3592 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 754 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3598 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 755 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3604 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 756 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3610 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 757 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3616 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 758 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3622 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 759 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3628 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 760 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3634 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 761 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3640 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 762 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3646 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 763 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3652 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 764 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3658 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 765 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3664 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 766 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3670 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 767 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3676 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 770 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3682 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 772 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3688 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 773 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3694 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 774 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3700 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3706 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 778 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3712 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 779 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3718 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 780 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3724 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 781 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3730 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 782 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3736 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 783 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3742 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 784 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3748 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 785 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3754 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 786 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3760 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 787 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3766 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 788 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3772 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 789 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3778 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 790 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3784 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 791 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3790 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 792 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3796 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3802 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 794 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3808 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 795 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3814 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 796 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3820 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 797 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3826 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 798 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3832 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 799 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3838 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 800 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3844 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3850 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3856 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3862 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3868 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3874 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3880 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3886 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3892 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3898 "parser.cpp"
    break;

  case 98: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3904 "parser.cpp"
    break;

  case 99: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3910 "parser.cpp"
    break;

  case 100: /* column_constraints: column_constraint  */
#line 830 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3919 "parser.cpp"
    break;

  case 101: /* column_constraints: column_constraints column_constraint  */
#line 834 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3933 "parser.cpp"
    break;

  case 102: /* column_constraint: PRIMARY KEY  */
#line 844 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3941 "parser.cpp"
    break;

  case 103: /* column_constraint: UNIQUE  */
#line 847 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3949 "parser.cpp"
    break;

  case 104: /* column_constraint: NULLABLE  */
#line 850 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3957 "parser.cpp"
    break;

  case 105: /* column_constraint: NOT NULLABLE  */
#line 853 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3965 "parser.cpp"
    break;

  case 106: /* default_expr: DEFAULT constant_expr  */
#line 857 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 3973 "parser.cpp"
    break;

  case 107: /* default_expr: %empty  */
#line 860 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 3981 "parser.cpp"
    break;

  case 108: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 865 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3991 "parser.cpp"
    break;

  case 109: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 870 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4001 "parser.cpp"
    break;

  case 110: /* identifier_array: IDENTIFIER  */
#line 877 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4012 "parser.cpp"
    break;

  case 111: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 883 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4023 "parser.cpp"
    break;

  case 112: /* delete_statement: DELETE FROM table_name where_clause  */
#line 893 "parser.y"
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
#line 4040 "parser.cpp"
    break;

  case 113: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 909 "parser.y"
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
#line 4079 "parser.cpp"
    break;

  case 114: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 943 "parser.y"
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
#line 4096 "parser.cpp"
    break;

  case 115: /* optional_identifier_array: '(' identifier_array ')'  */
#line 956 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4104 "parser.cpp"
    break;

  case 116: /* optional_identifier_array: %empty  */
#line 959 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4112 "parser.cpp"
    break;

  case 117: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 966 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4122 "parser.cpp"
    break;

  case 118: /* explain_type: ANALYZE  */
#line 972 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4130 "parser.cpp"
    break;

  case 119: /* explain_type: AST  */
#line 975 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4138 "parser.cpp"
    break;

  case 120: /* explain_type: RAW  */
#line 978 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4146 "parser.cpp"
    break;

  case 121: /* explain_type: LOGICAL  */
#line 981 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4154 "parser.cpp"
    break;

  case 122: /* explain_type: PHYSICAL  */
#line 984 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4162 "parser.cpp"
    break;

  case 123: /* explain_type: PIPELINE  */
#line 987 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4170 "parser.cpp"
    break;

  case 124: /* explain_type: FRAGMENT  */
#line 990 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4178 "parser.cpp"
    break;

  case 125: /* explain_type: %empty  */
#line 993 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4186 "parser.cpp"
    break;

  case 126: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1000 "parser.y"
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
#line 4203 "parser.cpp"
    break;

  case 127: /* update_expr_array: update_expr  */
#line 1013 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4212 "parser.cpp"
    break;

  case 128: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1017 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4221 "parser.cpp"
    break;

  case 129: /* update_expr: IDENTIFIER '=' expr  */
#line 1022 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4233 "parser.cpp"
    break;

  case 130: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1035 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4249 "parser.cpp"
    break;

  case 131: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1048 "parser.y"
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
#line 4267 "parser.cpp"
    break;

  case 132: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1063 "parser.y"
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
#line 4285 "parser.cpp"
    break;

  case 133: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1078 "parser.y"
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
#line 4303 "parser.cpp"
    break;

  case 134: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1093 "parser.y"
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
#line 4326 "parser.cpp"
    break;

  case 135: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1116 "parser.y"
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
#line 4372 "parser.cpp"
    break;

  case 136: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1157 "parser.y"
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
#line 4418 "parser.cpp"
    break;

  case 137: /* select_statement: select_without_paren  */
#line 1202 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4426 "parser.cpp"
    break;

  case 138: /* select_statement: select_with_paren  */
#line 1205 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4434 "parser.cpp"
    break;

  case 139: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1208 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4448 "parser.cpp"
    break;

  case 140: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1217 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4462 "parser.cpp"
    break;

  case 141: /* select_with_paren: '(' select_without_paren ')'  */
#line 1227 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4470 "parser.cpp"
    break;

  case 142: /* select_with_paren: '(' select_with_paren ')'  */
#line 1230 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4478 "parser.cpp"
    break;

  case 143: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1234 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4487 "parser.cpp"
    break;

  case 144: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1239 "parser.y"
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
#line 4513 "parser.cpp"
    break;

  case 145: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1261 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4521 "parser.cpp"
    break;

  case 146: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1264 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4529 "parser.cpp"
    break;

  case 147: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1269 "parser.y"
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
#line 4549 "parser.cpp"
    break;

  case 148: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1285 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4557 "parser.cpp"
    break;

  case 149: /* order_by_clause: %empty  */
#line 1288 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4565 "parser.cpp"
    break;

  case 150: /* order_by_expr_list: order_by_expr  */
#line 1292 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4574 "parser.cpp"
    break;

  case 151: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1296 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4583 "parser.cpp"
    break;

  case 152: /* order_by_expr: expr order_by_type  */
#line 1301 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4593 "parser.cpp"
    break;

  case 153: /* order_by_type: ASC  */
#line 1307 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4601 "parser.cpp"
    break;

  case 154: /* order_by_type: DESC  */
#line 1310 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4609 "parser.cpp"
    break;

  case 155: /* order_by_type: %empty  */
#line 1313 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4617 "parser.cpp"
    break;

  case 156: /* limit_expr: LIMIT expr  */
#line 1317 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4625 "parser.cpp"
    break;

  case 157: /* limit_expr: %empty  */
#line 1321 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4631 "parser.cpp"
    break;

  case 158: /* offset_expr: OFFSET expr  */
#line 1323 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4639 "parser.cpp"
    break;

  case 159: /* offset_expr: %empty  */
#line 1327 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4645 "parser.cpp"
    break;

  case 160: /* distinct: DISTINCT  */
#line 1329 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4653 "parser.cpp"
    break;

  case 161: /* distinct: %empty  */
#line 1332 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4661 "parser.cpp"
    break;

  case 162: /* from_clause: FROM table_reference  */
#line 1336 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4669 "parser.cpp"
    break;

  case 163: /* from_clause: %empty  */
#line 1339 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4677 "parser.cpp"
    break;

  case 164: /* search_clause: SEARCH sub_search_array  */
#line 1343 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4687 "parser.cpp"
    break;

  case 165: /* search_clause: %empty  */
#line 1348 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4695 "parser.cpp"
    break;

  case 166: /* where_clause: WHERE expr  */
#line 1352 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4703 "parser.cpp"
    break;

  case 167: /* where_clause: %empty  */
#line 1355 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4711 "parser.cpp"
    break;

  case 168: /* having_clause: HAVING expr  */
#line 1359 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4719 "parser.cpp"
    break;

  case 169: /* having_clause: %empty  */
#line 1362 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4727 "parser.cpp"
    break;

  case 170: /* group_by_clause: GROUP BY expr_array  */
#line 1366 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4735 "parser.cpp"
    break;

  case 171: /* group_by_clause: %empty  */
#line 1369 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4743 "parser.cpp"
    break;

  case 172: /* set_operator: UNION  */
#line 1373 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4751 "parser.cpp"
    break;

  case 173: /* set_operator: UNION ALL  */
#line 1376 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4759 "parser.cpp"
    break;

  case 174: /* set_operator: INTERSECT  */
#line 1379 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4767 "parser.cpp"
    break;

  case 175: /* set_operator: EXCEPT  */
#line 1382 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4775 "parser.cpp"
    break;

  case 176: /* table_reference: table_reference_unit  */
#line 1390 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4783 "parser.cpp"
    break;

  case 177: /* table_reference: table_reference ',' table_reference_unit  */
#line 1393 "parser.y"
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
#line 4801 "parser.cpp"
    break;

  case 180: /* table_reference_name: table_name table_alias  */
#line 1410 "parser.y"
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
#line 4819 "parser.cpp"
    break;

  case 181: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1424 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4830 "parser.cpp"
    break;

  case 182: /* table_name: IDENTIFIER  */
#line 1433 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4840 "parser.cpp"
    break;

  case 183: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1438 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4852 "parser.cpp"
    break;

  case 184: /* table_alias: AS IDENTIFIER  */
#line 1447 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4862 "parser.cpp"
    break;

  case 185: /* table_alias: IDENTIFIER  */
#line 1452 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4872 "parser.cpp"
    break;

  case 186: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1457 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4883 "parser.cpp"
    break;

  case 187: /* table_alias: %empty  */
#line 1463 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4891 "parser.cpp"
    break;

  case 188: /* with_clause: WITH with_expr_list  */
#line 1470 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4899 "parser.cpp"
    break;

  case 189: /* with_clause: %empty  */
#line 1473 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4907 "parser.cpp"
    break;

  case 190: /* with_expr_list: with_expr  */
#line 1477 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4916 "parser.cpp"
    break;

  case 191: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1480 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4925 "parser.cpp"
    break;

  case 192: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1485 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4937 "parser.cpp"
    break;

  case 193: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1497 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4949 "parser.cpp"
    break;

  case 194: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1504 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4962 "parser.cpp"
    break;

  case 195: /* join_type: INNER  */
#line 1518 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4970 "parser.cpp"
    break;

  case 196: /* join_type: LEFT  */
#line 1521 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4978 "parser.cpp"
    break;

  case 197: /* join_type: RIGHT  */
#line 1524 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4986 "parser.cpp"
    break;

  case 198: /* join_type: OUTER  */
#line 1527 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4994 "parser.cpp"
    break;

  case 199: /* join_type: FULL  */
#line 1530 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5002 "parser.cpp"
    break;

  case 200: /* join_type: CROSS  */
#line 1533 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5010 "parser.cpp"
    break;

  case 201: /* join_type: %empty  */
#line 1536 "parser.y"
                {
}
#line 5017 "parser.cpp"
    break;

  case 202: /* show_statement: SHOW DATABASES  */
#line 1542 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5026 "parser.cpp"
    break;

  case 203: /* show_statement: SHOW TABLES  */
#line 1546 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5035 "parser.cpp"
    break;

  case 204: /* show_statement: SHOW VIEWS  */
#line 1550 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5044 "parser.cpp"
    break;

  case 205: /* show_statement: SHOW CONFIGS  */
#line 1554 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5053 "parser.cpp"
    break;

  case 206: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1558 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5065 "parser.cpp"
    break;

  case 207: /* show_statement: SHOW PROFILES  */
#line 1565 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5074 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW SESSION VARIABLES  */
#line 1569 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5083 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1573 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5092 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1577 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5103 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1583 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5114 "parser.cpp"
    break;

  case 212: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1589 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5125 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW TABLE table_name  */
#line 1595 "parser.y"
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
#line 5141 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1606 "parser.y"
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
#line 5157 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1617 "parser.y"
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
#line 5173 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1628 "parser.y"
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
#line 5190 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1640 "parser.y"
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
#line 5207 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1652 "parser.y"
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
#line 5225 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1665 "parser.y"
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
#line 5244 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1679 "parser.y"
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
#line 5260 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1690 "parser.y"
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
#line 5279 "parser.cpp"
    break;

  case 222: /* flush_statement: FLUSH DATA  */
#line 1708 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5288 "parser.cpp"
    break;

  case 223: /* flush_statement: FLUSH LOG  */
#line 1712 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5297 "parser.cpp"
    break;

  case 224: /* flush_statement: FLUSH BUFFER  */
#line 1716 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5306 "parser.cpp"
    break;

  case 225: /* optimize_statement: OPTIMIZE table_name  */
#line 1724 "parser.y"
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
#line 5321 "parser.cpp"
    break;

  case 226: /* command_statement: USE IDENTIFIER  */
#line 1738 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5332 "parser.cpp"
    break;

  case 227: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1744 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5342 "parser.cpp"
    break;

  case 228: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1749 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5353 "parser.cpp"
    break;

  case 229: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1755 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5364 "parser.cpp"
    break;

  case 230: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1761 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5377 "parser.cpp"
    break;

  case 231: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1769 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5388 "parser.cpp"
    break;

  case 232: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1775 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5399 "parser.cpp"
    break;

  case 233: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1781 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5410 "parser.cpp"
    break;

  case 234: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1787 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5421 "parser.cpp"
    break;

  case 235: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1793 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5434 "parser.cpp"
    break;

  case 236: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1801 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5445 "parser.cpp"
    break;

  case 237: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1807 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5456 "parser.cpp"
    break;

  case 238: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1813 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5467 "parser.cpp"
    break;

  case 239: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1819 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5478 "parser.cpp"
    break;

  case 240: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1825 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5491 "parser.cpp"
    break;

  case 241: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1833 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5502 "parser.cpp"
    break;

  case 242: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1839 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5513 "parser.cpp"
    break;

  case 243: /* compact_statement: COMPACT TABLE table_name  */
#line 1846 "parser.y"
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
#line 5530 "parser.cpp"
    break;

  case 244: /* expr_array: expr_alias  */
#line 1863 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5539 "parser.cpp"
    break;

  case 245: /* expr_array: expr_array ',' expr_alias  */
#line 1867 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5548 "parser.cpp"
    break;

  case 246: /* expr_array_list: '(' expr_array ')'  */
#line 1872 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5557 "parser.cpp"
    break;

  case 247: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1876 "parser.y"
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
#line 5577 "parser.cpp"
    break;

  case 248: /* expr_alias: expr AS IDENTIFIER  */
#line 1903 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5588 "parser.cpp"
    break;

  case 249: /* expr_alias: expr  */
#line 1909 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5596 "parser.cpp"
    break;

  case 255: /* operand: '(' expr ')'  */
#line 1919 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5604 "parser.cpp"
    break;

  case 256: /* operand: '(' select_without_paren ')'  */
#line 1922 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5615 "parser.cpp"
    break;

  case 257: /* operand: constant_expr  */
#line 1928 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5623 "parser.cpp"
    break;

  case 267: /* extra_match_tensor_option: ',' STRING  */
#line 1941 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5631 "parser.cpp"
    break;

  case 268: /* extra_match_tensor_option: %empty  */
#line 1944 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5639 "parser.cpp"
    break;

  case 269: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 1950 "parser.y"
                                                                                                                           {
    infinity::MatchTensorExpr* match_tensor_expr = new infinity::MatchTensorExpr();
    (yyval.expr_t) = match_tensor_expr;
    // search column
    match_tensor_expr->column_expr_.reset((yyvsp[-8].expr_t));
    // search tensor
    match_tensor_expr->tensor_expr_.reset((yyvsp[-6].const_expr_t));
    // tensor data type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_tensor_expr->embedding_data_type_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    // search method
    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_tensor_expr->search_method_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    // search options
    if ((yyvsp[-1].str_value)) {
        match_tensor_expr->options_text_ = (yyvsp[-1].str_value);
        free((yyvsp[-1].str_value));
    }
}
#line 5665 "parser.cpp"
    break;

  case 270: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1972 "parser.y"
                                                                                                                        {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // KNN search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    if(strcmp((yyvsp[-4].str_value), "l2") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-4].str_value), "ip") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-4].str_value), "cosine") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-4].str_value), "hamming") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
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
    if(strcmp((yyvsp[-6].str_value), "float") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-8].const_expr_t)->double_array_.empty())) {
            match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
            match_vector_expr->embedding_data_ptr_ = new float[match_vector_expr->dimension_];
            for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
                ((float*)(match_vector_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-8].const_expr_t)->long_array_.empty())) {
            match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
            match_vector_expr->embedding_data_ptr_ = new float[match_vector_expr->dimension_];
            for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
                ((float*)(match_vector_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "tinyint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        match_vector_expr->embedding_data_ptr_ = new char[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((char*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "smallint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        match_vector_expr->embedding_data_ptr_ = new short int[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((short int*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "integer") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        match_vector_expr->embedding_data_ptr_ = new int[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((int*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bigint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        match_vector_expr->embedding_data_ptr_ = new long[match_vector_expr->dimension_];

        memcpy(match_vector_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->long_array_.data(), match_vector_expr->dimension_ * sizeof(long));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bit") == 0 and match_vector_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        if(match_vector_expr->dimension_ % 8 == 0) {
            match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = match_vector_expr->dimension_ / 8;
            char *char_ptr = new char[embedding_size];
            uint8_t *data_ptr = reinterpret_cast<uint8_t *>(char_ptr);
            match_vector_expr->embedding_data_ptr_ = char_ptr;
            for(long i = 0; i < embedding_size; ++ i) {
                uint8_t embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        embedding_unit |= (uint8_t(1) << bit_idx);
                    } else if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        // no-op
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
                data_ptr[i] = embedding_unit;
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

    } else if(strcmp((yyvsp[-6].str_value), "double") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        match_vector_expr->embedding_data_ptr_ = new double[match_vector_expr->dimension_];

        memcpy(match_vector_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->double_array_.data(), match_vector_expr->dimension_ * sizeof(double));
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
    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
}
#line 5835 "parser.cpp"
    break;

  case 271: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2138 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5848 "parser.cpp"
    break;

  case 272: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2146 "parser.y"
                                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-5].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5863 "parser.cpp"
    break;

  case 273: /* query_expr: QUERY '(' STRING ')'  */
#line 2157 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5874 "parser.cpp"
    break;

  case 274: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2163 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5887 "parser.cpp"
    break;

  case 275: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2172 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5898 "parser.cpp"
    break;

  case 276: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2178 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5911 "parser.cpp"
    break;

  case 277: /* sub_search_array: match_vector_expr  */
#line 2188 "parser.y"
                                     {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5920 "parser.cpp"
    break;

  case 278: /* sub_search_array: match_text_expr  */
#line 2192 "parser.y"
                  {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5929 "parser.cpp"
    break;

  case 279: /* sub_search_array: match_tensor_expr  */
#line 2196 "parser.y"
                    {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5938 "parser.cpp"
    break;

  case 280: /* sub_search_array: query_expr  */
#line 2200 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5947 "parser.cpp"
    break;

  case 281: /* sub_search_array: fusion_expr  */
#line 2204 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5956 "parser.cpp"
    break;

  case 282: /* sub_search_array: sub_search_array ',' match_vector_expr  */
#line 2208 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5965 "parser.cpp"
    break;

  case 283: /* sub_search_array: sub_search_array ',' match_text_expr  */
#line 2212 "parser.y"
                                       {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5974 "parser.cpp"
    break;

  case 284: /* sub_search_array: sub_search_array ',' match_tensor_expr  */
#line 2216 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5983 "parser.cpp"
    break;

  case 285: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2220 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5992 "parser.cpp"
    break;

  case 286: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2224 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6001 "parser.cpp"
    break;

  case 287: /* function_expr: IDENTIFIER '(' ')'  */
#line 2229 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6014 "parser.cpp"
    break;

  case 288: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2237 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6027 "parser.cpp"
    break;

  case 289: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2245 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6041 "parser.cpp"
    break;

  case 290: /* function_expr: operand IS NOT NULLABLE  */
#line 2254 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6053 "parser.cpp"
    break;

  case 291: /* function_expr: operand IS NULLABLE  */
#line 2261 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6065 "parser.cpp"
    break;

  case 292: /* function_expr: NOT operand  */
#line 2268 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6077 "parser.cpp"
    break;

  case 293: /* function_expr: '-' operand  */
#line 2275 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6089 "parser.cpp"
    break;

  case 294: /* function_expr: '+' operand  */
#line 2282 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6101 "parser.cpp"
    break;

  case 295: /* function_expr: operand '-' operand  */
#line 2289 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6114 "parser.cpp"
    break;

  case 296: /* function_expr: operand '+' operand  */
#line 2297 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6127 "parser.cpp"
    break;

  case 297: /* function_expr: operand '*' operand  */
#line 2305 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6140 "parser.cpp"
    break;

  case 298: /* function_expr: operand '/' operand  */
#line 2313 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6153 "parser.cpp"
    break;

  case 299: /* function_expr: operand '%' operand  */
#line 2321 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6166 "parser.cpp"
    break;

  case 300: /* function_expr: operand '=' operand  */
#line 2329 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6179 "parser.cpp"
    break;

  case 301: /* function_expr: operand EQUAL operand  */
#line 2337 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6192 "parser.cpp"
    break;

  case 302: /* function_expr: operand NOT_EQ operand  */
#line 2345 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6205 "parser.cpp"
    break;

  case 303: /* function_expr: operand '<' operand  */
#line 2353 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6218 "parser.cpp"
    break;

  case 304: /* function_expr: operand '>' operand  */
#line 2361 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6231 "parser.cpp"
    break;

  case 305: /* function_expr: operand LESS_EQ operand  */
#line 2369 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6244 "parser.cpp"
    break;

  case 306: /* function_expr: operand GREATER_EQ operand  */
#line 2377 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6257 "parser.cpp"
    break;

  case 307: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2385 "parser.y"
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
#line 6292 "parser.cpp"
    break;

  case 308: /* function_expr: operand LIKE operand  */
#line 2415 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6305 "parser.cpp"
    break;

  case 309: /* function_expr: operand NOT LIKE operand  */
#line 2423 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6318 "parser.cpp"
    break;

  case 310: /* conjunction_expr: expr AND expr  */
#line 2432 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6331 "parser.cpp"
    break;

  case 311: /* conjunction_expr: expr OR expr  */
#line 2440 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6344 "parser.cpp"
    break;

  case 312: /* between_expr: operand BETWEEN operand AND operand  */
#line 2449 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6356 "parser.cpp"
    break;

  case 313: /* in_expr: operand IN '(' expr_array ')'  */
#line 2457 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6367 "parser.cpp"
    break;

  case 314: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2463 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6378 "parser.cpp"
    break;

  case 315: /* case_expr: CASE expr case_check_array END  */
#line 2470 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6389 "parser.cpp"
    break;

  case 316: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2476 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6401 "parser.cpp"
    break;

  case 317: /* case_expr: CASE case_check_array END  */
#line 2483 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6411 "parser.cpp"
    break;

  case 318: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2488 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6422 "parser.cpp"
    break;

  case 319: /* case_check_array: WHEN expr THEN expr  */
#line 2495 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6434 "parser.cpp"
    break;

  case 320: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2502 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6446 "parser.cpp"
    break;

  case 321: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2510 "parser.y"
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
#line 6474 "parser.cpp"
    break;

  case 322: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2534 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6485 "parser.cpp"
    break;

  case 323: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2540 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6496 "parser.cpp"
    break;

  case 324: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2546 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6508 "parser.cpp"
    break;

  case 325: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2553 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6520 "parser.cpp"
    break;

  case 326: /* column_expr: IDENTIFIER  */
#line 2561 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6532 "parser.cpp"
    break;

  case 327: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2568 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6544 "parser.cpp"
    break;

  case 328: /* column_expr: '*'  */
#line 2575 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6554 "parser.cpp"
    break;

  case 329: /* column_expr: column_expr '.' '*'  */
#line 2580 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6568 "parser.cpp"
    break;

  case 330: /* constant_expr: STRING  */
#line 2590 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6578 "parser.cpp"
    break;

  case 331: /* constant_expr: TRUE  */
#line 2595 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6588 "parser.cpp"
    break;

  case 332: /* constant_expr: FALSE  */
#line 2600 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6598 "parser.cpp"
    break;

  case 333: /* constant_expr: DOUBLE_VALUE  */
#line 2605 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6608 "parser.cpp"
    break;

  case 334: /* constant_expr: LONG_VALUE  */
#line 2610 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6618 "parser.cpp"
    break;

  case 335: /* constant_expr: DATE STRING  */
#line 2615 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6628 "parser.cpp"
    break;

  case 336: /* constant_expr: TIME STRING  */
#line 2620 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6638 "parser.cpp"
    break;

  case 337: /* constant_expr: DATETIME STRING  */
#line 2625 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6648 "parser.cpp"
    break;

  case 338: /* constant_expr: TIMESTAMP STRING  */
#line 2630 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6658 "parser.cpp"
    break;

  case 339: /* constant_expr: INTERVAL interval_expr  */
#line 2635 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6666 "parser.cpp"
    break;

  case 340: /* constant_expr: interval_expr  */
#line 2638 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6674 "parser.cpp"
    break;

  case 341: /* constant_expr: common_array_expr  */
#line 2641 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6682 "parser.cpp"
    break;

  case 342: /* common_array_expr: array_expr  */
#line 2645 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6690 "parser.cpp"
    break;

  case 343: /* common_array_expr: subarray_array_expr  */
#line 2648 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6698 "parser.cpp"
    break;

  case 344: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2652 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6706 "parser.cpp"
    break;

  case 345: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2656 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 6716 "parser.cpp"
    break;

  case 346: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2661 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6725 "parser.cpp"
    break;

  case 347: /* array_expr: long_array_expr  */
#line 2666 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6733 "parser.cpp"
    break;

  case 348: /* array_expr: double_array_expr  */
#line 2669 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6741 "parser.cpp"
    break;

  case 349: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2673 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6749 "parser.cpp"
    break;

  case 350: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2677 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6759 "parser.cpp"
    break;

  case 351: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2682 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6768 "parser.cpp"
    break;

  case 352: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2687 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6776 "parser.cpp"
    break;

  case 353: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2691 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6786 "parser.cpp"
    break;

  case 354: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2696 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6795 "parser.cpp"
    break;

  case 355: /* interval_expr: LONG_VALUE SECONDS  */
#line 2701 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6806 "parser.cpp"
    break;

  case 356: /* interval_expr: LONG_VALUE SECOND  */
#line 2707 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6817 "parser.cpp"
    break;

  case 357: /* interval_expr: LONG_VALUE MINUTES  */
#line 2713 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6828 "parser.cpp"
    break;

  case 358: /* interval_expr: LONG_VALUE MINUTE  */
#line 2719 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6839 "parser.cpp"
    break;

  case 359: /* interval_expr: LONG_VALUE HOURS  */
#line 2725 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6850 "parser.cpp"
    break;

  case 360: /* interval_expr: LONG_VALUE HOUR  */
#line 2731 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6861 "parser.cpp"
    break;

  case 361: /* interval_expr: LONG_VALUE DAYS  */
#line 2737 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6872 "parser.cpp"
    break;

  case 362: /* interval_expr: LONG_VALUE DAY  */
#line 2743 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6883 "parser.cpp"
    break;

  case 363: /* interval_expr: LONG_VALUE MONTHS  */
#line 2749 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6894 "parser.cpp"
    break;

  case 364: /* interval_expr: LONG_VALUE MONTH  */
#line 2755 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6905 "parser.cpp"
    break;

  case 365: /* interval_expr: LONG_VALUE YEARS  */
#line 2761 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6916 "parser.cpp"
    break;

  case 366: /* interval_expr: LONG_VALUE YEAR  */
#line 2767 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6927 "parser.cpp"
    break;

  case 367: /* copy_option_list: copy_option  */
#line 2778 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6936 "parser.cpp"
    break;

  case 368: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2782 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6945 "parser.cpp"
    break;

  case 369: /* copy_option: FORMAT IDENTIFIER  */
#line 2787 "parser.y"
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
#line 6972 "parser.cpp"
    break;

  case 370: /* copy_option: DELIMITER STRING  */
#line 2809 "parser.y"
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
#line 6987 "parser.cpp"
    break;

  case 371: /* copy_option: HEADER  */
#line 2819 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6997 "parser.cpp"
    break;

  case 372: /* file_path: STRING  */
#line 2825 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7005 "parser.cpp"
    break;

  case 373: /* if_exists: IF EXISTS  */
#line 2829 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7011 "parser.cpp"
    break;

  case 374: /* if_exists: %empty  */
#line 2830 "parser.y"
  { (yyval.bool_value) = false; }
#line 7017 "parser.cpp"
    break;

  case 375: /* if_not_exists: IF NOT EXISTS  */
#line 2832 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7023 "parser.cpp"
    break;

  case 376: /* if_not_exists: %empty  */
#line 2833 "parser.y"
  { (yyval.bool_value) = false; }
#line 7029 "parser.cpp"
    break;

  case 379: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2848 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7042 "parser.cpp"
    break;

  case 380: /* if_not_exists_info: %empty  */
#line 2856 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7050 "parser.cpp"
    break;

  case 381: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2860 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 7058 "parser.cpp"
    break;

  case 382: /* with_index_param_list: %empty  */
#line 2863 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7066 "parser.cpp"
    break;

  case 383: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2867 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7074 "parser.cpp"
    break;

  case 384: /* optional_table_properties_list: %empty  */
#line 2870 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7082 "parser.cpp"
    break;

  case 385: /* index_param_list: index_param  */
#line 2874 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7091 "parser.cpp"
    break;

  case 386: /* index_param_list: index_param_list ',' index_param  */
#line 2878 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7100 "parser.cpp"
    break;

  case 387: /* index_param: IDENTIFIER  */
#line 2883 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7110 "parser.cpp"
    break;

  case 388: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2888 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7123 "parser.cpp"
    break;

  case 389: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2896 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7135 "parser.cpp"
    break;

  case 390: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2903 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7147 "parser.cpp"
    break;

  case 391: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2914 "parser.y"
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
#line 7200 "parser.cpp"
    break;

  case 392: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2962 "parser.y"
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
#line 7254 "parser.cpp"
    break;

  case 393: /* index_info_list: '(' identifier_array ')'  */
#line 3011 "parser.y"
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
#line 7272 "parser.cpp"
    break;


#line 7276 "parser.cpp"

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

#line 3025 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

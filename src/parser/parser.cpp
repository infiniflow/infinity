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
  YYSYMBOL_TENSORARRAY = 120,              /* TENSORARRAY  */
  YYSYMBOL_PRIMARY = 121,                  /* PRIMARY  */
  YYSYMBOL_KEY = 122,                      /* KEY  */
  YYSYMBOL_UNIQUE = 123,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 124,                 /* NULLABLE  */
  YYSYMBOL_IS = 125,                       /* IS  */
  YYSYMBOL_DEFAULT = 126,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 127,                     /* TRUE  */
  YYSYMBOL_FALSE = 128,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 129,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 130,                   /* SECOND  */
  YYSYMBOL_SECONDS = 131,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 132,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 133,                  /* MINUTES  */
  YYSYMBOL_HOUR = 134,                     /* HOUR  */
  YYSYMBOL_HOURS = 135,                    /* HOURS  */
  YYSYMBOL_DAY = 136,                      /* DAY  */
  YYSYMBOL_DAYS = 137,                     /* DAYS  */
  YYSYMBOL_MONTH = 138,                    /* MONTH  */
  YYSYMBOL_MONTHS = 139,                   /* MONTHS  */
  YYSYMBOL_YEAR = 140,                     /* YEAR  */
  YYSYMBOL_YEARS = 141,                    /* YEARS  */
  YYSYMBOL_EQUAL = 142,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 143,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 144,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 145,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 146,                  /* BETWEEN  */
  YYSYMBOL_AND = 147,                      /* AND  */
  YYSYMBOL_OR = 148,                       /* OR  */
  YYSYMBOL_EXTRACT = 149,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 150,                     /* LIKE  */
  YYSYMBOL_DATA = 151,                     /* DATA  */
  YYSYMBOL_LOG = 152,                      /* LOG  */
  YYSYMBOL_BUFFER = 153,                   /* BUFFER  */
  YYSYMBOL_KNN = 154,                      /* KNN  */
  YYSYMBOL_USING = 155,                    /* USING  */
  YYSYMBOL_SESSION = 156,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 157,                   /* GLOBAL  */
  YYSYMBOL_OFF = 158,                      /* OFF  */
  YYSYMBOL_EXPORT = 159,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 160,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 161,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 162,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 163,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 164,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 165,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 166,                   /* SEARCH  */
  YYSYMBOL_MATCH = 167,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 168,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 169,                    /* QUERY  */
  YYSYMBOL_FUSION = 170,                   /* FUSION  */
  YYSYMBOL_NUMBER = 171,                   /* NUMBER  */
  YYSYMBOL_172_ = 172,                     /* '='  */
  YYSYMBOL_173_ = 173,                     /* '<'  */
  YYSYMBOL_174_ = 174,                     /* '>'  */
  YYSYMBOL_175_ = 175,                     /* '+'  */
  YYSYMBOL_176_ = 176,                     /* '-'  */
  YYSYMBOL_177_ = 177,                     /* '*'  */
  YYSYMBOL_178_ = 178,                     /* '/'  */
  YYSYMBOL_179_ = 179,                     /* '%'  */
  YYSYMBOL_180_ = 180,                     /* '['  */
  YYSYMBOL_181_ = 181,                     /* ']'  */
  YYSYMBOL_182_ = 182,                     /* '('  */
  YYSYMBOL_183_ = 183,                     /* ')'  */
  YYSYMBOL_184_ = 184,                     /* '.'  */
  YYSYMBOL_185_ = 185,                     /* ';'  */
  YYSYMBOL_186_ = 186,                     /* ','  */
  YYSYMBOL_YYACCEPT = 187,                 /* $accept  */
  YYSYMBOL_input_pattern = 188,            /* input_pattern  */
  YYSYMBOL_statement_list = 189,           /* statement_list  */
  YYSYMBOL_statement = 190,                /* statement  */
  YYSYMBOL_explainable_statement = 191,    /* explainable_statement  */
  YYSYMBOL_create_statement = 192,         /* create_statement  */
  YYSYMBOL_table_element_array = 193,      /* table_element_array  */
  YYSYMBOL_table_element = 194,            /* table_element  */
  YYSYMBOL_table_column = 195,             /* table_column  */
  YYSYMBOL_column_type = 196,              /* column_type  */
  YYSYMBOL_column_constraints = 197,       /* column_constraints  */
  YYSYMBOL_column_constraint = 198,        /* column_constraint  */
  YYSYMBOL_default_expr = 199,             /* default_expr  */
  YYSYMBOL_table_constraint = 200,         /* table_constraint  */
  YYSYMBOL_identifier_array = 201,         /* identifier_array  */
  YYSYMBOL_delete_statement = 202,         /* delete_statement  */
  YYSYMBOL_insert_statement = 203,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 204, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 205,        /* explain_statement  */
  YYSYMBOL_explain_type = 206,             /* explain_type  */
  YYSYMBOL_update_statement = 207,         /* update_statement  */
  YYSYMBOL_update_expr_array = 208,        /* update_expr_array  */
  YYSYMBOL_update_expr = 209,              /* update_expr  */
  YYSYMBOL_drop_statement = 210,           /* drop_statement  */
  YYSYMBOL_copy_statement = 211,           /* copy_statement  */
  YYSYMBOL_select_statement = 212,         /* select_statement  */
  YYSYMBOL_select_with_paren = 213,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 214,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 215, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 216, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 217, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 218,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 219,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 220,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 221,            /* order_by_type  */
  YYSYMBOL_limit_expr = 222,               /* limit_expr  */
  YYSYMBOL_offset_expr = 223,              /* offset_expr  */
  YYSYMBOL_distinct = 224,                 /* distinct  */
  YYSYMBOL_from_clause = 225,              /* from_clause  */
  YYSYMBOL_search_clause = 226,            /* search_clause  */
  YYSYMBOL_where_clause = 227,             /* where_clause  */
  YYSYMBOL_having_clause = 228,            /* having_clause  */
  YYSYMBOL_group_by_clause = 229,          /* group_by_clause  */
  YYSYMBOL_set_operator = 230,             /* set_operator  */
  YYSYMBOL_table_reference = 231,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 232,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 233,     /* table_reference_name  */
  YYSYMBOL_table_name = 234,               /* table_name  */
  YYSYMBOL_table_alias = 235,              /* table_alias  */
  YYSYMBOL_with_clause = 236,              /* with_clause  */
  YYSYMBOL_with_expr_list = 237,           /* with_expr_list  */
  YYSYMBOL_with_expr = 238,                /* with_expr  */
  YYSYMBOL_join_clause = 239,              /* join_clause  */
  YYSYMBOL_join_type = 240,                /* join_type  */
  YYSYMBOL_show_statement = 241,           /* show_statement  */
  YYSYMBOL_flush_statement = 242,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 243,       /* optimize_statement  */
  YYSYMBOL_command_statement = 244,        /* command_statement  */
  YYSYMBOL_compact_statement = 245,        /* compact_statement  */
  YYSYMBOL_expr_array = 246,               /* expr_array  */
  YYSYMBOL_expr_array_list = 247,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 248,               /* expr_alias  */
  YYSYMBOL_expr = 249,                     /* expr  */
  YYSYMBOL_operand = 250,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 251, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 252,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 253,        /* match_vector_expr  */
  YYSYMBOL_match_text_expr = 254,          /* match_text_expr  */
  YYSYMBOL_query_expr = 255,               /* query_expr  */
  YYSYMBOL_fusion_expr = 256,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 257,         /* sub_search_array  */
  YYSYMBOL_function_expr = 258,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 259,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 260,             /* between_expr  */
  YYSYMBOL_in_expr = 261,                  /* in_expr  */
  YYSYMBOL_case_expr = 262,                /* case_expr  */
  YYSYMBOL_case_check_array = 263,         /* case_check_array  */
  YYSYMBOL_cast_expr = 264,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 265,            /* subquery_expr  */
  YYSYMBOL_column_expr = 266,              /* column_expr  */
  YYSYMBOL_constant_expr = 267,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 268,        /* common_array_expr  */
  YYSYMBOL_subarray_array_expr = 269,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 270, /* unclosed_subarray_array_expr  */
  YYSYMBOL_array_expr = 271,               /* array_expr  */
  YYSYMBOL_long_array_expr = 272,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 273, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 274,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 275, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 276,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 277,         /* copy_option_list  */
  YYSYMBOL_copy_option = 278,              /* copy_option  */
  YYSYMBOL_file_path = 279,                /* file_path  */
  YYSYMBOL_if_exists = 280,                /* if_exists  */
  YYSYMBOL_if_not_exists = 281,            /* if_not_exists  */
  YYSYMBOL_semicolon = 282,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 283,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 284,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 285, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 286,         /* index_param_list  */
  YYSYMBOL_index_param = 287,              /* index_param  */
  YYSYMBOL_index_info_list = 288           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 417 "parser.cpp"

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
#define YYLAST   993

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  187
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  102
/* YYNRULES -- Number of rules.  */
#define YYNRULES  391
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  798

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   426


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
       2,     2,     2,     2,     2,     2,     2,   179,     2,     2,
     182,   183,   177,   175,   186,   176,   184,   178,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   185,
     173,   172,   174,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   180,     2,   181,     2,     2,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   477,   477,   481,   487,   494,   495,   496,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   508,   509,
     510,   511,   512,   513,   514,   515,   516,   517,   518,   519,
     526,   543,   559,   588,   604,   622,   651,   655,   661,   664,
     670,   709,   745,   746,   747,   748,   749,   750,   751,   752,
     753,   754,   755,   756,   757,   758,   759,   760,   761,   762,
     763,   766,   768,   769,   770,   771,   774,   775,   776,   777,
     778,   779,   780,   781,   782,   783,   784,   785,   786,   787,
     788,   789,   790,   791,   792,   793,   794,   795,   796,   797,
     798,   799,   800,   801,   802,   803,   804,   805,   824,   828,
     838,   841,   844,   847,   851,   854,   859,   864,   871,   877,
     887,   903,   937,   950,   953,   960,   966,   969,   972,   975,
     978,   981,   984,   987,   994,  1007,  1011,  1016,  1029,  1042,
    1057,  1072,  1087,  1110,  1151,  1196,  1199,  1202,  1211,  1221,
    1224,  1228,  1233,  1255,  1258,  1263,  1279,  1282,  1286,  1290,
    1295,  1301,  1304,  1307,  1311,  1315,  1317,  1321,  1323,  1326,
    1330,  1333,  1337,  1342,  1346,  1349,  1353,  1356,  1360,  1363,
    1367,  1370,  1373,  1376,  1384,  1387,  1402,  1402,  1404,  1418,
    1427,  1432,  1441,  1446,  1451,  1457,  1464,  1467,  1471,  1474,
    1479,  1491,  1498,  1512,  1515,  1518,  1521,  1524,  1527,  1530,
    1536,  1540,  1544,  1548,  1552,  1559,  1563,  1567,  1571,  1577,
    1583,  1589,  1600,  1611,  1622,  1634,  1646,  1659,  1673,  1684,
    1702,  1706,  1710,  1718,  1732,  1738,  1743,  1749,  1755,  1763,
    1769,  1775,  1781,  1787,  1795,  1801,  1807,  1813,  1819,  1827,
    1833,  1840,  1857,  1861,  1866,  1870,  1897,  1903,  1907,  1908,
    1909,  1910,  1911,  1913,  1916,  1922,  1925,  1926,  1927,  1928,
    1929,  1930,  1931,  1932,  1933,  1935,  1938,  1944,  1966,  2132,
    2140,  2151,  2157,  2166,  2172,  2182,  2186,  2190,  2194,  2198,
    2202,  2206,  2210,  2214,  2218,  2223,  2231,  2239,  2248,  2255,
    2262,  2269,  2276,  2283,  2291,  2299,  2307,  2315,  2323,  2331,
    2339,  2347,  2355,  2363,  2371,  2379,  2409,  2417,  2426,  2434,
    2443,  2451,  2457,  2464,  2470,  2477,  2482,  2489,  2496,  2504,
    2528,  2534,  2540,  2547,  2555,  2562,  2569,  2574,  2584,  2589,
    2594,  2599,  2604,  2609,  2614,  2619,  2624,  2629,  2632,  2635,
    2639,  2642,  2646,  2650,  2655,  2660,  2663,  2667,  2671,  2676,
    2681,  2685,  2690,  2695,  2701,  2707,  2713,  2719,  2725,  2731,
    2737,  2743,  2749,  2755,  2761,  2772,  2776,  2781,  2803,  2813,
    2819,  2823,  2824,  2826,  2827,  2829,  2830,  2842,  2850,  2854,
    2857,  2861,  2864,  2868,  2872,  2877,  2882,  2890,  2897,  2908,
    2956,  3005
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
  "EMBEDDING", "VECTOR", "BIT", "TEXT", "TENSOR", "TENSORARRAY", "PRIMARY",
  "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE",
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

#define YYPACT_NINF (-698)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-379)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     611,   279,    23,   339,    85,   -17,    85,    56,   436,   231,
      46,   138,    95,    85,   109,   -27,   -63,   139,   -32,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,   235,  -698,  -698,
     135,  -698,  -698,  -698,  -698,  -698,    72,    72,    72,    72,
      17,    85,   125,   125,   125,   125,   125,    74,   218,    85,
      97,   206,   268,   281,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,   637,   300,    85,  -698,  -698,  -698,    50,    58,  -698,
     304,  -698,    85,  -698,  -698,  -698,  -698,  -698,   255,   127,
    -698,   318,   134,   147,  -698,    52,  -698,   333,  -698,  -698,
       0,   293,  -698,   298,   322,   375,    85,    85,    85,   401,
     343,   232,   341,   424,    85,    85,    85,   434,   440,   445,
     414,   446,   446,    22,    26,    48,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,   235,  -698,  -698,  -698,  -698,  -698,  -698,
     361,  -698,   479,  -698,   490,  -698,  -698,   319,   109,   446,
    -698,  -698,  -698,  -698,     0,  -698,  -698,  -698,   395,   448,
     431,   438,  -698,   -33,  -698,   232,  -698,    85,   516,    24,
    -698,  -698,  -698,  -698,  -698,   457,  -698,   349,   -52,  -698,
     395,  -698,  -698,   450,   460,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
     524,   534,  -698,  -698,  -698,  -698,  -698,   135,  -698,  -698,
     360,   362,   385,  -698,  -698,   604,   429,   392,   409,   248,
     547,   555,   564,   590,  -698,  -698,   594,   420,    83,   421,
     425,   512,   512,  -698,     5,   282,   -40,  -698,   -21,   582,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,   426,  -698,  -698,  -698,  -112,  -698,  -698,   -99,
    -698,   -89,  -698,   395,   395,   538,  -698,   -63,    27,   556,
     435,  -698,    55,   437,  -698,    85,   395,   445,  -698,   117,
     447,   449,  -698,   247,   452,  -698,  -698,   165,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
     512,   451,   659,   549,   395,   395,   -76,   186,  -698,  -698,
    -698,  -698,   604,  -698,   626,   461,   463,   472,   628,   651,
     195,   195,  -698,  -698,  -698,   459,    73,     4,   395,   491,
     656,   395,   395,   -50,   483,    -4,   512,   512,   512,   512,
     512,   512,   512,   512,   512,   512,   512,   512,   512,   512,
      10,  -698,   486,  -698,   661,  -698,   663,   487,  -698,   -25,
     117,   395,  -698,   235,   750,   552,   493,   129,  -698,  -698,
    -698,   -63,   516,   494,  -698,   675,   395,   497,  -698,   117,
    -698,   386,   386,   674,  -698,  -698,   395,  -698,   160,   549,
     535,   501,    30,   -54,   193,  -698,   395,   395,   609,   395,
     687,    11,   172,   196,  -698,  -698,   -63,   507,   523,  -698,
      54,  -698,  -698,    66,   414,  -698,  -698,   548,   514,   512,
     282,   573,  -698,   311,   311,    51,    51,   638,   311,   311,
      51,    51,   195,   195,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,   395,  -698,  -698,  -698,   117,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,   526,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,   529,
     531,   532,   533,   123,   536,   516,   670,    27,   235,   219,
     516,  -698,   243,   537,   705,   714,  -698,   261,  -698,   277,
     671,   308,  -698,   539,  -698,   750,   395,  -698,   395,   -41,
      60,   512,    57,   543,  -698,    96,  -698,   716,  -698,   719,
      20,     4,   672,  -698,  -698,  -698,  -698,  -698,  -698,   673,
    -698,   743,  -698,  -698,  -698,  -698,  -698,  -698,   561,   695,
     282,   311,   566,   309,  -698,   512,  -698,   744,   208,   348,
     444,   456,   627,   630,  -698,  -698,    13,   123,  -698,  -698,
     516,   323,   571,  -698,  -698,   607,   324,  -698,   395,  -698,
    -698,  -698,   386,  -698,   758,  -698,  -698,   583,   117,    25,
    -698,   395,   527,   585,   763,   486,   586,   588,    54,   523,
       4,     4,   591,    66,   718,   713,   592,   325,  -698,  -698,
     659,   342,   593,   600,   601,   603,   605,   606,   608,   612,
     613,   614,   619,   620,   621,   622,   632,   634,   635,   636,
     639,   640,   641,   642,   643,   644,   677,   678,   681,   682,
     685,   686,   688,   689,  -698,  -698,  -698,  -698,  -698,   369,
    -698,   771,   773,   623,   377,  -698,  -698,  -698,  -698,   117,
    -698,   332,   690,   383,   691,  -698,  -698,  -698,  -698,   727,
     516,  -698,  -698,  -698,  -698,  -698,   395,   395,  -698,  -698,
    -698,  -698,   789,   791,   817,   818,   854,   855,   867,   872,
     873,   874,   875,   876,   877,   878,   879,   880,   881,   882,
     883,   884,   885,   886,   887,   888,   889,   890,   891,   892,
     893,   894,   895,   896,   897,  -698,   732,   393,  -698,   698,
     902,  -698,   903,  -698,   904,   905,   395,   406,   720,   117,
     728,   729,   730,   731,   733,   734,   735,   736,   737,   738,
     739,   740,   741,   742,   745,   746,   747,   748,   749,   751,
     752,   753,   754,   755,   756,   757,   759,   760,   761,   762,
     764,   765,   766,   357,  -698,   771,   768,  -698,   698,   724,
     769,   767,   117,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,   771,  -698,   911,  -698,   922,   407,   770,   772,  -698,
     921,   929,   774,   776,  -698,  -698,   698,  -698
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     187,     0,     0,     0,     0,     0,     0,     0,   123,     0,
       0,     0,     0,     0,     0,     0,   187,     0,   376,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   136,   135,
       0,     8,    14,    15,    16,    17,   374,   374,   374,   374,
     374,     0,   372,   372,   372,   372,   372,   180,     0,     0,
       0,     0,     0,     0,   117,   121,   118,   119,   120,   122,
     116,   187,     0,     0,   201,   202,   200,     0,     0,   203,
       0,   205,     0,   220,   221,   222,   224,   223,     0,   186,
     188,     0,     0,     0,     1,   187,     2,   170,   172,   173,
       0,   159,   141,   147,     0,     0,     0,     0,     0,     0,
       0,   114,     0,     0,     0,     0,     0,     0,     0,     0,
     165,     0,     0,     0,     0,     0,   115,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   210,
     211,   206,     0,   207,     0,   204,   241,     0,     0,     0,
     140,   139,     4,   171,     0,   137,   138,   158,     0,     0,
     155,     0,    30,     0,    31,   114,   377,     0,     0,   187,
     371,   128,   130,   129,   131,     0,   181,     0,   165,   125,
       0,   110,   370,     0,     0,   228,   230,   229,   226,   227,
     233,   235,   234,   231,   232,   238,   240,   239,   236,   237,
       0,     0,   213,   212,   218,   208,   209,     0,   189,   225,
       0,     0,   324,   328,   331,   332,     0,     0,     0,     0,
       0,     0,     0,     0,   329,   330,     0,     0,     0,     0,
       0,     0,     0,   326,     0,   187,   161,   242,   247,   248,
     262,   260,   261,   263,   264,   257,   252,   251,   250,   258,
     259,   249,   256,   255,   339,   341,     0,   340,   345,     0,
     346,     0,   338,     0,     0,   157,   373,   187,     0,     0,
       0,   108,     0,     0,   112,     0,     0,     0,   124,   164,
       0,     0,   219,   214,     0,   144,   143,     0,   354,   353,
     356,   355,   358,   357,   360,   359,   362,   361,   364,   363,
       0,     0,   290,   187,     0,     0,     0,     0,   333,   334,
     335,   336,     0,   337,     0,     0,     0,     0,     0,     0,
     292,   291,   351,   348,   343,     0,     0,     0,     0,   163,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   342,     0,   347,     0,   350,     0,   146,   148,   153,
     154,     0,   142,    33,     0,     0,     0,     0,    36,    38,
      39,   187,     0,    35,   113,     0,     0,   111,   132,   127,
     126,     0,     0,     0,   215,   190,     0,   285,     0,   187,
       0,     0,     0,     0,     0,   315,     0,     0,     0,     0,
       0,     0,     0,     0,   254,   253,   187,   160,   174,   176,
     185,   177,   243,     0,   165,   246,   308,   309,     0,     0,
     187,     0,   289,   299,   300,   303,   304,     0,   306,   298,
     301,   302,   294,   293,   295,   296,   297,   325,   327,   344,
     349,   352,     0,   151,   152,   150,   156,    42,    45,    46,
      43,    44,    47,    48,    62,    49,    51,    50,    65,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,     0,
       0,     0,     0,   105,     0,     0,   382,     0,    34,     0,
       0,   109,     0,     0,     0,     0,   369,     0,   365,     0,
     216,     0,   286,     0,   320,     0,     0,   313,     0,     0,
       0,     0,     0,     0,   324,     0,   271,     0,   273,     0,
       0,     0,     0,   194,   195,   196,   197,   193,   198,     0,
     183,     0,   178,   277,   275,   276,   278,   279,   162,   169,
     187,   307,     0,     0,   288,     0,   149,     0,     0,     0,
       0,     0,     0,     0,   101,   102,     0,   105,    98,    40,
       0,     0,     0,    32,    37,   391,     0,   244,     0,   368,
     367,   134,     0,   133,     0,   287,   321,     0,   317,     0,
     316,     0,     0,     0,     0,     0,     0,     0,   185,   175,
       0,     0,   182,     0,     0,   167,     0,     0,   322,   311,
     310,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   103,   100,   104,    99,    41,     0,
     107,     0,     0,     0,     0,   366,   217,   319,   314,   318,
     305,     0,     0,     0,     0,   272,   274,   179,   191,     0,
       0,   282,   280,   281,   283,   284,     0,     0,   145,   323,
     312,    64,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,   385,     0,   383,   380,
       0,   245,     0,   269,     0,     0,     0,     0,   168,   166,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   381,     0,     0,   389,   380,     0,
       0,     0,   192,   184,    63,    69,    70,    67,    68,    71,
      72,    73,    66,    93,    94,    91,    92,    95,    96,    97,
      90,    77,    78,    75,    76,    79,    80,    81,    74,    85,
      86,    83,    84,    87,    88,    89,    82,   386,   388,   387,
     384,     0,   390,     0,   270,     0,     0,     0,   266,   379,
       0,     0,     0,     0,   265,   267,   380,   268
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -698,  -698,  -698,   856,  -698,   899,  -698,   484,  -698,   469,
    -698,   418,   427,  -698,  -357,   900,   901,   808,  -698,  -698,
     906,  -698,   679,   907,   908,   -57,   949,   -15,   775,   822,
     -42,  -698,  -698,   541,  -698,  -698,  -698,  -698,  -698,  -698,
    -162,  -698,  -698,  -698,  -698,   470,  -181,    32,   402,  -698,
    -698,   836,  -698,  -698,   914,   915,   916,   917,   918,  -275,
    -698,   662,  -170,  -172,  -698,  -394,  -383,  -382,  -381,  -379,
    -698,  -698,  -698,  -698,  -698,  -698,   692,  -698,  -698,   595,
     453,  -221,  -698,  -698,   419,  -698,  -698,  -698,  -698,   777,
     615,   432,    -7,   283,   382,  -698,  -698,  -697,  -698,   200,
     250,  -698
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   357,   358,   359,   463,
     537,   538,   539,   360,   262,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   347,   348,   435,   255,   352,   148,   319,   404,
     171,   648,   575,    90,   397,   398,   399,   400,   512,    30,
      79,    80,   401,   509,    31,    32,    33,    34,    35,   226,
     367,   227,   228,   229,   792,   230,   231,   232,   233,   234,
     518,   235,   236,   237,   238,   239,   297,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     477,   478,   173,   103,    95,    86,   100,   737,   543,   687,
     688,   363
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     269,    83,   378,   314,   123,   469,   268,    47,    91,   513,
     312,   313,   295,   427,   494,   170,    14,   203,   204,   205,
     514,   515,   516,   257,   517,   175,   408,   176,   177,   180,
     354,   181,   182,   486,   292,   320,    48,   317,    50,   296,
      87,   782,    88,   560,    89,    77,   433,   434,   146,   310,
     311,   185,  -375,   186,   187,   316,    41,   510,   263,     1,
      49,     2,     3,     4,     5,     6,     7,     8,     9,   341,
     411,   321,   322,   101,   342,    10,    72,    11,    12,    13,
    -378,   110,   343,   349,   350,   178,   485,   344,    47,   183,
      94,   472,   345,   321,   322,   130,   369,   346,    76,   797,
     409,   481,   201,    14,   136,   174,   321,   322,   541,   628,
     511,   188,    78,   546,   210,   211,   212,   213,   292,    16,
     412,   429,   321,   322,   382,   383,   321,   322,   153,   154,
     155,    14,   199,    81,   267,   523,   162,   163,   164,    84,
     214,   215,   216,    91,   264,    94,   318,   561,   355,   258,
     356,   406,   407,    85,   413,   414,   415,   416,   417,   418,
     419,   420,   421,   422,   423,   424,   425,   426,   202,   203,
     204,   205,   321,   322,   111,   112,   325,   321,   322,   641,
     179,   436,   144,   619,   184,   224,   396,   428,   223,   260,
     642,   643,   644,   224,   645,  -379,  -379,   532,   102,   305,
     353,   306,   307,   568,   321,   322,   189,   321,   322,   113,
     315,    15,    51,    52,   131,   132,   489,   490,    53,   492,
     321,   322,   133,   134,  -379,  -379,   335,   336,   337,   338,
     339,   376,   109,   218,    16,   219,   220,   521,   364,   206,
     207,   365,   519,   563,   533,   577,   534,   535,   208,   536,
     209,   202,   203,   204,   205,    87,   395,    88,   108,    89,
      62,    63,   349,    64,   321,   322,   210,   211,   212,   213,
     385,   114,   386,   624,   387,    65,    66,   487,   381,   488,
     340,   387,   565,   697,   115,   202,   203,   204,   205,    73,
      74,    75,   214,   215,   216,   373,   374,   368,   582,   583,
     584,   585,   586,   129,   468,   587,   588,   135,    36,    37,
      38,   137,   466,   138,   217,   467,   558,   140,   559,   562,
      39,    40,   206,   207,   139,   589,   104,   105,   106,   107,
     141,   208,   218,   209,   219,   220,   295,   312,   313,   500,
     221,   222,   223,   482,   634,   224,   318,   225,   377,   210,
     211,   212,   213,   580,   143,   496,   206,   207,   497,   147,
     777,    14,   778,   779,   483,   208,   149,   209,    42,    43,
      44,   698,   337,   338,   339,   214,   215,   216,   152,   498,
      45,    46,   499,   210,   211,   212,   213,    67,    68,   638,
     639,   629,    69,    70,    71,   522,   151,   217,   202,   203,
     204,   205,   545,   190,   156,   365,   157,   191,   192,   214,
     215,   216,   193,   194,   158,   218,   160,   219,   220,    96,
      97,    98,    99,   221,   222,   223,   547,   161,   224,   318,
     225,   217,   202,   203,   204,   205,   325,   165,   590,   591,
     592,   593,   594,   166,   551,   595,   596,   552,   167,   218,
     172,   219,   220,  -379,  -379,   328,   329,   221,   222,   223,
     553,  -379,   224,   552,   225,   597,   474,   475,   476,   206,
     207,    54,    55,    56,    57,    58,    59,   699,   208,    60,
     209,   170,   195,  -379,   333,   334,   335,   336,   337,   338,
     339,   555,   579,   196,   318,   318,   210,   211,   212,   213,
     254,   197,   253,   290,   291,   576,   620,   623,   650,   365,
     365,   318,   208,   256,   209,   202,   203,   204,   205,   261,
     265,   266,   214,   215,   216,   651,   742,   272,   652,   270,
     210,   211,   212,   213,   598,   599,   600,   601,   602,   271,
     273,   603,   604,   275,   217,   276,   606,   607,   608,   609,
     610,   298,   685,   611,   612,   365,   214,   215,   216,   299,
     691,   605,   218,   318,   219,   220,   693,   277,   300,   694,
     221,   222,   223,   613,   293,   224,   734,   225,   217,   735,
     502,  -199,   503,   504,   505,   506,   290,   507,   508,   743,
     789,   294,   365,   735,   301,   208,   218,   209,   219,   220,
     302,   380,   304,   308,   221,   222,   223,   309,   351,   224,
     340,   225,   361,   210,   211,   212,   213,   362,     1,   366,
       2,     3,     4,     5,     6,     7,     8,     9,    14,   371,
     388,   372,   392,   379,    10,   375,    11,    12,    13,   214,
     215,   216,   394,   389,     1,   390,     2,     3,     4,     5,
       6,     7,   325,     9,   391,   393,   323,   403,   324,   405,
      10,   217,    11,    12,    13,   410,   224,   430,   431,   326,
     327,   328,   329,   432,   464,   465,   470,   331,   471,   218,
     480,   219,   220,   473,   484,   409,   491,   221,   222,   223,
      14,   493,   224,   501,   225,   321,   520,   524,   542,   332,
     333,   334,   335,   336,   337,   338,   339,   325,   527,   549,
     630,   528,   380,   529,   530,   531,    14,   550,   540,   548,
     566,   554,   556,   567,   326,   327,   328,   329,   330,   564,
     570,   571,   331,   380,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   572,   573,   574,   578,
     581,   614,   615,   621,   332,   333,   334,   335,   336,   337,
     338,   339,   622,   325,   626,   631,   627,   633,   647,   635,
      15,   636,   646,   640,   686,   649,   689,   736,   690,   653,
     326,   327,   328,   329,   325,   525,   654,   655,   331,   656,
     696,   657,   658,    16,   659,   700,    15,   701,   660,   661,
     662,   326,   327,   328,   329,   663,   664,   665,   666,   331,
     332,   333,   334,   335,   336,   337,   338,   339,   667,    16,
     668,   669,   670,   702,   703,   671,   672,   673,   674,   675,
     676,   332,   333,   334,   335,   336,   337,   338,   339,   437,
     438,   439,   440,   441,   442,   443,   444,   445,   446,   447,
     448,   449,   450,   451,   452,   453,   454,   455,   456,   457,
     704,   705,   458,   677,   678,   459,   460,   679,   680,   461,
     462,   681,   682,   706,   683,   684,   692,   695,   707,   708,
     709,   710,   711,   712,   713,   714,   715,   716,   717,   718,
     719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
     729,   730,   731,   732,   733,   738,   318,   739,   740,   741,
     783,   744,   745,   746,   747,   787,   748,   749,   750,   751,
     752,   753,   754,   755,   756,   757,   788,   793,   758,   759,
     760,   761,   762,   794,   763,   764,   765,   766,   767,   768,
     769,   142,   770,   771,   772,   773,   370,   774,   775,   776,
     781,   544,   784,   785,   557,   617,   790,   795,   791,   796,
     117,   118,   119,   259,   618,    82,   200,   120,   121,   122,
     637,   569,   274,   526,   198,   124,   125,   126,   127,   128,
     402,   786,   632,     0,   625,   780,   495,   479,   384,   616,
       0,     0,     0,   303
};

static const yytype_int16 yycheck[] =
{
     170,    16,   277,   224,    61,   362,   168,     3,     8,   403,
       5,     6,    88,     3,     3,    67,    79,     4,     5,     6,
     403,   403,   403,    56,   403,     3,    76,     5,     6,     3,
       3,     5,     6,    87,   206,    56,     4,    77,     6,   209,
      20,   738,    22,    84,    24,    13,    71,    72,    90,   221,
     222,     3,     0,     5,     6,   225,    33,     3,    34,     7,
      77,     9,    10,    11,    12,    13,    14,    15,    16,   181,
      74,   147,   148,    41,   186,    23,    30,    25,    26,    27,
      63,    49,   181,   253,   254,    63,    56,   186,     3,    63,
      73,   366,   181,   147,   148,    63,   266,   186,     3,   796,
     150,   376,   144,    79,    72,   112,   147,   148,   465,    84,
      56,    63,     3,   470,   101,   102,   103,   104,   290,   182,
     124,   342,   147,   148,   294,   295,   147,   148,    96,    97,
      98,    79,   139,   160,   186,   410,   104,   105,   106,     0,
     127,   128,   129,     8,   159,    73,   186,    87,   121,   182,
     123,   321,   322,   185,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   335,   336,   337,   338,   339,     3,     4,
       5,     6,   147,   148,    77,    78,   125,   147,   148,   573,
     158,   351,   182,   540,   158,   180,   182,   177,   177,   157,
     573,   573,   573,   180,   573,   144,   145,    74,    73,   116,
     257,   118,   119,   183,   147,   148,   158,   147,   148,     3,
     225,   159,   156,   157,   164,   165,   386,   387,   162,   389,
     147,   148,   164,   165,   173,   174,   175,   176,   177,   178,
     179,    66,    14,   167,   182,   169,   170,   409,   183,    74,
      75,   186,   404,   186,   121,   520,   123,   124,    83,   126,
      85,     3,     4,     5,     6,    20,   183,    22,   184,    24,
      29,    30,   432,    32,   147,   148,   101,   102,   103,   104,
      84,     3,    86,   548,    88,    44,    45,    84,   293,    86,
     184,    88,   186,   640,     3,     3,     4,     5,     6,   151,
     152,   153,   127,   128,   129,    48,    49,   265,    90,    91,
      92,    93,    94,     3,   361,    97,    98,     3,    29,    30,
      31,    56,   183,   186,   149,   186,   486,   183,   488,   491,
      41,    42,    74,    75,     6,   117,    43,    44,    45,    46,
     183,    83,   167,    85,   169,   170,    88,     5,     6,   396,
     175,   176,   177,   183,   565,   180,   186,   182,   183,   101,
     102,   103,   104,   525,    21,   183,    74,    75,   186,    66,
       3,    79,     5,     6,   379,    83,    68,    85,    29,    30,
      31,   646,   177,   178,   179,   127,   128,   129,     3,   183,
      41,    42,   186,   101,   102,   103,   104,   156,   157,   570,
     571,   561,   161,   162,   163,   410,    74,   149,     3,     4,
       5,     6,   183,    42,     3,   186,    63,    46,    47,   127,
     128,   129,    51,    52,   182,   167,    75,   169,   170,    37,
      38,    39,    40,   175,   176,   177,   183,     3,   180,   186,
     182,   149,     3,     4,     5,     6,   125,     3,    90,    91,
      92,    93,    94,     3,   183,    97,    98,   186,     3,   167,
       4,   169,   170,   142,   143,   144,   145,   175,   176,   177,
     183,   150,   180,   186,   182,   117,    80,    81,    82,    74,
      75,    35,    36,    37,    38,    39,    40,   647,    83,    43,
      85,    67,     3,   172,   173,   174,   175,   176,   177,   178,
     179,   183,   183,     3,   186,   186,   101,   102,   103,   104,
      69,   182,    54,    74,    75,   520,   183,   183,   183,   186,
     186,   186,    83,    75,    85,     3,     4,     5,     6,     3,
      63,   172,   127,   128,   129,   183,   696,     3,   186,    79,
     101,   102,   103,   104,    90,    91,    92,    93,    94,    79,
       6,    97,    98,   183,   149,   183,    90,    91,    92,    93,
      94,     4,   183,    97,    98,   186,   127,   128,   129,     4,
     183,   117,   167,   186,   169,   170,   183,   182,     4,   186,
     175,   176,   177,   117,   182,   180,   183,   182,   149,   186,
      57,    58,    59,    60,    61,    62,    74,    64,    65,   183,
     183,   182,   186,   186,     4,    83,   167,    85,   169,   170,
       6,    74,   182,   182,   175,   176,   177,   182,    70,   180,
     184,   182,    56,   101,   102,   103,   104,   182,     7,   182,
       9,    10,    11,    12,    13,    14,    15,    16,    79,   182,
       4,   182,     4,   182,    23,   183,    25,    26,    27,   127,
     128,   129,   183,   182,     7,   182,     9,    10,    11,    12,
      13,    14,   125,    16,   182,     4,    74,   166,    76,     3,
      23,   149,    25,    26,    27,   182,   180,     6,     5,   142,
     143,   144,   145,   186,   122,   182,   182,   150,     3,   167,
       6,   169,   170,   186,   183,   150,    77,   175,   176,   177,
      79,     4,   180,   186,   182,   147,   182,   124,    28,   172,
     173,   174,   175,   176,   177,   178,   179,   125,   182,     4,
     183,   182,    74,   182,   182,   182,    79,     3,   182,   182,
       4,    50,   183,     4,   142,   143,   144,   145,   146,   186,
      58,    58,   150,    74,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,     3,   186,    53,   183,
       6,   124,   122,   182,   172,   173,   174,   175,   176,   177,
     178,   179,   155,   125,     6,   180,   183,     4,    55,   183,
     159,   183,    54,   182,     3,   183,     3,    79,   155,   186,
     142,   143,   144,   145,   125,   147,   186,   186,   150,   186,
      63,   186,   186,   182,   186,     6,   159,     6,   186,   186,
     186,   142,   143,   144,   145,   186,   186,   186,   186,   150,
     172,   173,   174,   175,   176,   177,   178,   179,   186,   182,
     186,   186,   186,     6,     6,   186,   186,   186,   186,   186,
     186,   172,   173,   174,   175,   176,   177,   178,   179,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
       6,     6,   112,   186,   186,   115,   116,   186,   186,   119,
     120,   186,   186,     6,   186,   186,   186,   186,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   172,     3,   186,     4,     4,     4,
     186,   183,   183,   183,   183,     4,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,     4,     6,   183,   183,
     183,   183,   183,     4,   183,   183,   183,   183,   183,   183,
     183,    85,   183,   183,   183,   183,   267,   183,   183,   183,
     182,   467,   183,   186,   485,   537,   186,   183,   186,   183,
      61,    61,    61,   155,   537,    16,   144,    61,    61,    61,
     568,   501,   197,   432,   138,    61,    61,    61,    61,    61,
     318,   781,   563,    -1,   552,   735,   391,   372,   296,   536,
      -1,    -1,    -1,   216
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   159,   182,   188,   189,   190,
     192,   202,   203,   205,   207,   210,   211,   212,   213,   214,
     236,   241,   242,   243,   244,   245,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   234,    77,
     234,   156,   157,   162,    35,    36,    37,    38,    39,    40,
      43,   206,    29,    30,    32,    44,    45,   156,   157,   161,
     162,   163,    30,   151,   152,   153,     3,   234,     3,   237,
     238,   160,   213,   214,     0,   185,   282,    20,    22,    24,
     230,     8,   215,   217,    73,   281,   281,   281,   281,   281,
     283,   234,    73,   280,   280,   280,   280,   280,   184,    14,
     234,    77,    78,     3,     3,     3,   191,   192,   202,   203,
     207,   210,   211,   212,   241,   242,   243,   244,   245,     3,
     234,   164,   165,   164,   165,     3,   234,    56,   186,     6,
     183,   183,   190,    21,   182,   216,   217,    66,   224,    68,
     218,    74,     3,   234,   234,   234,     3,    63,   182,   204,
      75,     3,   234,   234,   234,     3,     3,     3,   208,   209,
      67,   227,     4,   279,   279,     3,     5,     6,    63,   158,
       3,     5,     6,    63,   158,     3,     5,     6,    63,   158,
      42,    46,    47,    51,    52,     3,     3,   182,   238,   279,
     216,   217,     3,     4,     5,     6,    74,    75,    83,    85,
     101,   102,   103,   104,   127,   128,   129,   149,   167,   169,
     170,   175,   176,   177,   180,   182,   246,   248,   249,   250,
     252,   253,   254,   255,   256,   258,   259,   260,   261,   262,
     264,   265,   266,   267,   268,   269,   270,   271,   272,   273,
     274,   275,   276,    54,    69,   222,    75,    56,   182,   204,
     234,     3,   201,    34,   214,    63,   172,   186,   227,   249,
      79,    79,     3,     6,   215,   183,   183,   182,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
      74,    75,   250,   182,   182,    88,   249,   263,     4,     4,
       4,     4,     6,   276,   182,   116,   118,   119,   182,   182,
     250,   250,     5,     6,   268,   214,   249,    77,   186,   225,
      56,   147,   148,    74,    76,   125,   142,   143,   144,   145,
     146,   150,   172,   173,   174,   175,   176,   177,   178,   179,
     184,   181,   186,   181,   186,   181,   186,   219,   220,   249,
     249,    70,   223,   212,     3,   121,   123,   193,   194,   195,
     200,    56,   182,   288,   183,   186,   182,   247,   234,   249,
     209,   182,   182,    48,    49,   183,    66,   183,   246,   182,
      74,   214,   249,   249,   263,    84,    86,    88,     4,   182,
     182,   182,     4,     4,   183,   183,   182,   231,   232,   233,
     234,   239,   248,   166,   226,     3,   249,   249,    76,   150,
     182,    74,   124,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,     3,   177,   268,
       6,     5,   186,    71,    72,   221,   249,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   112,   115,
     116,   119,   120,   196,   122,   182,   183,   186,   212,   201,
     182,     3,   246,   186,    80,    81,    82,   277,   278,   277,
       6,   246,   183,   214,   183,    56,    87,    84,    86,   249,
     249,    77,   249,     4,     3,   266,   183,   186,   183,   186,
     212,   186,    57,    59,    60,    61,    62,    64,    65,   240,
       3,    56,   235,   252,   253,   254,   255,   256,   257,   227,
     182,   250,   214,   246,   124,   147,   220,   182,   182,   182,
     182,   182,    74,   121,   123,   124,   126,   197,   198,   199,
     182,   201,    28,   285,   194,   183,   201,   183,   182,     4,
       3,   183,   186,   183,    50,   183,   183,   196,   249,   249,
      84,    87,   250,   186,   186,   186,     4,     4,   183,   232,
      58,    58,     3,   186,    53,   229,   214,   246,   183,   183,
     250,     6,    90,    91,    92,    93,    94,    97,    98,   117,
      90,    91,    92,    93,    94,    97,    98,   117,    90,    91,
      92,    93,    94,    97,    98,   117,    90,    91,    92,    93,
      94,    97,    98,   117,   124,   122,   267,   198,   199,   201,
     183,   182,   155,   183,   246,   278,     6,   183,    84,   249,
     183,   180,   271,     4,   268,   183,   183,   235,   233,   233,
     182,   252,   253,   254,   255,   256,    54,    55,   228,   183,
     183,   183,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   183,     3,   286,   287,     3,
     155,   183,   186,   183,   186,   186,    63,   201,   246,   249,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   172,   183,   186,    79,   284,     3,     4,
       4,     4,   249,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,     3,     5,     6,
     287,   182,   284,   186,   183,   186,   286,     4,     4,   183,
     186,   186,   251,     6,     4,   183,   183,   284
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   187,   188,   189,   189,   190,   190,   190,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     192,   192,   192,   192,   192,   192,   193,   193,   194,   194,
     195,   195,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   197,   197,
     198,   198,   198,   198,   199,   199,   200,   200,   201,   201,
     202,   203,   203,   204,   204,   205,   206,   206,   206,   206,
     206,   206,   206,   206,   207,   208,   208,   209,   210,   210,
     210,   210,   210,   211,   211,   212,   212,   212,   212,   213,
     213,   214,   215,   216,   216,   217,   218,   218,   219,   219,
     220,   221,   221,   221,   222,   222,   223,   223,   224,   224,
     225,   225,   226,   226,   227,   227,   228,   228,   229,   229,
     230,   230,   230,   230,   231,   231,   232,   232,   233,   233,
     234,   234,   235,   235,   235,   235,   236,   236,   237,   237,
     238,   239,   239,   240,   240,   240,   240,   240,   240,   240,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     242,   242,   242,   243,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   245,   246,   246,   247,   247,   248,   248,   249,   249,
     249,   249,   249,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   251,   251,   252,   253,   254,
     254,   255,   255,   256,   256,   257,   257,   257,   257,   257,
     257,   257,   257,   257,   257,   258,   258,   258,   258,   258,
     258,   258,   258,   258,   258,   258,   258,   258,   258,   258,
     258,   258,   258,   258,   258,   258,   258,   258,   259,   259,
     260,   261,   261,   262,   262,   262,   262,   263,   263,   264,
     265,   265,   265,   265,   266,   266,   266,   266,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     268,   268,   269,   270,   270,   271,   271,   272,   273,   273,
     274,   275,   275,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   277,   277,   278,   278,   278,
     279,   280,   280,   281,   281,   282,   282,   283,   283,   284,
     284,   285,   285,   286,   286,   287,   287,   287,   287,   288,
     288,   288
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
       6,     6,     6,     6,     6,     6,     6,     6,     1,     2,
       2,     1,     1,     2,     2,     0,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     1,     3,     3,     4,     4,
       4,     4,     6,     8,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,     8,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     3,     2,     3,     3,     4,     4,
       3,     3,     4,     4,     5,     6,     7,     9,     4,     5,
       2,     2,     2,     2,     2,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     3,     1,     3,     3,     5,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     0,    12,    14,     7,
       9,     4,     6,     4,     6,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     4,     5,     4,     3,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     6,     3,     4,     3,     3,
       5,     5,     6,     4,     6,     3,     5,     4,     5,     6,
       4,     5,     5,     6,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     2,     2,     3,     1,     1,     2,     2,     3,
       2,     2,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     2,     2,     1,
       1,     2,     0,     3,     0,     1,     0,     2,     0,     4,
       0,     4,     0,     1,     3,     1,     3,     3,     3,     6,
       7,     3
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
#line 2105 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2113 "parser.cpp"
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
#line 2127 "parser.cpp"
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
#line 2141 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2152 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2160 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2169 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2178 "parser.cpp"
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
#line 2192 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2203 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2213 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2223 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2233 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2243 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2253 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2263 "parser.cpp"
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
#line 2277 "parser.cpp"
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
#line 2291 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2301 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2309 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2317 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2326 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2334 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2342 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2350 "parser.cpp"
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
#line 2364 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2373 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2382 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2391 "parser.cpp"
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
#line 2404 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2413 "parser.cpp"
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
#line 2427 "parser.cpp"
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
#line 2441 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 340 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2451 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2460 "parser.cpp"
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
#line 2474 "parser.cpp"
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
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2507 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2515 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2563 "parser.cpp"
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
#line 2577 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2585 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2593 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2609 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2617 "parser.cpp"
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
#line 2630 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2638 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2646 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2654 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2662 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2670 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2678 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2686 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2694 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2702 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2710 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2726 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2734 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2742 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2753 "parser.cpp"
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
#line 2767 "parser.cpp"
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
#line 2781 "parser.cpp"
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
#line 2795 "parser.cpp"
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

#line 2903 "parser.cpp"

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
#line 3118 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 481 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3129 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 487 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3140 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 494 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3146 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3152 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 496 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3158 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 497 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3164 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3170 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3176 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3182 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 501 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3188 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 502 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3194 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 503 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3200 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 504 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3206 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 505 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3212 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 506 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3218 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 508 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3224 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 509 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3230 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3236 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3242 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 512 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3248 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3254 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3260 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3266 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 516 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3272 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 517 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3278 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 518 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3284 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3290 "parser.cpp"
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
#line 3310 "parser.cpp"
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
#line 3328 "parser.cpp"
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
#line 3361 "parser.cpp"
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
#line 3381 "parser.cpp"
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
#line 3402 "parser.cpp"
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
#line 3435 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 651 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3444 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 655 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3453 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 661 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3461 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 664 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3469 "parser.cpp"
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
#line 3513 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 709 "parser.y"
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
#line 3552 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 745 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3558 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 746 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3564 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 747 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3570 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 748 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3576 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 749 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3582 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 750 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3588 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 751 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3594 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 752 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3600 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 753 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3606 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 754 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3612 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 755 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3618 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 756 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3624 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 757 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3630 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 758 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3636 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 759 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3642 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 760 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3648 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 761 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3654 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 762 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3660 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 763 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3666 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 766 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3672 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 768 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3678 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 769 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3684 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 770 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3690 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 771 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3696 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 774 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3702 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 775 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3708 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 776 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3714 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 777 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3720 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 778 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3726 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 779 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3732 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 780 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3738 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 781 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3744 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 782 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3750 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 783 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3756 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 784 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3762 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 785 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3768 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 786 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3774 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 787 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3780 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 788 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3786 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 789 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3792 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 790 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3798 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 791 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3804 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 792 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3810 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3816 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 794 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3822 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 795 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3828 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 796 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3834 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 797 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3840 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 798 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3846 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 799 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3852 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 800 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3858 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3864 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3870 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3876 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3882 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3888 "parser.cpp"
    break;

  case 98: /* column_constraints: column_constraint  */
#line 824 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3897 "parser.cpp"
    break;

  case 99: /* column_constraints: column_constraints column_constraint  */
#line 828 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3911 "parser.cpp"
    break;

  case 100: /* column_constraint: PRIMARY KEY  */
#line 838 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3919 "parser.cpp"
    break;

  case 101: /* column_constraint: UNIQUE  */
#line 841 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3927 "parser.cpp"
    break;

  case 102: /* column_constraint: NULLABLE  */
#line 844 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3935 "parser.cpp"
    break;

  case 103: /* column_constraint: NOT NULLABLE  */
#line 847 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3943 "parser.cpp"
    break;

  case 104: /* default_expr: DEFAULT constant_expr  */
#line 851 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 3951 "parser.cpp"
    break;

  case 105: /* default_expr: %empty  */
#line 854 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 3959 "parser.cpp"
    break;

  case 106: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 859 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3969 "parser.cpp"
    break;

  case 107: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 864 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3979 "parser.cpp"
    break;

  case 108: /* identifier_array: IDENTIFIER  */
#line 871 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3990 "parser.cpp"
    break;

  case 109: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 877 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4001 "parser.cpp"
    break;

  case 110: /* delete_statement: DELETE FROM table_name where_clause  */
#line 887 "parser.y"
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
#line 4018 "parser.cpp"
    break;

  case 111: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 903 "parser.y"
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
#line 4057 "parser.cpp"
    break;

  case 112: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 937 "parser.y"
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
#line 4074 "parser.cpp"
    break;

  case 113: /* optional_identifier_array: '(' identifier_array ')'  */
#line 950 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4082 "parser.cpp"
    break;

  case 114: /* optional_identifier_array: %empty  */
#line 953 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4090 "parser.cpp"
    break;

  case 115: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 960 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4100 "parser.cpp"
    break;

  case 116: /* explain_type: ANALYZE  */
#line 966 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4108 "parser.cpp"
    break;

  case 117: /* explain_type: AST  */
#line 969 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4116 "parser.cpp"
    break;

  case 118: /* explain_type: RAW  */
#line 972 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4124 "parser.cpp"
    break;

  case 119: /* explain_type: LOGICAL  */
#line 975 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4132 "parser.cpp"
    break;

  case 120: /* explain_type: PHYSICAL  */
#line 978 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4140 "parser.cpp"
    break;

  case 121: /* explain_type: PIPELINE  */
#line 981 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4148 "parser.cpp"
    break;

  case 122: /* explain_type: FRAGMENT  */
#line 984 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4156 "parser.cpp"
    break;

  case 123: /* explain_type: %empty  */
#line 987 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4164 "parser.cpp"
    break;

  case 124: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 994 "parser.y"
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
#line 4181 "parser.cpp"
    break;

  case 125: /* update_expr_array: update_expr  */
#line 1007 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4190 "parser.cpp"
    break;

  case 126: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1011 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4199 "parser.cpp"
    break;

  case 127: /* update_expr: IDENTIFIER '=' expr  */
#line 1016 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4211 "parser.cpp"
    break;

  case 128: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1029 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4227 "parser.cpp"
    break;

  case 129: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1042 "parser.y"
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
#line 4245 "parser.cpp"
    break;

  case 130: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1057 "parser.y"
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
#line 4263 "parser.cpp"
    break;

  case 131: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1072 "parser.y"
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
#line 4281 "parser.cpp"
    break;

  case 132: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1087 "parser.y"
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
#line 4304 "parser.cpp"
    break;

  case 133: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1110 "parser.y"
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
#line 4350 "parser.cpp"
    break;

  case 134: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1151 "parser.y"
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
#line 4396 "parser.cpp"
    break;

  case 135: /* select_statement: select_without_paren  */
#line 1196 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4404 "parser.cpp"
    break;

  case 136: /* select_statement: select_with_paren  */
#line 1199 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4412 "parser.cpp"
    break;

  case 137: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1202 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4426 "parser.cpp"
    break;

  case 138: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1211 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4440 "parser.cpp"
    break;

  case 139: /* select_with_paren: '(' select_without_paren ')'  */
#line 1221 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4448 "parser.cpp"
    break;

  case 140: /* select_with_paren: '(' select_with_paren ')'  */
#line 1224 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4456 "parser.cpp"
    break;

  case 141: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1228 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4465 "parser.cpp"
    break;

  case 142: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1233 "parser.y"
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
#line 4491 "parser.cpp"
    break;

  case 143: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1255 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4499 "parser.cpp"
    break;

  case 144: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1258 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4507 "parser.cpp"
    break;

  case 145: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1263 "parser.y"
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
#line 4527 "parser.cpp"
    break;

  case 146: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1279 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4535 "parser.cpp"
    break;

  case 147: /* order_by_clause: %empty  */
#line 1282 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4543 "parser.cpp"
    break;

  case 148: /* order_by_expr_list: order_by_expr  */
#line 1286 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4552 "parser.cpp"
    break;

  case 149: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1290 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4561 "parser.cpp"
    break;

  case 150: /* order_by_expr: expr order_by_type  */
#line 1295 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4571 "parser.cpp"
    break;

  case 151: /* order_by_type: ASC  */
#line 1301 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4579 "parser.cpp"
    break;

  case 152: /* order_by_type: DESC  */
#line 1304 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4587 "parser.cpp"
    break;

  case 153: /* order_by_type: %empty  */
#line 1307 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4595 "parser.cpp"
    break;

  case 154: /* limit_expr: LIMIT expr  */
#line 1311 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4603 "parser.cpp"
    break;

  case 155: /* limit_expr: %empty  */
#line 1315 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4609 "parser.cpp"
    break;

  case 156: /* offset_expr: OFFSET expr  */
#line 1317 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4617 "parser.cpp"
    break;

  case 157: /* offset_expr: %empty  */
#line 1321 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4623 "parser.cpp"
    break;

  case 158: /* distinct: DISTINCT  */
#line 1323 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4631 "parser.cpp"
    break;

  case 159: /* distinct: %empty  */
#line 1326 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4639 "parser.cpp"
    break;

  case 160: /* from_clause: FROM table_reference  */
#line 1330 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4647 "parser.cpp"
    break;

  case 161: /* from_clause: %empty  */
#line 1333 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4655 "parser.cpp"
    break;

  case 162: /* search_clause: SEARCH sub_search_array  */
#line 1337 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4665 "parser.cpp"
    break;

  case 163: /* search_clause: %empty  */
#line 1342 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4673 "parser.cpp"
    break;

  case 164: /* where_clause: WHERE expr  */
#line 1346 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4681 "parser.cpp"
    break;

  case 165: /* where_clause: %empty  */
#line 1349 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4689 "parser.cpp"
    break;

  case 166: /* having_clause: HAVING expr  */
#line 1353 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4697 "parser.cpp"
    break;

  case 167: /* having_clause: %empty  */
#line 1356 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4705 "parser.cpp"
    break;

  case 168: /* group_by_clause: GROUP BY expr_array  */
#line 1360 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4713 "parser.cpp"
    break;

  case 169: /* group_by_clause: %empty  */
#line 1363 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4721 "parser.cpp"
    break;

  case 170: /* set_operator: UNION  */
#line 1367 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4729 "parser.cpp"
    break;

  case 171: /* set_operator: UNION ALL  */
#line 1370 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4737 "parser.cpp"
    break;

  case 172: /* set_operator: INTERSECT  */
#line 1373 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4745 "parser.cpp"
    break;

  case 173: /* set_operator: EXCEPT  */
#line 1376 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4753 "parser.cpp"
    break;

  case 174: /* table_reference: table_reference_unit  */
#line 1384 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4761 "parser.cpp"
    break;

  case 175: /* table_reference: table_reference ',' table_reference_unit  */
#line 1387 "parser.y"
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
#line 4779 "parser.cpp"
    break;

  case 178: /* table_reference_name: table_name table_alias  */
#line 1404 "parser.y"
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
#line 4797 "parser.cpp"
    break;

  case 179: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1418 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4808 "parser.cpp"
    break;

  case 180: /* table_name: IDENTIFIER  */
#line 1427 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4818 "parser.cpp"
    break;

  case 181: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1432 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4830 "parser.cpp"
    break;

  case 182: /* table_alias: AS IDENTIFIER  */
#line 1441 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4840 "parser.cpp"
    break;

  case 183: /* table_alias: IDENTIFIER  */
#line 1446 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4850 "parser.cpp"
    break;

  case 184: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1451 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4861 "parser.cpp"
    break;

  case 185: /* table_alias: %empty  */
#line 1457 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4869 "parser.cpp"
    break;

  case 186: /* with_clause: WITH with_expr_list  */
#line 1464 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4877 "parser.cpp"
    break;

  case 187: /* with_clause: %empty  */
#line 1467 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4885 "parser.cpp"
    break;

  case 188: /* with_expr_list: with_expr  */
#line 1471 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4894 "parser.cpp"
    break;

  case 189: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1474 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4903 "parser.cpp"
    break;

  case 190: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1479 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4915 "parser.cpp"
    break;

  case 191: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1491 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4927 "parser.cpp"
    break;

  case 192: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1498 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4940 "parser.cpp"
    break;

  case 193: /* join_type: INNER  */
#line 1512 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4948 "parser.cpp"
    break;

  case 194: /* join_type: LEFT  */
#line 1515 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4956 "parser.cpp"
    break;

  case 195: /* join_type: RIGHT  */
#line 1518 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4964 "parser.cpp"
    break;

  case 196: /* join_type: OUTER  */
#line 1521 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4972 "parser.cpp"
    break;

  case 197: /* join_type: FULL  */
#line 1524 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4980 "parser.cpp"
    break;

  case 198: /* join_type: CROSS  */
#line 1527 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4988 "parser.cpp"
    break;

  case 199: /* join_type: %empty  */
#line 1530 "parser.y"
                {
}
#line 4995 "parser.cpp"
    break;

  case 200: /* show_statement: SHOW DATABASES  */
#line 1536 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5004 "parser.cpp"
    break;

  case 201: /* show_statement: SHOW TABLES  */
#line 1540 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5013 "parser.cpp"
    break;

  case 202: /* show_statement: SHOW VIEWS  */
#line 1544 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5022 "parser.cpp"
    break;

  case 203: /* show_statement: SHOW CONFIGS  */
#line 1548 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5031 "parser.cpp"
    break;

  case 204: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1552 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5043 "parser.cpp"
    break;

  case 205: /* show_statement: SHOW PROFILES  */
#line 1559 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5052 "parser.cpp"
    break;

  case 206: /* show_statement: SHOW SESSION VARIABLES  */
#line 1563 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5061 "parser.cpp"
    break;

  case 207: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1567 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5070 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1571 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5081 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1577 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5092 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1583 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5103 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW TABLE table_name  */
#line 1589 "parser.y"
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
#line 5119 "parser.cpp"
    break;

  case 212: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1600 "parser.y"
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
#line 5135 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1611 "parser.y"
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
#line 5151 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1622 "parser.y"
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
#line 5168 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1634 "parser.y"
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
#line 5185 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1646 "parser.y"
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
#line 5203 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1659 "parser.y"
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
#line 5222 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1673 "parser.y"
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
#line 5238 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1684 "parser.y"
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
#line 5257 "parser.cpp"
    break;

  case 220: /* flush_statement: FLUSH DATA  */
#line 1702 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5266 "parser.cpp"
    break;

  case 221: /* flush_statement: FLUSH LOG  */
#line 1706 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5275 "parser.cpp"
    break;

  case 222: /* flush_statement: FLUSH BUFFER  */
#line 1710 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5284 "parser.cpp"
    break;

  case 223: /* optimize_statement: OPTIMIZE table_name  */
#line 1718 "parser.y"
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
#line 5299 "parser.cpp"
    break;

  case 224: /* command_statement: USE IDENTIFIER  */
#line 1732 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5310 "parser.cpp"
    break;

  case 225: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1738 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5320 "parser.cpp"
    break;

  case 226: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1743 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5331 "parser.cpp"
    break;

  case 227: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1749 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5342 "parser.cpp"
    break;

  case 228: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1755 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5355 "parser.cpp"
    break;

  case 229: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1763 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5366 "parser.cpp"
    break;

  case 230: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1769 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5377 "parser.cpp"
    break;

  case 231: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1775 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5388 "parser.cpp"
    break;

  case 232: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1781 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5399 "parser.cpp"
    break;

  case 233: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1787 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5412 "parser.cpp"
    break;

  case 234: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1795 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5423 "parser.cpp"
    break;

  case 235: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1801 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5434 "parser.cpp"
    break;

  case 236: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1807 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5445 "parser.cpp"
    break;

  case 237: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1813 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5456 "parser.cpp"
    break;

  case 238: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1819 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5469 "parser.cpp"
    break;

  case 239: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1827 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5480 "parser.cpp"
    break;

  case 240: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1833 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5491 "parser.cpp"
    break;

  case 241: /* compact_statement: COMPACT TABLE table_name  */
#line 1840 "parser.y"
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
#line 5508 "parser.cpp"
    break;

  case 242: /* expr_array: expr_alias  */
#line 1857 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5517 "parser.cpp"
    break;

  case 243: /* expr_array: expr_array ',' expr_alias  */
#line 1861 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5526 "parser.cpp"
    break;

  case 244: /* expr_array_list: '(' expr_array ')'  */
#line 1866 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5535 "parser.cpp"
    break;

  case 245: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1870 "parser.y"
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
#line 5555 "parser.cpp"
    break;

  case 246: /* expr_alias: expr AS IDENTIFIER  */
#line 1897 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5566 "parser.cpp"
    break;

  case 247: /* expr_alias: expr  */
#line 1903 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5574 "parser.cpp"
    break;

  case 253: /* operand: '(' expr ')'  */
#line 1913 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5582 "parser.cpp"
    break;

  case 254: /* operand: '(' select_without_paren ')'  */
#line 1916 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5593 "parser.cpp"
    break;

  case 255: /* operand: constant_expr  */
#line 1922 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5601 "parser.cpp"
    break;

  case 265: /* extra_match_tensor_option: ',' STRING  */
#line 1935 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5609 "parser.cpp"
    break;

  case 266: /* extra_match_tensor_option: %empty  */
#line 1938 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5617 "parser.cpp"
    break;

  case 267: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 1944 "parser.y"
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
#line 5643 "parser.cpp"
    break;

  case 268: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1966 "parser.y"
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
#line 5813 "parser.cpp"
    break;

  case 269: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2132 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5826 "parser.cpp"
    break;

  case 270: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2140 "parser.y"
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
#line 5841 "parser.cpp"
    break;

  case 271: /* query_expr: QUERY '(' STRING ')'  */
#line 2151 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5852 "parser.cpp"
    break;

  case 272: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2157 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5865 "parser.cpp"
    break;

  case 273: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2166 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5876 "parser.cpp"
    break;

  case 274: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2172 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5889 "parser.cpp"
    break;

  case 275: /* sub_search_array: match_vector_expr  */
#line 2182 "parser.y"
                                     {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5898 "parser.cpp"
    break;

  case 276: /* sub_search_array: match_text_expr  */
#line 2186 "parser.y"
                  {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5907 "parser.cpp"
    break;

  case 277: /* sub_search_array: match_tensor_expr  */
#line 2190 "parser.y"
                    {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5916 "parser.cpp"
    break;

  case 278: /* sub_search_array: query_expr  */
#line 2194 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5925 "parser.cpp"
    break;

  case 279: /* sub_search_array: fusion_expr  */
#line 2198 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5934 "parser.cpp"
    break;

  case 280: /* sub_search_array: sub_search_array ',' match_vector_expr  */
#line 2202 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5943 "parser.cpp"
    break;

  case 281: /* sub_search_array: sub_search_array ',' match_text_expr  */
#line 2206 "parser.y"
                                       {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5952 "parser.cpp"
    break;

  case 282: /* sub_search_array: sub_search_array ',' match_tensor_expr  */
#line 2210 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5961 "parser.cpp"
    break;

  case 283: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2214 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5970 "parser.cpp"
    break;

  case 284: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2218 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5979 "parser.cpp"
    break;

  case 285: /* function_expr: IDENTIFIER '(' ')'  */
#line 2223 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5992 "parser.cpp"
    break;

  case 286: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2231 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6005 "parser.cpp"
    break;

  case 287: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2239 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6019 "parser.cpp"
    break;

  case 288: /* function_expr: operand IS NOT NULLABLE  */
#line 2248 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6031 "parser.cpp"
    break;

  case 289: /* function_expr: operand IS NULLABLE  */
#line 2255 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6043 "parser.cpp"
    break;

  case 290: /* function_expr: NOT operand  */
#line 2262 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6055 "parser.cpp"
    break;

  case 291: /* function_expr: '-' operand  */
#line 2269 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6067 "parser.cpp"
    break;

  case 292: /* function_expr: '+' operand  */
#line 2276 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6079 "parser.cpp"
    break;

  case 293: /* function_expr: operand '-' operand  */
#line 2283 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6092 "parser.cpp"
    break;

  case 294: /* function_expr: operand '+' operand  */
#line 2291 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6105 "parser.cpp"
    break;

  case 295: /* function_expr: operand '*' operand  */
#line 2299 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6118 "parser.cpp"
    break;

  case 296: /* function_expr: operand '/' operand  */
#line 2307 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6131 "parser.cpp"
    break;

  case 297: /* function_expr: operand '%' operand  */
#line 2315 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6144 "parser.cpp"
    break;

  case 298: /* function_expr: operand '=' operand  */
#line 2323 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6157 "parser.cpp"
    break;

  case 299: /* function_expr: operand EQUAL operand  */
#line 2331 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6170 "parser.cpp"
    break;

  case 300: /* function_expr: operand NOT_EQ operand  */
#line 2339 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6183 "parser.cpp"
    break;

  case 301: /* function_expr: operand '<' operand  */
#line 2347 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6196 "parser.cpp"
    break;

  case 302: /* function_expr: operand '>' operand  */
#line 2355 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6209 "parser.cpp"
    break;

  case 303: /* function_expr: operand LESS_EQ operand  */
#line 2363 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6222 "parser.cpp"
    break;

  case 304: /* function_expr: operand GREATER_EQ operand  */
#line 2371 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6235 "parser.cpp"
    break;

  case 305: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2379 "parser.y"
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
#line 6270 "parser.cpp"
    break;

  case 306: /* function_expr: operand LIKE operand  */
#line 2409 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6283 "parser.cpp"
    break;

  case 307: /* function_expr: operand NOT LIKE operand  */
#line 2417 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6296 "parser.cpp"
    break;

  case 308: /* conjunction_expr: expr AND expr  */
#line 2426 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6309 "parser.cpp"
    break;

  case 309: /* conjunction_expr: expr OR expr  */
#line 2434 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6322 "parser.cpp"
    break;

  case 310: /* between_expr: operand BETWEEN operand AND operand  */
#line 2443 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6334 "parser.cpp"
    break;

  case 311: /* in_expr: operand IN '(' expr_array ')'  */
#line 2451 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6345 "parser.cpp"
    break;

  case 312: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2457 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6356 "parser.cpp"
    break;

  case 313: /* case_expr: CASE expr case_check_array END  */
#line 2464 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6367 "parser.cpp"
    break;

  case 314: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2470 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6379 "parser.cpp"
    break;

  case 315: /* case_expr: CASE case_check_array END  */
#line 2477 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6389 "parser.cpp"
    break;

  case 316: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2482 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6400 "parser.cpp"
    break;

  case 317: /* case_check_array: WHEN expr THEN expr  */
#line 2489 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6412 "parser.cpp"
    break;

  case 318: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2496 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6424 "parser.cpp"
    break;

  case 319: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2504 "parser.y"
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
#line 6452 "parser.cpp"
    break;

  case 320: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2528 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6463 "parser.cpp"
    break;

  case 321: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2534 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6474 "parser.cpp"
    break;

  case 322: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2540 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6486 "parser.cpp"
    break;

  case 323: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2547 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6498 "parser.cpp"
    break;

  case 324: /* column_expr: IDENTIFIER  */
#line 2555 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6510 "parser.cpp"
    break;

  case 325: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2562 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6522 "parser.cpp"
    break;

  case 326: /* column_expr: '*'  */
#line 2569 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6532 "parser.cpp"
    break;

  case 327: /* column_expr: column_expr '.' '*'  */
#line 2574 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6546 "parser.cpp"
    break;

  case 328: /* constant_expr: STRING  */
#line 2584 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6556 "parser.cpp"
    break;

  case 329: /* constant_expr: TRUE  */
#line 2589 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6566 "parser.cpp"
    break;

  case 330: /* constant_expr: FALSE  */
#line 2594 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6576 "parser.cpp"
    break;

  case 331: /* constant_expr: DOUBLE_VALUE  */
#line 2599 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6586 "parser.cpp"
    break;

  case 332: /* constant_expr: LONG_VALUE  */
#line 2604 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6596 "parser.cpp"
    break;

  case 333: /* constant_expr: DATE STRING  */
#line 2609 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6606 "parser.cpp"
    break;

  case 334: /* constant_expr: TIME STRING  */
#line 2614 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6616 "parser.cpp"
    break;

  case 335: /* constant_expr: DATETIME STRING  */
#line 2619 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6626 "parser.cpp"
    break;

  case 336: /* constant_expr: TIMESTAMP STRING  */
#line 2624 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6636 "parser.cpp"
    break;

  case 337: /* constant_expr: INTERVAL interval_expr  */
#line 2629 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6644 "parser.cpp"
    break;

  case 338: /* constant_expr: interval_expr  */
#line 2632 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6652 "parser.cpp"
    break;

  case 339: /* constant_expr: common_array_expr  */
#line 2635 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6660 "parser.cpp"
    break;

  case 340: /* common_array_expr: array_expr  */
#line 2639 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6668 "parser.cpp"
    break;

  case 341: /* common_array_expr: subarray_array_expr  */
#line 2642 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6676 "parser.cpp"
    break;

  case 342: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2646 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6684 "parser.cpp"
    break;

  case 343: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2650 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 6694 "parser.cpp"
    break;

  case 344: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2655 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6703 "parser.cpp"
    break;

  case 345: /* array_expr: long_array_expr  */
#line 2660 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6711 "parser.cpp"
    break;

  case 346: /* array_expr: double_array_expr  */
#line 2663 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6719 "parser.cpp"
    break;

  case 347: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2667 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6727 "parser.cpp"
    break;

  case 348: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2671 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6737 "parser.cpp"
    break;

  case 349: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2676 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6746 "parser.cpp"
    break;

  case 350: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2681 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6754 "parser.cpp"
    break;

  case 351: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2685 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6764 "parser.cpp"
    break;

  case 352: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2690 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6773 "parser.cpp"
    break;

  case 353: /* interval_expr: LONG_VALUE SECONDS  */
#line 2695 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6784 "parser.cpp"
    break;

  case 354: /* interval_expr: LONG_VALUE SECOND  */
#line 2701 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6795 "parser.cpp"
    break;

  case 355: /* interval_expr: LONG_VALUE MINUTES  */
#line 2707 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6806 "parser.cpp"
    break;

  case 356: /* interval_expr: LONG_VALUE MINUTE  */
#line 2713 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6817 "parser.cpp"
    break;

  case 357: /* interval_expr: LONG_VALUE HOURS  */
#line 2719 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6828 "parser.cpp"
    break;

  case 358: /* interval_expr: LONG_VALUE HOUR  */
#line 2725 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6839 "parser.cpp"
    break;

  case 359: /* interval_expr: LONG_VALUE DAYS  */
#line 2731 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6850 "parser.cpp"
    break;

  case 360: /* interval_expr: LONG_VALUE DAY  */
#line 2737 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6861 "parser.cpp"
    break;

  case 361: /* interval_expr: LONG_VALUE MONTHS  */
#line 2743 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6872 "parser.cpp"
    break;

  case 362: /* interval_expr: LONG_VALUE MONTH  */
#line 2749 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6883 "parser.cpp"
    break;

  case 363: /* interval_expr: LONG_VALUE YEARS  */
#line 2755 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6894 "parser.cpp"
    break;

  case 364: /* interval_expr: LONG_VALUE YEAR  */
#line 2761 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6905 "parser.cpp"
    break;

  case 365: /* copy_option_list: copy_option  */
#line 2772 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6914 "parser.cpp"
    break;

  case 366: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2776 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6923 "parser.cpp"
    break;

  case 367: /* copy_option: FORMAT IDENTIFIER  */
#line 2781 "parser.y"
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
#line 6950 "parser.cpp"
    break;

  case 368: /* copy_option: DELIMITER STRING  */
#line 2803 "parser.y"
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
#line 6965 "parser.cpp"
    break;

  case 369: /* copy_option: HEADER  */
#line 2813 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6975 "parser.cpp"
    break;

  case 370: /* file_path: STRING  */
#line 2819 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6983 "parser.cpp"
    break;

  case 371: /* if_exists: IF EXISTS  */
#line 2823 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6989 "parser.cpp"
    break;

  case 372: /* if_exists: %empty  */
#line 2824 "parser.y"
  { (yyval.bool_value) = false; }
#line 6995 "parser.cpp"
    break;

  case 373: /* if_not_exists: IF NOT EXISTS  */
#line 2826 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7001 "parser.cpp"
    break;

  case 374: /* if_not_exists: %empty  */
#line 2827 "parser.y"
  { (yyval.bool_value) = false; }
#line 7007 "parser.cpp"
    break;

  case 377: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2842 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7020 "parser.cpp"
    break;

  case 378: /* if_not_exists_info: %empty  */
#line 2850 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7028 "parser.cpp"
    break;

  case 379: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2854 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 7036 "parser.cpp"
    break;

  case 380: /* with_index_param_list: %empty  */
#line 2857 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7044 "parser.cpp"
    break;

  case 381: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2861 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7052 "parser.cpp"
    break;

  case 382: /* optional_table_properties_list: %empty  */
#line 2864 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7060 "parser.cpp"
    break;

  case 383: /* index_param_list: index_param  */
#line 2868 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7069 "parser.cpp"
    break;

  case 384: /* index_param_list: index_param_list ',' index_param  */
#line 2872 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7078 "parser.cpp"
    break;

  case 385: /* index_param: IDENTIFIER  */
#line 2877 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7088 "parser.cpp"
    break;

  case 386: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2882 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7101 "parser.cpp"
    break;

  case 387: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2890 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7113 "parser.cpp"
    break;

  case 388: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2897 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7125 "parser.cpp"
    break;

  case 389: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2908 "parser.y"
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
#line 7178 "parser.cpp"
    break;

  case 390: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2956 "parser.y"
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
#line 7232 "parser.cpp"
    break;

  case 391: /* index_info_list: '(' identifier_array ')'  */
#line 3005 "parser.y"
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
#line 7250 "parser.cpp"
    break;


#line 7254 "parser.cpp"

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

#line 3019 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

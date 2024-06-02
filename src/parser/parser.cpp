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
  YYSYMBOL_188_ = 188,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 189,                 /* $accept  */
  YYSYMBOL_input_pattern = 190,            /* input_pattern  */
  YYSYMBOL_statement_list = 191,           /* statement_list  */
  YYSYMBOL_statement = 192,                /* statement  */
  YYSYMBOL_explainable_statement = 193,    /* explainable_statement  */
  YYSYMBOL_create_statement = 194,         /* create_statement  */
  YYSYMBOL_table_element_array = 195,      /* table_element_array  */
  YYSYMBOL_table_element = 196,            /* table_element  */
  YYSYMBOL_table_column = 197,             /* table_column  */
  YYSYMBOL_column_type = 198,              /* column_type  */
  YYSYMBOL_column_constraints = 199,       /* column_constraints  */
  YYSYMBOL_column_constraint = 200,        /* column_constraint  */
  YYSYMBOL_default_expr = 201,             /* default_expr  */
  YYSYMBOL_table_constraint = 202,         /* table_constraint  */
  YYSYMBOL_identifier_array = 203,         /* identifier_array  */
  YYSYMBOL_delete_statement = 204,         /* delete_statement  */
  YYSYMBOL_insert_statement = 205,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 206, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 207,        /* explain_statement  */
  YYSYMBOL_explain_type = 208,             /* explain_type  */
  YYSYMBOL_update_statement = 209,         /* update_statement  */
  YYSYMBOL_update_expr_array = 210,        /* update_expr_array  */
  YYSYMBOL_update_expr = 211,              /* update_expr  */
  YYSYMBOL_drop_statement = 212,           /* drop_statement  */
  YYSYMBOL_copy_statement = 213,           /* copy_statement  */
  YYSYMBOL_select_statement = 214,         /* select_statement  */
  YYSYMBOL_select_with_paren = 215,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 216,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 217, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 218, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 219, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 220,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 221,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 222,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 223,            /* order_by_type  */
  YYSYMBOL_limit_expr = 224,               /* limit_expr  */
  YYSYMBOL_offset_expr = 225,              /* offset_expr  */
  YYSYMBOL_distinct = 226,                 /* distinct  */
  YYSYMBOL_from_clause = 227,              /* from_clause  */
  YYSYMBOL_search_clause = 228,            /* search_clause  */
  YYSYMBOL_where_clause = 229,             /* where_clause  */
  YYSYMBOL_having_clause = 230,            /* having_clause  */
  YYSYMBOL_group_by_clause = 231,          /* group_by_clause  */
  YYSYMBOL_set_operator = 232,             /* set_operator  */
  YYSYMBOL_table_reference = 233,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 234,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 235,     /* table_reference_name  */
  YYSYMBOL_table_name = 236,               /* table_name  */
  YYSYMBOL_table_alias = 237,              /* table_alias  */
  YYSYMBOL_with_clause = 238,              /* with_clause  */
  YYSYMBOL_with_expr_list = 239,           /* with_expr_list  */
  YYSYMBOL_with_expr = 240,                /* with_expr  */
  YYSYMBOL_join_clause = 241,              /* join_clause  */
  YYSYMBOL_join_type = 242,                /* join_type  */
  YYSYMBOL_show_statement = 243,           /* show_statement  */
  YYSYMBOL_flush_statement = 244,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 245,       /* optimize_statement  */
  YYSYMBOL_command_statement = 246,        /* command_statement  */
  YYSYMBOL_compact_statement = 247,        /* compact_statement  */
  YYSYMBOL_expr_array = 248,               /* expr_array  */
  YYSYMBOL_expr_array_list = 249,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 250,               /* expr_alias  */
  YYSYMBOL_expr = 251,                     /* expr  */
  YYSYMBOL_operand = 252,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 253, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 254,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 255,        /* match_vector_expr  */
  YYSYMBOL_match_text_expr = 256,          /* match_text_expr  */
  YYSYMBOL_query_expr = 257,               /* query_expr  */
  YYSYMBOL_fusion_expr = 258,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 259,         /* sub_search_array  */
  YYSYMBOL_function_expr = 260,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 261,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 262,             /* between_expr  */
  YYSYMBOL_in_expr = 263,                  /* in_expr  */
  YYSYMBOL_case_expr = 264,                /* case_expr  */
  YYSYMBOL_case_check_array = 265,         /* case_check_array  */
  YYSYMBOL_cast_expr = 266,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 267,            /* subquery_expr  */
  YYSYMBOL_column_expr = 268,              /* column_expr  */
  YYSYMBOL_constant_expr = 269,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 270,        /* common_array_expr  */
  YYSYMBOL_subarray_array_expr = 271,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 272, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 273,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 274,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 275, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 276, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 277, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 278,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 279,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 280,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 281,               /* array_expr  */
  YYSYMBOL_long_array_expr = 282,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 283, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 284,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 285, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 286,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 287,         /* copy_option_list  */
  YYSYMBOL_copy_option = 288,              /* copy_option  */
  YYSYMBOL_file_path = 289,                /* file_path  */
  YYSYMBOL_if_exists = 290,                /* if_exists  */
  YYSYMBOL_if_not_exists = 291,            /* if_not_exists  */
  YYSYMBOL_semicolon = 292,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 293,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 294,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 295, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 296,         /* index_param_list  */
  YYSYMBOL_index_param = 297,              /* index_param  */
  YYSYMBOL_index_info_list = 298           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 427 "parser.cpp"

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
#define YYLAST   1096

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  189
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  110
/* YYNRULES -- Number of rules.  */
#define YYNRULES  413
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  857

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
       2,     2,     2,     2,     2,     2,     2,     2,   188,   186,
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
       0,   492,   492,   496,   502,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   520,   521,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     541,   558,   574,   603,   619,   637,   666,   670,   676,   679,
     685,   733,   769,   770,   771,   772,   773,   774,   775,   776,
     777,   778,   779,   780,   781,   782,   783,   784,   785,   786,
     787,   790,   792,   793,   794,   795,   798,   799,   800,   801,
     802,   803,   804,   805,   806,   807,   808,   809,   810,   811,
     812,   813,   814,   815,   816,   817,   818,   819,   820,   821,
     822,   823,   824,   825,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   836,   837,   856,   860,   870,   873,
     876,   879,   883,   886,   891,   896,   903,   909,   919,   935,
     969,   982,   985,   992,   998,  1001,  1004,  1007,  1010,  1013,
    1016,  1019,  1026,  1039,  1043,  1048,  1061,  1074,  1089,  1104,
    1119,  1142,  1183,  1228,  1231,  1234,  1243,  1253,  1256,  1260,
    1265,  1287,  1290,  1295,  1311,  1314,  1318,  1322,  1327,  1333,
    1336,  1339,  1343,  1347,  1349,  1353,  1355,  1358,  1362,  1365,
    1369,  1374,  1378,  1381,  1385,  1388,  1392,  1395,  1399,  1402,
    1405,  1408,  1416,  1419,  1434,  1434,  1436,  1450,  1459,  1464,
    1473,  1478,  1483,  1489,  1496,  1499,  1503,  1506,  1511,  1523,
    1530,  1544,  1547,  1550,  1553,  1556,  1559,  1562,  1568,  1572,
    1576,  1580,  1584,  1591,  1595,  1599,  1603,  1609,  1615,  1621,
    1632,  1643,  1654,  1666,  1678,  1691,  1705,  1716,  1730,  1750,
    1754,  1758,  1766,  1780,  1786,  1791,  1797,  1803,  1811,  1817,
    1823,  1829,  1835,  1843,  1849,  1855,  1861,  1867,  1875,  1881,
    1888,  1905,  1909,  1914,  1918,  1945,  1951,  1955,  1956,  1957,
    1958,  1959,  1961,  1964,  1970,  1973,  1974,  1975,  1976,  1977,
    1978,  1979,  1980,  1981,  1983,  1986,  1992,  2011,  2053,  2061,
    2072,  2078,  2087,  2093,  2106,  2110,  2114,  2118,  2122,  2126,
    2130,  2134,  2138,  2142,  2147,  2155,  2163,  2172,  2179,  2186,
    2193,  2200,  2207,  2215,  2223,  2231,  2239,  2247,  2255,  2263,
    2271,  2279,  2287,  2295,  2303,  2333,  2341,  2350,  2358,  2367,
    2375,  2381,  2388,  2394,  2401,  2406,  2413,  2420,  2428,  2452,
    2458,  2464,  2471,  2479,  2486,  2493,  2498,  2508,  2513,  2518,
    2523,  2528,  2533,  2538,  2543,  2548,  2553,  2556,  2559,  2563,
    2566,  2569,  2572,  2576,  2580,  2585,  2590,  2593,  2597,  2601,
    2608,  2615,  2619,  2626,  2633,  2637,  2641,  2645,  2648,  2652,
    2656,  2661,  2666,  2670,  2675,  2680,  2686,  2692,  2698,  2704,
    2710,  2716,  2722,  2728,  2734,  2740,  2746,  2757,  2761,  2766,
    2791,  2801,  2807,  2811,  2812,  2814,  2815,  2817,  2818,  2830,
    2838,  2842,  2845,  2849,  2852,  2856,  2860,  2865,  2870,  2878,
    2885,  2896,  2944,  2993
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
  "'('", "')'", "'.'", "';'", "','", "':'", "$accept", "input_pattern",
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
  "subarray_array_expr", "unclosed_subarray_array_expr",
  "sparse_array_expr", "long_sparse_array_expr",
  "unclosed_long_sparse_array_expr", "double_sparse_array_expr",
  "unclosed_double_sparse_array_expr", "empty_array_expr",
  "int_sparse_ele", "float_sparse_ele", "array_expr", "long_array_expr",
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

#define YYPACT_NINF (-756)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-401)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     337,   220,    19,   270,    55,   -10,    55,    66,   450,   224,
      71,    62,    77,    55,    85,   -47,   -58,   127,   -23,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,   180,  -756,  -756,
     189,  -756,  -756,  -756,  -756,  -756,   128,   128,   128,   128,
      23,    55,   154,   154,   154,   154,   154,    52,   228,    55,
      99,   261,   268,   282,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,   263,   299,    55,  -756,  -756,  -756,    93,   144,  -756,
     330,  -756,    55,  -756,  -756,  -756,  -756,  -756,   269,   151,
    -756,   339,   159,   177,  -756,    15,  -756,   353,  -756,  -756,
      35,   319,  -756,   329,   316,   398,    55,    55,    55,   403,
     345,   241,   368,   442,    55,    55,    55,   444,   456,   469,
     409,   480,   480,    54,    58,    69,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,   180,  -756,  -756,  -756,  -756,  -756,  -756,
     289,  -756,   488,  -756,   505,  -756,  -756,   313,    85,   480,
    -756,  -756,  -756,  -756,    35,  -756,  -756,  -756,   429,   461,
     455,   446,  -756,   -21,  -756,   241,  -756,    55,   526,    92,
    -756,  -756,  -756,  -756,  -756,   471,  -756,   365,   -22,  -756,
     429,  -756,  -756,   473,   487,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
     565,   572,  -756,  -756,  -756,  -756,  -756,   189,  -756,  -756,
     400,   402,   406,  -756,  -756,   710,   452,   407,   408,   292,
     579,   588,   589,   590,  -756,  -756,   595,   412,   164,   420,
     421,   513,   513,  -756,    44,   324,   -54,  -756,    20,   571,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,   423,  -756,  -756,  -756,  -109,  -756,  -756,  -100,
    -756,   -48,  -756,  -756,  -756,    -3,  -756,     5,  -756,   429,
     429,   539,  -756,   -58,    48,   555,   430,  -756,  -118,   435,
    -756,    55,   429,   469,  -756,   235,   438,   441,   580,   382,
     443,  -756,  -756,   137,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,   513,   448,   560,   546,
     429,   429,   -11,   148,  -756,  -756,  -756,  -756,   710,  -756,
     628,   457,   463,   470,   650,   670,   113,   113,  -756,   490,
    -756,  -756,  -756,  -756,   492,   -56,    16,   429,   512,   677,
     429,   429,   -29,   499,    -9,   513,   513,   513,   513,   513,
     513,   513,   513,   513,   513,   513,   513,   513,   513,    41,
    -756,   504,  -756,   681,  -756,   682,  -756,   686,  -756,   688,
     508,  -756,    37,   235,   429,  -756,   180,   673,   575,   516,
      60,  -756,  -756,  -756,   -58,   526,   517,  -756,   698,   429,
     515,  -756,   235,  -756,   385,   385,   702,   703,  -756,  -756,
     429,  -756,    68,   546,   559,   528,    25,   -51,   238,  -756,
     429,   429,   636,   429,   715,    43,   119,   132,   445,  -756,
    -756,   -58,   533,   600,  -756,    50,  -756,  -756,   221,   409,
    -756,  -756,   573,   540,   513,   324,   603,  -756,   581,   581,
     303,   303,   397,   581,   581,   303,   303,   113,   113,  -756,
    -756,  -756,  -756,  -756,  -756,   541,  -756,   542,  -756,  -756,
    -756,   429,  -756,  -756,  -756,   235,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,   548,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,   569,   570,
     601,   604,   607,   121,   608,   526,   713,    48,   180,   181,
     526,  -756,   192,   617,   738,   740,  -756,   252,  -756,   253,
    -756,   733,   257,  -756,   602,  -756,   673,   429,  -756,   429,
     -30,   -25,   513,    18,   614,  -756,   147,  -756,   805,  -756,
     806,  -756,  -756,    -6,    16,   760,  -756,  -756,  -756,  -756,
    -756,  -756,   761,  -756,   820,  -756,  -756,  -756,  -756,  -756,
    -756,   637,   772,   324,   581,   642,   277,  -756,   513,   821,
     823,  -756,   824,   321,   558,   705,   714,   723,   704,   709,
    -756,  -756,    79,   121,  -756,  -756,   526,   322,   651,  -756,
    -756,   679,   326,  -756,   429,  -756,  -756,  -756,   385,  -756,
     827,  -756,  -756,   652,   235,   -13,  -756,   429,   493,   656,
     834,   504,   655,   669,    50,   600,    16,    16,   671,   221,
     801,   802,   672,   338,  -756,  -756,   560,   352,   674,   675,
     676,   678,   680,   683,   684,   685,   687,   689,   690,   691,
     692,   693,   694,   695,   696,   697,   699,   700,   701,   706,
     707,   708,   711,   712,   716,   717,   718,   719,   720,   721,
     722,   724,   725,   726,   727,   728,   729,   730,  -756,  -756,
    -756,  -756,  -756,   360,  -756,   855,   856,   734,   362,  -756,
    -756,  -756,  -756,   235,  -756,   545,   731,   376,   732,  -756,
    -756,  -756,  -756,   797,   526,  -756,  -756,  -756,  -756,  -756,
     429,   429,  -756,  -756,  -756,  -756,   858,   860,   862,   863,
     867,   869,   879,   883,   885,   886,   890,   891,   894,   895,
     896,   904,   914,   915,   916,   917,   918,   919,   920,   921,
     922,   923,   924,   925,   926,   927,   928,   929,   930,   931,
     932,   933,   934,   935,   936,   937,   938,  -756,   773,   377,
    -756,   866,   944,  -756,  -756,   945,  -756,   946,   947,   429,
     378,   765,   235,   764,   769,   770,   771,   774,   775,   776,
     777,   778,   779,   780,   781,   782,   783,   784,   785,   786,
     787,   788,   789,   790,   791,   792,   793,   794,   795,   796,
     798,   799,   800,   803,   804,   807,   808,   809,   810,   811,
     812,   813,   814,   815,   399,  -756,   855,   817,  -756,   866,
     816,   818,   819,   235,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
     855,  -756,   952,  -756,   953,   401,   822,   825,  -756,   975,
     981,   826,   829,  -756,  -756,   866,  -756
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     195,     0,     0,     0,     0,     0,     0,     0,   131,     0,
       0,     0,     0,     0,     0,     0,   195,     0,   398,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   144,   143,
       0,     8,    14,    15,    16,    17,   396,   396,   396,   396,
     396,     0,   394,   394,   394,   394,   394,   188,     0,     0,
       0,     0,     0,     0,   125,   129,   126,   127,   128,   130,
     124,   195,     0,     0,   209,   210,   208,     0,     0,   211,
       0,   213,     0,   229,   230,   231,   233,   232,     0,   194,
     196,     0,     0,     0,     1,   195,     2,   178,   180,   181,
       0,   167,   149,   155,     0,     0,     0,     0,     0,     0,
       0,   122,     0,     0,     0,     0,     0,     0,     0,     0,
     173,     0,     0,     0,     0,     0,   123,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   218,
     219,   214,     0,   215,     0,   212,   250,     0,     0,     0,
     148,   147,     4,   179,     0,   145,   146,   166,     0,     0,
     163,     0,    30,     0,    31,   122,   399,     0,     0,   195,
     393,   136,   138,   137,   139,     0,   189,     0,   173,   133,
       0,   118,   392,     0,     0,   237,   239,   238,   235,   236,
     242,   244,   243,   240,   241,   247,   249,   248,   245,   246,
       0,     0,   221,   220,   226,   216,   217,     0,   197,   234,
       0,     0,   333,   337,   340,   341,     0,     0,     0,     0,
       0,     0,     0,     0,   338,   339,     0,     0,     0,     0,
       0,     0,     0,   335,     0,   195,   169,   251,   256,   257,
     271,   269,   270,   272,   273,   266,   261,   260,   259,   267,
     268,   258,   265,   264,   348,   350,     0,   351,   356,     0,
     357,     0,   352,   349,   367,     0,   368,     0,   347,     0,
       0,   165,   395,   195,     0,     0,     0,   116,     0,     0,
     120,     0,     0,     0,   132,   172,     0,     0,   227,   222,
       0,   152,   151,     0,   376,   375,   378,   377,   380,   379,
     382,   381,   384,   383,   386,   385,     0,     0,   299,   195,
       0,     0,     0,     0,   342,   343,   344,   345,     0,   346,
       0,     0,     0,     0,     0,     0,   301,   300,   373,   370,
     364,   354,   359,   362,     0,     0,     0,     0,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     353,     0,   358,     0,   361,     0,   369,     0,   372,     0,
     154,   156,   161,   162,     0,   150,    33,     0,     0,     0,
       0,    36,    38,    39,   195,     0,    35,   121,     0,     0,
     119,   140,   135,   134,     0,     0,     0,     0,   223,   198,
       0,   294,     0,   195,     0,     0,     0,     0,     0,   324,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   263,
     262,   195,   168,   182,   184,   193,   185,   252,     0,   173,
     255,   317,   318,     0,     0,   195,     0,   298,   308,   309,
     312,   313,     0,   315,   307,   310,   311,   303,   302,   304,
     305,   306,   334,   336,   355,     0,   360,     0,   363,   371,
     374,     0,   159,   160,   158,   164,    42,    45,    46,    43,
      44,    47,    48,    62,    49,    51,    50,    65,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,     0,     0,
       0,     0,     0,   113,     0,     0,   404,     0,    34,     0,
       0,   117,     0,     0,     0,     0,   391,     0,   387,     0,
     228,   224,     0,   295,     0,   329,     0,     0,   322,     0,
       0,     0,     0,     0,     0,   333,     0,   280,     0,   282,
       0,   366,   365,     0,     0,     0,   202,   203,   204,   205,
     201,   206,     0,   191,     0,   186,   286,   284,   285,   287,
     288,   170,   177,   195,   316,     0,     0,   297,     0,     0,
       0,   157,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,     0,   113,   106,    40,     0,     0,     0,    32,
      37,   413,     0,   253,     0,   390,   389,   142,     0,   141,
       0,   296,   330,     0,   326,     0,   325,     0,     0,     0,
       0,     0,     0,     0,   193,   183,     0,     0,   190,     0,
       0,   175,     0,     0,   331,   320,   319,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   108,
     112,   107,    41,     0,   115,     0,     0,     0,     0,   388,
     225,   328,   323,   327,   314,     0,     0,     0,     0,   281,
     283,   187,   199,     0,     0,   291,   289,   290,   292,   293,
       0,     0,   153,   332,   321,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   114,   407,     0,
     405,   402,     0,   254,   370,     0,   278,     0,     0,     0,
       0,   176,   174,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   403,     0,     0,   411,   402,
       0,     0,     0,   200,   192,    63,    69,    70,    67,    68,
      71,    72,    73,    66,    93,    94,    91,    92,    95,    96,
      97,    90,    77,    78,    75,    76,    79,    80,    81,    74,
     101,   102,    99,   100,   103,   104,   105,    98,    85,    86,
      83,    84,    87,    88,    89,    82,   408,   410,   409,   406,
       0,   412,     0,   279,     0,     0,     0,   275,   401,     0,
       0,     0,     0,   274,   276,   402,   277
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -756,  -756,  -756,   901,  -756,   940,  -756,   502,  -756,   484,
    -756,   451,   453,  -756,  -370,   943,   950,   850,  -756,  -756,
     954,  -756,   735,   956,   957,   -57,   991,   -15,   828,   875,
     -53,  -756,  -756,   576,  -756,  -756,  -756,  -756,  -756,  -756,
    -162,  -756,  -756,  -756,  -756,   496,   -98,     7,   427,  -756,
    -756,   884,  -756,  -756,   962,   963,   965,   967,   968,  -280,
    -756,   736,  -170,  -189,  -756,  -411,  -410,  -409,  -408,  -406,
    -756,  -756,  -756,  -756,  -756,  -756,   737,  -756,  -756,   625,
     472,  -222,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
     739,   741,   447,  -756,  -756,  -756,  -756,   830,   646,   454,
     -44,   312,   333,  -756,  -756,  -755,  -756,   193,   249,  -756
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   370,   371,   372,   483,
     563,   564,   565,   373,   268,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   360,   361,   454,   261,   365,   148,   328,   419,
     171,   682,   601,    90,   412,   413,   414,   415,   535,    30,
      79,    80,   416,   532,    31,    32,    33,    34,    35,   226,
     380,   227,   228,   229,   851,   230,   231,   232,   233,   234,
     541,   235,   236,   237,   238,   239,   303,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     322,   323,   253,   254,   255,   256,   257,   258,   497,   498,
     173,   103,    95,    86,   100,   788,   569,   729,   730,   376
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     275,    83,   321,   392,   123,   489,   274,   536,   537,   538,
     539,    48,   540,    50,    87,  -397,    88,   298,    89,    47,
      77,    14,     1,   326,     2,     3,     4,     5,     6,     7,
       8,     9,   316,   317,   841,   263,   507,   146,    10,   302,
      11,    12,    13,    91,   442,   170,   515,   423,   101,   318,
     319,   367,    41,   533,   586,   325,   110,   175,    47,   176,
     177,   180,   587,   181,   182,   426,   377,    49,   174,   378,
     130,   662,   185,   350,   186,   187,   329,   301,   351,   136,
      76,   506,   352,   203,   204,   205,  -400,   353,    78,   362,
     363,   201,   330,   331,    14,   199,    94,   330,   331,   492,
     856,    72,   382,   153,   154,   155,   534,   298,   452,   453,
     502,   162,   163,   164,    81,   567,   427,   178,   330,   331,
     572,   183,   424,   330,   331,    16,   269,    84,   410,   444,
     396,   397,   188,   327,   354,   330,   331,   330,   331,   355,
     202,   203,   204,   205,   270,   546,   428,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     421,   422,   264,    85,   266,   273,   330,   331,   330,   331,
     368,    14,   369,   330,   331,    15,   111,   112,   594,   356,
     210,   211,   212,   213,   357,   330,   331,   358,   675,   676,
     677,   678,   359,   679,   455,   558,   653,    91,    16,   411,
      87,    94,    88,   390,    89,   589,   366,   214,   215,   216,
     324,   206,   207,   179,    73,    74,    75,   184,   144,   443,
     208,   223,   209,    51,    52,   224,   320,   102,   189,    53,
     510,   511,   399,   513,   400,   544,   401,   108,   210,   211,
     212,   213,   109,   559,   486,   560,   561,   487,   562,    36,
      37,    38,   503,    62,    63,   327,    64,   542,   131,   132,
     224,    39,    40,   603,   113,   214,   215,   216,    65,    66,
       1,   114,     2,     3,     4,     5,     6,     7,   381,     9,
     311,   362,   312,   313,   395,   115,    10,   217,    11,    12,
      13,   346,   347,   348,   658,   202,   203,   204,   205,    42,
      43,    44,   129,   517,   740,   218,   518,   219,   220,   133,
     134,    45,    46,   221,   222,   223,   519,   488,   224,   520,
     225,   391,   508,   588,   509,   137,   401,   202,   203,   204,
     205,   190,   349,   135,   591,   191,   192,   584,   138,   585,
     193,   194,    14,   140,     1,   139,     2,     3,     4,     5,
       6,     7,     8,     9,   523,   104,   105,   106,   107,   606,
      10,   141,    11,    12,    13,   571,   206,   207,   378,   668,
      96,    97,    98,    99,   143,   208,   573,   209,   504,   327,
     301,    67,    68,   330,   331,   147,    69,    70,    71,   218,
     151,   219,   220,   210,   211,   212,   213,   149,   206,   207,
     741,   152,   836,    14,   837,   838,   156,   208,   157,   209,
     545,   608,   609,   610,   611,   612,    14,   663,   613,   614,
     214,   215,   216,    15,   158,   210,   211,   212,   213,   334,
     387,   388,   202,   203,   204,   205,   577,   579,   615,   578,
     578,   581,   217,   160,   327,   161,    16,   165,  -401,  -401,
     521,   522,   214,   215,   216,   202,   203,   204,   205,   166,
     218,   605,   219,   220,   327,   494,   495,   496,   221,   222,
     223,   394,   167,   224,   217,   225,   170,  -401,  -401,   344,
     345,   346,   347,   348,   172,    54,    55,    56,    57,    58,
      59,   195,   218,    60,   219,   220,   197,    15,   672,   673,
     221,   222,   223,   206,   207,   224,   654,   225,   196,   378,
     657,   742,   208,   378,   209,   259,   202,   203,   204,   205,
      16,   262,   684,   334,   260,   327,   296,   297,   602,   267,
     210,   211,   212,   213,   271,   208,   685,   209,   272,   686,
     335,   336,   337,   338,   727,   548,   733,   378,   340,   327,
     318,   734,   276,   210,   211,   212,   213,   214,   215,   216,
     736,   785,   794,   737,   786,   378,   277,   394,   278,   793,
     341,   342,   343,   344,   345,   346,   347,   348,   279,   217,
     214,   215,   216,   304,   281,   848,   282,   296,   786,   283,
     299,   300,   305,   306,   307,   310,   208,   218,   209,   219,
     220,   308,   217,   314,   315,   221,   222,   223,   349,   364,
     224,   374,   225,   375,   210,   211,   212,   213,   379,   334,
     218,   384,   219,   220,   385,    14,   386,   389,   221,   222,
     223,   393,   402,   224,   394,   225,   335,   336,   337,   338,
     403,   214,   215,   216,   340,   332,   404,   333,   616,   617,
     618,   619,   620,   405,   406,   621,   622,   525,  -207,   526,
     527,   528,   529,   217,   530,   531,   341,   342,   343,   344,
     345,   346,   347,   348,   407,   623,   409,   664,   408,   418,
     420,   218,   425,   219,   220,   224,   334,   445,   447,   221,
     222,   223,   449,   450,   224,   451,   225,   334,   484,   485,
     490,   491,   493,   335,   336,   337,   338,   334,   500,   501,
     424,   340,   505,   512,   335,   336,   337,   338,   339,   514,
     524,   330,   340,   543,  -401,  -401,   337,   338,   547,   549,
     550,   552,  -401,   341,   342,   343,   344,   345,   346,   347,
     348,   568,   575,   576,   341,   342,   343,   344,   345,   346,
     347,   348,   553,   554,  -401,   342,   343,   344,   345,   346,
     347,   348,   456,   457,   458,   459,   460,   461,   462,   463,
     464,   465,   466,   467,   468,   469,   470,   471,   472,   473,
     474,   475,   476,   580,   555,   477,   582,   556,   478,   479,
     557,   566,   480,   481,   482,   624,   625,   626,   627,   628,
     574,   590,   629,   630,   632,   633,   634,   635,   636,   592,
     593,   637,   638,   640,   641,   642,   643,   644,   596,   597,
     645,   646,   631,   598,   599,   600,   604,   522,   521,   648,
     607,   639,   649,   660,   655,   656,   661,   665,   667,   669,
     647,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   670,   674,   680,   683,   681,   728,   731,
     739,   687,   688,   689,   743,   690,   744,   691,   745,   746,
     692,   693,   694,   747,   695,   748,   696,   697,   698,   699,
     700,   701,   702,   703,   704,   749,   705,   706,   707,   750,
     732,   751,   752,   708,   709,   710,   753,   754,   711,   712,
     755,   756,   757,   713,   714,   715,   716,   717,   718,   719,
     758,   720,   721,   722,   723,   724,   725,   726,   735,   738,
     759,   760,   761,   762,   763,   764,   765,   766,   767,   768,
     769,   770,   771,   772,   773,   774,   775,   776,   777,   778,
     779,   780,   781,   782,   783,   787,   784,   789,   795,   790,
     791,   792,   327,   796,   797,   798,   846,   847,   799,   800,
     801,   802,   803,   804,   805,   806,   807,   808,   809,   810,
     811,   812,   813,   814,   815,   816,   817,   818,   819,   820,
     821,   852,   822,   823,   824,   853,   142,   825,   826,   570,
     583,   827,   828,   829,   830,   831,   832,   833,   834,   835,
     840,   117,   843,   842,   118,   265,   844,    82,   383,   849,
     854,   119,   850,   855,   651,   120,   652,   121,   122,   200,
     595,   671,   198,   124,   125,   280,   126,   551,   127,   128,
     516,   499,   659,   845,   650,   839,   666,     0,     0,   398,
       0,     0,     0,     0,     0,     0,   309,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   417,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   446,     0,     0,     0,   448
};

static const yytype_int16 yycheck[] =
{
     170,    16,   224,   283,    61,   375,   168,   418,   418,   418,
     418,     4,   418,     6,    20,     0,    22,   206,    24,     3,
      13,    79,     7,    77,     9,    10,    11,    12,    13,    14,
      15,    16,   221,   222,   789,    56,    87,    90,    23,   209,
      25,    26,    27,     8,     3,    67,     3,    76,    41,     5,
       6,     3,    33,     3,    84,   225,    49,     3,     3,     5,
       6,     3,    87,     5,     6,    74,   184,    77,   112,   187,
      63,    84,     3,   182,     5,     6,    56,    88,   187,    72,
       3,    56,   182,     4,     5,     6,    63,   187,     3,   259,
     260,   144,   148,   149,    79,   139,    73,   148,   149,   379,
     855,    30,   272,    96,    97,    98,    56,   296,    71,    72,
     390,   104,   105,   106,   161,   485,   125,    63,   148,   149,
     490,    63,   151,   148,   149,   183,    34,     0,   184,   351,
     300,   301,    63,   187,   182,   148,   149,   148,   149,   187,
       3,     4,     5,     6,   159,   425,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     330,   331,   183,   186,   157,   187,   148,   149,   148,   149,
     122,    79,   124,   148,   149,   160,    77,    78,   184,   182,
     101,   102,   103,   104,   187,   148,   149,   182,   599,   599,
     599,   599,   187,   599,   364,    74,   566,     8,   183,   183,
      20,    73,    22,    66,    24,   187,   263,   128,   129,   130,
     225,    74,    75,   159,   152,   153,   154,   159,   183,   178,
      83,   178,    85,   157,   158,   181,   182,    73,   159,   163,
     400,   401,    84,   403,    86,   424,    88,   185,   101,   102,
     103,   104,    14,   122,   184,   124,   125,   187,   127,    29,
      30,    31,   184,    29,    30,   187,    32,   419,   165,   166,
     181,    41,    42,   543,     3,   128,   129,   130,    44,    45,
       7,     3,     9,    10,    11,    12,    13,    14,   271,    16,
     116,   451,   118,   119,   299,     3,    23,   150,    25,    26,
      27,   178,   179,   180,   574,     3,     4,     5,     6,    29,
      30,    31,     3,   184,   674,   168,   187,   170,   171,   165,
     166,    41,    42,   176,   177,   178,   184,   374,   181,   187,
     183,   184,    84,   512,    86,    56,    88,     3,     4,     5,
       6,    42,   185,     3,   187,    46,    47,   507,   187,   509,
      51,    52,    79,   184,     7,     6,     9,    10,    11,    12,
      13,    14,    15,    16,   411,    43,    44,    45,    46,   548,
      23,   184,    25,    26,    27,   184,    74,    75,   187,   591,
      37,    38,    39,    40,    21,    83,   184,    85,   393,   187,
      88,   157,   158,   148,   149,    66,   162,   163,   164,   168,
      74,   170,   171,   101,   102,   103,   104,    68,    74,    75,
     680,     3,     3,    79,     5,     6,     3,    83,    63,    85,
     425,    90,    91,    92,    93,    94,    79,   587,    97,    98,
     128,   129,   130,   160,   183,   101,   102,   103,   104,   126,
      48,    49,     3,     4,     5,     6,   184,   184,   117,   187,
     187,   184,   150,    75,   187,     3,   183,     3,   145,   146,
       5,     6,   128,   129,   130,     3,     4,     5,     6,     3,
     168,   184,   170,   171,   187,    80,    81,    82,   176,   177,
     178,    74,     3,   181,   150,   183,    67,   174,   175,   176,
     177,   178,   179,   180,     4,    35,    36,    37,    38,    39,
      40,     3,   168,    43,   170,   171,   183,   160,   596,   597,
     176,   177,   178,    74,    75,   181,   184,   183,     3,   187,
     184,   681,    83,   187,    85,    54,     3,     4,     5,     6,
     183,    75,   184,   126,    69,   187,    74,    75,   543,     3,
     101,   102,   103,   104,    63,    83,   184,    85,   173,   187,
     143,   144,   145,   146,   184,   148,   184,   187,   151,   187,
       5,     6,    79,   101,   102,   103,   104,   128,   129,   130,
     184,   184,   184,   187,   187,   187,    79,    74,     3,   739,
     173,   174,   175,   176,   177,   178,   179,   180,     6,   150,
     128,   129,   130,     4,   184,   184,   184,    74,   187,   183,
     183,   183,     4,     4,     4,   183,    83,   168,    85,   170,
     171,     6,   150,   183,   183,   176,   177,   178,   185,    70,
     181,    56,   183,   183,   101,   102,   103,   104,   183,   126,
     168,   183,   170,   171,   183,    79,    46,   184,   176,   177,
     178,   183,     4,   181,    74,   183,   143,   144,   145,   146,
     183,   128,   129,   130,   151,    74,   183,    76,    90,    91,
      92,    93,    94,   183,     4,    97,    98,    57,    58,    59,
      60,    61,    62,   150,    64,    65,   173,   174,   175,   176,
     177,   178,   179,   180,     4,   117,   184,   184,   188,   167,
       3,   168,   183,   170,   171,   181,   126,     6,     6,   176,
     177,   178,     6,     5,   181,   187,   183,   126,   123,   183,
     183,     3,   187,   143,   144,   145,   146,   126,     6,     6,
     151,   151,   184,    77,   143,   144,   145,   146,   147,     4,
     187,   148,   151,   183,   143,   144,   145,   146,   125,   188,
     188,   183,   151,   173,   174,   175,   176,   177,   178,   179,
     180,    28,     4,     3,   173,   174,   175,   176,   177,   178,
     179,   180,   183,   183,   173,   174,   175,   176,   177,   178,
     179,   180,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,    50,   183,   112,   184,   183,   115,   116,
     183,   183,   119,   120,   121,    90,    91,    92,    93,    94,
     183,   187,    97,    98,    90,    91,    92,    93,    94,     4,
       4,    97,    98,    90,    91,    92,    93,    94,    58,    58,
      97,    98,   117,     3,   187,    53,   184,     6,     5,   125,
       6,   117,   123,     6,   183,   156,   184,   181,     4,   184,
     117,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   184,   183,    54,   184,    55,     3,     3,
      63,   187,   187,   187,     6,   187,     6,   187,     6,     6,
     187,   187,   187,     6,   187,     6,   187,   187,   187,   187,
     187,   187,   187,   187,   187,     6,   187,   187,   187,     6,
     156,     6,     6,   187,   187,   187,     6,     6,   187,   187,
       6,     6,     6,   187,   187,   187,   187,   187,   187,   187,
       6,   187,   187,   187,   187,   187,   187,   187,   187,   187,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,    79,   173,     3,   184,     4,
       4,     4,   187,   184,   184,   184,     4,     4,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,     6,   184,   184,   184,     4,    85,   184,   184,   487,
     506,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     183,    61,   184,   187,    61,   155,   187,    16,   273,   187,
     184,    61,   187,   184,   563,    61,   563,    61,    61,   144,
     524,   594,   138,    61,    61,   197,    61,   451,    61,    61,
     405,   385,   578,   840,   562,   786,   589,    -1,    -1,   302,
      -1,    -1,    -1,    -1,    -1,    -1,   216,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   327,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   353,    -1,    -1,    -1,   355
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   160,   183,   190,   191,   192,
     194,   204,   205,   207,   209,   212,   213,   214,   215,   216,
     238,   243,   244,   245,   246,   247,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   236,    77,
     236,   157,   158,   163,    35,    36,    37,    38,    39,    40,
      43,   208,    29,    30,    32,    44,    45,   157,   158,   162,
     163,   164,    30,   152,   153,   154,     3,   236,     3,   239,
     240,   161,   215,   216,     0,   186,   292,    20,    22,    24,
     232,     8,   217,   219,    73,   291,   291,   291,   291,   291,
     293,   236,    73,   290,   290,   290,   290,   290,   185,    14,
     236,    77,    78,     3,     3,     3,   193,   194,   204,   205,
     209,   212,   213,   214,   243,   244,   245,   246,   247,     3,
     236,   165,   166,   165,   166,     3,   236,    56,   187,     6,
     184,   184,   192,    21,   183,   218,   219,    66,   226,    68,
     220,    74,     3,   236,   236,   236,     3,    63,   183,   206,
      75,     3,   236,   236,   236,     3,     3,     3,   210,   211,
      67,   229,     4,   289,   289,     3,     5,     6,    63,   159,
       3,     5,     6,    63,   159,     3,     5,     6,    63,   159,
      42,    46,    47,    51,    52,     3,     3,   183,   240,   289,
     218,   219,     3,     4,     5,     6,    74,    75,    83,    85,
     101,   102,   103,   104,   128,   129,   130,   150,   168,   170,
     171,   176,   177,   178,   181,   183,   248,   250,   251,   252,
     254,   255,   256,   257,   258,   260,   261,   262,   263,   264,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   278,   281,   282,   283,   284,   285,   286,    54,
      69,   224,    75,    56,   183,   206,   236,     3,   203,    34,
     216,    63,   173,   187,   229,   251,    79,    79,     3,     6,
     217,   184,   184,   183,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,    74,    75,   252,   183,
     183,    88,   251,   265,     4,     4,     4,     4,     6,   286,
     183,   116,   118,   119,   183,   183,   252,   252,     5,     6,
     182,   270,   279,   280,   216,   251,    77,   187,   227,    56,
     148,   149,    74,    76,   126,   143,   144,   145,   146,   147,
     151,   173,   174,   175,   176,   177,   178,   179,   180,   185,
     182,   187,   182,   187,   182,   187,   182,   187,   182,   187,
     221,   222,   251,   251,    70,   225,   214,     3,   122,   124,
     195,   196,   197,   202,    56,   183,   298,   184,   187,   183,
     249,   236,   251,   211,   183,   183,    46,    48,    49,   184,
      66,   184,   248,   183,    74,   216,   251,   251,   265,    84,
      86,    88,     4,   183,   183,   183,     4,     4,   188,   184,
     184,   183,   233,   234,   235,   236,   241,   250,   167,   228,
       3,   251,   251,    76,   151,   183,    74,   125,   252,   252,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,     3,   178,   270,     6,   279,     6,   280,     6,
       5,   187,    71,    72,   223,   251,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   112,   115,   116,
     119,   120,   121,   198,   123,   183,   184,   187,   214,   203,
     183,     3,   248,   187,    80,    81,    82,   287,   288,   287,
       6,     6,   248,   184,   216,   184,    56,    87,    84,    86,
     251,   251,    77,   251,     4,     3,   268,   184,   187,   184,
     187,     5,     6,   214,   187,    57,    59,    60,    61,    62,
      64,    65,   242,     3,    56,   237,   254,   255,   256,   257,
     258,   259,   229,   183,   252,   216,   248,   125,   148,   188,
     188,   222,   183,   183,   183,   183,   183,   183,    74,   122,
     124,   125,   127,   199,   200,   201,   183,   203,    28,   295,
     196,   184,   203,   184,   183,     4,     3,   184,   187,   184,
      50,   184,   184,   198,   251,   251,    84,    87,   252,   187,
     187,   187,     4,     4,   184,   234,    58,    58,     3,   187,
      53,   231,   216,   248,   184,   184,   252,     6,    90,    91,
      92,    93,    94,    97,    98,   117,    90,    91,    92,    93,
      94,    97,    98,   117,    90,    91,    92,    93,    94,    97,
      98,   117,    90,    91,    92,    93,    94,    97,    98,   117,
      90,    91,    92,    93,    94,    97,    98,   117,   125,   123,
     269,   200,   201,   203,   184,   183,   156,   184,   248,   288,
       6,   184,    84,   251,   184,   181,   281,     4,   270,   184,
     184,   237,   235,   235,   183,   254,   255,   256,   257,   258,
      54,    55,   230,   184,   184,   184,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   184,     3,   296,
     297,     3,   156,   184,     6,   187,   184,   187,   187,    63,
     203,   248,   251,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   173,   184,   187,    79,   294,     3,
       4,     4,     4,   251,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,     3,     5,     6,   297,
     183,   294,   187,   184,   187,   296,     4,     4,   184,   187,
     187,   253,     6,     4,   184,   184,   294
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   189,   190,   191,   191,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   192,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     194,   194,   194,   194,   194,   194,   195,   195,   196,   196,
     197,   197,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   199,   199,   200,   200,
     200,   200,   201,   201,   202,   202,   203,   203,   204,   205,
     205,   206,   206,   207,   208,   208,   208,   208,   208,   208,
     208,   208,   209,   210,   210,   211,   212,   212,   212,   212,
     212,   213,   213,   214,   214,   214,   214,   215,   215,   216,
     217,   218,   218,   219,   220,   220,   221,   221,   222,   223,
     223,   223,   224,   224,   225,   225,   226,   226,   227,   227,
     228,   228,   229,   229,   230,   230,   231,   231,   232,   232,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   237,   237,   237,   238,   238,   239,   239,   240,   241,
     241,   242,   242,   242,   242,   242,   242,   242,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   244,
     244,   244,   245,   246,   246,   246,   246,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   246,   246,   246,   246,
     247,   248,   248,   249,   249,   250,   250,   251,   251,   251,
     251,   251,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,   252,   252,   253,   253,   254,   255,   256,   256,
     257,   257,   258,   258,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   261,   261,   262,
     263,   263,   264,   264,   264,   264,   265,   265,   266,   267,
     267,   267,   267,   268,   268,   268,   268,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   270,
     270,   270,   270,   271,   272,   272,   273,   273,   274,   275,
     275,   276,   277,   277,   278,   279,   280,   281,   281,   282,
     283,   283,   284,   285,   285,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   287,   287,   288,
     288,   288,   289,   290,   290,   291,   291,   292,   292,   293,
     293,   294,   294,   295,   295,   296,   296,   297,   297,   297,
     297,   298,   298,   298
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
       6,     6,     6,     6,     6,     6,     1,     2,     2,     1,
       1,     2,     2,     0,     5,     4,     1,     3,     4,     6,
       5,     3,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     0,     5,     1,     3,     3,     4,     4,     4,     4,
       6,     8,     8,     1,     1,     3,     3,     3,     3,     2,
       4,     3,     3,     8,     3,     0,     1,     3,     2,     1,
       1,     0,     2,     0,     2,     0,     1,     0,     2,     0,
       2,     0,     2,     0,     2,     0,     3,     0,     1,     2,
       1,     1,     1,     3,     1,     1,     2,     4,     1,     3,
       2,     1,     5,     0,     2,     0,     1,     3,     5,     4,
       6,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     2,     3,     2,     3,     3,     4,     4,     3,     3,
       4,     4,     5,     6,     7,     9,     4,     5,     7,     2,
       2,     2,     2,     2,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       3,     1,     3,     3,     5,     3,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     0,    12,    14,     7,     9,
       4,     6,     4,     6,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     4,     5,     4,     3,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     6,     3,     4,     3,     3,     5,
       5,     6,     4,     6,     3,     5,     4,     5,     6,     4,
       5,     5,     6,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     1,     1,     2,     2,
       3,     2,     2,     3,     2,     3,     3,     1,     1,     2,
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
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2163 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2171 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 224 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2185 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 214 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2199 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2210 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2218 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2227 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2236 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 274 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2250 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2261 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2271 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2281 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2291 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2301 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2311 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2321 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2335 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2349 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2359 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2367 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2375 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2384 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2392 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2400 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2408 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 234 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2422 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2431 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2449 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 294 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2462 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2471 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 284 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2485 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 284 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2509 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2518 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 234 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2532 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 244 "parser.y"
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
#line 2549 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2557 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2565 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2573 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2581 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2589 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2597 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2613 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2621 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 234 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2635 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2643 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2651 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2659 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2667 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2675 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 355 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2688 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2696 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2704 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2712 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2720 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2728 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2736 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2744 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2752 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2760 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2776 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2784 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 364 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2800 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 368 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2808 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2816 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2824 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2840 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2856 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2864 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2875 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 190 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2889 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 190 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 180 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 2917 "parser.cpp"
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

#line 3025 "parser.cpp"

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
#line 492 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3240 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 496 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3251 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 502 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3262 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 509 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3268 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3274 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3280 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3286 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3292 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3298 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3304 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3310 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3316 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 518 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3322 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 519 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3328 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3334 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3340 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 523 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3346 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3352 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3358 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3364 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3370 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3376 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3382 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3388 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 531 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3394 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 532 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3400 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 533 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3406 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3412 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 541 "parser.y"
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
#line 3432 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 558 "parser.y"
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
#line 3450 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 574 "parser.y"
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
#line 3483 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 603 "parser.y"
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
#line 3503 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 619 "parser.y"
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
#line 3524 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 637 "parser.y"
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
#line 3557 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 666 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3566 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 670 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3575 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 676 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3583 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 679 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3591 "parser.cpp"
    break;

  case 40: /* table_column: IDENTIFIER column_type default_expr  */
#line 685 "parser.y"
                                    {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                free((yyvsp[-2].str_value));
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
            if (type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create sparse info.");
                free((yyvsp[-2].str_value));
                YYERROR;
            }
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
#line 3644 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 733 "parser.y"
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
#line 3683 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 769 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3689 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 770 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3695 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 771 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3701 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 772 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3707 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 773 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3713 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 774 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3719 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3725 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 776 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3731 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 777 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3737 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 778 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3743 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 779 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3749 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 780 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3755 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 781 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3761 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 782 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3767 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 783 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3773 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 784 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3779 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 785 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3785 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 786 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3791 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 787 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3797 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 790 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3803 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 792 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3809 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3815 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 794 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3821 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 795 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3827 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 798 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3833 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 799 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3839 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 800 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3845 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3851 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3857 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3863 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3869 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3875 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3881 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3887 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3893 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3899 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3905 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3911 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3917 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3923 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3929 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3935 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3941 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3947 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3953 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3959 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3965 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3971 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3977 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3983 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3989 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3995 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4001 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4007 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4013 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4019 "parser.cpp"
    break;

  case 98: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4025 "parser.cpp"
    break;

  case 99: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4031 "parser.cpp"
    break;

  case 100: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4037 "parser.cpp"
    break;

  case 101: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4043 "parser.cpp"
    break;

  case 102: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4049 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4055 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4061 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4067 "parser.cpp"
    break;

  case 106: /* column_constraints: column_constraint  */
#line 856 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4076 "parser.cpp"
    break;

  case 107: /* column_constraints: column_constraints column_constraint  */
#line 860 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4090 "parser.cpp"
    break;

  case 108: /* column_constraint: PRIMARY KEY  */
#line 870 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4098 "parser.cpp"
    break;

  case 109: /* column_constraint: UNIQUE  */
#line 873 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4106 "parser.cpp"
    break;

  case 110: /* column_constraint: NULLABLE  */
#line 876 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4114 "parser.cpp"
    break;

  case 111: /* column_constraint: NOT NULLABLE  */
#line 879 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4122 "parser.cpp"
    break;

  case 112: /* default_expr: DEFAULT constant_expr  */
#line 883 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4130 "parser.cpp"
    break;

  case 113: /* default_expr: %empty  */
#line 886 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4138 "parser.cpp"
    break;

  case 114: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 891 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4148 "parser.cpp"
    break;

  case 115: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 896 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4158 "parser.cpp"
    break;

  case 116: /* identifier_array: IDENTIFIER  */
#line 903 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4169 "parser.cpp"
    break;

  case 117: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 909 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4180 "parser.cpp"
    break;

  case 118: /* delete_statement: DELETE FROM table_name where_clause  */
#line 919 "parser.y"
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
#line 4197 "parser.cpp"
    break;

  case 119: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 935 "parser.y"
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
#line 4236 "parser.cpp"
    break;

  case 120: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 969 "parser.y"
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
#line 4253 "parser.cpp"
    break;

  case 121: /* optional_identifier_array: '(' identifier_array ')'  */
#line 982 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4261 "parser.cpp"
    break;

  case 122: /* optional_identifier_array: %empty  */
#line 985 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4269 "parser.cpp"
    break;

  case 123: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 992 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4279 "parser.cpp"
    break;

  case 124: /* explain_type: ANALYZE  */
#line 998 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4287 "parser.cpp"
    break;

  case 125: /* explain_type: AST  */
#line 1001 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4295 "parser.cpp"
    break;

  case 126: /* explain_type: RAW  */
#line 1004 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4303 "parser.cpp"
    break;

  case 127: /* explain_type: LOGICAL  */
#line 1007 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4311 "parser.cpp"
    break;

  case 128: /* explain_type: PHYSICAL  */
#line 1010 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4319 "parser.cpp"
    break;

  case 129: /* explain_type: PIPELINE  */
#line 1013 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4327 "parser.cpp"
    break;

  case 130: /* explain_type: FRAGMENT  */
#line 1016 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4335 "parser.cpp"
    break;

  case 131: /* explain_type: %empty  */
#line 1019 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4343 "parser.cpp"
    break;

  case 132: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1026 "parser.y"
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
#line 4360 "parser.cpp"
    break;

  case 133: /* update_expr_array: update_expr  */
#line 1039 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4369 "parser.cpp"
    break;

  case 134: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1043 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4378 "parser.cpp"
    break;

  case 135: /* update_expr: IDENTIFIER '=' expr  */
#line 1048 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4390 "parser.cpp"
    break;

  case 136: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1061 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4406 "parser.cpp"
    break;

  case 137: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1074 "parser.y"
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
#line 4424 "parser.cpp"
    break;

  case 138: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1089 "parser.y"
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
#line 4442 "parser.cpp"
    break;

  case 139: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1104 "parser.y"
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
#line 4460 "parser.cpp"
    break;

  case 140: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1119 "parser.y"
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
#line 4483 "parser.cpp"
    break;

  case 141: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1142 "parser.y"
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
#line 4529 "parser.cpp"
    break;

  case 142: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1183 "parser.y"
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
#line 4575 "parser.cpp"
    break;

  case 143: /* select_statement: select_without_paren  */
#line 1228 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4583 "parser.cpp"
    break;

  case 144: /* select_statement: select_with_paren  */
#line 1231 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4591 "parser.cpp"
    break;

  case 145: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1234 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4605 "parser.cpp"
    break;

  case 146: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1243 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4619 "parser.cpp"
    break;

  case 147: /* select_with_paren: '(' select_without_paren ')'  */
#line 1253 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4627 "parser.cpp"
    break;

  case 148: /* select_with_paren: '(' select_with_paren ')'  */
#line 1256 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4635 "parser.cpp"
    break;

  case 149: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1260 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4644 "parser.cpp"
    break;

  case 150: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1265 "parser.y"
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
#line 4670 "parser.cpp"
    break;

  case 151: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1287 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4678 "parser.cpp"
    break;

  case 152: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1290 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4686 "parser.cpp"
    break;

  case 153: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1295 "parser.y"
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
#line 4706 "parser.cpp"
    break;

  case 154: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1311 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4714 "parser.cpp"
    break;

  case 155: /* order_by_clause: %empty  */
#line 1314 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4722 "parser.cpp"
    break;

  case 156: /* order_by_expr_list: order_by_expr  */
#line 1318 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4731 "parser.cpp"
    break;

  case 157: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1322 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4740 "parser.cpp"
    break;

  case 158: /* order_by_expr: expr order_by_type  */
#line 1327 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4750 "parser.cpp"
    break;

  case 159: /* order_by_type: ASC  */
#line 1333 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4758 "parser.cpp"
    break;

  case 160: /* order_by_type: DESC  */
#line 1336 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4766 "parser.cpp"
    break;

  case 161: /* order_by_type: %empty  */
#line 1339 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4774 "parser.cpp"
    break;

  case 162: /* limit_expr: LIMIT expr  */
#line 1343 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4782 "parser.cpp"
    break;

  case 163: /* limit_expr: %empty  */
#line 1347 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4788 "parser.cpp"
    break;

  case 164: /* offset_expr: OFFSET expr  */
#line 1349 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4796 "parser.cpp"
    break;

  case 165: /* offset_expr: %empty  */
#line 1353 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4802 "parser.cpp"
    break;

  case 166: /* distinct: DISTINCT  */
#line 1355 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4810 "parser.cpp"
    break;

  case 167: /* distinct: %empty  */
#line 1358 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4818 "parser.cpp"
    break;

  case 168: /* from_clause: FROM table_reference  */
#line 1362 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4826 "parser.cpp"
    break;

  case 169: /* from_clause: %empty  */
#line 1365 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4834 "parser.cpp"
    break;

  case 170: /* search_clause: SEARCH sub_search_array  */
#line 1369 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4844 "parser.cpp"
    break;

  case 171: /* search_clause: %empty  */
#line 1374 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4852 "parser.cpp"
    break;

  case 172: /* where_clause: WHERE expr  */
#line 1378 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4860 "parser.cpp"
    break;

  case 173: /* where_clause: %empty  */
#line 1381 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4868 "parser.cpp"
    break;

  case 174: /* having_clause: HAVING expr  */
#line 1385 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4876 "parser.cpp"
    break;

  case 175: /* having_clause: %empty  */
#line 1388 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4884 "parser.cpp"
    break;

  case 176: /* group_by_clause: GROUP BY expr_array  */
#line 1392 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4892 "parser.cpp"
    break;

  case 177: /* group_by_clause: %empty  */
#line 1395 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4900 "parser.cpp"
    break;

  case 178: /* set_operator: UNION  */
#line 1399 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4908 "parser.cpp"
    break;

  case 179: /* set_operator: UNION ALL  */
#line 1402 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4916 "parser.cpp"
    break;

  case 180: /* set_operator: INTERSECT  */
#line 1405 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4924 "parser.cpp"
    break;

  case 181: /* set_operator: EXCEPT  */
#line 1408 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4932 "parser.cpp"
    break;

  case 182: /* table_reference: table_reference_unit  */
#line 1416 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4940 "parser.cpp"
    break;

  case 183: /* table_reference: table_reference ',' table_reference_unit  */
#line 1419 "parser.y"
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
#line 4958 "parser.cpp"
    break;

  case 186: /* table_reference_name: table_name table_alias  */
#line 1436 "parser.y"
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
#line 4976 "parser.cpp"
    break;

  case 187: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1450 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4987 "parser.cpp"
    break;

  case 188: /* table_name: IDENTIFIER  */
#line 1459 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4997 "parser.cpp"
    break;

  case 189: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1464 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5009 "parser.cpp"
    break;

  case 190: /* table_alias: AS IDENTIFIER  */
#line 1473 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5019 "parser.cpp"
    break;

  case 191: /* table_alias: IDENTIFIER  */
#line 1478 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5029 "parser.cpp"
    break;

  case 192: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1483 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5040 "parser.cpp"
    break;

  case 193: /* table_alias: %empty  */
#line 1489 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5048 "parser.cpp"
    break;

  case 194: /* with_clause: WITH with_expr_list  */
#line 1496 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5056 "parser.cpp"
    break;

  case 195: /* with_clause: %empty  */
#line 1499 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5064 "parser.cpp"
    break;

  case 196: /* with_expr_list: with_expr  */
#line 1503 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5073 "parser.cpp"
    break;

  case 197: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1506 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5082 "parser.cpp"
    break;

  case 198: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1511 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5094 "parser.cpp"
    break;

  case 199: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1523 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5106 "parser.cpp"
    break;

  case 200: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1530 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5119 "parser.cpp"
    break;

  case 201: /* join_type: INNER  */
#line 1544 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5127 "parser.cpp"
    break;

  case 202: /* join_type: LEFT  */
#line 1547 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5135 "parser.cpp"
    break;

  case 203: /* join_type: RIGHT  */
#line 1550 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5143 "parser.cpp"
    break;

  case 204: /* join_type: OUTER  */
#line 1553 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5151 "parser.cpp"
    break;

  case 205: /* join_type: FULL  */
#line 1556 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5159 "parser.cpp"
    break;

  case 206: /* join_type: CROSS  */
#line 1559 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5167 "parser.cpp"
    break;

  case 207: /* join_type: %empty  */
#line 1562 "parser.y"
                {
}
#line 5174 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW DATABASES  */
#line 1568 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5183 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW TABLES  */
#line 1572 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5192 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW VIEWS  */
#line 1576 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5201 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW CONFIGS  */
#line 1580 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5210 "parser.cpp"
    break;

  case 212: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1584 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5222 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW PROFILES  */
#line 1591 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5231 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW SESSION VARIABLES  */
#line 1595 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5240 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1599 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5249 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1603 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5260 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1609 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5271 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1615 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5282 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW TABLE table_name  */
#line 1621 "parser.y"
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
#line 5298 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1632 "parser.y"
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
#line 5314 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1643 "parser.y"
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
#line 5330 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1654 "parser.y"
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
#line 5347 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1666 "parser.y"
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
#line 5364 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1678 "parser.y"
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
#line 5382 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1691 "parser.y"
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
#line 5401 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1705 "parser.y"
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
#line 5417 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1716 "parser.y"
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
#line 5436 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1730 "parser.y"
                                                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexSegment;
    if((yyvsp[-4].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-4].table_name_t)->schema_name_ptr_;
        free((yyvsp[-4].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-4].table_name_t)->table_name_ptr_;
    free((yyvsp[-4].table_name_t)->table_name_ptr_);
    delete (yyvsp[-4].table_name_t);

    (yyval.show_stmt)->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
}
#line 5457 "parser.cpp"
    break;

  case 229: /* flush_statement: FLUSH DATA  */
#line 1750 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5466 "parser.cpp"
    break;

  case 230: /* flush_statement: FLUSH LOG  */
#line 1754 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5475 "parser.cpp"
    break;

  case 231: /* flush_statement: FLUSH BUFFER  */
#line 1758 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5484 "parser.cpp"
    break;

  case 232: /* optimize_statement: OPTIMIZE table_name  */
#line 1766 "parser.y"
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
#line 5499 "parser.cpp"
    break;

  case 233: /* command_statement: USE IDENTIFIER  */
#line 1780 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5510 "parser.cpp"
    break;

  case 234: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1786 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5520 "parser.cpp"
    break;

  case 235: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1791 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5531 "parser.cpp"
    break;

  case 236: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1797 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5542 "parser.cpp"
    break;

  case 237: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1803 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5555 "parser.cpp"
    break;

  case 238: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1811 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5566 "parser.cpp"
    break;

  case 239: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1817 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5577 "parser.cpp"
    break;

  case 240: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1823 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5588 "parser.cpp"
    break;

  case 241: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1829 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5599 "parser.cpp"
    break;

  case 242: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1835 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5612 "parser.cpp"
    break;

  case 243: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1843 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5623 "parser.cpp"
    break;

  case 244: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1849 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5634 "parser.cpp"
    break;

  case 245: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1855 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5645 "parser.cpp"
    break;

  case 246: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1861 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5656 "parser.cpp"
    break;

  case 247: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1867 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5669 "parser.cpp"
    break;

  case 248: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1875 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5680 "parser.cpp"
    break;

  case 249: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1881 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5691 "parser.cpp"
    break;

  case 250: /* compact_statement: COMPACT TABLE table_name  */
#line 1888 "parser.y"
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
#line 5708 "parser.cpp"
    break;

  case 251: /* expr_array: expr_alias  */
#line 1905 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5717 "parser.cpp"
    break;

  case 252: /* expr_array: expr_array ',' expr_alias  */
#line 1909 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5726 "parser.cpp"
    break;

  case 253: /* expr_array_list: '(' expr_array ')'  */
#line 1914 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5735 "parser.cpp"
    break;

  case 254: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1918 "parser.y"
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
#line 5755 "parser.cpp"
    break;

  case 255: /* expr_alias: expr AS IDENTIFIER  */
#line 1945 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5766 "parser.cpp"
    break;

  case 256: /* expr_alias: expr  */
#line 1951 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5774 "parser.cpp"
    break;

  case 262: /* operand: '(' expr ')'  */
#line 1961 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5782 "parser.cpp"
    break;

  case 263: /* operand: '(' select_without_paren ')'  */
#line 1964 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5793 "parser.cpp"
    break;

  case 264: /* operand: constant_expr  */
#line 1970 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5801 "parser.cpp"
    break;

  case 274: /* extra_match_tensor_option: ',' STRING  */
#line 1983 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5809 "parser.cpp"
    break;

  case 275: /* extra_match_tensor_option: %empty  */
#line 1986 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5817 "parser.cpp"
    break;

  case 276: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 1992 "parser.y"
                                                                                                                           {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-8].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-2].str_value));
    // search options
    if ((yyvsp[-1].str_value)) {
        match_tensor_expr->SetExtraOptions((yyvsp[-1].str_value));
    }
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 5838 "parser.cpp"
    break;

  case 277: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2011 "parser.y"
                                                                                                                        {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // KNN search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-4].str_value));
    if (!check) {
        goto Error;
    }

    // KNN data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    if (!check) {
        goto Error;
    }
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return;
Error:
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
Return:
    ;
}
#line 5884 "parser.cpp"
    break;

  case 278: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2053 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5897 "parser.cpp"
    break;

  case 279: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2061 "parser.y"
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
#line 5912 "parser.cpp"
    break;

  case 280: /* query_expr: QUERY '(' STRING ')'  */
#line 2072 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5923 "parser.cpp"
    break;

  case 281: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2078 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5936 "parser.cpp"
    break;

  case 282: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2087 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5947 "parser.cpp"
    break;

  case 283: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2093 "parser.y"
                                   {
    auto fusion_expr = std::make_unique<infinity::FusionExpr>();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    (yyvsp[-3].str_value) = nullptr;
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyvsp[-1].str_value) = nullptr;
    fusion_expr->JobAfterParser();
    (yyval.expr_t) = fusion_expr.release();
}
#line 5963 "parser.cpp"
    break;

  case 284: /* sub_search_array: match_vector_expr  */
#line 2106 "parser.y"
                                     {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5972 "parser.cpp"
    break;

  case 285: /* sub_search_array: match_text_expr  */
#line 2110 "parser.y"
                  {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5981 "parser.cpp"
    break;

  case 286: /* sub_search_array: match_tensor_expr  */
#line 2114 "parser.y"
                    {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5990 "parser.cpp"
    break;

  case 287: /* sub_search_array: query_expr  */
#line 2118 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5999 "parser.cpp"
    break;

  case 288: /* sub_search_array: fusion_expr  */
#line 2122 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6008 "parser.cpp"
    break;

  case 289: /* sub_search_array: sub_search_array ',' match_vector_expr  */
#line 2126 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6017 "parser.cpp"
    break;

  case 290: /* sub_search_array: sub_search_array ',' match_text_expr  */
#line 2130 "parser.y"
                                       {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6026 "parser.cpp"
    break;

  case 291: /* sub_search_array: sub_search_array ',' match_tensor_expr  */
#line 2134 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6035 "parser.cpp"
    break;

  case 292: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2138 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6044 "parser.cpp"
    break;

  case 293: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2142 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6053 "parser.cpp"
    break;

  case 294: /* function_expr: IDENTIFIER '(' ')'  */
#line 2147 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6066 "parser.cpp"
    break;

  case 295: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2155 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6079 "parser.cpp"
    break;

  case 296: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2163 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6093 "parser.cpp"
    break;

  case 297: /* function_expr: operand IS NOT NULLABLE  */
#line 2172 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6105 "parser.cpp"
    break;

  case 298: /* function_expr: operand IS NULLABLE  */
#line 2179 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6117 "parser.cpp"
    break;

  case 299: /* function_expr: NOT operand  */
#line 2186 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6129 "parser.cpp"
    break;

  case 300: /* function_expr: '-' operand  */
#line 2193 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6141 "parser.cpp"
    break;

  case 301: /* function_expr: '+' operand  */
#line 2200 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6153 "parser.cpp"
    break;

  case 302: /* function_expr: operand '-' operand  */
#line 2207 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6166 "parser.cpp"
    break;

  case 303: /* function_expr: operand '+' operand  */
#line 2215 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6179 "parser.cpp"
    break;

  case 304: /* function_expr: operand '*' operand  */
#line 2223 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6192 "parser.cpp"
    break;

  case 305: /* function_expr: operand '/' operand  */
#line 2231 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6205 "parser.cpp"
    break;

  case 306: /* function_expr: operand '%' operand  */
#line 2239 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6218 "parser.cpp"
    break;

  case 307: /* function_expr: operand '=' operand  */
#line 2247 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6231 "parser.cpp"
    break;

  case 308: /* function_expr: operand EQUAL operand  */
#line 2255 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6244 "parser.cpp"
    break;

  case 309: /* function_expr: operand NOT_EQ operand  */
#line 2263 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6257 "parser.cpp"
    break;

  case 310: /* function_expr: operand '<' operand  */
#line 2271 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6270 "parser.cpp"
    break;

  case 311: /* function_expr: operand '>' operand  */
#line 2279 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6283 "parser.cpp"
    break;

  case 312: /* function_expr: operand LESS_EQ operand  */
#line 2287 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6296 "parser.cpp"
    break;

  case 313: /* function_expr: operand GREATER_EQ operand  */
#line 2295 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6309 "parser.cpp"
    break;

  case 314: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2303 "parser.y"
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
#line 6344 "parser.cpp"
    break;

  case 315: /* function_expr: operand LIKE operand  */
#line 2333 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6357 "parser.cpp"
    break;

  case 316: /* function_expr: operand NOT LIKE operand  */
#line 2341 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6370 "parser.cpp"
    break;

  case 317: /* conjunction_expr: expr AND expr  */
#line 2350 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6383 "parser.cpp"
    break;

  case 318: /* conjunction_expr: expr OR expr  */
#line 2358 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6396 "parser.cpp"
    break;

  case 319: /* between_expr: operand BETWEEN operand AND operand  */
#line 2367 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6408 "parser.cpp"
    break;

  case 320: /* in_expr: operand IN '(' expr_array ')'  */
#line 2375 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6419 "parser.cpp"
    break;

  case 321: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2381 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6430 "parser.cpp"
    break;

  case 322: /* case_expr: CASE expr case_check_array END  */
#line 2388 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6441 "parser.cpp"
    break;

  case 323: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2394 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6453 "parser.cpp"
    break;

  case 324: /* case_expr: CASE case_check_array END  */
#line 2401 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6463 "parser.cpp"
    break;

  case 325: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2406 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6474 "parser.cpp"
    break;

  case 326: /* case_check_array: WHEN expr THEN expr  */
#line 2413 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6486 "parser.cpp"
    break;

  case 327: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2420 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6498 "parser.cpp"
    break;

  case 328: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2428 "parser.y"
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
#line 6526 "parser.cpp"
    break;

  case 329: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2452 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6537 "parser.cpp"
    break;

  case 330: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2458 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6548 "parser.cpp"
    break;

  case 331: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2464 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6560 "parser.cpp"
    break;

  case 332: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2471 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6572 "parser.cpp"
    break;

  case 333: /* column_expr: IDENTIFIER  */
#line 2479 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6584 "parser.cpp"
    break;

  case 334: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2486 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6596 "parser.cpp"
    break;

  case 335: /* column_expr: '*'  */
#line 2493 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6606 "parser.cpp"
    break;

  case 336: /* column_expr: column_expr '.' '*'  */
#line 2498 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6620 "parser.cpp"
    break;

  case 337: /* constant_expr: STRING  */
#line 2508 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6630 "parser.cpp"
    break;

  case 338: /* constant_expr: TRUE  */
#line 2513 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6640 "parser.cpp"
    break;

  case 339: /* constant_expr: FALSE  */
#line 2518 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6650 "parser.cpp"
    break;

  case 340: /* constant_expr: DOUBLE_VALUE  */
#line 2523 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6660 "parser.cpp"
    break;

  case 341: /* constant_expr: LONG_VALUE  */
#line 2528 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6670 "parser.cpp"
    break;

  case 342: /* constant_expr: DATE STRING  */
#line 2533 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6680 "parser.cpp"
    break;

  case 343: /* constant_expr: TIME STRING  */
#line 2538 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6690 "parser.cpp"
    break;

  case 344: /* constant_expr: DATETIME STRING  */
#line 2543 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6700 "parser.cpp"
    break;

  case 345: /* constant_expr: TIMESTAMP STRING  */
#line 2548 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6710 "parser.cpp"
    break;

  case 346: /* constant_expr: INTERVAL interval_expr  */
#line 2553 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6718 "parser.cpp"
    break;

  case 347: /* constant_expr: interval_expr  */
#line 2556 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6726 "parser.cpp"
    break;

  case 348: /* constant_expr: common_array_expr  */
#line 2559 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6734 "parser.cpp"
    break;

  case 349: /* common_array_expr: array_expr  */
#line 2563 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6742 "parser.cpp"
    break;

  case 350: /* common_array_expr: subarray_array_expr  */
#line 2566 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6750 "parser.cpp"
    break;

  case 351: /* common_array_expr: sparse_array_expr  */
#line 2569 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6758 "parser.cpp"
    break;

  case 352: /* common_array_expr: empty_array_expr  */
#line 2572 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6766 "parser.cpp"
    break;

  case 353: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2576 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6774 "parser.cpp"
    break;

  case 354: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2580 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 6784 "parser.cpp"
    break;

  case 355: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2585 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6793 "parser.cpp"
    break;

  case 356: /* sparse_array_expr: long_sparse_array_expr  */
#line 2590 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6801 "parser.cpp"
    break;

  case 357: /* sparse_array_expr: double_sparse_array_expr  */
#line 2593 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6809 "parser.cpp"
    break;

  case 358: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2597 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6817 "parser.cpp"
    break;

  case 359: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2601 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 6829 "parser.cpp"
    break;

  case 360: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2608 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6840 "parser.cpp"
    break;

  case 361: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2615 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6848 "parser.cpp"
    break;

  case 362: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2619 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 6860 "parser.cpp"
    break;

  case 363: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2626 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6871 "parser.cpp"
    break;

  case 364: /* empty_array_expr: '[' ']'  */
#line 2633 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 6879 "parser.cpp"
    break;

  case 365: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 2637 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 6887 "parser.cpp"
    break;

  case 366: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 2641 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 6895 "parser.cpp"
    break;

  case 367: /* array_expr: long_array_expr  */
#line 2645 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6903 "parser.cpp"
    break;

  case 368: /* array_expr: double_array_expr  */
#line 2648 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6911 "parser.cpp"
    break;

  case 369: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2652 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6919 "parser.cpp"
    break;

  case 370: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2656 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6929 "parser.cpp"
    break;

  case 371: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2661 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6938 "parser.cpp"
    break;

  case 372: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2666 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6946 "parser.cpp"
    break;

  case 373: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2670 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6956 "parser.cpp"
    break;

  case 374: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2675 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6965 "parser.cpp"
    break;

  case 375: /* interval_expr: LONG_VALUE SECONDS  */
#line 2680 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6976 "parser.cpp"
    break;

  case 376: /* interval_expr: LONG_VALUE SECOND  */
#line 2686 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6987 "parser.cpp"
    break;

  case 377: /* interval_expr: LONG_VALUE MINUTES  */
#line 2692 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6998 "parser.cpp"
    break;

  case 378: /* interval_expr: LONG_VALUE MINUTE  */
#line 2698 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7009 "parser.cpp"
    break;

  case 379: /* interval_expr: LONG_VALUE HOURS  */
#line 2704 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7020 "parser.cpp"
    break;

  case 380: /* interval_expr: LONG_VALUE HOUR  */
#line 2710 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7031 "parser.cpp"
    break;

  case 381: /* interval_expr: LONG_VALUE DAYS  */
#line 2716 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7042 "parser.cpp"
    break;

  case 382: /* interval_expr: LONG_VALUE DAY  */
#line 2722 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7053 "parser.cpp"
    break;

  case 383: /* interval_expr: LONG_VALUE MONTHS  */
#line 2728 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7064 "parser.cpp"
    break;

  case 384: /* interval_expr: LONG_VALUE MONTH  */
#line 2734 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7075 "parser.cpp"
    break;

  case 385: /* interval_expr: LONG_VALUE YEARS  */
#line 2740 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7086 "parser.cpp"
    break;

  case 386: /* interval_expr: LONG_VALUE YEAR  */
#line 2746 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7097 "parser.cpp"
    break;

  case 387: /* copy_option_list: copy_option  */
#line 2757 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7106 "parser.cpp"
    break;

  case 388: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2761 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7115 "parser.cpp"
    break;

  case 389: /* copy_option: FORMAT IDENTIFIER  */
#line 2766 "parser.y"
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
    } else if (strcasecmp((yyvsp[0].str_value), "csr") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kCSR;
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 7145 "parser.cpp"
    break;

  case 390: /* copy_option: DELIMITER STRING  */
#line 2791 "parser.y"
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
#line 7160 "parser.cpp"
    break;

  case 391: /* copy_option: HEADER  */
#line 2801 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7170 "parser.cpp"
    break;

  case 392: /* file_path: STRING  */
#line 2807 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7178 "parser.cpp"
    break;

  case 393: /* if_exists: IF EXISTS  */
#line 2811 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7184 "parser.cpp"
    break;

  case 394: /* if_exists: %empty  */
#line 2812 "parser.y"
  { (yyval.bool_value) = false; }
#line 7190 "parser.cpp"
    break;

  case 395: /* if_not_exists: IF NOT EXISTS  */
#line 2814 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7196 "parser.cpp"
    break;

  case 396: /* if_not_exists: %empty  */
#line 2815 "parser.y"
  { (yyval.bool_value) = false; }
#line 7202 "parser.cpp"
    break;

  case 399: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2830 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7215 "parser.cpp"
    break;

  case 400: /* if_not_exists_info: %empty  */
#line 2838 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7223 "parser.cpp"
    break;

  case 401: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2842 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7231 "parser.cpp"
    break;

  case 402: /* with_index_param_list: %empty  */
#line 2845 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7239 "parser.cpp"
    break;

  case 403: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2849 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7247 "parser.cpp"
    break;

  case 404: /* optional_table_properties_list: %empty  */
#line 2852 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7255 "parser.cpp"
    break;

  case 405: /* index_param_list: index_param  */
#line 2856 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7264 "parser.cpp"
    break;

  case 406: /* index_param_list: index_param_list ',' index_param  */
#line 2860 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7273 "parser.cpp"
    break;

  case 407: /* index_param: IDENTIFIER  */
#line 2865 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7283 "parser.cpp"
    break;

  case 408: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2870 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7296 "parser.cpp"
    break;

  case 409: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2878 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7308 "parser.cpp"
    break;

  case 410: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2885 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7320 "parser.cpp"
    break;

  case 411: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2896 "parser.y"
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
#line 7373 "parser.cpp"
    break;

  case 412: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2944 "parser.y"
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
#line 7427 "parser.cpp"
    break;

  case 413: /* index_info_list: '(' identifier_array ')'  */
#line 2993 "parser.y"
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
#line 7445 "parser.cpp"
    break;


#line 7449 "parser.cpp"

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

#line 3007 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
  YYSYMBOL_CHUNK = 53,                     /* CHUNK  */
  YYSYMBOL_GROUP = 54,                     /* GROUP  */
  YYSYMBOL_BY = 55,                        /* BY  */
  YYSYMBOL_HAVING = 56,                    /* HAVING  */
  YYSYMBOL_AS = 57,                        /* AS  */
  YYSYMBOL_NATURAL = 58,                   /* NATURAL  */
  YYSYMBOL_JOIN = 59,                      /* JOIN  */
  YYSYMBOL_LEFT = 60,                      /* LEFT  */
  YYSYMBOL_RIGHT = 61,                     /* RIGHT  */
  YYSYMBOL_OUTER = 62,                     /* OUTER  */
  YYSYMBOL_FULL = 63,                      /* FULL  */
  YYSYMBOL_ON = 64,                        /* ON  */
  YYSYMBOL_INNER = 65,                     /* INNER  */
  YYSYMBOL_CROSS = 66,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 67,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 68,                     /* WHERE  */
  YYSYMBOL_ORDER = 69,                     /* ORDER  */
  YYSYMBOL_LIMIT = 70,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 71,                    /* OFFSET  */
  YYSYMBOL_ASC = 72,                       /* ASC  */
  YYSYMBOL_DESC = 73,                      /* DESC  */
  YYSYMBOL_IF = 74,                        /* IF  */
  YYSYMBOL_NOT = 75,                       /* NOT  */
  YYSYMBOL_EXISTS = 76,                    /* EXISTS  */
  YYSYMBOL_IN = 77,                        /* IN  */
  YYSYMBOL_FROM = 78,                      /* FROM  */
  YYSYMBOL_TO = 79,                        /* TO  */
  YYSYMBOL_WITH = 80,                      /* WITH  */
  YYSYMBOL_DELIMITER = 81,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 82,                    /* FORMAT  */
  YYSYMBOL_HEADER = 83,                    /* HEADER  */
  YYSYMBOL_CAST = 84,                      /* CAST  */
  YYSYMBOL_END = 85,                       /* END  */
  YYSYMBOL_CASE = 86,                      /* CASE  */
  YYSYMBOL_ELSE = 87,                      /* ELSE  */
  YYSYMBOL_THEN = 88,                      /* THEN  */
  YYSYMBOL_WHEN = 89,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 90,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 91,                   /* INTEGER  */
  YYSYMBOL_INT = 92,                       /* INT  */
  YYSYMBOL_TINYINT = 93,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 94,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 95,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 96,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 97,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 98,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 99,                    /* DOUBLE  */
  YYSYMBOL_REAL = 100,                     /* REAL  */
  YYSYMBOL_DECIMAL = 101,                  /* DECIMAL  */
  YYSYMBOL_DATE = 102,                     /* DATE  */
  YYSYMBOL_TIME = 103,                     /* TIME  */
  YYSYMBOL_DATETIME = 104,                 /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 105,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 106,                     /* UUID  */
  YYSYMBOL_POINT = 107,                    /* POINT  */
  YYSYMBOL_LINE = 108,                     /* LINE  */
  YYSYMBOL_LSEG = 109,                     /* LSEG  */
  YYSYMBOL_BOX = 110,                      /* BOX  */
  YYSYMBOL_PATH = 111,                     /* PATH  */
  YYSYMBOL_POLYGON = 112,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 113,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 114,                     /* BLOB  */
  YYSYMBOL_BITMAP = 115,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 116,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 117,                   /* VECTOR  */
  YYSYMBOL_BIT = 118,                      /* BIT  */
  YYSYMBOL_TEXT = 119,                     /* TEXT  */
  YYSYMBOL_TENSOR = 120,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 121,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 122,              /* TENSORARRAY  */
  YYSYMBOL_PRIMARY = 123,                  /* PRIMARY  */
  YYSYMBOL_KEY = 124,                      /* KEY  */
  YYSYMBOL_UNIQUE = 125,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 126,                 /* NULLABLE  */
  YYSYMBOL_IS = 127,                       /* IS  */
  YYSYMBOL_DEFAULT = 128,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 129,                     /* TRUE  */
  YYSYMBOL_FALSE = 130,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 131,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 132,                   /* SECOND  */
  YYSYMBOL_SECONDS = 133,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 134,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 135,                  /* MINUTES  */
  YYSYMBOL_HOUR = 136,                     /* HOUR  */
  YYSYMBOL_HOURS = 137,                    /* HOURS  */
  YYSYMBOL_DAY = 138,                      /* DAY  */
  YYSYMBOL_DAYS = 139,                     /* DAYS  */
  YYSYMBOL_MONTH = 140,                    /* MONTH  */
  YYSYMBOL_MONTHS = 141,                   /* MONTHS  */
  YYSYMBOL_YEAR = 142,                     /* YEAR  */
  YYSYMBOL_YEARS = 143,                    /* YEARS  */
  YYSYMBOL_EQUAL = 144,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 145,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 146,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 147,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 148,                  /* BETWEEN  */
  YYSYMBOL_AND = 149,                      /* AND  */
  YYSYMBOL_OR = 150,                       /* OR  */
  YYSYMBOL_EXTRACT = 151,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 152,                     /* LIKE  */
  YYSYMBOL_DATA = 153,                     /* DATA  */
  YYSYMBOL_LOG = 154,                      /* LOG  */
  YYSYMBOL_BUFFER = 155,                   /* BUFFER  */
  YYSYMBOL_KNN = 156,                      /* KNN  */
  YYSYMBOL_USING = 157,                    /* USING  */
  YYSYMBOL_SESSION = 158,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 159,                   /* GLOBAL  */
  YYSYMBOL_OFF = 160,                      /* OFF  */
  YYSYMBOL_EXPORT = 161,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 162,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 163,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 164,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 165,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 166,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 167,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 168,                   /* SEARCH  */
  YYSYMBOL_MATCH = 169,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 170,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 171,                    /* QUERY  */
  YYSYMBOL_FUSION = 172,                   /* FUSION  */
  YYSYMBOL_NUMBER = 173,                   /* NUMBER  */
  YYSYMBOL_174_ = 174,                     /* '='  */
  YYSYMBOL_175_ = 175,                     /* '<'  */
  YYSYMBOL_176_ = 176,                     /* '>'  */
  YYSYMBOL_177_ = 177,                     /* '+'  */
  YYSYMBOL_178_ = 178,                     /* '-'  */
  YYSYMBOL_179_ = 179,                     /* '*'  */
  YYSYMBOL_180_ = 180,                     /* '/'  */
  YYSYMBOL_181_ = 181,                     /* '%'  */
  YYSYMBOL_182_ = 182,                     /* '['  */
  YYSYMBOL_183_ = 183,                     /* ']'  */
  YYSYMBOL_184_ = 184,                     /* '('  */
  YYSYMBOL_185_ = 185,                     /* ')'  */
  YYSYMBOL_186_ = 186,                     /* '.'  */
  YYSYMBOL_187_ = 187,                     /* ';'  */
  YYSYMBOL_188_ = 188,                     /* ','  */
  YYSYMBOL_189_ = 189,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 190,                 /* $accept  */
  YYSYMBOL_input_pattern = 191,            /* input_pattern  */
  YYSYMBOL_statement_list = 192,           /* statement_list  */
  YYSYMBOL_statement = 193,                /* statement  */
  YYSYMBOL_explainable_statement = 194,    /* explainable_statement  */
  YYSYMBOL_create_statement = 195,         /* create_statement  */
  YYSYMBOL_table_element_array = 196,      /* table_element_array  */
  YYSYMBOL_table_element = 197,            /* table_element  */
  YYSYMBOL_table_column = 198,             /* table_column  */
  YYSYMBOL_column_type = 199,              /* column_type  */
  YYSYMBOL_column_constraints = 200,       /* column_constraints  */
  YYSYMBOL_column_constraint = 201,        /* column_constraint  */
  YYSYMBOL_default_expr = 202,             /* default_expr  */
  YYSYMBOL_table_constraint = 203,         /* table_constraint  */
  YYSYMBOL_identifier_array = 204,         /* identifier_array  */
  YYSYMBOL_delete_statement = 205,         /* delete_statement  */
  YYSYMBOL_insert_statement = 206,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 207, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 208,        /* explain_statement  */
  YYSYMBOL_explain_type = 209,             /* explain_type  */
  YYSYMBOL_update_statement = 210,         /* update_statement  */
  YYSYMBOL_update_expr_array = 211,        /* update_expr_array  */
  YYSYMBOL_update_expr = 212,              /* update_expr  */
  YYSYMBOL_drop_statement = 213,           /* drop_statement  */
  YYSYMBOL_copy_statement = 214,           /* copy_statement  */
  YYSYMBOL_select_statement = 215,         /* select_statement  */
  YYSYMBOL_select_with_paren = 216,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 217,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 218, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 219, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 220, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 221,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 222,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 223,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 224,            /* order_by_type  */
  YYSYMBOL_limit_expr = 225,               /* limit_expr  */
  YYSYMBOL_offset_expr = 226,              /* offset_expr  */
  YYSYMBOL_distinct = 227,                 /* distinct  */
  YYSYMBOL_from_clause = 228,              /* from_clause  */
  YYSYMBOL_search_clause = 229,            /* search_clause  */
  YYSYMBOL_where_clause = 230,             /* where_clause  */
  YYSYMBOL_having_clause = 231,            /* having_clause  */
  YYSYMBOL_group_by_clause = 232,          /* group_by_clause  */
  YYSYMBOL_set_operator = 233,             /* set_operator  */
  YYSYMBOL_table_reference = 234,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 235,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 236,     /* table_reference_name  */
  YYSYMBOL_table_name = 237,               /* table_name  */
  YYSYMBOL_table_alias = 238,              /* table_alias  */
  YYSYMBOL_with_clause = 239,              /* with_clause  */
  YYSYMBOL_with_expr_list = 240,           /* with_expr_list  */
  YYSYMBOL_with_expr = 241,                /* with_expr  */
  YYSYMBOL_join_clause = 242,              /* join_clause  */
  YYSYMBOL_join_type = 243,                /* join_type  */
  YYSYMBOL_show_statement = 244,           /* show_statement  */
  YYSYMBOL_flush_statement = 245,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 246,       /* optimize_statement  */
  YYSYMBOL_command_statement = 247,        /* command_statement  */
  YYSYMBOL_compact_statement = 248,        /* compact_statement  */
  YYSYMBOL_expr_array = 249,               /* expr_array  */
  YYSYMBOL_expr_array_list = 250,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 251,               /* expr_alias  */
  YYSYMBOL_expr = 252,                     /* expr  */
  YYSYMBOL_operand = 253,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 254, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 255,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 256,        /* match_vector_expr  */
  YYSYMBOL_match_text_expr = 257,          /* match_text_expr  */
  YYSYMBOL_query_expr = 258,               /* query_expr  */
  YYSYMBOL_fusion_expr = 259,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 260,         /* sub_search_array  */
  YYSYMBOL_function_expr = 261,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 262,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 263,             /* between_expr  */
  YYSYMBOL_in_expr = 264,                  /* in_expr  */
  YYSYMBOL_case_expr = 265,                /* case_expr  */
  YYSYMBOL_case_check_array = 266,         /* case_check_array  */
  YYSYMBOL_cast_expr = 267,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 268,            /* subquery_expr  */
  YYSYMBOL_column_expr = 269,              /* column_expr  */
  YYSYMBOL_constant_expr = 270,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 271,        /* common_array_expr  */
  YYSYMBOL_subarray_array_expr = 272,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 273, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 274,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 275,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 276, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 277, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 278, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 279,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 280,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 281,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 282,               /* array_expr  */
  YYSYMBOL_long_array_expr = 283,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 284, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 285,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 286, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 287,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 288,         /* copy_option_list  */
  YYSYMBOL_copy_option = 289,              /* copy_option  */
  YYSYMBOL_file_path = 290,                /* file_path  */
  YYSYMBOL_if_exists = 291,                /* if_exists  */
  YYSYMBOL_if_not_exists = 292,            /* if_not_exists  */
  YYSYMBOL_semicolon = 293,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 294,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 295,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 296, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 297,         /* index_param_list  */
  YYSYMBOL_index_param = 298,              /* index_param  */
  YYSYMBOL_index_info_list = 299           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 428 "parser.cpp"

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
#define YYLAST   1046

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  190
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  110
/* YYNRULES -- Number of rules.  */
#define YYNRULES  414
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  859

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   428


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
       2,     2,     2,     2,     2,     2,     2,   181,     2,     2,
     184,   185,   179,   177,   188,   178,   186,   180,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   189,   187,
     175,   174,   176,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   182,     2,   183,     2,     2,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171,   172,   173
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
    1632,  1643,  1654,  1666,  1678,  1691,  1705,  1716,  1730,  1746,
    1767,  1771,  1775,  1783,  1797,  1803,  1808,  1814,  1820,  1828,
    1834,  1840,  1846,  1852,  1860,  1866,  1872,  1878,  1884,  1892,
    1898,  1905,  1922,  1926,  1931,  1935,  1962,  1968,  1972,  1973,
    1974,  1975,  1976,  1978,  1981,  1987,  1990,  1991,  1992,  1993,
    1994,  1995,  1996,  1997,  1998,  2000,  2003,  2009,  2028,  2070,
    2078,  2089,  2095,  2104,  2110,  2123,  2127,  2131,  2135,  2139,
    2143,  2147,  2151,  2155,  2159,  2164,  2172,  2180,  2189,  2196,
    2203,  2210,  2217,  2224,  2232,  2240,  2248,  2256,  2264,  2272,
    2280,  2288,  2296,  2304,  2312,  2320,  2350,  2358,  2367,  2375,
    2384,  2392,  2398,  2405,  2411,  2418,  2423,  2430,  2437,  2445,
    2469,  2475,  2481,  2488,  2496,  2503,  2510,  2515,  2525,  2530,
    2535,  2540,  2545,  2550,  2555,  2560,  2565,  2570,  2573,  2576,
    2580,  2583,  2586,  2589,  2593,  2597,  2602,  2607,  2610,  2614,
    2618,  2625,  2632,  2636,  2643,  2650,  2654,  2658,  2662,  2665,
    2669,  2673,  2678,  2683,  2687,  2692,  2697,  2703,  2709,  2715,
    2721,  2727,  2733,  2739,  2745,  2751,  2757,  2763,  2774,  2778,
    2783,  2808,  2818,  2824,  2828,  2829,  2831,  2832,  2834,  2835,
    2847,  2855,  2859,  2862,  2866,  2869,  2873,  2877,  2882,  2887,
    2895,  2902,  2913,  2961,  3010
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
  "CHUNK", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT",
  "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE",
  "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN",
  "FROM", "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END",
  "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
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

#define YYPACT_NINF (-757)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-402)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     573,   214,    24,   259,    58,    -9,    58,   -83,   427,   133,
     107,   197,   140,    58,   164,    56,   -46,   179,    -1,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,   304,  -757,  -757,
     226,  -757,  -757,  -757,  -757,  -757,   142,   142,   142,   142,
       4,    58,   162,   162,   162,   162,   162,    60,   235,    58,
     126,   255,   261,   280,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,   702,   283,    58,  -757,  -757,  -757,   137,   169,  -757,
     290,  -757,    58,  -757,  -757,  -757,  -757,  -757,   238,    66,
    -757,   316,   145,   207,  -757,    15,  -757,   376,  -757,  -757,
      11,   334,  -757,   342,   340,   414,    58,    58,    58,   418,
     386,   284,   359,   469,    58,    58,    58,   488,   489,   495,
     405,   498,   498,    57,    61,    68,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,   304,  -757,  -757,  -757,  -757,  -757,  -757,
     296,  -757,   500,  -757,   501,  -757,  -757,   325,   164,   498,
    -757,  -757,  -757,  -757,    11,  -757,  -757,  -757,   385,   467,
     457,   464,  -757,   -13,  -757,   284,  -757,    58,   530,    17,
    -757,  -757,  -757,  -757,  -757,   477,  -757,   369,   -54,  -757,
     385,  -757,  -757,   466,   468,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
     546,   544,  -757,  -757,  -757,  -757,  -757,   226,  -757,  -757,
     366,   367,   374,  -757,  -757,   714,   490,   375,   377,   269,
     549,   551,   556,   564,  -757,  -757,   566,   389,   -11,   391,
     393,   526,   526,  -757,    31,   328,   -60,  -757,    20,   470,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,   384,  -757,  -757,  -757,   -92,  -757,  -757,   -90,
    -757,    41,  -757,  -757,  -757,    44,  -757,    64,  -757,   385,
     385,   507,  -757,   -46,    43,   522,   397,  -757,   -69,   406,
    -757,    58,   385,   495,  -757,   191,   407,   419,   558,   308,
     417,  -757,  -757,   136,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,   526,   421,   651,   527,
     385,   385,    25,   240,  -757,  -757,  -757,  -757,   714,  -757,
     602,   424,   425,   429,   607,   619,   275,   275,  -757,   435,
    -757,  -757,  -757,  -757,   440,    52,     9,   385,   458,   624,
     385,   385,   -30,   448,   -26,   526,   526,   526,   526,   526,
     526,   526,   526,   526,   526,   526,   526,   526,   526,    18,
    -757,   451,  -757,   628,  -757,   629,  -757,   630,  -757,   632,
     450,  -757,   -14,   191,   385,  -757,   304,   774,   515,   456,
      91,  -757,  -757,  -757,   -46,   530,   459,  -757,   639,   385,
     472,  -757,   191,  -757,   403,   403,   646,   648,  -757,  -757,
     385,  -757,    92,   527,   506,   478,    23,   -23,   320,  -757,
     385,   385,   586,   385,   661,    40,    97,   114,   371,  -757,
    -757,   -46,   483,   302,  -757,   130,  -757,  -757,    54,   405,
    -757,  -757,   517,   491,   526,   328,   547,  -757,   664,   664,
     248,   248,   609,   664,   664,   248,   248,   275,   275,  -757,
    -757,  -757,  -757,  -757,  -757,   487,  -757,   496,  -757,  -757,
    -757,   385,  -757,  -757,  -757,   191,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,   494,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,   504,   505,
     508,   509,   510,   125,   512,   530,   662,    43,   304,   131,
     530,  -757,   161,   516,   687,   696,  -757,   181,  -757,   208,
     649,   657,   228,  -757,   532,  -757,   774,   385,  -757,   385,
     -62,    35,   526,   -64,   513,  -757,   -94,  -757,   715,  -757,
     716,  -757,  -757,    30,     9,   665,  -757,  -757,  -757,  -757,
    -757,  -757,   671,  -757,   718,  -757,  -757,  -757,  -757,  -757,
    -757,   535,   677,   328,   664,   548,   234,  -757,   526,   726,
     730,  -757,   731,   288,   383,   426,   588,   806,   612,   620,
    -757,  -757,    78,   125,  -757,  -757,   530,   249,   559,  -757,
    -757,   589,   251,  -757,   385,  -757,  -757,  -757,   403,  -757,
     739,   742,  -757,  -757,   565,   191,   -32,  -757,   385,   595,
     567,   747,   451,   574,   575,   130,   302,     9,     9,   568,
      54,   707,   708,   578,   257,  -757,  -757,   651,   264,   577,
     579,   580,   591,   593,   604,   605,   606,   611,   613,   614,
     616,   617,   618,   625,   626,   627,   631,   633,   634,   635,
     636,   645,   647,   670,   672,   673,   674,   697,   700,   701,
     704,   705,   719,   720,   721,   722,   723,   724,   725,  -757,
    -757,  -757,  -757,  -757,   295,  -757,   763,   797,   650,   323,
    -757,  -757,  -757,  -757,  -757,   191,  -757,   379,   727,   338,
     728,  -757,  -757,  -757,  -757,   713,   530,  -757,  -757,  -757,
    -757,  -757,   385,   385,  -757,  -757,  -757,  -757,   811,   812,
     814,   828,   830,   831,   853,   896,   897,   900,   908,   911,
     912,   913,   914,   915,   916,   917,   919,   920,   921,   922,
     923,   924,   925,   926,   927,   928,   929,   930,   931,   932,
     933,   934,   935,   936,   937,   938,   939,   940,   941,  -757,
     638,   349,  -757,   868,   946,  -757,  -757,   947,  -757,   948,
     949,   385,   350,   762,   191,   769,   770,   771,   772,   773,
     775,   776,   777,   778,   779,   780,   781,   782,   783,   784,
     785,   786,   787,   788,   789,   790,   791,   792,   793,   794,
     795,   796,   798,   799,   800,   801,   802,   803,   804,   805,
     807,   808,   809,   810,   813,   815,   256,  -757,   763,   817,
    -757,   868,   816,   818,   819,   191,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
    -757,  -757,   763,  -757,   955,  -757,   978,   354,   820,   821,
    -757,   985,   992,   825,   826,  -757,  -757,   868,  -757
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     195,     0,     0,     0,     0,     0,     0,     0,   131,     0,
       0,     0,     0,     0,     0,     0,   195,     0,   399,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   144,   143,
       0,     8,    14,    15,    16,    17,   397,   397,   397,   397,
     397,     0,   395,   395,   395,   395,   395,   188,     0,     0,
       0,     0,     0,     0,   125,   129,   126,   127,   128,   130,
     124,   195,     0,     0,   209,   210,   208,     0,     0,   211,
       0,   213,     0,   230,   231,   232,   234,   233,     0,   194,
     196,     0,     0,     0,     1,   195,     2,   178,   180,   181,
       0,   167,   149,   155,     0,     0,     0,     0,     0,     0,
       0,   122,     0,     0,     0,     0,     0,     0,     0,     0,
     173,     0,     0,     0,     0,     0,   123,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   218,
     219,   214,     0,   215,     0,   212,   251,     0,     0,     0,
     148,   147,     4,   179,     0,   145,   146,   166,     0,     0,
     163,     0,    30,     0,    31,   122,   400,     0,     0,   195,
     394,   136,   138,   137,   139,     0,   189,     0,   173,   133,
       0,   118,   393,     0,     0,   238,   240,   239,   236,   237,
     243,   245,   244,   241,   242,   248,   250,   249,   246,   247,
       0,     0,   221,   220,   226,   216,   217,     0,   197,   235,
       0,     0,   334,   338,   341,   342,     0,     0,     0,     0,
       0,     0,     0,     0,   339,   340,     0,     0,     0,     0,
       0,     0,     0,   336,     0,   195,   169,   252,   257,   258,
     272,   270,   271,   273,   274,   267,   262,   261,   260,   268,
     269,   259,   266,   265,   349,   351,     0,   352,   357,     0,
     358,     0,   353,   350,   368,     0,   369,     0,   348,     0,
       0,   165,   396,   195,     0,     0,     0,   116,     0,     0,
     120,     0,     0,     0,   132,   172,     0,     0,   227,   222,
       0,   152,   151,     0,   377,   376,   379,   378,   381,   380,
     383,   382,   385,   384,   387,   386,     0,     0,   300,   195,
       0,     0,     0,     0,   343,   344,   345,   346,     0,   347,
       0,     0,     0,     0,     0,     0,   302,   301,   374,   371,
     365,   355,   360,   363,     0,     0,     0,     0,   171,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     354,     0,   359,     0,   362,     0,   370,     0,   373,     0,
     154,   156,   161,   162,     0,   150,    33,     0,     0,     0,
       0,    36,    38,    39,   195,     0,    35,   121,     0,     0,
     119,   140,   135,   134,     0,     0,     0,     0,   223,   198,
       0,   295,     0,   195,     0,     0,     0,     0,     0,   325,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   264,
     263,   195,   168,   182,   184,   193,   185,   253,     0,   173,
     256,   318,   319,     0,     0,   195,     0,   299,   309,   310,
     313,   314,     0,   316,   308,   311,   312,   304,   303,   305,
     306,   307,   335,   337,   356,     0,   361,     0,   364,   372,
     375,     0,   159,   160,   158,   164,    42,    45,    46,    43,
      44,    47,    48,    62,    49,    51,    50,    65,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,     0,     0,
       0,     0,     0,   113,     0,     0,   405,     0,    34,     0,
       0,   117,     0,     0,     0,     0,   392,     0,   388,     0,
     228,   224,     0,   296,     0,   330,     0,     0,   323,     0,
       0,     0,     0,     0,     0,   334,     0,   281,     0,   283,
       0,   367,   366,     0,     0,     0,   202,   203,   204,   205,
     201,   206,     0,   191,     0,   186,   287,   285,   286,   288,
     289,   170,   177,   195,   317,     0,     0,   298,     0,     0,
       0,   157,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,     0,   113,   106,    40,     0,     0,     0,    32,
      37,   414,     0,   254,     0,   391,   390,   142,     0,   141,
       0,     0,   297,   331,     0,   327,     0,   326,     0,     0,
       0,     0,     0,     0,     0,   193,   183,     0,     0,   190,
       0,     0,   175,     0,     0,   332,   321,   320,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   111,
     108,   112,   107,    41,     0,   115,     0,     0,     0,     0,
     389,   229,   225,   329,   324,   328,   315,     0,     0,     0,
       0,   282,   284,   187,   199,     0,     0,   292,   290,   291,
     293,   294,     0,     0,   153,   333,   322,    64,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   114,
     408,     0,   406,   403,     0,   255,   371,     0,   279,     0,
       0,     0,     0,   176,   174,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   404,     0,     0,
     412,   403,     0,     0,     0,   200,   192,    63,    69,    70,
      67,    68,    71,    72,    73,    66,    93,    94,    91,    92,
      95,    96,    97,    90,    77,    78,    75,    76,    79,    80,
      81,    74,   101,   102,    99,   100,   103,   104,   105,    98,
      85,    86,    83,    84,    87,    88,    89,    82,   409,   411,
     410,   407,     0,   413,     0,   280,     0,     0,     0,   276,
     402,     0,     0,     0,     0,   275,   277,   403,   278
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -757,  -757,  -757,   942,  -757,   944,  -757,   519,  -757,   493,
    -757,   434,   439,  -757,  -370,   951,   952,   859,  -757,  -757,
     954,  -757,   743,   956,   957,   -57,  1003,   -15,   823,   877,
      98,  -757,  -757,   571,  -757,  -757,  -757,  -757,  -757,  -757,
    -162,  -757,  -757,  -757,  -757,   499,  -154,     7,   430,  -757,
    -757,   886,  -757,  -757,   965,   967,   968,   969,   970,  -280,
    -757,   706,  -170,  -189,  -757,  -411,  -410,  -409,  -408,  -402,
    -757,  -757,  -757,  -757,  -757,  -757,   732,  -757,  -757,   637,
     473,  -222,  -757,  -757,  -757,  -757,  -757,  -757,  -757,  -757,
     679,   681,   447,  -757,  -757,  -757,  -757,   822,   654,   462,
     -67,   225,   272,  -757,  -757,  -756,  -757,   199,   258,  -757
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   370,   371,   372,   483,
     563,   564,   565,   373,   268,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   360,   361,   454,   261,   365,   148,   328,   419,
     171,   684,   602,    90,   412,   413,   414,   415,   535,    30,
      79,    80,   416,   532,    31,    32,    33,    34,    35,   226,
     380,   227,   228,   229,   853,   230,   231,   232,   233,   234,
     541,   235,   236,   237,   238,   239,   303,   240,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     322,   323,   253,   254,   255,   256,   257,   258,   497,   498,
     173,   103,    95,    86,   100,   790,   569,   731,   732,   376
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     275,    83,   321,   392,   123,   489,   274,   536,   537,   538,
     539,    48,    47,    50,   170,  -398,   540,   298,   326,    91,
      77,   442,     1,   587,     2,     3,     4,     5,     6,     7,
       8,     9,   316,   317,    14,   843,   318,   319,    10,   302,
      11,    12,    13,   515,   263,   174,   367,   423,   101,   426,
      87,   269,    88,   664,    89,   325,   110,    41,   452,   453,
     175,    47,   176,   177,   180,   507,   181,   182,  -401,    49,
     130,   185,   199,   186,   187,    51,    52,   329,    94,   136,
     506,    53,   203,   204,   205,   330,   331,   330,   331,   362,
     363,   350,   349,   352,   592,    14,   351,    14,   353,   492,
     427,   858,   382,   153,   154,   155,   311,   298,   312,   313,
     502,   162,   163,   164,   301,   567,   377,   330,   331,   378,
     572,   178,   424,   588,   590,   183,   330,   331,   327,   444,
     396,   397,   188,   533,   273,   330,   331,    72,    16,   202,
     203,   204,   205,    76,   270,   546,   428,   429,   430,   431,
     432,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     421,   422,    62,    63,   266,    64,   368,    78,   369,   330,
     331,   264,   330,   331,   330,   331,    15,    65,    66,    84,
     210,   211,   212,   213,   330,   331,    85,   534,   146,   677,
     678,   679,   680,   411,   455,   144,   654,   443,   681,    16,
     558,   330,   331,   390,   111,   112,   366,   214,   215,   216,
     324,   206,   207,   224,   320,   595,    94,   179,    81,   223,
     208,   184,   209,   218,   354,   219,   220,   356,   189,   355,
     510,   511,   357,   513,    91,   544,   102,   410,   210,   211,
     212,   213,   201,    36,    37,    38,   108,   358,   559,   109,
     560,   561,   359,   562,   138,    39,    40,   542,   113,   838,
     224,   839,   840,   604,   114,   214,   215,   216,   104,   105,
     106,   107,   202,   203,   204,   205,   486,   503,   381,   487,
     327,   362,   517,   115,   395,   518,   129,   217,    42,    43,
      44,    67,    68,   135,   659,   137,    69,    70,    71,   519,
      45,    46,   520,   131,   132,   218,   742,   219,   220,    96,
      97,    98,    99,   221,   222,   223,   571,   488,   224,   378,
     225,   391,   139,   589,    87,   399,    88,   400,    89,   401,
     140,   202,   203,   204,   205,   133,   134,   585,   190,   586,
     330,   331,   191,   192,   206,   207,   573,   193,   194,   327,
      73,    74,    75,   208,   523,   209,   387,   388,   301,   607,
     525,  -207,   526,   527,   528,   529,   577,   530,   531,   578,
     670,   210,   211,   212,   213,   334,   521,   522,   504,   609,
     610,   611,   612,   613,   318,   736,   614,   615,   202,   203,
     204,   205,   141,   579,  -402,  -402,   578,   143,   214,   215,
     216,   147,   743,   206,   207,   508,   616,   509,    14,   401,
     545,   149,   208,   582,   209,   151,   327,   152,   665,   606,
     217,   156,   327,  -402,  -402,   344,   345,   346,   347,   348,
     210,   211,   212,   213,   655,   160,   658,   378,   218,   378,
     219,   220,   686,   674,   675,   327,   221,   222,   223,   687,
     157,   224,   688,   225,   346,   347,   348,   214,   215,   216,
     206,   207,    54,    55,    56,    57,    58,    59,   158,   208,
      60,   209,   161,   170,   617,   618,   619,   620,   621,   217,
     729,   622,   623,   378,   494,   495,   496,   210,   211,   212,
     213,   165,   166,   202,   203,   204,   205,   218,   167,   219,
     220,   624,   172,   195,   196,   221,   222,   223,   735,   197,
     224,   327,   225,   744,   214,   215,   216,   625,   626,   627,
     628,   629,   259,   738,   630,   631,   739,   260,   603,   202,
     203,   204,   205,   267,   787,   796,   217,   788,   378,   850,
     262,   271,   788,   272,   632,   332,   276,   333,   277,   278,
     279,   281,   282,   304,   218,   305,   219,   220,   283,   299,
     306,   300,   221,   222,   223,   296,   297,   224,   307,   225,
     349,   795,   308,   310,   208,   314,   209,   315,   364,   374,
       1,   375,     2,     3,     4,     5,     6,     7,     8,     9,
     379,   384,   210,   211,   212,   213,    10,   334,    11,    12,
      13,   296,   389,   385,   386,   393,   402,    14,   403,   404,
     208,   406,   209,   405,   335,   336,   337,   338,   339,   214,
     215,   216,   340,   407,   408,   409,   418,   420,   210,   211,
     212,   213,   425,   224,   445,   447,   449,   450,   451,   484,
     485,   217,   491,   490,   341,   342,   343,   344,   345,   346,
     347,   348,   500,    14,   501,   214,   215,   216,   424,   218,
     493,   219,   220,   505,   512,   514,   330,   221,   222,   223,
     394,   524,   224,   547,   225,   543,   549,   217,   552,   633,
     634,   635,   636,   637,   394,   550,   638,   639,   553,   554,
     568,   575,   555,   556,   557,   218,   566,   219,   220,   576,
     574,   591,   580,   221,   222,   223,   640,   581,   224,     1,
     225,     2,     3,     4,     5,     6,     7,   583,     9,   593,
     594,   599,   334,   600,   597,    10,   394,    11,    12,    13,
     598,   601,   522,   605,    15,   521,   334,   608,   649,   335,
     336,   337,   338,   656,   650,   661,   657,   340,   662,   667,
     663,   669,   676,   335,   336,   337,   338,    16,   548,   671,
     672,   340,   682,   685,   683,   689,   730,   690,   691,   341,
     342,   343,   344,   345,   346,   347,   348,   741,   334,   692,
     666,   693,    14,   341,   342,   343,   344,   345,   346,   347,
     348,   334,   694,   695,   696,   335,   336,   337,   338,   697,
     733,   698,   699,   340,   700,   701,   702,   734,  -402,  -402,
     337,   338,   786,   703,   704,   705,  -402,   745,   746,   706,
     747,   707,   708,   709,   710,   341,   342,   343,   344,   345,
     346,   347,   348,   711,   748,   712,   749,   750,  -402,   342,
     343,   344,   345,   346,   347,   348,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   713,   751,
     714,   715,   716,    15,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   465,   466,   467,   468,   469,   470,   471,
     472,   473,   474,   475,   476,   717,    16,   477,   718,   719,
     478,   479,   720,   721,   480,   481,   482,   641,   642,   643,
     644,   645,   752,   753,   646,   647,   754,   722,   723,   724,
     725,   726,   727,   728,   755,   737,   740,   756,   757,   758,
     759,   760,   761,   762,   648,   763,   764,   765,   766,   767,
     768,   769,   770,   771,   772,   773,   774,   775,   776,   777,
     778,   779,   780,   781,   782,   783,   784,   785,   789,   791,
     327,   792,   793,   794,   797,   798,   799,   800,   801,   848,
     802,   803,   804,   805,   806,   807,   808,   809,   810,   811,
     812,   813,   814,   815,   816,   817,   818,   819,   820,   821,
     822,   823,   849,   824,   825,   826,   827,   828,   829,   830,
     831,   854,   832,   833,   834,   835,   855,   652,   836,   584,
     837,   842,   653,   845,   844,   117,   570,   846,   851,   852,
     856,   857,   118,   119,   265,   120,   383,   121,   122,    82,
     280,   200,   551,   596,   198,   673,   124,   142,   125,   126,
     127,   128,   446,   417,   398,   651,   448,   668,   309,   499,
     660,   847,   516,     0,     0,     0,   841
};

static const yytype_int16 yycheck[] =
{
     170,    16,   224,   283,    61,   375,   168,   418,   418,   418,
     418,     4,     3,     6,    68,     0,   418,   206,    78,     8,
      13,     3,     7,    85,     9,    10,    11,    12,    13,    14,
      15,    16,   221,   222,    80,   791,     5,     6,    23,   209,
      25,    26,    27,     3,    57,   112,     3,    77,    41,    75,
      20,    34,    22,    85,    24,   225,    49,    33,    72,    73,
       3,     3,     5,     6,     3,    88,     5,     6,    64,    78,
      63,     3,   139,     5,     6,   158,   159,    57,    74,    72,
      57,   164,     4,     5,     6,   149,   150,   149,   150,   259,
     260,   183,   186,   183,   188,    80,   188,    80,   188,   379,
     126,   857,   272,    96,    97,    98,   117,   296,   119,   120,
     390,   104,   105,   106,    89,   485,   185,   149,   150,   188,
     490,    64,   152,    88,   188,    64,   149,   150,   188,   351,
     300,   301,    64,     3,   188,   149,   150,    30,   184,     3,
       4,     5,     6,     3,   159,   425,   335,   336,   337,   338,
     339,   340,   341,   342,   343,   344,   345,   346,   347,   348,
     330,   331,    29,    30,   157,    32,   123,     3,   125,   149,
     150,   184,   149,   150,   149,   150,   161,    44,    45,     0,
     102,   103,   104,   105,   149,   150,   187,    57,    90,   600,
     600,   600,   600,   184,   364,   184,   566,   179,   600,   184,
      75,   149,   150,    67,    78,    79,   263,   129,   130,   131,
     225,    75,    76,   182,   183,   185,    74,   160,   162,   179,
      84,   160,    86,   169,   183,   171,   172,   183,   160,   188,
     400,   401,   188,   403,     8,   424,    74,   185,   102,   103,
     104,   105,   144,    29,    30,    31,   186,   183,   123,    14,
     125,   126,   188,   128,   188,    41,    42,   419,     3,     3,
     182,     5,     6,   543,     3,   129,   130,   131,    43,    44,
      45,    46,     3,     4,     5,     6,   185,   185,   271,   188,
     188,   451,   185,     3,   299,   188,     3,   151,    29,    30,
      31,   158,   159,     3,   574,    57,   163,   164,   165,   185,
      41,    42,   188,   166,   167,   169,   676,   171,   172,    37,
      38,    39,    40,   177,   178,   179,   185,   374,   182,   188,
     184,   185,     6,   512,    20,    85,    22,    87,    24,    89,
     185,     3,     4,     5,     6,   166,   167,   507,    42,   509,
     149,   150,    46,    47,    75,    76,   185,    51,    52,   188,
     153,   154,   155,    84,   411,    86,    48,    49,    89,   548,
      58,    59,    60,    61,    62,    63,   185,    65,    66,   188,
     592,   102,   103,   104,   105,   127,     5,     6,   393,    91,
      92,    93,    94,    95,     5,     6,    98,    99,     3,     4,
       5,     6,   185,   185,   146,   147,   188,    21,   129,   130,
     131,    67,   682,    75,    76,    85,   118,    87,    80,    89,
     425,    69,    84,   185,    86,    75,   188,     3,   588,   185,
     151,     3,   188,   175,   176,   177,   178,   179,   180,   181,
     102,   103,   104,   105,   185,    76,   185,   188,   169,   188,
     171,   172,   185,   597,   598,   188,   177,   178,   179,   185,
      64,   182,   188,   184,   179,   180,   181,   129,   130,   131,
      75,    76,    35,    36,    37,    38,    39,    40,   184,    84,
      43,    86,     3,    68,    91,    92,    93,    94,    95,   151,
     185,    98,    99,   188,    81,    82,    83,   102,   103,   104,
     105,     3,     3,     3,     4,     5,     6,   169,     3,   171,
     172,   118,     4,     3,     3,   177,   178,   179,   185,   184,
     182,   188,   184,   683,   129,   130,   131,    91,    92,    93,
      94,    95,    55,   185,    98,    99,   188,    70,   543,     3,
       4,     5,     6,     3,   185,   185,   151,   188,   188,   185,
      76,    64,   188,   174,   118,    75,    80,    77,    80,     3,
       6,   185,   185,     4,   169,     4,   171,   172,   184,   184,
       4,   184,   177,   178,   179,    75,    76,   182,     4,   184,
     186,   741,     6,   184,    84,   184,    86,   184,    71,    57,
       7,   184,     9,    10,    11,    12,    13,    14,    15,    16,
     184,   184,   102,   103,   104,   105,    23,   127,    25,    26,
      27,    75,   185,   184,    46,   184,     4,    80,   184,   184,
      84,     4,    86,   184,   144,   145,   146,   147,   148,   129,
     130,   131,   152,     4,   189,   185,   168,     3,   102,   103,
     104,   105,   184,   182,     6,     6,     6,     5,   188,   124,
     184,   151,     3,   184,   174,   175,   176,   177,   178,   179,
     180,   181,     6,    80,     6,   129,   130,   131,   152,   169,
     188,   171,   172,   185,    78,     4,   149,   177,   178,   179,
      75,   188,   182,   126,   184,   184,   189,   151,   184,    91,
      92,    93,    94,    95,    75,   189,    98,    99,   184,   184,
      28,     4,   184,   184,   184,   169,   184,   171,   172,     3,
     184,   188,    53,   177,   178,   179,   118,    50,   182,     7,
     184,     9,    10,    11,    12,    13,    14,   185,    16,     4,
       4,     3,   127,   188,    59,    23,    75,    25,    26,    27,
      59,    54,     6,   185,   161,     5,   127,     6,   126,   144,
     145,   146,   147,   184,   124,     6,   157,   152,     6,   182,
     185,     4,   184,   144,   145,   146,   147,   184,   149,   185,
     185,   152,    55,   185,    56,   188,     3,   188,   188,   174,
     175,   176,   177,   178,   179,   180,   181,    64,   127,   188,
     185,   188,    80,   174,   175,   176,   177,   178,   179,   180,
     181,   127,   188,   188,   188,   144,   145,   146,   147,   188,
       3,   188,   188,   152,   188,   188,   188,   157,   144,   145,
     146,   147,   174,   188,   188,   188,   152,     6,     6,   188,
       6,   188,   188,   188,   188,   174,   175,   176,   177,   178,
     179,   180,   181,   188,     6,   188,     6,     6,   174,   175,
     176,   177,   178,   179,   180,   181,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   188,     6,
     188,   188,   188,   161,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   188,   184,   113,   188,   188,
     116,   117,   188,   188,   120,   121,   122,    91,    92,    93,
      94,    95,     6,     6,    98,    99,     6,   188,   188,   188,
     188,   188,   188,   188,     6,   188,   188,     6,     6,     6,
       6,     6,     6,     6,   118,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,    80,     3,
     188,     4,     4,     4,   185,   185,   185,   185,   185,     4,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,     4,   185,   185,   185,   185,   185,   185,   185,
     185,     6,   185,   185,   185,   185,     4,   563,   185,   506,
     185,   184,   563,   185,   188,    61,   487,   188,   188,   188,
     185,   185,    61,    61,   155,    61,   273,    61,    61,    16,
     197,   144,   451,   524,   138,   595,    61,    85,    61,    61,
      61,    61,   353,   327,   302,   562,   355,   590,   216,   385,
     578,   842,   405,    -1,    -1,    -1,   788
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   161,   184,   191,   192,   193,
     195,   205,   206,   208,   210,   213,   214,   215,   216,   217,
     239,   244,   245,   246,   247,   248,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   237,    78,
     237,   158,   159,   164,    35,    36,    37,    38,    39,    40,
      43,   209,    29,    30,    32,    44,    45,   158,   159,   163,
     164,   165,    30,   153,   154,   155,     3,   237,     3,   240,
     241,   162,   216,   217,     0,   187,   293,    20,    22,    24,
     233,     8,   218,   220,    74,   292,   292,   292,   292,   292,
     294,   237,    74,   291,   291,   291,   291,   291,   186,    14,
     237,    78,    79,     3,     3,     3,   194,   195,   205,   206,
     210,   213,   214,   215,   244,   245,   246,   247,   248,     3,
     237,   166,   167,   166,   167,     3,   237,    57,   188,     6,
     185,   185,   193,    21,   184,   219,   220,    67,   227,    69,
     221,    75,     3,   237,   237,   237,     3,    64,   184,   207,
      76,     3,   237,   237,   237,     3,     3,     3,   211,   212,
      68,   230,     4,   290,   290,     3,     5,     6,    64,   160,
       3,     5,     6,    64,   160,     3,     5,     6,    64,   160,
      42,    46,    47,    51,    52,     3,     3,   184,   241,   290,
     219,   220,     3,     4,     5,     6,    75,    76,    84,    86,
     102,   103,   104,   105,   129,   130,   131,   151,   169,   171,
     172,   177,   178,   179,   182,   184,   249,   251,   252,   253,
     255,   256,   257,   258,   259,   261,   262,   263,   264,   265,
     267,   268,   269,   270,   271,   272,   273,   274,   275,   276,
     277,   278,   279,   282,   283,   284,   285,   286,   287,    55,
      70,   225,    76,    57,   184,   207,   237,     3,   204,    34,
     217,    64,   174,   188,   230,   252,    80,    80,     3,     6,
     218,   185,   185,   184,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,    75,    76,   253,   184,
     184,    89,   252,   266,     4,     4,     4,     4,     6,   287,
     184,   117,   119,   120,   184,   184,   253,   253,     5,     6,
     183,   271,   280,   281,   217,   252,    78,   188,   228,    57,
     149,   150,    75,    77,   127,   144,   145,   146,   147,   148,
     152,   174,   175,   176,   177,   178,   179,   180,   181,   186,
     183,   188,   183,   188,   183,   188,   183,   188,   183,   188,
     222,   223,   252,   252,    71,   226,   215,     3,   123,   125,
     196,   197,   198,   203,    57,   184,   299,   185,   188,   184,
     250,   237,   252,   212,   184,   184,    46,    48,    49,   185,
      67,   185,   249,   184,    75,   217,   252,   252,   266,    85,
      87,    89,     4,   184,   184,   184,     4,     4,   189,   185,
     185,   184,   234,   235,   236,   237,   242,   251,   168,   229,
       3,   252,   252,    77,   152,   184,    75,   126,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,     3,   179,   271,     6,   280,     6,   281,     6,
       5,   188,    72,    73,   224,   252,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   113,   116,   117,
     120,   121,   122,   199,   124,   184,   185,   188,   215,   204,
     184,     3,   249,   188,    81,    82,    83,   288,   289,   288,
       6,     6,   249,   185,   217,   185,    57,    88,    85,    87,
     252,   252,    78,   252,     4,     3,   269,   185,   188,   185,
     188,     5,     6,   215,   188,    58,    60,    61,    62,    63,
      65,    66,   243,     3,    57,   238,   255,   256,   257,   258,
     259,   260,   230,   184,   253,   217,   249,   126,   149,   189,
     189,   223,   184,   184,   184,   184,   184,   184,    75,   123,
     125,   126,   128,   200,   201,   202,   184,   204,    28,   296,
     197,   185,   204,   185,   184,     4,     3,   185,   188,   185,
      53,    50,   185,   185,   199,   252,   252,    85,    88,   253,
     188,   188,   188,     4,     4,   185,   235,    59,    59,     3,
     188,    54,   232,   217,   249,   185,   185,   253,     6,    91,
      92,    93,    94,    95,    98,    99,   118,    91,    92,    93,
      94,    95,    98,    99,   118,    91,    92,    93,    94,    95,
      98,    99,   118,    91,    92,    93,    94,    95,    98,    99,
     118,    91,    92,    93,    94,    95,    98,    99,   118,   126,
     124,   270,   201,   202,   204,   185,   184,   157,   185,   249,
     289,     6,     6,   185,    85,   252,   185,   182,   282,     4,
     271,   185,   185,   238,   236,   236,   184,   255,   256,   257,
     258,   259,    55,    56,   231,   185,   185,   185,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   185,
       3,   297,   298,     3,   157,   185,     6,   188,   185,   188,
     188,    64,   204,   249,   252,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   174,   185,   188,    80,
     295,     3,     4,     4,     4,   252,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,     3,     5,
       6,   298,   184,   295,   188,   185,   188,   297,     4,     4,
     185,   188,   188,   254,     6,     4,   185,   185,   295
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   190,   191,   192,   192,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   193,   193,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     195,   195,   195,   195,   195,   195,   196,   196,   197,   197,
     198,   198,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   200,   200,   201,   201,
     201,   201,   202,   202,   203,   203,   204,   204,   205,   206,
     206,   207,   207,   208,   209,   209,   209,   209,   209,   209,
     209,   209,   210,   211,   211,   212,   213,   213,   213,   213,
     213,   214,   214,   215,   215,   215,   215,   216,   216,   217,
     218,   219,   219,   220,   221,   221,   222,   222,   223,   224,
     224,   224,   225,   225,   226,   226,   227,   227,   228,   228,
     229,   229,   230,   230,   231,   231,   232,   232,   233,   233,
     233,   233,   234,   234,   235,   235,   236,   236,   237,   237,
     238,   238,   238,   238,   239,   239,   240,   240,   241,   242,
     242,   243,   243,   243,   243,   243,   243,   243,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     245,   245,   245,   246,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   248,   249,   249,   250,   250,   251,   251,   252,   252,
     252,   252,   252,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   254,   254,   255,   256,   257,
     257,   258,   258,   259,   259,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   262,   262,
     263,   264,   264,   265,   265,   265,   265,   266,   266,   267,
     268,   268,   268,   268,   269,   269,   269,   269,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     271,   271,   271,   271,   272,   273,   273,   274,   274,   275,
     276,   276,   277,   278,   278,   279,   280,   281,   282,   282,
     283,   284,   284,   285,   286,   286,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   288,   288,
     289,   289,   289,   290,   291,   291,   292,   292,   293,   293,
     294,   294,   295,   295,   296,   296,   297,   297,   298,   298,
     298,   298,   299,   299,   299
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
       4,     4,     5,     6,     7,     9,     4,     5,     7,     9,
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
       1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     3,     3,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       2,     2,     1,     1,     2,     0,     3,     0,     1,     0,
       2,     0,     4,     0,     4,     0,     1,     3,     1,     3,
       3,     3,     6,     7,     3
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
#line 2154 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2162 "parser.cpp"
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
#line 2176 "parser.cpp"
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
#line 2190 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2201 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2209 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2218 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2227 "parser.cpp"
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
#line 2241 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2252 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2262 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2272 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2282 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2292 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2302 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2312 "parser.cpp"
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
#line 2326 "parser.cpp"
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
#line 2340 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2350 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2358 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2366 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2375 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2383 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2391 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2399 "parser.cpp"
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
#line 2413 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2422 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2431 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2440 "parser.cpp"
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
#line 2453 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2462 "parser.cpp"
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
#line 2476 "parser.cpp"
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
#line 2490 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2500 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2509 "parser.cpp"
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
#line 2523 "parser.cpp"
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
#line 2540 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2548 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2556 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2564 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2572 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2580 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2596 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2604 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2612 "parser.cpp"
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
#line 2626 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2634 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2642 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2650 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2658 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2666 "parser.cpp"
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
#line 2679 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2687 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2695 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2703 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2711 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2719 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2727 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2735 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2743 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2751 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 364 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2791 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 368 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2807 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2815 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2823 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2831 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2839 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2847 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2855 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2866 "parser.cpp"
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
#line 2880 "parser.cpp"
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
#line 2894 "parser.cpp"
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
#line 2908 "parser.cpp"
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

#line 3016 "parser.cpp"

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
#line 3231 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 496 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3242 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 502 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3253 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 509 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3259 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3265 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3271 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3277 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3283 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3289 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3295 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3301 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3307 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 518 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3313 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 519 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3319 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3325 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3331 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 523 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3337 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3343 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3349 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3355 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3361 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3367 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3373 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3379 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 531 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3385 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 532 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3391 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 533 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3397 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3403 "parser.cpp"
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
#line 3423 "parser.cpp"
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
#line 3441 "parser.cpp"
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
#line 3474 "parser.cpp"
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
#line 3494 "parser.cpp"
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
#line 3515 "parser.cpp"
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
#line 3548 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 666 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3557 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 670 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3566 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 676 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3574 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 679 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3582 "parser.cpp"
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
#line 3635 "parser.cpp"
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
#line 3674 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 769 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3680 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 770 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3686 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 771 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3692 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 772 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3698 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 773 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3704 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 774 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3710 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3716 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 776 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3722 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 777 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3728 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 778 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3734 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 779 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3740 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 780 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3746 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 781 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3752 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 782 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3758 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 783 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3764 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 784 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3770 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 785 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3776 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 786 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3782 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 787 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3788 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 790 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3794 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 792 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3800 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3806 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 794 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3812 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 795 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3818 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 798 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3824 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 799 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3830 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 800 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3836 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3842 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3848 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3854 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3860 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3866 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3872 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3878 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3884 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3890 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3896 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3902 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3908 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3914 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3920 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3926 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3932 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3938 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3944 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3950 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3956 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3962 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3968 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3974 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3980 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3986 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3992 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3998 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4004 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4010 "parser.cpp"
    break;

  case 98: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4016 "parser.cpp"
    break;

  case 99: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4022 "parser.cpp"
    break;

  case 100: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4028 "parser.cpp"
    break;

  case 101: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4034 "parser.cpp"
    break;

  case 102: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4040 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4046 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4052 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4058 "parser.cpp"
    break;

  case 106: /* column_constraints: column_constraint  */
#line 856 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4067 "parser.cpp"
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
#line 4081 "parser.cpp"
    break;

  case 108: /* column_constraint: PRIMARY KEY  */
#line 870 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4089 "parser.cpp"
    break;

  case 109: /* column_constraint: UNIQUE  */
#line 873 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4097 "parser.cpp"
    break;

  case 110: /* column_constraint: NULLABLE  */
#line 876 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4105 "parser.cpp"
    break;

  case 111: /* column_constraint: NOT NULLABLE  */
#line 879 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4113 "parser.cpp"
    break;

  case 112: /* default_expr: DEFAULT constant_expr  */
#line 883 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4121 "parser.cpp"
    break;

  case 113: /* default_expr: %empty  */
#line 886 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4129 "parser.cpp"
    break;

  case 114: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 891 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4139 "parser.cpp"
    break;

  case 115: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 896 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4149 "parser.cpp"
    break;

  case 116: /* identifier_array: IDENTIFIER  */
#line 903 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4160 "parser.cpp"
    break;

  case 117: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 909 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4171 "parser.cpp"
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
#line 4188 "parser.cpp"
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
#line 4227 "parser.cpp"
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
#line 4244 "parser.cpp"
    break;

  case 121: /* optional_identifier_array: '(' identifier_array ')'  */
#line 982 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4252 "parser.cpp"
    break;

  case 122: /* optional_identifier_array: %empty  */
#line 985 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4260 "parser.cpp"
    break;

  case 123: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 992 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4270 "parser.cpp"
    break;

  case 124: /* explain_type: ANALYZE  */
#line 998 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4278 "parser.cpp"
    break;

  case 125: /* explain_type: AST  */
#line 1001 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4286 "parser.cpp"
    break;

  case 126: /* explain_type: RAW  */
#line 1004 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4294 "parser.cpp"
    break;

  case 127: /* explain_type: LOGICAL  */
#line 1007 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4302 "parser.cpp"
    break;

  case 128: /* explain_type: PHYSICAL  */
#line 1010 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4310 "parser.cpp"
    break;

  case 129: /* explain_type: PIPELINE  */
#line 1013 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4318 "parser.cpp"
    break;

  case 130: /* explain_type: FRAGMENT  */
#line 1016 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4326 "parser.cpp"
    break;

  case 131: /* explain_type: %empty  */
#line 1019 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4334 "parser.cpp"
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
#line 4351 "parser.cpp"
    break;

  case 133: /* update_expr_array: update_expr  */
#line 1039 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4360 "parser.cpp"
    break;

  case 134: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1043 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4369 "parser.cpp"
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
#line 4381 "parser.cpp"
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
#line 4397 "parser.cpp"
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
#line 4415 "parser.cpp"
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
#line 4433 "parser.cpp"
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
#line 4451 "parser.cpp"
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
#line 4474 "parser.cpp"
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
#line 4520 "parser.cpp"
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
#line 4566 "parser.cpp"
    break;

  case 143: /* select_statement: select_without_paren  */
#line 1228 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4574 "parser.cpp"
    break;

  case 144: /* select_statement: select_with_paren  */
#line 1231 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4582 "parser.cpp"
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
#line 4596 "parser.cpp"
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
#line 4610 "parser.cpp"
    break;

  case 147: /* select_with_paren: '(' select_without_paren ')'  */
#line 1253 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4618 "parser.cpp"
    break;

  case 148: /* select_with_paren: '(' select_with_paren ')'  */
#line 1256 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4626 "parser.cpp"
    break;

  case 149: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1260 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4635 "parser.cpp"
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
#line 4661 "parser.cpp"
    break;

  case 151: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1287 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4669 "parser.cpp"
    break;

  case 152: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1290 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4677 "parser.cpp"
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
#line 4697 "parser.cpp"
    break;

  case 154: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1311 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4705 "parser.cpp"
    break;

  case 155: /* order_by_clause: %empty  */
#line 1314 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4713 "parser.cpp"
    break;

  case 156: /* order_by_expr_list: order_by_expr  */
#line 1318 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4722 "parser.cpp"
    break;

  case 157: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1322 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4731 "parser.cpp"
    break;

  case 158: /* order_by_expr: expr order_by_type  */
#line 1327 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4741 "parser.cpp"
    break;

  case 159: /* order_by_type: ASC  */
#line 1333 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4749 "parser.cpp"
    break;

  case 160: /* order_by_type: DESC  */
#line 1336 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4757 "parser.cpp"
    break;

  case 161: /* order_by_type: %empty  */
#line 1339 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4765 "parser.cpp"
    break;

  case 162: /* limit_expr: LIMIT expr  */
#line 1343 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4773 "parser.cpp"
    break;

  case 163: /* limit_expr: %empty  */
#line 1347 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4779 "parser.cpp"
    break;

  case 164: /* offset_expr: OFFSET expr  */
#line 1349 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4787 "parser.cpp"
    break;

  case 165: /* offset_expr: %empty  */
#line 1353 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4793 "parser.cpp"
    break;

  case 166: /* distinct: DISTINCT  */
#line 1355 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4801 "parser.cpp"
    break;

  case 167: /* distinct: %empty  */
#line 1358 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4809 "parser.cpp"
    break;

  case 168: /* from_clause: FROM table_reference  */
#line 1362 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4817 "parser.cpp"
    break;

  case 169: /* from_clause: %empty  */
#line 1365 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4825 "parser.cpp"
    break;

  case 170: /* search_clause: SEARCH sub_search_array  */
#line 1369 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4835 "parser.cpp"
    break;

  case 171: /* search_clause: %empty  */
#line 1374 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4843 "parser.cpp"
    break;

  case 172: /* where_clause: WHERE expr  */
#line 1378 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4851 "parser.cpp"
    break;

  case 173: /* where_clause: %empty  */
#line 1381 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4859 "parser.cpp"
    break;

  case 174: /* having_clause: HAVING expr  */
#line 1385 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4867 "parser.cpp"
    break;

  case 175: /* having_clause: %empty  */
#line 1388 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4875 "parser.cpp"
    break;

  case 176: /* group_by_clause: GROUP BY expr_array  */
#line 1392 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4883 "parser.cpp"
    break;

  case 177: /* group_by_clause: %empty  */
#line 1395 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4891 "parser.cpp"
    break;

  case 178: /* set_operator: UNION  */
#line 1399 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4899 "parser.cpp"
    break;

  case 179: /* set_operator: UNION ALL  */
#line 1402 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4907 "parser.cpp"
    break;

  case 180: /* set_operator: INTERSECT  */
#line 1405 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4915 "parser.cpp"
    break;

  case 181: /* set_operator: EXCEPT  */
#line 1408 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4923 "parser.cpp"
    break;

  case 182: /* table_reference: table_reference_unit  */
#line 1416 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4931 "parser.cpp"
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
#line 4949 "parser.cpp"
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
#line 4967 "parser.cpp"
    break;

  case 187: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1450 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4978 "parser.cpp"
    break;

  case 188: /* table_name: IDENTIFIER  */
#line 1459 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4988 "parser.cpp"
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
#line 5000 "parser.cpp"
    break;

  case 190: /* table_alias: AS IDENTIFIER  */
#line 1473 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5010 "parser.cpp"
    break;

  case 191: /* table_alias: IDENTIFIER  */
#line 1478 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5020 "parser.cpp"
    break;

  case 192: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1483 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5031 "parser.cpp"
    break;

  case 193: /* table_alias: %empty  */
#line 1489 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5039 "parser.cpp"
    break;

  case 194: /* with_clause: WITH with_expr_list  */
#line 1496 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5047 "parser.cpp"
    break;

  case 195: /* with_clause: %empty  */
#line 1499 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5055 "parser.cpp"
    break;

  case 196: /* with_expr_list: with_expr  */
#line 1503 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5064 "parser.cpp"
    break;

  case 197: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1506 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5073 "parser.cpp"
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
#line 5085 "parser.cpp"
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
#line 5097 "parser.cpp"
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
#line 5110 "parser.cpp"
    break;

  case 201: /* join_type: INNER  */
#line 1544 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5118 "parser.cpp"
    break;

  case 202: /* join_type: LEFT  */
#line 1547 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5126 "parser.cpp"
    break;

  case 203: /* join_type: RIGHT  */
#line 1550 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5134 "parser.cpp"
    break;

  case 204: /* join_type: OUTER  */
#line 1553 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5142 "parser.cpp"
    break;

  case 205: /* join_type: FULL  */
#line 1556 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5150 "parser.cpp"
    break;

  case 206: /* join_type: CROSS  */
#line 1559 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5158 "parser.cpp"
    break;

  case 207: /* join_type: %empty  */
#line 1562 "parser.y"
                {
}
#line 5165 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW DATABASES  */
#line 1568 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5174 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW TABLES  */
#line 1572 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5183 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW VIEWS  */
#line 1576 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5192 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW CONFIGS  */
#line 1580 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5201 "parser.cpp"
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
#line 5213 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW PROFILES  */
#line 1591 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5222 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW SESSION VARIABLES  */
#line 1595 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5231 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1599 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5240 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1603 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5251 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1609 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5262 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1615 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5273 "parser.cpp"
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
#line 5289 "parser.cpp"
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
#line 5305 "parser.cpp"
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
#line 5321 "parser.cpp"
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
#line 5338 "parser.cpp"
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
#line 5355 "parser.cpp"
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
#line 5373 "parser.cpp"
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
#line 5392 "parser.cpp"
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
#line 5408 "parser.cpp"
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
#line 5427 "parser.cpp"
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
#line 5448 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1746 "parser.y"
                                                                             {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexChunk;
      if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
          (yyval.show_stmt)->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
          free((yyvsp[-6].table_name_t)->schema_name_ptr_);
      }
      (yyval.show_stmt)->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
      free((yyvsp[-6].table_name_t)->table_name_ptr_);
      delete (yyvsp[-6].table_name_t);

      (yyval.show_stmt)->index_name_ = (yyvsp[-4].str_value);
      free((yyvsp[-4].str_value));

      (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
      (yyval.show_stmt)->chunk_id_ = (yyvsp[0].long_value);
  }
#line 5470 "parser.cpp"
    break;

  case 230: /* flush_statement: FLUSH DATA  */
#line 1767 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5479 "parser.cpp"
    break;

  case 231: /* flush_statement: FLUSH LOG  */
#line 1771 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5488 "parser.cpp"
    break;

  case 232: /* flush_statement: FLUSH BUFFER  */
#line 1775 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5497 "parser.cpp"
    break;

  case 233: /* optimize_statement: OPTIMIZE table_name  */
#line 1783 "parser.y"
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
#line 5512 "parser.cpp"
    break;

  case 234: /* command_statement: USE IDENTIFIER  */
#line 1797 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5523 "parser.cpp"
    break;

  case 235: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1803 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5533 "parser.cpp"
    break;

  case 236: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1808 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5544 "parser.cpp"
    break;

  case 237: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1814 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5555 "parser.cpp"
    break;

  case 238: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1820 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5568 "parser.cpp"
    break;

  case 239: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1828 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5579 "parser.cpp"
    break;

  case 240: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1834 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5590 "parser.cpp"
    break;

  case 241: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1840 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5601 "parser.cpp"
    break;

  case 242: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1846 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5612 "parser.cpp"
    break;

  case 243: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1852 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5625 "parser.cpp"
    break;

  case 244: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1860 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5636 "parser.cpp"
    break;

  case 245: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1866 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5647 "parser.cpp"
    break;

  case 246: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1872 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5658 "parser.cpp"
    break;

  case 247: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1878 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5669 "parser.cpp"
    break;

  case 248: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1884 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5682 "parser.cpp"
    break;

  case 249: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1892 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5693 "parser.cpp"
    break;

  case 250: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1898 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5704 "parser.cpp"
    break;

  case 251: /* compact_statement: COMPACT TABLE table_name  */
#line 1905 "parser.y"
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
#line 5721 "parser.cpp"
    break;

  case 252: /* expr_array: expr_alias  */
#line 1922 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5730 "parser.cpp"
    break;

  case 253: /* expr_array: expr_array ',' expr_alias  */
#line 1926 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5739 "parser.cpp"
    break;

  case 254: /* expr_array_list: '(' expr_array ')'  */
#line 1931 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5748 "parser.cpp"
    break;

  case 255: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1935 "parser.y"
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
#line 5768 "parser.cpp"
    break;

  case 256: /* expr_alias: expr AS IDENTIFIER  */
#line 1962 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5779 "parser.cpp"
    break;

  case 257: /* expr_alias: expr  */
#line 1968 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5787 "parser.cpp"
    break;

  case 263: /* operand: '(' expr ')'  */
#line 1978 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5795 "parser.cpp"
    break;

  case 264: /* operand: '(' select_without_paren ')'  */
#line 1981 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5806 "parser.cpp"
    break;

  case 265: /* operand: constant_expr  */
#line 1987 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5814 "parser.cpp"
    break;

  case 275: /* extra_match_tensor_option: ',' STRING  */
#line 2000 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5822 "parser.cpp"
    break;

  case 276: /* extra_match_tensor_option: %empty  */
#line 2003 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5830 "parser.cpp"
    break;

  case 277: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2009 "parser.y"
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
#line 5851 "parser.cpp"
    break;

  case 278: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2028 "parser.y"
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
#line 5897 "parser.cpp"
    break;

  case 279: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2070 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5910 "parser.cpp"
    break;

  case 280: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2078 "parser.y"
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
#line 5925 "parser.cpp"
    break;

  case 281: /* query_expr: QUERY '(' STRING ')'  */
#line 2089 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5936 "parser.cpp"
    break;

  case 282: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2095 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5949 "parser.cpp"
    break;

  case 283: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2104 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5960 "parser.cpp"
    break;

  case 284: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2110 "parser.y"
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
#line 5976 "parser.cpp"
    break;

  case 285: /* sub_search_array: match_vector_expr  */
#line 2123 "parser.y"
                                     {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5985 "parser.cpp"
    break;

  case 286: /* sub_search_array: match_text_expr  */
#line 2127 "parser.y"
                  {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5994 "parser.cpp"
    break;

  case 287: /* sub_search_array: match_tensor_expr  */
#line 2131 "parser.y"
                    {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6003 "parser.cpp"
    break;

  case 288: /* sub_search_array: query_expr  */
#line 2135 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6012 "parser.cpp"
    break;

  case 289: /* sub_search_array: fusion_expr  */
#line 2139 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6021 "parser.cpp"
    break;

  case 290: /* sub_search_array: sub_search_array ',' match_vector_expr  */
#line 2143 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6030 "parser.cpp"
    break;

  case 291: /* sub_search_array: sub_search_array ',' match_text_expr  */
#line 2147 "parser.y"
                                       {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6039 "parser.cpp"
    break;

  case 292: /* sub_search_array: sub_search_array ',' match_tensor_expr  */
#line 2151 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6048 "parser.cpp"
    break;

  case 293: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2155 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6057 "parser.cpp"
    break;

  case 294: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2159 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6066 "parser.cpp"
    break;

  case 295: /* function_expr: IDENTIFIER '(' ')'  */
#line 2164 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6079 "parser.cpp"
    break;

  case 296: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2172 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6092 "parser.cpp"
    break;

  case 297: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2180 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6106 "parser.cpp"
    break;

  case 298: /* function_expr: operand IS NOT NULLABLE  */
#line 2189 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6118 "parser.cpp"
    break;

  case 299: /* function_expr: operand IS NULLABLE  */
#line 2196 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6130 "parser.cpp"
    break;

  case 300: /* function_expr: NOT operand  */
#line 2203 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6142 "parser.cpp"
    break;

  case 301: /* function_expr: '-' operand  */
#line 2210 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6154 "parser.cpp"
    break;

  case 302: /* function_expr: '+' operand  */
#line 2217 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6166 "parser.cpp"
    break;

  case 303: /* function_expr: operand '-' operand  */
#line 2224 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6179 "parser.cpp"
    break;

  case 304: /* function_expr: operand '+' operand  */
#line 2232 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6192 "parser.cpp"
    break;

  case 305: /* function_expr: operand '*' operand  */
#line 2240 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6205 "parser.cpp"
    break;

  case 306: /* function_expr: operand '/' operand  */
#line 2248 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6218 "parser.cpp"
    break;

  case 307: /* function_expr: operand '%' operand  */
#line 2256 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6231 "parser.cpp"
    break;

  case 308: /* function_expr: operand '=' operand  */
#line 2264 "parser.y"
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

  case 309: /* function_expr: operand EQUAL operand  */
#line 2272 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6257 "parser.cpp"
    break;

  case 310: /* function_expr: operand NOT_EQ operand  */
#line 2280 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6270 "parser.cpp"
    break;

  case 311: /* function_expr: operand '<' operand  */
#line 2288 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6283 "parser.cpp"
    break;

  case 312: /* function_expr: operand '>' operand  */
#line 2296 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6296 "parser.cpp"
    break;

  case 313: /* function_expr: operand LESS_EQ operand  */
#line 2304 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6309 "parser.cpp"
    break;

  case 314: /* function_expr: operand GREATER_EQ operand  */
#line 2312 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6322 "parser.cpp"
    break;

  case 315: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2320 "parser.y"
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
#line 6357 "parser.cpp"
    break;

  case 316: /* function_expr: operand LIKE operand  */
#line 2350 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6370 "parser.cpp"
    break;

  case 317: /* function_expr: operand NOT LIKE operand  */
#line 2358 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6383 "parser.cpp"
    break;

  case 318: /* conjunction_expr: expr AND expr  */
#line 2367 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6396 "parser.cpp"
    break;

  case 319: /* conjunction_expr: expr OR expr  */
#line 2375 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6409 "parser.cpp"
    break;

  case 320: /* between_expr: operand BETWEEN operand AND operand  */
#line 2384 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6421 "parser.cpp"
    break;

  case 321: /* in_expr: operand IN '(' expr_array ')'  */
#line 2392 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6432 "parser.cpp"
    break;

  case 322: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2398 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6443 "parser.cpp"
    break;

  case 323: /* case_expr: CASE expr case_check_array END  */
#line 2405 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6454 "parser.cpp"
    break;

  case 324: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2411 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6466 "parser.cpp"
    break;

  case 325: /* case_expr: CASE case_check_array END  */
#line 2418 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6476 "parser.cpp"
    break;

  case 326: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2423 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6487 "parser.cpp"
    break;

  case 327: /* case_check_array: WHEN expr THEN expr  */
#line 2430 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6499 "parser.cpp"
    break;

  case 328: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2437 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6511 "parser.cpp"
    break;

  case 329: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2445 "parser.y"
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
#line 6539 "parser.cpp"
    break;

  case 330: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2469 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6550 "parser.cpp"
    break;

  case 331: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2475 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6561 "parser.cpp"
    break;

  case 332: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2481 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6573 "parser.cpp"
    break;

  case 333: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2488 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6585 "parser.cpp"
    break;

  case 334: /* column_expr: IDENTIFIER  */
#line 2496 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6597 "parser.cpp"
    break;

  case 335: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2503 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6609 "parser.cpp"
    break;

  case 336: /* column_expr: '*'  */
#line 2510 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6619 "parser.cpp"
    break;

  case 337: /* column_expr: column_expr '.' '*'  */
#line 2515 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6633 "parser.cpp"
    break;

  case 338: /* constant_expr: STRING  */
#line 2525 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6643 "parser.cpp"
    break;

  case 339: /* constant_expr: TRUE  */
#line 2530 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6653 "parser.cpp"
    break;

  case 340: /* constant_expr: FALSE  */
#line 2535 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6663 "parser.cpp"
    break;

  case 341: /* constant_expr: DOUBLE_VALUE  */
#line 2540 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6673 "parser.cpp"
    break;

  case 342: /* constant_expr: LONG_VALUE  */
#line 2545 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6683 "parser.cpp"
    break;

  case 343: /* constant_expr: DATE STRING  */
#line 2550 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6693 "parser.cpp"
    break;

  case 344: /* constant_expr: TIME STRING  */
#line 2555 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6703 "parser.cpp"
    break;

  case 345: /* constant_expr: DATETIME STRING  */
#line 2560 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6713 "parser.cpp"
    break;

  case 346: /* constant_expr: TIMESTAMP STRING  */
#line 2565 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6723 "parser.cpp"
    break;

  case 347: /* constant_expr: INTERVAL interval_expr  */
#line 2570 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6731 "parser.cpp"
    break;

  case 348: /* constant_expr: interval_expr  */
#line 2573 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6739 "parser.cpp"
    break;

  case 349: /* constant_expr: common_array_expr  */
#line 2576 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6747 "parser.cpp"
    break;

  case 350: /* common_array_expr: array_expr  */
#line 2580 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6755 "parser.cpp"
    break;

  case 351: /* common_array_expr: subarray_array_expr  */
#line 2583 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6763 "parser.cpp"
    break;

  case 352: /* common_array_expr: sparse_array_expr  */
#line 2586 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6771 "parser.cpp"
    break;

  case 353: /* common_array_expr: empty_array_expr  */
#line 2589 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6779 "parser.cpp"
    break;

  case 354: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2593 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6787 "parser.cpp"
    break;

  case 355: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2597 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 6797 "parser.cpp"
    break;

  case 356: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2602 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6806 "parser.cpp"
    break;

  case 357: /* sparse_array_expr: long_sparse_array_expr  */
#line 2607 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6814 "parser.cpp"
    break;

  case 358: /* sparse_array_expr: double_sparse_array_expr  */
#line 2610 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6822 "parser.cpp"
    break;

  case 359: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2614 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6830 "parser.cpp"
    break;

  case 360: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2618 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 6842 "parser.cpp"
    break;

  case 361: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2625 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6853 "parser.cpp"
    break;

  case 362: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2632 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6861 "parser.cpp"
    break;

  case 363: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2636 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 6873 "parser.cpp"
    break;

  case 364: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2643 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6884 "parser.cpp"
    break;

  case 365: /* empty_array_expr: '[' ']'  */
#line 2650 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 6892 "parser.cpp"
    break;

  case 366: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 2654 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 6900 "parser.cpp"
    break;

  case 367: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 2658 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 6908 "parser.cpp"
    break;

  case 368: /* array_expr: long_array_expr  */
#line 2662 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6916 "parser.cpp"
    break;

  case 369: /* array_expr: double_array_expr  */
#line 2665 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6924 "parser.cpp"
    break;

  case 370: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2669 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6932 "parser.cpp"
    break;

  case 371: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2673 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6942 "parser.cpp"
    break;

  case 372: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2678 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6951 "parser.cpp"
    break;

  case 373: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2683 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6959 "parser.cpp"
    break;

  case 374: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2687 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6969 "parser.cpp"
    break;

  case 375: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2692 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6978 "parser.cpp"
    break;

  case 376: /* interval_expr: LONG_VALUE SECONDS  */
#line 2697 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6989 "parser.cpp"
    break;

  case 377: /* interval_expr: LONG_VALUE SECOND  */
#line 2703 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7000 "parser.cpp"
    break;

  case 378: /* interval_expr: LONG_VALUE MINUTES  */
#line 2709 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7011 "parser.cpp"
    break;

  case 379: /* interval_expr: LONG_VALUE MINUTE  */
#line 2715 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7022 "parser.cpp"
    break;

  case 380: /* interval_expr: LONG_VALUE HOURS  */
#line 2721 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7033 "parser.cpp"
    break;

  case 381: /* interval_expr: LONG_VALUE HOUR  */
#line 2727 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7044 "parser.cpp"
    break;

  case 382: /* interval_expr: LONG_VALUE DAYS  */
#line 2733 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7055 "parser.cpp"
    break;

  case 383: /* interval_expr: LONG_VALUE DAY  */
#line 2739 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7066 "parser.cpp"
    break;

  case 384: /* interval_expr: LONG_VALUE MONTHS  */
#line 2745 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7077 "parser.cpp"
    break;

  case 385: /* interval_expr: LONG_VALUE MONTH  */
#line 2751 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7088 "parser.cpp"
    break;

  case 386: /* interval_expr: LONG_VALUE YEARS  */
#line 2757 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7099 "parser.cpp"
    break;

  case 387: /* interval_expr: LONG_VALUE YEAR  */
#line 2763 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7110 "parser.cpp"
    break;

  case 388: /* copy_option_list: copy_option  */
#line 2774 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7119 "parser.cpp"
    break;

  case 389: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2778 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7128 "parser.cpp"
    break;

  case 390: /* copy_option: FORMAT IDENTIFIER  */
#line 2783 "parser.y"
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
#line 7158 "parser.cpp"
    break;

  case 391: /* copy_option: DELIMITER STRING  */
#line 2808 "parser.y"
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
#line 7173 "parser.cpp"
    break;

  case 392: /* copy_option: HEADER  */
#line 2818 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7183 "parser.cpp"
    break;

  case 393: /* file_path: STRING  */
#line 2824 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7191 "parser.cpp"
    break;

  case 394: /* if_exists: IF EXISTS  */
#line 2828 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7197 "parser.cpp"
    break;

  case 395: /* if_exists: %empty  */
#line 2829 "parser.y"
  { (yyval.bool_value) = false; }
#line 7203 "parser.cpp"
    break;

  case 396: /* if_not_exists: IF NOT EXISTS  */
#line 2831 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7209 "parser.cpp"
    break;

  case 397: /* if_not_exists: %empty  */
#line 2832 "parser.y"
  { (yyval.bool_value) = false; }
#line 7215 "parser.cpp"
    break;

  case 400: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2847 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7228 "parser.cpp"
    break;

  case 401: /* if_not_exists_info: %empty  */
#line 2855 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7236 "parser.cpp"
    break;

  case 402: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2859 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7244 "parser.cpp"
    break;

  case 403: /* with_index_param_list: %empty  */
#line 2862 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7252 "parser.cpp"
    break;

  case 404: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2866 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7260 "parser.cpp"
    break;

  case 405: /* optional_table_properties_list: %empty  */
#line 2869 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7268 "parser.cpp"
    break;

  case 406: /* index_param_list: index_param  */
#line 2873 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7277 "parser.cpp"
    break;

  case 407: /* index_param_list: index_param_list ',' index_param  */
#line 2877 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7286 "parser.cpp"
    break;

  case 408: /* index_param: IDENTIFIER  */
#line 2882 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7296 "parser.cpp"
    break;

  case 409: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2887 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7309 "parser.cpp"
    break;

  case 410: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2895 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7321 "parser.cpp"
    break;

  case 411: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2902 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7333 "parser.cpp"
    break;

  case 412: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2913 "parser.y"
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
#line 7386 "parser.cpp"
    break;

  case 413: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2961 "parser.y"
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
#line 7440 "parser.cpp"
    break;

  case 414: /* index_info_list: '(' identifier_array ')'  */
#line 3010 "parser.y"
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
#line 7458 "parser.cpp"
    break;


#line 7462 "parser.cpp"

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

#line 3024 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

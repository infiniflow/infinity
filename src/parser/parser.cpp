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
  YYSYMBOL_TRANSACTIONS = 156,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 157,              /* TRANSACTION  */
  YYSYMBOL_KNN = 158,                      /* KNN  */
  YYSYMBOL_USING = 159,                    /* USING  */
  YYSYMBOL_SESSION = 160,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 161,                   /* GLOBAL  */
  YYSYMBOL_OFF = 162,                      /* OFF  */
  YYSYMBOL_EXPORT = 163,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 164,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 165,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 166,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 167,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 168,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 169,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 170,                   /* SEARCH  */
  YYSYMBOL_MATCH = 171,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 172,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 173,                    /* QUERY  */
  YYSYMBOL_QUERIES = 174,                  /* QUERIES  */
  YYSYMBOL_FUSION = 175,                   /* FUSION  */
  YYSYMBOL_NUMBER = 176,                   /* NUMBER  */
  YYSYMBOL_177_ = 177,                     /* '='  */
  YYSYMBOL_178_ = 178,                     /* '<'  */
  YYSYMBOL_179_ = 179,                     /* '>'  */
  YYSYMBOL_180_ = 180,                     /* '+'  */
  YYSYMBOL_181_ = 181,                     /* '-'  */
  YYSYMBOL_182_ = 182,                     /* '*'  */
  YYSYMBOL_183_ = 183,                     /* '/'  */
  YYSYMBOL_184_ = 184,                     /* '%'  */
  YYSYMBOL_185_ = 185,                     /* '['  */
  YYSYMBOL_186_ = 186,                     /* ']'  */
  YYSYMBOL_187_ = 187,                     /* '('  */
  YYSYMBOL_188_ = 188,                     /* ')'  */
  YYSYMBOL_189_ = 189,                     /* '.'  */
  YYSYMBOL_190_ = 190,                     /* ';'  */
  YYSYMBOL_191_ = 191,                     /* ','  */
  YYSYMBOL_192_ = 192,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 193,                 /* $accept  */
  YYSYMBOL_input_pattern = 194,            /* input_pattern  */
  YYSYMBOL_statement_list = 195,           /* statement_list  */
  YYSYMBOL_statement = 196,                /* statement  */
  YYSYMBOL_explainable_statement = 197,    /* explainable_statement  */
  YYSYMBOL_create_statement = 198,         /* create_statement  */
  YYSYMBOL_table_element_array = 199,      /* table_element_array  */
  YYSYMBOL_table_element = 200,            /* table_element  */
  YYSYMBOL_table_column = 201,             /* table_column  */
  YYSYMBOL_column_type = 202,              /* column_type  */
  YYSYMBOL_column_constraints = 203,       /* column_constraints  */
  YYSYMBOL_column_constraint = 204,        /* column_constraint  */
  YYSYMBOL_default_expr = 205,             /* default_expr  */
  YYSYMBOL_table_constraint = 206,         /* table_constraint  */
  YYSYMBOL_identifier_array = 207,         /* identifier_array  */
  YYSYMBOL_delete_statement = 208,         /* delete_statement  */
  YYSYMBOL_insert_statement = 209,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 210, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 211,        /* explain_statement  */
  YYSYMBOL_explain_type = 212,             /* explain_type  */
  YYSYMBOL_update_statement = 213,         /* update_statement  */
  YYSYMBOL_update_expr_array = 214,        /* update_expr_array  */
  YYSYMBOL_update_expr = 215,              /* update_expr  */
  YYSYMBOL_drop_statement = 216,           /* drop_statement  */
  YYSYMBOL_copy_statement = 217,           /* copy_statement  */
  YYSYMBOL_select_statement = 218,         /* select_statement  */
  YYSYMBOL_select_with_paren = 219,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 220,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 221, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 222, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 223, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 224,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 225,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 226,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 227,            /* order_by_type  */
  YYSYMBOL_limit_expr = 228,               /* limit_expr  */
  YYSYMBOL_offset_expr = 229,              /* offset_expr  */
  YYSYMBOL_distinct = 230,                 /* distinct  */
  YYSYMBOL_from_clause = 231,              /* from_clause  */
  YYSYMBOL_search_clause = 232,            /* search_clause  */
  YYSYMBOL_where_clause = 233,             /* where_clause  */
  YYSYMBOL_having_clause = 234,            /* having_clause  */
  YYSYMBOL_group_by_clause = 235,          /* group_by_clause  */
  YYSYMBOL_set_operator = 236,             /* set_operator  */
  YYSYMBOL_table_reference = 237,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 238,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 239,     /* table_reference_name  */
  YYSYMBOL_table_name = 240,               /* table_name  */
  YYSYMBOL_table_alias = 241,              /* table_alias  */
  YYSYMBOL_with_clause = 242,              /* with_clause  */
  YYSYMBOL_with_expr_list = 243,           /* with_expr_list  */
  YYSYMBOL_with_expr = 244,                /* with_expr  */
  YYSYMBOL_join_clause = 245,              /* join_clause  */
  YYSYMBOL_join_type = 246,                /* join_type  */
  YYSYMBOL_show_statement = 247,           /* show_statement  */
  YYSYMBOL_flush_statement = 248,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 249,       /* optimize_statement  */
  YYSYMBOL_command_statement = 250,        /* command_statement  */
  YYSYMBOL_compact_statement = 251,        /* compact_statement  */
  YYSYMBOL_expr_array = 252,               /* expr_array  */
  YYSYMBOL_expr_array_list = 253,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 254,               /* expr_alias  */
  YYSYMBOL_expr = 255,                     /* expr  */
  YYSYMBOL_operand = 256,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 257, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 258,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 259,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 260,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 261,          /* match_text_expr  */
  YYSYMBOL_query_expr = 262,               /* query_expr  */
  YYSYMBOL_fusion_expr = 263,              /* fusion_expr  */
  YYSYMBOL_sub_search = 264,               /* sub_search  */
  YYSYMBOL_sub_search_array = 265,         /* sub_search_array  */
  YYSYMBOL_function_expr = 266,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 267,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 268,             /* between_expr  */
  YYSYMBOL_in_expr = 269,                  /* in_expr  */
  YYSYMBOL_case_expr = 270,                /* case_expr  */
  YYSYMBOL_case_check_array = 271,         /* case_check_array  */
  YYSYMBOL_cast_expr = 272,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 273,            /* subquery_expr  */
  YYSYMBOL_column_expr = 274,              /* column_expr  */
  YYSYMBOL_constant_expr = 275,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 276,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 277, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 278,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 279, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 280,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 281,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 282, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 283, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 284, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 285,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 286,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 287,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 288,               /* array_expr  */
  YYSYMBOL_long_array_expr = 289,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 290, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 291,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 292, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 293,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 294,         /* copy_option_list  */
  YYSYMBOL_copy_option = 295,              /* copy_option  */
  YYSYMBOL_file_path = 296,                /* file_path  */
  YYSYMBOL_if_exists = 297,                /* if_exists  */
  YYSYMBOL_if_not_exists = 298,            /* if_not_exists  */
  YYSYMBOL_semicolon = 299,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 300,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 301,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 302, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 303,         /* index_param_list  */
  YYSYMBOL_index_param = 304,              /* index_param  */
  YYSYMBOL_index_info_list = 305           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 434 "parser.cpp"

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
#define YYFINAL  89
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1123

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  193
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  113
/* YYNRULES -- Number of rules.  */
#define YYNRULES  423
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  891

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   431


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
       2,     2,     2,     2,     2,     2,     2,   184,     2,     2,
     187,   188,   182,   180,   191,   181,   189,   183,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   192,   190,
     178,   177,   179,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   185,     2,   186,     2,     2,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   492,   492,   496,   502,   509,   510,   511,   512,   513,
     514,   515,   516,   517,   518,   519,   520,   521,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     541,   558,   574,   603,   619,   637,   666,   670,   676,   679,
     686,   736,   772,   773,   774,   775,   776,   777,   778,   779,
     780,   781,   782,   783,   784,   785,   786,   787,   788,   789,
     790,   793,   795,   796,   797,   798,   801,   802,   803,   804,
     805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   823,   824,
     825,   826,   827,   828,   829,   830,   831,   832,   833,   834,
     835,   836,   837,   838,   839,   840,   859,   863,   873,   876,
     879,   882,   886,   889,   894,   899,   906,   912,   922,   938,
     972,   985,   988,   995,  1001,  1004,  1007,  1010,  1013,  1016,
    1019,  1022,  1029,  1042,  1046,  1051,  1064,  1077,  1092,  1107,
    1122,  1145,  1186,  1229,  1274,  1277,  1280,  1289,  1299,  1302,
    1306,  1311,  1333,  1336,  1341,  1357,  1360,  1364,  1368,  1373,
    1379,  1382,  1385,  1389,  1393,  1395,  1399,  1401,  1404,  1408,
    1411,  1415,  1420,  1424,  1427,  1431,  1434,  1438,  1441,  1445,
    1448,  1451,  1454,  1462,  1465,  1480,  1480,  1482,  1496,  1505,
    1510,  1519,  1524,  1529,  1535,  1542,  1545,  1549,  1552,  1557,
    1569,  1576,  1590,  1593,  1596,  1599,  1602,  1605,  1608,  1614,
    1618,  1622,  1626,  1630,  1637,  1641,  1645,  1649,  1654,  1658,
    1663,  1667,  1671,  1677,  1683,  1689,  1700,  1711,  1722,  1734,
    1746,  1759,  1773,  1784,  1798,  1814,  1835,  1839,  1843,  1851,
    1865,  1871,  1876,  1882,  1888,  1896,  1902,  1908,  1914,  1920,
    1928,  1934,  1940,  1946,  1952,  1960,  1966,  1973,  1990,  1994,
    1999,  2003,  2030,  2036,  2040,  2041,  2042,  2043,  2044,  2046,
    2049,  2055,  2058,  2059,  2060,  2061,  2062,  2063,  2064,  2065,
    2066,  2067,  2069,  2072,  2078,  2097,  2141,  2159,  2167,  2178,
    2184,  2193,  2199,  2211,  2214,  2217,  2220,  2223,  2226,  2230,
    2234,  2239,  2247,  2255,  2264,  2271,  2278,  2285,  2292,  2299,
    2307,  2315,  2323,  2331,  2339,  2347,  2355,  2363,  2371,  2379,
    2387,  2395,  2425,  2433,  2442,  2450,  2459,  2467,  2473,  2480,
    2486,  2493,  2498,  2505,  2512,  2520,  2544,  2550,  2556,  2563,
    2571,  2578,  2585,  2590,  2600,  2605,  2610,  2615,  2620,  2625,
    2630,  2635,  2640,  2645,  2648,  2651,  2655,  2658,  2661,  2664,
    2668,  2671,  2674,  2678,  2682,  2687,  2692,  2695,  2699,  2703,
    2710,  2717,  2721,  2728,  2735,  2739,  2743,  2747,  2750,  2754,
    2758,  2763,  2768,  2772,  2777,  2782,  2788,  2794,  2800,  2806,
    2812,  2818,  2824,  2830,  2836,  2842,  2848,  2859,  2863,  2868,
    2896,  2906,  2912,  2916,  2917,  2919,  2920,  2922,  2923,  2935,
    2943,  2947,  2950,  2954,  2957,  2961,  2965,  2970,  2976,  2986,
    2993,  3004,  3054,  3103
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
  "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION", "KNN", "USING",
  "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG",
  "PROFILES", "VARIABLES", "VARIABLE", "SEARCH", "MATCH", "MAXSIM",
  "QUERY", "QUERIES", "FUSION", "NUMBER", "'='", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'",
  "','", "':'", "$accept", "input_pattern", "statement_list", "statement",
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
  "expr_array_list", "expr_alias", "expr", "operand",
  "extra_match_tensor_option", "match_tensor_expr", "match_vector_expr",
  "match_sparse_expr", "match_text_expr", "query_expr", "fusion_expr",
  "sub_search", "sub_search_array", "function_expr", "conjunction_expr",
  "between_expr", "in_expr", "case_expr", "case_check_array", "cast_expr",
  "subquery_expr", "column_expr", "constant_expr", "common_array_expr",
  "common_sparse_array_expr", "subarray_array_expr",
  "unclosed_subarray_array_expr", "sparse_array_expr",
  "long_sparse_array_expr", "unclosed_long_sparse_array_expr",
  "double_sparse_array_expr", "unclosed_double_sparse_array_expr",
  "empty_array_expr", "int_sparse_ele", "float_sparse_ele", "array_expr",
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

#define YYPACT_NINF (-664)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-411)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     465,   219,    74,   277,   118,    52,   118,   134,   866,   527,
      51,   167,   135,   118,   137,    -8,   -56,   160,    12,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,   125,  -664,  -664,
     212,  -664,  -664,  -664,  -664,  -664,   119,   119,   119,   119,
       3,   118,   172,   172,   172,   172,   172,    67,   297,   118,
     -46,   286,   352,   354,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,   490,   361,   118,  -664,  -664,  -664,  -664,  -664,   327,
      61,   133,  -664,   378,  -664,   226,  -664,   118,  -664,  -664,
    -664,  -664,  -664,   339,   213,  -664,   404,   228,   236,  -664,
      31,  -664,   405,  -664,  -664,     2,   370,  -664,   372,   376,
     444,   118,   118,   118,   461,   407,   302,   397,   507,   118,
     118,   118,   519,   523,   525,   426,   526,   526,   356,    63,
      73,    82,  -664,  -664,  -664,  -664,  -664,  -664,  -664,   125,
    -664,  -664,  -664,  -664,  -664,  -664,   289,  -664,  -664,   531,
    -664,   537,  -664,   536,  -664,   357,   137,   526,  -664,  -664,
    -664,  -664,     2,  -664,  -664,  -664,   356,   492,   476,   473,
    -664,   -39,  -664,   302,  -664,   118,   555,    28,  -664,  -664,
    -664,  -664,  -664,   499,  -664,   387,   -49,  -664,   356,  -664,
    -664,   485,   493,   388,  -664,  -664,   665,   449,   389,   390,
     248,   570,   577,   583,   584,  -664,  -664,   585,   409,   138,
     410,   411,   464,   464,  -664,    15,   263,    48,  -664,    -6,
     586,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,   412,  -664,  -664,  -664,  -126,  -664,
    -664,  -111,  -664,   -95,  -664,  -664,  -664,    79,  -664,    86,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,   599,   593,  -664,  -664,
    -664,  -664,  -664,  -664,   212,  -664,  -664,   416,   417,   -52,
     356,   356,   535,  -664,   -56,    99,   550,   421,  -664,   185,
     423,  -664,   118,   356,   525,  -664,    27,   424,   425,   111,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,   464,   427,   596,   538,   356,   356,   154,   186,
    -664,  -664,  -664,  -664,   665,  -664,   612,   430,   432,   434,
     436,   621,   622,   300,   300,  -664,   435,  -664,  -664,  -664,
    -664,   450,    35,   561,   356,   638,   356,   356,   -43,   455,
     -36,   464,   464,   464,   464,   464,   464,   464,   464,   464,
     464,   464,   464,   464,   464,     8,  -664,   458,  -664,   641,
    -664,   642,  -664,   648,  -664,   651,   611,   326,   470,  -664,
    -664,    11,   489,   469,  -664,     1,    27,   356,  -664,   125,
     739,   552,   475,   192,  -664,  -664,  -664,   -56,   555,   494,
    -664,   675,   356,   488,  -664,    27,  -664,   479,   479,   356,
    -664,   194,   538,   533,   498,    56,   -51,   198,  -664,   356,
     356,   613,   356,   685,    22,   356,   200,   207,   384,  -664,
    -664,   526,  -664,  -664,  -664,   541,   508,   464,   263,   571,
    -664,   644,   644,   486,   486,   575,   644,   644,   486,   486,
     300,   300,  -664,  -664,  -664,  -664,  -664,  -664,   504,  -664,
     506,  -664,  -664,  -664,   693,   697,  -664,  -664,   -56,   513,
     751,  -664,    49,  -664,   183,   426,   356,  -664,  -664,  -664,
      27,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,   530,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,   539,   542,   548,   549,   557,   153,   558,
     555,   686,    99,   125,   209,   555,  -664,   214,   559,   711,
     715,  -664,   215,  -664,   220,   221,  -664,   540,  -664,   739,
     356,  -664,   356,   -30,   131,   464,   -79,   534,  -664,   157,
     -55,  -664,   733,  -664,   735,  -664,  -664,   667,   263,   644,
     562,   227,  -664,   464,   743,   746,   707,   712,    39,    11,
     702,  -664,  -664,  -664,  -664,  -664,  -664,   713,  -664,   778,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,   591,   729,
    -664,   779,   491,   771,   780,   789,   801,   658,   600,   662,
    -664,  -664,   189,  -664,   664,   555,   229,   606,  -664,  -664,
     635,   234,  -664,   356,  -664,  -664,  -664,   479,  -664,  -664,
    -664,   607,    27,     4,  -664,   356,   528,   630,   814,   458,
     634,   632,   663,   666,   669,   258,  -664,  -664,   596,   844,
     848,    49,   751,    11,    11,   671,   183,   812,   820,   274,
     677,   694,   695,   699,   700,   706,   717,   719,   720,   721,
     722,   723,   724,   725,   726,   727,   730,   731,   732,   734,
     736,   737,   738,   740,   741,   742,   744,   745,   747,   748,
     749,   750,   752,   753,   754,   755,   756,   757,   758,   759,
    -664,   874,  -664,    78,  -664,  -664,  -664,   275,  -664,   874,
     917,   765,   305,  -664,  -664,  -664,    27,  -664,   451,   760,
     307,   761,    10,   762,  -664,  -664,  -664,  -664,  -664,   479,
    -664,  -664,  -664,  -664,  -664,  -664,   862,   555,  -664,   356,
     356,  -664,  -664,   924,   928,   931,   936,   948,   949,   950,
     951,   952,   953,   954,   955,   956,   957,   958,   959,   960,
     961,   962,   963,   964,   965,   966,   967,   968,   969,   970,
     971,   972,   973,   974,   975,   976,   977,   978,   979,   980,
     981,   982,   983,   984,   815,   317,  -664,  -664,  -664,   321,
     911,   990,  -664,  -664,   991,  -664,   992,   993,   994,   323,
     356,   330,   803,    27,   811,   813,   816,   817,   818,   819,
     821,   822,   823,   824,   825,   826,   827,   828,   829,   830,
     831,   832,   833,   834,   835,   836,   837,   838,   839,   840,
     841,   842,   843,   845,   846,   847,   849,   850,   851,   852,
     853,   854,   855,   856,   857,    98,  -664,   874,  -664,  -664,
     911,   809,   858,   859,   860,  -664,    27,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,  -664,
    -664,  -664,  -664,  -664,   998,  -664,   999,  1002,   861,   863,
     865,  1026,  1032,   867,   911,   868,  -664,  -664,  -664,   911,
    -664
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     196,     0,     0,     0,     0,     0,     0,     0,   131,     0,
       0,     0,     0,     0,     0,     0,   196,     0,   408,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   145,   144,
       0,     8,    14,    15,    16,    17,   406,   406,   406,   406,
     406,     0,   404,   404,   404,   404,   404,   189,     0,     0,
       0,     0,     0,     0,   125,   129,   126,   127,   128,   130,
     124,   196,     0,     0,   210,   211,   209,   215,   218,     0,
       0,     0,   212,     0,   214,     0,   216,     0,   236,   237,
     238,   240,   239,     0,   195,   197,     0,     0,     0,     1,
     196,     2,   179,   181,   182,     0,   168,   150,   156,     0,
       0,     0,     0,     0,     0,     0,   122,     0,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,     0,
       0,     0,   123,    18,    23,    25,    24,    19,    20,    22,
      21,    26,    27,    28,    29,   224,   225,   219,   220,     0,
     221,     0,   213,     0,   257,     0,     0,     0,   149,   148,
       4,   180,     0,   146,   147,   167,     0,     0,   164,     0,
      30,     0,    31,   122,   409,     0,     0,   196,   403,   136,
     138,   137,   139,     0,   190,     0,   174,   133,     0,   118,
     402,     0,     0,   340,   344,   347,   348,     0,     0,     0,
       0,     0,     0,     0,     0,   345,   346,     0,     0,     0,
       0,     0,     0,     0,   342,     0,   196,     0,   258,   263,
     264,   278,   276,   279,   277,   280,   281,   273,   268,   267,
     266,   274,   275,   265,   272,   271,   355,   357,     0,   358,
     366,     0,   367,     0,   359,   356,   377,     0,   378,     0,
     354,   244,   246,   245,   242,   243,   249,   251,   250,   247,
     248,   254,   256,   255,   252,   253,     0,     0,   227,   226,
     232,   222,   223,   217,     0,   198,   241,     0,     0,   170,
       0,     0,   166,   405,   196,     0,     0,     0,   116,     0,
       0,   120,     0,     0,     0,   132,   173,     0,     0,     0,
     386,   385,   388,   387,   390,   389,   392,   391,   394,   393,
     396,   395,     0,     0,   306,   196,     0,     0,     0,     0,
     349,   350,   351,   352,     0,   353,     0,     0,     0,     0,
       0,     0,     0,   308,   307,   383,   380,   374,   364,   369,
     372,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   363,     0,   368,     0,
     371,     0,   379,     0,   382,     0,   233,   228,     0,   153,
     152,     0,   172,   155,   157,   162,   163,     0,   151,    33,
       0,     0,     0,     0,    36,    38,    39,   196,     0,    35,
     121,     0,     0,   119,   140,   135,   134,     0,     0,     0,
     301,     0,   196,     0,     0,     0,     0,     0,   331,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   270,
     269,     0,   259,   262,   324,   325,     0,     0,   196,     0,
     305,   315,   316,   319,   320,     0,   322,   314,   317,   318,
     310,   309,   311,   312,   313,   341,   343,   365,     0,   370,
       0,   373,   381,   384,     0,     0,   229,   199,   196,   169,
     183,   185,   194,   186,     0,   174,     0,   160,   161,   159,
     165,    42,    45,    46,    43,    44,    47,    48,    62,    49,
      51,    50,    65,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,     0,     0,     0,     0,     0,   412,     0,
       0,   414,     0,    34,     0,     0,   117,     0,     0,     0,
       0,   401,     0,   397,     0,     0,   302,     0,   336,     0,
       0,   329,     0,     0,     0,     0,     0,     0,   340,     0,
       0,   289,     0,   291,     0,   376,   375,     0,   196,   323,
       0,     0,   304,     0,     0,     0,   234,   230,     0,     0,
       0,   203,   204,   205,   206,   202,   207,     0,   192,     0,
     187,   295,   293,   296,   294,   297,   298,   299,   171,   178,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     109,   110,   113,   106,   113,     0,     0,     0,    32,    37,
     423,     0,   260,     0,   400,   399,   143,     0,   141,   303,
     337,     0,   333,     0,   332,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   338,   327,   326,     0,
       0,   194,   184,     0,     0,   191,     0,     0,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     111,     0,   108,     0,   107,    41,    40,     0,   115,     0,
       0,     0,     0,   398,   335,   330,   334,   321,     0,     0,
       0,     0,     0,     0,   360,   362,   361,   290,   292,     0,
     339,   328,   235,   231,   188,   200,     0,     0,   300,     0,
       0,   154,    64,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   417,     0,   415,   112,   114,     0,
     412,     0,   261,   380,     0,   287,     0,     0,     0,     0,
       0,     0,   177,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   411,     0,   413,   421,
     412,     0,     0,     0,     0,   142,   201,   193,    63,    69,
      70,    67,    68,    71,    72,    73,    66,    93,    94,    91,
      92,    95,    96,    97,    90,    77,    78,    75,    76,    79,
      80,    81,    74,   101,   102,    99,   100,   103,   104,   105,
      98,    85,    86,    83,    84,    87,    88,    89,    82,   418,
     420,   419,   416,   422,     0,   288,     0,     0,     0,   283,
       0,     0,     0,     0,   412,     0,   282,   284,   286,   412,
     285
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -664,  -664,  -664,   985,  -664,   986,  -664,   546,  -664,   543,
    -664,   467,   474,  -664,  -382,   996,  1000,   896,  -664,  -664,
    1003,  -664,   776,  1004,  1005,   -57,  1047,   -15,   804,   915,
     -42,  -664,  -664,   603,  -664,  -664,  -664,  -664,  -664,  -664,
    -168,  -664,  -664,  -664,  -664,   521,  -104,    23,   452,  -664,
    -664,   925,  -664,  -664,  1011,  1013,  1015,  1016,  1017,  -154,
    -664,   763,  -178,  -180,  -664,  -451,  -447,  -434,  -416,  -415,
    -414,   453,  -664,  -664,  -664,  -664,  -664,  -664,   772,  -664,
    -664,   668,   408,  -202,  -664,  -664,  -664,   477,  -664,  -664,
    -664,  -664,   478,   764,   728,  -294,  -664,  -664,  -664,  -664,
     886,  -389,   487,  -112,   283,   332,  -664,  -664,  -663,  -664,
     406,   269,  -664
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   122,    20,   383,   384,   385,   498,
     582,   583,   675,   386,   279,    21,    22,   167,    23,    61,
      24,   176,   177,    25,    26,    27,    28,    29,    97,   153,
      98,   158,   373,   374,   469,   272,   378,   156,   372,   465,
     179,   711,   628,    95,   459,   460,   461,   462,   560,    30,
      84,    85,   463,   557,    31,    32,    33,    34,    35,   207,
     393,   208,   209,   210,   883,   211,   212,   213,   214,   215,
     216,   567,   568,   217,   218,   219,   220,   221,   309,   222,
     223,   224,   225,   226,   693,   227,   228,   229,   230,   231,
     232,   233,   234,   329,   330,   235,   236,   237,   238,   239,
     240,   512,   513,   181,   108,   100,    91,   105,   584,   588,
     755,   756,   389
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     286,    88,   269,   328,   129,   182,   504,   304,   285,   514,
      96,   445,   308,   561,    47,   325,   326,   562,   274,   178,
     325,   326,   323,   324,    14,   528,   371,    48,   332,    50,
     563,  -407,   116,   117,   426,   266,    82,   520,     1,   429,
       2,     3,     4,     5,     6,     7,     8,     9,   564,   565,
     566,   335,   558,   154,    10,   604,    11,    12,    13,    92,
     356,    93,   280,    94,   106,   357,   241,  -410,   242,   243,
     336,   337,   115,   467,   468,   358,   246,    99,   247,   248,
     359,    77,   184,   185,   186,   251,   136,   252,   253,   685,
     430,   360,   375,   376,   336,   337,   361,   819,   336,   337,
     144,   869,   380,   870,   871,   395,   559,    41,    14,   427,
     268,    14,   607,   519,   183,   184,   185,   186,   586,   336,
     337,    47,   304,   591,   161,   162,   163,   244,   405,   406,
      49,    16,   170,   171,   172,   401,   610,   249,    81,   334,
      83,   118,   284,   336,   337,    92,   254,    93,   275,    94,
     336,   337,   281,   336,   337,   447,    86,   873,   424,   425,
      89,   431,   432,   433,   434,   435,   436,   437,   438,   439,
     440,   441,   442,   443,   444,   561,   336,   337,   399,   562,
     191,   192,   193,   194,   336,   337,   187,   188,   277,   152,
     446,   331,   563,    99,    15,   189,   327,   190,   458,   470,
     205,   327,    90,   677,   204,   336,   337,   195,   196,   197,
     564,   565,   566,   191,   192,   193,   194,   379,    16,   605,
      96,   888,   381,   420,   382,   245,   890,   621,   577,   138,
     139,   523,   524,   578,   526,   250,   333,   530,   507,   334,
     195,   196,   197,   307,   255,   515,   107,   539,    36,    37,
      38,   183,   184,   185,   186,   317,   113,   318,   319,   320,
      39,    40,   198,   205,   577,   362,   183,   184,   185,   186,
     363,   408,   364,   409,   541,   410,   579,   365,   580,   581,
     336,   337,   199,   521,   200,   522,   201,   410,   375,   119,
     404,   202,   203,   204,    51,    52,   205,   569,   206,   400,
      53,   140,   141,   336,   337,   394,    42,    43,    44,   537,
     769,   114,   579,   689,   580,   581,   696,   673,    45,    46,
      78,    79,    80,   187,   188,   771,   109,   110,   111,   112,
     503,   256,   189,   137,   190,   257,   258,   307,   187,   188,
     259,   260,   602,    14,   603,   606,   355,   189,   609,   190,
     191,   192,   193,   194,   199,   120,   200,   121,   201,   183,
     184,   185,   186,   618,   135,   191,   192,   193,   194,   101,
     102,   103,   104,   390,   455,   456,   391,   195,   196,   197,
     501,   142,   516,   502,   615,   334,   143,   517,   531,   535,
     536,   532,   195,   196,   197,   533,   145,   590,   534,   198,
     391,   548,   592,   596,   146,   334,   597,   691,   598,   599,
     147,   597,   334,   540,   198,   617,   148,   678,   334,   199,
     391,   200,   681,   201,   149,   391,   151,   686,   202,   203,
     204,   187,   188,   205,   199,   206,   200,   155,   201,   682,
     189,   157,   190,   202,   203,   204,   701,   160,   205,   334,
     206,   159,   183,   184,   185,   186,   325,   763,   191,   192,
     193,   194,   712,   758,   164,   713,   391,   183,   184,   185,
     186,   165,     1,   168,     2,     3,     4,     5,     6,     7,
       8,     9,   352,   353,   354,   195,   196,   197,    10,   166,
      11,    12,    13,   762,   178,   765,   334,     1,   766,     2,
       3,     4,     5,     6,     7,   816,     9,   198,   817,   818,
     169,   825,   817,    10,   597,    11,    12,    13,   827,   705,
     706,   391,   173,   614,   302,   303,   174,   199,   175,   200,
     180,   201,   773,   189,   261,   190,   202,   203,   204,   302,
     262,   205,   263,   206,   264,    14,   271,   270,   189,   273,
     190,   191,   192,   193,   194,   772,    62,    63,   278,    64,
     509,   510,   511,   282,   283,   287,   191,   192,   193,   194,
      14,    65,    66,   288,   310,   289,   305,   306,   195,   196,
     197,   311,   630,   631,   632,   633,   634,   312,   313,   635,
     636,   314,   826,   195,   196,   197,   316,   321,   322,   367,
     198,   355,   366,   403,   369,   370,   377,   387,   388,   637,
     392,   397,   398,   340,   402,   198,   411,   412,    14,   413,
     199,   414,   200,   415,   201,   416,   417,   418,    15,   202,
     203,   204,  -411,  -411,   205,   199,   206,   200,   419,   201,
     421,   423,   428,   205,   202,   203,   204,   448,   450,   205,
     403,   206,    16,    15,   452,   340,   453,   454,   457,   464,
     466,   338,   500,   339,  -411,  -411,   350,   351,   352,   353,
     354,   403,   341,   342,   343,   344,   499,    16,   506,   508,
     346,   505,    67,    68,    69,   427,   518,    70,    71,   527,
     336,   525,    72,    73,    74,   538,   544,   542,   545,   546,
      75,    76,   340,   547,   549,   347,   348,   349,   350,   351,
     352,   353,   354,   340,   587,   594,   687,   571,   595,   341,
     342,   343,   344,   340,   543,   608,   572,   346,   600,   573,
     341,   342,   343,   344,   345,   574,   575,   611,   346,   612,
     341,   342,   343,   344,   576,   585,   593,   613,   346,   536,
     616,   535,   347,   348,   349,   350,   351,   352,   353,   354,
     619,   623,   620,   347,   348,   349,   350,   351,   352,   353,
     354,   340,   624,   347,   348,   349,   350,   351,   352,   353,
     354,   625,   626,   627,   670,   629,   672,   671,  -411,  -411,
     343,   344,   673,   679,   680,   684,  -411,   290,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   301,   550,
    -208,   551,   552,   553,   554,   688,   555,   556,   690,   692,
     697,  -411,   348,   349,   350,   351,   352,   353,   354,   471,
     472,   473,   474,   475,   476,   477,   478,   479,   480,   481,
     482,   483,   484,   485,   486,   487,   488,   489,   490,   491,
     702,   698,   492,   699,   703,   493,   494,   700,   707,   495,
     496,   497,   638,   639,   640,   641,   642,   709,   714,   643,
     644,   646,   647,   648,   649,   650,   710,   754,   651,   652,
     654,   655,   656,   657,   658,   715,   716,   659,   660,   645,
     717,   718,   662,   663,   664,   665,   666,   719,   653,   667,
     668,    54,    55,    56,    57,    58,    59,   661,   720,    60,
     721,   722,   723,   724,   725,   726,   727,   728,   729,   669,
     760,   730,   731,   732,   761,   733,   770,   734,   735,   736,
     774,   737,   738,   739,   775,   740,   741,   776,   742,   743,
     744,   745,   777,   746,   747,   748,   749,   750,   751,   752,
     753,   764,   767,   768,   778,   779,   780,   781,   782,   783,
     784,   785,   786,   787,   788,   789,   790,   791,   792,   793,
     794,   795,   796,   797,   798,   799,   800,   801,   802,   803,
     804,   805,   806,   807,   808,   809,   810,   811,   812,   813,
     814,   578,   815,   820,   334,   821,   822,   823,   824,   828,
     874,   829,   878,   879,   830,   831,   832,   833,   880,   834,
     835,   836,   837,   838,   839,   840,   841,   842,   843,   844,
     845,   846,   847,   848,   849,   850,   851,   852,   853,   854,
     855,   856,   885,   857,   858,   859,   886,   860,   861,   862,
     863,   864,   865,   866,   867,   868,   875,   123,   589,   674,
     876,   877,   881,   884,   882,   887,   889,   124,   676,   276,
     396,   125,   601,    87,   126,   127,   128,   267,   368,   570,
     622,   265,   130,   704,   131,   150,   132,   133,   134,   708,
     407,   757,   529,   315,   683,   759,   872,   694,   695,   451,
       0,     0,     0,     0,     0,     0,     0,   422,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   449
};

static const yytype_int16 yycheck[] =
{
     178,    16,   156,   205,    61,   117,   388,   187,   176,   398,
       8,     3,   190,   464,     3,     5,     6,   464,    57,    68,
       5,     6,   202,   203,    80,     3,    78,     4,   206,     6,
     464,     0,    78,    79,    77,   147,    13,    88,     7,    75,
       9,    10,    11,    12,    13,    14,    15,    16,   464,   464,
     464,    57,     3,    95,    23,    85,    25,    26,    27,    20,
     186,    22,    34,    24,    41,   191,     3,    64,     5,     6,
     149,   150,    49,    72,    73,   186,     3,    74,     5,     6,
     191,    30,     4,     5,     6,     3,    63,     5,     6,    85,
     126,   186,   270,   271,   149,   150,   191,   760,   149,   150,
      77,     3,     3,     5,     6,   283,    57,    33,    80,   152,
     152,    80,   191,    57,     3,     4,     5,     6,   500,   149,
     150,     3,   302,   505,   101,   102,   103,    64,   306,   307,
      78,   187,   109,   110,   111,   289,   191,    64,     3,   191,
       3,   187,   191,   149,   150,    20,    64,    22,   187,    24,
     149,   150,   167,   149,   150,   357,   164,   820,   336,   337,
       0,   341,   342,   343,   344,   345,   346,   347,   348,   349,
     350,   351,   352,   353,   354,   626,   149,   150,    67,   626,
     102,   103,   104,   105,   149,   150,    75,    76,   165,   187,
     182,   206,   626,    74,   163,    84,   186,    86,   187,   377,
     185,   186,   190,   585,   182,   149,   150,   129,   130,   131,
     626,   626,   626,   102,   103,   104,   105,   274,   187,    88,
       8,   884,   123,   188,   125,   162,   889,   188,    75,   168,
     169,   409,   410,    80,   412,   162,   188,   415,   392,   191,
     129,   130,   131,    89,   162,   399,    74,   427,    29,    30,
      31,     3,     4,     5,     6,   117,   189,   119,   120,   121,
      41,    42,   151,   185,    75,   186,     3,     4,     5,     6,
     191,    85,   186,    87,   428,    89,   123,   191,   125,   126,
     149,   150,   171,    85,   173,    87,   175,    89,   466,     3,
     305,   180,   181,   182,   160,   161,   185,   465,   187,   188,
     166,   168,   169,   149,   150,   282,    29,    30,    31,   421,
     699,    14,   123,   607,   125,   126,   610,   128,    41,    42,
     153,   154,   155,    75,    76,   707,    43,    44,    45,    46,
     387,    42,    84,     6,    86,    46,    47,    89,    75,    76,
      51,    52,   520,    80,   522,   525,   189,    84,   191,    86,
     102,   103,   104,   105,   171,     3,   173,     3,   175,     3,
       4,     5,     6,   543,     3,   102,   103,   104,   105,    37,
      38,    39,    40,   188,    48,    49,   191,   129,   130,   131,
     188,     3,   188,   191,   538,   191,   160,   402,   188,     5,
       6,   191,   129,   130,   131,   188,    57,   188,   191,   151,
     191,   458,   188,   188,   191,   191,   191,   609,   188,   188,
       6,   191,   191,   428,   151,   188,   188,   188,   191,   171,
     191,   173,   188,   175,   188,   191,    21,   605,   180,   181,
     182,    75,    76,   185,   171,   187,   173,    67,   175,   593,
      84,    69,    86,   180,   181,   182,   188,     3,   185,   191,
     187,    75,     3,     4,     5,     6,     5,     6,   102,   103,
     104,   105,   188,   188,     3,   191,   191,     3,     4,     5,
       6,    64,     7,    76,     9,    10,    11,    12,    13,    14,
      15,    16,   182,   183,   184,   129,   130,   131,    23,   187,
      25,    26,    27,   188,    68,   188,   191,     7,   191,     9,
      10,    11,    12,    13,    14,   188,    16,   151,   191,   188,
       3,   188,   191,    23,   191,    25,    26,    27,   188,   623,
     624,   191,     3,   538,    75,    76,     3,   171,     3,   173,
       4,   175,   710,    84,     3,    86,   180,   181,   182,    75,
       3,   185,     6,   187,   187,    80,    70,    55,    84,    76,
      86,   102,   103,   104,   105,   709,    29,    30,     3,    32,
      81,    82,    83,    64,   177,    80,   102,   103,   104,   105,
      80,    44,    45,    80,     4,   187,   187,   187,   129,   130,
     131,     4,    91,    92,    93,    94,    95,     4,     4,    98,
      99,     6,   770,   129,   130,   131,   187,   187,   187,     6,
     151,   189,     3,    75,   188,   188,    71,    57,   187,   118,
     187,   187,   187,   127,   187,   151,     4,   187,    80,   187,
     171,   187,   173,   187,   175,     4,     4,   192,   163,   180,
     181,   182,   146,   147,   185,   171,   187,   173,   188,   175,
      79,     3,   187,   185,   180,   181,   182,     6,     6,   185,
      75,   187,   187,   163,     6,   127,     5,    46,   188,   170,
     191,    75,   187,    77,   178,   179,   180,   181,   182,   183,
     184,    75,   144,   145,   146,   147,   124,   187,     3,   191,
     152,   187,   155,   156,   157,   152,   188,   160,   161,     4,
     149,    78,   165,   166,   167,   187,   192,   126,   192,     6,
     173,   174,   127,     6,   191,   177,   178,   179,   180,   181,
     182,   183,   184,   127,    28,     4,   188,   187,     3,   144,
     145,   146,   147,   127,   149,   191,   187,   152,   188,   187,
     144,   145,   146,   147,   148,   187,   187,     4,   152,     4,
     144,   145,   146,   147,   187,   187,   187,    80,   152,     6,
     188,     5,   177,   178,   179,   180,   181,   182,   183,   184,
      53,    59,    50,   177,   178,   179,   180,   181,   182,   183,
     184,   127,    59,   177,   178,   179,   180,   181,   182,   183,
     184,     3,   191,    54,   126,     6,   124,   187,   144,   145,
     146,   147,   128,   187,   159,   188,   152,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,    58,
      59,    60,    61,    62,    63,   185,    65,    66,     4,   185,
     188,   177,   178,   179,   180,   181,   182,   183,   184,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
       6,   188,   113,   187,     6,   116,   117,   188,   187,   120,
     121,   122,    91,    92,    93,    94,    95,    55,   191,    98,
      99,    91,    92,    93,    94,    95,    56,     3,    98,    99,
      91,    92,    93,    94,    95,   191,   191,    98,    99,   118,
     191,   191,    91,    92,    93,    94,    95,   191,   118,    98,
      99,    35,    36,    37,    38,    39,    40,   118,   191,    43,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   118,
       3,   191,   191,   191,   159,   191,    64,   191,   191,   191,
       6,   191,   191,   191,     6,   191,   191,     6,   191,   191,
     191,   191,     6,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,    80,   177,     3,   191,     4,     4,     4,     4,   188,
     191,   188,     4,     4,   188,   188,   188,   188,     6,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,     6,   188,   188,   188,     4,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,    61,   502,   582,
     191,   191,   191,   188,   191,   188,   188,    61,   584,   163,
     284,    61,   519,    16,    61,    61,    61,   152,   264,   466,
     549,   146,    61,   621,    61,    90,    61,    61,    61,   626,
     308,   673,   414,   197,   597,   679,   817,   610,   610,   361,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   334,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   359
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   163,   187,   194,   195,   196,
     198,   208,   209,   211,   213,   216,   217,   218,   219,   220,
     242,   247,   248,   249,   250,   251,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   240,    78,
     240,   160,   161,   166,    35,    36,    37,    38,    39,    40,
      43,   212,    29,    30,    32,    44,    45,   155,   156,   157,
     160,   161,   165,   166,   167,   173,   174,    30,   153,   154,
     155,     3,   240,     3,   243,   244,   164,   219,   220,     0,
     190,   299,    20,    22,    24,   236,     8,   221,   223,    74,
     298,   298,   298,   298,   298,   300,   240,    74,   297,   297,
     297,   297,   297,   189,    14,   240,    78,    79,   187,     3,
       3,     3,   197,   198,   208,   209,   213,   216,   217,   218,
     247,   248,   249,   250,   251,     3,   240,     6,   168,   169,
     168,   169,     3,   160,   240,    57,   191,     6,   188,   188,
     196,    21,   187,   222,   223,    67,   230,    69,   224,    75,
       3,   240,   240,   240,     3,    64,   187,   210,    76,     3,
     240,   240,   240,     3,     3,     3,   214,   215,    68,   233,
       4,   296,   296,     3,     4,     5,     6,    75,    76,    84,
      86,   102,   103,   104,   105,   129,   130,   131,   151,   171,
     173,   175,   180,   181,   182,   185,   187,   252,   254,   255,
     256,   258,   259,   260,   261,   262,   263,   266,   267,   268,
     269,   270,   272,   273,   274,   275,   276,   278,   279,   280,
     281,   282,   283,   284,   285,   288,   289,   290,   291,   292,
     293,     3,     5,     6,    64,   162,     3,     5,     6,    64,
     162,     3,     5,     6,    64,   162,    42,    46,    47,    51,
      52,     3,     3,     6,   187,   244,   296,   222,   223,   252,
      55,    70,   228,    76,    57,   187,   210,   240,     3,   207,
      34,   220,    64,   177,   191,   233,   255,    80,    80,   187,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,    75,    76,   256,   187,   187,    89,   255,   271,
       4,     4,     4,     4,     6,   293,   187,   117,   119,   120,
     121,   187,   187,   256,   256,     5,     6,   186,   276,   286,
     287,   220,   255,   188,   191,    57,   149,   150,    75,    77,
     127,   144,   145,   146,   147,   148,   152,   177,   178,   179,
     180,   181,   182,   183,   184,   189,   186,   191,   186,   191,
     186,   191,   186,   191,   186,   191,     3,     6,   221,   188,
     188,    78,   231,   225,   226,   255,   255,    71,   229,   218,
       3,   123,   125,   199,   200,   201,   206,    57,   187,   305,
     188,   191,   187,   253,   240,   255,   215,   187,   187,    67,
     188,   252,   187,    75,   220,   255,   255,   271,    85,    87,
      89,     4,   187,   187,   187,   187,     4,     4,   192,   188,
     188,    79,   254,     3,   255,   255,    77,   152,   187,    75,
     126,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   256,     3,   182,   276,     6,   286,
       6,   287,     6,     5,    46,    48,    49,   188,   187,   237,
     238,   239,   240,   245,   170,   232,   191,    72,    73,   227,
     255,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   113,   116,   117,   120,   121,   122,   202,   124,
     187,   188,   191,   218,   207,   187,     3,   252,   191,    81,
      82,    83,   294,   295,   294,   252,   188,   220,   188,    57,
      88,    85,    87,   255,   255,    78,   255,     4,     3,   274,
     255,   188,   191,   188,   191,     5,     6,   296,   187,   256,
     220,   252,   126,   149,   192,   192,     6,     6,   218,   191,
      58,    60,    61,    62,    63,    65,    66,   246,     3,    57,
     241,   258,   259,   260,   261,   262,   263,   264,   265,   233,
     226,   187,   187,   187,   187,   187,   187,    75,    80,   123,
     125,   126,   203,   204,   301,   187,   207,    28,   302,   200,
     188,   207,   188,   187,     4,     3,   188,   191,   188,   188,
     188,   202,   255,   255,    85,    88,   256,   191,   191,   191,
     191,     4,     4,    80,   220,   252,   188,   188,   256,    53,
      50,   188,   238,    59,    59,     3,   191,    54,   235,     6,
      91,    92,    93,    94,    95,    98,    99,   118,    91,    92,
      93,    94,    95,    98,    99,   118,    91,    92,    93,    94,
      95,    98,    99,   118,    91,    92,    93,    94,    95,    98,
      99,   118,    91,    92,    93,    94,    95,    98,    99,   118,
     126,   187,   124,   128,   204,   205,   205,   207,   188,   187,
     159,   188,   252,   295,   188,    85,   255,   188,   185,   288,
       4,   276,   185,   277,   280,   285,   288,   188,   188,   187,
     188,   188,     6,     6,   241,   239,   239,   187,   264,    55,
      56,   234,   188,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,     3,   303,   304,   275,   188,   303,
       3,   159,   188,     6,   191,   188,   191,   191,   191,   294,
      64,   207,   252,   255,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   177,   188,   191,   188,   301,
       3,     4,     4,     4,     4,   188,   255,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,     3,
       5,     6,   304,   301,   191,   188,   191,   191,     4,     4,
       6,   191,   191,   257,   188,     6,     4,   188,   301,   188,
     301
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   193,   194,   195,   195,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     198,   198,   198,   198,   198,   198,   199,   199,   200,   200,
     201,   201,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   203,   203,   204,   204,
     204,   204,   205,   205,   206,   206,   207,   207,   208,   209,
     209,   210,   210,   211,   212,   212,   212,   212,   212,   212,
     212,   212,   213,   214,   214,   215,   216,   216,   216,   216,
     216,   217,   217,   217,   218,   218,   218,   218,   219,   219,
     220,   221,   222,   222,   223,   224,   224,   225,   225,   226,
     227,   227,   227,   228,   228,   229,   229,   230,   230,   231,
     231,   232,   232,   233,   233,   234,   234,   235,   235,   236,
     236,   236,   236,   237,   237,   238,   238,   239,   239,   240,
     240,   241,   241,   241,   241,   242,   242,   243,   243,   244,
     245,   245,   246,   246,   246,   246,   246,   246,   246,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   247,   248,   248,   248,   249,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   251,   252,   252,
     253,   253,   254,   254,   255,   255,   255,   255,   255,   256,
     256,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   257,   257,   258,   259,   260,   261,   261,   262,
     262,   263,   263,   264,   264,   264,   264,   264,   264,   265,
     265,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   267,   267,   268,   269,   269,   270,
     270,   270,   270,   271,   271,   272,   273,   273,   273,   273,
     274,   274,   274,   274,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   276,   276,   276,   276,
     277,   277,   277,   278,   279,   279,   280,   280,   281,   282,
     282,   283,   284,   284,   285,   286,   287,   288,   288,   289,
     290,   290,   291,   292,   292,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   294,   294,   295,
     295,   295,   296,   297,   297,   298,   298,   299,   299,   300,
     300,   301,   301,   302,   302,   303,   303,   304,   304,   304,
     304,   305,   305,   305
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     8,     6,     7,     6,     1,     3,     1,     1,
       4,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     4,     1,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     1,     2,     2,     1,
       1,     2,     2,     0,     5,     4,     1,     3,     4,     6,
       5,     3,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     0,     5,     1,     3,     3,     4,     4,     4,     4,
       6,     8,    11,     8,     1,     1,     3,     3,     3,     3,
       2,     4,     3,     3,     8,     3,     0,     1,     3,     2,
       1,     1,     0,     2,     0,     2,     0,     1,     0,     2,
       0,     2,     0,     2,     0,     2,     0,     3,     0,     1,
       2,     1,     1,     1,     3,     1,     1,     2,     4,     1,
       3,     2,     1,     5,     0,     2,     0,     1,     3,     5,
       4,     6,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     2,     2,     3,     2,     2,     2,     4,     2,     3,
       3,     3,     4,     4,     3,     3,     4,     4,     5,     6,
       7,     9,     4,     5,     7,     9,     2,     2,     2,     2,
       2,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     3,     1,     3,
       3,     5,     3,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     0,    12,    14,    12,     7,     9,     4,
       6,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     4,     5,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
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
#line 2195 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2203 "parser.cpp"
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
#line 2217 "parser.cpp"
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
#line 2231 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2242 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2250 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2259 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2268 "parser.cpp"
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
#line 2282 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2293 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2303 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2313 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2323 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2333 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2343 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2353 "parser.cpp"
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
#line 2367 "parser.cpp"
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
#line 2381 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2391 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2399 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2407 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2416 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2424 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2432 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2440 "parser.cpp"
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
#line 2454 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2463 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2472 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2481 "parser.cpp"
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
#line 2494 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2503 "parser.cpp"
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
#line 2517 "parser.cpp"
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
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2541 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2550 "parser.cpp"
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
#line 2564 "parser.cpp"
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
#line 2581 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2589 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2597 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2613 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2621 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2637 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2645 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2653 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2661 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2669 "parser.cpp"
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
#line 2683 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2691 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2699 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2715 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2723 "parser.cpp"
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
#line 2736 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2744 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2752 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2760 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2776 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2784 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2800 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2808 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2816 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2824 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2840 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 364 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2856 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 368 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2864 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2872 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2880 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2888 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2896 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2904 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2912 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2920 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2931 "parser.cpp"
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
#line 2945 "parser.cpp"
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
#line 2959 "parser.cpp"
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
#line 2973 "parser.cpp"
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

#line 3081 "parser.cpp"

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
#line 3296 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 496 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3307 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 502 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3318 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 509 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3324 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3330 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3336 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3342 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3348 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3354 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3360 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3366 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3372 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 518 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3378 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 519 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3384 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3390 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3396 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 523 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3402 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3408 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3414 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3420 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3426 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3432 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3438 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3444 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 531 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3450 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 532 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3456 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 533 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3462 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3468 "parser.cpp"
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
#line 3488 "parser.cpp"
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
#line 3506 "parser.cpp"
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
#line 3539 "parser.cpp"
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
#line 3559 "parser.cpp"
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
#line 3580 "parser.cpp"
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
#line 3613 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 666 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3622 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 670 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3631 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 676 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3639 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 679 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3647 "parser.cpp"
    break;

  case 40: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 686 "parser.y"
                                                          {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-1].with_index_param_list_t));
    switch((yyvsp[-2].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-2].column_type_t).precision, (yyvsp[-2].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                free((yyvsp[-3].str_value));
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
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-2].column_type_t).embedding_type_, (yyvsp[-2].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kSparse: {
            auto store_type = infinity::SparseInfo::ParseStoreType(index_param_list);
            type_info_ptr = infinity::SparseInfo::Make((yyvsp[-2].column_type_t).embedding_type_, (yyvsp[-2].column_type_t).width, store_type);
            if (type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create sparse info.");
                free((yyvsp[-3].str_value));
                YYERROR;
            }
            break;
        }
        default: {
            break;
        }
    }

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, std::move(default_expr));

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3702 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 736 "parser.y"
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
#line 3741 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 772 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3747 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 773 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3753 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 774 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3759 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3765 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 776 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3771 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 777 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3777 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3783 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3789 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 780 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3795 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 781 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3801 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 782 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3807 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 783 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3813 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 784 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3819 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 785 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3825 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 786 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3831 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 787 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3837 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 788 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3843 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 789 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3849 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 790 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3855 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 793 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3861 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 795 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3867 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 796 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3873 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 797 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3879 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 798 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3885 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3891 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3897 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3903 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3909 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3915 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3921 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3927 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3933 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3939 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3945 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3951 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3957 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3963 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3969 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3975 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3981 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3987 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3993 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3999 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4005 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4011 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4017 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4023 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4029 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4035 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4041 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4047 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4053 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4059 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4065 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4071 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4077 "parser.cpp"
    break;

  case 98: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4083 "parser.cpp"
    break;

  case 99: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4089 "parser.cpp"
    break;

  case 100: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4095 "parser.cpp"
    break;

  case 101: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4101 "parser.cpp"
    break;

  case 102: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4107 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4113 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4119 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4125 "parser.cpp"
    break;

  case 106: /* column_constraints: column_constraint  */
#line 859 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4134 "parser.cpp"
    break;

  case 107: /* column_constraints: column_constraints column_constraint  */
#line 863 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4148 "parser.cpp"
    break;

  case 108: /* column_constraint: PRIMARY KEY  */
#line 873 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4156 "parser.cpp"
    break;

  case 109: /* column_constraint: UNIQUE  */
#line 876 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4164 "parser.cpp"
    break;

  case 110: /* column_constraint: NULLABLE  */
#line 879 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4172 "parser.cpp"
    break;

  case 111: /* column_constraint: NOT NULLABLE  */
#line 882 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4180 "parser.cpp"
    break;

  case 112: /* default_expr: DEFAULT constant_expr  */
#line 886 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4188 "parser.cpp"
    break;

  case 113: /* default_expr: %empty  */
#line 889 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4196 "parser.cpp"
    break;

  case 114: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 894 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4206 "parser.cpp"
    break;

  case 115: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 899 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4216 "parser.cpp"
    break;

  case 116: /* identifier_array: IDENTIFIER  */
#line 906 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4227 "parser.cpp"
    break;

  case 117: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 912 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4238 "parser.cpp"
    break;

  case 118: /* delete_statement: DELETE FROM table_name where_clause  */
#line 922 "parser.y"
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
#line 4255 "parser.cpp"
    break;

  case 119: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 938 "parser.y"
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
#line 4294 "parser.cpp"
    break;

  case 120: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 972 "parser.y"
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
#line 4311 "parser.cpp"
    break;

  case 121: /* optional_identifier_array: '(' identifier_array ')'  */
#line 985 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4319 "parser.cpp"
    break;

  case 122: /* optional_identifier_array: %empty  */
#line 988 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4327 "parser.cpp"
    break;

  case 123: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 995 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4337 "parser.cpp"
    break;

  case 124: /* explain_type: ANALYZE  */
#line 1001 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4345 "parser.cpp"
    break;

  case 125: /* explain_type: AST  */
#line 1004 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4353 "parser.cpp"
    break;

  case 126: /* explain_type: RAW  */
#line 1007 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4361 "parser.cpp"
    break;

  case 127: /* explain_type: LOGICAL  */
#line 1010 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4369 "parser.cpp"
    break;

  case 128: /* explain_type: PHYSICAL  */
#line 1013 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4377 "parser.cpp"
    break;

  case 129: /* explain_type: PIPELINE  */
#line 1016 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4385 "parser.cpp"
    break;

  case 130: /* explain_type: FRAGMENT  */
#line 1019 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4393 "parser.cpp"
    break;

  case 131: /* explain_type: %empty  */
#line 1022 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4401 "parser.cpp"
    break;

  case 132: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1029 "parser.y"
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
#line 4418 "parser.cpp"
    break;

  case 133: /* update_expr_array: update_expr  */
#line 1042 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4427 "parser.cpp"
    break;

  case 134: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1046 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4436 "parser.cpp"
    break;

  case 135: /* update_expr: IDENTIFIER '=' expr  */
#line 1051 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4448 "parser.cpp"
    break;

  case 136: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1064 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4464 "parser.cpp"
    break;

  case 137: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1077 "parser.y"
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
#line 4482 "parser.cpp"
    break;

  case 138: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1092 "parser.y"
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
#line 4500 "parser.cpp"
    break;

  case 139: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1107 "parser.y"
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
#line 4518 "parser.cpp"
    break;

  case 140: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1122 "parser.y"
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
#line 4541 "parser.cpp"
    break;

  case 141: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1145 "parser.y"
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
#line 4587 "parser.cpp"
    break;

  case 142: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1186 "parser.y"
                                                                                {
    (yyval.copy_stmt) = new infinity::CopyStatement();

    // Copy To
    (yyval.copy_stmt)->copy_from_ = false;

    // table_name
    if((yyvsp[-9].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.copy_stmt)->schema_name_ = (yyvsp[-9].table_name_t)->schema_name_ptr_;
        free((yyvsp[-9].table_name_t)->schema_name_ptr_);
    }
    (yyval.copy_stmt)->table_name_ = (yyvsp[-9].table_name_t)->table_name_ptr_;
    free((yyvsp[-9].table_name_t)->table_name_ptr_);
    delete (yyvsp[-9].table_name_t);

    (yyval.copy_stmt)->expr_array_ = (yyvsp[-7].expr_array_t);

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
#line 4635 "parser.cpp"
    break;

  case 143: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1229 "parser.y"
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
#line 4681 "parser.cpp"
    break;

  case 144: /* select_statement: select_without_paren  */
#line 1274 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4689 "parser.cpp"
    break;

  case 145: /* select_statement: select_with_paren  */
#line 1277 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4697 "parser.cpp"
    break;

  case 146: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1280 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4711 "parser.cpp"
    break;

  case 147: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1289 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4725 "parser.cpp"
    break;

  case 148: /* select_with_paren: '(' select_without_paren ')'  */
#line 1299 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4733 "parser.cpp"
    break;

  case 149: /* select_with_paren: '(' select_with_paren ')'  */
#line 1302 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4741 "parser.cpp"
    break;

  case 150: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1306 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4750 "parser.cpp"
    break;

  case 151: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1311 "parser.y"
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
#line 4776 "parser.cpp"
    break;

  case 152: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1333 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4784 "parser.cpp"
    break;

  case 153: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1336 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4792 "parser.cpp"
    break;

  case 154: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1341 "parser.y"
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
#line 4812 "parser.cpp"
    break;

  case 155: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1357 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4820 "parser.cpp"
    break;

  case 156: /* order_by_clause: %empty  */
#line 1360 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4828 "parser.cpp"
    break;

  case 157: /* order_by_expr_list: order_by_expr  */
#line 1364 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4837 "parser.cpp"
    break;

  case 158: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1368 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4846 "parser.cpp"
    break;

  case 159: /* order_by_expr: expr order_by_type  */
#line 1373 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4856 "parser.cpp"
    break;

  case 160: /* order_by_type: ASC  */
#line 1379 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4864 "parser.cpp"
    break;

  case 161: /* order_by_type: DESC  */
#line 1382 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4872 "parser.cpp"
    break;

  case 162: /* order_by_type: %empty  */
#line 1385 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4880 "parser.cpp"
    break;

  case 163: /* limit_expr: LIMIT expr  */
#line 1389 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4888 "parser.cpp"
    break;

  case 164: /* limit_expr: %empty  */
#line 1393 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4894 "parser.cpp"
    break;

  case 165: /* offset_expr: OFFSET expr  */
#line 1395 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4902 "parser.cpp"
    break;

  case 166: /* offset_expr: %empty  */
#line 1399 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4908 "parser.cpp"
    break;

  case 167: /* distinct: DISTINCT  */
#line 1401 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4916 "parser.cpp"
    break;

  case 168: /* distinct: %empty  */
#line 1404 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4924 "parser.cpp"
    break;

  case 169: /* from_clause: FROM table_reference  */
#line 1408 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4932 "parser.cpp"
    break;

  case 170: /* from_clause: %empty  */
#line 1411 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4940 "parser.cpp"
    break;

  case 171: /* search_clause: SEARCH sub_search_array  */
#line 1415 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4950 "parser.cpp"
    break;

  case 172: /* search_clause: %empty  */
#line 1420 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4958 "parser.cpp"
    break;

  case 173: /* where_clause: WHERE expr  */
#line 1424 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4966 "parser.cpp"
    break;

  case 174: /* where_clause: %empty  */
#line 1427 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4974 "parser.cpp"
    break;

  case 175: /* having_clause: HAVING expr  */
#line 1431 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4982 "parser.cpp"
    break;

  case 176: /* having_clause: %empty  */
#line 1434 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4990 "parser.cpp"
    break;

  case 177: /* group_by_clause: GROUP BY expr_array  */
#line 1438 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4998 "parser.cpp"
    break;

  case 178: /* group_by_clause: %empty  */
#line 1441 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5006 "parser.cpp"
    break;

  case 179: /* set_operator: UNION  */
#line 1445 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5014 "parser.cpp"
    break;

  case 180: /* set_operator: UNION ALL  */
#line 1448 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5022 "parser.cpp"
    break;

  case 181: /* set_operator: INTERSECT  */
#line 1451 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5030 "parser.cpp"
    break;

  case 182: /* set_operator: EXCEPT  */
#line 1454 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5038 "parser.cpp"
    break;

  case 183: /* table_reference: table_reference_unit  */
#line 1462 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5046 "parser.cpp"
    break;

  case 184: /* table_reference: table_reference ',' table_reference_unit  */
#line 1465 "parser.y"
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
#line 5064 "parser.cpp"
    break;

  case 187: /* table_reference_name: table_name table_alias  */
#line 1482 "parser.y"
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
#line 5082 "parser.cpp"
    break;

  case 188: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1496 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5093 "parser.cpp"
    break;

  case 189: /* table_name: IDENTIFIER  */
#line 1505 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5103 "parser.cpp"
    break;

  case 190: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1510 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5115 "parser.cpp"
    break;

  case 191: /* table_alias: AS IDENTIFIER  */
#line 1519 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5125 "parser.cpp"
    break;

  case 192: /* table_alias: IDENTIFIER  */
#line 1524 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5135 "parser.cpp"
    break;

  case 193: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1529 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5146 "parser.cpp"
    break;

  case 194: /* table_alias: %empty  */
#line 1535 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5154 "parser.cpp"
    break;

  case 195: /* with_clause: WITH with_expr_list  */
#line 1542 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5162 "parser.cpp"
    break;

  case 196: /* with_clause: %empty  */
#line 1545 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5170 "parser.cpp"
    break;

  case 197: /* with_expr_list: with_expr  */
#line 1549 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5179 "parser.cpp"
    break;

  case 198: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1552 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5188 "parser.cpp"
    break;

  case 199: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1557 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5200 "parser.cpp"
    break;

  case 200: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1569 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5212 "parser.cpp"
    break;

  case 201: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1576 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5225 "parser.cpp"
    break;

  case 202: /* join_type: INNER  */
#line 1590 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5233 "parser.cpp"
    break;

  case 203: /* join_type: LEFT  */
#line 1593 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5241 "parser.cpp"
    break;

  case 204: /* join_type: RIGHT  */
#line 1596 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5249 "parser.cpp"
    break;

  case 205: /* join_type: OUTER  */
#line 1599 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5257 "parser.cpp"
    break;

  case 206: /* join_type: FULL  */
#line 1602 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5265 "parser.cpp"
    break;

  case 207: /* join_type: CROSS  */
#line 1605 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5273 "parser.cpp"
    break;

  case 208: /* join_type: %empty  */
#line 1608 "parser.y"
                {
}
#line 5280 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW DATABASES  */
#line 1614 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5289 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW TABLES  */
#line 1618 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5298 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW VIEWS  */
#line 1622 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5307 "parser.cpp"
    break;

  case 212: /* show_statement: SHOW CONFIGS  */
#line 1626 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5316 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1630 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5328 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW PROFILES  */
#line 1637 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5337 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW BUFFER  */
#line 1641 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5346 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW QUERIES  */
#line 1645 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5355 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1649 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5365 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW TRANSACTIONS  */
#line 1654 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5374 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1658 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5384 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW SESSION VARIABLES  */
#line 1663 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5393 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1667 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5402 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1671 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5413 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1677 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5424 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1683 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5435 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW TABLE table_name  */
#line 1689 "parser.y"
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
#line 5451 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1700 "parser.y"
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
#line 5467 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1711 "parser.y"
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
#line 5483 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1722 "parser.y"
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
#line 5500 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1734 "parser.y"
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
#line 5517 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1746 "parser.y"
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
#line 5535 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1759 "parser.y"
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
#line 5554 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1773 "parser.y"
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
#line 5570 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1784 "parser.y"
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
#line 5589 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1798 "parser.y"
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
#line 5610 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1814 "parser.y"
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
#line 5632 "parser.cpp"
    break;

  case 236: /* flush_statement: FLUSH DATA  */
#line 1835 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5641 "parser.cpp"
    break;

  case 237: /* flush_statement: FLUSH LOG  */
#line 1839 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5650 "parser.cpp"
    break;

  case 238: /* flush_statement: FLUSH BUFFER  */
#line 1843 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5659 "parser.cpp"
    break;

  case 239: /* optimize_statement: OPTIMIZE table_name  */
#line 1851 "parser.y"
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
#line 5674 "parser.cpp"
    break;

  case 240: /* command_statement: USE IDENTIFIER  */
#line 1865 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5685 "parser.cpp"
    break;

  case 241: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1871 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5695 "parser.cpp"
    break;

  case 242: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1876 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5706 "parser.cpp"
    break;

  case 243: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1882 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5717 "parser.cpp"
    break;

  case 244: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1888 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5730 "parser.cpp"
    break;

  case 245: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1896 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5741 "parser.cpp"
    break;

  case 246: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1902 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5752 "parser.cpp"
    break;

  case 247: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1908 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5763 "parser.cpp"
    break;

  case 248: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1914 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5774 "parser.cpp"
    break;

  case 249: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1920 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5787 "parser.cpp"
    break;

  case 250: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1928 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5798 "parser.cpp"
    break;

  case 251: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1934 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5809 "parser.cpp"
    break;

  case 252: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1940 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5820 "parser.cpp"
    break;

  case 253: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1946 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5831 "parser.cpp"
    break;

  case 254: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1952 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5844 "parser.cpp"
    break;

  case 255: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1960 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5855 "parser.cpp"
    break;

  case 256: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1966 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5866 "parser.cpp"
    break;

  case 257: /* compact_statement: COMPACT TABLE table_name  */
#line 1973 "parser.y"
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
#line 5883 "parser.cpp"
    break;

  case 258: /* expr_array: expr_alias  */
#line 1990 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5892 "parser.cpp"
    break;

  case 259: /* expr_array: expr_array ',' expr_alias  */
#line 1994 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5901 "parser.cpp"
    break;

  case 260: /* expr_array_list: '(' expr_array ')'  */
#line 1999 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5910 "parser.cpp"
    break;

  case 261: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2003 "parser.y"
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
#line 5930 "parser.cpp"
    break;

  case 262: /* expr_alias: expr AS IDENTIFIER  */
#line 2030 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5941 "parser.cpp"
    break;

  case 263: /* expr_alias: expr  */
#line 2036 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5949 "parser.cpp"
    break;

  case 269: /* operand: '(' expr ')'  */
#line 2046 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5957 "parser.cpp"
    break;

  case 270: /* operand: '(' select_without_paren ')'  */
#line 2049 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5968 "parser.cpp"
    break;

  case 271: /* operand: constant_expr  */
#line 2055 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5976 "parser.cpp"
    break;

  case 282: /* extra_match_tensor_option: ',' STRING  */
#line 2069 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5984 "parser.cpp"
    break;

  case 283: /* extra_match_tensor_option: %empty  */
#line 2072 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5992 "parser.cpp"
    break;

  case 284: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2078 "parser.y"
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
#line 6013 "parser.cpp"
    break;

  case 285: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2097 "parser.y"
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
#line 6059 "parser.cpp"
    break;

  case 286: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2141 "parser.y"
                                                                                                                          {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-8].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-6].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-4].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-2].long_value), (yyvsp[0].with_index_param_list_t));
}
#line 6081 "parser.cpp"
    break;

  case 287: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2159 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6094 "parser.cpp"
    break;

  case 288: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2167 "parser.y"
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
#line 6109 "parser.cpp"
    break;

  case 289: /* query_expr: QUERY '(' STRING ')'  */
#line 2178 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6120 "parser.cpp"
    break;

  case 290: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2184 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6133 "parser.cpp"
    break;

  case 291: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2193 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6144 "parser.cpp"
    break;

  case 292: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2199 "parser.y"
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
#line 6160 "parser.cpp"
    break;

  case 293: /* sub_search: match_vector_expr  */
#line 2211 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6168 "parser.cpp"
    break;

  case 294: /* sub_search: match_text_expr  */
#line 2214 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6176 "parser.cpp"
    break;

  case 295: /* sub_search: match_tensor_expr  */
#line 2217 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6184 "parser.cpp"
    break;

  case 296: /* sub_search: match_sparse_expr  */
#line 2220 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6192 "parser.cpp"
    break;

  case 297: /* sub_search: query_expr  */
#line 2223 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6200 "parser.cpp"
    break;

  case 298: /* sub_search: fusion_expr  */
#line 2226 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6208 "parser.cpp"
    break;

  case 299: /* sub_search_array: sub_search  */
#line 2230 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6217 "parser.cpp"
    break;

  case 300: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2234 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6226 "parser.cpp"
    break;

  case 301: /* function_expr: IDENTIFIER '(' ')'  */
#line 2239 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6239 "parser.cpp"
    break;

  case 302: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2247 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6252 "parser.cpp"
    break;

  case 303: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2255 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6266 "parser.cpp"
    break;

  case 304: /* function_expr: operand IS NOT NULLABLE  */
#line 2264 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6278 "parser.cpp"
    break;

  case 305: /* function_expr: operand IS NULLABLE  */
#line 2271 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6290 "parser.cpp"
    break;

  case 306: /* function_expr: NOT operand  */
#line 2278 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6302 "parser.cpp"
    break;

  case 307: /* function_expr: '-' operand  */
#line 2285 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6314 "parser.cpp"
    break;

  case 308: /* function_expr: '+' operand  */
#line 2292 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6326 "parser.cpp"
    break;

  case 309: /* function_expr: operand '-' operand  */
#line 2299 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6339 "parser.cpp"
    break;

  case 310: /* function_expr: operand '+' operand  */
#line 2307 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6352 "parser.cpp"
    break;

  case 311: /* function_expr: operand '*' operand  */
#line 2315 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6365 "parser.cpp"
    break;

  case 312: /* function_expr: operand '/' operand  */
#line 2323 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6378 "parser.cpp"
    break;

  case 313: /* function_expr: operand '%' operand  */
#line 2331 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6391 "parser.cpp"
    break;

  case 314: /* function_expr: operand '=' operand  */
#line 2339 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6404 "parser.cpp"
    break;

  case 315: /* function_expr: operand EQUAL operand  */
#line 2347 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6417 "parser.cpp"
    break;

  case 316: /* function_expr: operand NOT_EQ operand  */
#line 2355 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6430 "parser.cpp"
    break;

  case 317: /* function_expr: operand '<' operand  */
#line 2363 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6443 "parser.cpp"
    break;

  case 318: /* function_expr: operand '>' operand  */
#line 2371 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6456 "parser.cpp"
    break;

  case 319: /* function_expr: operand LESS_EQ operand  */
#line 2379 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6469 "parser.cpp"
    break;

  case 320: /* function_expr: operand GREATER_EQ operand  */
#line 2387 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6482 "parser.cpp"
    break;

  case 321: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2395 "parser.y"
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
#line 6517 "parser.cpp"
    break;

  case 322: /* function_expr: operand LIKE operand  */
#line 2425 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6530 "parser.cpp"
    break;

  case 323: /* function_expr: operand NOT LIKE operand  */
#line 2433 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6543 "parser.cpp"
    break;

  case 324: /* conjunction_expr: expr AND expr  */
#line 2442 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6556 "parser.cpp"
    break;

  case 325: /* conjunction_expr: expr OR expr  */
#line 2450 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6569 "parser.cpp"
    break;

  case 326: /* between_expr: operand BETWEEN operand AND operand  */
#line 2459 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6581 "parser.cpp"
    break;

  case 327: /* in_expr: operand IN '(' expr_array ')'  */
#line 2467 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6592 "parser.cpp"
    break;

  case 328: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2473 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6603 "parser.cpp"
    break;

  case 329: /* case_expr: CASE expr case_check_array END  */
#line 2480 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6614 "parser.cpp"
    break;

  case 330: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2486 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6626 "parser.cpp"
    break;

  case 331: /* case_expr: CASE case_check_array END  */
#line 2493 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6636 "parser.cpp"
    break;

  case 332: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2498 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6647 "parser.cpp"
    break;

  case 333: /* case_check_array: WHEN expr THEN expr  */
#line 2505 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6659 "parser.cpp"
    break;

  case 334: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2512 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6671 "parser.cpp"
    break;

  case 335: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2520 "parser.y"
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
#line 6699 "parser.cpp"
    break;

  case 336: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2544 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6710 "parser.cpp"
    break;

  case 337: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2550 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6721 "parser.cpp"
    break;

  case 338: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2556 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6733 "parser.cpp"
    break;

  case 339: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2563 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6745 "parser.cpp"
    break;

  case 340: /* column_expr: IDENTIFIER  */
#line 2571 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6757 "parser.cpp"
    break;

  case 341: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2578 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6769 "parser.cpp"
    break;

  case 342: /* column_expr: '*'  */
#line 2585 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6779 "parser.cpp"
    break;

  case 343: /* column_expr: column_expr '.' '*'  */
#line 2590 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6793 "parser.cpp"
    break;

  case 344: /* constant_expr: STRING  */
#line 2600 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6803 "parser.cpp"
    break;

  case 345: /* constant_expr: TRUE  */
#line 2605 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6813 "parser.cpp"
    break;

  case 346: /* constant_expr: FALSE  */
#line 2610 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6823 "parser.cpp"
    break;

  case 347: /* constant_expr: DOUBLE_VALUE  */
#line 2615 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6833 "parser.cpp"
    break;

  case 348: /* constant_expr: LONG_VALUE  */
#line 2620 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6843 "parser.cpp"
    break;

  case 349: /* constant_expr: DATE STRING  */
#line 2625 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6853 "parser.cpp"
    break;

  case 350: /* constant_expr: TIME STRING  */
#line 2630 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6863 "parser.cpp"
    break;

  case 351: /* constant_expr: DATETIME STRING  */
#line 2635 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6873 "parser.cpp"
    break;

  case 352: /* constant_expr: TIMESTAMP STRING  */
#line 2640 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6883 "parser.cpp"
    break;

  case 353: /* constant_expr: INTERVAL interval_expr  */
#line 2645 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6891 "parser.cpp"
    break;

  case 354: /* constant_expr: interval_expr  */
#line 2648 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6899 "parser.cpp"
    break;

  case 355: /* constant_expr: common_array_expr  */
#line 2651 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6907 "parser.cpp"
    break;

  case 356: /* common_array_expr: array_expr  */
#line 2655 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6915 "parser.cpp"
    break;

  case 357: /* common_array_expr: subarray_array_expr  */
#line 2658 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6923 "parser.cpp"
    break;

  case 358: /* common_array_expr: sparse_array_expr  */
#line 2661 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6931 "parser.cpp"
    break;

  case 359: /* common_array_expr: empty_array_expr  */
#line 2664 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6939 "parser.cpp"
    break;

  case 360: /* common_sparse_array_expr: sparse_array_expr  */
#line 2668 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6947 "parser.cpp"
    break;

  case 361: /* common_sparse_array_expr: array_expr  */
#line 2671 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6955 "parser.cpp"
    break;

  case 362: /* common_sparse_array_expr: empty_array_expr  */
#line 2674 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6963 "parser.cpp"
    break;

  case 363: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2678 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6971 "parser.cpp"
    break;

  case 364: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2682 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 6981 "parser.cpp"
    break;

  case 365: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2687 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6990 "parser.cpp"
    break;

  case 366: /* sparse_array_expr: long_sparse_array_expr  */
#line 2692 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6998 "parser.cpp"
    break;

  case 367: /* sparse_array_expr: double_sparse_array_expr  */
#line 2695 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7006 "parser.cpp"
    break;

  case 368: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2699 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7014 "parser.cpp"
    break;

  case 369: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2703 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7026 "parser.cpp"
    break;

  case 370: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2710 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7037 "parser.cpp"
    break;

  case 371: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2717 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7045 "parser.cpp"
    break;

  case 372: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2721 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7057 "parser.cpp"
    break;

  case 373: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2728 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7068 "parser.cpp"
    break;

  case 374: /* empty_array_expr: '[' ']'  */
#line 2735 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7076 "parser.cpp"
    break;

  case 375: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 2739 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7084 "parser.cpp"
    break;

  case 376: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 2743 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7092 "parser.cpp"
    break;

  case 377: /* array_expr: long_array_expr  */
#line 2747 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7100 "parser.cpp"
    break;

  case 378: /* array_expr: double_array_expr  */
#line 2750 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7108 "parser.cpp"
    break;

  case 379: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2754 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7116 "parser.cpp"
    break;

  case 380: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2758 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7126 "parser.cpp"
    break;

  case 381: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2763 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7135 "parser.cpp"
    break;

  case 382: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2768 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7143 "parser.cpp"
    break;

  case 383: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2772 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7153 "parser.cpp"
    break;

  case 384: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2777 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7162 "parser.cpp"
    break;

  case 385: /* interval_expr: LONG_VALUE SECONDS  */
#line 2782 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7173 "parser.cpp"
    break;

  case 386: /* interval_expr: LONG_VALUE SECOND  */
#line 2788 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7184 "parser.cpp"
    break;

  case 387: /* interval_expr: LONG_VALUE MINUTES  */
#line 2794 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7195 "parser.cpp"
    break;

  case 388: /* interval_expr: LONG_VALUE MINUTE  */
#line 2800 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7206 "parser.cpp"
    break;

  case 389: /* interval_expr: LONG_VALUE HOURS  */
#line 2806 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7217 "parser.cpp"
    break;

  case 390: /* interval_expr: LONG_VALUE HOUR  */
#line 2812 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7228 "parser.cpp"
    break;

  case 391: /* interval_expr: LONG_VALUE DAYS  */
#line 2818 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7239 "parser.cpp"
    break;

  case 392: /* interval_expr: LONG_VALUE DAY  */
#line 2824 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7250 "parser.cpp"
    break;

  case 393: /* interval_expr: LONG_VALUE MONTHS  */
#line 2830 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7261 "parser.cpp"
    break;

  case 394: /* interval_expr: LONG_VALUE MONTH  */
#line 2836 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7272 "parser.cpp"
    break;

  case 395: /* interval_expr: LONG_VALUE YEARS  */
#line 2842 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7283 "parser.cpp"
    break;

  case 396: /* interval_expr: LONG_VALUE YEAR  */
#line 2848 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7294 "parser.cpp"
    break;

  case 397: /* copy_option_list: copy_option  */
#line 2859 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7303 "parser.cpp"
    break;

  case 398: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2863 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7312 "parser.cpp"
    break;

  case 399: /* copy_option: FORMAT IDENTIFIER  */
#line 2868 "parser.y"
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
    } else if (strcasecmp((yyvsp[0].str_value), "bvecs") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kBVECS;
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 7345 "parser.cpp"
    break;

  case 400: /* copy_option: DELIMITER STRING  */
#line 2896 "parser.y"
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
#line 7360 "parser.cpp"
    break;

  case 401: /* copy_option: HEADER  */
#line 2906 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7370 "parser.cpp"
    break;

  case 402: /* file_path: STRING  */
#line 2912 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7378 "parser.cpp"
    break;

  case 403: /* if_exists: IF EXISTS  */
#line 2916 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7384 "parser.cpp"
    break;

  case 404: /* if_exists: %empty  */
#line 2917 "parser.y"
  { (yyval.bool_value) = false; }
#line 7390 "parser.cpp"
    break;

  case 405: /* if_not_exists: IF NOT EXISTS  */
#line 2919 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7396 "parser.cpp"
    break;

  case 406: /* if_not_exists: %empty  */
#line 2920 "parser.y"
  { (yyval.bool_value) = false; }
#line 7402 "parser.cpp"
    break;

  case 409: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2935 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7415 "parser.cpp"
    break;

  case 410: /* if_not_exists_info: %empty  */
#line 2943 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7423 "parser.cpp"
    break;

  case 411: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2947 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7431 "parser.cpp"
    break;

  case 412: /* with_index_param_list: %empty  */
#line 2950 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7439 "parser.cpp"
    break;

  case 413: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2954 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7447 "parser.cpp"
    break;

  case 414: /* optional_table_properties_list: %empty  */
#line 2957 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7455 "parser.cpp"
    break;

  case 415: /* index_param_list: index_param  */
#line 2961 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7464 "parser.cpp"
    break;

  case 416: /* index_param_list: index_param_list ',' index_param  */
#line 2965 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7473 "parser.cpp"
    break;

  case 417: /* index_param: IDENTIFIER  */
#line 2970 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7484 "parser.cpp"
    break;

  case 418: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2976 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7499 "parser.cpp"
    break;

  case 419: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2986 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7511 "parser.cpp"
    break;

  case 420: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2993 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7523 "parser.cpp"
    break;

  case 421: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3004 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
    } else if (strcmp((yyvsp[-1].str_value), "emvb") == 0) {
        index_type = infinity::IndexType::kEMVB;
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
#line 7578 "parser.cpp"
    break;

  case 422: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3054 "parser.y"
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
#line 7632 "parser.cpp"
    break;

  case 423: /* index_info_list: '(' identifier_array ')'  */
#line 3103 "parser.y"
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
#line 7650 "parser.cpp"
    break;


#line 7654 "parser.cpp"

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

#line 3117 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
  YYSYMBOL_USING = 158,                    /* USING  */
  YYSYMBOL_SESSION = 159,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 160,                   /* GLOBAL  */
  YYSYMBOL_OFF = 161,                      /* OFF  */
  YYSYMBOL_EXPORT = 162,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 163,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 164,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 165,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 166,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 167,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 168,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 169,                    /* DELTA  */
  YYSYMBOL_LOGS = 170,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 171,                 /* CATALOGS  */
  YYSYMBOL_SEARCH = 172,                   /* SEARCH  */
  YYSYMBOL_MATCH = 173,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 174,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 175,                    /* QUERY  */
  YYSYMBOL_QUERIES = 176,                  /* QUERIES  */
  YYSYMBOL_FUSION = 177,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 178,                 /* ROWLIMIT  */
  YYSYMBOL_NUMBER = 179,                   /* NUMBER  */
  YYSYMBOL_180_ = 180,                     /* '='  */
  YYSYMBOL_181_ = 181,                     /* '<'  */
  YYSYMBOL_182_ = 182,                     /* '>'  */
  YYSYMBOL_183_ = 183,                     /* '+'  */
  YYSYMBOL_184_ = 184,                     /* '-'  */
  YYSYMBOL_185_ = 185,                     /* '*'  */
  YYSYMBOL_186_ = 186,                     /* '/'  */
  YYSYMBOL_187_ = 187,                     /* '%'  */
  YYSYMBOL_188_ = 188,                     /* '['  */
  YYSYMBOL_189_ = 189,                     /* ']'  */
  YYSYMBOL_190_ = 190,                     /* '('  */
  YYSYMBOL_191_ = 191,                     /* ')'  */
  YYSYMBOL_192_ = 192,                     /* '.'  */
  YYSYMBOL_193_ = 193,                     /* ';'  */
  YYSYMBOL_194_ = 194,                     /* ','  */
  YYSYMBOL_195_ = 195,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 196,                 /* $accept  */
  YYSYMBOL_input_pattern = 197,            /* input_pattern  */
  YYSYMBOL_statement_list = 198,           /* statement_list  */
  YYSYMBOL_statement = 199,                /* statement  */
  YYSYMBOL_explainable_statement = 200,    /* explainable_statement  */
  YYSYMBOL_create_statement = 201,         /* create_statement  */
  YYSYMBOL_table_element_array = 202,      /* table_element_array  */
  YYSYMBOL_table_element = 203,            /* table_element  */
  YYSYMBOL_table_column = 204,             /* table_column  */
  YYSYMBOL_column_type = 205,              /* column_type  */
  YYSYMBOL_column_constraints = 206,       /* column_constraints  */
  YYSYMBOL_column_constraint = 207,        /* column_constraint  */
  YYSYMBOL_default_expr = 208,             /* default_expr  */
  YYSYMBOL_table_constraint = 209,         /* table_constraint  */
  YYSYMBOL_identifier_array = 210,         /* identifier_array  */
  YYSYMBOL_delete_statement = 211,         /* delete_statement  */
  YYSYMBOL_insert_statement = 212,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 213, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 214,        /* explain_statement  */
  YYSYMBOL_explain_type = 215,             /* explain_type  */
  YYSYMBOL_update_statement = 216,         /* update_statement  */
  YYSYMBOL_update_expr_array = 217,        /* update_expr_array  */
  YYSYMBOL_update_expr = 218,              /* update_expr  */
  YYSYMBOL_drop_statement = 219,           /* drop_statement  */
  YYSYMBOL_copy_statement = 220,           /* copy_statement  */
  YYSYMBOL_select_statement = 221,         /* select_statement  */
  YYSYMBOL_select_with_paren = 222,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 223,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 224, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 225, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 226, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 227,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 228,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 229,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 230,            /* order_by_type  */
  YYSYMBOL_limit_expr = 231,               /* limit_expr  */
  YYSYMBOL_offset_expr = 232,              /* offset_expr  */
  YYSYMBOL_distinct = 233,                 /* distinct  */
  YYSYMBOL_from_clause = 234,              /* from_clause  */
  YYSYMBOL_search_clause = 235,            /* search_clause  */
  YYSYMBOL_where_clause = 236,             /* where_clause  */
  YYSYMBOL_having_clause = 237,            /* having_clause  */
  YYSYMBOL_group_by_clause = 238,          /* group_by_clause  */
  YYSYMBOL_set_operator = 239,             /* set_operator  */
  YYSYMBOL_table_reference = 240,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 241,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 242,     /* table_reference_name  */
  YYSYMBOL_table_name = 243,               /* table_name  */
  YYSYMBOL_table_alias = 244,              /* table_alias  */
  YYSYMBOL_with_clause = 245,              /* with_clause  */
  YYSYMBOL_with_expr_list = 246,           /* with_expr_list  */
  YYSYMBOL_with_expr = 247,                /* with_expr  */
  YYSYMBOL_join_clause = 248,              /* join_clause  */
  YYSYMBOL_join_type = 249,                /* join_type  */
  YYSYMBOL_show_statement = 250,           /* show_statement  */
  YYSYMBOL_flush_statement = 251,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 252,       /* optimize_statement  */
  YYSYMBOL_command_statement = 253,        /* command_statement  */
  YYSYMBOL_compact_statement = 254,        /* compact_statement  */
  YYSYMBOL_expr_array = 255,               /* expr_array  */
  YYSYMBOL_expr_array_list = 256,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 257,               /* expr_alias  */
  YYSYMBOL_expr = 258,                     /* expr  */
  YYSYMBOL_operand = 259,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 260, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 261,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 262,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 263,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 264,          /* match_text_expr  */
  YYSYMBOL_query_expr = 265,               /* query_expr  */
  YYSYMBOL_fusion_expr = 266,              /* fusion_expr  */
  YYSYMBOL_sub_search = 267,               /* sub_search  */
  YYSYMBOL_sub_search_array = 268,         /* sub_search_array  */
  YYSYMBOL_function_expr = 269,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 270,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 271,             /* between_expr  */
  YYSYMBOL_in_expr = 272,                  /* in_expr  */
  YYSYMBOL_case_expr = 273,                /* case_expr  */
  YYSYMBOL_case_check_array = 274,         /* case_check_array  */
  YYSYMBOL_cast_expr = 275,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 276,            /* subquery_expr  */
  YYSYMBOL_column_expr = 277,              /* column_expr  */
  YYSYMBOL_constant_expr = 278,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 279,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 280, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 281,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 282, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 283,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 284,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 285, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 286, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 287, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 288,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 289,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 290,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 291,               /* array_expr  */
  YYSYMBOL_long_array_expr = 292,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 293, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 294,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 295, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 296,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 297,         /* copy_option_list  */
  YYSYMBOL_copy_option = 298,              /* copy_option  */
  YYSYMBOL_file_path = 299,                /* file_path  */
  YYSYMBOL_if_exists = 300,                /* if_exists  */
  YYSYMBOL_if_not_exists = 301,            /* if_not_exists  */
  YYSYMBOL_semicolon = 302,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 303,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 304,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 305, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 306,         /* index_param_list  */
  YYSYMBOL_index_param = 307,              /* index_param  */
  YYSYMBOL_index_info_list = 308,          /* index_info_list  */
  YYSYMBOL_index_info_list_one_pack = 309  /* index_info_list_one_pack  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 438 "parser.cpp"

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
#define YYFINAL  93
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1105

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  196
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  433
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  905

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   434


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
       2,     2,     2,     2,     2,     2,     2,   187,     2,     2,
     190,   191,   185,   183,   194,   184,   192,   186,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   195,   193,
     181,   180,   182,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   188,     2,   189,     2,     2,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179
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
    1122,  1145,  1198,  1253,  1304,  1307,  1310,  1319,  1329,  1332,
    1336,  1341,  1363,  1366,  1371,  1387,  1390,  1394,  1398,  1403,
    1409,  1412,  1415,  1419,  1423,  1425,  1429,  1431,  1434,  1438,
    1441,  1445,  1450,  1454,  1457,  1461,  1464,  1468,  1471,  1475,
    1478,  1481,  1484,  1492,  1495,  1510,  1510,  1512,  1526,  1535,
    1540,  1549,  1554,  1559,  1565,  1572,  1575,  1579,  1582,  1587,
    1599,  1606,  1620,  1623,  1626,  1629,  1632,  1635,  1638,  1644,
    1648,  1652,  1656,  1660,  1667,  1671,  1675,  1679,  1684,  1688,
    1693,  1697,  1701,  1707,  1713,  1719,  1730,  1741,  1752,  1764,
    1776,  1789,  1803,  1814,  1828,  1844,  1861,  1865,  1869,  1877,
    1881,  1885,  1893,  1904,  1927,  1933,  1938,  1944,  1950,  1958,
    1964,  1970,  1976,  1982,  1990,  1996,  2002,  2008,  2014,  2022,
    2028,  2035,  2052,  2056,  2061,  2065,  2092,  2098,  2102,  2103,
    2104,  2105,  2106,  2108,  2111,  2117,  2120,  2121,  2122,  2123,
    2124,  2125,  2126,  2127,  2128,  2129,  2131,  2134,  2140,  2159,
    2201,  2247,  2265,  2283,  2291,  2302,  2308,  2317,  2323,  2335,
    2338,  2341,  2344,  2347,  2350,  2354,  2358,  2363,  2371,  2379,
    2388,  2395,  2402,  2409,  2416,  2423,  2431,  2439,  2447,  2455,
    2463,  2471,  2479,  2487,  2495,  2503,  2511,  2519,  2549,  2557,
    2566,  2574,  2583,  2591,  2597,  2604,  2610,  2617,  2622,  2629,
    2636,  2644,  2668,  2674,  2680,  2687,  2695,  2702,  2709,  2714,
    2724,  2729,  2734,  2739,  2744,  2749,  2754,  2759,  2764,  2769,
    2772,  2775,  2779,  2782,  2785,  2788,  2792,  2795,  2798,  2802,
    2806,  2811,  2816,  2819,  2823,  2827,  2834,  2841,  2845,  2852,
    2859,  2863,  2867,  2871,  2874,  2878,  2882,  2887,  2892,  2896,
    2901,  2906,  2912,  2918,  2924,  2930,  2936,  2942,  2948,  2954,
    2960,  2966,  2972,  2983,  2987,  2992,  3022,  3032,  3037,  3042,
    3047,  3053,  3057,  3058,  3060,  3061,  3063,  3064,  3076,  3084,
    3088,  3091,  3095,  3098,  3102,  3106,  3111,  3117,  3127,  3134,
    3145,  3149,  3157,  3209
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
  "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION", "USING",
  "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG",
  "PROFILES", "VARIABLES", "VARIABLE", "DELTA", "LOGS", "CATALOGS",
  "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "':'", "$accept",
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
  "compact_statement", "expr_array", "expr_array_list", "expr_alias",
  "expr", "operand", "extra_match_tensor_option", "match_tensor_expr",
  "match_vector_expr", "match_sparse_expr", "match_text_expr",
  "query_expr", "fusion_expr", "sub_search", "sub_search_array",
  "function_expr", "conjunction_expr", "between_expr", "in_expr",
  "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "common_array_expr",
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
  "index_info_list", "index_info_list_one_pack", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-505)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-420)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     311,    90,    33,   223,    51,    12,    51,    68,   930,   568,
      81,   128,   132,   134,   149,   -65,   -44,   157,    53,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,   279,  -505,  -505,
     205,  -505,  -505,  -505,  -505,  -505,   185,   185,   185,   185,
      21,    51,   193,   193,   193,   193,   193,    77,   280,    51,
      -7,   295,   299,   326,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,   493,   332,    51,  -505,  -505,  -505,  -505,  -505,   268,
     142,   214,  -505,   337,  -505,   172,  -505,  -505,   189,  -505,
      51,  -505,  -505,  -505,  -505,   -48,  -505,   304,   169,  -505,
     362,   183,   203,  -505,    30,  -505,   375,  -505,  -505,     5,
     350,  -505,   353,   361,   469,    51,    51,    51,   471,   412,
     292,   408,   507,    51,    51,    51,   509,   511,   518,   454,
     523,   523,   456,    64,    70,    78,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,   279,  -505,  -505,  -505,  -505,  -505,  -505,
     333,  -505,  -505,   531,  -505,   536,  -505,  -505,   535,  -505,
      51,   358,   149,   523,  -505,  -505,  -505,  -505,     5,  -505,
    -505,  -505,   456,   497,   484,   477,  -505,   -43,  -505,   292,
    -505,    51,   552,    28,  -505,  -505,  -505,  -505,  -505,   492,
    -505,   377,   -39,  -505,   456,  -505,  -505,   482,   485,   373,
    -505,  -505,   747,   491,   374,   378,   182,   565,   567,   570,
     572,  -505,  -505,   573,   388,   211,   390,   391,   585,   585,
    -505,    20,   340,    23,  -505,   -37,   665,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
     392,  -505,  -505,  -505,  -107,  -505,  -505,   -87,  -505,     0,
    -505,  -505,  -505,    54,  -505,    67,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,   579,   577,  -505,  -505,  -505,  -505,  -505,  -505,
     512,   205,  -505,  -505,   410,   411,   -45,   456,   456,   528,
    -505,   -44,    93,   546,   414,  -505,    32,   416,  -505,    51,
     456,   518,  -505,   264,   418,   419,   302,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,   585,
     420,   675,   534,   456,   456,    95,   262,  -505,  -505,  -505,
    -505,   747,  -505,   607,   425,   426,   427,   428,   615,   619,
     271,   271,  -505,   429,  -505,  -505,  -505,  -505,   434,    41,
     547,   456,   624,   456,   456,   -19,   438,    22,   585,   585,
     585,   585,   585,   585,   585,   585,   585,   585,   585,   585,
     585,   585,    15,  -505,   442,  -505,   626,  -505,   628,  -505,
     629,  -505,   631,   592,   381,   453,  -505,   457,  -505,  -505,
       8,   473,   455,  -505,    31,   264,   456,  -505,   279,   824,
     526,   461,    46,  -505,  -505,  -505,   -44,   552,   414,  -505,
    -505,   644,   456,   458,  -505,   264,  -505,    18,    18,   456,
    -505,    56,   534,   501,   463,    -9,   -11,   269,  -505,   456,
     456,   580,   456,   655,    16,   456,    69,   123,   435,  -505,
    -505,   523,  -505,  -505,  -505,   513,   475,   585,   340,   537,
    -505,   726,   726,   510,   510,   651,   726,   726,   510,   510,
     271,   271,  -505,  -505,  -505,  -505,  -505,  -505,   466,  -505,
     472,  -505,  -505,  -505,   664,   666,  -505,   670,  -505,   -44,
     486,   833,  -505,    48,  -505,   187,   454,   456,  -505,  -505,
    -505,   264,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,   488,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,   494,   495,   496,   508,   519,   154,
     522,   552,   654,    93,   279,   196,  -505,  -505,   204,   527,
     693,   694,   697,   699,  -505,   702,   227,  -505,   243,   244,
    -505,   529,  -505,   824,   456,  -505,   456,   -35,     4,   585,
     -85,   524,  -505,   -49,    61,  -505,   709,  -505,   715,  -505,
    -505,   641,   340,   726,   540,   255,  -505,   585,   716,   730,
     688,   695,   571,   256,  -505,    39,     8,   696,  -505,  -505,
    -505,  -505,  -505,  -505,   698,  -505,   749,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,   559,   705,  -505,   750,   197,
     452,   612,   856,   865,   635,   639,  -505,  -505,   147,  -505,
     636,   552,   260,   575,  -505,  -505,   608,  -505,   456,  -505,
    -505,  -505,  -505,  -505,  -505,    18,  -505,  -505,  -505,   576,
     264,     1,  -505,   456,   602,   583,   768,   442,   586,   588,
     589,   587,   590,   261,  -505,  -505,   675,   770,   784,   422,
    -505,   670,    48,   833,     8,     8,   601,   187,   739,   743,
     272,   610,   611,   613,   614,   620,   621,   622,   630,   632,
     634,   645,   646,   647,   648,   649,   650,   660,   669,   671,
     672,   673,   674,   680,   681,   682,   683,   703,   706,   707,
     708,   710,   711,   741,   742,   744,   745,   748,   758,   759,
     767,  -505,  -505,    74,  -505,  -505,  -505,   273,  -505,   670,
     798,   274,  -505,  -505,  -505,   264,  -505,   483,   777,   287,
     778,    26,   781,  -505,  -505,  -505,  -505,  -505,    18,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,   754,
     552,  -505,   456,   456,  -505,  -505,   800,   817,   819,   823,
     858,   863,   897,   937,   956,   970,   971,   972,   973,   974,
     975,   976,   978,   979,   980,   981,   982,   983,   984,   985,
     986,   987,   988,   989,   990,   991,   992,   993,   994,   995,
     996,   997,   998,   999,  1000,  1001,  1002,  -505,  -505,   289,
     512,  -505,  -505,   826,  -505,  1005,  1006,  1007,   317,   456,
     335,   818,   264,   822,   825,   827,   828,   829,   830,   831,
     832,   834,   835,   836,   837,   838,   839,   840,   841,   842,
     843,   844,   845,   846,   847,   848,   849,   850,   851,   852,
     853,   854,   855,   857,   859,   860,   861,   862,   864,   866,
     867,   868,   869,   870,  -505,  -505,   820,   871,   821,   342,
    -505,   264,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  1013,  -505,  1020,   512,  1041,   344,
     872,  -505,   873,   512,  1043,  1050,   874,   512,  -505,   876,
    -505,  -505,  -505,   512,  -505
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     196,     0,     0,     0,     0,     0,     0,     0,   131,     0,
       0,     0,     0,     0,     0,     0,   196,     0,   417,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   145,   144,
       0,     8,    14,    15,    16,    17,   415,   415,   415,   415,
     415,     0,   413,   413,   413,   413,   413,   189,     0,     0,
       0,     0,     0,     0,   125,   129,   126,   127,   128,   130,
     124,   196,     0,     0,   210,   211,   209,   215,   218,     0,
       0,     0,   212,     0,   214,     0,   236,   238,     0,   216,
       0,   239,   240,   241,   244,   189,   242,     0,   195,   197,
       0,     0,     0,     1,   196,     2,   179,   181,   182,     0,
     168,   150,   156,     0,     0,     0,     0,     0,     0,     0,
     122,     0,     0,     0,     0,     0,     0,     0,     0,   174,
       0,     0,     0,     0,     0,     0,   123,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   224,
     225,   219,   220,     0,   221,     0,   213,   237,     0,   261,
       0,     0,     0,     0,   149,   148,     4,   180,     0,   146,
     147,   167,     0,     0,   164,     0,    30,     0,    31,   122,
     418,     0,     0,   196,   412,   136,   138,   137,   139,     0,
     190,     0,   174,   133,     0,   118,   411,     0,     0,   346,
     350,   353,   354,     0,     0,     0,     0,     0,     0,     0,
       0,   351,   352,     0,     0,     0,     0,     0,     0,     0,
     348,     0,   196,     0,   262,   267,   268,   282,   280,   283,
     281,   284,   285,   277,   272,   271,   270,   278,   279,   269,
     276,   275,   361,   363,     0,   364,   372,     0,   373,     0,
     365,   362,   383,     0,   384,     0,   360,   248,   250,   249,
     246,   247,   253,   255,   254,   251,   252,   258,   260,   259,
     256,   257,     0,     0,   227,   226,   232,   222,   223,   217,
     421,     0,   198,   245,     0,     0,   170,     0,     0,   166,
     414,   196,     0,     0,     0,   116,     0,     0,   120,     0,
       0,     0,   132,   173,     0,     0,     0,   392,   391,   394,
     393,   396,   395,   398,   397,   400,   399,   402,   401,     0,
       0,   312,   196,     0,     0,     0,     0,   355,   356,   357,
     358,     0,   359,     0,     0,     0,     0,     0,     0,     0,
     314,   313,   389,   386,   380,   370,   375,   378,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   369,     0,   374,     0,   377,     0,   385,
       0,   388,     0,   233,   228,     0,   243,     0,   153,   152,
       0,   172,   155,   157,   162,   163,     0,   151,    33,     0,
       0,     0,     0,    36,    38,    39,   196,     0,    35,   430,
     121,     0,     0,   119,   140,   135,   134,     0,     0,     0,
     307,     0,   196,     0,     0,     0,     0,     0,   337,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   274,
     273,     0,   263,   266,   330,   331,     0,     0,   196,     0,
     311,   321,   322,   325,   326,     0,   328,   320,   323,   324,
     316,   315,   317,   318,   319,   347,   349,   371,     0,   376,
       0,   379,   387,   390,     0,     0,   229,     0,   199,   196,
     169,   183,   185,   194,   186,     0,   174,     0,   160,   161,
     159,   165,    42,    45,    46,    43,    44,    47,    48,    62,
      49,    51,    50,    65,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,     0,     0,     0,     0,     0,   421,
       0,     0,   423,     0,    34,     0,   431,   117,     0,     0,
       0,     0,     0,     0,   407,     0,     0,   403,     0,     0,
     308,     0,   342,     0,     0,   335,     0,     0,     0,     0,
       0,     0,   346,     0,     0,   295,     0,   297,     0,   382,
     381,     0,   196,   329,     0,     0,   310,     0,     0,     0,
     234,   230,   426,     0,   424,     0,     0,     0,   203,   204,
     205,   206,   202,   207,     0,   192,     0,   187,   301,   299,
     302,   300,   303,   304,   305,   171,   178,   158,     0,     0,
       0,     0,     0,     0,     0,     0,   109,   110,   113,   106,
     113,     0,     0,     0,    32,    37,   433,   264,     0,   409,
     408,   406,   405,   410,   143,     0,   141,   309,   343,     0,
     339,     0,   338,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   344,   333,   332,     0,     0,     0,
     420,     0,   194,   184,     0,     0,   191,     0,     0,   176,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   111,   108,     0,   107,    41,    40,     0,   115,     0,
       0,     0,   404,   341,   336,   340,   327,     0,     0,     0,
       0,     0,     0,   366,   368,   367,   296,   298,     0,   345,
     334,   235,   231,   427,   429,   428,   425,   188,   200,     0,
       0,   306,     0,     0,   154,    64,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   112,   114,     0,
     421,   265,   386,     0,   293,     0,     0,     0,     0,     0,
       0,   177,   175,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   422,   432,     0,     0,     0,     0,
     142,   201,   193,    63,    69,    70,    67,    68,    71,    72,
      73,    66,    93,    94,    91,    92,    95,    96,    97,    90,
      77,    78,    75,    76,    79,    80,    81,    74,   101,   102,
      99,   100,   103,   104,   105,    98,    85,    86,    83,    84,
      87,    88,    89,    82,     0,   294,     0,   421,     0,     0,
     287,   292,     0,   421,     0,     0,     0,   421,   290,     0,
     286,   288,   291,   421,   289
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -505,  -505,  -505,   962,  -505,  1008,  -505,   550,  -505,   538,
    -505,   470,   474,  -505,  -389,  1009,  1011,   904,  -505,  -505,
    1014,  -505,   785,  1016,  1017,   -57,  1063,   -15,   809,   923,
     -31,  -505,  -505,   605,  -505,  -505,  -505,  -505,  -505,  -505,
    -172,  -505,  -505,  -505,  -505,   517,  -146,    11,   443,  -505,
    -505,   932,  -505,  -505,  1025,  1026,  1027,  1028,  1029,  -160,
    -505,   751,  -184,  -186,  -505,  -454,  -448,  -441,  -440,  -428,
    -426,   444,  -505,  -505,  -505,  -505,  -505,  -505,   779,  -505,
    -505,   676,   400,  -208,  -505,  -505,  -505,   467,  -505,  -505,
    -505,  -505,   468,   731,   733,  -252,  -505,  -505,  -505,  -505,
     895,  -399,   487,  -115,   357,   371,  -505,  -505,  -504,  -505,
     404,   464,  -505,   701
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   126,    20,   392,   393,   394,   509,
     598,   599,   695,   395,   286,    21,    22,   173,    23,    61,
      24,   182,   183,    25,    26,    27,    28,    29,   101,   159,
     102,   164,   382,   383,   480,   279,   387,   162,   381,   476,
     185,   734,   649,    99,   470,   471,   472,   473,   577,    30,
      88,    89,   474,   574,    31,    32,    33,    34,    35,   213,
     403,   214,   215,   216,   896,   217,   218,   219,   220,   221,
     222,   584,   585,   223,   224,   225,   226,   227,   316,   228,
     229,   230,   231,   232,   712,   233,   234,   235,   236,   237,
     238,   239,   240,   336,   337,   241,   242,   243,   244,   245,
     246,   526,   527,   187,   112,   104,    95,   109,   376,   604,
     563,   564,   398,   399
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     293,    92,   276,   335,   133,   600,   188,   311,   515,   528,
     292,    47,   315,   100,   281,    48,   150,    50,   455,   542,
     342,   578,   330,   331,    86,   332,   333,   579,   339,   184,
    -416,   332,   333,   380,   580,   581,    14,     1,   273,     2,
       3,     4,     5,     6,     7,     8,     9,   582,   533,   583,
     622,   575,   110,    10,    47,    11,    12,    13,   436,    96,
     119,    97,   287,    98,   343,   344,    41,   247,   160,   248,
     249,   120,   121,   252,   140,   253,   254,   534,   190,   191,
     192,   257,   363,   258,   259,  -419,   704,   364,   520,   521,
      49,   149,   623,   384,   385,   103,   389,   439,    90,   522,
     523,   524,   365,   478,   479,   576,   405,   366,    14,   625,
      14,    80,   343,   344,   343,   344,   167,   168,   169,    36,
      37,    38,   602,   311,   176,   177,   178,   275,   250,   415,
     416,    39,    40,   437,   255,    84,   411,    85,   343,   344,
     343,   344,   260,   362,   117,   627,    16,   282,   440,   341,
     343,   344,    87,   343,   344,   291,   457,    93,   288,   434,
     435,   270,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,   197,   198,   199,   200,
     343,   344,   284,   122,   314,   189,   190,   191,   192,   367,
     343,   344,    15,   578,   368,   158,   525,   338,   469,   579,
     456,   210,   481,   201,   202,   203,   580,   581,   211,   334,
     343,   344,   697,   100,   340,   334,   390,   341,   391,   582,
      16,   583,   594,   400,   388,   251,   401,    51,    52,   594,
     642,   256,   430,    53,   375,   537,   538,   512,   540,   261,
     513,   544,   518,   369,   343,   344,    94,   530,   370,   529,
     341,   553,    42,    43,    44,   628,   371,   193,   194,   103,
     545,   372,   211,   546,    45,    46,   195,   111,   196,   117,
     595,   314,   596,   597,   141,   693,   835,   595,   555,   596,
     597,    81,    82,    83,   197,   198,   199,   200,   651,   652,
     653,   654,   655,   384,   118,   656,   657,   414,   123,    96,
     404,    97,   124,    98,   586,   189,   190,   191,   192,   142,
     143,   201,   202,   203,   547,   658,   551,   548,     1,   788,
       2,     3,     4,     5,     6,     7,     8,     9,   324,   125,
     325,   326,   327,   204,    10,   139,    11,    12,    13,   514,
     146,   790,   147,   189,   190,   191,   192,   418,   148,   419,
     620,   420,   621,   624,   535,   205,   536,   206,   420,   207,
     205,   151,   206,   152,   207,   208,   209,   210,   153,   409,
     211,   636,   212,   708,   154,   262,   715,   193,   194,   263,
     264,   144,   145,   891,   265,   266,   195,   606,   196,   898,
     401,    14,   633,   902,   155,   607,   157,   531,   341,   904,
     113,   114,   115,   116,   197,   198,   199,   200,   105,   106,
     107,   108,   565,   343,   344,   193,   194,   161,   614,   710,
      14,   615,   163,   554,   195,   723,   196,   724,   725,   465,
     466,   201,   202,   203,   616,   617,   165,   615,   341,   705,
     549,   550,   197,   198,   199,   200,   635,   640,   701,   341,
     641,   698,   720,   204,   401,   341,   359,   360,   361,   189,
     190,   191,   192,   735,   778,   781,   736,   401,   341,   201,
     202,   203,   166,    15,   170,   205,   171,   206,   784,   207,
     834,   785,   172,   641,   174,   208,   209,   210,   332,   782,
     211,   204,   212,   410,   189,   190,   191,   192,   728,   729,
       1,    16,     2,     3,     4,     5,     6,     7,   840,     9,
     175,   615,   179,   205,   180,   206,    10,   207,    11,    12,
      13,   181,   184,   208,   209,   210,   842,   186,   211,   401,
     212,   193,   194,   887,   267,   893,   888,   632,   894,   268,
     195,   269,   196,   659,   660,   661,   662,   663,   271,   792,
     664,   665,   277,   280,   278,   285,   289,   290,   197,   198,
     199,   200,   294,   296,   312,   295,   309,   310,   313,   317,
     666,   318,   791,    14,   319,   195,   320,   196,   323,   321,
     328,   329,   373,   374,   362,   201,   202,   203,   189,   190,
     191,   192,   375,   197,   198,   199,   200,    62,    63,   386,
      64,   378,   379,   396,   397,   841,   402,   204,   407,   408,
     412,   421,    65,    66,    14,   422,   423,   424,   425,   426,
     201,   202,   203,   427,   428,   429,   431,   433,   438,   205,
     211,   206,   458,   207,   460,   462,   463,   347,   464,   208,
     209,   210,   204,   467,   211,   475,   212,   517,   468,   477,
     510,   511,   519,   437,   532,    15,  -420,  -420,   539,   541,
     309,   558,   343,   556,   205,   552,   206,   559,   207,   195,
     560,   196,   561,   562,   208,   209,   210,   413,   588,   211,
     566,   212,   603,    16,   589,   590,   591,   197,   198,   199,
     200,  -420,  -420,   357,   358,   359,   360,   361,   592,   609,
     610,   611,   612,   667,   668,   669,   670,   671,   613,   593,
     672,   673,   601,   629,   201,   202,   203,   608,   626,   630,
     618,   631,   550,    67,    68,    69,   413,    70,    71,   347,
     674,   634,    72,    73,    74,   549,   204,    75,    76,    77,
     345,   637,   346,    78,    79,   638,   348,   349,   350,   351,
     413,   639,   646,   647,   353,   644,   650,   645,   205,   648,
     206,   691,   207,   692,   693,   699,   700,   703,   208,   209,
     210,   707,   709,   211,   711,   212,   721,   718,   347,   716,
     717,   719,   354,   355,   356,   357,   358,   359,   360,   361,
     722,   730,   347,   706,   732,   348,   349,   350,   351,   733,
     557,   780,   347,   353,   737,   738,   793,   739,   740,   348,
     349,   350,   351,   352,   741,   742,   743,   353,   789,   348,
     349,   350,   351,   794,   744,   795,   745,   353,   746,   796,
     836,   354,   355,   356,   357,   358,   359,   360,   361,   747,
     748,   749,   750,   751,   752,   354,   355,   356,   357,   358,
     359,   360,   361,   347,   753,   354,   355,   356,   357,   358,
     359,   360,   361,   754,   797,   755,   756,   757,   758,   798,
    -420,  -420,   350,   351,   759,   760,   761,   762,  -420,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   567,  -208,   568,   569,   570,   571,   763,   572,   573,
     764,   765,   766,   799,   767,   768,  -420,   355,   356,   357,
     358,   359,   360,   361,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   497,
     498,   499,   500,   501,   502,   769,   770,   503,   771,   772,
     504,   505,   773,   800,   506,   507,   508,   675,   676,   677,
     678,   679,   774,   775,   680,   681,   683,   684,   685,   686,
     687,   776,   801,   688,   689,    54,    55,    56,    57,    58,
      59,   783,   786,    60,   682,   787,   802,   803,   804,   805,
     806,   807,   808,   690,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   823,   824,
     825,   826,   827,   828,   829,   830,   831,   832,   833,   837,
     838,   839,   341,   843,   884,   886,   844,   889,   845,   846,
     847,   848,   849,   850,   890,   851,   852,   853,   854,   855,
     856,   857,   858,   859,   860,   861,   862,   863,   864,   865,
     866,   867,   868,   869,   870,   871,   872,   892,   873,   899,
     874,   875,   876,   877,   900,   878,   156,   879,   880,   881,
     882,   883,   885,   605,   897,   901,   895,   903,   694,   127,
     128,   619,   129,   283,   696,   130,   406,   131,   132,    91,
     377,   274,   587,   643,   272,   727,   134,   135,   136,   137,
     138,   731,   432,   777,   417,   713,   714,   459,   322,   516,
     543,   461,   702,   779,     0,   726
};

static const yytype_int16 yycheck[] =
{
     184,    16,   162,   211,    61,   509,   121,   193,   397,   408,
     182,     3,   196,     8,    57,     4,    64,     6,     3,     3,
      57,   475,   208,   209,    13,     5,     6,   475,   212,    68,
       0,     5,     6,    78,   475,   475,    80,     7,   153,     9,
      10,    11,    12,    13,    14,    15,    16,   475,    57,   475,
      85,     3,    41,    23,     3,    25,    26,    27,    77,    20,
      49,    22,    34,    24,   149,   150,    33,     3,    99,     5,
       6,    78,    79,     3,    63,     5,     6,    88,     4,     5,
       6,     3,   189,     5,     6,    64,    85,   194,    70,    71,
      78,    80,    88,   277,   278,    74,     3,    75,   163,    81,
      82,    83,   189,    72,    73,    57,   290,   194,    80,   194,
      80,    30,   149,   150,   149,   150,   105,   106,   107,    29,
      30,    31,   511,   309,   113,   114,   115,   158,    64,   313,
     314,    41,    42,   152,    64,     3,   296,     3,   149,   150,
     149,   150,    64,   192,   192,   194,   190,   190,   126,   194,
     149,   150,     3,   149,   150,   194,   364,     0,   173,   343,
     344,   150,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,   102,   103,   104,   105,
     149,   150,   171,   190,    89,     3,     4,     5,     6,   189,
     149,   150,   162,   647,   194,   190,   178,   212,   190,   647,
     185,   185,   386,   129,   130,   131,   647,   647,   188,   189,
     149,   150,   601,     8,   191,   189,   123,   194,   125,   647,
     190,   647,    75,   191,   281,   161,   194,   159,   160,    75,
     191,   161,   191,   165,    80,   419,   420,   191,   422,   161,
     194,   425,   402,   189,   149,   150,   193,   191,   194,   409,
     194,   437,    29,    30,    31,   194,   189,    75,    76,    74,
     191,   194,   188,   194,    41,    42,    84,    74,    86,   192,
     123,    89,   125,   126,     6,   128,   780,   123,   438,   125,
     126,   153,   154,   155,   102,   103,   104,   105,    91,    92,
      93,    94,    95,   477,    14,    98,    99,   312,     3,    20,
     289,    22,     3,    24,   476,     3,     4,     5,     6,   167,
     168,   129,   130,   131,   191,   118,   431,   194,     7,   718,
       9,    10,    11,    12,    13,    14,    15,    16,   117,     3,
     119,   120,   121,   151,    23,     3,    25,    26,    27,   396,
       3,   730,   170,     3,     4,     5,     6,    85,   159,    87,
     534,    89,   536,   539,    85,   173,    87,   175,    89,   177,
     173,    57,   175,   194,   177,   183,   184,   185,     6,    67,
     188,   557,   190,   625,   191,    42,   628,    75,    76,    46,
      47,   167,   168,   887,    51,    52,    84,   191,    86,   893,
     194,    80,   552,   897,   191,   191,    21,   412,   194,   903,
      43,    44,    45,    46,   102,   103,   104,   105,    37,    38,
      39,    40,   469,   149,   150,    75,    76,    67,   191,   627,
      80,   194,    69,   438,    84,     3,    86,     5,     6,    48,
      49,   129,   130,   131,   191,   191,    75,   194,   194,   623,
       5,     6,   102,   103,   104,   105,   191,   191,   608,   194,
     194,   191,   191,   151,   194,   194,   185,   186,   187,     3,
       4,     5,     6,   191,   191,   191,   194,   194,   194,   129,
     130,   131,     3,   162,     3,   173,    64,   175,   191,   177,
     191,   194,   190,   194,    76,   183,   184,   185,     5,     6,
     188,   151,   190,   191,     3,     4,     5,     6,   644,   645,
       7,   190,     9,    10,    11,    12,    13,    14,   191,    16,
       3,   194,     3,   173,     3,   175,    23,   177,    25,    26,
      27,     3,    68,   183,   184,   185,   191,     4,   188,   194,
     190,    75,    76,   191,     3,   191,   194,   552,   194,     3,
      84,     6,    86,    91,    92,    93,    94,    95,   190,   733,
      98,    99,    55,    76,    70,     3,    64,   180,   102,   103,
     104,   105,    80,   190,   190,    80,    75,    76,   190,     4,
     118,     4,   732,    80,     4,    84,     4,    86,   190,     6,
     190,   190,     3,     6,   192,   129,   130,   131,     3,     4,
       5,     6,    80,   102,   103,   104,   105,    29,    30,    71,
      32,   191,   191,    57,   190,   789,   190,   151,   190,   190,
     190,     4,    44,    45,    80,   190,   190,   190,   190,     4,
     129,   130,   131,     4,   195,   191,    79,     3,   190,   173,
     188,   175,     6,   177,     6,     6,     5,   127,    46,   183,
     184,   185,   151,   190,   188,   172,   190,     3,   191,   194,
     124,   190,   194,   152,   191,   162,   146,   147,    78,     4,
      75,   195,   149,   126,   173,   190,   175,   195,   177,    84,
       6,    86,     6,     3,   183,   184,   185,    75,   190,   188,
     194,   190,    28,   190,   190,   190,   190,   102,   103,   104,
     105,   181,   182,   183,   184,   185,   186,   187,   190,     6,
       6,     4,     3,    91,    92,    93,    94,    95,     6,   190,
      98,    99,   190,     4,   129,   130,   131,   190,   194,     4,
     191,    80,     6,   155,   156,   157,    75,   159,   160,   127,
     118,   191,   164,   165,   166,     5,   151,   169,   170,   171,
      75,    53,    77,   175,   176,    50,   144,   145,   146,   147,
      75,   180,     3,   194,   152,    59,     6,    59,   173,    54,
     175,   126,   177,   124,   128,   190,   158,   191,   183,   184,
     185,   188,     4,   188,   188,   190,     6,   190,   127,   191,
     191,   191,   180,   181,   182,   183,   184,   185,   186,   187,
       6,   190,   127,   191,    55,   144,   145,   146,   147,    56,
     149,     3,   127,   152,   194,   194,     6,   194,   194,   144,
     145,   146,   147,   148,   194,   194,   194,   152,    64,   144,
     145,   146,   147,     6,   194,     6,   194,   152,   194,     6,
       4,   180,   181,   182,   183,   184,   185,   186,   187,   194,
     194,   194,   194,   194,   194,   180,   181,   182,   183,   184,
     185,   186,   187,   127,   194,   180,   181,   182,   183,   184,
     185,   186,   187,   194,     6,   194,   194,   194,   194,     6,
     144,   145,   146,   147,   194,   194,   194,   194,   152,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,    58,    59,    60,    61,    62,    63,   194,    65,    66,
     194,   194,   194,     6,   194,   194,   180,   181,   182,   183,
     184,   185,   186,   187,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   194,   194,   113,   194,   194,
     116,   117,   194,     6,   120,   121,   122,    91,    92,    93,
      94,    95,   194,   194,    98,    99,    91,    92,    93,    94,
      95,   194,     6,    98,    99,    35,    36,    37,    38,    39,
      40,   194,   194,    43,   118,   194,     6,     6,     6,     6,
       6,     6,     6,   118,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     4,
       4,     4,   194,   191,   194,   194,   191,     4,   191,   191,
     191,   191,   191,   191,     4,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,     6,   191,     6,
     191,   191,   191,   191,     4,   191,    94,   191,   191,   191,
     191,   191,   191,   513,   191,   191,   194,   191,   598,    61,
      61,   533,    61,   169,   600,    61,   291,    61,    61,    16,
     271,   158,   477,   566,   152,   642,    61,    61,    61,    61,
      61,   647,   341,   693,   315,   628,   628,   366,   203,   398,
     424,   368,   615,   699,    -1,   641
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   162,   190,   197,   198,   199,
     201,   211,   212,   214,   216,   219,   220,   221,   222,   223,
     245,   250,   251,   252,   253,   254,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   243,    78,
     243,   159,   160,   165,    35,    36,    37,    38,    39,    40,
      43,   215,    29,    30,    32,    44,    45,   155,   156,   157,
     159,   160,   164,   165,   166,   169,   170,   171,   175,   176,
      30,   153,   154,   155,     3,     3,   243,     3,   246,   247,
     163,   222,   223,     0,   193,   302,    20,    22,    24,   239,
       8,   224,   226,    74,   301,   301,   301,   301,   301,   303,
     243,    74,   300,   300,   300,   300,   300,   192,    14,   243,
      78,    79,   190,     3,     3,     3,   200,   201,   211,   212,
     216,   219,   220,   221,   250,   251,   252,   253,   254,     3,
     243,     6,   167,   168,   167,   168,     3,   170,   159,   243,
      64,    57,   194,     6,   191,   191,   199,    21,   190,   225,
     226,    67,   233,    69,   227,    75,     3,   243,   243,   243,
       3,    64,   190,   213,    76,     3,   243,   243,   243,     3,
       3,     3,   217,   218,    68,   236,     4,   299,   299,     3,
       4,     5,     6,    75,    76,    84,    86,   102,   103,   104,
     105,   129,   130,   131,   151,   173,   175,   177,   183,   184,
     185,   188,   190,   255,   257,   258,   259,   261,   262,   263,
     264,   265,   266,   269,   270,   271,   272,   273,   275,   276,
     277,   278,   279,   281,   282,   283,   284,   285,   286,   287,
     288,   291,   292,   293,   294,   295,   296,     3,     5,     6,
      64,   161,     3,     5,     6,    64,   161,     3,     5,     6,
      64,   161,    42,    46,    47,    51,    52,     3,     3,     6,
     243,   190,   247,   299,   225,   226,   255,    55,    70,   231,
      76,    57,   190,   213,   243,     3,   210,    34,   223,    64,
     180,   194,   236,   258,    80,    80,   190,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,    75,
      76,   259,   190,   190,    89,   258,   274,     4,     4,     4,
       4,     6,   296,   190,   117,   119,   120,   121,   190,   190,
     259,   259,     5,     6,   189,   279,   289,   290,   223,   258,
     191,   194,    57,   149,   150,    75,    77,   127,   144,   145,
     146,   147,   148,   152,   180,   181,   182,   183,   184,   185,
     186,   187,   192,   189,   194,   189,   194,   189,   194,   189,
     194,   189,   194,     3,     6,    80,   304,   224,   191,   191,
      78,   234,   228,   229,   258,   258,    71,   232,   221,     3,
     123,   125,   202,   203,   204,   209,    57,   190,   308,   309,
     191,   194,   190,   256,   243,   258,   218,   190,   190,    67,
     191,   255,   190,    75,   223,   258,   258,   274,    85,    87,
      89,     4,   190,   190,   190,   190,     4,     4,   195,   191,
     191,    79,   257,     3,   258,   258,    77,   152,   190,    75,
     126,   259,   259,   259,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   259,     3,   185,   279,     6,   289,
       6,   290,     6,     5,    46,    48,    49,   190,   191,   190,
     240,   241,   242,   243,   248,   172,   235,   194,    72,    73,
     230,   258,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   113,   116,   117,   120,   121,   122,   205,
     124,   190,   191,   194,   221,   210,   309,     3,   255,   194,
      70,    71,    81,    82,    83,   178,   297,   298,   297,   255,
     191,   223,   191,    57,    88,    85,    87,   258,   258,    78,
     258,     4,     3,   277,   258,   191,   194,   191,   194,     5,
       6,   299,   190,   259,   223,   255,   126,   149,   195,   195,
       6,     6,     3,   306,   307,   221,   194,    58,    60,    61,
      62,    63,    65,    66,   249,     3,    57,   244,   261,   262,
     263,   264,   265,   266,   267,   268,   236,   229,   190,   190,
     190,   190,   190,   190,    75,   123,   125,   126,   206,   207,
     304,   190,   210,    28,   305,   203,   191,   191,   190,     6,
       6,     4,     3,     6,   191,   194,   191,   191,   191,   205,
     258,   258,    85,    88,   259,   194,   194,   194,   194,     4,
       4,    80,   223,   255,   191,   191,   259,    53,    50,   180,
     191,   194,   191,   241,    59,    59,     3,   194,    54,   238,
       6,    91,    92,    93,    94,    95,    98,    99,   118,    91,
      92,    93,    94,    95,    98,    99,   118,    91,    92,    93,
      94,    95,    98,    99,   118,    91,    92,    93,    94,    95,
      98,    99,   118,    91,    92,    93,    94,    95,    98,    99,
     118,   126,   124,   128,   207,   208,   208,   210,   191,   190,
     158,   255,   298,   191,    85,   258,   191,   188,   291,     4,
     279,   188,   280,   283,   288,   291,   191,   191,   190,   191,
     191,     6,     6,     3,     5,     6,   307,   244,   242,   242,
     190,   267,    55,    56,   237,   191,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   278,   191,   306,
       3,   191,     6,   194,   191,   194,   194,   194,   297,    64,
     210,   255,   258,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   191,   304,     4,     4,     4,     4,
     191,   258,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   194,   191,   194,   191,   194,     4,
       4,   304,     6,   191,   194,   194,   260,   191,   304,     6,
       4,   191,   304,   191,   304
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   196,   197,   198,   198,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     201,   201,   201,   201,   201,   201,   202,   202,   203,   203,
     204,   204,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   206,   206,   207,   207,
     207,   207,   208,   208,   209,   209,   210,   210,   211,   212,
     212,   213,   213,   214,   215,   215,   215,   215,   215,   215,
     215,   215,   216,   217,   217,   218,   219,   219,   219,   219,
     219,   220,   220,   220,   221,   221,   221,   221,   222,   222,
     223,   224,   225,   225,   226,   227,   227,   228,   228,   229,
     230,   230,   230,   231,   231,   232,   232,   233,   233,   234,
     234,   235,   235,   236,   236,   237,   237,   238,   238,   239,
     239,   239,   239,   240,   240,   241,   241,   242,   242,   243,
     243,   244,   244,   244,   244,   245,   245,   246,   246,   247,
     248,   248,   249,   249,   249,   249,   249,   249,   249,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   251,
     251,   251,   252,   252,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   254,   255,   255,   256,   256,   257,   257,   258,   258,
     258,   258,   258,   259,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   259,   259,   260,   260,   261,   262,
     262,   263,   263,   264,   264,   265,   265,   266,   266,   267,
     267,   267,   267,   267,   267,   268,   268,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     270,   270,   271,   272,   272,   273,   273,   273,   273,   274,
     274,   275,   276,   276,   276,   276,   277,   277,   277,   277,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   279,   279,   279,   279,   280,   280,   280,   281,
     282,   282,   283,   283,   284,   285,   285,   286,   287,   287,
     288,   289,   290,   291,   291,   292,   293,   293,   294,   295,
     295,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   297,   297,   298,   298,   298,   298,   298,
     298,   299,   300,   300,   301,   301,   302,   302,   303,   303,
     304,   304,   305,   305,   306,   306,   307,   307,   307,   307,
     308,   308,   309,   309
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
       7,     9,     4,     5,     7,     9,     2,     3,     2,     2,
       2,     2,     2,     5,     2,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     3,     1,     3,     3,     5,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     0,    12,    14,
      12,    12,    10,     7,     9,     4,     6,     4,     6,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     3,     1,     1,     2,     2,     3,     2,     2,     3,
       2,     3,     3,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     2,     2,     1,     2,     2,
       2,     1,     2,     0,     3,     0,     1,     0,     2,     0,
       4,     0,     4,     0,     1,     3,     1,     3,     3,     3,
       1,     2,     6,     3
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
#line 2202 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2210 "parser.cpp"
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
#line 2224 "parser.cpp"
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
#line 2238 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2249 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2257 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2266 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2275 "parser.cpp"
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
#line 2289 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2300 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2310 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2320 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2330 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2340 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2350 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2360 "parser.cpp"
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
#line 2374 "parser.cpp"
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
#line 2388 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2398 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2406 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2414 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2423 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2431 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2439 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2447 "parser.cpp"
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
#line 2461 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2479 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2488 "parser.cpp"
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
#line 2501 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2510 "parser.cpp"
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
#line 2524 "parser.cpp"
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
#line 2538 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2548 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2557 "parser.cpp"
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
#line 2571 "parser.cpp"
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
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2596 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2604 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2612 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2620 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2628 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2636 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2644 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2660 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2668 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2676 "parser.cpp"
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
#line 2690 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2698 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2706 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2714 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2722 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2730 "parser.cpp"
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
#line 2743 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2751 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2791 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2807 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2815 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2823 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2831 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2839 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2847 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2855 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 364 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2863 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 368 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2871 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2887 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2895 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2911 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2919 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2927 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2938 "parser.cpp"
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
#line 2952 "parser.cpp"
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
#line 2966 "parser.cpp"
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
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list_one_pack: /* index_info_list_one_pack  */
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
#line 2994 "parser.cpp"
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

#line 3102 "parser.cpp"

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
#line 3317 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 496 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3328 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 502 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3339 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 509 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3345 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3351 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3357 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3363 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3369 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3375 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3381 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3387 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3393 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 518 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3399 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 519 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3405 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3411 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3417 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 523 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3423 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3429 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3435 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3441 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3447 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3453 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3459 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3465 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 531 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3471 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 532 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3477 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 533 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3483 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3489 "parser.cpp"
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
#line 3509 "parser.cpp"
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
#line 3527 "parser.cpp"
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
#line 3560 "parser.cpp"
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
#line 3580 "parser.cpp"
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
#line 3601 "parser.cpp"
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
#line 3634 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 666 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3643 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 670 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3652 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 676 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3660 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 679 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3668 "parser.cpp"
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
#line 3723 "parser.cpp"
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
#line 3762 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 772 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3768 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 773 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3774 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 774 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3780 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3786 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 776 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3792 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 777 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3798 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3804 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3810 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 780 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3816 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 781 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3822 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 782 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3828 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 783 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3834 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 784 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3840 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 785 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3846 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 786 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3852 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 787 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3858 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 788 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3864 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 789 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3870 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 790 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3876 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 793 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3882 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 795 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3888 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 796 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3894 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 797 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3900 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 798 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3906 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3912 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 802 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3918 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3924 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3930 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3936 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3942 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3948 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3954 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3960 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3966 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3972 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3978 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3984 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3990 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3996 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4002 "parser.cpp"
    break;

  case 82: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4008 "parser.cpp"
    break;

  case 83: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4014 "parser.cpp"
    break;

  case 84: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4020 "parser.cpp"
    break;

  case 85: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4026 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4032 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4038 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4044 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4050 "parser.cpp"
    break;

  case 90: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4056 "parser.cpp"
    break;

  case 91: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4062 "parser.cpp"
    break;

  case 92: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4068 "parser.cpp"
    break;

  case 93: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4074 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4080 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4086 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4092 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4098 "parser.cpp"
    break;

  case 98: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4104 "parser.cpp"
    break;

  case 99: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4110 "parser.cpp"
    break;

  case 100: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4116 "parser.cpp"
    break;

  case 101: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4122 "parser.cpp"
    break;

  case 102: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4128 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4134 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4140 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4146 "parser.cpp"
    break;

  case 106: /* column_constraints: column_constraint  */
#line 859 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4155 "parser.cpp"
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
#line 4169 "parser.cpp"
    break;

  case 108: /* column_constraint: PRIMARY KEY  */
#line 873 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4177 "parser.cpp"
    break;

  case 109: /* column_constraint: UNIQUE  */
#line 876 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4185 "parser.cpp"
    break;

  case 110: /* column_constraint: NULLABLE  */
#line 879 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4193 "parser.cpp"
    break;

  case 111: /* column_constraint: NOT NULLABLE  */
#line 882 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4201 "parser.cpp"
    break;

  case 112: /* default_expr: DEFAULT constant_expr  */
#line 886 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4209 "parser.cpp"
    break;

  case 113: /* default_expr: %empty  */
#line 889 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4217 "parser.cpp"
    break;

  case 114: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 894 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4227 "parser.cpp"
    break;

  case 115: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 899 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4237 "parser.cpp"
    break;

  case 116: /* identifier_array: IDENTIFIER  */
#line 906 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4248 "parser.cpp"
    break;

  case 117: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 912 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4259 "parser.cpp"
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
#line 4276 "parser.cpp"
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
#line 4315 "parser.cpp"
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
#line 4332 "parser.cpp"
    break;

  case 121: /* optional_identifier_array: '(' identifier_array ')'  */
#line 985 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4340 "parser.cpp"
    break;

  case 122: /* optional_identifier_array: %empty  */
#line 988 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4348 "parser.cpp"
    break;

  case 123: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 995 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4358 "parser.cpp"
    break;

  case 124: /* explain_type: ANALYZE  */
#line 1001 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4366 "parser.cpp"
    break;

  case 125: /* explain_type: AST  */
#line 1004 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4374 "parser.cpp"
    break;

  case 126: /* explain_type: RAW  */
#line 1007 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4382 "parser.cpp"
    break;

  case 127: /* explain_type: LOGICAL  */
#line 1010 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4390 "parser.cpp"
    break;

  case 128: /* explain_type: PHYSICAL  */
#line 1013 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4398 "parser.cpp"
    break;

  case 129: /* explain_type: PIPELINE  */
#line 1016 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4406 "parser.cpp"
    break;

  case 130: /* explain_type: FRAGMENT  */
#line 1019 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4414 "parser.cpp"
    break;

  case 131: /* explain_type: %empty  */
#line 1022 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4422 "parser.cpp"
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
#line 4439 "parser.cpp"
    break;

  case 133: /* update_expr_array: update_expr  */
#line 1042 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4448 "parser.cpp"
    break;

  case 134: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1046 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4457 "parser.cpp"
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
#line 4469 "parser.cpp"
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
#line 4485 "parser.cpp"
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
#line 4503 "parser.cpp"
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
#line 4521 "parser.cpp"
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
#line 4539 "parser.cpp"
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
#line 4562 "parser.cpp"
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
            case infinity::CopyOptionType::kOffset: {
                (yyval.copy_stmt)->offset_ = option_ptr->offset_;
                break;
            }
            case infinity::CopyOptionType::kLimit: {
                (yyval.copy_stmt)->limit_ = option_ptr->limit_;
                break;
            }
            case infinity::CopyOptionType::kRowLimit: {
                (yyval.copy_stmt)->row_limit_ = option_ptr->row_limit_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 4620 "parser.cpp"
    break;

  case 142: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1198 "parser.y"
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
            case infinity::CopyOptionType::kOffset: {
                (yyval.copy_stmt)->offset_ = option_ptr->offset_;
                break;
            }
            case infinity::CopyOptionType::kLimit: {
                (yyval.copy_stmt)->limit_ = option_ptr->limit_;
                break;
            }
            case infinity::CopyOptionType::kRowLimit: {
                (yyval.copy_stmt)->row_limit_ = option_ptr->row_limit_;
                break;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 4680 "parser.cpp"
    break;

  case 143: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1253 "parser.y"
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
            default: {
                delete option_ptr;
                delete (yyvsp[-1].copy_option_array);
                yyerror(&yyloc, scanner, result, "Invalid import option");
                YYERROR;
            }
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 4732 "parser.cpp"
    break;

  case 144: /* select_statement: select_without_paren  */
#line 1304 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4740 "parser.cpp"
    break;

  case 145: /* select_statement: select_with_paren  */
#line 1307 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4748 "parser.cpp"
    break;

  case 146: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1310 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4762 "parser.cpp"
    break;

  case 147: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1319 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4776 "parser.cpp"
    break;

  case 148: /* select_with_paren: '(' select_without_paren ')'  */
#line 1329 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4784 "parser.cpp"
    break;

  case 149: /* select_with_paren: '(' select_with_paren ')'  */
#line 1332 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4792 "parser.cpp"
    break;

  case 150: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1336 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4801 "parser.cpp"
    break;

  case 151: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1341 "parser.y"
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
#line 4827 "parser.cpp"
    break;

  case 152: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1363 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4835 "parser.cpp"
    break;

  case 153: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1366 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4843 "parser.cpp"
    break;

  case 154: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1371 "parser.y"
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
#line 4863 "parser.cpp"
    break;

  case 155: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1387 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4871 "parser.cpp"
    break;

  case 156: /* order_by_clause: %empty  */
#line 1390 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4879 "parser.cpp"
    break;

  case 157: /* order_by_expr_list: order_by_expr  */
#line 1394 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4888 "parser.cpp"
    break;

  case 158: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1398 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4897 "parser.cpp"
    break;

  case 159: /* order_by_expr: expr order_by_type  */
#line 1403 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4907 "parser.cpp"
    break;

  case 160: /* order_by_type: ASC  */
#line 1409 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4915 "parser.cpp"
    break;

  case 161: /* order_by_type: DESC  */
#line 1412 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4923 "parser.cpp"
    break;

  case 162: /* order_by_type: %empty  */
#line 1415 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4931 "parser.cpp"
    break;

  case 163: /* limit_expr: LIMIT expr  */
#line 1419 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4939 "parser.cpp"
    break;

  case 164: /* limit_expr: %empty  */
#line 1423 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4945 "parser.cpp"
    break;

  case 165: /* offset_expr: OFFSET expr  */
#line 1425 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4953 "parser.cpp"
    break;

  case 166: /* offset_expr: %empty  */
#line 1429 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4959 "parser.cpp"
    break;

  case 167: /* distinct: DISTINCT  */
#line 1431 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4967 "parser.cpp"
    break;

  case 168: /* distinct: %empty  */
#line 1434 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4975 "parser.cpp"
    break;

  case 169: /* from_clause: FROM table_reference  */
#line 1438 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4983 "parser.cpp"
    break;

  case 170: /* from_clause: %empty  */
#line 1441 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4991 "parser.cpp"
    break;

  case 171: /* search_clause: SEARCH sub_search_array  */
#line 1445 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5001 "parser.cpp"
    break;

  case 172: /* search_clause: %empty  */
#line 1450 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5009 "parser.cpp"
    break;

  case 173: /* where_clause: WHERE expr  */
#line 1454 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5017 "parser.cpp"
    break;

  case 174: /* where_clause: %empty  */
#line 1457 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5025 "parser.cpp"
    break;

  case 175: /* having_clause: HAVING expr  */
#line 1461 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5033 "parser.cpp"
    break;

  case 176: /* having_clause: %empty  */
#line 1464 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5041 "parser.cpp"
    break;

  case 177: /* group_by_clause: GROUP BY expr_array  */
#line 1468 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5049 "parser.cpp"
    break;

  case 178: /* group_by_clause: %empty  */
#line 1471 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5057 "parser.cpp"
    break;

  case 179: /* set_operator: UNION  */
#line 1475 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5065 "parser.cpp"
    break;

  case 180: /* set_operator: UNION ALL  */
#line 1478 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5073 "parser.cpp"
    break;

  case 181: /* set_operator: INTERSECT  */
#line 1481 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5081 "parser.cpp"
    break;

  case 182: /* set_operator: EXCEPT  */
#line 1484 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5089 "parser.cpp"
    break;

  case 183: /* table_reference: table_reference_unit  */
#line 1492 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5097 "parser.cpp"
    break;

  case 184: /* table_reference: table_reference ',' table_reference_unit  */
#line 1495 "parser.y"
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
#line 5115 "parser.cpp"
    break;

  case 187: /* table_reference_name: table_name table_alias  */
#line 1512 "parser.y"
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
#line 5133 "parser.cpp"
    break;

  case 188: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1526 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5144 "parser.cpp"
    break;

  case 189: /* table_name: IDENTIFIER  */
#line 1535 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5154 "parser.cpp"
    break;

  case 190: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1540 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5166 "parser.cpp"
    break;

  case 191: /* table_alias: AS IDENTIFIER  */
#line 1549 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5176 "parser.cpp"
    break;

  case 192: /* table_alias: IDENTIFIER  */
#line 1554 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5186 "parser.cpp"
    break;

  case 193: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1559 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5197 "parser.cpp"
    break;

  case 194: /* table_alias: %empty  */
#line 1565 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5205 "parser.cpp"
    break;

  case 195: /* with_clause: WITH with_expr_list  */
#line 1572 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5213 "parser.cpp"
    break;

  case 196: /* with_clause: %empty  */
#line 1575 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5221 "parser.cpp"
    break;

  case 197: /* with_expr_list: with_expr  */
#line 1579 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5230 "parser.cpp"
    break;

  case 198: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1582 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5239 "parser.cpp"
    break;

  case 199: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1587 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5251 "parser.cpp"
    break;

  case 200: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1599 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5263 "parser.cpp"
    break;

  case 201: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1606 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5276 "parser.cpp"
    break;

  case 202: /* join_type: INNER  */
#line 1620 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5284 "parser.cpp"
    break;

  case 203: /* join_type: LEFT  */
#line 1623 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5292 "parser.cpp"
    break;

  case 204: /* join_type: RIGHT  */
#line 1626 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5300 "parser.cpp"
    break;

  case 205: /* join_type: OUTER  */
#line 1629 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5308 "parser.cpp"
    break;

  case 206: /* join_type: FULL  */
#line 1632 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5316 "parser.cpp"
    break;

  case 207: /* join_type: CROSS  */
#line 1635 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5324 "parser.cpp"
    break;

  case 208: /* join_type: %empty  */
#line 1638 "parser.y"
                {
}
#line 5331 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW DATABASES  */
#line 1644 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5340 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW TABLES  */
#line 1648 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5349 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW VIEWS  */
#line 1652 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5358 "parser.cpp"
    break;

  case 212: /* show_statement: SHOW CONFIGS  */
#line 1656 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5367 "parser.cpp"
    break;

  case 213: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1660 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5379 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW PROFILES  */
#line 1667 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5388 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW BUFFER  */
#line 1671 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5397 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW QUERIES  */
#line 1675 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5406 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1679 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5416 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW TRANSACTIONS  */
#line 1684 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5425 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1688 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5435 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW SESSION VARIABLES  */
#line 1693 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5444 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1697 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5453 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1701 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5464 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1707 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5475 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1713 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5486 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW TABLE table_name  */
#line 1719 "parser.y"
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
#line 5502 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1730 "parser.y"
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
#line 5518 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1741 "parser.y"
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
#line 5534 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1752 "parser.y"
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
#line 5551 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1764 "parser.y"
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
#line 5568 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1776 "parser.y"
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
#line 5586 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1789 "parser.y"
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
#line 5605 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1803 "parser.y"
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
#line 5621 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1814 "parser.y"
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
#line 5640 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1828 "parser.y"
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
#line 5661 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1844 "parser.y"
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
#line 5683 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW LOGS  */
#line 1861 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5692 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW DELTA LOGS  */
#line 1865 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5701 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW CATALOGS  */
#line 1869 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5710 "parser.cpp"
    break;

  case 239: /* flush_statement: FLUSH DATA  */
#line 1877 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5719 "parser.cpp"
    break;

  case 240: /* flush_statement: FLUSH LOG  */
#line 1881 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5728 "parser.cpp"
    break;

  case 241: /* flush_statement: FLUSH BUFFER  */
#line 1885 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5737 "parser.cpp"
    break;

  case 242: /* optimize_statement: OPTIMIZE table_name  */
#line 1893 "parser.y"
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
#line 5752 "parser.cpp"
    break;

  case 243: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1904 "parser.y"
                                                         {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    (yyval.optimize_stmt)->index_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));

    for (auto *&index_param : *(yyvsp[0].with_index_param_list_t)) {
        (yyval.optimize_stmt)->opt_params_.emplace_back(std::unique_ptr<infinity::InitParameter>(index_param));
        index_param = nullptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
}
#line 5776 "parser.cpp"
    break;

  case 244: /* command_statement: USE IDENTIFIER  */
#line 1927 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5787 "parser.cpp"
    break;

  case 245: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1933 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5797 "parser.cpp"
    break;

  case 246: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1938 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5808 "parser.cpp"
    break;

  case 247: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1944 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5819 "parser.cpp"
    break;

  case 248: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1950 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5832 "parser.cpp"
    break;

  case 249: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1958 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5843 "parser.cpp"
    break;

  case 250: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1964 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5854 "parser.cpp"
    break;

  case 251: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1970 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5865 "parser.cpp"
    break;

  case 252: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1976 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5876 "parser.cpp"
    break;

  case 253: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1982 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5889 "parser.cpp"
    break;

  case 254: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1990 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5900 "parser.cpp"
    break;

  case 255: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1996 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5911 "parser.cpp"
    break;

  case 256: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2002 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5922 "parser.cpp"
    break;

  case 257: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2008 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5933 "parser.cpp"
    break;

  case 258: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2014 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5946 "parser.cpp"
    break;

  case 259: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2022 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5957 "parser.cpp"
    break;

  case 260: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2028 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5968 "parser.cpp"
    break;

  case 261: /* compact_statement: COMPACT TABLE table_name  */
#line 2035 "parser.y"
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
#line 5985 "parser.cpp"
    break;

  case 262: /* expr_array: expr_alias  */
#line 2052 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5994 "parser.cpp"
    break;

  case 263: /* expr_array: expr_array ',' expr_alias  */
#line 2056 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6003 "parser.cpp"
    break;

  case 264: /* expr_array_list: '(' expr_array ')'  */
#line 2061 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6012 "parser.cpp"
    break;

  case 265: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2065 "parser.y"
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
#line 6032 "parser.cpp"
    break;

  case 266: /* expr_alias: expr AS IDENTIFIER  */
#line 2092 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6043 "parser.cpp"
    break;

  case 267: /* expr_alias: expr  */
#line 2098 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6051 "parser.cpp"
    break;

  case 273: /* operand: '(' expr ')'  */
#line 2108 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6059 "parser.cpp"
    break;

  case 274: /* operand: '(' select_without_paren ')'  */
#line 2111 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6070 "parser.cpp"
    break;

  case 275: /* operand: constant_expr  */
#line 2117 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6078 "parser.cpp"
    break;

  case 286: /* extra_match_tensor_option: ',' STRING  */
#line 2131 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6086 "parser.cpp"
    break;

  case 287: /* extra_match_tensor_option: %empty  */
#line 2134 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6094 "parser.cpp"
    break;

  case 288: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2140 "parser.y"
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
#line 6115 "parser.cpp"
    break;

  case 289: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2159 "parser.y"
                                                                                                                        {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-4].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    if (!check) {
        goto Error1;
    }
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 6161 "parser.cpp"
    break;

  case 290: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2201 "parser.y"
                                                                                     {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-8].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-2].str_value));
    if (!check) {
        goto Error2;
    }

    // vector search data type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    if (!check) {
        goto Error2;
    }
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return2;

Error2:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 6208 "parser.cpp"
    break;

  case 291: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2247 "parser.y"
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
#line 6230 "parser.cpp"
    break;

  case 292: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2265 "parser.y"
                                                                                        {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-6].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-4].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-2].str_value));

    // topn and options
    match_sparse_expr->SetOptParams(infinity::DEFAULT_MATCH_SPARSE_TOP_N, (yyvsp[0].with_index_param_list_t));
}
#line 6252 "parser.cpp"
    break;

  case 293: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2283 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6265 "parser.cpp"
    break;

  case 294: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2291 "parser.y"
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
#line 6280 "parser.cpp"
    break;

  case 295: /* query_expr: QUERY '(' STRING ')'  */
#line 2302 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6291 "parser.cpp"
    break;

  case 296: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2308 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6304 "parser.cpp"
    break;

  case 297: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2317 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6315 "parser.cpp"
    break;

  case 298: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2323 "parser.y"
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
#line 6331 "parser.cpp"
    break;

  case 299: /* sub_search: match_vector_expr  */
#line 2335 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6339 "parser.cpp"
    break;

  case 300: /* sub_search: match_text_expr  */
#line 2338 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6347 "parser.cpp"
    break;

  case 301: /* sub_search: match_tensor_expr  */
#line 2341 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6355 "parser.cpp"
    break;

  case 302: /* sub_search: match_sparse_expr  */
#line 2344 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6363 "parser.cpp"
    break;

  case 303: /* sub_search: query_expr  */
#line 2347 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6371 "parser.cpp"
    break;

  case 304: /* sub_search: fusion_expr  */
#line 2350 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6379 "parser.cpp"
    break;

  case 305: /* sub_search_array: sub_search  */
#line 2354 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6388 "parser.cpp"
    break;

  case 306: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2358 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6397 "parser.cpp"
    break;

  case 307: /* function_expr: IDENTIFIER '(' ')'  */
#line 2363 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6410 "parser.cpp"
    break;

  case 308: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2371 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6423 "parser.cpp"
    break;

  case 309: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2379 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6437 "parser.cpp"
    break;

  case 310: /* function_expr: operand IS NOT NULLABLE  */
#line 2388 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6449 "parser.cpp"
    break;

  case 311: /* function_expr: operand IS NULLABLE  */
#line 2395 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6461 "parser.cpp"
    break;

  case 312: /* function_expr: NOT operand  */
#line 2402 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6473 "parser.cpp"
    break;

  case 313: /* function_expr: '-' operand  */
#line 2409 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6485 "parser.cpp"
    break;

  case 314: /* function_expr: '+' operand  */
#line 2416 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6497 "parser.cpp"
    break;

  case 315: /* function_expr: operand '-' operand  */
#line 2423 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6510 "parser.cpp"
    break;

  case 316: /* function_expr: operand '+' operand  */
#line 2431 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6523 "parser.cpp"
    break;

  case 317: /* function_expr: operand '*' operand  */
#line 2439 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6536 "parser.cpp"
    break;

  case 318: /* function_expr: operand '/' operand  */
#line 2447 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6549 "parser.cpp"
    break;

  case 319: /* function_expr: operand '%' operand  */
#line 2455 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6562 "parser.cpp"
    break;

  case 320: /* function_expr: operand '=' operand  */
#line 2463 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6575 "parser.cpp"
    break;

  case 321: /* function_expr: operand EQUAL operand  */
#line 2471 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6588 "parser.cpp"
    break;

  case 322: /* function_expr: operand NOT_EQ operand  */
#line 2479 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6601 "parser.cpp"
    break;

  case 323: /* function_expr: operand '<' operand  */
#line 2487 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6614 "parser.cpp"
    break;

  case 324: /* function_expr: operand '>' operand  */
#line 2495 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6627 "parser.cpp"
    break;

  case 325: /* function_expr: operand LESS_EQ operand  */
#line 2503 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6640 "parser.cpp"
    break;

  case 326: /* function_expr: operand GREATER_EQ operand  */
#line 2511 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6653 "parser.cpp"
    break;

  case 327: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2519 "parser.y"
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
#line 6688 "parser.cpp"
    break;

  case 328: /* function_expr: operand LIKE operand  */
#line 2549 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6701 "parser.cpp"
    break;

  case 329: /* function_expr: operand NOT LIKE operand  */
#line 2557 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6714 "parser.cpp"
    break;

  case 330: /* conjunction_expr: expr AND expr  */
#line 2566 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6727 "parser.cpp"
    break;

  case 331: /* conjunction_expr: expr OR expr  */
#line 2574 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6740 "parser.cpp"
    break;

  case 332: /* between_expr: operand BETWEEN operand AND operand  */
#line 2583 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6752 "parser.cpp"
    break;

  case 333: /* in_expr: operand IN '(' expr_array ')'  */
#line 2591 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6763 "parser.cpp"
    break;

  case 334: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2597 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6774 "parser.cpp"
    break;

  case 335: /* case_expr: CASE expr case_check_array END  */
#line 2604 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6785 "parser.cpp"
    break;

  case 336: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2610 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6797 "parser.cpp"
    break;

  case 337: /* case_expr: CASE case_check_array END  */
#line 2617 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6807 "parser.cpp"
    break;

  case 338: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2622 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6818 "parser.cpp"
    break;

  case 339: /* case_check_array: WHEN expr THEN expr  */
#line 2629 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6830 "parser.cpp"
    break;

  case 340: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2636 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6842 "parser.cpp"
    break;

  case 341: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2644 "parser.y"
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
#line 6870 "parser.cpp"
    break;

  case 342: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2668 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6881 "parser.cpp"
    break;

  case 343: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2674 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6892 "parser.cpp"
    break;

  case 344: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2680 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6904 "parser.cpp"
    break;

  case 345: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2687 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6916 "parser.cpp"
    break;

  case 346: /* column_expr: IDENTIFIER  */
#line 2695 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6928 "parser.cpp"
    break;

  case 347: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2702 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6940 "parser.cpp"
    break;

  case 348: /* column_expr: '*'  */
#line 2709 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6950 "parser.cpp"
    break;

  case 349: /* column_expr: column_expr '.' '*'  */
#line 2714 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6964 "parser.cpp"
    break;

  case 350: /* constant_expr: STRING  */
#line 2724 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6974 "parser.cpp"
    break;

  case 351: /* constant_expr: TRUE  */
#line 2729 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6984 "parser.cpp"
    break;

  case 352: /* constant_expr: FALSE  */
#line 2734 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6994 "parser.cpp"
    break;

  case 353: /* constant_expr: DOUBLE_VALUE  */
#line 2739 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7004 "parser.cpp"
    break;

  case 354: /* constant_expr: LONG_VALUE  */
#line 2744 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7014 "parser.cpp"
    break;

  case 355: /* constant_expr: DATE STRING  */
#line 2749 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7024 "parser.cpp"
    break;

  case 356: /* constant_expr: TIME STRING  */
#line 2754 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7034 "parser.cpp"
    break;

  case 357: /* constant_expr: DATETIME STRING  */
#line 2759 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7044 "parser.cpp"
    break;

  case 358: /* constant_expr: TIMESTAMP STRING  */
#line 2764 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7054 "parser.cpp"
    break;

  case 359: /* constant_expr: INTERVAL interval_expr  */
#line 2769 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7062 "parser.cpp"
    break;

  case 360: /* constant_expr: interval_expr  */
#line 2772 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7070 "parser.cpp"
    break;

  case 361: /* constant_expr: common_array_expr  */
#line 2775 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7078 "parser.cpp"
    break;

  case 362: /* common_array_expr: array_expr  */
#line 2779 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7086 "parser.cpp"
    break;

  case 363: /* common_array_expr: subarray_array_expr  */
#line 2782 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7094 "parser.cpp"
    break;

  case 364: /* common_array_expr: sparse_array_expr  */
#line 2785 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7102 "parser.cpp"
    break;

  case 365: /* common_array_expr: empty_array_expr  */
#line 2788 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7110 "parser.cpp"
    break;

  case 366: /* common_sparse_array_expr: sparse_array_expr  */
#line 2792 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7118 "parser.cpp"
    break;

  case 367: /* common_sparse_array_expr: array_expr  */
#line 2795 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7126 "parser.cpp"
    break;

  case 368: /* common_sparse_array_expr: empty_array_expr  */
#line 2798 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7134 "parser.cpp"
    break;

  case 369: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2802 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7142 "parser.cpp"
    break;

  case 370: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2806 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7152 "parser.cpp"
    break;

  case 371: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2811 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7161 "parser.cpp"
    break;

  case 372: /* sparse_array_expr: long_sparse_array_expr  */
#line 2816 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7169 "parser.cpp"
    break;

  case 373: /* sparse_array_expr: double_sparse_array_expr  */
#line 2819 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7177 "parser.cpp"
    break;

  case 374: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2823 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7185 "parser.cpp"
    break;

  case 375: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2827 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7197 "parser.cpp"
    break;

  case 376: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2834 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7208 "parser.cpp"
    break;

  case 377: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2841 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7216 "parser.cpp"
    break;

  case 378: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2845 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7228 "parser.cpp"
    break;

  case 379: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2852 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7239 "parser.cpp"
    break;

  case 380: /* empty_array_expr: '[' ']'  */
#line 2859 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7247 "parser.cpp"
    break;

  case 381: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 2863 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7255 "parser.cpp"
    break;

  case 382: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 2867 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7263 "parser.cpp"
    break;

  case 383: /* array_expr: long_array_expr  */
#line 2871 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7271 "parser.cpp"
    break;

  case 384: /* array_expr: double_array_expr  */
#line 2874 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7279 "parser.cpp"
    break;

  case 385: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2878 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7287 "parser.cpp"
    break;

  case 386: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2882 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7297 "parser.cpp"
    break;

  case 387: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2887 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7306 "parser.cpp"
    break;

  case 388: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2892 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7314 "parser.cpp"
    break;

  case 389: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2896 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7324 "parser.cpp"
    break;

  case 390: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2901 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7333 "parser.cpp"
    break;

  case 391: /* interval_expr: LONG_VALUE SECONDS  */
#line 2906 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7344 "parser.cpp"
    break;

  case 392: /* interval_expr: LONG_VALUE SECOND  */
#line 2912 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7355 "parser.cpp"
    break;

  case 393: /* interval_expr: LONG_VALUE MINUTES  */
#line 2918 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7366 "parser.cpp"
    break;

  case 394: /* interval_expr: LONG_VALUE MINUTE  */
#line 2924 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7377 "parser.cpp"
    break;

  case 395: /* interval_expr: LONG_VALUE HOURS  */
#line 2930 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7388 "parser.cpp"
    break;

  case 396: /* interval_expr: LONG_VALUE HOUR  */
#line 2936 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7399 "parser.cpp"
    break;

  case 397: /* interval_expr: LONG_VALUE DAYS  */
#line 2942 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7410 "parser.cpp"
    break;

  case 398: /* interval_expr: LONG_VALUE DAY  */
#line 2948 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7421 "parser.cpp"
    break;

  case 399: /* interval_expr: LONG_VALUE MONTHS  */
#line 2954 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7432 "parser.cpp"
    break;

  case 400: /* interval_expr: LONG_VALUE MONTH  */
#line 2960 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7443 "parser.cpp"
    break;

  case 401: /* interval_expr: LONG_VALUE YEARS  */
#line 2966 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7454 "parser.cpp"
    break;

  case 402: /* interval_expr: LONG_VALUE YEAR  */
#line 2972 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7465 "parser.cpp"
    break;

  case 403: /* copy_option_list: copy_option  */
#line 2983 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7474 "parser.cpp"
    break;

  case 404: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2987 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7483 "parser.cpp"
    break;

  case 405: /* copy_option: FORMAT IDENTIFIER  */
#line 2992 "parser.y"
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
    } else if (strcasecmp((yyvsp[0].str_value), "parquet") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kPARQUET;
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 7518 "parser.cpp"
    break;

  case 406: /* copy_option: DELIMITER STRING  */
#line 3022 "parser.y"
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
#line 7533 "parser.cpp"
    break;

  case 407: /* copy_option: HEADER  */
#line 3032 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7543 "parser.cpp"
    break;

  case 408: /* copy_option: OFFSET LONG_VALUE  */
#line 3037 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 7553 "parser.cpp"
    break;

  case 409: /* copy_option: LIMIT LONG_VALUE  */
#line 3042 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 7563 "parser.cpp"
    break;

  case 410: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3047 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 7573 "parser.cpp"
    break;

  case 411: /* file_path: STRING  */
#line 3053 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7581 "parser.cpp"
    break;

  case 412: /* if_exists: IF EXISTS  */
#line 3057 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7587 "parser.cpp"
    break;

  case 413: /* if_exists: %empty  */
#line 3058 "parser.y"
  { (yyval.bool_value) = false; }
#line 7593 "parser.cpp"
    break;

  case 414: /* if_not_exists: IF NOT EXISTS  */
#line 3060 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7599 "parser.cpp"
    break;

  case 415: /* if_not_exists: %empty  */
#line 3061 "parser.y"
  { (yyval.bool_value) = false; }
#line 7605 "parser.cpp"
    break;

  case 418: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3076 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7618 "parser.cpp"
    break;

  case 419: /* if_not_exists_info: %empty  */
#line 3084 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7626 "parser.cpp"
    break;

  case 420: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3088 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7634 "parser.cpp"
    break;

  case 421: /* with_index_param_list: %empty  */
#line 3091 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7642 "parser.cpp"
    break;

  case 422: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3095 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7650 "parser.cpp"
    break;

  case 423: /* optional_table_properties_list: %empty  */
#line 3098 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7658 "parser.cpp"
    break;

  case 424: /* index_param_list: index_param  */
#line 3102 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7667 "parser.cpp"
    break;

  case 425: /* index_param_list: index_param_list ',' index_param  */
#line 3106 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7676 "parser.cpp"
    break;

  case 426: /* index_param: IDENTIFIER  */
#line 3111 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7687 "parser.cpp"
    break;

  case 427: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3117 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7702 "parser.cpp"
    break;

  case 428: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3127 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7714 "parser.cpp"
    break;

  case 429: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3134 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7726 "parser.cpp"
    break;

  case 430: /* index_info_list: index_info_list_one_pack  */
#line 3145 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 7735 "parser.cpp"
    break;

  case 431: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3149 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 7747 "parser.cpp"
    break;

  case 432: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3157 "parser.y"
                                                                                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "bmp") == 0) {
        index_type = infinity::IndexType::kBMP;
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
#line 7804 "parser.cpp"
    break;

  case 433: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3209 "parser.y"
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
#line 7822 "parser.cpp"
    break;


#line 7826 "parser.cpp"

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

#line 3223 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

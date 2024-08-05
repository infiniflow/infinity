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
  YYSYMBOL_FLOAT16 = 105,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 106,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 107,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 108,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 109,                     /* UUID  */
  YYSYMBOL_POINT = 110,                    /* POINT  */
  YYSYMBOL_LINE = 111,                     /* LINE  */
  YYSYMBOL_LSEG = 112,                     /* LSEG  */
  YYSYMBOL_BOX = 113,                      /* BOX  */
  YYSYMBOL_PATH = 114,                     /* PATH  */
  YYSYMBOL_POLYGON = 115,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 116,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 117,                     /* BLOB  */
  YYSYMBOL_BITMAP = 118,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 119,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 120,                   /* VECTOR  */
  YYSYMBOL_BIT = 121,                      /* BIT  */
  YYSYMBOL_TEXT = 122,                     /* TEXT  */
  YYSYMBOL_TENSOR = 123,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 124,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 125,              /* TENSORARRAY  */
  YYSYMBOL_PRIMARY = 126,                  /* PRIMARY  */
  YYSYMBOL_KEY = 127,                      /* KEY  */
  YYSYMBOL_UNIQUE = 128,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 129,                 /* NULLABLE  */
  YYSYMBOL_IS = 130,                       /* IS  */
  YYSYMBOL_DEFAULT = 131,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 132,                     /* TRUE  */
  YYSYMBOL_FALSE = 133,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 134,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 135,                   /* SECOND  */
  YYSYMBOL_SECONDS = 136,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 137,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 138,                  /* MINUTES  */
  YYSYMBOL_HOUR = 139,                     /* HOUR  */
  YYSYMBOL_HOURS = 140,                    /* HOURS  */
  YYSYMBOL_DAY = 141,                      /* DAY  */
  YYSYMBOL_DAYS = 142,                     /* DAYS  */
  YYSYMBOL_MONTH = 143,                    /* MONTH  */
  YYSYMBOL_MONTHS = 144,                   /* MONTHS  */
  YYSYMBOL_YEAR = 145,                     /* YEAR  */
  YYSYMBOL_YEARS = 146,                    /* YEARS  */
  YYSYMBOL_EQUAL = 147,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 148,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 149,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 150,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 151,                  /* BETWEEN  */
  YYSYMBOL_AND = 152,                      /* AND  */
  YYSYMBOL_OR = 153,                       /* OR  */
  YYSYMBOL_EXTRACT = 154,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 155,                     /* LIKE  */
  YYSYMBOL_DATA = 156,                     /* DATA  */
  YYSYMBOL_LOG = 157,                      /* LOG  */
  YYSYMBOL_BUFFER = 158,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 159,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 160,              /* TRANSACTION  */
  YYSYMBOL_USING = 161,                    /* USING  */
  YYSYMBOL_SESSION = 162,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 163,                   /* GLOBAL  */
  YYSYMBOL_OFF = 164,                      /* OFF  */
  YYSYMBOL_EXPORT = 165,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 166,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 167,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 168,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 169,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 170,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 171,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 172,                    /* DELTA  */
  YYSYMBOL_LOGS = 173,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 174,                 /* CATALOGS  */
  YYSYMBOL_SEARCH = 175,                   /* SEARCH  */
  YYSYMBOL_MATCH = 176,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 177,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 178,                    /* QUERY  */
  YYSYMBOL_QUERIES = 179,                  /* QUERIES  */
  YYSYMBOL_FUSION = 180,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 181,                 /* ROWLIMIT  */
  YYSYMBOL_NUMBER = 182,                   /* NUMBER  */
  YYSYMBOL_183_ = 183,                     /* '='  */
  YYSYMBOL_184_ = 184,                     /* '<'  */
  YYSYMBOL_185_ = 185,                     /* '>'  */
  YYSYMBOL_186_ = 186,                     /* '+'  */
  YYSYMBOL_187_ = 187,                     /* '-'  */
  YYSYMBOL_188_ = 188,                     /* '*'  */
  YYSYMBOL_189_ = 189,                     /* '/'  */
  YYSYMBOL_190_ = 190,                     /* '%'  */
  YYSYMBOL_191_ = 191,                     /* '['  */
  YYSYMBOL_192_ = 192,                     /* ']'  */
  YYSYMBOL_193_ = 193,                     /* '('  */
  YYSYMBOL_194_ = 194,                     /* ')'  */
  YYSYMBOL_195_ = 195,                     /* '.'  */
  YYSYMBOL_196_ = 196,                     /* ';'  */
  YYSYMBOL_197_ = 197,                     /* ','  */
  YYSYMBOL_198_ = 198,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 199,                 /* $accept  */
  YYSYMBOL_input_pattern = 200,            /* input_pattern  */
  YYSYMBOL_statement_list = 201,           /* statement_list  */
  YYSYMBOL_statement = 202,                /* statement  */
  YYSYMBOL_explainable_statement = 203,    /* explainable_statement  */
  YYSYMBOL_create_statement = 204,         /* create_statement  */
  YYSYMBOL_table_element_array = 205,      /* table_element_array  */
  YYSYMBOL_table_element = 206,            /* table_element  */
  YYSYMBOL_table_column = 207,             /* table_column  */
  YYSYMBOL_column_type = 208,              /* column_type  */
  YYSYMBOL_column_constraints = 209,       /* column_constraints  */
  YYSYMBOL_column_constraint = 210,        /* column_constraint  */
  YYSYMBOL_default_expr = 211,             /* default_expr  */
  YYSYMBOL_table_constraint = 212,         /* table_constraint  */
  YYSYMBOL_identifier_array = 213,         /* identifier_array  */
  YYSYMBOL_delete_statement = 214,         /* delete_statement  */
  YYSYMBOL_insert_statement = 215,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 216, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 217,        /* explain_statement  */
  YYSYMBOL_explain_type = 218,             /* explain_type  */
  YYSYMBOL_update_statement = 219,         /* update_statement  */
  YYSYMBOL_update_expr_array = 220,        /* update_expr_array  */
  YYSYMBOL_update_expr = 221,              /* update_expr  */
  YYSYMBOL_drop_statement = 222,           /* drop_statement  */
  YYSYMBOL_copy_statement = 223,           /* copy_statement  */
  YYSYMBOL_select_statement = 224,         /* select_statement  */
  YYSYMBOL_select_with_paren = 225,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 226,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 227, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 228, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 229, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 230,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 231,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 232,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 233,            /* order_by_type  */
  YYSYMBOL_limit_expr = 234,               /* limit_expr  */
  YYSYMBOL_offset_expr = 235,              /* offset_expr  */
  YYSYMBOL_distinct = 236,                 /* distinct  */
  YYSYMBOL_from_clause = 237,              /* from_clause  */
  YYSYMBOL_search_clause = 238,            /* search_clause  */
  YYSYMBOL_where_clause = 239,             /* where_clause  */
  YYSYMBOL_having_clause = 240,            /* having_clause  */
  YYSYMBOL_group_by_clause = 241,          /* group_by_clause  */
  YYSYMBOL_set_operator = 242,             /* set_operator  */
  YYSYMBOL_table_reference = 243,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 244,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 245,     /* table_reference_name  */
  YYSYMBOL_table_name = 246,               /* table_name  */
  YYSYMBOL_table_alias = 247,              /* table_alias  */
  YYSYMBOL_with_clause = 248,              /* with_clause  */
  YYSYMBOL_with_expr_list = 249,           /* with_expr_list  */
  YYSYMBOL_with_expr = 250,                /* with_expr  */
  YYSYMBOL_join_clause = 251,              /* join_clause  */
  YYSYMBOL_join_type = 252,                /* join_type  */
  YYSYMBOL_show_statement = 253,           /* show_statement  */
  YYSYMBOL_flush_statement = 254,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 255,       /* optimize_statement  */
  YYSYMBOL_command_statement = 256,        /* command_statement  */
  YYSYMBOL_compact_statement = 257,        /* compact_statement  */
  YYSYMBOL_expr_array = 258,               /* expr_array  */
  YYSYMBOL_expr_array_list = 259,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 260,               /* expr_alias  */
  YYSYMBOL_expr = 261,                     /* expr  */
  YYSYMBOL_operand = 262,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 263, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 264,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 265,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 266,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 267,          /* match_text_expr  */
  YYSYMBOL_query_expr = 268,               /* query_expr  */
  YYSYMBOL_fusion_expr = 269,              /* fusion_expr  */
  YYSYMBOL_sub_search = 270,               /* sub_search  */
  YYSYMBOL_sub_search_array = 271,         /* sub_search_array  */
  YYSYMBOL_function_expr = 272,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 273,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 274,             /* between_expr  */
  YYSYMBOL_in_expr = 275,                  /* in_expr  */
  YYSYMBOL_case_expr = 276,                /* case_expr  */
  YYSYMBOL_case_check_array = 277,         /* case_check_array  */
  YYSYMBOL_cast_expr = 278,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 279,            /* subquery_expr  */
  YYSYMBOL_column_expr = 280,              /* column_expr  */
  YYSYMBOL_constant_expr = 281,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 282,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 283, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 284,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 285, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 286,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 287,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 288, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 289, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 290, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 291,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 292,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 293,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 294,               /* array_expr  */
  YYSYMBOL_long_array_expr = 295,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 296, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 297,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 298, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 299,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 300,         /* copy_option_list  */
  YYSYMBOL_copy_option = 301,              /* copy_option  */
  YYSYMBOL_file_path = 302,                /* file_path  */
  YYSYMBOL_if_exists = 303,                /* if_exists  */
  YYSYMBOL_if_not_exists = 304,            /* if_not_exists  */
  YYSYMBOL_semicolon = 305,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 306,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 307,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 308, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 309,         /* index_param_list  */
  YYSYMBOL_index_param = 310,              /* index_param  */
  YYSYMBOL_index_info_list = 311,          /* index_info_list  */
  YYSYMBOL_index_info_list_one_pack = 312  /* index_info_list_one_pack  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 441 "parser.cpp"

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
#define YYLAST   1202

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  199
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  444
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  946

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   437


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
       2,     2,     2,     2,     2,     2,     2,   190,     2,     2,
     193,   194,   188,   186,   197,   187,   195,   189,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   198,   196,
     184,   183,   185,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   191,     2,   192,     2,     2,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182
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
     790,   791,   792,   795,   797,   798,   799,   800,   803,   804,
     805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   822,   823,   824,
     825,   826,   827,   828,   829,   830,   831,   832,   833,   834,
     835,   836,   837,   838,   839,   840,   841,   842,   843,   844,
     845,   846,   847,   848,   849,   850,   851,   870,   874,   884,
     887,   890,   893,   897,   900,   905,   910,   917,   923,   933,
     949,   983,   996,   999,  1006,  1012,  1015,  1018,  1021,  1024,
    1027,  1030,  1033,  1040,  1053,  1057,  1062,  1075,  1088,  1103,
    1118,  1133,  1156,  1209,  1264,  1315,  1318,  1321,  1330,  1340,
    1343,  1347,  1352,  1374,  1377,  1382,  1398,  1401,  1405,  1409,
    1414,  1420,  1423,  1426,  1430,  1434,  1436,  1440,  1442,  1445,
    1449,  1452,  1456,  1461,  1465,  1468,  1472,  1475,  1479,  1482,
    1486,  1489,  1492,  1495,  1503,  1506,  1521,  1521,  1523,  1537,
    1546,  1551,  1560,  1565,  1570,  1576,  1583,  1586,  1590,  1593,
    1598,  1610,  1617,  1631,  1634,  1637,  1640,  1643,  1646,  1649,
    1655,  1659,  1663,  1667,  1671,  1678,  1682,  1686,  1690,  1695,
    1699,  1704,  1708,  1712,  1718,  1724,  1730,  1741,  1752,  1763,
    1775,  1787,  1800,  1814,  1825,  1839,  1855,  1872,  1876,  1880,
    1888,  1892,  1896,  1904,  1915,  1938,  1944,  1949,  1955,  1961,
    1969,  1975,  1981,  1987,  1993,  2001,  2007,  2013,  2019,  2025,
    2033,  2039,  2046,  2063,  2067,  2072,  2076,  2103,  2109,  2113,
    2114,  2115,  2116,  2117,  2119,  2122,  2128,  2131,  2132,  2133,
    2134,  2135,  2136,  2137,  2138,  2139,  2140,  2142,  2145,  2151,
    2170,  2212,  2258,  2276,  2294,  2302,  2313,  2319,  2328,  2334,
    2346,  2349,  2352,  2355,  2358,  2361,  2365,  2369,  2374,  2382,
    2390,  2399,  2406,  2413,  2420,  2427,  2434,  2442,  2450,  2458,
    2466,  2474,  2482,  2490,  2498,  2506,  2514,  2522,  2530,  2560,
    2568,  2577,  2585,  2594,  2602,  2608,  2615,  2621,  2628,  2633,
    2640,  2647,  2655,  2679,  2685,  2691,  2698,  2706,  2713,  2720,
    2725,  2735,  2740,  2745,  2750,  2755,  2760,  2765,  2770,  2775,
    2780,  2783,  2786,  2790,  2793,  2796,  2799,  2803,  2806,  2809,
    2813,  2817,  2822,  2827,  2830,  2834,  2838,  2845,  2852,  2856,
    2863,  2870,  2874,  2878,  2882,  2885,  2889,  2893,  2898,  2903,
    2907,  2912,  2917,  2923,  2929,  2935,  2941,  2947,  2953,  2959,
    2965,  2971,  2977,  2983,  2994,  2998,  3003,  3033,  3043,  3048,
    3053,  3058,  3064,  3068,  3069,  3071,  3072,  3074,  3075,  3087,
    3095,  3099,  3102,  3106,  3109,  3113,  3117,  3122,  3128,  3138,
    3145,  3156,  3160,  3168,  3222
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
  "DECIMAL", "DATE", "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED",
  "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON",
  "CIRCLE", "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT",
  "TENSOR", "SPARSE", "TENSORARRAY", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "USING", "SESSION", "GLOBAL", "OFF",
  "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "SEARCH", "MATCH", "MAXSIM",
  "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "NUMBER", "'='", "'<'", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'",
  "';'", "','", "':'", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "table_element", "table_column", "column_type",
  "column_constraints", "column_constraint", "default_expr",
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

#define YYPACT_NINF (-509)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-431)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     661,   233,    43,   337,   107,    17,   107,    84,   348,   294,
      98,   313,   173,   200,   208,    64,   -41,   248,    69,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,   298,  -509,  -509,
     274,  -509,  -509,  -509,  -509,  -509,   198,   198,   198,   198,
      35,   107,   224,   224,   224,   224,   224,    99,   300,   107,
      -1,   318,   354,   361,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,   692,   377,   107,  -509,  -509,  -509,  -509,  -509,   383,
     119,   236,  -509,   390,  -509,   226,  -509,  -509,   239,  -509,
     107,  -509,  -509,  -509,  -509,   -14,  -509,   359,   214,  -509,
     423,   241,   243,  -509,    20,  -509,   421,  -509,  -509,     5,
     380,  -509,   407,   374,   456,   107,   107,   107,   493,   436,
     336,   460,   542,   107,   107,   107,   544,   556,   564,   508,
     574,   574,   440,    51,    56,    63,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,   298,  -509,  -509,  -509,  -509,  -509,  -509,
      61,  -509,  -509,   579,  -509,   585,  -509,  -509,   591,  -509,
     107,   422,   208,   574,  -509,  -509,  -509,  -509,     5,  -509,
    -509,  -509,   440,   559,   547,   543,  -509,   -43,  -509,   336,
    -509,   107,   619,    41,  -509,  -509,  -509,  -509,  -509,   560,
    -509,   446,   -13,  -509,   440,  -509,  -509,   554,   555,   430,
    -509,  -509,   761,   505,   443,   447,   306,   637,   638,   639,
     640,  -509,  -509,   641,   452,   179,   453,   458,   546,   546,
    -509,    13,   399,   -83,  -509,     1,   580,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
     457,  -509,  -509,  -509,  -110,  -509,  -509,   -60,  -509,     2,
    -509,  -509,  -509,    40,  -509,    42,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,   650,   654,  -509,  -509,  -509,  -509,  -509,  -509,
     576,   274,  -509,  -509,   467,   468,    -6,   440,   440,   587,
    -509,   -41,    67,   607,   470,  -509,   -55,   472,  -509,   107,
     440,   564,  -509,   312,   473,   474,   183,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,   546,
     476,   158,   602,   440,   440,   -18,   264,  -509,  -509,  -509,
    -509,   761,  -509,   685,   497,   501,   502,   504,   705,   707,
     316,   316,  -509,   514,  -509,  -509,  -509,  -509,   519,   -63,
     635,   440,   713,   440,   440,   -10,   527,     9,   546,   546,
     546,   546,   546,   546,   546,   546,   546,   546,   546,   546,
     546,   546,    18,  -509,   530,  -509,   717,  -509,   719,  -509,
     732,  -509,   735,   696,   464,   567,  -509,   558,  -509,  -509,
       8,   573,   552,  -509,    -9,   312,   440,  -509,   298,   739,
     644,   581,    46,  -509,  -509,  -509,   -41,   619,   470,  -509,
    -509,   772,   440,   586,  -509,   312,  -509,    15,    15,   440,
    -509,    87,   602,   621,   583,    76,    -5,   271,  -509,   440,
     440,   700,   440,   781,    22,   440,   168,   178,   483,  -509,
    -509,   574,  -509,  -509,  -509,   634,   595,   546,   399,   673,
    -509,   606,   606,   146,   146,   632,   606,   606,   146,   146,
     316,   316,  -509,  -509,  -509,  -509,  -509,  -509,   605,  -509,
     608,  -509,  -509,  -509,   801,   802,  -509,   806,  -509,   -41,
     613,   816,  -509,    62,  -509,   220,   508,   440,  -509,  -509,
    -509,   312,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,   630,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,   631,   653,   660,   663,
     667,   151,   668,   619,   786,    67,   298,   218,  -509,  -509,
     230,   677,   819,   822,   867,   877,  -509,   878,   231,  -509,
     254,   261,  -509,   689,  -509,   739,   440,  -509,   440,   -12,
      92,   546,   -51,   690,  -509,    24,   -48,  -509,   882,  -509,
     884,  -509,  -509,   809,   399,   606,   697,   283,  -509,   546,
     886,   885,   840,   844,   725,   284,  -509,    29,     8,   850,
    -509,  -509,  -509,  -509,  -509,  -509,   851,  -509,   908,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,   715,   859,  -509,
     909,   463,   652,   162,   774,   706,   785,   789,  -509,  -509,
     142,  -509,   787,   619,   293,   724,  -509,  -509,   758,  -509,
     440,  -509,  -509,  -509,  -509,  -509,  -509,    15,  -509,  -509,
    -509,   726,   312,     3,  -509,   440,   416,   730,   918,   530,
     733,   729,   731,   734,   736,   301,  -509,  -509,   158,   920,
     922,   322,  -509,   806,    62,   816,     8,     8,   738,   220,
     874,   876,   320,   737,   740,   741,   742,   743,   744,   745,
     746,   747,   842,   748,   749,   750,   751,   752,   753,   754,
     755,   756,   757,   843,   759,   760,   762,   763,   764,   765,
     766,   767,   768,   769,   770,   771,   773,   775,   776,   777,
     778,   779,   780,   782,   783,   784,   788,   790,   791,   792,
     862,   793,  -509,  -509,    75,  -509,  -509,  -509,   324,  -509,
     806,   930,   325,  -509,  -509,  -509,   312,  -509,   535,   794,
     326,   795,    32,   796,  -509,  -509,  -509,  -509,  -509,    15,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
     894,   619,  -509,   440,   440,  -509,  -509,   963,   965,   972,
     976,   977,   978,   980,   988,   989,   990,   800,   992,   993,
     994,   995,   996,   997,   998,   999,  1000,  1001,   811,  1003,
    1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,  1012,  1013,
    1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,
    1024,  1025,  1026,  1027,  1028,   838,  1030,  -509,  -509,   331,
     576,  -509,  -509,  1033,  -509,  1034,  1035,  1036,   333,   440,
     340,   845,   312,   847,   849,   852,   853,   854,   855,   856,
     857,   858,   860,  1038,   861,   863,   864,   865,   866,   868,
     869,   870,   871,   872,  1039,   873,   875,   879,   880,   881,
     883,   887,   888,   889,   890,   891,   892,   893,   895,   896,
     897,   898,   899,   900,   901,   902,   903,   904,   905,   906,
     907,  1047,   910,  -509,  -509,   911,   912,   913,   341,  -509,
     312,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,   915,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,   917,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
    -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,  -509,
     919,  -509,  1052,  -509,  1057,   576,  1062,  -509,  -509,  -509,
     401,   921,  -509,   923,   576,  1064,  1067,   925,   576,  -509,
     926,  -509,  -509,  -509,   576,  -509
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     207,     0,     0,     0,     0,     0,     0,     0,   142,     0,
       0,     0,     0,     0,     0,     0,   207,     0,   428,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   156,   155,
       0,     8,    14,    15,    16,    17,   426,   426,   426,   426,
     426,     0,   424,   424,   424,   424,   424,   200,     0,     0,
       0,     0,     0,     0,   136,   140,   137,   138,   139,   141,
     135,   207,     0,     0,   221,   222,   220,   226,   229,     0,
       0,     0,   223,     0,   225,     0,   247,   249,     0,   227,
       0,   250,   251,   252,   255,   200,   253,     0,   206,   208,
       0,     0,     0,     1,   207,     2,   190,   192,   193,     0,
     179,   161,   167,     0,     0,     0,     0,     0,     0,     0,
     133,     0,     0,     0,     0,     0,     0,     0,     0,   185,
       0,     0,     0,     0,     0,     0,   134,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   235,
     236,   230,   231,     0,   232,     0,   224,   248,     0,   272,
       0,     0,     0,     0,   160,   159,     4,   191,     0,   157,
     158,   178,     0,     0,   175,     0,    30,     0,    31,   133,
     429,     0,     0,   207,   423,   147,   149,   148,   150,     0,
     201,     0,   185,   144,     0,   129,   422,     0,     0,   357,
     361,   364,   365,     0,     0,     0,     0,     0,     0,     0,
       0,   362,   363,     0,     0,     0,     0,     0,     0,     0,
     359,     0,   207,     0,   273,   278,   279,   293,   291,   294,
     292,   295,   296,   288,   283,   282,   281,   289,   290,   280,
     287,   286,   372,   374,     0,   375,   383,     0,   384,     0,
     376,   373,   394,     0,   395,     0,   371,   259,   261,   260,
     257,   258,   264,   266,   265,   262,   263,   269,   271,   270,
     267,   268,     0,     0,   238,   237,   243,   233,   234,   228,
     432,     0,   209,   256,     0,     0,   181,     0,     0,   177,
     425,   207,     0,     0,     0,   127,     0,     0,   131,     0,
       0,     0,   143,   184,     0,     0,     0,   403,   402,   405,
     404,   407,   406,   409,   408,   411,   410,   413,   412,     0,
       0,   323,   207,     0,     0,     0,     0,   366,   367,   368,
     369,     0,   370,     0,     0,     0,     0,     0,     0,     0,
     325,   324,   400,   397,   391,   381,   386,   389,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   380,     0,   385,     0,   388,     0,   396,
       0,   399,     0,   244,   239,     0,   254,     0,   164,   163,
       0,   183,   166,   168,   173,   174,     0,   162,    33,     0,
       0,     0,     0,    36,    38,    39,   207,     0,    35,   441,
     132,     0,     0,   130,   151,   146,   145,     0,     0,     0,
     318,     0,   207,     0,     0,     0,     0,     0,   348,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   285,
     284,     0,   274,   277,   341,   342,     0,     0,   207,     0,
     322,   332,   333,   336,   337,     0,   339,   331,   334,   335,
     327,   326,   328,   329,   330,   358,   360,   382,     0,   387,
       0,   390,   398,   401,     0,     0,   240,     0,   210,   207,
     180,   194,   196,   205,   197,     0,   185,     0,   171,   172,
     170,   176,    42,    45,    46,    43,    44,    47,    48,    64,
      49,    51,    50,    67,    54,    55,    56,    52,    53,    57,
      58,    59,    60,    61,    62,    63,     0,     0,     0,     0,
       0,   432,     0,     0,   434,     0,    34,     0,   442,   128,
       0,     0,     0,     0,     0,     0,   418,     0,     0,   414,
       0,     0,   319,     0,   353,     0,     0,   346,     0,     0,
       0,     0,     0,     0,   357,     0,     0,   306,     0,   308,
       0,   393,   392,     0,   207,   340,     0,     0,   321,     0,
       0,     0,   245,   241,   437,     0,   435,     0,     0,     0,
     214,   215,   216,   217,   213,   218,     0,   203,     0,   198,
     312,   310,   313,   311,   314,   315,   316,   182,   189,   169,
       0,     0,     0,     0,     0,     0,     0,     0,   120,   121,
     124,   117,   124,     0,     0,     0,    32,    37,   444,   275,
       0,   420,   419,   417,   416,   421,   154,     0,   152,   320,
     354,     0,   350,     0,   349,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   355,   344,   343,     0,
       0,     0,   431,     0,   205,   195,     0,     0,   202,     0,
       0,   187,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   122,   119,     0,   118,    41,    40,     0,   126,
       0,     0,     0,   415,   352,   347,   351,   338,     0,     0,
       0,     0,     0,     0,   377,   379,   378,   307,   309,     0,
     356,   345,   246,   242,   438,   440,   439,   436,   199,   211,
       0,     0,   317,     0,     0,   165,    66,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   123,   125,     0,
     432,   276,   397,     0,   304,     0,     0,     0,     0,     0,
       0,   188,   186,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   433,   443,     0,     0,     0,     0,   153,
     212,   204,    65,    71,    72,    69,    70,    73,    74,    75,
      76,    77,     0,    68,   101,   102,    99,   100,   103,   104,
     105,   106,   107,     0,    98,    82,    83,    80,    81,    84,
      85,    86,    79,   112,   113,   110,   111,   114,   115,   116,
     109,    90,    91,    88,    89,    92,    93,    94,    95,    96,
       0,    87,     0,   305,     0,   432,     0,    78,   108,    97,
       0,   298,   303,     0,   432,     0,     0,     0,   432,   301,
       0,   297,   299,   302,   432,   300
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -509,  -509,  -509,   982,  -509,  1041,  -509,   557,  -509,   545,
    -509,   478,   477,  -509,  -391,  1042,  1044,   938,  -509,  -509,
    1051,  -509,   797,  1053,  1054,   -56,  1100,   -15,   914,   964,
      -7,  -509,  -509,   646,  -509,  -509,  -509,  -509,  -509,  -509,
    -172,  -509,  -509,  -509,  -509,   553,   -35,    11,   480,  -509,
    -509,   973,  -509,  -509,  1065,  1066,  1068,  1069,  1070,  -160,
    -509,   798,  -184,  -186,  -509,  -459,  -449,  -435,  -433,  -431,
    -427,   479,  -509,  -509,  -509,  -509,  -509,  -509,   817,  -509,
    -509,   709,   431,  -207,  -509,  -509,  -509,   506,  -509,  -509,
    -509,  -509,   507,   799,   803,   -34,  -509,  -509,  -509,  -509,
     931,  -400,   521,  -112,   375,   394,  -509,  -509,  -508,  -509,
     432,   498,  -509,   804
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   126,    20,   392,   393,   394,   511,
     600,   601,   706,   395,   286,    21,    22,   173,    23,    61,
      24,   182,   183,    25,    26,    27,    28,    29,   101,   159,
     102,   164,   382,   383,   480,   279,   387,   162,   381,   476,
     185,   745,   651,    99,   470,   471,   472,   473,   579,    30,
      88,    89,   474,   576,    31,    32,    33,    34,    35,   213,
     403,   214,   215,   216,   937,   217,   218,   219,   220,   221,
     222,   586,   587,   223,   224,   225,   226,   227,   316,   228,
     229,   230,   231,   232,   723,   233,   234,   235,   236,   237,
     238,   239,   240,   336,   337,   241,   242,   243,   244,   245,
     246,   528,   529,   187,   112,   104,    95,   109,   376,   606,
     565,   566,   398,   399
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     293,    92,   276,   602,   335,   133,   517,   311,   530,   188,
     292,    47,   315,   100,   281,    48,   580,    50,   332,   333,
    -427,   455,   330,   331,    86,   544,   581,     1,   339,     2,
       3,     4,     5,     6,     7,     8,     9,   332,   333,    14,
     582,   273,   583,    10,   584,    11,    12,    13,   585,    96,
     150,    97,   110,    98,   247,   184,   248,   249,   342,   252,
     119,   253,   254,   478,   479,   577,   257,   436,   258,   259,
     389,   314,   380,   624,   140,   287,    41,   120,   121,   190,
     191,   192,   363,   536,   439,   522,   523,   364,   715,   343,
     344,   149,   160,   384,   385,    49,   524,   525,   526,  -430,
      14,   343,   344,   262,   343,   344,   405,   263,   264,   103,
      47,   340,   265,   266,   341,   250,   167,   168,   169,   578,
     255,    14,   604,   311,   176,   177,   178,   260,    80,   415,
     416,   430,   365,   535,   343,   344,   411,   366,   440,   400,
     343,   344,   401,   343,   344,   437,   627,   343,   344,   630,
     282,   275,    16,   343,   344,   343,   344,   457,   288,   434,
     435,   270,   441,   442,   443,   444,   445,   446,   447,   448,
     449,   450,   451,   452,   453,   454,    84,   197,   198,   199,
     625,   117,   284,   200,   291,    15,   189,   190,   191,   192,
     580,   341,   122,   390,   367,   391,   527,   338,   158,   368,
     581,   469,   481,    85,   211,   334,   456,   201,   202,   203,
     210,    87,   708,    16,   582,   251,   583,   596,   584,   362,
     256,   629,   585,   644,   334,   388,   596,   261,   343,   344,
      90,   375,   369,   413,   371,   539,   540,   370,   542,   372,
     514,   546,   520,   515,   343,   344,    51,    52,    93,   531,
     409,   555,    53,   675,   676,   677,   678,   679,   193,   194,
     680,   681,    36,    37,    38,    94,   211,   195,   597,   196,
     598,   599,   103,   704,    39,    40,   347,   597,   557,   598,
     599,   532,   100,   682,   341,   197,   198,   199,   347,   142,
     143,   200,   864,   384,   117,  -431,  -431,   414,   111,   324,
     404,   325,   326,   327,   588,   348,   349,   350,   351,   189,
     190,   191,   192,   353,   118,   201,   202,   203,    96,   553,
      97,   123,    98,    62,    63,   734,    64,   735,   736,   808,
    -431,  -431,   357,   358,   359,   360,   361,   204,    65,    66,
     516,   354,   355,   356,   357,   358,   359,   360,   361,   418,
     810,   419,   622,   420,   623,   626,   537,   124,   538,   205,
     420,   206,   547,   207,   125,   548,    42,    43,    44,   208,
     209,   210,   549,   638,   211,   550,   212,   410,    45,    46,
     139,   193,   194,    54,    55,    56,    57,    58,    59,   141,
     195,    60,   196,   146,   635,   314,   205,   533,   206,   147,
     207,   148,   189,   190,   191,   192,   144,   145,   197,   198,
     199,   152,   608,   567,   200,   401,   151,   932,   113,   114,
     115,   116,   721,   556,   609,   616,   939,   341,   617,   153,
     943,   105,   106,   107,   108,   154,   945,   155,   201,   202,
     203,   716,   157,   189,   190,   191,   192,   161,   618,   165,
     712,   617,    67,    68,    69,   619,    70,    71,   341,   166,
     204,    72,    73,    74,   343,   344,    75,    76,    77,    81,
      82,    83,    78,    79,   193,   194,   163,   637,   642,    14,
     341,   643,   205,   195,   206,   196,   207,   709,   551,   552,
     401,   413,   208,   209,   210,   731,   170,   211,   341,   212,
     171,   197,   198,   199,   359,   360,   361,   200,   189,   190,
     191,   192,   465,   466,   746,   193,   194,   747,   798,   801,
     804,   401,   341,   805,   195,   863,   196,   869,   643,   172,
     617,   201,   202,   203,   871,   925,   174,   401,   926,   634,
     332,   802,   197,   198,   199,   175,   347,   179,   200,   189,
     190,   191,   192,   204,   653,   654,   655,   656,   657,   180,
     812,   658,   659,   348,   349,   350,   351,   181,   660,   661,
     662,   353,   201,   202,   203,   205,   184,   206,   186,   207,
     309,   310,   267,   811,   663,   208,   209,   210,   268,   195,
     211,   196,   212,   719,   204,   934,   726,   269,   935,   354,
     355,   356,   357,   358,   359,   360,   361,   197,   198,   199,
     717,   739,   740,   200,   277,   271,   205,   278,   206,   280,
     207,   309,   285,   296,   289,   870,   208,   209,   210,   290,
     195,   211,   196,   212,   294,   295,   312,   201,   202,   203,
     313,   317,   318,   319,   320,   323,   328,   321,   197,   198,
     199,   329,   362,   373,   200,   345,   375,   346,   386,   204,
     374,   378,   379,   397,   396,   402,   407,   408,     1,   412,
       2,     3,     4,     5,     6,     7,     8,     9,   201,   202,
     203,   205,    14,   206,    10,   207,    11,    12,    13,   421,
     422,   208,   209,   210,   423,   424,   211,   425,   212,     1,
     204,     2,     3,     4,     5,     6,     7,   413,     9,   426,
     347,   427,   428,   429,   431,    10,   433,    11,    12,    13,
     438,   211,   205,   458,   206,   460,   207,   348,   349,   350,
     351,   352,   208,   209,   210,   353,   347,   211,   462,   212,
     463,    14,   464,   664,   665,   666,   667,   668,   475,   477,
     669,   670,   468,  -431,  -431,   350,   351,   671,   672,   673,
     467,  -431,   347,   354,   355,   356,   357,   358,   359,   360,
     361,   512,    14,   674,   513,   519,   437,   534,   541,   348,
     349,   350,   351,   521,   559,   543,   343,   353,   554,  -431,
     355,   356,   357,   358,   359,   360,   361,   691,   692,   693,
     694,   695,   558,   560,   696,   697,   561,   562,   563,   564,
     568,   698,   699,   700,   605,   354,   355,   356,   357,   358,
     359,   360,   361,   590,   591,   611,    15,   701,   612,   482,
     483,   484,   485,   486,   487,   488,   489,   490,   491,   492,
     493,   494,   495,   496,   497,   498,   592,   499,   500,   501,
     502,   503,   504,   593,    16,   505,   594,    15,   506,   507,
     595,   603,   508,   509,   510,   683,   684,   685,   686,   687,
     610,   613,   688,   689,   569,  -219,   570,   571,   572,   573,
     614,   574,   575,   620,   615,    16,   631,   628,   632,   633,
     551,   636,   552,   639,   640,   690,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   641,   646,
     647,   648,   649,   650,   702,   652,   703,   710,   704,   711,
     714,   718,   720,   727,   722,   728,   732,   729,   733,   743,
     730,   741,   744,   800,   748,   757,   768,   749,   750,   751,
     752,   753,   754,   755,   756,   758,   759,   760,   761,   762,
     763,   764,   765,   766,   767,   795,   769,   770,   809,   771,
     772,   773,   774,   775,   776,   777,   778,   779,   780,   813,
     781,   814,   782,   783,   784,   785,   786,   787,   815,   788,
     789,   790,   816,   817,   818,   791,   819,   792,   793,   794,
     796,   803,   806,   807,   820,   821,   822,   823,   824,   825,
     826,   827,   828,   829,   830,   831,   832,   833,   834,   835,
     836,   837,   838,   839,   840,   841,   842,   843,   844,   845,
     846,   847,   848,   849,   850,   851,   852,   853,   854,   855,
     856,   857,   858,   859,   860,   861,   862,   865,   866,   867,
     868,   872,   341,   873,   882,   893,   874,   875,   876,   877,
     878,   879,   880,   920,   881,   883,   930,   884,   885,   886,
     887,   931,   888,   889,   890,   891,   892,   894,   933,   895,
     940,   941,   607,   896,   897,   898,   156,   899,   705,   707,
     621,   900,   901,   902,   903,   904,   905,   906,   406,   907,
     908,   909,   910,   911,   912,   913,   914,   915,   916,   917,
     918,   919,   127,   128,   921,   129,   923,   283,   922,   927,
     924,   928,   130,   929,   131,   132,    91,   938,   936,   942,
     944,   645,   274,   589,   738,   272,   134,   135,   742,   136,
     137,   138,   417,   545,   322,   797,   724,   725,   713,   432,
       0,   737,   799,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   459,     0,     0,     0,     0,
       0,   461,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   377,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   518
};

static const yytype_int16 yycheck[] =
{
     184,    16,   162,   511,   211,    61,   397,   193,   408,   121,
     182,     3,   196,     8,    57,     4,   475,     6,     5,     6,
       0,     3,   208,   209,    13,     3,   475,     7,   212,     9,
      10,    11,    12,    13,    14,    15,    16,     5,     6,    80,
     475,   153,   475,    23,   475,    25,    26,    27,   475,    20,
      64,    22,    41,    24,     3,    68,     5,     6,    57,     3,
      49,     5,     6,    72,    73,     3,     3,    77,     5,     6,
       3,    89,    78,    85,    63,    34,    33,    78,    79,     4,
       5,     6,   192,    88,    75,    70,    71,   197,    85,   152,
     153,    80,    99,   277,   278,    78,    81,    82,    83,    64,
      80,   152,   153,    42,   152,   153,   290,    46,    47,    74,
       3,   194,    51,    52,   197,    64,   105,   106,   107,    57,
      64,    80,   513,   309,   113,   114,   115,    64,    30,   313,
     314,   194,   192,    57,   152,   153,   296,   197,   129,   194,
     152,   153,   197,   152,   153,   155,   197,   152,   153,   197,
     193,   158,   193,   152,   153,   152,   153,   364,   173,   343,
     344,   150,   348,   349,   350,   351,   352,   353,   354,   355,
     356,   357,   358,   359,   360,   361,     3,   102,   103,   104,
      88,   195,   171,   108,   197,   165,     3,     4,     5,     6,
     649,   197,   193,   126,   192,   128,   181,   212,   193,   197,
     649,   193,   386,     3,   191,   192,   188,   132,   133,   134,
     188,     3,   603,   193,   649,   164,   649,    75,   649,   195,
     164,   197,   649,   194,   192,   281,    75,   164,   152,   153,
     166,    80,   192,    75,   192,   419,   420,   197,   422,   197,
     194,   425,   402,   197,   152,   153,   162,   163,     0,   409,
      67,   437,   168,    91,    92,    93,    94,    95,    75,    76,
      98,    99,    29,    30,    31,   196,   191,    84,   126,    86,
     128,   129,    74,   131,    41,    42,   130,   126,   438,   128,
     129,   194,     8,   121,   197,   102,   103,   104,   130,   170,
     171,   108,   800,   477,   195,   149,   150,   312,    74,   120,
     289,   122,   123,   124,   476,   147,   148,   149,   150,     3,
       4,     5,     6,   155,    14,   132,   133,   134,    20,   431,
      22,     3,    24,    29,    30,     3,    32,     5,     6,   729,
     184,   185,   186,   187,   188,   189,   190,   154,    44,    45,
     396,   183,   184,   185,   186,   187,   188,   189,   190,    85,
     741,    87,   536,    89,   538,   541,    85,     3,    87,   176,
      89,   178,   194,   180,     3,   197,    29,    30,    31,   186,
     187,   188,   194,   559,   191,   197,   193,   194,    41,    42,
       3,    75,    76,    35,    36,    37,    38,    39,    40,     6,
      84,    43,    86,     3,   554,    89,   176,   412,   178,   173,
     180,   162,     3,     4,     5,     6,   170,   171,   102,   103,
     104,   197,   194,   469,   108,   197,    57,   925,    43,    44,
      45,    46,   629,   438,   194,   194,   934,   197,   197,     6,
     938,    37,    38,    39,    40,   194,   944,   194,   132,   133,
     134,   625,    21,     3,     4,     5,     6,    67,   194,    75,
     610,   197,   158,   159,   160,   194,   162,   163,   197,     3,
     154,   167,   168,   169,   152,   153,   172,   173,   174,   156,
     157,   158,   178,   179,    75,    76,    69,   194,   194,    80,
     197,   197,   176,    84,   178,    86,   180,   194,     5,     6,
     197,    75,   186,   187,   188,   194,     3,   191,   197,   193,
      64,   102,   103,   104,   188,   189,   190,   108,     3,     4,
       5,     6,    48,    49,   194,    75,    76,   197,   194,   194,
     194,   197,   197,   197,    84,   194,    86,   194,   197,   193,
     197,   132,   133,   134,   194,   194,    76,   197,   197,   554,
       5,     6,   102,   103,   104,     3,   130,     3,   108,     3,
       4,     5,     6,   154,    91,    92,    93,    94,    95,     3,
     744,    98,    99,   147,   148,   149,   150,     3,   105,   106,
     107,   155,   132,   133,   134,   176,    68,   178,     4,   180,
      75,    76,     3,   743,   121,   186,   187,   188,     3,    84,
     191,    86,   193,   627,   154,   194,   630,     6,   197,   183,
     184,   185,   186,   187,   188,   189,   190,   102,   103,   104,
     194,   646,   647,   108,    55,   193,   176,    70,   178,    76,
     180,    75,     3,   193,    64,   809,   186,   187,   188,   183,
      84,   191,    86,   193,    80,    80,   193,   132,   133,   134,
     193,     4,     4,     4,     4,   193,   193,     6,   102,   103,
     104,   193,   195,     3,   108,    75,    80,    77,    71,   154,
       6,   194,   194,   193,    57,   193,   193,   193,     7,   193,
       9,    10,    11,    12,    13,    14,    15,    16,   132,   133,
     134,   176,    80,   178,    23,   180,    25,    26,    27,     4,
     193,   186,   187,   188,   193,   193,   191,   193,   193,     7,
     154,     9,    10,    11,    12,    13,    14,    75,    16,     4,
     130,     4,   198,   194,    79,    23,     3,    25,    26,    27,
     193,   191,   176,     6,   178,     6,   180,   147,   148,   149,
     150,   151,   186,   187,   188,   155,   130,   191,     6,   193,
       5,    80,    46,    91,    92,    93,    94,    95,   175,   197,
      98,    99,   194,   147,   148,   149,   150,   105,   106,   107,
     193,   155,   130,   183,   184,   185,   186,   187,   188,   189,
     190,   127,    80,   121,   193,     3,   155,   194,    78,   147,
     148,   149,   150,   197,   152,     4,   152,   155,   193,   183,
     184,   185,   186,   187,   188,   189,   190,    91,    92,    93,
      94,    95,   129,   198,    98,    99,   198,     6,     6,     3,
     197,   105,   106,   107,    28,   183,   184,   185,   186,   187,
     188,   189,   190,   193,   193,     6,   165,   121,     6,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   193,   108,   109,   110,
     111,   112,   113,   193,   193,   116,   193,   165,   119,   120,
     193,   193,   123,   124,   125,    91,    92,    93,    94,    95,
     193,     4,    98,    99,    58,    59,    60,    61,    62,    63,
       3,    65,    66,   194,     6,   193,     4,   197,     4,    80,
       5,   194,     6,    53,    50,   121,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   183,    59,
      59,     3,   197,    54,   129,     6,   127,   193,   131,   161,
     194,   191,     4,   194,   191,   194,     6,   193,     6,    55,
     194,   193,    56,     3,   197,    93,    93,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,    93,   197,   197,    64,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,     6,
     197,     6,   197,   197,   197,   197,   197,   197,     6,   197,
     197,   197,     6,     6,     6,   197,     6,   197,   197,   197,
     197,   197,   197,   197,     6,     6,     6,   197,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   197,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   197,     6,     4,     4,     4,
       4,   194,   197,   194,     6,     6,   194,   194,   194,   194,
     194,   194,   194,     6,   194,   194,     4,   194,   194,   194,
     194,     4,   194,   194,   194,   194,   194,   194,     6,   194,
       6,     4,   515,   194,   194,   194,    94,   194,   600,   602,
     535,   194,   194,   194,   194,   194,   194,   194,   291,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,    61,    61,   194,    61,   194,   169,   197,   194,
     197,   194,    61,   194,    61,    61,    16,   194,   197,   194,
     194,   568,   158,   477,   644,   152,    61,    61,   649,    61,
      61,    61,   315,   424,   203,   704,   630,   630,   617,   341,
      -1,   643,   710,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   366,    -1,    -1,    -1,    -1,
      -1,   368,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   271,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   398
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   165,   193,   200,   201,   202,
     204,   214,   215,   217,   219,   222,   223,   224,   225,   226,
     248,   253,   254,   255,   256,   257,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   246,    78,
     246,   162,   163,   168,    35,    36,    37,    38,    39,    40,
      43,   218,    29,    30,    32,    44,    45,   158,   159,   160,
     162,   163,   167,   168,   169,   172,   173,   174,   178,   179,
      30,   156,   157,   158,     3,     3,   246,     3,   249,   250,
     166,   225,   226,     0,   196,   305,    20,    22,    24,   242,
       8,   227,   229,    74,   304,   304,   304,   304,   304,   306,
     246,    74,   303,   303,   303,   303,   303,   195,    14,   246,
      78,    79,   193,     3,     3,     3,   203,   204,   214,   215,
     219,   222,   223,   224,   253,   254,   255,   256,   257,     3,
     246,     6,   170,   171,   170,   171,     3,   173,   162,   246,
      64,    57,   197,     6,   194,   194,   202,    21,   193,   228,
     229,    67,   236,    69,   230,    75,     3,   246,   246,   246,
       3,    64,   193,   216,    76,     3,   246,   246,   246,     3,
       3,     3,   220,   221,    68,   239,     4,   302,   302,     3,
       4,     5,     6,    75,    76,    84,    86,   102,   103,   104,
     108,   132,   133,   134,   154,   176,   178,   180,   186,   187,
     188,   191,   193,   258,   260,   261,   262,   264,   265,   266,
     267,   268,   269,   272,   273,   274,   275,   276,   278,   279,
     280,   281,   282,   284,   285,   286,   287,   288,   289,   290,
     291,   294,   295,   296,   297,   298,   299,     3,     5,     6,
      64,   164,     3,     5,     6,    64,   164,     3,     5,     6,
      64,   164,    42,    46,    47,    51,    52,     3,     3,     6,
     246,   193,   250,   302,   228,   229,   258,    55,    70,   234,
      76,    57,   193,   216,   246,     3,   213,    34,   226,    64,
     183,   197,   239,   261,    80,    80,   193,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    75,
      76,   262,   193,   193,    89,   261,   277,     4,     4,     4,
       4,     6,   299,   193,   120,   122,   123,   124,   193,   193,
     262,   262,     5,     6,   192,   282,   292,   293,   226,   261,
     194,   197,    57,   152,   153,    75,    77,   130,   147,   148,
     149,   150,   151,   155,   183,   184,   185,   186,   187,   188,
     189,   190,   195,   192,   197,   192,   197,   192,   197,   192,
     197,   192,   197,     3,     6,    80,   307,   227,   194,   194,
      78,   237,   231,   232,   261,   261,    71,   235,   224,     3,
     126,   128,   205,   206,   207,   212,    57,   193,   311,   312,
     194,   197,   193,   259,   246,   261,   221,   193,   193,    67,
     194,   258,   193,    75,   226,   261,   261,   277,    85,    87,
      89,     4,   193,   193,   193,   193,     4,     4,   198,   194,
     194,    79,   260,     3,   261,   261,    77,   155,   193,    75,
     129,   262,   262,   262,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,     3,   188,   282,     6,   292,
       6,   293,     6,     5,    46,    48,    49,   193,   194,   193,
     243,   244,   245,   246,   251,   175,   238,   197,    72,    73,
     233,   261,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   108,
     109,   110,   111,   112,   113,   116,   119,   120,   123,   124,
     125,   208,   127,   193,   194,   197,   224,   213,   312,     3,
     258,   197,    70,    71,    81,    82,    83,   181,   300,   301,
     300,   258,   194,   226,   194,    57,    88,    85,    87,   261,
     261,    78,   261,     4,     3,   280,   261,   194,   197,   194,
     197,     5,     6,   302,   193,   262,   226,   258,   129,   152,
     198,   198,     6,     6,     3,   309,   310,   224,   197,    58,
      60,    61,    62,    63,    65,    66,   252,     3,    57,   247,
     264,   265,   266,   267,   268,   269,   270,   271,   239,   232,
     193,   193,   193,   193,   193,   193,    75,   126,   128,   129,
     209,   210,   307,   193,   213,    28,   308,   206,   194,   194,
     193,     6,     6,     4,     3,     6,   194,   197,   194,   194,
     194,   208,   261,   261,    85,    88,   262,   197,   197,   197,
     197,     4,     4,    80,   226,   258,   194,   194,   262,    53,
      50,   183,   194,   197,   194,   244,    59,    59,     3,   197,
      54,   241,     6,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   121,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   121,    91,    92,    93,    94,    95,
      98,    99,   121,    91,    92,    93,    94,    95,    98,    99,
     121,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   121,   129,   127,   131,   210,   211,   211,   213,   194,
     193,   161,   258,   301,   194,    85,   261,   194,   191,   294,
       4,   282,   191,   283,   286,   291,   294,   194,   194,   193,
     194,   194,     6,     6,     3,     5,     6,   310,   247,   245,
     245,   193,   270,    55,    56,   240,   194,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,    93,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,    93,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,    93,   197,   281,   194,   309,
       3,   194,     6,   197,   194,   197,   197,   197,   300,    64,
     213,   258,   261,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   197,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   197,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   197,     6,   194,   307,     4,     4,     4,     4,   194,
     261,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,     6,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,     6,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
       6,   194,   197,   194,   197,   194,   197,   194,   194,   194,
       4,     4,   307,     6,   194,   197,   197,   263,   194,   307,
       6,     4,   194,   307,   194,   307
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   199,   200,   201,   201,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     204,   204,   204,   204,   204,   204,   205,   205,   206,   206,
     207,   207,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   209,   209,   210,
     210,   210,   210,   211,   211,   212,   212,   213,   213,   214,
     215,   215,   216,   216,   217,   218,   218,   218,   218,   218,
     218,   218,   218,   219,   220,   220,   221,   222,   222,   222,
     222,   222,   223,   223,   223,   224,   224,   224,   224,   225,
     225,   226,   227,   228,   228,   229,   230,   230,   231,   231,
     232,   233,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   237,   238,   238,   239,   239,   240,   240,   241,   241,
     242,   242,   242,   242,   243,   243,   244,   244,   245,   245,
     246,   246,   247,   247,   247,   247,   248,   248,   249,   249,
     250,   251,   251,   252,   252,   252,   252,   252,   252,   252,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   253,   253,   253,   253,   253,   253,
     254,   254,   254,   255,   255,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   257,   258,   258,   259,   259,   260,   260,   261,
     261,   261,   261,   261,   262,   262,   262,   262,   262,   262,
     262,   262,   262,   262,   262,   262,   262,   263,   263,   264,
     265,   265,   266,   266,   267,   267,   268,   268,   269,   269,
     270,   270,   270,   270,   270,   270,   271,   271,   272,   272,
     272,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   273,   273,   274,   275,   275,   276,   276,   276,   276,
     277,   277,   278,   279,   279,   279,   279,   280,   280,   280,
     280,   281,   281,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   281,   282,   282,   282,   282,   283,   283,   283,
     284,   285,   285,   286,   286,   287,   288,   288,   289,   290,
     290,   291,   292,   293,   294,   294,   295,   296,   296,   297,
     298,   298,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   300,   300,   301,   301,   301,   301,
     301,   301,   302,   303,   303,   304,   304,   305,   305,   306,
     306,   307,   307,   308,   308,   309,   309,   310,   310,   310,
     310,   311,   311,   312,   312
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
       1,     1,     1,     1,     1,     6,     4,     1,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     1,     2,     2,
       1,     1,     2,     2,     0,     5,     4,     1,     3,     4,
       6,     5,     3,     0,     3,     1,     1,     1,     1,     1,
       1,     1,     0,     5,     1,     3,     3,     4,     4,     4,
       4,     6,     8,    11,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,     8,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     3,     2,     2,     2,     4,     2,
       3,     3,     3,     4,     4,     3,     3,     4,     4,     5,
       6,     7,     9,     4,     5,     7,     9,     2,     3,     2,
       2,     2,     2,     2,     5,     2,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     3,     1,     3,     3,     5,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,    12,
      14,    12,    12,    10,     7,     9,     4,     6,     4,     6,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     4,
       5,     4,     3,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       4,     3,     3,     5,     5,     6,     4,     6,     3,     5,
       4,     5,     6,     4,     5,     5,     6,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     3,     3,     1,     1,     2,     2,     3,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     2,
       2,     2,     1,     2,     0,     3,     0,     1,     0,     2,
       0,     4,     0,     4,     0,     1,     3,     1,     3,     3,
       3,     1,     2,     6,     3
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
#line 2241 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2249 "parser.cpp"
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
#line 2263 "parser.cpp"
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
#line 2277 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2288 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2296 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2305 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2314 "parser.cpp"
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
#line 2328 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2339 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2349 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2359 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2369 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2379 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2389 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 349 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2399 "parser.cpp"
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
#line 2413 "parser.cpp"
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
#line 2427 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2437 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2445 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2453 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2462 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2478 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2486 "parser.cpp"
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
#line 2500 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2509 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2518 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2527 "parser.cpp"
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
#line 2540 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 327 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2549 "parser.cpp"
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
#line 2563 "parser.cpp"
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
#line 2577 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2587 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2596 "parser.cpp"
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
#line 2610 "parser.cpp"
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
#line 2627 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2635 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2643 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2651 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2659 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2667 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2675 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2683 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2691 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2699 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2715 "parser.cpp"
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
#line 2729 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2737 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2745 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2753 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2761 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2769 "parser.cpp"
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
#line 2782 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2790 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2798 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 319 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2814 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2830 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2838 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2846 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2854 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2862 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2870 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2878 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2886 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2894 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 364 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2902 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 368 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2910 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2918 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2926 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2934 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2942 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2950 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 323 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2958 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 310 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2966 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2977 "parser.cpp"
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
#line 2991 "parser.cpp"
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
#line 3005 "parser.cpp"
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
#line 3019 "parser.cpp"
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
#line 3033 "parser.cpp"
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

#line 3141 "parser.cpp"

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
#line 3356 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 496 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3367 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 502 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3378 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 509 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3384 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3390 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 511 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3396 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3402 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3408 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3414 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3420 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3426 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3432 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 518 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3438 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 519 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3444 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3450 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3456 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 523 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3462 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3468 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3474 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3480 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3486 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3492 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3498 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3504 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 531 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3510 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 532 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3516 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 533 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3522 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3528 "parser.cpp"
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
#line 3548 "parser.cpp"
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
#line 3566 "parser.cpp"
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
#line 3599 "parser.cpp"
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
#line 3619 "parser.cpp"
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
#line 3640 "parser.cpp"
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
#line 3673 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 666 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3682 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 670 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3691 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 676 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3699 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 679 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3707 "parser.cpp"
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
#line 3762 "parser.cpp"
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
#line 3801 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 772 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3807 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 773 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3813 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 774 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3819 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 775 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3825 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 776 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3831 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 777 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3837 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3843 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3849 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 780 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3855 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 781 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3861 "parser.cpp"
    break;

  case 52: /* column_type: FLOAT16  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3867 "parser.cpp"
    break;

  case 53: /* column_type: BFLOAT16  */
#line 783 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3873 "parser.cpp"
    break;

  case 54: /* column_type: DATE  */
#line 784 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3879 "parser.cpp"
    break;

  case 55: /* column_type: TIME  */
#line 785 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3885 "parser.cpp"
    break;

  case 56: /* column_type: DATETIME  */
#line 786 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3891 "parser.cpp"
    break;

  case 57: /* column_type: TIMESTAMP  */
#line 787 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3897 "parser.cpp"
    break;

  case 58: /* column_type: UUID  */
#line 788 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3903 "parser.cpp"
    break;

  case 59: /* column_type: POINT  */
#line 789 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3909 "parser.cpp"
    break;

  case 60: /* column_type: LINE  */
#line 790 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3915 "parser.cpp"
    break;

  case 61: /* column_type: LSEG  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3921 "parser.cpp"
    break;

  case 62: /* column_type: BOX  */
#line 792 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3927 "parser.cpp"
    break;

  case 63: /* column_type: CIRCLE  */
#line 795 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3933 "parser.cpp"
    break;

  case 64: /* column_type: VARCHAR  */
#line 797 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3939 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 798 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3945 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 799 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3951 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL  */
#line 800 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3957 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3963 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3969 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3975 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3981 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3987 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3993 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3999 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4005 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4011 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4017 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4023 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4029 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4035 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4041 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4047 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4053 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4059 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4065 "parser.cpp"
    break;

  case 86: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4071 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4077 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4083 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4089 "parser.cpp"
    break;

  case 90: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4095 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4101 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4107 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4113 "parser.cpp"
    break;

  case 94: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4119 "parser.cpp"
    break;

  case 95: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4125 "parser.cpp"
    break;

  case 96: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4131 "parser.cpp"
    break;

  case 97: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4137 "parser.cpp"
    break;

  case 98: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4143 "parser.cpp"
    break;

  case 99: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4149 "parser.cpp"
    break;

  case 100: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4155 "parser.cpp"
    break;

  case 101: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4161 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4167 "parser.cpp"
    break;

  case 103: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4173 "parser.cpp"
    break;

  case 104: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4179 "parser.cpp"
    break;

  case 105: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4185 "parser.cpp"
    break;

  case 106: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4191 "parser.cpp"
    break;

  case 107: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4197 "parser.cpp"
    break;

  case 108: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4203 "parser.cpp"
    break;

  case 109: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4209 "parser.cpp"
    break;

  case 110: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4215 "parser.cpp"
    break;

  case 111: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4221 "parser.cpp"
    break;

  case 112: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4227 "parser.cpp"
    break;

  case 113: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4233 "parser.cpp"
    break;

  case 114: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4239 "parser.cpp"
    break;

  case 115: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4245 "parser.cpp"
    break;

  case 116: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4251 "parser.cpp"
    break;

  case 117: /* column_constraints: column_constraint  */
#line 870 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4260 "parser.cpp"
    break;

  case 118: /* column_constraints: column_constraints column_constraint  */
#line 874 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4274 "parser.cpp"
    break;

  case 119: /* column_constraint: PRIMARY KEY  */
#line 884 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4282 "parser.cpp"
    break;

  case 120: /* column_constraint: UNIQUE  */
#line 887 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4290 "parser.cpp"
    break;

  case 121: /* column_constraint: NULLABLE  */
#line 890 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4298 "parser.cpp"
    break;

  case 122: /* column_constraint: NOT NULLABLE  */
#line 893 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4306 "parser.cpp"
    break;

  case 123: /* default_expr: DEFAULT constant_expr  */
#line 897 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4314 "parser.cpp"
    break;

  case 124: /* default_expr: %empty  */
#line 900 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4322 "parser.cpp"
    break;

  case 125: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 905 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4332 "parser.cpp"
    break;

  case 126: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 910 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4342 "parser.cpp"
    break;

  case 127: /* identifier_array: IDENTIFIER  */
#line 917 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4353 "parser.cpp"
    break;

  case 128: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 923 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4364 "parser.cpp"
    break;

  case 129: /* delete_statement: DELETE FROM table_name where_clause  */
#line 933 "parser.y"
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
#line 4381 "parser.cpp"
    break;

  case 130: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 949 "parser.y"
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
#line 4420 "parser.cpp"
    break;

  case 131: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 983 "parser.y"
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
#line 4437 "parser.cpp"
    break;

  case 132: /* optional_identifier_array: '(' identifier_array ')'  */
#line 996 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4445 "parser.cpp"
    break;

  case 133: /* optional_identifier_array: %empty  */
#line 999 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4453 "parser.cpp"
    break;

  case 134: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1006 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4463 "parser.cpp"
    break;

  case 135: /* explain_type: ANALYZE  */
#line 1012 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4471 "parser.cpp"
    break;

  case 136: /* explain_type: AST  */
#line 1015 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4479 "parser.cpp"
    break;

  case 137: /* explain_type: RAW  */
#line 1018 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4487 "parser.cpp"
    break;

  case 138: /* explain_type: LOGICAL  */
#line 1021 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4495 "parser.cpp"
    break;

  case 139: /* explain_type: PHYSICAL  */
#line 1024 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4503 "parser.cpp"
    break;

  case 140: /* explain_type: PIPELINE  */
#line 1027 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4511 "parser.cpp"
    break;

  case 141: /* explain_type: FRAGMENT  */
#line 1030 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4519 "parser.cpp"
    break;

  case 142: /* explain_type: %empty  */
#line 1033 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4527 "parser.cpp"
    break;

  case 143: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1040 "parser.y"
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
#line 4544 "parser.cpp"
    break;

  case 144: /* update_expr_array: update_expr  */
#line 1053 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4553 "parser.cpp"
    break;

  case 145: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1057 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4562 "parser.cpp"
    break;

  case 146: /* update_expr: IDENTIFIER '=' expr  */
#line 1062 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4574 "parser.cpp"
    break;

  case 147: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1075 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4590 "parser.cpp"
    break;

  case 148: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1088 "parser.y"
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
#line 4608 "parser.cpp"
    break;

  case 149: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1103 "parser.y"
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
#line 4626 "parser.cpp"
    break;

  case 150: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1118 "parser.y"
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
#line 4644 "parser.cpp"
    break;

  case 151: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1133 "parser.y"
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
#line 4667 "parser.cpp"
    break;

  case 152: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1156 "parser.y"
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
#line 4725 "parser.cpp"
    break;

  case 153: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1209 "parser.y"
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
#line 4785 "parser.cpp"
    break;

  case 154: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1264 "parser.y"
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
#line 4837 "parser.cpp"
    break;

  case 155: /* select_statement: select_without_paren  */
#line 1315 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4845 "parser.cpp"
    break;

  case 156: /* select_statement: select_with_paren  */
#line 1318 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4853 "parser.cpp"
    break;

  case 157: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1321 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4867 "parser.cpp"
    break;

  case 158: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1330 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4881 "parser.cpp"
    break;

  case 159: /* select_with_paren: '(' select_without_paren ')'  */
#line 1340 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4889 "parser.cpp"
    break;

  case 160: /* select_with_paren: '(' select_with_paren ')'  */
#line 1343 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4897 "parser.cpp"
    break;

  case 161: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1347 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4906 "parser.cpp"
    break;

  case 162: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1352 "parser.y"
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
#line 4932 "parser.cpp"
    break;

  case 163: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1374 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4940 "parser.cpp"
    break;

  case 164: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1377 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4948 "parser.cpp"
    break;

  case 165: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1382 "parser.y"
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
#line 4968 "parser.cpp"
    break;

  case 166: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1398 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4976 "parser.cpp"
    break;

  case 167: /* order_by_clause: %empty  */
#line 1401 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4984 "parser.cpp"
    break;

  case 168: /* order_by_expr_list: order_by_expr  */
#line 1405 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4993 "parser.cpp"
    break;

  case 169: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1409 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5002 "parser.cpp"
    break;

  case 170: /* order_by_expr: expr order_by_type  */
#line 1414 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5012 "parser.cpp"
    break;

  case 171: /* order_by_type: ASC  */
#line 1420 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5020 "parser.cpp"
    break;

  case 172: /* order_by_type: DESC  */
#line 1423 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5028 "parser.cpp"
    break;

  case 173: /* order_by_type: %empty  */
#line 1426 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5036 "parser.cpp"
    break;

  case 174: /* limit_expr: LIMIT expr  */
#line 1430 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5044 "parser.cpp"
    break;

  case 175: /* limit_expr: %empty  */
#line 1434 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5050 "parser.cpp"
    break;

  case 176: /* offset_expr: OFFSET expr  */
#line 1436 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5058 "parser.cpp"
    break;

  case 177: /* offset_expr: %empty  */
#line 1440 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5064 "parser.cpp"
    break;

  case 178: /* distinct: DISTINCT  */
#line 1442 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5072 "parser.cpp"
    break;

  case 179: /* distinct: %empty  */
#line 1445 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5080 "parser.cpp"
    break;

  case 180: /* from_clause: FROM table_reference  */
#line 1449 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5088 "parser.cpp"
    break;

  case 181: /* from_clause: %empty  */
#line 1452 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5096 "parser.cpp"
    break;

  case 182: /* search_clause: SEARCH sub_search_array  */
#line 1456 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5106 "parser.cpp"
    break;

  case 183: /* search_clause: %empty  */
#line 1461 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5114 "parser.cpp"
    break;

  case 184: /* where_clause: WHERE expr  */
#line 1465 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5122 "parser.cpp"
    break;

  case 185: /* where_clause: %empty  */
#line 1468 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5130 "parser.cpp"
    break;

  case 186: /* having_clause: HAVING expr  */
#line 1472 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5138 "parser.cpp"
    break;

  case 187: /* having_clause: %empty  */
#line 1475 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5146 "parser.cpp"
    break;

  case 188: /* group_by_clause: GROUP BY expr_array  */
#line 1479 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5154 "parser.cpp"
    break;

  case 189: /* group_by_clause: %empty  */
#line 1482 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5162 "parser.cpp"
    break;

  case 190: /* set_operator: UNION  */
#line 1486 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5170 "parser.cpp"
    break;

  case 191: /* set_operator: UNION ALL  */
#line 1489 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5178 "parser.cpp"
    break;

  case 192: /* set_operator: INTERSECT  */
#line 1492 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5186 "parser.cpp"
    break;

  case 193: /* set_operator: EXCEPT  */
#line 1495 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5194 "parser.cpp"
    break;

  case 194: /* table_reference: table_reference_unit  */
#line 1503 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5202 "parser.cpp"
    break;

  case 195: /* table_reference: table_reference ',' table_reference_unit  */
#line 1506 "parser.y"
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
#line 5220 "parser.cpp"
    break;

  case 198: /* table_reference_name: table_name table_alias  */
#line 1523 "parser.y"
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
#line 5238 "parser.cpp"
    break;

  case 199: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1537 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5249 "parser.cpp"
    break;

  case 200: /* table_name: IDENTIFIER  */
#line 1546 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5259 "parser.cpp"
    break;

  case 201: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1551 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5271 "parser.cpp"
    break;

  case 202: /* table_alias: AS IDENTIFIER  */
#line 1560 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5281 "parser.cpp"
    break;

  case 203: /* table_alias: IDENTIFIER  */
#line 1565 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5291 "parser.cpp"
    break;

  case 204: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1570 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5302 "parser.cpp"
    break;

  case 205: /* table_alias: %empty  */
#line 1576 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5310 "parser.cpp"
    break;

  case 206: /* with_clause: WITH with_expr_list  */
#line 1583 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5318 "parser.cpp"
    break;

  case 207: /* with_clause: %empty  */
#line 1586 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5326 "parser.cpp"
    break;

  case 208: /* with_expr_list: with_expr  */
#line 1590 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5335 "parser.cpp"
    break;

  case 209: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1593 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5344 "parser.cpp"
    break;

  case 210: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1598 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5356 "parser.cpp"
    break;

  case 211: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1610 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5368 "parser.cpp"
    break;

  case 212: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1617 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5381 "parser.cpp"
    break;

  case 213: /* join_type: INNER  */
#line 1631 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5389 "parser.cpp"
    break;

  case 214: /* join_type: LEFT  */
#line 1634 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5397 "parser.cpp"
    break;

  case 215: /* join_type: RIGHT  */
#line 1637 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5405 "parser.cpp"
    break;

  case 216: /* join_type: OUTER  */
#line 1640 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5413 "parser.cpp"
    break;

  case 217: /* join_type: FULL  */
#line 1643 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5421 "parser.cpp"
    break;

  case 218: /* join_type: CROSS  */
#line 1646 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5429 "parser.cpp"
    break;

  case 219: /* join_type: %empty  */
#line 1649 "parser.y"
                {
}
#line 5436 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW DATABASES  */
#line 1655 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5445 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW TABLES  */
#line 1659 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5454 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW VIEWS  */
#line 1663 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5463 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW CONFIGS  */
#line 1667 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5472 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1671 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5484 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW PROFILES  */
#line 1678 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5493 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW BUFFER  */
#line 1682 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5502 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW QUERIES  */
#line 1686 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5511 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1690 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5521 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW TRANSACTIONS  */
#line 1695 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5530 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1699 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5540 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW SESSION VARIABLES  */
#line 1704 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5549 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1708 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5558 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1712 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5569 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1718 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5580 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1724 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5591 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW TABLE table_name  */
#line 1730 "parser.y"
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
#line 5607 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1741 "parser.y"
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
#line 5623 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1752 "parser.y"
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
#line 5639 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1763 "parser.y"
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
#line 5656 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1775 "parser.y"
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
#line 5673 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1787 "parser.y"
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
#line 5691 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1800 "parser.y"
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
#line 5710 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1814 "parser.y"
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
#line 5726 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1825 "parser.y"
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
#line 5745 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1839 "parser.y"
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
#line 5766 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1855 "parser.y"
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
#line 5788 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW LOGS  */
#line 1872 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5797 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW DELTA LOGS  */
#line 1876 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5806 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW CATALOGS  */
#line 1880 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5815 "parser.cpp"
    break;

  case 250: /* flush_statement: FLUSH DATA  */
#line 1888 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5824 "parser.cpp"
    break;

  case 251: /* flush_statement: FLUSH LOG  */
#line 1892 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5833 "parser.cpp"
    break;

  case 252: /* flush_statement: FLUSH BUFFER  */
#line 1896 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5842 "parser.cpp"
    break;

  case 253: /* optimize_statement: OPTIMIZE table_name  */
#line 1904 "parser.y"
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
#line 5857 "parser.cpp"
    break;

  case 254: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1915 "parser.y"
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
#line 5881 "parser.cpp"
    break;

  case 255: /* command_statement: USE IDENTIFIER  */
#line 1938 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5892 "parser.cpp"
    break;

  case 256: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1944 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5902 "parser.cpp"
    break;

  case 257: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1949 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5913 "parser.cpp"
    break;

  case 258: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1955 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5924 "parser.cpp"
    break;

  case 259: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1961 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5937 "parser.cpp"
    break;

  case 260: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1969 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5948 "parser.cpp"
    break;

  case 261: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1975 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5959 "parser.cpp"
    break;

  case 262: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1981 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5970 "parser.cpp"
    break;

  case 263: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1987 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5981 "parser.cpp"
    break;

  case 264: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1993 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5994 "parser.cpp"
    break;

  case 265: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2001 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6005 "parser.cpp"
    break;

  case 266: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2007 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6016 "parser.cpp"
    break;

  case 267: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2013 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6027 "parser.cpp"
    break;

  case 268: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2019 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6038 "parser.cpp"
    break;

  case 269: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2025 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6051 "parser.cpp"
    break;

  case 270: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2033 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6062 "parser.cpp"
    break;

  case 271: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2039 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6073 "parser.cpp"
    break;

  case 272: /* compact_statement: COMPACT TABLE table_name  */
#line 2046 "parser.y"
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
#line 6090 "parser.cpp"
    break;

  case 273: /* expr_array: expr_alias  */
#line 2063 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6099 "parser.cpp"
    break;

  case 274: /* expr_array: expr_array ',' expr_alias  */
#line 2067 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6108 "parser.cpp"
    break;

  case 275: /* expr_array_list: '(' expr_array ')'  */
#line 2072 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6117 "parser.cpp"
    break;

  case 276: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2076 "parser.y"
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
#line 6137 "parser.cpp"
    break;

  case 277: /* expr_alias: expr AS IDENTIFIER  */
#line 2103 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6148 "parser.cpp"
    break;

  case 278: /* expr_alias: expr  */
#line 2109 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6156 "parser.cpp"
    break;

  case 284: /* operand: '(' expr ')'  */
#line 2119 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6164 "parser.cpp"
    break;

  case 285: /* operand: '(' select_without_paren ')'  */
#line 2122 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6175 "parser.cpp"
    break;

  case 286: /* operand: constant_expr  */
#line 2128 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6183 "parser.cpp"
    break;

  case 297: /* extra_match_tensor_option: ',' STRING  */
#line 2142 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6191 "parser.cpp"
    break;

  case 298: /* extra_match_tensor_option: %empty  */
#line 2145 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6199 "parser.cpp"
    break;

  case 299: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2151 "parser.y"
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
#line 6220 "parser.cpp"
    break;

  case 300: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2170 "parser.y"
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
#line 6266 "parser.cpp"
    break;

  case 301: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2212 "parser.y"
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
#line 6313 "parser.cpp"
    break;

  case 302: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2258 "parser.y"
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
#line 6335 "parser.cpp"
    break;

  case 303: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2276 "parser.y"
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
#line 6357 "parser.cpp"
    break;

  case 304: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2294 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6370 "parser.cpp"
    break;

  case 305: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2302 "parser.y"
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
#line 6385 "parser.cpp"
    break;

  case 306: /* query_expr: QUERY '(' STRING ')'  */
#line 2313 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6396 "parser.cpp"
    break;

  case 307: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2319 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6409 "parser.cpp"
    break;

  case 308: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2328 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6420 "parser.cpp"
    break;

  case 309: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2334 "parser.y"
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
#line 6436 "parser.cpp"
    break;

  case 310: /* sub_search: match_vector_expr  */
#line 2346 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6444 "parser.cpp"
    break;

  case 311: /* sub_search: match_text_expr  */
#line 2349 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6452 "parser.cpp"
    break;

  case 312: /* sub_search: match_tensor_expr  */
#line 2352 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6460 "parser.cpp"
    break;

  case 313: /* sub_search: match_sparse_expr  */
#line 2355 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6468 "parser.cpp"
    break;

  case 314: /* sub_search: query_expr  */
#line 2358 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6476 "parser.cpp"
    break;

  case 315: /* sub_search: fusion_expr  */
#line 2361 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6484 "parser.cpp"
    break;

  case 316: /* sub_search_array: sub_search  */
#line 2365 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6493 "parser.cpp"
    break;

  case 317: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2369 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6502 "parser.cpp"
    break;

  case 318: /* function_expr: IDENTIFIER '(' ')'  */
#line 2374 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6515 "parser.cpp"
    break;

  case 319: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2382 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6528 "parser.cpp"
    break;

  case 320: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2390 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6542 "parser.cpp"
    break;

  case 321: /* function_expr: operand IS NOT NULLABLE  */
#line 2399 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6554 "parser.cpp"
    break;

  case 322: /* function_expr: operand IS NULLABLE  */
#line 2406 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6566 "parser.cpp"
    break;

  case 323: /* function_expr: NOT operand  */
#line 2413 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6578 "parser.cpp"
    break;

  case 324: /* function_expr: '-' operand  */
#line 2420 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6590 "parser.cpp"
    break;

  case 325: /* function_expr: '+' operand  */
#line 2427 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6602 "parser.cpp"
    break;

  case 326: /* function_expr: operand '-' operand  */
#line 2434 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6615 "parser.cpp"
    break;

  case 327: /* function_expr: operand '+' operand  */
#line 2442 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6628 "parser.cpp"
    break;

  case 328: /* function_expr: operand '*' operand  */
#line 2450 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6641 "parser.cpp"
    break;

  case 329: /* function_expr: operand '/' operand  */
#line 2458 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6654 "parser.cpp"
    break;

  case 330: /* function_expr: operand '%' operand  */
#line 2466 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6667 "parser.cpp"
    break;

  case 331: /* function_expr: operand '=' operand  */
#line 2474 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6680 "parser.cpp"
    break;

  case 332: /* function_expr: operand EQUAL operand  */
#line 2482 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6693 "parser.cpp"
    break;

  case 333: /* function_expr: operand NOT_EQ operand  */
#line 2490 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6706 "parser.cpp"
    break;

  case 334: /* function_expr: operand '<' operand  */
#line 2498 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6719 "parser.cpp"
    break;

  case 335: /* function_expr: operand '>' operand  */
#line 2506 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6732 "parser.cpp"
    break;

  case 336: /* function_expr: operand LESS_EQ operand  */
#line 2514 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6745 "parser.cpp"
    break;

  case 337: /* function_expr: operand GREATER_EQ operand  */
#line 2522 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6758 "parser.cpp"
    break;

  case 338: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2530 "parser.y"
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
#line 6793 "parser.cpp"
    break;

  case 339: /* function_expr: operand LIKE operand  */
#line 2560 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6806 "parser.cpp"
    break;

  case 340: /* function_expr: operand NOT LIKE operand  */
#line 2568 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6819 "parser.cpp"
    break;

  case 341: /* conjunction_expr: expr AND expr  */
#line 2577 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6832 "parser.cpp"
    break;

  case 342: /* conjunction_expr: expr OR expr  */
#line 2585 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6845 "parser.cpp"
    break;

  case 343: /* between_expr: operand BETWEEN operand AND operand  */
#line 2594 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6857 "parser.cpp"
    break;

  case 344: /* in_expr: operand IN '(' expr_array ')'  */
#line 2602 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6868 "parser.cpp"
    break;

  case 345: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2608 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6879 "parser.cpp"
    break;

  case 346: /* case_expr: CASE expr case_check_array END  */
#line 2615 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6890 "parser.cpp"
    break;

  case 347: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2621 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6902 "parser.cpp"
    break;

  case 348: /* case_expr: CASE case_check_array END  */
#line 2628 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6912 "parser.cpp"
    break;

  case 349: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2633 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6923 "parser.cpp"
    break;

  case 350: /* case_check_array: WHEN expr THEN expr  */
#line 2640 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6935 "parser.cpp"
    break;

  case 351: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2647 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6947 "parser.cpp"
    break;

  case 352: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2655 "parser.y"
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
#line 6975 "parser.cpp"
    break;

  case 353: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2679 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6986 "parser.cpp"
    break;

  case 354: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2685 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6997 "parser.cpp"
    break;

  case 355: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2691 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7009 "parser.cpp"
    break;

  case 356: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2698 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7021 "parser.cpp"
    break;

  case 357: /* column_expr: IDENTIFIER  */
#line 2706 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7033 "parser.cpp"
    break;

  case 358: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2713 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7045 "parser.cpp"
    break;

  case 359: /* column_expr: '*'  */
#line 2720 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7055 "parser.cpp"
    break;

  case 360: /* column_expr: column_expr '.' '*'  */
#line 2725 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7069 "parser.cpp"
    break;

  case 361: /* constant_expr: STRING  */
#line 2735 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7079 "parser.cpp"
    break;

  case 362: /* constant_expr: TRUE  */
#line 2740 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7089 "parser.cpp"
    break;

  case 363: /* constant_expr: FALSE  */
#line 2745 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7099 "parser.cpp"
    break;

  case 364: /* constant_expr: DOUBLE_VALUE  */
#line 2750 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7109 "parser.cpp"
    break;

  case 365: /* constant_expr: LONG_VALUE  */
#line 2755 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7119 "parser.cpp"
    break;

  case 366: /* constant_expr: DATE STRING  */
#line 2760 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7129 "parser.cpp"
    break;

  case 367: /* constant_expr: TIME STRING  */
#line 2765 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7139 "parser.cpp"
    break;

  case 368: /* constant_expr: DATETIME STRING  */
#line 2770 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7149 "parser.cpp"
    break;

  case 369: /* constant_expr: TIMESTAMP STRING  */
#line 2775 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7159 "parser.cpp"
    break;

  case 370: /* constant_expr: INTERVAL interval_expr  */
#line 2780 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7167 "parser.cpp"
    break;

  case 371: /* constant_expr: interval_expr  */
#line 2783 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7175 "parser.cpp"
    break;

  case 372: /* constant_expr: common_array_expr  */
#line 2786 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7183 "parser.cpp"
    break;

  case 373: /* common_array_expr: array_expr  */
#line 2790 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7191 "parser.cpp"
    break;

  case 374: /* common_array_expr: subarray_array_expr  */
#line 2793 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7199 "parser.cpp"
    break;

  case 375: /* common_array_expr: sparse_array_expr  */
#line 2796 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7207 "parser.cpp"
    break;

  case 376: /* common_array_expr: empty_array_expr  */
#line 2799 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7215 "parser.cpp"
    break;

  case 377: /* common_sparse_array_expr: sparse_array_expr  */
#line 2803 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7223 "parser.cpp"
    break;

  case 378: /* common_sparse_array_expr: array_expr  */
#line 2806 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7231 "parser.cpp"
    break;

  case 379: /* common_sparse_array_expr: empty_array_expr  */
#line 2809 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7239 "parser.cpp"
    break;

  case 380: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2813 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7247 "parser.cpp"
    break;

  case 381: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2817 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7257 "parser.cpp"
    break;

  case 382: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2822 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7266 "parser.cpp"
    break;

  case 383: /* sparse_array_expr: long_sparse_array_expr  */
#line 2827 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7274 "parser.cpp"
    break;

  case 384: /* sparse_array_expr: double_sparse_array_expr  */
#line 2830 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7282 "parser.cpp"
    break;

  case 385: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2834 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7290 "parser.cpp"
    break;

  case 386: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2838 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7302 "parser.cpp"
    break;

  case 387: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2845 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7313 "parser.cpp"
    break;

  case 388: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2852 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7321 "parser.cpp"
    break;

  case 389: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2856 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7333 "parser.cpp"
    break;

  case 390: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2863 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7344 "parser.cpp"
    break;

  case 391: /* empty_array_expr: '[' ']'  */
#line 2870 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7352 "parser.cpp"
    break;

  case 392: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 2874 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7360 "parser.cpp"
    break;

  case 393: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 2878 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7368 "parser.cpp"
    break;

  case 394: /* array_expr: long_array_expr  */
#line 2882 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7376 "parser.cpp"
    break;

  case 395: /* array_expr: double_array_expr  */
#line 2885 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7384 "parser.cpp"
    break;

  case 396: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2889 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7392 "parser.cpp"
    break;

  case 397: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2893 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7402 "parser.cpp"
    break;

  case 398: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2898 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7411 "parser.cpp"
    break;

  case 399: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2903 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7419 "parser.cpp"
    break;

  case 400: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2907 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7429 "parser.cpp"
    break;

  case 401: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2912 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7438 "parser.cpp"
    break;

  case 402: /* interval_expr: LONG_VALUE SECONDS  */
#line 2917 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7449 "parser.cpp"
    break;

  case 403: /* interval_expr: LONG_VALUE SECOND  */
#line 2923 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7460 "parser.cpp"
    break;

  case 404: /* interval_expr: LONG_VALUE MINUTES  */
#line 2929 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7471 "parser.cpp"
    break;

  case 405: /* interval_expr: LONG_VALUE MINUTE  */
#line 2935 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7482 "parser.cpp"
    break;

  case 406: /* interval_expr: LONG_VALUE HOURS  */
#line 2941 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7493 "parser.cpp"
    break;

  case 407: /* interval_expr: LONG_VALUE HOUR  */
#line 2947 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7504 "parser.cpp"
    break;

  case 408: /* interval_expr: LONG_VALUE DAYS  */
#line 2953 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7515 "parser.cpp"
    break;

  case 409: /* interval_expr: LONG_VALUE DAY  */
#line 2959 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7526 "parser.cpp"
    break;

  case 410: /* interval_expr: LONG_VALUE MONTHS  */
#line 2965 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7537 "parser.cpp"
    break;

  case 411: /* interval_expr: LONG_VALUE MONTH  */
#line 2971 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7548 "parser.cpp"
    break;

  case 412: /* interval_expr: LONG_VALUE YEARS  */
#line 2977 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7559 "parser.cpp"
    break;

  case 413: /* interval_expr: LONG_VALUE YEAR  */
#line 2983 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7570 "parser.cpp"
    break;

  case 414: /* copy_option_list: copy_option  */
#line 2994 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7579 "parser.cpp"
    break;

  case 415: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2998 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7588 "parser.cpp"
    break;

  case 416: /* copy_option: FORMAT IDENTIFIER  */
#line 3003 "parser.y"
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
#line 7623 "parser.cpp"
    break;

  case 417: /* copy_option: DELIMITER STRING  */
#line 3033 "parser.y"
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
#line 7638 "parser.cpp"
    break;

  case 418: /* copy_option: HEADER  */
#line 3043 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7648 "parser.cpp"
    break;

  case 419: /* copy_option: OFFSET LONG_VALUE  */
#line 3048 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 7658 "parser.cpp"
    break;

  case 420: /* copy_option: LIMIT LONG_VALUE  */
#line 3053 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 7668 "parser.cpp"
    break;

  case 421: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3058 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 7678 "parser.cpp"
    break;

  case 422: /* file_path: STRING  */
#line 3064 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7686 "parser.cpp"
    break;

  case 423: /* if_exists: IF EXISTS  */
#line 3068 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7692 "parser.cpp"
    break;

  case 424: /* if_exists: %empty  */
#line 3069 "parser.y"
  { (yyval.bool_value) = false; }
#line 7698 "parser.cpp"
    break;

  case 425: /* if_not_exists: IF NOT EXISTS  */
#line 3071 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7704 "parser.cpp"
    break;

  case 426: /* if_not_exists: %empty  */
#line 3072 "parser.y"
  { (yyval.bool_value) = false; }
#line 7710 "parser.cpp"
    break;

  case 429: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3087 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7723 "parser.cpp"
    break;

  case 430: /* if_not_exists_info: %empty  */
#line 3095 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7731 "parser.cpp"
    break;

  case 431: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3099 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7739 "parser.cpp"
    break;

  case 432: /* with_index_param_list: %empty  */
#line 3102 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7747 "parser.cpp"
    break;

  case 433: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3106 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7755 "parser.cpp"
    break;

  case 434: /* optional_table_properties_list: %empty  */
#line 3109 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7763 "parser.cpp"
    break;

  case 435: /* index_param_list: index_param  */
#line 3113 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7772 "parser.cpp"
    break;

  case 436: /* index_param_list: index_param_list ',' index_param  */
#line 3117 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7781 "parser.cpp"
    break;

  case 437: /* index_param: IDENTIFIER  */
#line 3122 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7792 "parser.cpp"
    break;

  case 438: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3128 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7807 "parser.cpp"
    break;

  case 439: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3138 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7819 "parser.cpp"
    break;

  case 440: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3145 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7831 "parser.cpp"
    break;

  case 441: /* index_info_list: index_info_list_one_pack  */
#line 3156 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 7840 "parser.cpp"
    break;

  case 442: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3160 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 7852 "parser.cpp"
    break;

  case 443: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3168 "parser.y"
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
    } else if(strcmp((yyvsp[-1].str_value), "diskann") == 0){
        index_type = infinity::IndexType::kDiskAnn;
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
#line 7911 "parser.cpp"
    break;

  case 444: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3222 "parser.y"
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
#line 7929 "parser.cpp"
    break;


#line 7933 "parser.cpp"

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

#line 3236 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

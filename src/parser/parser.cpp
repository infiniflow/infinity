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
  YYSYMBOL_CATALOG = 175,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 176,                   /* SEARCH  */
  YYSYMBOL_MATCH = 177,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 178,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 179,                    /* QUERY  */
  YYSYMBOL_QUERIES = 180,                  /* QUERIES  */
  YYSYMBOL_FUSION = 181,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 182,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 183,                    /* ADMIN  */
  YYSYMBOL_NUMBER = 184,                   /* NUMBER  */
  YYSYMBOL_185_ = 185,                     /* '='  */
  YYSYMBOL_186_ = 186,                     /* '<'  */
  YYSYMBOL_187_ = 187,                     /* '>'  */
  YYSYMBOL_188_ = 188,                     /* '+'  */
  YYSYMBOL_189_ = 189,                     /* '-'  */
  YYSYMBOL_190_ = 190,                     /* '*'  */
  YYSYMBOL_191_ = 191,                     /* '/'  */
  YYSYMBOL_192_ = 192,                     /* '%'  */
  YYSYMBOL_193_ = 193,                     /* '['  */
  YYSYMBOL_194_ = 194,                     /* ']'  */
  YYSYMBOL_195_ = 195,                     /* '('  */
  YYSYMBOL_196_ = 196,                     /* ')'  */
  YYSYMBOL_197_ = 197,                     /* '.'  */
  YYSYMBOL_198_ = 198,                     /* ';'  */
  YYSYMBOL_199_ = 199,                     /* ','  */
  YYSYMBOL_200_ = 200,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 201,                 /* $accept  */
  YYSYMBOL_input_pattern = 202,            /* input_pattern  */
  YYSYMBOL_statement_list = 203,           /* statement_list  */
  YYSYMBOL_statement = 204,                /* statement  */
  YYSYMBOL_explainable_statement = 205,    /* explainable_statement  */
  YYSYMBOL_create_statement = 206,         /* create_statement  */
  YYSYMBOL_table_element_array = 207,      /* table_element_array  */
  YYSYMBOL_table_element = 208,            /* table_element  */
  YYSYMBOL_table_column = 209,             /* table_column  */
  YYSYMBOL_column_type = 210,              /* column_type  */
  YYSYMBOL_column_constraints = 211,       /* column_constraints  */
  YYSYMBOL_column_constraint = 212,        /* column_constraint  */
  YYSYMBOL_default_expr = 213,             /* default_expr  */
  YYSYMBOL_table_constraint = 214,         /* table_constraint  */
  YYSYMBOL_identifier_array = 215,         /* identifier_array  */
  YYSYMBOL_delete_statement = 216,         /* delete_statement  */
  YYSYMBOL_insert_statement = 217,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 218, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 219,        /* explain_statement  */
  YYSYMBOL_explain_type = 220,             /* explain_type  */
  YYSYMBOL_update_statement = 221,         /* update_statement  */
  YYSYMBOL_update_expr_array = 222,        /* update_expr_array  */
  YYSYMBOL_update_expr = 223,              /* update_expr  */
  YYSYMBOL_drop_statement = 224,           /* drop_statement  */
  YYSYMBOL_copy_statement = 225,           /* copy_statement  */
  YYSYMBOL_select_statement = 226,         /* select_statement  */
  YYSYMBOL_select_with_paren = 227,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 228,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 229, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 230, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 231, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 232,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 233,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 234,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 235,            /* order_by_type  */
  YYSYMBOL_limit_expr = 236,               /* limit_expr  */
  YYSYMBOL_offset_expr = 237,              /* offset_expr  */
  YYSYMBOL_distinct = 238,                 /* distinct  */
  YYSYMBOL_from_clause = 239,              /* from_clause  */
  YYSYMBOL_search_clause = 240,            /* search_clause  */
  YYSYMBOL_where_clause = 241,             /* where_clause  */
  YYSYMBOL_having_clause = 242,            /* having_clause  */
  YYSYMBOL_group_by_clause = 243,          /* group_by_clause  */
  YYSYMBOL_set_operator = 244,             /* set_operator  */
  YYSYMBOL_table_reference = 245,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 246,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 247,     /* table_reference_name  */
  YYSYMBOL_table_name = 248,               /* table_name  */
  YYSYMBOL_table_alias = 249,              /* table_alias  */
  YYSYMBOL_with_clause = 250,              /* with_clause  */
  YYSYMBOL_with_expr_list = 251,           /* with_expr_list  */
  YYSYMBOL_with_expr = 252,                /* with_expr  */
  YYSYMBOL_join_clause = 253,              /* join_clause  */
  YYSYMBOL_join_type = 254,                /* join_type  */
  YYSYMBOL_show_statement = 255,           /* show_statement  */
  YYSYMBOL_flush_statement = 256,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 257,       /* optimize_statement  */
  YYSYMBOL_command_statement = 258,        /* command_statement  */
  YYSYMBOL_compact_statement = 259,        /* compact_statement  */
  YYSYMBOL_admin_statement = 260,          /* admin_statement  */
  YYSYMBOL_expr_array = 261,               /* expr_array  */
  YYSYMBOL_expr_array_list = 262,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 263,               /* expr_alias  */
  YYSYMBOL_expr = 264,                     /* expr  */
  YYSYMBOL_operand = 265,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 266, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 267,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 268,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 269,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 270,          /* match_text_expr  */
  YYSYMBOL_query_expr = 271,               /* query_expr  */
  YYSYMBOL_fusion_expr = 272,              /* fusion_expr  */
  YYSYMBOL_sub_search = 273,               /* sub_search  */
  YYSYMBOL_sub_search_array = 274,         /* sub_search_array  */
  YYSYMBOL_function_expr = 275,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 276,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 277,             /* between_expr  */
  YYSYMBOL_in_expr = 278,                  /* in_expr  */
  YYSYMBOL_case_expr = 279,                /* case_expr  */
  YYSYMBOL_case_check_array = 280,         /* case_check_array  */
  YYSYMBOL_cast_expr = 281,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 282,            /* subquery_expr  */
  YYSYMBOL_column_expr = 283,              /* column_expr  */
  YYSYMBOL_constant_expr = 284,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 285,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 286, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 287,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 288, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 289,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 290,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 291, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 292, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 293, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 294,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 295,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 296,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 297,               /* array_expr  */
  YYSYMBOL_long_array_expr = 298,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 299, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 300,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 301, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 302,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 303,         /* copy_option_list  */
  YYSYMBOL_copy_option = 304,              /* copy_option  */
  YYSYMBOL_file_path = 305,                /* file_path  */
  YYSYMBOL_if_exists = 306,                /* if_exists  */
  YYSYMBOL_if_not_exists = 307,            /* if_not_exists  */
  YYSYMBOL_semicolon = 308,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 309,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 310,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 311, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 312,         /* index_param_list  */
  YYSYMBOL_index_param = 313,              /* index_param  */
  YYSYMBOL_index_info_list = 314,          /* index_info_list  */
  YYSYMBOL_index_info_list_one_pack = 315  /* index_info_list_one_pack  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 444 "parser.cpp"

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
#define YYFINAL  96
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1200

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  201
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  458
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  950

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   439


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
       2,     2,     2,     2,     2,     2,     2,   192,     2,     2,
     195,   196,   190,   188,   199,   189,   197,   191,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   200,   198,
     186,   185,   187,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   193,     2,   194,     2,     2,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   496,   496,   500,   506,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
     539,   546,   563,   579,   608,   624,   642,   671,   675,   681,
     684,   691,   741,   777,   778,   779,   780,   781,   782,   783,
     784,   785,   786,   787,   788,   789,   790,   791,   792,   793,
     794,   795,   796,   797,   800,   802,   803,   804,   805,   808,
     809,   810,   811,   812,   813,   814,   815,   816,   817,   818,
     819,   820,   821,   822,   823,   824,   825,   826,   827,   828,
     829,   830,   831,   832,   833,   834,   835,   836,   837,   838,
     839,   840,   841,   842,   843,   844,   845,   846,   847,   848,
     849,   868,   872,   882,   885,   888,   891,   895,   898,   903,
     908,   915,   921,   931,   947,   981,   994,   997,  1004,  1010,
    1013,  1016,  1019,  1022,  1025,  1028,  1031,  1038,  1051,  1055,
    1060,  1073,  1086,  1101,  1116,  1131,  1154,  1207,  1262,  1313,
    1316,  1319,  1328,  1338,  1341,  1345,  1350,  1372,  1375,  1380,
    1396,  1399,  1403,  1407,  1412,  1418,  1421,  1424,  1428,  1432,
    1434,  1438,  1440,  1443,  1447,  1450,  1454,  1459,  1463,  1466,
    1470,  1473,  1477,  1480,  1484,  1487,  1490,  1493,  1501,  1504,
    1519,  1519,  1521,  1535,  1544,  1549,  1558,  1563,  1568,  1574,
    1581,  1584,  1588,  1591,  1596,  1608,  1615,  1629,  1632,  1635,
    1638,  1641,  1644,  1647,  1653,  1657,  1661,  1665,  1669,  1676,
    1680,  1684,  1688,  1693,  1697,  1702,  1706,  1710,  1716,  1722,
    1728,  1739,  1750,  1761,  1773,  1785,  1798,  1812,  1823,  1837,
    1853,  1870,  1874,  1878,  1886,  1890,  1894,  1902,  1913,  1936,
    1942,  1947,  1953,  1959,  1967,  1973,  1979,  1985,  1991,  1999,
    2005,  2011,  2017,  2023,  2031,  2037,  2044,  2057,  2061,  2066,
    2071,  2077,  2083,  2090,  2097,  2105,  2113,  2122,  2131,  2141,
    2148,  2156,  2164,  2173,  2177,  2182,  2187,  2198,  2202,  2207,
    2211,  2238,  2244,  2248,  2249,  2250,  2251,  2252,  2254,  2257,
    2263,  2266,  2267,  2268,  2269,  2270,  2271,  2272,  2273,  2274,
    2275,  2277,  2280,  2286,  2305,  2347,  2393,  2411,  2429,  2437,
    2448,  2454,  2463,  2469,  2481,  2484,  2487,  2490,  2493,  2496,
    2500,  2504,  2509,  2517,  2525,  2534,  2541,  2548,  2555,  2562,
    2569,  2577,  2585,  2593,  2601,  2609,  2617,  2625,  2633,  2641,
    2649,  2657,  2665,  2695,  2703,  2712,  2720,  2729,  2737,  2743,
    2750,  2756,  2763,  2768,  2775,  2782,  2790,  2814,  2820,  2826,
    2833,  2841,  2848,  2855,  2860,  2870,  2875,  2880,  2885,  2890,
    2895,  2900,  2905,  2910,  2915,  2918,  2921,  2925,  2928,  2931,
    2934,  2938,  2941,  2944,  2948,  2952,  2957,  2962,  2965,  2969,
    2973,  2980,  2987,  2991,  2998,  3005,  3009,  3013,  3017,  3020,
    3024,  3028,  3033,  3038,  3042,  3047,  3052,  3058,  3064,  3070,
    3076,  3082,  3088,  3094,  3100,  3106,  3112,  3118,  3129,  3133,
    3138,  3168,  3178,  3183,  3188,  3193,  3199,  3203,  3204,  3206,
    3207,  3209,  3210,  3222,  3230,  3234,  3237,  3241,  3244,  3248,
    3252,  3257,  3263,  3273,  3280,  3291,  3295,  3303,  3357
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
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "NUMBER",
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
  "compact_statement", "admin_statement", "expr_array", "expr_array_list",
  "expr_alias", "expr", "operand", "extra_match_tensor_option",
  "match_tensor_expr", "match_vector_expr", "match_sparse_expr",
  "match_text_expr", "query_expr", "fusion_expr", "sub_search",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
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

#define YYPACT_NINF (-523)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-445)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     209,   279,    -3,   376,    77,   -36,    77,   318,   781,   274,
      59,   342,   100,   104,   121,    -7,   133,   -44,   153,   109,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,   163,  -523,
    -523,   174,  -523,  -523,  -523,  -523,  -523,  -523,   243,   243,
     243,   243,     5,    77,   252,   252,   252,   252,   252,    40,
     320,    77,   -11,   329,   340,   362,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,    74,   369,    77,  -523,  -523,  -523,  -523,
    -523,   331,   125,   303,  -523,   385,  -523,   172,  -523,  -523,
     228,  -523,    77,  -523,  -523,  -523,  -523,   -37,  -523,   344,
     215,  -523,   402,   156,   220,   232,  -523,    34,  -523,   409,
    -523,  -523,     3,   378,  -523,   381,   388,   456,    77,    77,
      77,   462,   423,   317,   459,   511,    77,    77,    77,   544,
     546,   564,   505,   571,   571,   490,    88,    99,   124,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,   163,  -523,  -523,  -523,
    -523,  -523,  -523,   420,  -523,  -523,   580,  -523,   592,  -523,
    -523,   597,  -523,    77,   455,   121,   571,   678,  -523,  -523,
     685,  -523,  -523,  -523,  -523,     3,  -523,  -523,  -523,   490,
     611,   638,   636,  -523,   -40,  -523,   317,  -523,    77,   706,
      31,  -523,  -523,  -523,  -523,  -523,   650,  -523,   528,   -13,
    -523,   490,  -523,  -523,   635,   640,   526,  -523,  -523,   754,
     584,   527,   529,   336,   721,   722,   723,   724,  -523,  -523,
     725,   534,   157,   535,   541,   648,   648,  -523,    16,   452,
       1,  -523,   -19,   208,  -523,  -523,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,   540,  -523,  -523,
    -523,    57,  -523,  -523,    65,  -523,    76,  -523,  -523,  -523,
      79,  -523,   117,  -523,  -523,  -523,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,   736,
     739,  -523,  -523,  -523,  -523,  -523,  -523,   666,   174,  -523,
    -523,    30,   182,   553,   557,   -49,   490,   490,   683,  -523,
     -44,    53,   698,   562,  -523,   185,   563,  -523,    77,   490,
     564,  -523,   355,   565,   567,   190,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,   648,   569,
     422,   679,   490,   490,    39,   281,  -523,  -523,  -523,  -523,
     754,  -523,   762,   572,   573,   574,   576,   771,   772,   390,
     390,  -523,   578,  -523,  -523,  -523,  -523,   587,   119,   710,
     490,   796,   490,   490,   -45,   608,   -17,   648,   648,   648,
     648,   648,   648,   648,   648,   648,   648,   648,   648,   648,
     648,    22,  -523,   613,  -523,   798,  -523,   799,  -523,   805,
    -523,   808,   776,   474,   628,  -523,   630,   822,  -523,   824,
    -523,  -523,  -523,    11,   655,   634,  -523,     4,   355,   490,
    -523,   163,   763,   705,   644,   214,  -523,  -523,  -523,   -44,
     706,   562,  -523,  -523,   831,   490,   641,  -523,   355,  -523,
     179,   179,   490,  -523,   253,   679,   680,   646,    49,   101,
     288,  -523,   490,   490,   766,   490,   866,    23,   490,   265,
     289,   552,  -523,  -523,   571,  -523,  -523,  -523,   726,   682,
     648,   452,   751,  -523,   508,   508,   429,   429,   660,   508,
     508,   429,   429,   390,   390,  -523,  -523,  -523,  -523,  -523,
    -523,   681,  -523,   684,  -523,  -523,  -523,   879,   895,  -523,
     899,  -523,  -523,    83,   -44,   704,   735,  -523,    68,  -523,
     210,   505,   490,  -523,  -523,  -523,   355,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,   709,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,
    -523,   711,   712,   713,   714,   715,   207,   716,   706,   877,
      53,   163,   293,  -523,  -523,   334,   717,   907,   908,   911,
     913,  -523,   912,   338,  -523,   352,   400,  -523,   727,  -523,
     763,   490,  -523,   490,   -33,   138,   648,   -78,   718,  -523,
      46,   115,  -523,   915,  -523,   916,  -523,  -523,   841,   452,
     508,   728,   401,  -523,   648,   919,   917,   873,   878,   742,
     405,  -523,   923,  -523,    42,    11,   871,  -523,  -523,  -523,
    -523,  -523,  -523,   872,  -523,   929,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,   734,   880,  -523,   930,   384,   612,
     255,   649,   693,   806,   810,  -523,  -523,   171,  -523,   807,
     706,   406,   744,  -523,  -523,   779,  -523,   490,  -523,  -523,
    -523,  -523,  -523,  -523,   179,  -523,  -523,  -523,   745,   355,
     -12,  -523,   490,   354,   749,   939,   613,   752,   748,   750,
     753,   755,   431,  -523,  -523,   422,   941,   943,   418,  -523,
     899,   464,    68,   735,    11,    11,   757,   210,   898,   894,
     436,   756,   758,   759,   760,   761,   764,   765,   861,   767,
     768,   769,   770,   773,   774,   775,   777,   863,   778,   780,
     782,   783,   784,   785,   786,   787,   788,   789,   790,   791,
     792,   793,   794,   795,   797,   800,   801,   802,   803,   804,
     809,   811,   812,  -523,  -523,    14,  -523,  -523,  -523,   438,
    -523,   899,   958,   440,  -523,  -523,  -523,   355,  -523,   620,
     813,   447,   814,    10,   815,  -523,  -523,  -523,  -523,  -523,
     179,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,   956,
     959,  -523,  -523,  -523,  -523,   906,   706,  -523,   490,   490,
    -523,  -523,   965,   969,   972,   974,   989,   991,   992,   998,
     816,   999,  1000,  1001,  1003,  1010,  1011,  1012,  1013,   821,
    1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,
    1025,  1026,  1027,  1028,  1029,  1030,  1031,  1032,  1033,  1034,
    1035,  1036,  1037,  1038,  1039,  -523,  -523,   465,   666,  -523,
    -523,  1042,  -523,  1043,  1044,  1045,   466,   602,   633,   490,
     476,   851,   355,   855,   856,   857,   858,   859,   860,   862,
     864,  1051,   865,   867,   868,   869,   870,   874,   875,   876,
    1053,   881,   882,   883,   884,   885,   886,   887,   888,   889,
     890,   891,   892,   893,   896,   897,   900,   901,   902,   903,
     904,   905,   909,   910,   914,   918,  -523,  -523,   920,   921,
     922,   477,  -523,  1056,  -523,  1061,  -523,   355,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,   924,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,   926,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,
    -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,  -523,
    -523,  -523,  1064,  -523,  1065,   666,  1067,  -523,   639,  -523,
    -523,   478,   925,  -523,   927,  1068,  -523,   666,  1069,  1072,
     931,   666,  -523,  -523,   932,  -523,  -523,  -523,   666,  -523
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     201,     0,     0,     0,     0,     0,     0,     0,   136,     0,
       0,     0,     0,     0,     0,     0,     0,   201,     0,   442,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   150,
     149,     0,     8,    14,    15,    16,    17,    18,   440,   440,
     440,   440,   440,     0,   438,   438,   438,   438,   438,   194,
       0,     0,     0,     0,     0,     0,   130,   134,   131,   132,
     133,   135,   129,   201,     0,     0,   215,   216,   214,   220,
     223,     0,     0,     0,   217,     0,   219,     0,   241,   243,
       0,   221,     0,   244,   245,   246,   249,   194,   247,     0,
     200,   202,     0,     0,     0,     0,     1,   201,     2,   184,
     186,   187,     0,   173,   155,   161,     0,     0,     0,     0,
       0,     0,     0,   127,     0,     0,     0,     0,     0,     0,
       0,     0,   179,     0,     0,     0,     0,     0,     0,   128,
      19,    24,    26,    25,    20,    21,    23,    22,    27,    28,
      29,    30,   229,   230,   224,   225,     0,   226,     0,   218,
     242,     0,   266,     0,     0,     0,     0,     0,   283,   267,
       0,   154,   153,     4,   185,     0,   151,   152,   172,     0,
       0,   169,     0,    31,     0,    32,   127,   443,     0,     0,
     201,   437,   141,   143,   142,   144,     0,   195,     0,   179,
     138,     0,   123,   436,     0,     0,   371,   375,   378,   379,
       0,     0,     0,     0,     0,     0,     0,     0,   376,   377,
       0,     0,     0,     0,     0,     0,     0,   373,     0,   201,
       0,   287,   292,   293,   307,   305,   308,   306,   309,   310,
     302,   297,   296,   295,   303,   304,   294,   301,   300,   386,
     388,     0,   389,   397,     0,   398,     0,   390,   387,   408,
       0,   409,     0,   385,   253,   255,   254,   251,   252,   258,
     260,   259,   256,   257,   263,   265,   264,   261,   262,     0,
       0,   232,   231,   237,   227,   228,   222,   446,     0,   203,
     250,   284,   268,     0,     0,   175,     0,     0,   171,   439,
     201,     0,     0,     0,   121,     0,     0,   125,     0,     0,
       0,   137,   178,     0,     0,     0,   417,   416,   419,   418,
     421,   420,   423,   422,   425,   424,   427,   426,     0,     0,
     337,   201,     0,     0,     0,     0,   380,   381,   382,   383,
       0,   384,     0,     0,     0,     0,     0,     0,     0,   339,
     338,   414,   411,   405,   395,   400,   403,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   394,     0,   399,     0,   402,     0,   410,     0,
     413,     0,   238,   233,     0,   248,     0,     0,   285,     0,
     269,   158,   157,     0,   177,   160,   162,   167,   168,     0,
     156,    34,     0,     0,     0,     0,    37,    39,    40,   201,
       0,    36,   455,   126,     0,     0,   124,   145,   140,   139,
       0,     0,     0,   332,     0,   201,     0,     0,     0,     0,
       0,   362,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   299,   298,     0,   288,   291,   355,   356,     0,
       0,   201,     0,   336,   346,   347,   350,   351,     0,   353,
     345,   348,   349,   341,   340,   342,   343,   344,   372,   374,
     396,     0,   401,     0,   404,   412,   415,     0,     0,   234,
       0,   204,   286,   270,   201,   174,   188,   190,   199,   191,
       0,   179,     0,   165,   166,   164,   170,    43,    46,    47,
      44,    45,    48,    49,    65,    50,    52,    51,    68,    55,
      56,    57,    53,    54,    58,    59,    60,    61,    62,    63,
      64,     0,     0,     0,     0,     0,   446,     0,     0,   448,
       0,    35,     0,   456,   122,     0,     0,     0,     0,     0,
       0,   432,     0,     0,   428,     0,     0,   333,     0,   367,
       0,     0,   360,     0,     0,     0,     0,     0,     0,   371,
       0,     0,   320,     0,   322,     0,   407,   406,     0,   201,
     354,     0,     0,   335,     0,     0,     0,   239,   235,   451,
       0,   449,     0,   271,     0,     0,     0,   208,   209,   210,
     211,   207,   212,     0,   197,     0,   192,   326,   324,   327,
     325,   328,   329,   330,   176,   183,   163,     0,     0,     0,
       0,     0,     0,     0,     0,   114,   115,   118,   111,   118,
       0,     0,     0,    33,    38,   458,   289,     0,   434,   433,
     431,   430,   435,   148,     0,   146,   334,   368,     0,   364,
       0,   363,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   369,   358,   357,     0,     0,     0,   445,
       0,   272,   199,   189,     0,     0,   196,     0,     0,   181,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   116,   113,     0,   112,    42,    41,     0,
     120,     0,     0,     0,   429,   366,   361,   365,   352,     0,
       0,     0,     0,     0,     0,   391,   393,   392,   321,   323,
       0,   370,   359,   240,   236,   452,   454,   453,   450,     0,
       0,   273,   279,   193,   205,     0,     0,   331,     0,     0,
     159,    67,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   117,   119,     0,   446,   290,
     411,     0,   318,     0,     0,     0,     0,   280,   274,     0,
       0,   182,   180,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   447,   457,     0,     0,
       0,     0,   147,     0,   281,     0,   275,   206,   198,    66,
      72,    73,    70,    71,    74,    75,    76,     0,    69,    97,
      98,    95,    96,    99,   100,   101,     0,    94,    81,    82,
      79,    80,    83,    84,    85,    78,   106,   107,   104,   105,
     108,   109,   110,   103,    89,    90,    87,    88,    91,    92,
      93,    86,     0,   319,     0,   446,     0,   282,   276,    77,
     102,     0,   312,   317,     0,     0,   277,   446,     0,     0,
       0,   446,   278,   315,     0,   311,   313,   316,   446,   314
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -523,  -523,  -523,   993,  -523,  1040,  -523,   561,  -523,   545,
    -523,   485,   475,  -523,  -405,  1041,  1046,   935,  -523,  -523,
    1049,  -523,   818,  1050,  1052,   -57,  1090,   -16,   830,   951,
     -67,  -523,  -523,   637,  -523,  -523,  -523,  -523,  -523,  -523,
    -179,  -523,  -523,  -523,  -523,   547,    37,    27,   463,  -523,
    -523,   971,  -523,  -523,  1070,  1071,  1073,  1074,  1075,  -523,
    -167,  -523,   817,  -191,  -192,  -523,  -477,  -453,  -439,  -437,
    -436,  -427,   468,  -523,  -523,  -523,  -523,  -523,  -523,   819,
    -523,  -523,   694,   415,  -215,  -523,  -523,  -523,   492,  -523,
    -523,  -523,  -523,   493,   820,   823,  -505,  -523,  -523,  -523,
    -523,   934,  -412,   507,  -117,   473,   522,  -523,  -523,  -522,
    -523,   421,   486,  -523,   737
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   129,    21,   405,   406,   407,   526,
     617,   618,   717,   408,   295,    22,    23,   180,    24,    63,
      25,   189,   190,    26,    27,    28,    29,    30,   104,   166,
     105,   171,   395,   396,   495,   288,   400,   169,   394,   491,
     192,   760,   669,   102,   485,   486,   487,   488,   596,    31,
      90,    91,   489,   593,    32,    33,    34,    35,    36,    37,
     220,   416,   221,   222,   223,   940,   224,   225,   226,   227,
     228,   229,   603,   604,   230,   231,   232,   233,   234,   325,
     235,   236,   237,   238,   239,   734,   240,   241,   242,   243,
     244,   245,   246,   247,   345,   346,   248,   249,   250,   251,
     252,   253,   543,   544,   194,   115,   107,    98,   112,   385,
     623,   580,   581,   411,   412
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     302,    95,   285,   344,   619,   532,   136,   195,   320,   545,
     301,   103,   324,   597,    49,   341,   342,   290,   197,   198,
     199,   341,   342,   339,   340,   468,   559,   153,   348,   393,
      43,    50,   449,    52,  -441,   167,    14,   598,   351,   280,
      88,     1,    51,     2,     3,     4,     5,     6,     7,     8,
       9,   599,   641,   600,   601,   191,   402,    10,   452,    11,
      12,    13,    99,   602,   100,   296,   101,   123,   124,  -444,
     113,   594,   387,   726,   352,   353,   493,   494,   122,   106,
      49,     1,   388,     2,     3,     4,     5,     6,     7,    82,
       9,   254,   143,   255,   256,   397,   398,    10,   284,    11,
      12,    13,   259,    86,   260,   261,   550,    87,   418,   152,
     450,    14,   453,   582,    14,   583,   204,   205,   206,   352,
     353,   644,   207,   621,    89,   595,   320,   264,   323,   265,
     266,   428,   429,   352,   353,   174,   175,   176,   424,   730,
     352,   353,   737,   183,   184,   185,   208,   209,   210,    93,
     350,    17,   257,    96,    14,   291,   352,   353,   470,    92,
     120,   447,   448,   262,   297,   454,   455,   456,   457,   458,
     459,   460,   461,   462,   463,   464,   465,   466,   467,   403,
     277,   404,   103,    99,   125,   100,   300,   101,   267,   551,
     597,   352,   353,   196,   197,   198,   199,   349,   165,    15,
     350,   352,   353,   347,   343,   293,   484,   218,   496,   218,
     343,   389,   469,   217,   598,   719,     1,    16,     2,     3,
       4,     5,     6,     7,     8,     9,   642,   390,   599,    17,
     600,   601,    10,   401,    11,    12,    13,   120,   662,    15,
     602,   554,   555,   371,   557,   646,   613,   561,   535,   537,
     538,   372,   258,   352,   353,   546,   373,   422,   570,   374,
     539,   540,   541,   263,   375,   200,   201,   352,   353,    17,
     376,   352,   353,   378,   202,   377,   203,   333,   379,   334,
     335,   336,   613,   354,   572,   355,   867,   384,   268,    14,
     352,   353,   204,   205,   206,   145,   146,   614,   207,   615,
     616,   397,   715,    64,    65,   427,    66,    97,    38,    39,
      40,   380,   605,   157,   647,   443,   381,   106,    67,    68,
      41,    42,   208,   209,   210,   417,   114,   568,   816,   158,
     159,   160,   126,   614,   121,   615,   616,   144,   356,   196,
     197,   198,   199,   127,   211,   150,   689,   690,   691,   692,
     693,   820,   531,   694,   695,   357,   358,   359,   360,   361,
     639,   542,   640,   362,   643,   128,   431,   212,   432,   213,
     433,   214,   142,   552,    15,   553,   696,   433,   215,   216,
     217,   413,   655,   218,   414,   219,   423,   212,   149,   213,
     151,   214,    16,   363,   364,   365,   366,   367,   368,   369,
     370,   154,   652,   933,    17,    44,    45,    46,   156,   548,
     529,   200,   201,   530,   155,   943,   161,    47,    48,   947,
     202,   745,   203,   746,   747,   323,   949,   584,   162,   426,
     164,   732,    69,    70,    71,   571,    72,    73,   204,   205,
     206,    74,    75,    76,   207,   168,    77,    78,    79,   547,
     170,   727,   350,    80,    81,   196,   197,   198,   199,   173,
     723,   562,   269,   172,   563,   177,   270,   271,   208,   209,
     210,   272,   273,   147,   148,   671,   672,   673,   674,   675,
      53,    54,   676,   677,   356,   564,    55,   178,   565,   625,
     211,   678,   414,   196,   197,   198,   199,   426,    83,    84,
      85,   357,   358,   359,   360,   679,   749,   352,   353,   362,
     750,   751,   179,   212,   182,   213,   752,   214,   116,   117,
     118,   119,   478,   479,   215,   216,   217,   200,   201,   218,
     626,   219,    14,   350,   633,   181,   202,   634,   203,   363,
     364,   365,   366,   367,   368,   369,   370,   186,   635,   187,
     728,   634,   356,   651,   204,   205,   206,   566,   567,   356,
     207,   108,   109,   110,   111,   200,   201,   188,   822,   357,
     358,   359,   360,   191,   202,   193,   203,   362,  -445,  -445,
     368,   369,   370,   274,   208,   209,   210,   196,   197,   198,
     199,   821,   204,   205,   206,   275,   636,   654,   207,   350,
     350,   659,   720,   276,   660,   414,   211,   363,   364,   365,
     366,   367,   368,   369,   370,  -445,  -445,   366,   367,   368,
     369,   370,   208,   209,   210,   341,   810,   742,   877,   212,
     350,   213,   761,   214,   806,   762,   809,   414,   356,   350,
     215,   216,   217,   812,   211,   218,   813,   219,   873,   874,
     278,   196,   197,   198,   199,  -445,  -445,   359,   360,   318,
     319,   866,   872,  -445,   660,   634,   286,   212,   202,   213,
     203,   214,   878,   925,   937,   414,   926,   938,   215,   216,
     217,   875,   876,   218,   281,   219,   204,   205,   206,   935,
     936,   282,   207,  -445,   364,   365,   366,   367,   368,   369,
     370,   754,   755,   680,   681,   682,   683,   684,   287,   294,
     685,   686,   289,   299,   298,   303,   208,   209,   210,   687,
     304,   305,   321,   318,   322,   326,   327,   328,   329,   332,
     337,   330,   202,   688,   203,   426,   338,   371,   211,   382,
     697,   698,   699,   700,   701,   383,   384,   702,   703,   391,
     204,   205,   206,   392,   399,   409,   207,   410,   415,    14,
     420,   212,   421,   213,   425,   214,   434,   435,   436,   437,
     704,   438,   215,   216,   217,   439,   440,   218,   441,   219,
     208,   209,   210,   442,   705,   706,   707,   708,   709,   444,
     356,   710,   711,   586,  -213,   587,   588,   589,   590,   446,
     591,   592,   211,   451,   471,   473,   218,   357,   358,   359,
     360,   475,   574,   476,   712,   362,    56,    57,    58,    59,
      60,    61,   477,   480,    62,   212,   481,   213,   482,   214,
     483,   490,   527,   492,   534,   450,   215,   216,   217,   528,
     536,   218,   549,   219,   556,   363,   364,   365,   366,   367,
     368,   369,   370,   497,   498,   499,   500,   501,   502,   503,
     504,   505,   506,   507,   508,   509,   510,   511,   512,   513,
     558,   514,   515,   516,   517,   518,   519,   569,   352,   520,
     573,   575,   521,   522,   576,   577,   523,   524,   525,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   578,   579,   585,   607,   622,   608,   609,   610,   611,
     612,   620,   627,   628,   629,   630,   631,   645,   632,   648,
     649,   650,   566,   637,   653,   567,   656,   658,   657,   661,
     664,   665,   666,   667,   668,   713,   670,   714,   715,   721,
     722,   725,   729,   731,   738,   733,   739,   743,   740,   744,
     759,   741,   756,   758,   770,   763,   779,   764,   765,   766,
     767,   808,   817,   768,   769,   818,   771,   772,   773,   774,
     819,   823,   775,   776,   777,   824,   778,   780,   825,   781,
     826,   782,   783,   784,   785,   786,   787,   788,   789,   790,
     791,   792,   793,   794,   795,   827,   796,   828,   829,   797,
     798,   799,   800,   801,   830,   832,   833,   834,   802,   835,
     803,   804,   811,   814,   815,   831,   836,   837,   838,   839,
     840,   841,   842,   843,   844,   845,   846,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   856,   857,   858,   859,
     860,   861,   862,   863,   864,   865,   868,   869,   870,   871,
     350,   879,   880,   881,   882,   883,   884,   887,   885,   896,
     886,   888,   927,   889,   890,   891,   892,   928,   931,   932,
     893,   894,   895,   934,   942,   944,   945,   897,   898,   899,
     900,   901,   902,   903,   904,   905,   906,   907,   908,   909,
     163,   624,   910,   911,   718,   638,   912,   913,   914,   915,
     916,   917,   716,   130,   131,   918,   919,    94,   386,   132,
     920,   292,   133,   134,   921,   135,   283,   923,   419,   922,
     929,   924,   930,   941,   939,   753,   279,   946,   948,   606,
     805,   560,   663,   137,   138,   757,   139,   140,   141,   735,
     736,   724,   807,   430,   331,     0,   748,     0,   533,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   445,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   472,     0,     0,     0,     0,
     474
};

static const yytype_int16 yycheck[] =
{
     191,    17,   169,   218,   526,   410,    63,   124,   200,   421,
     189,     8,   203,   490,     3,     5,     6,    57,     4,     5,
       6,     5,     6,   215,   216,     3,     3,    64,   219,    78,
      33,     4,    77,     6,     0,   102,    80,   490,    57,   156,
      13,     7,    78,     9,    10,    11,    12,    13,    14,    15,
      16,   490,    85,   490,   490,    68,     3,    23,    75,    25,
      26,    27,    20,   490,    22,    34,    24,    78,    79,    64,
      43,     3,    42,    85,   152,   153,    72,    73,    51,    74,
       3,     7,    52,     9,    10,    11,    12,    13,    14,    30,
      16,     3,    65,     5,     6,   286,   287,    23,   165,    25,
      26,    27,     3,     3,     5,     6,    57,     3,   299,    82,
     155,    80,   129,    30,    80,    32,   102,   103,   104,   152,
     153,   199,   108,   528,     3,    57,   318,     3,    89,     5,
       6,   322,   323,   152,   153,   108,   109,   110,   305,   644,
     152,   153,   647,   116,   117,   118,   132,   133,   134,    16,
     199,   195,    64,     0,    80,   195,   152,   153,   373,   166,
     197,   352,   353,    64,   180,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   366,   367,   368,   369,   370,   126,
     153,   128,     8,    20,   195,    22,   199,    24,    64,    88,
     667,   152,   153,     3,     4,     5,     6,   196,   195,   165,
     199,   152,   153,   219,   194,   178,   195,   193,   399,   193,
     194,    29,   190,   190,   667,   620,     7,   183,     9,    10,
      11,    12,    13,    14,    15,    16,    88,    45,   667,   195,
     667,   667,    23,   290,    25,    26,    27,   197,   196,   165,
     667,   432,   433,   197,   435,   199,    75,   438,   415,    70,
      71,   194,   164,   152,   153,   422,   199,    67,   450,   194,
      81,    82,    83,   164,   199,    75,    76,   152,   153,   195,
     194,   152,   153,   194,    84,   199,    86,   120,   199,   122,
     123,   124,    75,    75,   451,    77,   808,    80,   164,    80,
     152,   153,   102,   103,   104,   170,   171,   126,   108,   128,
     129,   492,   131,    29,    30,   321,    32,   198,    29,    30,
      31,   194,   491,   157,   199,   196,   199,    74,    44,    45,
      41,    42,   132,   133,   134,   298,    74,   444,   740,   173,
     174,   175,     3,   126,    14,   128,   129,     6,   130,     3,
       4,     5,     6,     3,   154,   173,    91,    92,    93,    94,
      95,   756,   409,    98,    99,   147,   148,   149,   150,   151,
     551,   182,   553,   155,   556,     3,    85,   177,    87,   179,
      89,   181,     3,    85,   165,    87,   121,    89,   188,   189,
     190,   196,   574,   193,   199,   195,   196,   177,     3,   179,
     162,   181,   183,   185,   186,   187,   188,   189,   190,   191,
     192,    57,   569,   925,   195,    29,    30,    31,     6,   425,
     196,    75,    76,   199,   199,   937,   196,    41,    42,   941,
      84,     3,    86,     5,     6,    89,   948,   484,   196,    75,
      21,   646,   158,   159,   160,   451,   162,   163,   102,   103,
     104,   167,   168,   169,   108,    67,   172,   173,   174,   196,
      69,   642,   199,   179,   180,     3,     4,     5,     6,     3,
     627,   196,    42,    75,   199,     3,    46,    47,   132,   133,
     134,    51,    52,   170,   171,    91,    92,    93,    94,    95,
     162,   163,    98,    99,   130,   196,   168,    64,   199,   196,
     154,   107,   199,     3,     4,     5,     6,    75,   156,   157,
     158,   147,   148,   149,   150,   121,    42,   152,   153,   155,
      46,    47,   195,   177,     3,   179,    52,   181,    45,    46,
      47,    48,    48,    49,   188,   189,   190,    75,    76,   193,
     196,   195,    80,   199,   196,    76,    84,   199,    86,   185,
     186,   187,   188,   189,   190,   191,   192,     3,   196,     3,
     196,   199,   130,   569,   102,   103,   104,     5,     6,   130,
     108,    39,    40,    41,    42,    75,    76,     3,   759,   147,
     148,   149,   150,    68,    84,     4,    86,   155,   149,   150,
     190,   191,   192,     3,   132,   133,   134,     3,     4,     5,
       6,   758,   102,   103,   104,     3,   196,   196,   108,   199,
     199,   196,   196,     6,   199,   199,   154,   185,   186,   187,
     188,   189,   190,   191,   192,   186,   187,   188,   189,   190,
     191,   192,   132,   133,   134,     5,     6,   196,   819,   177,
     199,   179,   196,   181,   196,   199,   196,   199,   130,   199,
     188,   189,   190,   196,   154,   193,   199,   195,    46,    47,
     195,     3,     4,     5,     6,   147,   148,   149,   150,    75,
      76,   196,   196,   155,   199,   199,    55,   177,    84,   179,
      86,   181,   196,   196,   196,   199,   199,   199,   188,   189,
     190,    48,    49,   193,     6,   195,   102,   103,   104,    50,
      51,     6,   108,   185,   186,   187,   188,   189,   190,   191,
     192,   664,   665,    91,    92,    93,    94,    95,    70,     3,
      98,    99,    76,   185,    64,    80,   132,   133,   134,   107,
      80,   195,   195,    75,   195,     4,     4,     4,     4,   195,
     195,     6,    84,   121,    86,    75,   195,   197,   154,     3,
      91,    92,    93,    94,    95,     6,    80,    98,    99,   196,
     102,   103,   104,   196,    71,    57,   108,   195,   195,    80,
     195,   177,   195,   179,   195,   181,     4,   195,   195,   195,
     121,   195,   188,   189,   190,     4,     4,   193,   200,   195,
     132,   133,   134,   196,    91,    92,    93,    94,    95,    79,
     130,    98,    99,    58,    59,    60,    61,    62,    63,     3,
      65,    66,   154,   195,     6,     6,   193,   147,   148,   149,
     150,     6,   152,     5,   121,   155,    35,    36,    37,    38,
      39,    40,    46,   195,    43,   177,   196,   179,     6,   181,
       6,   176,   127,   199,     3,   155,   188,   189,   190,   195,
     199,   193,   196,   195,    78,   185,   186,   187,   188,   189,
     190,   191,   192,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
       4,   108,   109,   110,   111,   112,   113,   195,   152,   116,
     129,   200,   119,   120,   200,     6,   123,   124,   125,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,     6,     3,   199,   195,    28,   195,   195,   195,   195,
     195,   195,   195,     6,     6,     4,     3,   199,     6,     4,
       4,    80,     5,   196,   196,     6,    53,   185,    50,     6,
      59,    59,     3,   199,    54,   129,     6,   127,   131,   195,
     161,   196,   193,     4,   196,   193,   196,     6,   195,     6,
      56,   196,   195,    55,    93,   199,    93,   199,   199,   199,
     199,     3,     6,   199,   199,     6,   199,   199,   199,   199,
      64,     6,   199,   199,   199,     6,   199,   199,     6,   199,
       6,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,     6,   199,     6,     6,   199,
     199,   199,   199,   199,     6,     6,     6,     6,   199,     6,
     199,   199,   199,   199,   199,   199,     6,     6,     6,     6,
     199,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     4,     4,     4,     4,
     199,   196,   196,   196,   196,   196,   196,     6,   196,     6,
     196,   196,     6,   196,   196,   196,   196,     6,     4,     4,
     196,   196,   196,     6,     6,     6,     4,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
      97,   530,   196,   196,   619,   550,   196,   196,   196,   196,
     196,   196,   617,    63,    63,   196,   196,    17,   278,    63,
     196,   176,    63,    63,   196,    63,   165,   196,   300,   199,
     196,   199,   196,   196,   199,   662,   155,   196,   196,   492,
     715,   437,   585,    63,    63,   667,    63,    63,    63,   647,
     647,   634,   721,   324,   210,    -1,   660,    -1,   411,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   350,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   375,    -1,    -1,    -1,    -1,
     377
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   165,   183,   195,   202,   203,
     204,   206,   216,   217,   219,   221,   224,   225,   226,   227,
     228,   250,   255,   256,   257,   258,   259,   260,    29,    30,
      31,    41,    42,    33,    29,    30,    31,    41,    42,     3,
     248,    78,   248,   162,   163,   168,    35,    36,    37,    38,
      39,    40,    43,   220,    29,    30,    32,    44,    45,   158,
     159,   160,   162,   163,   167,   168,   169,   172,   173,   174,
     179,   180,    30,   156,   157,   158,     3,     3,   248,     3,
     251,   252,   166,    16,   227,   228,     0,   198,   308,    20,
      22,    24,   244,     8,   229,   231,    74,   307,   307,   307,
     307,   307,   309,   248,    74,   306,   306,   306,   306,   306,
     197,    14,   248,    78,    79,   195,     3,     3,     3,   205,
     206,   216,   217,   221,   224,   225,   226,   255,   256,   257,
     258,   259,     3,   248,     6,   170,   171,   170,   171,     3,
     173,   162,   248,    64,    57,   199,     6,   157,   173,   174,
     175,   196,   196,   204,    21,   195,   230,   231,    67,   238,
      69,   232,    75,     3,   248,   248,   248,     3,    64,   195,
     218,    76,     3,   248,   248,   248,     3,     3,     3,   222,
     223,    68,   241,     4,   305,   305,     3,     4,     5,     6,
      75,    76,    84,    86,   102,   103,   104,   108,   132,   133,
     134,   154,   177,   179,   181,   188,   189,   190,   193,   195,
     261,   263,   264,   265,   267,   268,   269,   270,   271,   272,
     275,   276,   277,   278,   279,   281,   282,   283,   284,   285,
     287,   288,   289,   290,   291,   292,   293,   294,   297,   298,
     299,   300,   301,   302,     3,     5,     6,    64,   164,     3,
       5,     6,    64,   164,     3,     5,     6,    64,   164,    42,
      46,    47,    51,    52,     3,     3,     6,   248,   195,   252,
     305,     6,     6,   230,   231,   261,    55,    70,   236,    76,
      57,   195,   218,   248,     3,   215,    34,   228,    64,   185,
     199,   241,   264,    80,    80,   195,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,    75,    76,
     265,   195,   195,    89,   264,   280,     4,     4,     4,     4,
       6,   302,   195,   120,   122,   123,   124,   195,   195,   265,
     265,     5,     6,   194,   285,   295,   296,   228,   264,   196,
     199,    57,   152,   153,    75,    77,   130,   147,   148,   149,
     150,   151,   155,   185,   186,   187,   188,   189,   190,   191,
     192,   197,   194,   199,   194,   199,   194,   199,   194,   199,
     194,   199,     3,     6,    80,   310,   229,    42,    52,    29,
      45,   196,   196,    78,   239,   233,   234,   264,   264,    71,
     237,   226,     3,   126,   128,   207,   208,   209,   214,    57,
     195,   314,   315,   196,   199,   195,   262,   248,   264,   223,
     195,   195,    67,   196,   261,   195,    75,   228,   264,   264,
     280,    85,    87,    89,     4,   195,   195,   195,   195,     4,
       4,   200,   196,   196,    79,   263,     3,   264,   264,    77,
     155,   195,    75,   129,   265,   265,   265,   265,   265,   265,
     265,   265,   265,   265,   265,   265,   265,   265,     3,   190,
     285,     6,   295,     6,   296,     6,     5,    46,    48,    49,
     195,   196,     6,     6,   195,   245,   246,   247,   248,   253,
     176,   240,   199,    72,    73,   235,   264,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   108,   109,   110,   111,   112,   113,
     116,   119,   120,   123,   124,   125,   210,   127,   195,   196,
     199,   226,   215,   315,     3,   261,   199,    70,    71,    81,
      82,    83,   182,   303,   304,   303,   261,   196,   228,   196,
      57,    88,    85,    87,   264,   264,    78,   264,     4,     3,
     283,   264,   196,   199,   196,   199,     5,     6,   305,   195,
     265,   228,   261,   129,   152,   200,   200,     6,     6,     3,
     312,   313,    30,    32,   226,   199,    58,    60,    61,    62,
      63,    65,    66,   254,     3,    57,   249,   267,   268,   269,
     270,   271,   272,   273,   274,   241,   234,   195,   195,   195,
     195,   195,   195,    75,   126,   128,   129,   211,   212,   310,
     195,   215,    28,   311,   208,   196,   196,   195,     6,     6,
       4,     3,     6,   196,   199,   196,   196,   196,   210,   264,
     264,    85,    88,   265,   199,   199,   199,   199,     4,     4,
      80,   228,   261,   196,   196,   265,    53,    50,   185,   196,
     199,     6,   196,   246,    59,    59,     3,   199,    54,   243,
       6,    91,    92,    93,    94,    95,    98,    99,   107,   121,
      91,    92,    93,    94,    95,    98,    99,   107,   121,    91,
      92,    93,    94,    95,    98,    99,   121,    91,    92,    93,
      94,    95,    98,    99,   121,    91,    92,    93,    94,    95,
      98,    99,   121,   129,   127,   131,   212,   213,   213,   215,
     196,   195,   161,   261,   304,   196,    85,   264,   196,   193,
     297,     4,   285,   193,   286,   289,   294,   297,   196,   196,
     195,   196,   196,     6,     6,     3,     5,     6,   313,    42,
      46,    47,    52,   249,   247,   247,   195,   273,    55,    56,
     242,   196,   199,   199,   199,   199,   199,   199,   199,   199,
      93,   199,   199,   199,   199,   199,   199,   199,   199,    93,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   284,   196,   312,     3,   196,
       6,   199,   196,   199,   199,   199,   303,     6,     6,    64,
     215,   261,   264,     6,     6,     6,     6,     6,     6,     6,
       6,   199,     6,     6,     6,     6,     6,     6,     6,     6,
     199,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   196,   310,     4,     4,
       4,     4,   196,    46,    47,    48,    49,   264,   196,   196,
     196,   196,   196,   196,   196,   196,   196,     6,   196,   196,
     196,   196,   196,   196,   196,   196,     6,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   199,   196,   199,   196,   199,     6,     6,   196,
     196,     4,     4,   310,     6,    50,    51,   196,   199,   199,
     266,   196,     6,   310,     6,     4,   196,   310,   196,   310
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   201,   202,   203,   203,   204,   204,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   206,   206,   206,   206,   206,   206,   207,   207,   208,
     208,   209,   209,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   211,   211,   212,   212,   212,   212,   213,   213,   214,
     214,   215,   215,   216,   217,   217,   218,   218,   219,   220,
     220,   220,   220,   220,   220,   220,   220,   221,   222,   222,
     223,   224,   224,   224,   224,   224,   225,   225,   225,   226,
     226,   226,   226,   227,   227,   228,   229,   230,   230,   231,
     232,   232,   233,   233,   234,   235,   235,   235,   236,   236,
     237,   237,   238,   238,   239,   239,   240,   240,   241,   241,
     242,   242,   243,   243,   244,   244,   244,   244,   245,   245,
     246,   246,   247,   247,   248,   248,   249,   249,   249,   249,
     250,   250,   251,   251,   252,   253,   253,   254,   254,   254,
     254,   254,   254,   254,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   256,   256,   256,   257,   257,   258,
     258,   258,   258,   258,   258,   258,   258,   258,   258,   258,
     258,   258,   258,   258,   258,   258,   259,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   261,   261,   262,
     262,   263,   263,   264,   264,   264,   264,   264,   265,   265,
     265,   265,   265,   265,   265,   265,   265,   265,   265,   265,
     265,   266,   266,   267,   268,   268,   269,   269,   270,   270,
     271,   271,   272,   272,   273,   273,   273,   273,   273,   273,
     274,   274,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   276,   276,   277,   278,   278,
     279,   279,   279,   279,   280,   280,   281,   282,   282,   282,
     282,   283,   283,   283,   283,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   285,   285,   285,
     285,   286,   286,   286,   287,   288,   288,   289,   289,   290,
     291,   291,   292,   293,   293,   294,   295,   296,   297,   297,
     298,   299,   299,   300,   301,   301,   302,   302,   302,   302,
     302,   302,   302,   302,   302,   302,   302,   302,   303,   303,
     304,   304,   304,   304,   304,   304,   305,   306,   306,   307,
     307,   308,   308,   309,   309,   310,   310,   311,   311,   312,
     312,   313,   313,   313,   313,   314,   314,   315,   315
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     8,     6,     7,     6,     1,     3,     1,
       1,     4,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     4,     1,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     1,     2,     2,     1,     1,     2,     2,     0,     5,
       4,     1,     3,     4,     6,     5,     3,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     0,     5,     1,     3,
       3,     4,     4,     4,     4,     6,     8,    11,     8,     1,
       1,     3,     3,     3,     3,     2,     4,     3,     3,     8,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       2,     0,     3,     0,     1,     2,     1,     1,     1,     3,
       1,     1,     2,     4,     1,     3,     2,     1,     5,     0,
       2,     0,     1,     3,     5,     4,     6,     1,     1,     1,
       1,     1,     1,     0,     2,     2,     2,     2,     3,     2,
       2,     2,     4,     2,     3,     3,     3,     4,     4,     3,
       3,     4,     4,     5,     6,     7,     9,     4,     5,     7,
       9,     2,     3,     2,     2,     2,     2,     2,     5,     2,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     3,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,     9,
      10,    11,    12,     3,     4,     5,     6,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     0,    12,    14,    12,    12,    10,     7,     9,
       4,     6,     4,     6,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     4,     5,     4,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     4,     3,     3,     5,     5,     6,
       4,     6,     3,     5,     4,     5,     6,     4,     5,     5,
       6,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     3,     3,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     1,     3,
       2,     2,     1,     2,     2,     2,     1,     2,     0,     3,
       0,     1,     0,     2,     0,     4,     0,     4,     0,     1,
       3,     1,     3,     3,     3,     1,     2,     6,     3
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
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2247 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2255 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 226 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2269 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 216 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2283 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2294 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2302 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2311 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2320 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 276 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2334 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 269 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2345 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2355 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2365 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2375 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2385 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2395 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2405 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 259 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2419 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 259 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2433 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2443 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2451 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2459 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2468 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2476 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2484 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2492 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2506 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2515 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2524 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2533 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 296 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2546 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2569 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2583 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2593 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2602 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2616 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 246 "parser.y"
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
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2641 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2649 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2657 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2665 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2673 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2681 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2689 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2697 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2705 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2713 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2721 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2735 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2743 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2751 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2788 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2796 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2804 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2812 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2892 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2900 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 366 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2908 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 370 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2916 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2924 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2932 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 209 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2997 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3011 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 3025 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list_one_pack: /* index_info_list_one_pack  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 3039 "parser.cpp"
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
#line 87 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 3147 "parser.cpp"

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
#line 496 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3362 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 500 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3373 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 506 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3384 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 513 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3390 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3396 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3402 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 516 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3408 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3414 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3420 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3426 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 520 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3432 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3438 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 522 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3444 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 523 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3450 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 524 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3456 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 525 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3462 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 526 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3468 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 528 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3474 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3480 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3486 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3492 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3498 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3504 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3510 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3516 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3522 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 537 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3528 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 538 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3534 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3540 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 546 "parser.y"
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
#line 3560 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 563 "parser.y"
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
#line 3578 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 579 "parser.y"
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
#line 3611 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 608 "parser.y"
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
#line 3631 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 624 "parser.y"
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
#line 3652 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 642 "parser.y"
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
#line 3685 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 671 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3694 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 675 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3703 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 681 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3711 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 684 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3719 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 691 "parser.y"
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
#line 3774 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 741 "parser.y"
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
#line 3813 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 777 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3819 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3825 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 779 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3831 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3837 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 781 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3843 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 782 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3849 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 783 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3855 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 784 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3861 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 785 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3867 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 786 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3873 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 787 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3879 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 788 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3885 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 789 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3891 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 790 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3897 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 791 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3903 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 792 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3909 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3915 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 794 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3921 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3927 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 796 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3933 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 797 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3939 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 800 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3945 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 802 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3951 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3957 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 804 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3963 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 805 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3969 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3975 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3981 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3987 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3993 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3999 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4005 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4011 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4017 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4023 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4029 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4035 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4041 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4047 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4053 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4059 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4065 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4071 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4077 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4083 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4089 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4095 "parser.cpp"
    break;

  case 90: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4101 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4107 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4113 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4119 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4125 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4131 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4137 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4143 "parser.cpp"
    break;

  case 98: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4149 "parser.cpp"
    break;

  case 99: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4155 "parser.cpp"
    break;

  case 100: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4161 "parser.cpp"
    break;

  case 101: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4167 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4173 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4179 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4185 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4191 "parser.cpp"
    break;

  case 106: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4197 "parser.cpp"
    break;

  case 107: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4203 "parser.cpp"
    break;

  case 108: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4209 "parser.cpp"
    break;

  case 109: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4215 "parser.cpp"
    break;

  case 110: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4221 "parser.cpp"
    break;

  case 111: /* column_constraints: column_constraint  */
#line 868 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4230 "parser.cpp"
    break;

  case 112: /* column_constraints: column_constraints column_constraint  */
#line 872 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4244 "parser.cpp"
    break;

  case 113: /* column_constraint: PRIMARY KEY  */
#line 882 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4252 "parser.cpp"
    break;

  case 114: /* column_constraint: UNIQUE  */
#line 885 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4260 "parser.cpp"
    break;

  case 115: /* column_constraint: NULLABLE  */
#line 888 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4268 "parser.cpp"
    break;

  case 116: /* column_constraint: NOT NULLABLE  */
#line 891 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4276 "parser.cpp"
    break;

  case 117: /* default_expr: DEFAULT constant_expr  */
#line 895 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4284 "parser.cpp"
    break;

  case 118: /* default_expr: %empty  */
#line 898 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4292 "parser.cpp"
    break;

  case 119: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 903 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4302 "parser.cpp"
    break;

  case 120: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 908 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4312 "parser.cpp"
    break;

  case 121: /* identifier_array: IDENTIFIER  */
#line 915 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4323 "parser.cpp"
    break;

  case 122: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 921 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4334 "parser.cpp"
    break;

  case 123: /* delete_statement: DELETE FROM table_name where_clause  */
#line 931 "parser.y"
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
#line 4351 "parser.cpp"
    break;

  case 124: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 947 "parser.y"
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
#line 4390 "parser.cpp"
    break;

  case 125: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 981 "parser.y"
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
#line 4407 "parser.cpp"
    break;

  case 126: /* optional_identifier_array: '(' identifier_array ')'  */
#line 994 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4415 "parser.cpp"
    break;

  case 127: /* optional_identifier_array: %empty  */
#line 997 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4423 "parser.cpp"
    break;

  case 128: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1004 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4433 "parser.cpp"
    break;

  case 129: /* explain_type: ANALYZE  */
#line 1010 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4441 "parser.cpp"
    break;

  case 130: /* explain_type: AST  */
#line 1013 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4449 "parser.cpp"
    break;

  case 131: /* explain_type: RAW  */
#line 1016 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4457 "parser.cpp"
    break;

  case 132: /* explain_type: LOGICAL  */
#line 1019 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4465 "parser.cpp"
    break;

  case 133: /* explain_type: PHYSICAL  */
#line 1022 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4473 "parser.cpp"
    break;

  case 134: /* explain_type: PIPELINE  */
#line 1025 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4481 "parser.cpp"
    break;

  case 135: /* explain_type: FRAGMENT  */
#line 1028 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4489 "parser.cpp"
    break;

  case 136: /* explain_type: %empty  */
#line 1031 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4497 "parser.cpp"
    break;

  case 137: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1038 "parser.y"
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
#line 4514 "parser.cpp"
    break;

  case 138: /* update_expr_array: update_expr  */
#line 1051 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4523 "parser.cpp"
    break;

  case 139: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1055 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4532 "parser.cpp"
    break;

  case 140: /* update_expr: IDENTIFIER '=' expr  */
#line 1060 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4544 "parser.cpp"
    break;

  case 141: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1073 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4560 "parser.cpp"
    break;

  case 142: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1086 "parser.y"
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
#line 4578 "parser.cpp"
    break;

  case 143: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1101 "parser.y"
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
#line 4596 "parser.cpp"
    break;

  case 144: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1116 "parser.y"
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
#line 4614 "parser.cpp"
    break;

  case 145: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1131 "parser.y"
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
#line 4637 "parser.cpp"
    break;

  case 146: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1154 "parser.y"
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
#line 4695 "parser.cpp"
    break;

  case 147: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1207 "parser.y"
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
#line 4755 "parser.cpp"
    break;

  case 148: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1262 "parser.y"
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
#line 4807 "parser.cpp"
    break;

  case 149: /* select_statement: select_without_paren  */
#line 1313 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4815 "parser.cpp"
    break;

  case 150: /* select_statement: select_with_paren  */
#line 1316 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4823 "parser.cpp"
    break;

  case 151: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
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
#line 4837 "parser.cpp"
    break;

  case 152: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1328 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4851 "parser.cpp"
    break;

  case 153: /* select_with_paren: '(' select_without_paren ')'  */
#line 1338 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4859 "parser.cpp"
    break;

  case 154: /* select_with_paren: '(' select_with_paren ')'  */
#line 1341 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4867 "parser.cpp"
    break;

  case 155: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1345 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4876 "parser.cpp"
    break;

  case 156: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1350 "parser.y"
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
#line 4902 "parser.cpp"
    break;

  case 157: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1372 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4910 "parser.cpp"
    break;

  case 158: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1375 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4918 "parser.cpp"
    break;

  case 159: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1380 "parser.y"
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
#line 4938 "parser.cpp"
    break;

  case 160: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1396 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4946 "parser.cpp"
    break;

  case 161: /* order_by_clause: %empty  */
#line 1399 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4954 "parser.cpp"
    break;

  case 162: /* order_by_expr_list: order_by_expr  */
#line 1403 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4963 "parser.cpp"
    break;

  case 163: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1407 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4972 "parser.cpp"
    break;

  case 164: /* order_by_expr: expr order_by_type  */
#line 1412 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4982 "parser.cpp"
    break;

  case 165: /* order_by_type: ASC  */
#line 1418 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4990 "parser.cpp"
    break;

  case 166: /* order_by_type: DESC  */
#line 1421 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4998 "parser.cpp"
    break;

  case 167: /* order_by_type: %empty  */
#line 1424 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5006 "parser.cpp"
    break;

  case 168: /* limit_expr: LIMIT expr  */
#line 1428 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5014 "parser.cpp"
    break;

  case 169: /* limit_expr: %empty  */
#line 1432 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5020 "parser.cpp"
    break;

  case 170: /* offset_expr: OFFSET expr  */
#line 1434 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5028 "parser.cpp"
    break;

  case 171: /* offset_expr: %empty  */
#line 1438 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5034 "parser.cpp"
    break;

  case 172: /* distinct: DISTINCT  */
#line 1440 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5042 "parser.cpp"
    break;

  case 173: /* distinct: %empty  */
#line 1443 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5050 "parser.cpp"
    break;

  case 174: /* from_clause: FROM table_reference  */
#line 1447 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5058 "parser.cpp"
    break;

  case 175: /* from_clause: %empty  */
#line 1450 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5066 "parser.cpp"
    break;

  case 176: /* search_clause: SEARCH sub_search_array  */
#line 1454 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5076 "parser.cpp"
    break;

  case 177: /* search_clause: %empty  */
#line 1459 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5084 "parser.cpp"
    break;

  case 178: /* where_clause: WHERE expr  */
#line 1463 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5092 "parser.cpp"
    break;

  case 179: /* where_clause: %empty  */
#line 1466 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5100 "parser.cpp"
    break;

  case 180: /* having_clause: HAVING expr  */
#line 1470 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5108 "parser.cpp"
    break;

  case 181: /* having_clause: %empty  */
#line 1473 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5116 "parser.cpp"
    break;

  case 182: /* group_by_clause: GROUP BY expr_array  */
#line 1477 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5124 "parser.cpp"
    break;

  case 183: /* group_by_clause: %empty  */
#line 1480 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5132 "parser.cpp"
    break;

  case 184: /* set_operator: UNION  */
#line 1484 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5140 "parser.cpp"
    break;

  case 185: /* set_operator: UNION ALL  */
#line 1487 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5148 "parser.cpp"
    break;

  case 186: /* set_operator: INTERSECT  */
#line 1490 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5156 "parser.cpp"
    break;

  case 187: /* set_operator: EXCEPT  */
#line 1493 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5164 "parser.cpp"
    break;

  case 188: /* table_reference: table_reference_unit  */
#line 1501 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5172 "parser.cpp"
    break;

  case 189: /* table_reference: table_reference ',' table_reference_unit  */
#line 1504 "parser.y"
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
#line 5190 "parser.cpp"
    break;

  case 192: /* table_reference_name: table_name table_alias  */
#line 1521 "parser.y"
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
#line 5208 "parser.cpp"
    break;

  case 193: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1535 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5219 "parser.cpp"
    break;

  case 194: /* table_name: IDENTIFIER  */
#line 1544 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5229 "parser.cpp"
    break;

  case 195: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1549 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5241 "parser.cpp"
    break;

  case 196: /* table_alias: AS IDENTIFIER  */
#line 1558 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5251 "parser.cpp"
    break;

  case 197: /* table_alias: IDENTIFIER  */
#line 1563 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5261 "parser.cpp"
    break;

  case 198: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1568 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5272 "parser.cpp"
    break;

  case 199: /* table_alias: %empty  */
#line 1574 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5280 "parser.cpp"
    break;

  case 200: /* with_clause: WITH with_expr_list  */
#line 1581 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5288 "parser.cpp"
    break;

  case 201: /* with_clause: %empty  */
#line 1584 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5296 "parser.cpp"
    break;

  case 202: /* with_expr_list: with_expr  */
#line 1588 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5305 "parser.cpp"
    break;

  case 203: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1591 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5314 "parser.cpp"
    break;

  case 204: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1596 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5326 "parser.cpp"
    break;

  case 205: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1608 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5338 "parser.cpp"
    break;

  case 206: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1615 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5351 "parser.cpp"
    break;

  case 207: /* join_type: INNER  */
#line 1629 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5359 "parser.cpp"
    break;

  case 208: /* join_type: LEFT  */
#line 1632 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5367 "parser.cpp"
    break;

  case 209: /* join_type: RIGHT  */
#line 1635 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5375 "parser.cpp"
    break;

  case 210: /* join_type: OUTER  */
#line 1638 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5383 "parser.cpp"
    break;

  case 211: /* join_type: FULL  */
#line 1641 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5391 "parser.cpp"
    break;

  case 212: /* join_type: CROSS  */
#line 1644 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5399 "parser.cpp"
    break;

  case 213: /* join_type: %empty  */
#line 1647 "parser.y"
                {
}
#line 5406 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW DATABASES  */
#line 1653 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5415 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW TABLES  */
#line 1657 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5424 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW VIEWS  */
#line 1661 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5433 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW CONFIGS  */
#line 1665 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5442 "parser.cpp"
    break;

  case 218: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1669 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5454 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW PROFILES  */
#line 1676 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5463 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW BUFFER  */
#line 1680 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5472 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW QUERIES  */
#line 1684 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5481 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1688 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5491 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW TRANSACTIONS  */
#line 1693 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5500 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1697 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5510 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW SESSION VARIABLES  */
#line 1702 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5519 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1706 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5528 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1710 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5539 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1716 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5550 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1722 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5561 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW TABLE table_name  */
#line 1728 "parser.y"
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
#line 5577 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1739 "parser.y"
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
#line 5593 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1750 "parser.y"
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
#line 5609 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1761 "parser.y"
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
#line 5626 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1773 "parser.y"
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
#line 5643 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1785 "parser.y"
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
#line 5661 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1798 "parser.y"
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
#line 5680 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1812 "parser.y"
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
#line 5696 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1823 "parser.y"
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
#line 5715 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1837 "parser.y"
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
#line 5736 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1853 "parser.y"
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
#line 5758 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW LOGS  */
#line 1870 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5767 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW DELTA LOGS  */
#line 1874 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5776 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW CATALOGS  */
#line 1878 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5785 "parser.cpp"
    break;

  case 244: /* flush_statement: FLUSH DATA  */
#line 1886 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5794 "parser.cpp"
    break;

  case 245: /* flush_statement: FLUSH LOG  */
#line 1890 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5803 "parser.cpp"
    break;

  case 246: /* flush_statement: FLUSH BUFFER  */
#line 1894 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5812 "parser.cpp"
    break;

  case 247: /* optimize_statement: OPTIMIZE table_name  */
#line 1902 "parser.y"
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
#line 5827 "parser.cpp"
    break;

  case 248: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1913 "parser.y"
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
#line 5851 "parser.cpp"
    break;

  case 249: /* command_statement: USE IDENTIFIER  */
#line 1936 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5862 "parser.cpp"
    break;

  case 250: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1942 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5872 "parser.cpp"
    break;

  case 251: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1947 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5883 "parser.cpp"
    break;

  case 252: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1953 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5894 "parser.cpp"
    break;

  case 253: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1959 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5907 "parser.cpp"
    break;

  case 254: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1967 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5918 "parser.cpp"
    break;

  case 255: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1973 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5929 "parser.cpp"
    break;

  case 256: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1979 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5940 "parser.cpp"
    break;

  case 257: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1985 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5951 "parser.cpp"
    break;

  case 258: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1991 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5964 "parser.cpp"
    break;

  case 259: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1999 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5975 "parser.cpp"
    break;

  case 260: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2005 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5986 "parser.cpp"
    break;

  case 261: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2011 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5997 "parser.cpp"
    break;

  case 262: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2017 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6008 "parser.cpp"
    break;

  case 263: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2023 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6021 "parser.cpp"
    break;

  case 264: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2031 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6032 "parser.cpp"
    break;

  case 265: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2037 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6043 "parser.cpp"
    break;

  case 266: /* compact_statement: COMPACT TABLE table_name  */
#line 2044 "parser.y"
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
#line 6060 "parser.cpp"
    break;

  case 267: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2057 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6069 "parser.cpp"
    break;

  case 268: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2061 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6079 "parser.cpp"
    break;

  case 269: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASES  */
#line 2066 "parser.y"
                                          {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-1].long_value);
}
#line 6089 "parser.cpp"
    break;

  case 270: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE  */
#line 2071 "parser.y"
                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[0].long_value);
}
#line 6100 "parser.cpp"
    break;

  case 271: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLES  */
#line 2077 "parser.y"
                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-1].long_value);
}
#line 6111 "parser.cpp"
    break;

  case 272: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE  */
#line 2083 "parser.y"
                                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[0].long_value);
}
#line 6123 "parser.cpp"
    break;

  case 273: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENTS  */
#line 2090 "parser.y"
                                                                              {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSegments;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-1].long_value);
}
#line 6135 "parser.cpp"
    break;

  case 274: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2097 "parser.y"
                                                                                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSegment;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[0].long_value);
}
#line 6148 "parser.cpp"
    break;

  case 275: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2105 "parser.y"
                                                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListBlocks;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-1].long_value);
}
#line 6161 "parser.cpp"
    break;

  case 276: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2113 "parser.y"
                                                                                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowBlock;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[0].long_value);
}
#line 6175 "parser.cpp"
    break;

  case 277: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2122 "parser.y"
                                                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListColumns;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[-1].long_value);
}
#line 6189 "parser.cpp"
    break;

  case 278: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2131 "parser.y"
                                                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowColumn;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->column_index_ = (yyvsp[0].long_value);
}
#line 6204 "parser.cpp"
    break;

  case 279: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE INDEXES  */
#line 2141 "parser.y"
                                                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListIndexes;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-1].long_value);
}
#line 6216 "parser.cpp"
    break;

  case 280: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE INDEX LONG_VALUE  */
#line 2148 "parser.y"
                                                                                      {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndex;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[0].long_value);
}
#line 6229 "parser.cpp"
    break;

  case 281: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE INDEX LONG_VALUE SEGMENTS  */
#line 2156 "parser.y"
                                                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListIndexSegments;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[-1].long_value);
}
#line 6242 "parser.cpp"
    break;

  case 282: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE DATABASE LONG_VALUE TABLE LONG_VALUE INDEX LONG_VALUE SEGMENT LONG_VALUE  */
#line 2164 "parser.y"
                                                                                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndexSegment;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[0].long_value);
}
#line 6256 "parser.cpp"
    break;

  case 283: /* admin_statement: ADMIN SHOW LOGS  */
#line 2173 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6265 "parser.cpp"
    break;

  case 284: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2177 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6275 "parser.cpp"
    break;

  case 285: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2182 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6285 "parser.cpp"
    break;

  case 286: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2187 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6296 "parser.cpp"
    break;

  case 287: /* expr_array: expr_alias  */
#line 2198 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6305 "parser.cpp"
    break;

  case 288: /* expr_array: expr_array ',' expr_alias  */
#line 2202 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6314 "parser.cpp"
    break;

  case 289: /* expr_array_list: '(' expr_array ')'  */
#line 2207 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6323 "parser.cpp"
    break;

  case 290: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2211 "parser.y"
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
#line 6343 "parser.cpp"
    break;

  case 291: /* expr_alias: expr AS IDENTIFIER  */
#line 2238 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6354 "parser.cpp"
    break;

  case 292: /* expr_alias: expr  */
#line 2244 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6362 "parser.cpp"
    break;

  case 298: /* operand: '(' expr ')'  */
#line 2254 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6370 "parser.cpp"
    break;

  case 299: /* operand: '(' select_without_paren ')'  */
#line 2257 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6381 "parser.cpp"
    break;

  case 300: /* operand: constant_expr  */
#line 2263 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6389 "parser.cpp"
    break;

  case 311: /* extra_match_tensor_option: ',' STRING  */
#line 2277 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6397 "parser.cpp"
    break;

  case 312: /* extra_match_tensor_option: %empty  */
#line 2280 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6405 "parser.cpp"
    break;

  case 313: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2286 "parser.y"
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
#line 6426 "parser.cpp"
    break;

  case 314: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2305 "parser.y"
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
#line 6472 "parser.cpp"
    break;

  case 315: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2347 "parser.y"
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
#line 6519 "parser.cpp"
    break;

  case 316: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2393 "parser.y"
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
#line 6541 "parser.cpp"
    break;

  case 317: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2411 "parser.y"
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
#line 6563 "parser.cpp"
    break;

  case 318: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2429 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6576 "parser.cpp"
    break;

  case 319: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2437 "parser.y"
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
#line 6591 "parser.cpp"
    break;

  case 320: /* query_expr: QUERY '(' STRING ')'  */
#line 2448 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6602 "parser.cpp"
    break;

  case 321: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2454 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6615 "parser.cpp"
    break;

  case 322: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2463 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6626 "parser.cpp"
    break;

  case 323: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2469 "parser.y"
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
#line 6642 "parser.cpp"
    break;

  case 324: /* sub_search: match_vector_expr  */
#line 2481 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6650 "parser.cpp"
    break;

  case 325: /* sub_search: match_text_expr  */
#line 2484 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6658 "parser.cpp"
    break;

  case 326: /* sub_search: match_tensor_expr  */
#line 2487 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6666 "parser.cpp"
    break;

  case 327: /* sub_search: match_sparse_expr  */
#line 2490 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6674 "parser.cpp"
    break;

  case 328: /* sub_search: query_expr  */
#line 2493 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6682 "parser.cpp"
    break;

  case 329: /* sub_search: fusion_expr  */
#line 2496 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6690 "parser.cpp"
    break;

  case 330: /* sub_search_array: sub_search  */
#line 2500 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6699 "parser.cpp"
    break;

  case 331: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2504 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6708 "parser.cpp"
    break;

  case 332: /* function_expr: IDENTIFIER '(' ')'  */
#line 2509 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6721 "parser.cpp"
    break;

  case 333: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2517 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6734 "parser.cpp"
    break;

  case 334: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2525 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6748 "parser.cpp"
    break;

  case 335: /* function_expr: operand IS NOT NULLABLE  */
#line 2534 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6760 "parser.cpp"
    break;

  case 336: /* function_expr: operand IS NULLABLE  */
#line 2541 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6772 "parser.cpp"
    break;

  case 337: /* function_expr: NOT operand  */
#line 2548 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6784 "parser.cpp"
    break;

  case 338: /* function_expr: '-' operand  */
#line 2555 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6796 "parser.cpp"
    break;

  case 339: /* function_expr: '+' operand  */
#line 2562 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6808 "parser.cpp"
    break;

  case 340: /* function_expr: operand '-' operand  */
#line 2569 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6821 "parser.cpp"
    break;

  case 341: /* function_expr: operand '+' operand  */
#line 2577 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6834 "parser.cpp"
    break;

  case 342: /* function_expr: operand '*' operand  */
#line 2585 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6847 "parser.cpp"
    break;

  case 343: /* function_expr: operand '/' operand  */
#line 2593 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6860 "parser.cpp"
    break;

  case 344: /* function_expr: operand '%' operand  */
#line 2601 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6873 "parser.cpp"
    break;

  case 345: /* function_expr: operand '=' operand  */
#line 2609 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6886 "parser.cpp"
    break;

  case 346: /* function_expr: operand EQUAL operand  */
#line 2617 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6899 "parser.cpp"
    break;

  case 347: /* function_expr: operand NOT_EQ operand  */
#line 2625 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6912 "parser.cpp"
    break;

  case 348: /* function_expr: operand '<' operand  */
#line 2633 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6925 "parser.cpp"
    break;

  case 349: /* function_expr: operand '>' operand  */
#line 2641 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6938 "parser.cpp"
    break;

  case 350: /* function_expr: operand LESS_EQ operand  */
#line 2649 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6951 "parser.cpp"
    break;

  case 351: /* function_expr: operand GREATER_EQ operand  */
#line 2657 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6964 "parser.cpp"
    break;

  case 352: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2665 "parser.y"
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
#line 6999 "parser.cpp"
    break;

  case 353: /* function_expr: operand LIKE operand  */
#line 2695 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7012 "parser.cpp"
    break;

  case 354: /* function_expr: operand NOT LIKE operand  */
#line 2703 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7025 "parser.cpp"
    break;

  case 355: /* conjunction_expr: expr AND expr  */
#line 2712 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7038 "parser.cpp"
    break;

  case 356: /* conjunction_expr: expr OR expr  */
#line 2720 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7051 "parser.cpp"
    break;

  case 357: /* between_expr: operand BETWEEN operand AND operand  */
#line 2729 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7063 "parser.cpp"
    break;

  case 358: /* in_expr: operand IN '(' expr_array ')'  */
#line 2737 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7074 "parser.cpp"
    break;

  case 359: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2743 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7085 "parser.cpp"
    break;

  case 360: /* case_expr: CASE expr case_check_array END  */
#line 2750 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7096 "parser.cpp"
    break;

  case 361: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2756 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7108 "parser.cpp"
    break;

  case 362: /* case_expr: CASE case_check_array END  */
#line 2763 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7118 "parser.cpp"
    break;

  case 363: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2768 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7129 "parser.cpp"
    break;

  case 364: /* case_check_array: WHEN expr THEN expr  */
#line 2775 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7141 "parser.cpp"
    break;

  case 365: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2782 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7153 "parser.cpp"
    break;

  case 366: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2790 "parser.y"
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
#line 7181 "parser.cpp"
    break;

  case 367: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2814 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7192 "parser.cpp"
    break;

  case 368: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2820 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7203 "parser.cpp"
    break;

  case 369: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2826 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7215 "parser.cpp"
    break;

  case 370: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2833 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7227 "parser.cpp"
    break;

  case 371: /* column_expr: IDENTIFIER  */
#line 2841 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7239 "parser.cpp"
    break;

  case 372: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2848 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7251 "parser.cpp"
    break;

  case 373: /* column_expr: '*'  */
#line 2855 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7261 "parser.cpp"
    break;

  case 374: /* column_expr: column_expr '.' '*'  */
#line 2860 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7275 "parser.cpp"
    break;

  case 375: /* constant_expr: STRING  */
#line 2870 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7285 "parser.cpp"
    break;

  case 376: /* constant_expr: TRUE  */
#line 2875 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7295 "parser.cpp"
    break;

  case 377: /* constant_expr: FALSE  */
#line 2880 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7305 "parser.cpp"
    break;

  case 378: /* constant_expr: DOUBLE_VALUE  */
#line 2885 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7315 "parser.cpp"
    break;

  case 379: /* constant_expr: LONG_VALUE  */
#line 2890 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7325 "parser.cpp"
    break;

  case 380: /* constant_expr: DATE STRING  */
#line 2895 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7335 "parser.cpp"
    break;

  case 381: /* constant_expr: TIME STRING  */
#line 2900 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7345 "parser.cpp"
    break;

  case 382: /* constant_expr: DATETIME STRING  */
#line 2905 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7355 "parser.cpp"
    break;

  case 383: /* constant_expr: TIMESTAMP STRING  */
#line 2910 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7365 "parser.cpp"
    break;

  case 384: /* constant_expr: INTERVAL interval_expr  */
#line 2915 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7373 "parser.cpp"
    break;

  case 385: /* constant_expr: interval_expr  */
#line 2918 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7381 "parser.cpp"
    break;

  case 386: /* constant_expr: common_array_expr  */
#line 2921 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7389 "parser.cpp"
    break;

  case 387: /* common_array_expr: array_expr  */
#line 2925 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7397 "parser.cpp"
    break;

  case 388: /* common_array_expr: subarray_array_expr  */
#line 2928 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7405 "parser.cpp"
    break;

  case 389: /* common_array_expr: sparse_array_expr  */
#line 2931 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7413 "parser.cpp"
    break;

  case 390: /* common_array_expr: empty_array_expr  */
#line 2934 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7421 "parser.cpp"
    break;

  case 391: /* common_sparse_array_expr: sparse_array_expr  */
#line 2938 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7429 "parser.cpp"
    break;

  case 392: /* common_sparse_array_expr: array_expr  */
#line 2941 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7437 "parser.cpp"
    break;

  case 393: /* common_sparse_array_expr: empty_array_expr  */
#line 2944 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7445 "parser.cpp"
    break;

  case 394: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2948 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7453 "parser.cpp"
    break;

  case 395: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2952 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7463 "parser.cpp"
    break;

  case 396: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2957 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7472 "parser.cpp"
    break;

  case 397: /* sparse_array_expr: long_sparse_array_expr  */
#line 2962 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7480 "parser.cpp"
    break;

  case 398: /* sparse_array_expr: double_sparse_array_expr  */
#line 2965 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7488 "parser.cpp"
    break;

  case 399: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 2969 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7496 "parser.cpp"
    break;

  case 400: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 2973 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7508 "parser.cpp"
    break;

  case 401: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 2980 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7519 "parser.cpp"
    break;

  case 402: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 2987 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7527 "parser.cpp"
    break;

  case 403: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 2991 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7539 "parser.cpp"
    break;

  case 404: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 2998 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7550 "parser.cpp"
    break;

  case 405: /* empty_array_expr: '[' ']'  */
#line 3005 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7558 "parser.cpp"
    break;

  case 406: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3009 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7566 "parser.cpp"
    break;

  case 407: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3013 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7574 "parser.cpp"
    break;

  case 408: /* array_expr: long_array_expr  */
#line 3017 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7582 "parser.cpp"
    break;

  case 409: /* array_expr: double_array_expr  */
#line 3020 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7590 "parser.cpp"
    break;

  case 410: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3024 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7598 "parser.cpp"
    break;

  case 411: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3028 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7608 "parser.cpp"
    break;

  case 412: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3033 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7617 "parser.cpp"
    break;

  case 413: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3038 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7625 "parser.cpp"
    break;

  case 414: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3042 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7635 "parser.cpp"
    break;

  case 415: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3047 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7644 "parser.cpp"
    break;

  case 416: /* interval_expr: LONG_VALUE SECONDS  */
#line 3052 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7655 "parser.cpp"
    break;

  case 417: /* interval_expr: LONG_VALUE SECOND  */
#line 3058 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7666 "parser.cpp"
    break;

  case 418: /* interval_expr: LONG_VALUE MINUTES  */
#line 3064 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7677 "parser.cpp"
    break;

  case 419: /* interval_expr: LONG_VALUE MINUTE  */
#line 3070 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7688 "parser.cpp"
    break;

  case 420: /* interval_expr: LONG_VALUE HOURS  */
#line 3076 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7699 "parser.cpp"
    break;

  case 421: /* interval_expr: LONG_VALUE HOUR  */
#line 3082 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7710 "parser.cpp"
    break;

  case 422: /* interval_expr: LONG_VALUE DAYS  */
#line 3088 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7721 "parser.cpp"
    break;

  case 423: /* interval_expr: LONG_VALUE DAY  */
#line 3094 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7732 "parser.cpp"
    break;

  case 424: /* interval_expr: LONG_VALUE MONTHS  */
#line 3100 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7743 "parser.cpp"
    break;

  case 425: /* interval_expr: LONG_VALUE MONTH  */
#line 3106 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7754 "parser.cpp"
    break;

  case 426: /* interval_expr: LONG_VALUE YEARS  */
#line 3112 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7765 "parser.cpp"
    break;

  case 427: /* interval_expr: LONG_VALUE YEAR  */
#line 3118 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7776 "parser.cpp"
    break;

  case 428: /* copy_option_list: copy_option  */
#line 3129 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7785 "parser.cpp"
    break;

  case 429: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3133 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7794 "parser.cpp"
    break;

  case 430: /* copy_option: FORMAT IDENTIFIER  */
#line 3138 "parser.y"
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
#line 7829 "parser.cpp"
    break;

  case 431: /* copy_option: DELIMITER STRING  */
#line 3168 "parser.y"
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
#line 7844 "parser.cpp"
    break;

  case 432: /* copy_option: HEADER  */
#line 3178 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7854 "parser.cpp"
    break;

  case 433: /* copy_option: OFFSET LONG_VALUE  */
#line 3183 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 7864 "parser.cpp"
    break;

  case 434: /* copy_option: LIMIT LONG_VALUE  */
#line 3188 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 7874 "parser.cpp"
    break;

  case 435: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3193 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 7884 "parser.cpp"
    break;

  case 436: /* file_path: STRING  */
#line 3199 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7892 "parser.cpp"
    break;

  case 437: /* if_exists: IF EXISTS  */
#line 3203 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7898 "parser.cpp"
    break;

  case 438: /* if_exists: %empty  */
#line 3204 "parser.y"
  { (yyval.bool_value) = false; }
#line 7904 "parser.cpp"
    break;

  case 439: /* if_not_exists: IF NOT EXISTS  */
#line 3206 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7910 "parser.cpp"
    break;

  case 440: /* if_not_exists: %empty  */
#line 3207 "parser.y"
  { (yyval.bool_value) = false; }
#line 7916 "parser.cpp"
    break;

  case 443: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3222 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7929 "parser.cpp"
    break;

  case 444: /* if_not_exists_info: %empty  */
#line 3230 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7937 "parser.cpp"
    break;

  case 445: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3234 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7945 "parser.cpp"
    break;

  case 446: /* with_index_param_list: %empty  */
#line 3237 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7953 "parser.cpp"
    break;

  case 447: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3241 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7961 "parser.cpp"
    break;

  case 448: /* optional_table_properties_list: %empty  */
#line 3244 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7969 "parser.cpp"
    break;

  case 449: /* index_param_list: index_param  */
#line 3248 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7978 "parser.cpp"
    break;

  case 450: /* index_param_list: index_param_list ',' index_param  */
#line 3252 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7987 "parser.cpp"
    break;

  case 451: /* index_param: IDENTIFIER  */
#line 3257 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7998 "parser.cpp"
    break;

  case 452: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3263 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8013 "parser.cpp"
    break;

  case 453: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3273 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8025 "parser.cpp"
    break;

  case 454: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3280 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8037 "parser.cpp"
    break;

  case 455: /* index_info_list: index_info_list_one_pack  */
#line 3291 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8046 "parser.cpp"
    break;

  case 456: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3295 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8058 "parser.cpp"
    break;

  case 457: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3303 "parser.y"
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
#line 8117 "parser.cpp"
    break;

  case 458: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3357 "parser.y"
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
#line 8135 "parser.cpp"
    break;


#line 8139 "parser.cpp"

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

#line 3371 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

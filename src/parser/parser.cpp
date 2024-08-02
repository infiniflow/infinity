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
#define YYLAST   1195

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  201
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  458
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  953

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
    2072,  2079,  2087,  2096,  2106,  2117,  2128,  2140,  2152,  2165,
    2175,  2186,  2197,  2209,  2213,  2218,  2223,  2234,  2238,  2243,
    2247,  2274,  2280,  2284,  2285,  2286,  2287,  2288,  2290,  2293,
    2299,  2302,  2303,  2304,  2305,  2306,  2307,  2308,  2309,  2310,
    2311,  2313,  2316,  2322,  2341,  2383,  2429,  2447,  2465,  2473,
    2484,  2490,  2499,  2505,  2517,  2520,  2523,  2526,  2529,  2532,
    2536,  2540,  2545,  2553,  2561,  2570,  2577,  2584,  2591,  2598,
    2605,  2613,  2621,  2629,  2637,  2645,  2653,  2661,  2669,  2677,
    2685,  2693,  2701,  2731,  2739,  2748,  2756,  2765,  2773,  2779,
    2786,  2792,  2799,  2804,  2811,  2818,  2826,  2850,  2856,  2862,
    2869,  2877,  2884,  2891,  2896,  2906,  2911,  2916,  2921,  2926,
    2931,  2936,  2941,  2946,  2951,  2954,  2957,  2961,  2964,  2967,
    2970,  2974,  2977,  2980,  2984,  2988,  2993,  2998,  3001,  3005,
    3009,  3016,  3023,  3027,  3034,  3041,  3045,  3049,  3053,  3056,
    3060,  3064,  3069,  3074,  3078,  3083,  3088,  3094,  3100,  3106,
    3112,  3118,  3124,  3130,  3136,  3142,  3148,  3154,  3165,  3169,
    3174,  3204,  3214,  3219,  3224,  3229,  3235,  3239,  3240,  3242,
    3243,  3245,  3246,  3258,  3266,  3270,  3273,  3277,  3280,  3284,
    3288,  3293,  3299,  3309,  3316,  3327,  3331,  3339,  3393
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

#define YYPACT_NINF (-522)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-445)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     536,   339,     5,   402,    53,   -13,    53,   128,   660,   582,
      61,   524,   108,   176,   212,    57,   222,     2,   255,    63,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   404,  -522,
    -522,   290,  -522,  -522,  -522,  -522,  -522,  -522,   236,   236,
     236,   236,   211,    53,   241,   241,   241,   241,   241,    79,
     313,    53,   -49,   345,   349,   356,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,    74,   368,    53,  -522,  -522,  -522,  -522,
    -522,   370,   111,   132,  -522,   380,  -522,   171,  -522,  -522,
     260,  -522,    53,  -522,  -522,  -522,  -522,   -42,  -522,   372,
     254,  -522,   462,   172,   309,   311,  -522,    34,  -522,   491,
    -522,  -522,     3,   453,  -522,   460,   461,   539,    53,    53,
      53,   553,   496,   369,   490,   565,    53,    53,    53,   567,
     578,   586,   527,   592,   592,   390,    66,    70,    99,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,   404,  -522,  -522,  -522,
    -522,  -522,  -522,   400,  -522,  -522,   596,  -522,   600,  -522,
    -522,   603,  -522,    53,   418,   212,   592,   611,  -522,  -522,
     616,  -522,  -522,  -522,  -522,     3,  -522,  -522,  -522,   390,
     568,   559,   558,  -522,   -39,  -522,   369,  -522,    53,   632,
     152,  -522,  -522,  -522,  -522,  -522,   576,  -522,   466,    -4,
    -522,   390,  -522,  -522,   575,   598,   471,  -522,  -522,   722,
     498,   477,   495,   184,   652,   698,   700,   701,  -522,  -522,
     702,   511,   296,   512,   514,   535,   535,  -522,    15,   337,
    -122,  -522,    84,   617,  -522,  -522,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,   513,  -522,  -522,
    -522,  -131,  -522,  -522,  -105,  -522,   -96,  -522,  -522,  -522,
      59,  -522,    68,  -522,  -522,  -522,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   708,
     707,  -522,  -522,  -522,  -522,  -522,  -522,   637,   290,  -522,
    -522,   237,   714,   525,   526,   -51,   390,   390,   655,  -522,
       2,   107,   670,   534,  -522,   -81,   537,  -522,    53,   390,
     586,  -522,   214,   538,   551,   118,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   535,   557,
     664,   654,   390,   390,   167,   421,  -522,  -522,  -522,  -522,
     722,  -522,   744,   562,   563,   564,   574,   749,   756,   493,
     493,  -522,   571,  -522,  -522,  -522,  -522,   585,   -46,   684,
     390,   779,   390,   390,   -15,   590,   -17,   535,   535,   535,
     535,   535,   535,   535,   535,   535,   535,   535,   535,   535,
     535,    10,  -522,   593,  -522,   777,  -522,   785,  -522,   787,
    -522,   791,   751,   341,   604,  -522,   602,   794,  -522,   235,
    -522,  -522,    11,   625,   618,  -522,     7,   214,   390,  -522,
     404,   789,   683,   620,   -57,  -522,  -522,  -522,     2,   632,
     534,  -522,  -522,   815,   390,   621,  -522,   214,  -522,    46,
      46,   390,  -522,   179,   654,   666,   626,    92,    96,   426,
    -522,   390,   390,   745,   390,   820,    29,   390,   202,   264,
     443,  -522,  -522,   592,  -522,  -522,  -522,   685,   643,   535,
     337,   710,  -522,   686,   686,   144,   144,   640,   686,   686,
     144,   144,   493,   493,  -522,  -522,  -522,  -522,  -522,  -522,
     642,  -522,   644,  -522,  -522,  -522,   834,   837,  -522,   842,
    -522,  -522,   840,  -522,     2,   648,   599,  -522,    90,  -522,
     340,   527,   390,  -522,  -522,  -522,   214,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   653,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,
    -522,   674,   675,   709,   711,   712,   229,   715,   632,   868,
     107,   404,   268,  -522,  -522,   289,   716,   897,   909,   912,
     914,  -522,   913,   300,  -522,   301,   332,  -522,   724,  -522,
     789,   390,  -522,   390,   -33,   113,   535,   -86,   719,  -522,
     140,    72,  -522,   917,  -522,   918,  -522,  -522,   843,   337,
     686,   728,   338,  -522,   535,   919,   921,   874,   878,   746,
     358,  -522,   923,    -5,    11,   871,  -522,  -522,  -522,  -522,
    -522,  -522,   873,  -522,   930,  -522,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,   735,   881,  -522,   931,   293,   388,   363,
     499,   555,   807,   811,  -522,  -522,   225,  -522,   808,   632,
     359,   747,  -522,  -522,   780,  -522,   390,  -522,  -522,  -522,
    -522,  -522,  -522,    46,  -522,  -522,  -522,   748,   214,   125,
    -522,   390,   588,   750,   936,   593,   752,   753,   754,   757,
     755,   376,  -522,  -522,   664,   940,   941,   472,  -522,   842,
     503,    90,   599,    11,    11,   758,   340,   893,   898,   408,
     759,   760,   761,   762,   763,   764,   765,   862,   766,   767,
     768,   769,   770,   771,   772,   773,   863,   774,   775,   776,
     778,   781,   782,   783,   784,   786,   788,   790,   792,   793,
     795,   796,   797,   798,   799,   800,   801,   802,   803,   804,
     805,   806,  -522,  -522,    49,  -522,  -522,  -522,   409,  -522,
     842,   954,   419,  -522,  -522,  -522,   214,  -522,   467,   809,
     429,   810,    20,   812,  -522,  -522,  -522,  -522,  -522,    46,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   970,  -522,
    -522,  -522,   915,   632,  -522,   390,   390,  -522,  -522,   972,
     978,   980,   982,   984,   987,  1000,  1001,   813,  1004,  1007,
    1008,  1009,  1010,  1011,  1012,  1013,   821,  1015,  1016,  1017,
    1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,  1027,
    1028,  1029,  1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,
    1038,  1039,  -522,  -522,   437,   637,  -522,  -522,  1042,  -522,
    1043,  1044,  1045,   445,  1046,   390,   446,   851,   214,   855,
     857,   858,   859,   860,   861,   864,   865,  1052,   866,   867,
     869,   870,   872,   875,   876,   877,  1053,   879,   880,   882,
     883,   884,   885,   886,   887,   888,   889,   890,   891,   892,
     894,   895,   896,   899,   900,   901,   902,   903,   904,   905,
     906,   907,  -522,  -522,   908,   910,   911,   474,  -522,   357,
     214,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,
     916,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,   920,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,  -522,
    -522,  -522,  -522,  -522,  -522,  1060,  -522,  1063,   637,  1064,
    1068,  1071,  -522,  -522,  -522,  -522,   475,   922,  -522,   924,
     530,   441,   637,  1083,  1065,   926,   637,  1087,  -522,  1088,
    -522,  -522,   927,  -522,  -522,  -522,  -522,   355,   637,  1098,
    -522,  -522,  -522
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
     158,   157,     0,   177,   160,   162,   167,   168,     0,   156,
      34,     0,     0,     0,     0,    37,    39,    40,   201,     0,
      36,   455,   126,     0,     0,   124,   145,   140,   139,     0,
       0,     0,   332,     0,   201,     0,     0,     0,     0,     0,
     362,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   299,   298,     0,   288,   291,   355,   356,     0,     0,
     201,     0,   336,   346,   347,   350,   351,     0,   353,   345,
     348,   349,   341,   340,   342,   343,   344,   372,   374,   396,
       0,   401,     0,   404,   412,   415,     0,     0,   234,     0,
     204,   286,     0,   269,   201,   174,   188,   190,   199,   191,
       0,   179,     0,   165,   166,   164,   170,    43,    46,    47,
      44,    45,    48,    49,    65,    50,    52,    51,    68,    55,
      56,    57,    53,    54,    58,    59,    60,    61,    62,    63,
      64,     0,     0,     0,     0,     0,   446,     0,     0,   448,
       0,    35,     0,   456,   122,     0,     0,     0,     0,     0,
       0,   432,     0,     0,   428,     0,     0,   333,     0,   367,
       0,     0,   360,     0,     0,     0,     0,     0,     0,   371,
       0,     0,   320,     0,   322,     0,   407,   406,     0,   201,
     354,     0,     0,   335,     0,     0,     0,   239,   235,   451,
       0,   449,   270,     0,     0,     0,   208,   209,   210,   211,
     207,   212,     0,   197,     0,   192,   326,   324,   327,   325,
     328,   329,   330,   176,   183,   163,     0,     0,     0,     0,
       0,     0,     0,     0,   114,   115,   118,   111,   118,     0,
       0,     0,    33,    38,   458,   289,     0,   434,   433,   431,
     430,   435,   148,     0,   146,   334,   368,     0,   364,     0,
     363,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   369,   358,   357,     0,     0,     0,   445,     0,
       0,   199,   189,     0,     0,   196,     0,     0,   181,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   116,   113,     0,   112,    42,    41,     0,   120,
       0,     0,     0,   429,   366,   361,   365,   352,     0,     0,
       0,     0,     0,     0,   391,   393,   392,   321,   323,     0,
     370,   359,   240,   236,   452,   454,   453,   450,     0,   271,
     193,   205,     0,     0,   331,     0,     0,   159,    67,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   117,   119,     0,   446,   290,   411,     0,   318,
       0,     0,     0,     0,   272,     0,     0,   182,   180,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   447,   457,     0,     0,     0,     0,   147,     0,
     206,   198,    66,    72,    73,    70,    71,    74,    75,    76,
       0,    69,    97,    98,    95,    96,    99,   100,   101,     0,
      94,    81,    82,    79,    80,    83,    84,    85,    78,   106,
     107,   104,   105,   108,   109,   110,   103,    89,    90,    87,
      88,    91,    92,    93,    86,     0,   319,     0,   446,     0,
       0,     0,   273,   279,    77,   102,     0,   312,   317,     0,
     280,   274,   446,     0,     0,     0,   446,     0,   281,     0,
     275,   315,     0,   311,   313,   316,   282,   276,   446,     0,
     277,   314,   278
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -522,  -522,  -522,  1014,  -522,  1050,  -522,   579,  -522,   569,
    -522,   489,   497,  -522,  -403,  1051,  1054,   932,  -522,  -522,
    1055,  -522,   824,  1062,  1066,   -59,  1109,   -16,   849,   963,
      31,  -522,  -522,   638,  -522,  -522,  -522,  -522,  -522,  -522,
    -179,  -522,  -522,  -522,  -522,   547,   -77,    27,   473,  -522,
    -522,   977,  -522,  -522,  1070,  1072,  1073,  1074,  1075,  -522,
    -167,  -522,   814,  -191,  -192,  -522,  -474,  -455,  -454,  -453,
    -448,  -439,   476,  -522,  -522,  -522,  -522,  -522,  -522,   816,
    -522,  -522,   703,   427,  -215,  -522,  -522,  -522,   500,  -522,
    -522,  -522,  -522,   501,   817,   818,  -236,  -522,  -522,  -522,
    -522,   933,  -411,   515,  -117,   276,   396,  -522,  -522,  -521,
    -522,   424,   486,  -522,   739
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   129,    21,   404,   405,   406,   526,
     616,   617,   716,   407,   295,    22,    23,   180,    24,    63,
      25,   189,   190,    26,    27,    28,    29,    30,   104,   166,
     105,   171,   394,   395,   495,   288,   399,   169,   393,   491,
     192,   757,   668,   102,   485,   486,   487,   488,   595,    31,
      90,    91,   489,   592,    32,    33,    34,    35,    36,    37,
     220,   415,   221,   222,   223,   935,   224,   225,   226,   227,
     228,   229,   602,   603,   230,   231,   232,   233,   234,   325,
     235,   236,   237,   238,   239,   733,   240,   241,   242,   243,
     244,   245,   246,   247,   345,   346,   248,   249,   250,   251,
     252,   253,   543,   544,   194,   115,   107,    98,   112,   385,
     622,   580,   581,   410,   411
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     302,    95,   285,   344,   136,   618,   532,   195,   320,   545,
     301,   103,   324,   467,    49,    99,   596,   100,   290,   101,
     341,   342,   153,   339,   340,   341,   342,   392,   348,   123,
     124,    50,   559,    52,  -441,   597,   598,   599,    43,   280,
      88,     1,   600,     2,     3,     4,     5,     6,     7,     8,
       9,   601,   640,   197,   198,   199,    49,    10,   451,    11,
      12,    13,   448,   372,   191,    51,   352,   353,   373,   254,
     113,   255,   256,   259,   349,   260,   261,   350,   122,   493,
     494,     1,    14,     2,     3,     4,     5,     6,     7,   374,
       9,    82,   143,   593,   375,   396,   397,    10,   376,    11,
      12,    13,   264,   377,   265,   266,   352,   353,   417,   152,
     401,    86,   452,   643,    14,   412,   537,   538,   413,   352,
     353,   196,   197,   198,   199,   620,   320,   539,   540,   541,
     257,   427,   428,   167,   262,   174,   175,   176,   423,   529,
     449,   351,   530,   183,   184,   185,   125,   594,   350,   550,
     442,   204,   205,   206,    14,   120,   291,   207,   469,   352,
     353,   446,   447,   267,   297,   453,   454,   455,   456,   457,
     458,   459,   460,   461,   462,   463,   464,   465,   466,    87,
     277,   208,   209,   210,   551,   421,   296,   196,   197,   198,
     199,   661,   596,   200,   201,   300,   284,    17,   165,    15,
     468,   641,   202,   347,   203,   293,   484,   496,   218,   343,
     725,   597,   598,   599,   343,    89,   718,    16,   600,   217,
     204,   205,   206,    92,   352,   353,   207,   601,   542,    17,
     258,   400,    14,   402,   263,   403,   352,   353,    93,    15,
     554,   555,   218,   557,   352,   353,   561,   535,   352,   353,
     208,   209,   210,   378,   546,    96,   323,   570,   379,   200,
     201,    97,   380,   268,   482,   352,   353,   381,   202,    17,
     203,   646,   211,   323,   356,  -444,   120,   352,   353,   387,
     483,   145,   146,   572,   863,   106,   204,   205,   206,   388,
      53,    54,   207,  -445,  -445,   212,    55,   213,   103,   214,
     612,   396,   147,   148,   612,   426,   215,   216,   217,   384,
     106,   218,   604,   219,   422,   114,   208,   209,   210,   352,
     353,   116,   117,   118,   119,   416,   568,   121,   813,   157,
    -445,  -445,   366,   367,   368,   369,   370,   371,   211,   645,
     196,   197,   198,   199,   150,   158,   159,   160,   126,   531,
     816,   613,   127,   614,   615,   613,   714,   614,   615,   128,
     638,   212,   639,   213,   642,   214,   352,   353,    38,    39,
      40,   142,   215,   216,   217,   547,   144,   218,   350,   219,
      41,    42,   654,   149,   670,   671,   672,   673,   674,   477,
     478,   675,   676,   196,   197,   198,   199,   928,   562,   920,
     677,   563,   651,   921,   922,   949,   950,   729,   548,   923,
     736,   941,   200,   201,   678,   945,   333,    14,   334,   335,
     336,   202,   151,   203,    99,   583,   100,   951,   101,   154,
     731,    44,    45,    46,   571,   108,   109,   110,   111,   204,
     205,   206,   269,    47,    48,   207,   270,   271,   566,   567,
     726,   272,   273,   155,   688,   689,   690,   691,   692,   722,
     564,   693,   694,   565,   624,   200,   201,   413,   156,   208,
     209,   210,   341,   807,   202,   744,   203,   745,   746,   679,
     680,   681,   682,   683,   695,   625,   684,   685,   350,   939,
     940,   211,   204,   205,   206,   686,   632,   634,   207,   633,
     633,   196,   197,   198,   199,   161,   430,   162,   431,   687,
     432,   552,   164,   553,   212,   432,   213,   212,   214,   213,
     168,   214,   208,   209,   210,   215,   216,   217,   635,   170,
     218,   350,   219,   748,   653,   749,   172,   350,   196,   197,
     198,   199,   173,     1,   211,     2,     3,     4,     5,     6,
       7,     8,     9,   650,   658,   719,   177,   659,   413,    10,
     178,    11,    12,    13,   179,   818,   181,   212,   182,   213,
     186,   214,   741,   318,   319,   350,   937,   938,   215,   216,
     217,   187,   202,   218,   203,   219,   751,   752,   817,   188,
     696,   697,   698,   699,   700,   191,   193,   701,   702,   274,
     204,   205,   206,   275,   758,   803,   207,   759,   413,   276,
     318,    64,    65,   278,    66,   806,    14,   281,   350,   202,
     703,   203,   282,   286,   870,   809,    67,    68,   810,   287,
     208,   209,   210,   862,   289,   294,   659,   204,   205,   206,
     298,   868,   871,   207,   633,   413,   704,   705,   706,   707,
     708,   299,   211,   709,   710,   303,   326,   585,  -213,   586,
     587,   588,   589,   425,   590,   591,   305,   208,   209,   210,
     918,   932,   321,   919,   933,   212,   711,   213,   304,   214,
      83,    84,    85,   368,   369,   370,   215,   216,   217,   211,
     322,   218,   354,   219,   355,    56,    57,    58,    59,    60,
      61,    15,   327,    62,   328,   329,   332,   337,   330,   338,
     371,   382,   212,   383,   213,   425,   214,   384,   356,    16,
     389,   390,   391,   215,   216,   217,   398,   408,   218,   409,
     219,    17,   414,   419,    14,   357,   358,   359,   360,   425,
      69,    70,    71,   362,    72,    73,   420,   356,   433,    74,
      75,    76,   424,   438,    77,    78,    79,   434,   435,   436,
     439,    80,    81,   443,   357,   358,   359,   360,   361,   437,
     356,   440,   362,   363,   364,   365,   366,   367,   368,   369,
     370,   441,   445,   470,   727,   450,   218,   357,   358,   359,
     360,   472,   574,   474,   356,   362,   475,   476,   480,   479,
     481,   490,   363,   364,   365,   366,   367,   368,   369,   370,
     527,   357,   358,   359,   360,   528,   356,   492,   534,   362,
     536,   449,   549,   556,   558,   363,   364,   365,   366,   367,
     368,   369,   370,  -445,  -445,   359,   360,   352,   569,   573,
     577,  -445,   575,   578,   576,   579,   582,   584,   606,   363,
     364,   365,   366,   367,   368,   369,   370,   306,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   607,
     608,  -445,   364,   365,   366,   367,   368,   369,   370,   497,
     498,   499,   500,   501,   502,   503,   504,   505,   506,   507,
     508,   509,   510,   511,   512,   513,   621,   514,   515,   516,
     517,   518,   519,   627,   609,   520,   610,   611,   521,   522,
     619,   626,   523,   524,   525,   628,   629,   630,   644,   631,
     636,   647,   648,   649,   652,   567,   566,   655,   656,   660,
     663,   657,   664,   665,   666,   667,   712,   669,   713,   714,
     730,   721,   720,   728,   724,   732,   742,   743,   755,   737,
     738,   740,   739,   753,   756,   767,   776,   805,   760,   761,
     762,   763,   764,   765,   766,   768,   769,   770,   771,   772,
     773,   774,   775,   777,   778,   779,   814,   780,   819,   815,
     781,   782,   783,   784,   820,   785,   821,   786,   822,   787,
     823,   788,   789,   824,   790,   791,   792,   793,   794,   795,
     796,   797,   798,   799,   800,   801,   825,   826,   808,   811,
     828,   812,   827,   829,   830,   831,   832,   833,   834,   835,
     836,   837,   838,   839,   840,   841,   842,   843,   844,   845,
     846,   847,   848,   849,   850,   851,   852,   853,   854,   855,
     856,   857,   858,   859,   860,   861,   864,   865,   866,   867,
     350,   872,   869,   873,   874,   875,   876,   877,   880,   889,
     878,   879,   881,   882,   926,   883,   884,   927,   885,   943,
     929,   886,   887,   888,   930,   890,   891,   931,   892,   893,
     894,   895,   896,   897,   898,   899,   900,   901,   902,   942,
     903,   904,   905,   946,   947,   906,   907,   908,   909,   910,
     911,   912,   913,   914,   952,   715,   916,   915,   292,   623,
     917,   163,   924,   130,   131,   717,   925,   132,   133,   637,
     936,   934,   944,   948,   418,   134,    94,   386,   283,   135,
     605,   662,   279,   137,   750,   138,   139,   140,   141,   560,
     429,   802,   754,   331,   804,   747,   734,   735,   723,   533,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   444,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   471,     0,     0,   473
};

static const yytype_int16 yycheck[] =
{
     191,    17,   169,   218,    63,   526,   409,   124,   200,   420,
     189,     8,   203,     3,     3,    20,   490,    22,    57,    24,
       5,     6,    64,   215,   216,     5,     6,    78,   219,    78,
      79,     4,     3,     6,     0,   490,   490,   490,    33,   156,
      13,     7,   490,     9,    10,    11,    12,    13,    14,    15,
      16,   490,    85,     4,     5,     6,     3,    23,    75,    25,
      26,    27,    77,   194,    68,    78,   152,   153,   199,     3,
      43,     5,     6,     3,   196,     5,     6,   199,    51,    72,
      73,     7,    80,     9,    10,    11,    12,    13,    14,   194,
      16,    30,    65,     3,   199,   286,   287,    23,   194,    25,
      26,    27,     3,   199,     5,     6,   152,   153,   299,    82,
       3,     3,   129,   199,    80,   196,    70,    71,   199,   152,
     153,     3,     4,     5,     6,   528,   318,    81,    82,    83,
      64,   322,   323,   102,    64,   108,   109,   110,   305,   196,
     155,    57,   199,   116,   117,   118,   195,    57,   199,    57,
     196,   102,   103,   104,    80,   197,   195,   108,   373,   152,
     153,   352,   353,    64,   180,   357,   358,   359,   360,   361,
     362,   363,   364,   365,   366,   367,   368,   369,   370,     3,
     153,   132,   133,   134,    88,    67,    34,     3,     4,     5,
       6,   196,   666,    75,    76,   199,   165,   195,   195,   165,
     190,    88,    84,   219,    86,   178,   195,   398,   193,   194,
      85,   666,   666,   666,   194,     3,   619,   183,   666,   190,
     102,   103,   104,   166,   152,   153,   108,   666,   182,   195,
     164,   290,    80,   126,   164,   128,   152,   153,    16,   165,
     431,   432,   193,   434,   152,   153,   437,   414,   152,   153,
     132,   133,   134,   194,   421,     0,    89,   449,   199,    75,
      76,   198,   194,   164,    29,   152,   153,   199,    84,   195,
      86,   199,   154,    89,   130,    64,   197,   152,   153,    42,
      45,   170,   171,   450,   805,    74,   102,   103,   104,    52,
     162,   163,   108,   149,   150,   177,   168,   179,     8,   181,
      75,   492,   170,   171,    75,   321,   188,   189,   190,    80,
      74,   193,   491,   195,   196,    74,   132,   133,   134,   152,
     153,    45,    46,    47,    48,   298,   443,    14,   739,   157,
     186,   187,   188,   189,   190,   191,   192,   197,   154,   199,
       3,     4,     5,     6,   173,   173,   174,   175,     3,   408,
     753,   126,     3,   128,   129,   126,   131,   128,   129,     3,
     551,   177,   553,   179,   556,   181,   152,   153,    29,    30,
      31,     3,   188,   189,   190,   196,     6,   193,   199,   195,
      41,    42,   574,     3,    91,    92,    93,    94,    95,    48,
      49,    98,    99,     3,     4,     5,     6,   918,   196,    42,
     107,   199,   569,    46,    47,    50,    51,   643,   424,    52,
     646,   932,    75,    76,   121,   936,   120,    80,   122,   123,
     124,    84,   162,    86,    20,   484,    22,   948,    24,    57,
     645,    29,    30,    31,   450,    39,    40,    41,    42,   102,
     103,   104,    42,    41,    42,   108,    46,    47,     5,     6,
     641,    51,    52,   199,    91,    92,    93,    94,    95,   626,
     196,    98,    99,   199,   196,    75,    76,   199,     6,   132,
     133,   134,     5,     6,    84,     3,    86,     5,     6,    91,
      92,    93,    94,    95,   121,   196,    98,    99,   199,    48,
      49,   154,   102,   103,   104,   107,   196,   196,   108,   199,
     199,     3,     4,     5,     6,   196,    85,   196,    87,   121,
      89,    85,    21,    87,   177,    89,   179,   177,   181,   179,
      67,   181,   132,   133,   134,   188,   189,   190,   196,    69,
     193,   199,   195,    30,   196,    32,    75,   199,     3,     4,
       5,     6,     3,     7,   154,     9,    10,    11,    12,    13,
      14,    15,    16,   569,   196,   196,     3,   199,   199,    23,
      64,    25,    26,    27,   195,   756,    76,   177,     3,   179,
       3,   181,   196,    75,    76,   199,    46,    47,   188,   189,
     190,     3,    84,   193,    86,   195,   663,   664,   755,     3,
      91,    92,    93,    94,    95,    68,     4,    98,    99,     3,
     102,   103,   104,     3,   196,   196,   108,   199,   199,     6,
      75,    29,    30,   195,    32,   196,    80,     6,   199,    84,
     121,    86,     6,    55,   815,   196,    44,    45,   199,    70,
     132,   133,   134,   196,    76,     3,   199,   102,   103,   104,
      64,   196,   196,   108,   199,   199,    91,    92,    93,    94,
      95,   185,   154,    98,    99,    80,     4,    58,    59,    60,
      61,    62,    63,    75,    65,    66,   195,   132,   133,   134,
     196,   196,   195,   199,   199,   177,   121,   179,    80,   181,
     156,   157,   158,   190,   191,   192,   188,   189,   190,   154,
     195,   193,    75,   195,    77,    35,    36,    37,    38,    39,
      40,   165,     4,    43,     4,     4,   195,   195,     6,   195,
     197,     3,   177,     6,   179,    75,   181,    80,   130,   183,
       6,   196,   196,   188,   189,   190,    71,    57,   193,   195,
     195,   195,   195,   195,    80,   147,   148,   149,   150,    75,
     158,   159,   160,   155,   162,   163,   195,   130,     4,   167,
     168,   169,   195,     4,   172,   173,   174,   195,   195,   195,
       4,   179,   180,    79,   147,   148,   149,   150,   151,   195,
     130,   200,   155,   185,   186,   187,   188,   189,   190,   191,
     192,   196,     3,     6,   196,   195,   193,   147,   148,   149,
     150,     6,   152,     6,   130,   155,     5,    46,   196,   195,
       6,   176,   185,   186,   187,   188,   189,   190,   191,   192,
     127,   147,   148,   149,   150,   195,   130,   199,     3,   155,
     199,   155,   196,    78,     4,   185,   186,   187,   188,   189,
     190,   191,   192,   147,   148,   149,   150,   152,   195,   129,
       6,   155,   200,     6,   200,     3,     6,   199,   195,   185,
     186,   187,   188,   189,   190,   191,   192,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   195,
     195,   185,   186,   187,   188,   189,   190,   191,   192,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,    28,   108,   109,   110,
     111,   112,   113,     6,   195,   116,   195,   195,   119,   120,
     195,   195,   123,   124,   125,     6,     4,     3,   199,     6,
     196,     4,     4,    80,   196,     6,     5,    53,    50,     6,
      59,   185,    59,     3,   199,    54,   129,     6,   127,   131,
       4,   161,   195,   193,   196,   193,     6,     6,    55,   196,
     196,   196,   195,   195,    56,    93,    93,     3,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,     6,   199,     6,    64,
     199,   199,   199,   199,     6,   199,     6,   199,     6,   199,
       6,   199,   199,     6,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,     6,     6,   199,   199,
       6,   199,   199,     6,     6,     6,     6,     6,     6,     6,
     199,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     4,     4,     4,     4,
     199,   196,     6,   196,   196,   196,   196,   196,     6,     6,
     196,   196,   196,   196,     4,   196,   196,     4,   196,     4,
       6,   196,   196,   196,     6,   196,   196,     6,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,     6,
     196,   196,   196,     6,     6,   196,   196,   196,   196,   196,
     196,   196,   196,   196,     6,   616,   196,   199,   176,   530,
     199,    97,   196,    63,    63,   618,   196,    63,    63,   550,
     196,   199,   196,   196,   300,    63,    17,   278,   165,    63,
     492,   584,   155,    63,   661,    63,    63,    63,    63,   436,
     324,   714,   666,   210,   720,   659,   646,   646,   633,   410,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   350,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   375,    -1,    -1,   377
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
     194,   199,     3,     6,    80,   310,   229,    42,    52,     6,
     196,   196,    78,   239,   233,   234,   264,   264,    71,   237,
     226,     3,   126,   128,   207,   208,   209,   214,    57,   195,
     314,   315,   196,   199,   195,   262,   248,   264,   223,   195,
     195,    67,   196,   261,   195,    75,   228,   264,   264,   280,
      85,    87,    89,     4,   195,   195,   195,   195,     4,     4,
     200,   196,   196,    79,   263,     3,   264,   264,    77,   155,
     195,    75,   129,   265,   265,   265,   265,   265,   265,   265,
     265,   265,   265,   265,   265,   265,   265,     3,   190,   285,
       6,   295,     6,   296,     6,     5,    46,    48,    49,   195,
     196,     6,    29,    45,   195,   245,   246,   247,   248,   253,
     176,   240,   199,    72,    73,   235,   264,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   108,   109,   110,   111,   112,   113,
     116,   119,   120,   123,   124,   125,   210,   127,   195,   196,
     199,   226,   215,   315,     3,   261,   199,    70,    71,    81,
      82,    83,   182,   303,   304,   303,   261,   196,   228,   196,
      57,    88,    85,    87,   264,   264,    78,   264,     4,     3,
     283,   264,   196,   199,   196,   199,     5,     6,   305,   195,
     265,   228,   261,   129,   152,   200,   200,     6,     6,     3,
     312,   313,     6,   226,   199,    58,    60,    61,    62,    63,
      65,    66,   254,     3,    57,   249,   267,   268,   269,   270,
     271,   272,   273,   274,   241,   234,   195,   195,   195,   195,
     195,   195,    75,   126,   128,   129,   211,   212,   310,   195,
     215,    28,   311,   208,   196,   196,   195,     6,     6,     4,
       3,     6,   196,   199,   196,   196,   196,   210,   264,   264,
      85,    88,   265,   199,   199,   199,   199,     4,     4,    80,
     228,   261,   196,   196,   265,    53,    50,   185,   196,   199,
       6,   196,   246,    59,    59,     3,   199,    54,   243,     6,
      91,    92,    93,    94,    95,    98,    99,   107,   121,    91,
      92,    93,    94,    95,    98,    99,   107,   121,    91,    92,
      93,    94,    95,    98,    99,   121,    91,    92,    93,    94,
      95,    98,    99,   121,    91,    92,    93,    94,    95,    98,
      99,   121,   129,   127,   131,   212,   213,   213,   215,   196,
     195,   161,   261,   304,   196,    85,   264,   196,   193,   297,
       4,   285,   193,   286,   289,   294,   297,   196,   196,   195,
     196,   196,     6,     6,     3,     5,     6,   313,    30,    32,
     249,   247,   247,   195,   273,    55,    56,   242,   196,   199,
     199,   199,   199,   199,   199,   199,   199,    93,   199,   199,
     199,   199,   199,   199,   199,   199,    93,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   199,   284,   196,   312,     3,   196,     6,   199,   196,
     199,   199,   199,   303,     6,    64,   215,   261,   264,     6,
       6,     6,     6,     6,     6,     6,     6,   199,     6,     6,
       6,     6,     6,     6,     6,     6,   199,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   196,   310,     4,     4,     4,     4,   196,     6,
     264,   196,   196,   196,   196,   196,   196,   196,   196,   196,
       6,   196,   196,   196,   196,   196,   196,   196,   196,     6,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   199,   196,   199,   196,   199,
      42,    46,    47,    52,   196,   196,     4,     4,   310,     6,
       6,     6,   196,   199,   199,   266,   196,    46,    47,    48,
      49,   310,     6,     4,   196,   310,     6,     6,   196,    50,
      51,   310,     6
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
       4,     4,     4,     4,     4,     4,     3,     3,     4,     6,
       7,     9,    10,    12,    13,    14,    15,    16,    17,    12,
      13,    14,    15,     3,     4,     5,     6,     1,     3,     3,
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
#line 2248 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2256 "parser.cpp"
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
#line 2270 "parser.cpp"
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
#line 2284 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2295 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2303 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2312 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2321 "parser.cpp"
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
#line 2335 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 269 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2346 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2356 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2366 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2376 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2386 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2396 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2406 "parser.cpp"
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
#line 2420 "parser.cpp"
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
#line 2434 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2444 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2452 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2460 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2469 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2477 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2485 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2493 "parser.cpp"
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
#line 2507 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2534 "parser.cpp"
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
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2556 "parser.cpp"
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
#line 2570 "parser.cpp"
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
#line 2584 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2594 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2603 "parser.cpp"
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
#line 2617 "parser.cpp"
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
#line 2634 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2642 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2650 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2658 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2666 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2674 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2682 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2690 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2698 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2706 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2714 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2722 "parser.cpp"
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
#line 2736 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2744 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2752 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2760 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2776 "parser.cpp"
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
#line 2789 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2797 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2805 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2813 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2821 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2829 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2837 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2845 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2853 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2861 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2869 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2877 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2885 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2893 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2901 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 366 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2909 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 370 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2917 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2925 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2933 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2941 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2949 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2957 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2965 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2973 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 209 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2984 "parser.cpp"
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
#line 2998 "parser.cpp"
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
#line 3012 "parser.cpp"
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
#line 3026 "parser.cpp"
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
#line 3040 "parser.cpp"
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

#line 3148 "parser.cpp"

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
#line 3363 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 500 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3374 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 506 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3385 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 513 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3391 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3397 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3403 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 516 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3409 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3415 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3421 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3427 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 520 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3433 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3439 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 522 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3445 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 523 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3451 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 524 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3457 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 525 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3463 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 526 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3469 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 528 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3475 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3481 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3487 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3493 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3499 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3505 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3511 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3517 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3523 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 537 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3529 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 538 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3535 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3541 "parser.cpp"
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
#line 3561 "parser.cpp"
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
#line 3579 "parser.cpp"
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
#line 3612 "parser.cpp"
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
#line 3632 "parser.cpp"
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
#line 3653 "parser.cpp"
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
#line 3686 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 671 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3695 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 675 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3704 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 681 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3712 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 684 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3720 "parser.cpp"
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
#line 3775 "parser.cpp"
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
#line 3814 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 777 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3820 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3826 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 779 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3832 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3838 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 781 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3844 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 782 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3850 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 783 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3856 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 784 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3862 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 785 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3868 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 786 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3874 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 787 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3880 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 788 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3886 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 789 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3892 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 790 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3898 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 791 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3904 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 792 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3910 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3916 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 794 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3922 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3928 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 796 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3934 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 797 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3940 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 800 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3946 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 802 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3952 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3958 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 804 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3964 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 805 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3970 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3976 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3982 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3988 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3994 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4000 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4006 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4012 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4018 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4024 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4030 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4036 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4042 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4048 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4054 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4060 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4066 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4072 "parser.cpp"
    break;

  case 86: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4078 "parser.cpp"
    break;

  case 87: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4084 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4090 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4096 "parser.cpp"
    break;

  case 90: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4102 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4108 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4114 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4120 "parser.cpp"
    break;

  case 94: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4126 "parser.cpp"
    break;

  case 95: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4132 "parser.cpp"
    break;

  case 96: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4138 "parser.cpp"
    break;

  case 97: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4144 "parser.cpp"
    break;

  case 98: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4150 "parser.cpp"
    break;

  case 99: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4156 "parser.cpp"
    break;

  case 100: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4162 "parser.cpp"
    break;

  case 101: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4168 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4174 "parser.cpp"
    break;

  case 103: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4180 "parser.cpp"
    break;

  case 104: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4186 "parser.cpp"
    break;

  case 105: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4192 "parser.cpp"
    break;

  case 106: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4198 "parser.cpp"
    break;

  case 107: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4204 "parser.cpp"
    break;

  case 108: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4210 "parser.cpp"
    break;

  case 109: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4216 "parser.cpp"
    break;

  case 110: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4222 "parser.cpp"
    break;

  case 111: /* column_constraints: column_constraint  */
#line 868 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4231 "parser.cpp"
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
#line 4245 "parser.cpp"
    break;

  case 113: /* column_constraint: PRIMARY KEY  */
#line 882 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4253 "parser.cpp"
    break;

  case 114: /* column_constraint: UNIQUE  */
#line 885 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4261 "parser.cpp"
    break;

  case 115: /* column_constraint: NULLABLE  */
#line 888 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4269 "parser.cpp"
    break;

  case 116: /* column_constraint: NOT NULLABLE  */
#line 891 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4277 "parser.cpp"
    break;

  case 117: /* default_expr: DEFAULT constant_expr  */
#line 895 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4285 "parser.cpp"
    break;

  case 118: /* default_expr: %empty  */
#line 898 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4293 "parser.cpp"
    break;

  case 119: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 903 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4303 "parser.cpp"
    break;

  case 120: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 908 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4313 "parser.cpp"
    break;

  case 121: /* identifier_array: IDENTIFIER  */
#line 915 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4324 "parser.cpp"
    break;

  case 122: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 921 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4335 "parser.cpp"
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
#line 4352 "parser.cpp"
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
#line 4391 "parser.cpp"
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
#line 4408 "parser.cpp"
    break;

  case 126: /* optional_identifier_array: '(' identifier_array ')'  */
#line 994 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4416 "parser.cpp"
    break;

  case 127: /* optional_identifier_array: %empty  */
#line 997 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4424 "parser.cpp"
    break;

  case 128: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1004 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4434 "parser.cpp"
    break;

  case 129: /* explain_type: ANALYZE  */
#line 1010 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4442 "parser.cpp"
    break;

  case 130: /* explain_type: AST  */
#line 1013 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4450 "parser.cpp"
    break;

  case 131: /* explain_type: RAW  */
#line 1016 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4458 "parser.cpp"
    break;

  case 132: /* explain_type: LOGICAL  */
#line 1019 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4466 "parser.cpp"
    break;

  case 133: /* explain_type: PHYSICAL  */
#line 1022 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4474 "parser.cpp"
    break;

  case 134: /* explain_type: PIPELINE  */
#line 1025 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4482 "parser.cpp"
    break;

  case 135: /* explain_type: FRAGMENT  */
#line 1028 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4490 "parser.cpp"
    break;

  case 136: /* explain_type: %empty  */
#line 1031 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4498 "parser.cpp"
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
#line 4515 "parser.cpp"
    break;

  case 138: /* update_expr_array: update_expr  */
#line 1051 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4524 "parser.cpp"
    break;

  case 139: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1055 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4533 "parser.cpp"
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
#line 4545 "parser.cpp"
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
#line 4561 "parser.cpp"
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
#line 4579 "parser.cpp"
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
#line 4597 "parser.cpp"
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
#line 4615 "parser.cpp"
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
#line 4638 "parser.cpp"
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
#line 4696 "parser.cpp"
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
#line 4756 "parser.cpp"
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
#line 4808 "parser.cpp"
    break;

  case 149: /* select_statement: select_without_paren  */
#line 1313 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4816 "parser.cpp"
    break;

  case 150: /* select_statement: select_with_paren  */
#line 1316 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4824 "parser.cpp"
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
#line 4838 "parser.cpp"
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
#line 4852 "parser.cpp"
    break;

  case 153: /* select_with_paren: '(' select_without_paren ')'  */
#line 1338 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4860 "parser.cpp"
    break;

  case 154: /* select_with_paren: '(' select_with_paren ')'  */
#line 1341 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4868 "parser.cpp"
    break;

  case 155: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1345 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4877 "parser.cpp"
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
#line 4903 "parser.cpp"
    break;

  case 157: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1372 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4911 "parser.cpp"
    break;

  case 158: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1375 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4919 "parser.cpp"
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
#line 4939 "parser.cpp"
    break;

  case 160: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1396 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4947 "parser.cpp"
    break;

  case 161: /* order_by_clause: %empty  */
#line 1399 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4955 "parser.cpp"
    break;

  case 162: /* order_by_expr_list: order_by_expr  */
#line 1403 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4964 "parser.cpp"
    break;

  case 163: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1407 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4973 "parser.cpp"
    break;

  case 164: /* order_by_expr: expr order_by_type  */
#line 1412 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4983 "parser.cpp"
    break;

  case 165: /* order_by_type: ASC  */
#line 1418 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4991 "parser.cpp"
    break;

  case 166: /* order_by_type: DESC  */
#line 1421 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4999 "parser.cpp"
    break;

  case 167: /* order_by_type: %empty  */
#line 1424 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5007 "parser.cpp"
    break;

  case 168: /* limit_expr: LIMIT expr  */
#line 1428 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5015 "parser.cpp"
    break;

  case 169: /* limit_expr: %empty  */
#line 1432 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5021 "parser.cpp"
    break;

  case 170: /* offset_expr: OFFSET expr  */
#line 1434 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5029 "parser.cpp"
    break;

  case 171: /* offset_expr: %empty  */
#line 1438 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5035 "parser.cpp"
    break;

  case 172: /* distinct: DISTINCT  */
#line 1440 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5043 "parser.cpp"
    break;

  case 173: /* distinct: %empty  */
#line 1443 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5051 "parser.cpp"
    break;

  case 174: /* from_clause: FROM table_reference  */
#line 1447 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5059 "parser.cpp"
    break;

  case 175: /* from_clause: %empty  */
#line 1450 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5067 "parser.cpp"
    break;

  case 176: /* search_clause: SEARCH sub_search_array  */
#line 1454 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5077 "parser.cpp"
    break;

  case 177: /* search_clause: %empty  */
#line 1459 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5085 "parser.cpp"
    break;

  case 178: /* where_clause: WHERE expr  */
#line 1463 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5093 "parser.cpp"
    break;

  case 179: /* where_clause: %empty  */
#line 1466 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5101 "parser.cpp"
    break;

  case 180: /* having_clause: HAVING expr  */
#line 1470 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5109 "parser.cpp"
    break;

  case 181: /* having_clause: %empty  */
#line 1473 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5117 "parser.cpp"
    break;

  case 182: /* group_by_clause: GROUP BY expr_array  */
#line 1477 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5125 "parser.cpp"
    break;

  case 183: /* group_by_clause: %empty  */
#line 1480 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5133 "parser.cpp"
    break;

  case 184: /* set_operator: UNION  */
#line 1484 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5141 "parser.cpp"
    break;

  case 185: /* set_operator: UNION ALL  */
#line 1487 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5149 "parser.cpp"
    break;

  case 186: /* set_operator: INTERSECT  */
#line 1490 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5157 "parser.cpp"
    break;

  case 187: /* set_operator: EXCEPT  */
#line 1493 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5165 "parser.cpp"
    break;

  case 188: /* table_reference: table_reference_unit  */
#line 1501 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5173 "parser.cpp"
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
#line 5191 "parser.cpp"
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
#line 5209 "parser.cpp"
    break;

  case 193: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1535 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5220 "parser.cpp"
    break;

  case 194: /* table_name: IDENTIFIER  */
#line 1544 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5230 "parser.cpp"
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
#line 5242 "parser.cpp"
    break;

  case 196: /* table_alias: AS IDENTIFIER  */
#line 1558 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5252 "parser.cpp"
    break;

  case 197: /* table_alias: IDENTIFIER  */
#line 1563 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5262 "parser.cpp"
    break;

  case 198: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1568 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5273 "parser.cpp"
    break;

  case 199: /* table_alias: %empty  */
#line 1574 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5281 "parser.cpp"
    break;

  case 200: /* with_clause: WITH with_expr_list  */
#line 1581 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5289 "parser.cpp"
    break;

  case 201: /* with_clause: %empty  */
#line 1584 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5297 "parser.cpp"
    break;

  case 202: /* with_expr_list: with_expr  */
#line 1588 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5306 "parser.cpp"
    break;

  case 203: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1591 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5315 "parser.cpp"
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
#line 5327 "parser.cpp"
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
#line 5339 "parser.cpp"
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
#line 5352 "parser.cpp"
    break;

  case 207: /* join_type: INNER  */
#line 1629 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5360 "parser.cpp"
    break;

  case 208: /* join_type: LEFT  */
#line 1632 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5368 "parser.cpp"
    break;

  case 209: /* join_type: RIGHT  */
#line 1635 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5376 "parser.cpp"
    break;

  case 210: /* join_type: OUTER  */
#line 1638 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5384 "parser.cpp"
    break;

  case 211: /* join_type: FULL  */
#line 1641 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5392 "parser.cpp"
    break;

  case 212: /* join_type: CROSS  */
#line 1644 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5400 "parser.cpp"
    break;

  case 213: /* join_type: %empty  */
#line 1647 "parser.y"
                {
}
#line 5407 "parser.cpp"
    break;

  case 214: /* show_statement: SHOW DATABASES  */
#line 1653 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5416 "parser.cpp"
    break;

  case 215: /* show_statement: SHOW TABLES  */
#line 1657 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5425 "parser.cpp"
    break;

  case 216: /* show_statement: SHOW VIEWS  */
#line 1661 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5434 "parser.cpp"
    break;

  case 217: /* show_statement: SHOW CONFIGS  */
#line 1665 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5443 "parser.cpp"
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
#line 5455 "parser.cpp"
    break;

  case 219: /* show_statement: SHOW PROFILES  */
#line 1676 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5464 "parser.cpp"
    break;

  case 220: /* show_statement: SHOW BUFFER  */
#line 1680 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5473 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW QUERIES  */
#line 1684 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5482 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1688 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5492 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW TRANSACTIONS  */
#line 1693 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5501 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1697 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5511 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW SESSION VARIABLES  */
#line 1702 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5520 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1706 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5529 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1710 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5540 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1716 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5551 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1722 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5562 "parser.cpp"
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
#line 5578 "parser.cpp"
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
#line 5594 "parser.cpp"
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
#line 5610 "parser.cpp"
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
#line 5627 "parser.cpp"
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
#line 5644 "parser.cpp"
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
#line 5662 "parser.cpp"
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
#line 5681 "parser.cpp"
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
#line 5697 "parser.cpp"
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
#line 5716 "parser.cpp"
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
#line 5737 "parser.cpp"
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
#line 5759 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW LOGS  */
#line 1870 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5768 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW DELTA LOGS  */
#line 1874 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5777 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW CATALOGS  */
#line 1878 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5786 "parser.cpp"
    break;

  case 244: /* flush_statement: FLUSH DATA  */
#line 1886 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5795 "parser.cpp"
    break;

  case 245: /* flush_statement: FLUSH LOG  */
#line 1890 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5804 "parser.cpp"
    break;

  case 246: /* flush_statement: FLUSH BUFFER  */
#line 1894 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5813 "parser.cpp"
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
#line 5828 "parser.cpp"
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
#line 5852 "parser.cpp"
    break;

  case 249: /* command_statement: USE IDENTIFIER  */
#line 1936 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5863 "parser.cpp"
    break;

  case 250: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1942 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5873 "parser.cpp"
    break;

  case 251: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1947 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5884 "parser.cpp"
    break;

  case 252: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1953 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5895 "parser.cpp"
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
#line 5908 "parser.cpp"
    break;

  case 254: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1967 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5919 "parser.cpp"
    break;

  case 255: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1973 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5930 "parser.cpp"
    break;

  case 256: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1979 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5941 "parser.cpp"
    break;

  case 257: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1985 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5952 "parser.cpp"
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
#line 5965 "parser.cpp"
    break;

  case 259: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1999 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5976 "parser.cpp"
    break;

  case 260: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2005 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5987 "parser.cpp"
    break;

  case 261: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2011 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5998 "parser.cpp"
    break;

  case 262: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2017 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6009 "parser.cpp"
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
#line 6022 "parser.cpp"
    break;

  case 264: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2031 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6033 "parser.cpp"
    break;

  case 265: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2037 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6044 "parser.cpp"
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
#line 6061 "parser.cpp"
    break;

  case 267: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2057 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6070 "parser.cpp"
    break;

  case 268: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2061 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6080 "parser.cpp"
    break;

  case 269: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2066 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6091 "parser.cpp"
    break;

  case 270: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2072 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6103 "parser.cpp"
    break;

  case 271: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2079 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6116 "parser.cpp"
    break;

  case 272: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2087 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6130 "parser.cpp"
    break;

  case 273: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2096 "parser.y"
                                                                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSegments;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-1].long_value);
}
#line 6145 "parser.cpp"
    break;

  case 274: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2106 "parser.y"
                                                                                                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSegment;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[0].long_value);
}
#line 6161 "parser.cpp"
    break;

  case 275: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2117 "parser.y"
                                                                                                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListBlocks;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-1].long_value);
}
#line 6177 "parser.cpp"
    break;

  case 276: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2128 "parser.y"
                                                                                                                                          {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowBlock;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-11].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[0].long_value);
}
#line 6194 "parser.cpp"
    break;

  case 277: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2140 "parser.y"
                                                                                                                                                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListColumns;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-12].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-11].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[-1].long_value);
}
#line 6211 "parser.cpp"
    break;

  case 278: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2152 "parser.y"
                                                                                                                                                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowColumn;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-13].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-12].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->block_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->column_index_ = (yyvsp[0].long_value);
}
#line 6229 "parser.cpp"
    break;

  case 279: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2165 "parser.y"
                                                                                                              {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListIndexes;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-1].long_value);
}
#line 6244 "parser.cpp"
    break;

  case 280: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2175 "parser.y"
                                                                                                                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndex;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[0].long_value);
}
#line 6260 "parser.cpp"
    break;

  case 281: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE SEGMENTS  */
#line 2186 "parser.y"
                                                                                                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListIndexSegments;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[-1].long_value);
}
#line 6276 "parser.cpp"
    break;

  case 282: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE SEGMENT LONG_VALUE  */
#line 2197 "parser.y"
                                                                                                                                          {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndexSegment;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-11].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->index_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[0].long_value);
}
#line 6293 "parser.cpp"
    break;

  case 283: /* admin_statement: ADMIN SHOW LOGS  */
#line 2209 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6302 "parser.cpp"
    break;

  case 284: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2213 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6312 "parser.cpp"
    break;

  case 285: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2218 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6322 "parser.cpp"
    break;

  case 286: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2223 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6333 "parser.cpp"
    break;

  case 287: /* expr_array: expr_alias  */
#line 2234 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6342 "parser.cpp"
    break;

  case 288: /* expr_array: expr_array ',' expr_alias  */
#line 2238 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6351 "parser.cpp"
    break;

  case 289: /* expr_array_list: '(' expr_array ')'  */
#line 2243 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6360 "parser.cpp"
    break;

  case 290: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2247 "parser.y"
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
#line 6380 "parser.cpp"
    break;

  case 291: /* expr_alias: expr AS IDENTIFIER  */
#line 2274 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6391 "parser.cpp"
    break;

  case 292: /* expr_alias: expr  */
#line 2280 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6399 "parser.cpp"
    break;

  case 298: /* operand: '(' expr ')'  */
#line 2290 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6407 "parser.cpp"
    break;

  case 299: /* operand: '(' select_without_paren ')'  */
#line 2293 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6418 "parser.cpp"
    break;

  case 300: /* operand: constant_expr  */
#line 2299 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6426 "parser.cpp"
    break;

  case 311: /* extra_match_tensor_option: ',' STRING  */
#line 2313 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6434 "parser.cpp"
    break;

  case 312: /* extra_match_tensor_option: %empty  */
#line 2316 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6442 "parser.cpp"
    break;

  case 313: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2322 "parser.y"
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
#line 6463 "parser.cpp"
    break;

  case 314: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2341 "parser.y"
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
#line 6509 "parser.cpp"
    break;

  case 315: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2383 "parser.y"
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
#line 6556 "parser.cpp"
    break;

  case 316: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2429 "parser.y"
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
#line 6578 "parser.cpp"
    break;

  case 317: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2447 "parser.y"
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
#line 6600 "parser.cpp"
    break;

  case 318: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2465 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6613 "parser.cpp"
    break;

  case 319: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2473 "parser.y"
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
#line 6628 "parser.cpp"
    break;

  case 320: /* query_expr: QUERY '(' STRING ')'  */
#line 2484 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6639 "parser.cpp"
    break;

  case 321: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2490 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6652 "parser.cpp"
    break;

  case 322: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2499 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6663 "parser.cpp"
    break;

  case 323: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2505 "parser.y"
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
#line 6679 "parser.cpp"
    break;

  case 324: /* sub_search: match_vector_expr  */
#line 2517 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6687 "parser.cpp"
    break;

  case 325: /* sub_search: match_text_expr  */
#line 2520 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6695 "parser.cpp"
    break;

  case 326: /* sub_search: match_tensor_expr  */
#line 2523 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6703 "parser.cpp"
    break;

  case 327: /* sub_search: match_sparse_expr  */
#line 2526 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6711 "parser.cpp"
    break;

  case 328: /* sub_search: query_expr  */
#line 2529 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6719 "parser.cpp"
    break;

  case 329: /* sub_search: fusion_expr  */
#line 2532 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6727 "parser.cpp"
    break;

  case 330: /* sub_search_array: sub_search  */
#line 2536 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6736 "parser.cpp"
    break;

  case 331: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2540 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6745 "parser.cpp"
    break;

  case 332: /* function_expr: IDENTIFIER '(' ')'  */
#line 2545 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6758 "parser.cpp"
    break;

  case 333: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2553 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6771 "parser.cpp"
    break;

  case 334: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2561 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6785 "parser.cpp"
    break;

  case 335: /* function_expr: operand IS NOT NULLABLE  */
#line 2570 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6797 "parser.cpp"
    break;

  case 336: /* function_expr: operand IS NULLABLE  */
#line 2577 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6809 "parser.cpp"
    break;

  case 337: /* function_expr: NOT operand  */
#line 2584 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6821 "parser.cpp"
    break;

  case 338: /* function_expr: '-' operand  */
#line 2591 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6833 "parser.cpp"
    break;

  case 339: /* function_expr: '+' operand  */
#line 2598 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6845 "parser.cpp"
    break;

  case 340: /* function_expr: operand '-' operand  */
#line 2605 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6858 "parser.cpp"
    break;

  case 341: /* function_expr: operand '+' operand  */
#line 2613 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6871 "parser.cpp"
    break;

  case 342: /* function_expr: operand '*' operand  */
#line 2621 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6884 "parser.cpp"
    break;

  case 343: /* function_expr: operand '/' operand  */
#line 2629 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6897 "parser.cpp"
    break;

  case 344: /* function_expr: operand '%' operand  */
#line 2637 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6910 "parser.cpp"
    break;

  case 345: /* function_expr: operand '=' operand  */
#line 2645 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6923 "parser.cpp"
    break;

  case 346: /* function_expr: operand EQUAL operand  */
#line 2653 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6936 "parser.cpp"
    break;

  case 347: /* function_expr: operand NOT_EQ operand  */
#line 2661 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6949 "parser.cpp"
    break;

  case 348: /* function_expr: operand '<' operand  */
#line 2669 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6962 "parser.cpp"
    break;

  case 349: /* function_expr: operand '>' operand  */
#line 2677 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6975 "parser.cpp"
    break;

  case 350: /* function_expr: operand LESS_EQ operand  */
#line 2685 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6988 "parser.cpp"
    break;

  case 351: /* function_expr: operand GREATER_EQ operand  */
#line 2693 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7001 "parser.cpp"
    break;

  case 352: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2701 "parser.y"
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
#line 7036 "parser.cpp"
    break;

  case 353: /* function_expr: operand LIKE operand  */
#line 2731 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7049 "parser.cpp"
    break;

  case 354: /* function_expr: operand NOT LIKE operand  */
#line 2739 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7062 "parser.cpp"
    break;

  case 355: /* conjunction_expr: expr AND expr  */
#line 2748 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7075 "parser.cpp"
    break;

  case 356: /* conjunction_expr: expr OR expr  */
#line 2756 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7088 "parser.cpp"
    break;

  case 357: /* between_expr: operand BETWEEN operand AND operand  */
#line 2765 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7100 "parser.cpp"
    break;

  case 358: /* in_expr: operand IN '(' expr_array ')'  */
#line 2773 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7111 "parser.cpp"
    break;

  case 359: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2779 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7122 "parser.cpp"
    break;

  case 360: /* case_expr: CASE expr case_check_array END  */
#line 2786 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7133 "parser.cpp"
    break;

  case 361: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2792 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7145 "parser.cpp"
    break;

  case 362: /* case_expr: CASE case_check_array END  */
#line 2799 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7155 "parser.cpp"
    break;

  case 363: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2804 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7166 "parser.cpp"
    break;

  case 364: /* case_check_array: WHEN expr THEN expr  */
#line 2811 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7178 "parser.cpp"
    break;

  case 365: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2818 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7190 "parser.cpp"
    break;

  case 366: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2826 "parser.y"
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
#line 7218 "parser.cpp"
    break;

  case 367: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2850 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7229 "parser.cpp"
    break;

  case 368: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2856 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7240 "parser.cpp"
    break;

  case 369: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2862 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7252 "parser.cpp"
    break;

  case 370: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2869 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7264 "parser.cpp"
    break;

  case 371: /* column_expr: IDENTIFIER  */
#line 2877 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7276 "parser.cpp"
    break;

  case 372: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2884 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7288 "parser.cpp"
    break;

  case 373: /* column_expr: '*'  */
#line 2891 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7298 "parser.cpp"
    break;

  case 374: /* column_expr: column_expr '.' '*'  */
#line 2896 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7312 "parser.cpp"
    break;

  case 375: /* constant_expr: STRING  */
#line 2906 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7322 "parser.cpp"
    break;

  case 376: /* constant_expr: TRUE  */
#line 2911 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7332 "parser.cpp"
    break;

  case 377: /* constant_expr: FALSE  */
#line 2916 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7342 "parser.cpp"
    break;

  case 378: /* constant_expr: DOUBLE_VALUE  */
#line 2921 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7352 "parser.cpp"
    break;

  case 379: /* constant_expr: LONG_VALUE  */
#line 2926 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7362 "parser.cpp"
    break;

  case 380: /* constant_expr: DATE STRING  */
#line 2931 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7372 "parser.cpp"
    break;

  case 381: /* constant_expr: TIME STRING  */
#line 2936 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7382 "parser.cpp"
    break;

  case 382: /* constant_expr: DATETIME STRING  */
#line 2941 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7392 "parser.cpp"
    break;

  case 383: /* constant_expr: TIMESTAMP STRING  */
#line 2946 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7402 "parser.cpp"
    break;

  case 384: /* constant_expr: INTERVAL interval_expr  */
#line 2951 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7410 "parser.cpp"
    break;

  case 385: /* constant_expr: interval_expr  */
#line 2954 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7418 "parser.cpp"
    break;

  case 386: /* constant_expr: common_array_expr  */
#line 2957 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7426 "parser.cpp"
    break;

  case 387: /* common_array_expr: array_expr  */
#line 2961 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7434 "parser.cpp"
    break;

  case 388: /* common_array_expr: subarray_array_expr  */
#line 2964 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7442 "parser.cpp"
    break;

  case 389: /* common_array_expr: sparse_array_expr  */
#line 2967 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7450 "parser.cpp"
    break;

  case 390: /* common_array_expr: empty_array_expr  */
#line 2970 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7458 "parser.cpp"
    break;

  case 391: /* common_sparse_array_expr: sparse_array_expr  */
#line 2974 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7466 "parser.cpp"
    break;

  case 392: /* common_sparse_array_expr: array_expr  */
#line 2977 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7474 "parser.cpp"
    break;

  case 393: /* common_sparse_array_expr: empty_array_expr  */
#line 2980 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7482 "parser.cpp"
    break;

  case 394: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2984 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7490 "parser.cpp"
    break;

  case 395: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2988 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7500 "parser.cpp"
    break;

  case 396: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 2993 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7509 "parser.cpp"
    break;

  case 397: /* sparse_array_expr: long_sparse_array_expr  */
#line 2998 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7517 "parser.cpp"
    break;

  case 398: /* sparse_array_expr: double_sparse_array_expr  */
#line 3001 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7525 "parser.cpp"
    break;

  case 399: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3005 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7533 "parser.cpp"
    break;

  case 400: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3009 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7545 "parser.cpp"
    break;

  case 401: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3016 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7556 "parser.cpp"
    break;

  case 402: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3023 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7564 "parser.cpp"
    break;

  case 403: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3027 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7576 "parser.cpp"
    break;

  case 404: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3034 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7587 "parser.cpp"
    break;

  case 405: /* empty_array_expr: '[' ']'  */
#line 3041 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7595 "parser.cpp"
    break;

  case 406: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3045 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7603 "parser.cpp"
    break;

  case 407: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3049 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7611 "parser.cpp"
    break;

  case 408: /* array_expr: long_array_expr  */
#line 3053 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7619 "parser.cpp"
    break;

  case 409: /* array_expr: double_array_expr  */
#line 3056 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7627 "parser.cpp"
    break;

  case 410: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3060 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7635 "parser.cpp"
    break;

  case 411: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3064 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7645 "parser.cpp"
    break;

  case 412: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3069 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7654 "parser.cpp"
    break;

  case 413: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3074 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7662 "parser.cpp"
    break;

  case 414: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3078 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7672 "parser.cpp"
    break;

  case 415: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3083 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7681 "parser.cpp"
    break;

  case 416: /* interval_expr: LONG_VALUE SECONDS  */
#line 3088 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7692 "parser.cpp"
    break;

  case 417: /* interval_expr: LONG_VALUE SECOND  */
#line 3094 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7703 "parser.cpp"
    break;

  case 418: /* interval_expr: LONG_VALUE MINUTES  */
#line 3100 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7714 "parser.cpp"
    break;

  case 419: /* interval_expr: LONG_VALUE MINUTE  */
#line 3106 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7725 "parser.cpp"
    break;

  case 420: /* interval_expr: LONG_VALUE HOURS  */
#line 3112 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7736 "parser.cpp"
    break;

  case 421: /* interval_expr: LONG_VALUE HOUR  */
#line 3118 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7747 "parser.cpp"
    break;

  case 422: /* interval_expr: LONG_VALUE DAYS  */
#line 3124 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7758 "parser.cpp"
    break;

  case 423: /* interval_expr: LONG_VALUE DAY  */
#line 3130 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7769 "parser.cpp"
    break;

  case 424: /* interval_expr: LONG_VALUE MONTHS  */
#line 3136 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7780 "parser.cpp"
    break;

  case 425: /* interval_expr: LONG_VALUE MONTH  */
#line 3142 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7791 "parser.cpp"
    break;

  case 426: /* interval_expr: LONG_VALUE YEARS  */
#line 3148 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7802 "parser.cpp"
    break;

  case 427: /* interval_expr: LONG_VALUE YEAR  */
#line 3154 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7813 "parser.cpp"
    break;

  case 428: /* copy_option_list: copy_option  */
#line 3165 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7822 "parser.cpp"
    break;

  case 429: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3169 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7831 "parser.cpp"
    break;

  case 430: /* copy_option: FORMAT IDENTIFIER  */
#line 3174 "parser.y"
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
#line 7866 "parser.cpp"
    break;

  case 431: /* copy_option: DELIMITER STRING  */
#line 3204 "parser.y"
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
#line 7881 "parser.cpp"
    break;

  case 432: /* copy_option: HEADER  */
#line 3214 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7891 "parser.cpp"
    break;

  case 433: /* copy_option: OFFSET LONG_VALUE  */
#line 3219 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 7901 "parser.cpp"
    break;

  case 434: /* copy_option: LIMIT LONG_VALUE  */
#line 3224 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 7911 "parser.cpp"
    break;

  case 435: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3229 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 7921 "parser.cpp"
    break;

  case 436: /* file_path: STRING  */
#line 3235 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7929 "parser.cpp"
    break;

  case 437: /* if_exists: IF EXISTS  */
#line 3239 "parser.y"
                     { (yyval.bool_value) = true; }
#line 7935 "parser.cpp"
    break;

  case 438: /* if_exists: %empty  */
#line 3240 "parser.y"
  { (yyval.bool_value) = false; }
#line 7941 "parser.cpp"
    break;

  case 439: /* if_not_exists: IF NOT EXISTS  */
#line 3242 "parser.y"
                              { (yyval.bool_value) = true; }
#line 7947 "parser.cpp"
    break;

  case 440: /* if_not_exists: %empty  */
#line 3243 "parser.y"
  { (yyval.bool_value) = false; }
#line 7953 "parser.cpp"
    break;

  case 443: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3258 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7966 "parser.cpp"
    break;

  case 444: /* if_not_exists_info: %empty  */
#line 3266 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 7974 "parser.cpp"
    break;

  case 445: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3270 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7982 "parser.cpp"
    break;

  case 446: /* with_index_param_list: %empty  */
#line 3273 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7990 "parser.cpp"
    break;

  case 447: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3277 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7998 "parser.cpp"
    break;

  case 448: /* optional_table_properties_list: %empty  */
#line 3280 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8006 "parser.cpp"
    break;

  case 449: /* index_param_list: index_param  */
#line 3284 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8015 "parser.cpp"
    break;

  case 450: /* index_param_list: index_param_list ',' index_param  */
#line 3288 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8024 "parser.cpp"
    break;

  case 451: /* index_param: IDENTIFIER  */
#line 3293 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8035 "parser.cpp"
    break;

  case 452: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3299 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8050 "parser.cpp"
    break;

  case 453: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3309 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8062 "parser.cpp"
    break;

  case 454: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3316 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8074 "parser.cpp"
    break;

  case 455: /* index_info_list: index_info_list_one_pack  */
#line 3327 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8083 "parser.cpp"
    break;

  case 456: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3331 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8095 "parser.cpp"
    break;

  case 457: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3339 "parser.y"
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
#line 8154 "parser.cpp"
    break;

  case 458: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3393 "parser.y"
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
#line 8172 "parser.cpp"
    break;


#line 8176 "parser.cpp"

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

#line 3407 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

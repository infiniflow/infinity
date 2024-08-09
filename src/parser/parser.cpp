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
  YYSYMBOL_MEMINDEX = 161,                 /* MEMINDEX  */
  YYSYMBOL_USING = 162,                    /* USING  */
  YYSYMBOL_SESSION = 163,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 164,                   /* GLOBAL  */
  YYSYMBOL_OFF = 165,                      /* OFF  */
  YYSYMBOL_EXPORT = 166,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 167,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 168,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 169,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 170,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 171,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 172,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 173,                    /* DELTA  */
  YYSYMBOL_LOGS = 174,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 175,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 176,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 177,                   /* SEARCH  */
  YYSYMBOL_MATCH = 178,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 179,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 180,                    /* QUERY  */
  YYSYMBOL_QUERIES = 181,                  /* QUERIES  */
  YYSYMBOL_FUSION = 182,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 183,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 184,                    /* ADMIN  */
  YYSYMBOL_PERSISTENCE = 185,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 186,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 187,                  /* OBJECTS  */
  YYSYMBOL_FILES = 188,                    /* FILES  */
  YYSYMBOL_NUMBER = 189,                   /* NUMBER  */
  YYSYMBOL_190_ = 190,                     /* '='  */
  YYSYMBOL_191_ = 191,                     /* '<'  */
  YYSYMBOL_192_ = 192,                     /* '>'  */
  YYSYMBOL_193_ = 193,                     /* '+'  */
  YYSYMBOL_194_ = 194,                     /* '-'  */
  YYSYMBOL_195_ = 195,                     /* '*'  */
  YYSYMBOL_196_ = 196,                     /* '/'  */
  YYSYMBOL_197_ = 197,                     /* '%'  */
  YYSYMBOL_198_ = 198,                     /* '['  */
  YYSYMBOL_199_ = 199,                     /* ']'  */
  YYSYMBOL_200_ = 200,                     /* '('  */
  YYSYMBOL_201_ = 201,                     /* ')'  */
  YYSYMBOL_202_ = 202,                     /* '.'  */
  YYSYMBOL_203_ = 203,                     /* ';'  */
  YYSYMBOL_204_ = 204,                     /* ','  */
  YYSYMBOL_205_ = 205,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 206,                 /* $accept  */
  YYSYMBOL_input_pattern = 207,            /* input_pattern  */
  YYSYMBOL_statement_list = 208,           /* statement_list  */
  YYSYMBOL_statement = 209,                /* statement  */
  YYSYMBOL_explainable_statement = 210,    /* explainable_statement  */
  YYSYMBOL_create_statement = 211,         /* create_statement  */
  YYSYMBOL_table_element_array = 212,      /* table_element_array  */
  YYSYMBOL_table_element = 213,            /* table_element  */
  YYSYMBOL_table_column = 214,             /* table_column  */
  YYSYMBOL_column_type = 215,              /* column_type  */
  YYSYMBOL_column_constraints = 216,       /* column_constraints  */
  YYSYMBOL_column_constraint = 217,        /* column_constraint  */
  YYSYMBOL_default_expr = 218,             /* default_expr  */
  YYSYMBOL_table_constraint = 219,         /* table_constraint  */
  YYSYMBOL_identifier_array = 220,         /* identifier_array  */
  YYSYMBOL_delete_statement = 221,         /* delete_statement  */
  YYSYMBOL_insert_statement = 222,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 223, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 224,        /* explain_statement  */
  YYSYMBOL_explain_type = 225,             /* explain_type  */
  YYSYMBOL_update_statement = 226,         /* update_statement  */
  YYSYMBOL_update_expr_array = 227,        /* update_expr_array  */
  YYSYMBOL_update_expr = 228,              /* update_expr  */
  YYSYMBOL_drop_statement = 229,           /* drop_statement  */
  YYSYMBOL_copy_statement = 230,           /* copy_statement  */
  YYSYMBOL_select_statement = 231,         /* select_statement  */
  YYSYMBOL_select_with_paren = 232,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 233,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 234, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 235, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 236, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 237,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 238,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 239,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 240,            /* order_by_type  */
  YYSYMBOL_limit_expr = 241,               /* limit_expr  */
  YYSYMBOL_offset_expr = 242,              /* offset_expr  */
  YYSYMBOL_distinct = 243,                 /* distinct  */
  YYSYMBOL_from_clause = 244,              /* from_clause  */
  YYSYMBOL_search_clause = 245,            /* search_clause  */
  YYSYMBOL_where_clause = 246,             /* where_clause  */
  YYSYMBOL_having_clause = 247,            /* having_clause  */
  YYSYMBOL_group_by_clause = 248,          /* group_by_clause  */
  YYSYMBOL_set_operator = 249,             /* set_operator  */
  YYSYMBOL_table_reference = 250,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 251,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 252,     /* table_reference_name  */
  YYSYMBOL_table_name = 253,               /* table_name  */
  YYSYMBOL_table_alias = 254,              /* table_alias  */
  YYSYMBOL_with_clause = 255,              /* with_clause  */
  YYSYMBOL_with_expr_list = 256,           /* with_expr_list  */
  YYSYMBOL_with_expr = 257,                /* with_expr  */
  YYSYMBOL_join_clause = 258,              /* join_clause  */
  YYSYMBOL_join_type = 259,                /* join_type  */
  YYSYMBOL_show_statement = 260,           /* show_statement  */
  YYSYMBOL_flush_statement = 261,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 262,       /* optimize_statement  */
  YYSYMBOL_command_statement = 263,        /* command_statement  */
  YYSYMBOL_compact_statement = 264,        /* compact_statement  */
  YYSYMBOL_admin_statement = 265,          /* admin_statement  */
  YYSYMBOL_expr_array = 266,               /* expr_array  */
  YYSYMBOL_expr_array_list = 267,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 268,               /* expr_alias  */
  YYSYMBOL_expr = 269,                     /* expr  */
  YYSYMBOL_operand = 270,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 271, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 272,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 273,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 274,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 275,          /* match_text_expr  */
  YYSYMBOL_query_expr = 276,               /* query_expr  */
  YYSYMBOL_fusion_expr = 277,              /* fusion_expr  */
  YYSYMBOL_sub_search = 278,               /* sub_search  */
  YYSYMBOL_sub_search_array = 279,         /* sub_search_array  */
  YYSYMBOL_function_expr = 280,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 281,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 282,             /* between_expr  */
  YYSYMBOL_in_expr = 283,                  /* in_expr  */
  YYSYMBOL_case_expr = 284,                /* case_expr  */
  YYSYMBOL_case_check_array = 285,         /* case_check_array  */
  YYSYMBOL_cast_expr = 286,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 287,            /* subquery_expr  */
  YYSYMBOL_column_expr = 288,              /* column_expr  */
  YYSYMBOL_constant_expr = 289,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 290,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 291, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 292,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 293, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 294,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 295,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 296, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 297, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 298, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 299,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 300,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 301,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 302,               /* array_expr  */
  YYSYMBOL_long_array_expr = 303,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 304, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 305,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 306, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 307,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 308,         /* copy_option_list  */
  YYSYMBOL_copy_option = 309,              /* copy_option  */
  YYSYMBOL_file_path = 310,                /* file_path  */
  YYSYMBOL_if_exists = 311,                /* if_exists  */
  YYSYMBOL_if_not_exists = 312,            /* if_not_exists  */
  YYSYMBOL_semicolon = 313,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 314,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 315,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 316, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 317,         /* index_param_list  */
  YYSYMBOL_index_param = 318,              /* index_param  */
  YYSYMBOL_index_info_list = 319,          /* index_info_list  */
  YYSYMBOL_index_info_list_one_pack = 320  /* index_info_list_one_pack  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 449 "parser.cpp"

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
#define YYFINAL  98
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1239

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  206
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  475
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1014

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   444


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
       2,     2,     2,     2,     2,     2,     2,   197,     2,     2,
     200,   201,   195,   193,   204,   194,   202,   196,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   205,   203,
     191,   190,   192,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   198,     2,   199,     2,     2,     2,     2,     2,     2,
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   497,   497,   501,   507,   514,   515,   516,   517,   518,
     519,   520,   521,   522,   523,   524,   525,   526,   527,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   547,   564,   580,   609,   625,   643,   672,   676,   682,
     685,   692,   742,   778,   779,   780,   781,   782,   783,   784,
     785,   786,   787,   788,   789,   790,   791,   792,   793,   794,
     795,   796,   797,   798,   801,   803,   804,   805,   806,   809,
     810,   811,   812,   813,   814,   815,   816,   817,   818,   819,
     820,   821,   822,   823,   824,   825,   826,   827,   828,   829,
     830,   831,   832,   833,   834,   835,   836,   837,   838,   839,
     840,   841,   842,   843,   844,   845,   846,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   856,   857,   858,   859,
     860,   861,   862,   863,   882,   886,   896,   899,   902,   905,
     909,   912,   917,   922,   929,   935,   945,   961,   995,  1008,
    1011,  1018,  1024,  1027,  1030,  1033,  1036,  1039,  1042,  1045,
    1052,  1065,  1069,  1074,  1087,  1100,  1115,  1130,  1145,  1168,
    1221,  1276,  1327,  1330,  1333,  1342,  1352,  1355,  1359,  1364,
    1386,  1389,  1394,  1410,  1413,  1417,  1421,  1426,  1432,  1435,
    1438,  1442,  1446,  1448,  1452,  1454,  1457,  1461,  1464,  1468,
    1473,  1477,  1480,  1484,  1487,  1491,  1494,  1498,  1501,  1504,
    1507,  1515,  1518,  1533,  1533,  1535,  1549,  1558,  1563,  1572,
    1577,  1582,  1588,  1595,  1598,  1602,  1605,  1610,  1622,  1629,
    1643,  1646,  1649,  1652,  1655,  1658,  1661,  1667,  1671,  1675,
    1679,  1683,  1690,  1694,  1698,  1702,  1706,  1711,  1715,  1720,
    1724,  1728,  1734,  1740,  1746,  1757,  1768,  1779,  1791,  1803,
    1816,  1830,  1841,  1855,  1871,  1888,  1892,  1896,  1900,  1904,
    1908,  1918,  1922,  1926,  1934,  1945,  1968,  1974,  1979,  1985,
    1991,  1999,  2005,  2011,  2017,  2023,  2031,  2037,  2043,  2049,
    2055,  2063,  2069,  2076,  2089,  2093,  2098,  2104,  2111,  2119,
    2128,  2138,  2148,  2159,  2170,  2182,  2194,  2204,  2215,  2227,
    2240,  2244,  2249,  2254,  2265,  2269,  2274,  2278,  2305,  2311,
    2315,  2316,  2317,  2318,  2319,  2321,  2324,  2330,  2333,  2334,
    2335,  2336,  2337,  2338,  2339,  2340,  2341,  2342,  2344,  2347,
    2353,  2372,  2414,  2460,  2478,  2496,  2504,  2515,  2521,  2530,
    2536,  2548,  2551,  2554,  2557,  2560,  2563,  2567,  2571,  2576,
    2584,  2592,  2601,  2608,  2615,  2622,  2629,  2636,  2644,  2652,
    2660,  2668,  2676,  2684,  2692,  2700,  2708,  2716,  2724,  2732,
    2762,  2770,  2779,  2787,  2796,  2804,  2810,  2817,  2823,  2830,
    2835,  2842,  2849,  2857,  2881,  2887,  2893,  2900,  2908,  2915,
    2922,  2927,  2937,  2942,  2947,  2952,  2957,  2962,  2967,  2972,
    2977,  2982,  2985,  2988,  2992,  2995,  2998,  3001,  3005,  3008,
    3011,  3015,  3019,  3024,  3029,  3032,  3036,  3040,  3047,  3054,
    3058,  3065,  3072,  3076,  3080,  3084,  3087,  3091,  3095,  3100,
    3105,  3109,  3114,  3119,  3125,  3131,  3137,  3143,  3149,  3155,
    3161,  3167,  3173,  3179,  3185,  3196,  3200,  3205,  3235,  3245,
    3250,  3255,  3260,  3266,  3270,  3271,  3273,  3274,  3276,  3277,
    3289,  3297,  3301,  3304,  3308,  3311,  3315,  3319,  3324,  3330,
    3340,  3347,  3358,  3362,  3370,  3424
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
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN",
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "NUMBER", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'",
  "'.'", "';'", "','", "':'", "$accept", "input_pattern", "statement_list",
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

#define YYPACT_NINF (-532)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-462)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     647,   257,    87,   263,    66,    68,    66,    89,   384,   582,
      59,   133,   132,   155,   169,    24,   187,   -45,   235,    47,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   320,  -532,
    -532,   256,  -532,  -532,  -532,  -532,  -532,  -532,   185,   185,
     185,   185,    57,    66,   211,   211,   211,   211,   211,    98,
     327,    66,   -13,   342,   348,   427,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,   544,   438,    66,  -532,  -532,  -532,  -532,
    -532,   411,  -532,    56,   145,  -532,   440,  -532,   275,  -532,
    -532,   293,  -532,   121,    66,  -532,  -532,  -532,  -532,   -32,
    -532,   421,   258,  -532,   480,    62,   287,   309,  -532,    67,
    -532,   497,  -532,  -532,     2,   462,  -532,   467,   456,   537,
      66,    66,    66,   538,   478,   346,   471,   549,    66,    66,
      66,   559,   565,   569,   508,   576,   576,   489,    58,    65,
      81,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   320,  -532,
    -532,  -532,  -532,  -532,  -532,   224,  -532,  -532,   579,  -532,
     581,  -532,  -532,   584,   590,  -532,  -532,  -532,    66,   388,
     169,   576,   589,  -532,  -532,   600,  -532,  -532,  -532,  -532,
       2,  -532,  -532,  -532,   489,   541,   555,   552,  -532,   -36,
    -532,   346,  -532,    66,   616,    15,  -532,  -532,  -532,  -532,
    -532,   575,  -532,   439,   -35,  -532,   489,  -532,  -532,   560,
     564,   445,  -532,  -532,   722,   604,   446,   447,   350,   648,
     649,   651,   661,  -532,  -532,   660,   468,   210,   475,   476,
     627,   627,  -532,    18,   405,  -116,  -532,    -9,   645,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,   479,  -532,  -532,  -532,  -144,  -532,  -532,  -142,
    -532,   -98,  -532,  -532,  -532,   -76,  -532,    27,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,   674,   672,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,   605,   256,  -532,  -532,   153,   680,   490,
     491,   -52,   489,   489,   622,  -532,   -45,    31,   637,   495,
    -532,   -99,   496,  -532,    66,   489,   569,  -532,   176,   498,
     499,   193,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,   627,   500,   726,   617,   489,   489,
      72,   261,  -532,  -532,  -532,  -532,   722,  -532,   697,   503,
     504,   505,   509,   710,   711,   163,   163,  -532,   511,  -532,
    -532,  -532,  -532,   516,    48,   639,   489,   716,   489,   489,
     -25,   521,   -21,   627,   627,   627,   627,   627,   627,   627,
     627,   627,   627,   627,   627,   627,   627,    14,  -532,   525,
    -532,   718,  -532,   719,  -532,   720,  -532,   723,   686,   328,
     533,  -532,   546,   728,  -532,    30,  -532,  -532,    11,   562,
     545,  -532,    41,   176,   489,  -532,   320,   855,   621,   553,
       3,  -532,  -532,  -532,   -45,   616,   495,  -532,  -532,   751,
     489,   561,  -532,   176,  -532,   191,   191,   489,  -532,    77,
     617,   609,   567,    -4,   -41,   359,  -532,   489,   489,   688,
     489,   765,    19,   489,   120,   148,   391,  -532,  -532,   576,
    -532,  -532,  -532,   618,   572,   627,   405,   644,  -532,   747,
     747,   173,   173,   696,   747,   747,   173,   173,   163,   163,
    -532,  -532,  -532,  -532,  -532,  -532,   571,  -532,   573,  -532,
    -532,  -532,   768,   771,  -532,   776,  -532,  -532,   774,  -532,
     -45,   583,   410,  -532,    53,  -532,   277,   508,   489,  -532,
    -532,  -532,   176,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,   585,  -532,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,   588,   591,   603,
     606,   608,   154,   610,   616,   755,    31,   320,   203,  -532,
    -532,   214,   611,   783,   784,   808,   811,  -532,   809,   234,
    -532,   236,   246,  -532,   615,  -532,   855,   489,  -532,   489,
     -49,   -37,   627,   -54,   613,  -532,   227,    36,  -532,   814,
    -532,   815,  -532,  -532,   743,   405,   747,   623,   260,  -532,
     627,   822,   824,   777,   782,   643,   273,  -532,   828,     5,
      11,   790,  -532,  -532,  -532,  -532,  -532,  -532,   791,  -532,
     849,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   650,
     799,  -532,   863,   307,   428,   543,   890,   907,   741,   744,
    -532,  -532,    25,  -532,   724,   616,   286,   678,  -532,  -532,
     717,  -532,   489,  -532,  -532,  -532,  -532,  -532,  -532,   191,
    -532,  -532,  -532,   671,   176,   -12,  -532,   489,   188,   682,
     878,   525,   685,   683,   684,   698,   699,   310,  -532,  -532,
     726,   893,   895,   251,  -532,   776,   213,    53,   410,    11,
      11,   703,   277,   850,   848,   311,   702,   704,   705,   706,
     707,   708,   709,   721,   725,   821,   727,   729,   730,   731,
     732,   758,   766,   769,   772,   773,   831,   786,   787,   788,
     789,   800,   803,   804,   805,   806,   812,   833,   813,   816,
     817,   818,   819,   820,   823,   825,   826,   827,   834,   829,
     830,   832,   835,   836,   837,   838,   839,   840,   841,   876,
     842,  -532,  -532,    34,  -532,  -532,  -532,   362,  -532,   776,
     904,   373,  -532,  -532,  -532,   176,  -532,   460,   843,   377,
     844,    13,   845,  -532,  -532,  -532,  -532,  -532,   191,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,   909,  -532,  -532,
    -532,   864,   616,  -532,   489,   489,  -532,  -532,   924,   926,
     966,   980,   981,   988,   997,  1009,  1012,  1013,   846,  1019,
    1020,  1026,  1029,  1031,  1032,  1045,  1046,  1047,  1048,   851,
    1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,  1059,
     862,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,  1069,
    1070,   873,  1072,  1073,  1074,  1075,  1076,  1077,  1078,  1079,
    1080,  1081,   884,  1083,  -532,  -532,   385,   605,  -532,  -532,
    1086,  -532,  1087,  1088,  1089,   400,  1090,   489,   412,   891,
     176,   896,   897,   898,   899,   900,   901,   902,   903,   905,
     906,  1099,   908,   910,   911,   912,   913,   914,   915,   916,
     917,   918,  1102,   919,   920,   921,   922,   923,   925,   927,
     928,   929,   930,  1104,   931,   932,   933,   934,   935,   936,
     937,   938,   939,   940,  1119,   941,   942,   943,   944,   945,
     946,   947,   948,   949,   950,  1121,   951,  -532,  -532,   952,
     953,   954,   414,  -532,   268,   176,  -532,  -532,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,   956,  -532,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   958,  -532,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   959,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
     960,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,
    -532,   961,  -532,  1149,  -532,  1151,   605,  1157,  1158,  1159,
    -532,  -532,  -532,  -532,  -532,  -532,  -532,  -532,   416,   962,
    -532,   967,  1161,   457,   605,  1163,  1166,   970,   605,   470,
    1167,  -532,  -532,   971,  -532,  -532,  -532,  1168,  -532,  1043,
     605,  -532,  -532,  -532
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     214,     0,     0,     0,     0,     0,     0,     0,   149,     0,
       0,     0,     0,     0,     0,     0,     0,   214,     0,   459,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   163,
     162,     0,     8,    14,    15,    16,    17,    18,   457,   457,
     457,   457,   457,     0,   455,   455,   455,   455,   455,   207,
       0,     0,     0,     0,     0,     0,   143,   147,   144,   145,
     146,   148,   142,   214,     0,     0,   228,   229,   227,   233,
     237,     0,   234,     0,     0,   230,     0,   232,     0,   255,
     257,     0,   235,     0,     0,   261,   262,   263,   266,   207,
     264,     0,   213,   215,     0,     0,     0,     0,     1,   214,
       2,   197,   199,   200,     0,   186,   168,   174,     0,     0,
       0,     0,     0,     0,     0,   140,     0,     0,     0,     0,
       0,     0,     0,     0,   192,     0,     0,     0,     0,     0,
       0,   141,    19,    24,    26,    25,    20,    21,    23,    22,
      27,    28,    29,    30,   243,   244,   238,   239,     0,   240,
       0,   231,   256,     0,     0,   259,   258,   283,     0,     0,
       0,     0,     0,   300,   284,     0,   167,   166,     4,   198,
       0,   164,   165,   185,     0,     0,   182,     0,    31,     0,
      32,   140,   460,     0,     0,   214,   454,   154,   156,   155,
     157,     0,   208,     0,   192,   151,     0,   136,   453,     0,
       0,   388,   392,   395,   396,     0,     0,     0,     0,     0,
       0,     0,     0,   393,   394,     0,     0,     0,     0,     0,
       0,     0,   390,     0,   214,     0,   304,   309,   310,   324,
     322,   325,   323,   326,   327,   319,   314,   313,   312,   320,
     321,   311,   318,   317,   403,   405,     0,   406,   414,     0,
     415,     0,   407,   404,   425,     0,   426,     0,   402,   270,
     272,   271,   268,   269,   275,   277,   276,   273,   274,   280,
     282,   281,   278,   279,     0,     0,   246,   245,   251,   241,
     242,   236,   260,   463,     0,   216,   267,   301,   285,     0,
       0,   188,     0,     0,   184,   456,   214,     0,     0,     0,
     134,     0,     0,   138,     0,     0,     0,   150,   191,     0,
       0,     0,   434,   433,   436,   435,   438,   437,   440,   439,
     442,   441,   444,   443,     0,     0,   354,   214,     0,     0,
       0,     0,   397,   398,   399,   400,     0,   401,     0,     0,
       0,     0,     0,     0,     0,   356,   355,   431,   428,   422,
     412,   417,   420,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   411,     0,
     416,     0,   419,     0,   427,     0,   430,     0,   252,   247,
       0,   265,     0,     0,   302,     0,   171,   170,     0,   190,
     173,   175,   180,   181,     0,   169,    34,     0,     0,     0,
       0,    37,    39,    40,   214,     0,    36,   472,   139,     0,
       0,   137,   158,   153,   152,     0,     0,     0,   349,     0,
     214,     0,     0,     0,     0,     0,   379,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   316,   315,     0,
     305,   308,   372,   373,     0,     0,   214,     0,   353,   363,
     364,   367,   368,     0,   370,   362,   365,   366,   358,   357,
     359,   360,   361,   389,   391,   413,     0,   418,     0,   421,
     429,   432,     0,     0,   248,     0,   217,   303,     0,   286,
     214,   187,   201,   203,   212,   204,     0,   192,     0,   178,
     179,   177,   183,    43,    46,    47,    44,    45,    48,    49,
      65,    50,    52,    51,    68,    55,    56,    57,    53,    54,
      58,    59,    60,    61,    62,    63,    64,     0,     0,     0,
       0,     0,   463,     0,     0,   465,     0,    35,     0,   473,
     135,     0,     0,     0,     0,     0,     0,   449,     0,     0,
     445,     0,     0,   350,     0,   384,     0,     0,   377,     0,
       0,     0,     0,     0,     0,   388,     0,     0,   337,     0,
     339,     0,   424,   423,     0,   214,   371,     0,     0,   352,
       0,     0,     0,   253,   249,   468,     0,   466,   287,     0,
       0,     0,   221,   222,   223,   224,   220,   225,     0,   210,
       0,   205,   343,   341,   344,   342,   345,   346,   347,   189,
     196,   176,     0,     0,     0,     0,     0,     0,     0,     0,
     127,   128,   131,   124,   131,     0,     0,     0,    33,    38,
     475,   306,     0,   451,   450,   448,   447,   452,   161,     0,
     159,   351,   385,     0,   381,     0,   380,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   386,   375,
     374,     0,     0,     0,   462,     0,     0,   212,   202,     0,
       0,   209,     0,     0,   194,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   129,   126,     0,   125,    42,    41,     0,   133,     0,
       0,     0,   446,   383,   378,   382,   369,     0,     0,     0,
       0,     0,     0,   408,   410,   409,   338,   340,     0,   387,
     376,   254,   250,   469,   471,   470,   467,     0,   288,   206,
     218,     0,     0,   348,     0,     0,   172,    67,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   130,   132,     0,   463,   307,   428,
       0,   335,     0,     0,     0,     0,   289,     0,     0,   195,
     193,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   464,   474,     0,
       0,     0,     0,   160,     0,   219,   211,    66,    72,    73,
      70,    71,    74,    75,    76,    77,    78,     0,    69,   105,
     106,   103,   104,   107,   108,   109,   110,   111,     0,   102,
      83,    84,    81,    82,    85,    86,    87,    88,    89,     0,
      80,   116,   117,   114,   115,   118,   119,   120,   121,   122,
       0,   113,    94,    95,    92,    93,    96,    97,    98,    99,
     100,     0,    91,     0,   336,     0,   463,     0,     0,     0,
     291,   290,   296,    79,   112,    90,   123,   101,     0,   329,
     334,     0,   297,   292,   463,     0,     0,     0,   463,     0,
       0,   293,   332,     0,   328,   330,   333,     0,   298,   294,
     463,   299,   295,   331
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -532,  -532,  -532,  1082,  -532,  1112,  -532,   640,  -532,   624,
    -532,   556,   558,  -532,  -410,  1114,  1116,  1002,  -532,  -532,
    1122,  -532,   880,  1124,  1125,   -57,  1172,   -14,   955,  1014,
     -63,  -532,  -532,   692,  -532,  -532,  -532,  -532,  -532,  -532,
    -185,  -532,  -532,  -532,  -532,   601,  -145,     7,   526,  -532,
    -532,  1034,  -532,  -532,  1129,  1132,  1133,  1134,  1135,  -532,
    -172,  -532,   847,  -196,  -190,  -532,  -480,  -459,  -454,  -452,
    -451,  -450,   527,  -532,  -532,  -532,  -532,  -532,  -532,   870,
    -532,  -532,   759,   469,  -219,  -532,  -532,  -532,   554,  -532,
    -532,  -532,  -532,   557,   852,   853,  -257,  -532,  -532,  -532,
    -532,   989,  -419,   566,  -118,   451,   461,  -532,  -532,  -531,
    -532,   472,   542,  -532,   792
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   131,    21,   410,   411,   412,   532,
     622,   623,   735,   413,   301,    22,    23,   185,    24,    63,
      25,   194,   195,    26,    27,    28,    29,    30,   106,   171,
     107,   176,   400,   401,   501,   294,   405,   174,   399,   497,
     197,   776,   674,   104,   491,   492,   493,   494,   601,    31,
      92,    93,   495,   598,    32,    33,    34,    35,    36,    37,
     225,   421,   226,   227,   228,   997,   229,   230,   231,   232,
     233,   234,   608,   609,   235,   236,   237,   238,   239,   331,
     240,   241,   242,   243,   244,   752,   245,   246,   247,   248,
     249,   250,   251,   252,   351,   352,   253,   254,   255,   256,
     257,   258,   549,   550,   199,   117,   109,   100,   114,   391,
     628,   586,   587,   416,   417
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     308,   624,   291,    97,   350,   538,   138,   551,   200,   307,
     105,    50,   330,    52,    49,   326,   602,   473,   347,   348,
      90,   296,   565,   347,   348,   101,   398,   102,   354,   103,
     345,   346,   158,   196,   407,    14,   646,   603,   202,   203,
     204,   172,   604,   286,   605,   606,   607,   557,   357,   302,
     115,   647,   454,   556,   457,   378,   599,   380,   124,   488,
     379,   259,   381,   260,   261,   125,   126,  -458,   264,    49,
     265,   266,   145,   744,     1,   489,     2,     3,     4,     5,
       6,     7,     8,     9,   269,   355,   270,   271,   356,    84,
      10,   157,    11,    12,    13,    14,   402,   403,   358,   359,
     618,   382,   418,   358,   359,   419,   383,   290,   458,   423,
     600,   358,   359,   499,   500,   358,   359,   179,   180,   181,
      43,  -461,   262,   384,   626,   188,   189,   190,   385,   267,
     455,   108,   433,   434,   326,    88,   209,   210,   211,   429,
     358,   359,   212,   358,   359,   272,    51,    14,   358,   359,
     649,   619,   356,   620,   621,    17,   733,   408,    89,   409,
     475,   329,   452,   453,   297,   283,   213,   214,   215,   306,
     122,   303,    91,   459,   460,   461,   462,   463,   464,   465,
     466,   467,   468,   469,   470,   471,   472,   127,   358,   359,
     299,    94,   602,   358,   359,   393,   201,   202,   203,   204,
     358,   359,   170,    95,   535,   394,   667,   536,   502,   474,
     353,   490,   349,   603,   222,   737,   223,   349,   604,   162,
     605,   606,   607,   263,   358,   359,   386,   147,   148,   618,
     268,   387,   223,    15,   390,    98,   163,   164,   165,   406,
     652,   560,   561,   767,   563,   768,   273,   567,   541,   448,
      99,    16,    53,    54,   763,   552,   764,   765,    55,   108,
     427,   543,   544,   431,   105,   576,   274,    17,   205,   206,
     275,   276,   545,   546,   547,   277,   278,   207,   553,   208,
     619,   356,   620,   621,   578,   116,    38,    39,    40,    85,
      86,    87,    44,    45,    46,   209,   210,   211,    41,    42,
     122,   212,   402,   362,    47,    48,   908,   154,   155,   156,
     978,   422,   610,   432,   979,   980,   149,   150,   362,   981,
     982,   568,  -462,  -462,   569,   213,   214,   215,   358,   359,
     339,   574,   340,   341,   342,   363,   364,   365,   366,   845,
     101,   123,   102,   368,   103,   128,   436,   216,   437,   570,
     438,   129,   571,   201,   202,   203,   204,   537,   374,   375,
     376,   644,   848,   645,  -462,  -462,   372,   373,   374,   375,
     376,   217,   648,   218,   548,   219,   483,   484,   369,   370,
     371,   372,   373,   374,   375,   376,   220,   221,   222,   746,
     660,   223,   748,   224,   428,   755,   572,   573,   676,   677,
     678,   679,   680,   657,   630,   681,   682,   419,   201,   202,
     203,   204,   683,   684,   685,   631,   554,   146,   356,    56,
      57,    58,    59,    60,    61,   205,   206,    62,   686,   377,
     130,   651,   750,   589,   207,   638,   208,   640,   639,   329,
     639,   144,   577,   151,   558,   990,   559,   641,   438,   152,
     356,   745,   209,   210,   211,   217,   153,   218,   212,   219,
     741,   659,   160,  1002,   356,   347,   839,  1006,   591,  -226,
     592,   593,   594,   595,   664,   596,   597,   665,   159,  1013,
     205,   206,   213,   214,   215,    14,   161,   738,   166,   207,
     419,   208,   201,   202,   203,   204,   118,   119,   120,   121,
     110,   111,   112,   113,   216,  1000,  1001,   209,   210,   211,
     167,   760,   777,   212,   356,   778,  1007,  1008,   169,   687,
     688,   689,   690,   691,   770,   771,   692,   693,   217,   173,
     218,   177,   219,   694,   695,   696,   175,   213,   214,   215,
     178,   182,   183,   220,   221,   222,   184,   186,   223,   697,
     224,     1,   187,     2,     3,     4,     5,     6,     7,   216,
       9,   656,   191,   835,   205,   206,   419,    10,   192,    11,
      12,    13,   193,   207,   838,   208,   196,   356,   841,   850,
     198,   842,   279,   217,   280,   218,   907,   219,   284,   665,
     281,   209,   210,   211,   282,   287,   292,   212,   220,   221,
     222,   913,   849,   223,   639,   224,   288,   201,   202,   203,
     204,    64,    65,   916,    66,   976,   419,   994,   977,   300,
     995,   213,   214,   215,    14,   293,    67,    68,   295,   305,
     201,   202,   203,   204,   698,   699,   700,   701,   702,   304,
     309,   703,   704,   216,   310,   311,   327,   328,   705,   706,
     707,   915,   332,   333,     1,   334,     2,     3,     4,     5,
       6,     7,     8,     9,   708,   335,   336,   217,   338,   218,
      10,   219,    11,    12,    13,   343,   344,   388,   389,   324,
     325,   377,   220,   221,   222,   390,   395,   223,   207,   224,
     208,   396,   397,   404,   414,   415,   420,    14,   425,   426,
     430,   439,   324,   440,   441,   442,   209,   210,   211,   443,
      15,   207,   212,   208,   444,   445,   446,   447,   449,   451,
     360,   456,   361,   223,   476,   478,   480,    14,   481,   209,
     210,   211,   482,   485,   487,   212,   213,   214,   215,   496,
      69,    70,    71,    72,    17,    73,    74,   486,   533,   498,
      75,    76,    77,   534,   540,    78,    79,    80,   216,   213,
     214,   215,    81,    82,   455,   542,   562,    83,   555,   564,
     358,   431,   575,   579,   583,   362,   581,   584,   582,   585,
     588,   216,   217,   627,   218,   612,   219,   590,   613,   633,
     634,   614,   363,   364,   365,   366,   367,   220,   221,   222,
     368,   431,   223,   615,   224,   217,   616,   218,   617,   219,
     625,   632,   635,    15,   636,   637,   642,   650,   653,   654,
     220,   221,   222,   655,   658,   223,   362,   224,   573,   572,
     661,    16,   662,   663,   666,   369,   370,   371,   372,   373,
     374,   375,   376,   363,   364,   365,   366,    17,   580,   669,
     670,   368,   671,   673,   672,   733,   362,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   675,
     731,   732,   743,   363,   364,   365,   366,   362,   739,   740,
     747,   368,   749,   751,   756,   757,   369,   370,   371,   372,
     373,   374,   375,   376,  -462,  -462,   365,   366,   758,   761,
     759,   762,  -462,   772,   775,   774,   779,   837,   780,   781,
     782,   783,   784,   785,   788,   846,   369,   370,   371,   372,
     373,   374,   375,   376,   799,   786,   810,   821,   847,   787,
     851,   789,   852,   790,   791,   792,   793,  -462,   370,   371,
     372,   373,   374,   375,   376,   503,   504,   505,   506,   507,
     508,   509,   510,   511,   512,   513,   514,   515,   516,   517,
     518,   519,   794,   520,   521,   522,   523,   524,   525,   832,
     795,   526,   853,   796,   527,   528,   797,   798,   529,   530,
     531,   709,   710,   711,   712,   713,   854,   855,   714,   715,
     800,   801,   802,   803,   856,   716,   717,   718,   720,   721,
     722,   723,   724,   857,   804,   725,   726,   805,   806,   807,
     808,   719,   727,   728,   729,   858,   809,   811,   859,   860,
     812,   813,   814,   815,   816,   862,   863,   817,   730,   818,
     819,   820,   864,   822,   823,   865,   824,   866,   867,   825,
     826,   827,   828,   829,   830,   831,   833,   840,   843,   844,
     861,   868,   869,   870,   871,   872,   873,   874,   875,   876,
     877,   878,   879,   880,   881,   882,   883,   884,   885,   886,
     887,   888,   889,   890,   891,   892,   893,   894,   895,   896,
     897,   898,   899,   900,   901,   902,   903,   904,   905,   906,
     909,   910,   911,   912,  1012,   356,   914,   917,   918,   919,
     920,   921,   922,   923,   924,   927,   925,   926,   938,   928,
     949,   929,   930,   931,   932,   933,   934,   935,   936,   937,
     939,   940,   941,   942,   943,   960,   944,   971,   945,   946,
     947,   948,   950,   951,   952,   953,   954,   955,   956,   957,
     958,   959,   961,   962,   963,   964,   965,   966,   967,   968,
     969,   970,   972,   988,   974,   989,   973,   983,   975,   984,
     985,   986,   987,   991,   992,   993,   996,   999,   998,  1003,
    1004,  1005,  1010,  1009,  1011,   132,   629,   133,   734,   134,
     643,   168,   736,   298,   289,   135,   424,   136,   137,    96,
     611,   668,   139,   769,   285,   140,   141,   142,   143,   773,
     435,   566,   834,   450,   337,   742,   753,   766,   539,   754,
       0,   836,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   477,     0,     0,   479,     0,     0,   392
};

static const yytype_int16 yycheck[] =
{
     196,   532,   174,    17,   223,   415,    63,   426,   126,   194,
       8,     4,   208,     6,     3,   205,   496,     3,     5,     6,
      13,    57,     3,     5,     6,    20,    78,    22,   224,    24,
     220,   221,    64,    68,     3,    80,    85,   496,     4,     5,
       6,   104,   496,   161,   496,   496,   496,    88,    57,    34,
      43,    88,    77,    57,    75,   199,     3,   199,    51,    29,
     204,     3,   204,     5,     6,    78,    79,     0,     3,     3,
       5,     6,    65,    85,     7,    45,     9,    10,    11,    12,
      13,    14,    15,    16,     3,   201,     5,     6,   204,    30,
      23,    84,    25,    26,    27,    80,   292,   293,   152,   153,
      75,   199,   201,   152,   153,   204,   204,   170,   129,   305,
      57,   152,   153,    72,    73,   152,   153,   110,   111,   112,
      33,    64,    64,   199,   534,   118,   119,   120,   204,    64,
     155,    74,   328,   329,   324,     3,   102,   103,   104,   311,
     152,   153,   108,   152,   153,    64,    78,    80,   152,   153,
     204,   126,   204,   128,   129,   200,   131,   126,     3,   128,
     379,    89,   358,   359,   200,   158,   132,   133,   134,   204,
     202,   185,     3,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   200,   152,   153,
     183,   167,   672,   152,   153,    42,     3,     4,     5,     6,
     152,   153,   200,    16,   201,    52,   201,   204,   404,   195,
     224,   200,   199,   672,   195,   625,   198,   199,   672,   157,
     672,   672,   672,   165,   152,   153,   199,   171,   172,    75,
     165,   204,   198,   166,    80,     0,   174,   175,   176,   296,
     204,   437,   438,    30,   440,    32,   165,   443,   420,   201,
     203,   184,   163,   164,     3,   427,     5,     6,   169,    74,
      67,    70,    71,    75,     8,   455,    42,   200,    75,    76,
      46,    47,    81,    82,    83,    51,    52,    84,   201,    86,
     126,   204,   128,   129,   456,    74,    29,    30,    31,   156,
     157,   158,    29,    30,    31,   102,   103,   104,    41,    42,
     202,   108,   498,   130,    41,    42,   837,   186,   187,   188,
      42,   304,   497,   327,    46,    47,   171,   172,   130,    51,
      52,   201,   149,   150,   204,   132,   133,   134,   152,   153,
     120,   449,   122,   123,   124,   147,   148,   149,   150,   758,
      20,    14,    22,   155,    24,     3,    85,   154,    87,   201,
      89,     3,   204,     3,     4,     5,     6,   414,   195,   196,
     197,   557,   772,   559,   191,   192,   193,   194,   195,   196,
     197,   178,   562,   180,   183,   182,    48,    49,   190,   191,
     192,   193,   194,   195,   196,   197,   193,   194,   195,   201,
     580,   198,   649,   200,   201,   652,     5,     6,    91,    92,
      93,    94,    95,   575,   201,    98,    99,   204,     3,     4,
       5,     6,   105,   106,   107,   201,   430,     6,   204,    35,
      36,    37,    38,    39,    40,    75,    76,    43,   121,   202,
       3,   204,   651,   490,    84,   201,    86,   201,   204,    89,
     204,     3,   456,     3,    85,   976,    87,   201,    89,   174,
     204,   647,   102,   103,   104,   178,   163,   180,   108,   182,
     632,   201,   204,   994,   204,     5,     6,   998,    58,    59,
      60,    61,    62,    63,   201,    65,    66,   204,    57,  1010,
      75,    76,   132,   133,   134,    80,     6,   201,   201,    84,
     204,    86,     3,     4,     5,     6,    45,    46,    47,    48,
      39,    40,    41,    42,   154,    48,    49,   102,   103,   104,
     201,   201,   201,   108,   204,   204,    46,    47,    21,    91,
      92,    93,    94,    95,   669,   670,    98,    99,   178,    67,
     180,    75,   182,   105,   106,   107,    69,   132,   133,   134,
       3,     3,    64,   193,   194,   195,   200,    76,   198,   121,
     200,     7,     3,     9,    10,    11,    12,    13,    14,   154,
      16,   575,     3,   201,    75,    76,   204,    23,     3,    25,
      26,    27,     3,    84,   201,    86,    68,   204,   201,   775,
       4,   204,     3,   178,     3,   180,   201,   182,   200,   204,
       6,   102,   103,   104,     4,     6,    55,   108,   193,   194,
     195,   201,   774,   198,   204,   200,     6,     3,     4,     5,
       6,    29,    30,   201,    32,   201,   204,   201,   204,     3,
     204,   132,   133,   134,    80,    70,    44,    45,    76,   190,
       3,     4,     5,     6,    91,    92,    93,    94,    95,    64,
      80,    98,    99,   154,    80,   200,   200,   200,   105,   106,
     107,   847,     4,     4,     7,     4,     9,    10,    11,    12,
      13,    14,    15,    16,   121,     4,     6,   178,   200,   180,
      23,   182,    25,    26,    27,   200,   200,     3,     6,    75,
      76,   202,   193,   194,   195,    80,     6,   198,    84,   200,
      86,   201,   201,    71,    57,   200,   200,    80,   200,   200,
     200,     4,    75,   200,   200,   200,   102,   103,   104,   200,
     166,    84,   108,    86,     4,     4,   205,   201,    79,     3,
      75,   200,    77,   198,     6,     6,     6,    80,     5,   102,
     103,   104,    46,   200,     6,   108,   132,   133,   134,   177,
     158,   159,   160,   161,   200,   163,   164,   201,   127,   204,
     168,   169,   170,   200,     3,   173,   174,   175,   154,   132,
     133,   134,   180,   181,   155,   204,    78,   185,   201,     4,
     152,    75,   200,   129,     6,   130,   205,     6,   205,     3,
       6,   154,   178,    28,   180,   200,   182,   204,   200,     6,
       6,   200,   147,   148,   149,   150,   151,   193,   194,   195,
     155,    75,   198,   200,   200,   178,   200,   180,   200,   182,
     200,   200,     4,   166,     3,     6,   201,   204,     4,     4,
     193,   194,   195,    80,   201,   198,   130,   200,     6,     5,
      53,   184,    50,   190,     6,   190,   191,   192,   193,   194,
     195,   196,   197,   147,   148,   149,   150,   200,   152,    59,
      59,   155,     3,    54,   204,   131,   130,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,     6,
     129,   127,   201,   147,   148,   149,   150,   130,   200,   162,
     198,   155,     4,   198,   201,   201,   190,   191,   192,   193,
     194,   195,   196,   197,   147,   148,   149,   150,   200,     6,
     201,     6,   155,   200,    56,    55,   204,     3,   204,   204,
     204,   204,   204,   204,    93,     6,   190,   191,   192,   193,
     194,   195,   196,   197,    93,   204,    93,    93,    64,   204,
       6,   204,     6,   204,   204,   204,   204,   190,   191,   192,
     193,   194,   195,   196,   197,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   204,   108,   109,   110,   111,   112,   113,    93,
     204,   116,     6,   204,   119,   120,   204,   204,   123,   124,
     125,    91,    92,    93,    94,    95,     6,     6,    98,    99,
     204,   204,   204,   204,     6,   105,   106,   107,    91,    92,
      93,    94,    95,     6,   204,    98,    99,   204,   204,   204,
     204,   121,   105,   106,   107,     6,   204,   204,     6,     6,
     204,   204,   204,   204,   204,     6,     6,   204,   121,   204,
     204,   204,     6,   204,   204,     6,   204,     6,     6,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,     6,     6,     6,     6,   204,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   204,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   204,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   204,     6,
       4,     4,     4,     4,    51,   204,     6,   201,   201,   201,
     201,   201,   201,   201,   201,     6,   201,   201,     6,   201,
       6,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,     6,   201,     6,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,   201,   201,     4,   201,     4,   204,   201,   204,   201,
     201,   201,   201,     6,     6,     6,   204,     6,   201,     6,
       4,   201,   201,     6,     6,    63,   536,    63,   622,    63,
     556,    99,   624,   181,   170,    63,   306,    63,    63,    17,
     498,   590,    63,   667,   160,    63,    63,    63,    63,   672,
     330,   442,   733,   356,   215,   639,   652,   665,   416,   652,
      -1,   739,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   381,    -1,    -1,   383,    -1,    -1,   284
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   166,   184,   200,   207,   208,
     209,   211,   221,   222,   224,   226,   229,   230,   231,   232,
     233,   255,   260,   261,   262,   263,   264,   265,    29,    30,
      31,    41,    42,    33,    29,    30,    31,    41,    42,     3,
     253,    78,   253,   163,   164,   169,    35,    36,    37,    38,
      39,    40,    43,   225,    29,    30,    32,    44,    45,   158,
     159,   160,   161,   163,   164,   168,   169,   170,   173,   174,
     175,   180,   181,   185,    30,   156,   157,   158,     3,     3,
     253,     3,   256,   257,   167,    16,   232,   233,     0,   203,
     313,    20,    22,    24,   249,     8,   234,   236,    74,   312,
     312,   312,   312,   312,   314,   253,    74,   311,   311,   311,
     311,   311,   202,    14,   253,    78,    79,   200,     3,     3,
       3,   210,   211,   221,   222,   226,   229,   230,   231,   260,
     261,   262,   263,   264,     3,   253,     6,   171,   172,   171,
     172,     3,   174,   163,   186,   187,   188,   253,    64,    57,
     204,     6,   157,   174,   175,   176,   201,   201,   209,    21,
     200,   235,   236,    67,   243,    69,   237,    75,     3,   253,
     253,   253,     3,    64,   200,   223,    76,     3,   253,   253,
     253,     3,     3,     3,   227,   228,    68,   246,     4,   310,
     310,     3,     4,     5,     6,    75,    76,    84,    86,   102,
     103,   104,   108,   132,   133,   134,   154,   178,   180,   182,
     193,   194,   195,   198,   200,   266,   268,   269,   270,   272,
     273,   274,   275,   276,   277,   280,   281,   282,   283,   284,
     286,   287,   288,   289,   290,   292,   293,   294,   295,   296,
     297,   298,   299,   302,   303,   304,   305,   306,   307,     3,
       5,     6,    64,   165,     3,     5,     6,    64,   165,     3,
       5,     6,    64,   165,    42,    46,    47,    51,    52,     3,
       3,     6,     4,   253,   200,   257,   310,     6,     6,   235,
     236,   266,    55,    70,   241,    76,    57,   200,   223,   253,
       3,   220,    34,   233,    64,   190,   204,   246,   269,    80,
      80,   200,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,    75,    76,   270,   200,   200,    89,
     269,   285,     4,     4,     4,     4,     6,   307,   200,   120,
     122,   123,   124,   200,   200,   270,   270,     5,     6,   199,
     290,   300,   301,   233,   269,   201,   204,    57,   152,   153,
      75,    77,   130,   147,   148,   149,   150,   151,   155,   190,
     191,   192,   193,   194,   195,   196,   197,   202,   199,   204,
     199,   204,   199,   204,   199,   204,   199,   204,     3,     6,
      80,   315,   234,    42,    52,     6,   201,   201,    78,   244,
     238,   239,   269,   269,    71,   242,   231,     3,   126,   128,
     212,   213,   214,   219,    57,   200,   319,   320,   201,   204,
     200,   267,   253,   269,   228,   200,   200,    67,   201,   266,
     200,    75,   233,   269,   269,   285,    85,    87,    89,     4,
     200,   200,   200,   200,     4,     4,   205,   201,   201,    79,
     268,     3,   269,   269,    77,   155,   200,    75,   129,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,     3,   195,   290,     6,   300,     6,   301,
       6,     5,    46,    48,    49,   200,   201,     6,    29,    45,
     200,   250,   251,   252,   253,   258,   177,   245,   204,    72,
      73,   240,   269,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     108,   109,   110,   111,   112,   113,   116,   119,   120,   123,
     124,   125,   215,   127,   200,   201,   204,   231,   220,   320,
       3,   266,   204,    70,    71,    81,    82,    83,   183,   308,
     309,   308,   266,   201,   233,   201,    57,    88,    85,    87,
     269,   269,    78,   269,     4,     3,   288,   269,   201,   204,
     201,   204,     5,     6,   310,   200,   270,   233,   266,   129,
     152,   205,   205,     6,     6,     3,   317,   318,     6,   231,
     204,    58,    60,    61,    62,    63,    65,    66,   259,     3,
      57,   254,   272,   273,   274,   275,   276,   277,   278,   279,
     246,   239,   200,   200,   200,   200,   200,   200,    75,   126,
     128,   129,   216,   217,   315,   200,   220,    28,   316,   213,
     201,   201,   200,     6,     6,     4,     3,     6,   201,   204,
     201,   201,   201,   215,   269,   269,    85,    88,   270,   204,
     204,   204,   204,     4,     4,    80,   233,   266,   201,   201,
     270,    53,    50,   190,   201,   204,     6,   201,   251,    59,
      59,     3,   204,    54,   248,     6,    91,    92,    93,    94,
      95,    98,    99,   105,   106,   107,   121,    91,    92,    93,
      94,    95,    98,    99,   105,   106,   107,   121,    91,    92,
      93,    94,    95,    98,    99,   105,   106,   107,   121,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   121,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     121,   129,   127,   131,   217,   218,   218,   220,   201,   200,
     162,   266,   309,   201,    85,   269,   201,   198,   302,     4,
     290,   198,   291,   294,   299,   302,   201,   201,   200,   201,
     201,     6,     6,     3,     5,     6,   318,    30,    32,   254,
     252,   252,   200,   278,    55,    56,   247,   201,   204,   204,
     204,   204,   204,   204,   204,   204,   204,   204,    93,   204,
     204,   204,   204,   204,   204,   204,   204,   204,   204,    93,
     204,   204,   204,   204,   204,   204,   204,   204,   204,   204,
      93,   204,   204,   204,   204,   204,   204,   204,   204,   204,
     204,    93,   204,   204,   204,   204,   204,   204,   204,   204,
     204,   204,    93,   204,   289,   201,   317,     3,   201,     6,
     204,   201,   204,   204,   204,   308,     6,    64,   220,   266,
     269,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   204,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   204,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   204,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   204,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   204,     6,   201,   315,     4,
       4,     4,     4,   201,     6,   269,   201,   201,   201,   201,
     201,   201,   201,   201,   201,   201,   201,     6,   201,   201,
     201,   201,   201,   201,   201,   201,   201,   201,     6,   201,
     201,   201,   201,   201,   201,   201,   201,   201,   201,     6,
     201,   201,   201,   201,   201,   201,   201,   201,   201,   201,
       6,   201,   201,   201,   201,   201,   201,   201,   201,   201,
     201,     6,   201,   204,   201,   204,   201,   204,    42,    46,
      47,    51,    52,   201,   201,   201,   201,   201,     4,     4,
     315,     6,     6,     6,   201,   204,   204,   271,   201,     6,
      48,    49,   315,     6,     4,   201,   315,    46,    47,     6,
     201,     6,    51,   315
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   206,   207,   208,   208,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   211,   211,   211,   211,   211,   211,   212,   212,   213,
     213,   214,   214,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   216,   216,   217,   217,   217,   217,
     218,   218,   219,   219,   220,   220,   221,   222,   222,   223,
     223,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     226,   227,   227,   228,   229,   229,   229,   229,   229,   230,
     230,   230,   231,   231,   231,   231,   232,   232,   233,   234,
     235,   235,   236,   237,   237,   238,   238,   239,   240,   240,
     240,   241,   241,   242,   242,   243,   243,   244,   244,   245,
     245,   246,   246,   247,   247,   248,   248,   249,   249,   249,
     249,   250,   250,   251,   251,   252,   252,   253,   253,   254,
     254,   254,   254,   255,   255,   256,   256,   257,   258,   258,
     259,   259,   259,   259,   259,   259,   259,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   260,   260,   260,   260,   260,   260,   260,   260,   260,
     260,   261,   261,   261,   262,   262,   263,   263,   263,   263,
     263,   263,   263,   263,   263,   263,   263,   263,   263,   263,
     263,   263,   263,   264,   265,   265,   265,   265,   265,   265,
     265,   265,   265,   265,   265,   265,   265,   265,   265,   265,
     265,   265,   265,   265,   266,   266,   267,   267,   268,   268,
     269,   269,   269,   269,   269,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   271,   271,
     272,   273,   273,   274,   274,   275,   275,   276,   276,   277,
     277,   278,   278,   278,   278,   278,   278,   279,   279,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   281,   281,   282,   283,   283,   284,   284,   284,
     284,   285,   285,   286,   287,   287,   287,   287,   288,   288,
     288,   288,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   290,   290,   290,   290,   291,   291,
     291,   292,   293,   293,   294,   294,   295,   296,   296,   297,
     298,   298,   299,   300,   301,   302,   302,   303,   304,   304,
     305,   306,   306,   307,   307,   307,   307,   307,   307,   307,
     307,   307,   307,   307,   307,   308,   308,   309,   309,   309,
     309,   309,   309,   310,   311,   311,   312,   312,   313,   313,
     314,   314,   315,   315,   316,   316,   317,   317,   318,   318,
     318,   318,   319,   319,   320,   320
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
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     1,     2,     2,     1,     1,     2,
       2,     0,     5,     4,     1,     3,     4,     6,     5,     3,
       0,     3,     1,     1,     1,     1,     1,     1,     1,     0,
       5,     1,     3,     3,     4,     4,     4,     4,     6,     8,
      11,     8,     1,     1,     3,     3,     3,     3,     2,     4,
       3,     3,     8,     3,     0,     1,     3,     2,     1,     1,
       0,     2,     0,     2,     0,     1,     0,     2,     0,     2,
       0,     2,     0,     2,     0,     3,     0,     1,     2,     1,
       1,     1,     3,     1,     1,     2,     4,     1,     3,     2,
       1,     5,     0,     2,     0,     1,     3,     5,     4,     6,
       1,     1,     1,     1,     1,     1,     0,     2,     2,     2,
       2,     3,     2,     2,     2,     2,     4,     2,     3,     3,
       3,     4,     4,     3,     3,     4,     4,     5,     6,     7,
       9,     4,     5,     7,     9,     2,     3,     2,     3,     3,
       4,     2,     2,     2,     2,     5,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     3,     3,     4,     6,     7,     9,    10,
      12,    12,    13,    14,    15,    16,    12,    13,    15,    16,
       3,     4,     5,     6,     1,     3,     3,     5,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     0,
      12,    14,    12,    12,    10,     7,     9,     4,     6,     4,
       6,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       4,     5,     4,     3,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     6,
       3,     4,     3,     3,     5,     5,     6,     4,     6,     3,
       5,     4,     5,     6,     4,     5,     5,     6,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     2,     3,     1,     1,     2,     2,     3,     2,
       2,     3,     2,     3,     3,     1,     1,     2,     2,     3,
       2,     2,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     1,     3,     2,     2,     1,
       2,     2,     2,     1,     2,     0,     3,     0,     1,     0,
       2,     0,     4,     0,     4,     0,     1,     3,     1,     3,
       3,     3,     1,     2,     6,     3
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
#line 2287 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2295 "parser.cpp"
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
#line 2309 "parser.cpp"
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
#line 2323 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2334 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2342 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2351 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2360 "parser.cpp"
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
#line 2374 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 269 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2385 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2395 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2405 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2415 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2425 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2435 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2445 "parser.cpp"
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
#line 2459 "parser.cpp"
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
#line 2473 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2508 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2524 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2532 "parser.cpp"
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
#line 2546 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2564 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2573 "parser.cpp"
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
#line 2586 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2595 "parser.cpp"
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
#line 2609 "parser.cpp"
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
#line 2623 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2642 "parser.cpp"
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
#line 2656 "parser.cpp"
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
#line 2673 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2681 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2689 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2697 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2705 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2713 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2721 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2729 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2737 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2745 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2753 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2761 "parser.cpp"
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
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2791 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2807 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2815 "parser.cpp"
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
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2892 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2900 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2908 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2916 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2924 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2932 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 366 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 370 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2996 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3004 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3012 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 209 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3023 "parser.cpp"
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
#line 3037 "parser.cpp"
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
#line 3051 "parser.cpp"
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
#line 3065 "parser.cpp"
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
#line 3079 "parser.cpp"
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

#line 3187 "parser.cpp"

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
#line 497 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3402 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 501 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3413 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 507 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3424 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 514 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3430 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3436 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 516 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3442 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 517 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3448 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3454 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3460 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 520 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3466 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 521 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3472 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 522 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3478 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 523 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3484 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 524 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3490 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 525 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3496 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 526 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3502 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 527 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3508 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 529 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3514 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3520 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3526 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 532 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3532 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3538 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3544 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3550 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 536 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3556 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3562 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 538 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3568 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3574 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 540 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3580 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 547 "parser.y"
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
#line 3600 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 564 "parser.y"
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
#line 3618 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 580 "parser.y"
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
#line 3651 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 609 "parser.y"
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
#line 3671 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 625 "parser.y"
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
#line 3692 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 643 "parser.y"
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
#line 3725 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 672 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3734 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 676 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3743 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 682 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3751 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 685 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3759 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 692 "parser.y"
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
#line 3814 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 742 "parser.y"
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
#line 3853 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 778 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3859 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 779 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3865 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 780 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3871 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 781 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3877 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 782 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3883 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 783 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3889 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 784 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3895 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 785 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3901 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 786 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3907 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 787 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3913 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 788 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3919 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 789 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3925 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 790 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3931 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3937 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 792 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3943 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 793 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3949 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 794 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3955 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 795 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3961 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 796 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3967 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3973 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 798 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3979 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 801 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3985 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 803 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3991 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 804 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3997 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 805 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4003 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 806 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4009 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4015 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4021 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4027 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4033 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4039 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4045 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4051 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4057 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4063 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4069 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4075 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4081 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4087 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4093 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4099 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4105 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4111 "parser.cpp"
    break;

  case 86: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4117 "parser.cpp"
    break;

  case 87: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4123 "parser.cpp"
    break;

  case 88: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4129 "parser.cpp"
    break;

  case 89: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4135 "parser.cpp"
    break;

  case 90: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4141 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4147 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4153 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4159 "parser.cpp"
    break;

  case 94: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4165 "parser.cpp"
    break;

  case 95: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4171 "parser.cpp"
    break;

  case 96: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4177 "parser.cpp"
    break;

  case 97: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4183 "parser.cpp"
    break;

  case 98: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4189 "parser.cpp"
    break;

  case 99: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4195 "parser.cpp"
    break;

  case 100: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4201 "parser.cpp"
    break;

  case 101: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4207 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4213 "parser.cpp"
    break;

  case 103: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4219 "parser.cpp"
    break;

  case 104: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4225 "parser.cpp"
    break;

  case 105: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4231 "parser.cpp"
    break;

  case 106: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4237 "parser.cpp"
    break;

  case 107: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4243 "parser.cpp"
    break;

  case 108: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4249 "parser.cpp"
    break;

  case 109: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4255 "parser.cpp"
    break;

  case 110: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4261 "parser.cpp"
    break;

  case 111: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4267 "parser.cpp"
    break;

  case 112: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4273 "parser.cpp"
    break;

  case 113: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4279 "parser.cpp"
    break;

  case 114: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4285 "parser.cpp"
    break;

  case 115: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4291 "parser.cpp"
    break;

  case 116: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4297 "parser.cpp"
    break;

  case 117: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4303 "parser.cpp"
    break;

  case 118: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4309 "parser.cpp"
    break;

  case 119: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4315 "parser.cpp"
    break;

  case 120: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4321 "parser.cpp"
    break;

  case 121: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4327 "parser.cpp"
    break;

  case 122: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4333 "parser.cpp"
    break;

  case 123: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4339 "parser.cpp"
    break;

  case 124: /* column_constraints: column_constraint  */
#line 882 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4348 "parser.cpp"
    break;

  case 125: /* column_constraints: column_constraints column_constraint  */
#line 886 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4362 "parser.cpp"
    break;

  case 126: /* column_constraint: PRIMARY KEY  */
#line 896 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4370 "parser.cpp"
    break;

  case 127: /* column_constraint: UNIQUE  */
#line 899 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4378 "parser.cpp"
    break;

  case 128: /* column_constraint: NULLABLE  */
#line 902 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4386 "parser.cpp"
    break;

  case 129: /* column_constraint: NOT NULLABLE  */
#line 905 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4394 "parser.cpp"
    break;

  case 130: /* default_expr: DEFAULT constant_expr  */
#line 909 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4402 "parser.cpp"
    break;

  case 131: /* default_expr: %empty  */
#line 912 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4410 "parser.cpp"
    break;

  case 132: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 917 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4420 "parser.cpp"
    break;

  case 133: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 922 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4430 "parser.cpp"
    break;

  case 134: /* identifier_array: IDENTIFIER  */
#line 929 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4441 "parser.cpp"
    break;

  case 135: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 935 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4452 "parser.cpp"
    break;

  case 136: /* delete_statement: DELETE FROM table_name where_clause  */
#line 945 "parser.y"
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
#line 4469 "parser.cpp"
    break;

  case 137: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 961 "parser.y"
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
#line 4508 "parser.cpp"
    break;

  case 138: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 995 "parser.y"
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
#line 4525 "parser.cpp"
    break;

  case 139: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1008 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4533 "parser.cpp"
    break;

  case 140: /* optional_identifier_array: %empty  */
#line 1011 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4541 "parser.cpp"
    break;

  case 141: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1018 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4551 "parser.cpp"
    break;

  case 142: /* explain_type: ANALYZE  */
#line 1024 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4559 "parser.cpp"
    break;

  case 143: /* explain_type: AST  */
#line 1027 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4567 "parser.cpp"
    break;

  case 144: /* explain_type: RAW  */
#line 1030 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4575 "parser.cpp"
    break;

  case 145: /* explain_type: LOGICAL  */
#line 1033 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4583 "parser.cpp"
    break;

  case 146: /* explain_type: PHYSICAL  */
#line 1036 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4591 "parser.cpp"
    break;

  case 147: /* explain_type: PIPELINE  */
#line 1039 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4599 "parser.cpp"
    break;

  case 148: /* explain_type: FRAGMENT  */
#line 1042 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4607 "parser.cpp"
    break;

  case 149: /* explain_type: %empty  */
#line 1045 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4615 "parser.cpp"
    break;

  case 150: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1052 "parser.y"
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
#line 4632 "parser.cpp"
    break;

  case 151: /* update_expr_array: update_expr  */
#line 1065 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4641 "parser.cpp"
    break;

  case 152: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1069 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4650 "parser.cpp"
    break;

  case 153: /* update_expr: IDENTIFIER '=' expr  */
#line 1074 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4662 "parser.cpp"
    break;

  case 154: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1087 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4678 "parser.cpp"
    break;

  case 155: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1100 "parser.y"
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
#line 4696 "parser.cpp"
    break;

  case 156: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1115 "parser.y"
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
#line 4714 "parser.cpp"
    break;

  case 157: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1130 "parser.y"
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
#line 4732 "parser.cpp"
    break;

  case 158: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1145 "parser.y"
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
#line 4755 "parser.cpp"
    break;

  case 159: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1168 "parser.y"
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
#line 4813 "parser.cpp"
    break;

  case 160: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1221 "parser.y"
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
#line 4873 "parser.cpp"
    break;

  case 161: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1276 "parser.y"
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
#line 4925 "parser.cpp"
    break;

  case 162: /* select_statement: select_without_paren  */
#line 1327 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4933 "parser.cpp"
    break;

  case 163: /* select_statement: select_with_paren  */
#line 1330 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4941 "parser.cpp"
    break;

  case 164: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1333 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4955 "parser.cpp"
    break;

  case 165: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1342 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4969 "parser.cpp"
    break;

  case 166: /* select_with_paren: '(' select_without_paren ')'  */
#line 1352 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4977 "parser.cpp"
    break;

  case 167: /* select_with_paren: '(' select_with_paren ')'  */
#line 1355 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4985 "parser.cpp"
    break;

  case 168: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1359 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4994 "parser.cpp"
    break;

  case 169: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1364 "parser.y"
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
#line 5020 "parser.cpp"
    break;

  case 170: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1386 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5028 "parser.cpp"
    break;

  case 171: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1389 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5036 "parser.cpp"
    break;

  case 172: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1394 "parser.y"
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
#line 5056 "parser.cpp"
    break;

  case 173: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1410 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5064 "parser.cpp"
    break;

  case 174: /* order_by_clause: %empty  */
#line 1413 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5072 "parser.cpp"
    break;

  case 175: /* order_by_expr_list: order_by_expr  */
#line 1417 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5081 "parser.cpp"
    break;

  case 176: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1421 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5090 "parser.cpp"
    break;

  case 177: /* order_by_expr: expr order_by_type  */
#line 1426 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5100 "parser.cpp"
    break;

  case 178: /* order_by_type: ASC  */
#line 1432 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5108 "parser.cpp"
    break;

  case 179: /* order_by_type: DESC  */
#line 1435 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5116 "parser.cpp"
    break;

  case 180: /* order_by_type: %empty  */
#line 1438 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5124 "parser.cpp"
    break;

  case 181: /* limit_expr: LIMIT expr  */
#line 1442 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5132 "parser.cpp"
    break;

  case 182: /* limit_expr: %empty  */
#line 1446 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5138 "parser.cpp"
    break;

  case 183: /* offset_expr: OFFSET expr  */
#line 1448 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5146 "parser.cpp"
    break;

  case 184: /* offset_expr: %empty  */
#line 1452 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5152 "parser.cpp"
    break;

  case 185: /* distinct: DISTINCT  */
#line 1454 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5160 "parser.cpp"
    break;

  case 186: /* distinct: %empty  */
#line 1457 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5168 "parser.cpp"
    break;

  case 187: /* from_clause: FROM table_reference  */
#line 1461 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5176 "parser.cpp"
    break;

  case 188: /* from_clause: %empty  */
#line 1464 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5184 "parser.cpp"
    break;

  case 189: /* search_clause: SEARCH sub_search_array  */
#line 1468 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5194 "parser.cpp"
    break;

  case 190: /* search_clause: %empty  */
#line 1473 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5202 "parser.cpp"
    break;

  case 191: /* where_clause: WHERE expr  */
#line 1477 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5210 "parser.cpp"
    break;

  case 192: /* where_clause: %empty  */
#line 1480 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5218 "parser.cpp"
    break;

  case 193: /* having_clause: HAVING expr  */
#line 1484 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5226 "parser.cpp"
    break;

  case 194: /* having_clause: %empty  */
#line 1487 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5234 "parser.cpp"
    break;

  case 195: /* group_by_clause: GROUP BY expr_array  */
#line 1491 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5242 "parser.cpp"
    break;

  case 196: /* group_by_clause: %empty  */
#line 1494 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5250 "parser.cpp"
    break;

  case 197: /* set_operator: UNION  */
#line 1498 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5258 "parser.cpp"
    break;

  case 198: /* set_operator: UNION ALL  */
#line 1501 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5266 "parser.cpp"
    break;

  case 199: /* set_operator: INTERSECT  */
#line 1504 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5274 "parser.cpp"
    break;

  case 200: /* set_operator: EXCEPT  */
#line 1507 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5282 "parser.cpp"
    break;

  case 201: /* table_reference: table_reference_unit  */
#line 1515 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5290 "parser.cpp"
    break;

  case 202: /* table_reference: table_reference ',' table_reference_unit  */
#line 1518 "parser.y"
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
#line 5308 "parser.cpp"
    break;

  case 205: /* table_reference_name: table_name table_alias  */
#line 1535 "parser.y"
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
#line 5326 "parser.cpp"
    break;

  case 206: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1549 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5337 "parser.cpp"
    break;

  case 207: /* table_name: IDENTIFIER  */
#line 1558 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5347 "parser.cpp"
    break;

  case 208: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1563 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5359 "parser.cpp"
    break;

  case 209: /* table_alias: AS IDENTIFIER  */
#line 1572 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5369 "parser.cpp"
    break;

  case 210: /* table_alias: IDENTIFIER  */
#line 1577 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5379 "parser.cpp"
    break;

  case 211: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1582 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5390 "parser.cpp"
    break;

  case 212: /* table_alias: %empty  */
#line 1588 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5398 "parser.cpp"
    break;

  case 213: /* with_clause: WITH with_expr_list  */
#line 1595 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5406 "parser.cpp"
    break;

  case 214: /* with_clause: %empty  */
#line 1598 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5414 "parser.cpp"
    break;

  case 215: /* with_expr_list: with_expr  */
#line 1602 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5423 "parser.cpp"
    break;

  case 216: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1605 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5432 "parser.cpp"
    break;

  case 217: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1610 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5444 "parser.cpp"
    break;

  case 218: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1622 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5456 "parser.cpp"
    break;

  case 219: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1629 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5469 "parser.cpp"
    break;

  case 220: /* join_type: INNER  */
#line 1643 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5477 "parser.cpp"
    break;

  case 221: /* join_type: LEFT  */
#line 1646 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5485 "parser.cpp"
    break;

  case 222: /* join_type: RIGHT  */
#line 1649 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5493 "parser.cpp"
    break;

  case 223: /* join_type: OUTER  */
#line 1652 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5501 "parser.cpp"
    break;

  case 224: /* join_type: FULL  */
#line 1655 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5509 "parser.cpp"
    break;

  case 225: /* join_type: CROSS  */
#line 1658 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5517 "parser.cpp"
    break;

  case 226: /* join_type: %empty  */
#line 1661 "parser.y"
                {
}
#line 5524 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW DATABASES  */
#line 1667 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5533 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW TABLES  */
#line 1671 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5542 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW VIEWS  */
#line 1675 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5551 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW CONFIGS  */
#line 1679 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5560 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1683 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5572 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW PROFILES  */
#line 1690 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5581 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW BUFFER  */
#line 1694 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5590 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW MEMINDEX  */
#line 1698 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5599 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW QUERIES  */
#line 1702 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5608 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1706 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5618 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW TRANSACTIONS  */
#line 1711 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5627 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1715 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5637 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW SESSION VARIABLES  */
#line 1720 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5646 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1724 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5655 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1728 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5666 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1734 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5677 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1740 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5688 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLE table_name  */
#line 1746 "parser.y"
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
#line 5704 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1757 "parser.y"
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
#line 5720 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1768 "parser.y"
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
#line 5736 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1779 "parser.y"
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
#line 5753 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1791 "parser.y"
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
#line 5770 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1803 "parser.y"
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
#line 5788 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1816 "parser.y"
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
#line 5807 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1830 "parser.y"
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
#line 5823 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1841 "parser.y"
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
#line 5842 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1855 "parser.y"
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
#line 5863 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1871 "parser.y"
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
#line 5885 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW LOGS  */
#line 1888 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5894 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW DELTA LOGS  */
#line 1892 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5903 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW CATALOGS  */
#line 1896 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5912 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1900 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 5921 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1904 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 5930 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1908 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 5941 "parser.cpp"
    break;

  case 261: /* flush_statement: FLUSH DATA  */
#line 1918 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5950 "parser.cpp"
    break;

  case 262: /* flush_statement: FLUSH LOG  */
#line 1922 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5959 "parser.cpp"
    break;

  case 263: /* flush_statement: FLUSH BUFFER  */
#line 1926 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5968 "parser.cpp"
    break;

  case 264: /* optimize_statement: OPTIMIZE table_name  */
#line 1934 "parser.y"
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
#line 5983 "parser.cpp"
    break;

  case 265: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1945 "parser.y"
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
#line 6007 "parser.cpp"
    break;

  case 266: /* command_statement: USE IDENTIFIER  */
#line 1968 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6018 "parser.cpp"
    break;

  case 267: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1974 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6028 "parser.cpp"
    break;

  case 268: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1979 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6039 "parser.cpp"
    break;

  case 269: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1985 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6050 "parser.cpp"
    break;

  case 270: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1991 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6063 "parser.cpp"
    break;

  case 271: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1999 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6074 "parser.cpp"
    break;

  case 272: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2005 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6085 "parser.cpp"
    break;

  case 273: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2011 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6096 "parser.cpp"
    break;

  case 274: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2017 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6107 "parser.cpp"
    break;

  case 275: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2023 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6120 "parser.cpp"
    break;

  case 276: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2031 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6131 "parser.cpp"
    break;

  case 277: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2037 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6142 "parser.cpp"
    break;

  case 278: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2043 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6153 "parser.cpp"
    break;

  case 279: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2049 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6164 "parser.cpp"
    break;

  case 280: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2055 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6177 "parser.cpp"
    break;

  case 281: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2063 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6188 "parser.cpp"
    break;

  case 282: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2069 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 283: /* compact_statement: COMPACT TABLE table_name  */
#line 2076 "parser.y"
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
#line 6216 "parser.cpp"
    break;

  case 284: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2089 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6225 "parser.cpp"
    break;

  case 285: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2093 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6235 "parser.cpp"
    break;

  case 286: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2098 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6246 "parser.cpp"
    break;

  case 287: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2104 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6258 "parser.cpp"
    break;

  case 288: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2111 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6271 "parser.cpp"
    break;

  case 289: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2119 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6285 "parser.cpp"
    break;

  case 290: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2128 "parser.y"
                                                                                                              {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowColumn;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-1].long_value);
}
#line 6300 "parser.cpp"
    break;

  case 291: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2138 "parser.y"
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
#line 6315 "parser.cpp"
    break;

  case 292: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2148 "parser.y"
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
#line 6331 "parser.cpp"
    break;

  case 293: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2159 "parser.y"
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
#line 6347 "parser.cpp"
    break;

  case 294: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2170 "parser.y"
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
#line 6364 "parser.cpp"
    break;

  case 295: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2182 "parser.y"
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
#line 6381 "parser.cpp"
    break;

  case 296: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2194 "parser.y"
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
#line 6396 "parser.cpp"
    break;

  case 297: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2204 "parser.y"
                                                                                                                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndex;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->index_meta_index_ = (yyvsp[0].long_value);
}
#line 6412 "parser.cpp"
    break;

  case 298: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2215 "parser.y"
                                                                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListIndexSegments;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-11].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-10].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-7].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->index_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->index_entry_index_ = (yyvsp[-1].long_value);
}
#line 6429 "parser.cpp"
    break;

  case 299: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2227 "parser.y"
                                                                                                                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowIndexSegment;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-12].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-11].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-9].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-8].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->table_entry_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->index_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->index_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->segment_index_ = (yyvsp[0].long_value);
}
#line 6447 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW LOGS  */
#line 2240 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6456 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2244 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6466 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2249 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6476 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2254 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6487 "parser.cpp"
    break;

  case 304: /* expr_array: expr_alias  */
#line 2265 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6496 "parser.cpp"
    break;

  case 305: /* expr_array: expr_array ',' expr_alias  */
#line 2269 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6505 "parser.cpp"
    break;

  case 306: /* expr_array_list: '(' expr_array ')'  */
#line 2274 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6514 "parser.cpp"
    break;

  case 307: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2278 "parser.y"
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
#line 6534 "parser.cpp"
    break;

  case 308: /* expr_alias: expr AS IDENTIFIER  */
#line 2305 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6545 "parser.cpp"
    break;

  case 309: /* expr_alias: expr  */
#line 2311 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6553 "parser.cpp"
    break;

  case 315: /* operand: '(' expr ')'  */
#line 2321 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6561 "parser.cpp"
    break;

  case 316: /* operand: '(' select_without_paren ')'  */
#line 2324 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6572 "parser.cpp"
    break;

  case 317: /* operand: constant_expr  */
#line 2330 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6580 "parser.cpp"
    break;

  case 328: /* extra_match_tensor_option: ',' STRING  */
#line 2344 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6588 "parser.cpp"
    break;

  case 329: /* extra_match_tensor_option: %empty  */
#line 2347 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6596 "parser.cpp"
    break;

  case 330: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2353 "parser.y"
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
#line 6617 "parser.cpp"
    break;

  case 331: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2372 "parser.y"
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
#line 6663 "parser.cpp"
    break;

  case 332: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2414 "parser.y"
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
#line 6710 "parser.cpp"
    break;

  case 333: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2460 "parser.y"
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
#line 6732 "parser.cpp"
    break;

  case 334: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2478 "parser.y"
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
#line 6754 "parser.cpp"
    break;

  case 335: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2496 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6767 "parser.cpp"
    break;

  case 336: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2504 "parser.y"
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
#line 6782 "parser.cpp"
    break;

  case 337: /* query_expr: QUERY '(' STRING ')'  */
#line 2515 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6793 "parser.cpp"
    break;

  case 338: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2521 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6806 "parser.cpp"
    break;

  case 339: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2530 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6817 "parser.cpp"
    break;

  case 340: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2536 "parser.y"
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
#line 6833 "parser.cpp"
    break;

  case 341: /* sub_search: match_vector_expr  */
#line 2548 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6841 "parser.cpp"
    break;

  case 342: /* sub_search: match_text_expr  */
#line 2551 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6849 "parser.cpp"
    break;

  case 343: /* sub_search: match_tensor_expr  */
#line 2554 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6857 "parser.cpp"
    break;

  case 344: /* sub_search: match_sparse_expr  */
#line 2557 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6865 "parser.cpp"
    break;

  case 345: /* sub_search: query_expr  */
#line 2560 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6873 "parser.cpp"
    break;

  case 346: /* sub_search: fusion_expr  */
#line 2563 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6881 "parser.cpp"
    break;

  case 347: /* sub_search_array: sub_search  */
#line 2567 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6890 "parser.cpp"
    break;

  case 348: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2571 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6899 "parser.cpp"
    break;

  case 349: /* function_expr: IDENTIFIER '(' ')'  */
#line 2576 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6912 "parser.cpp"
    break;

  case 350: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2584 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6925 "parser.cpp"
    break;

  case 351: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2592 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6939 "parser.cpp"
    break;

  case 352: /* function_expr: operand IS NOT NULLABLE  */
#line 2601 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6951 "parser.cpp"
    break;

  case 353: /* function_expr: operand IS NULLABLE  */
#line 2608 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6963 "parser.cpp"
    break;

  case 354: /* function_expr: NOT operand  */
#line 2615 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6975 "parser.cpp"
    break;

  case 355: /* function_expr: '-' operand  */
#line 2622 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6987 "parser.cpp"
    break;

  case 356: /* function_expr: '+' operand  */
#line 2629 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6999 "parser.cpp"
    break;

  case 357: /* function_expr: operand '-' operand  */
#line 2636 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7012 "parser.cpp"
    break;

  case 358: /* function_expr: operand '+' operand  */
#line 2644 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7025 "parser.cpp"
    break;

  case 359: /* function_expr: operand '*' operand  */
#line 2652 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7038 "parser.cpp"
    break;

  case 360: /* function_expr: operand '/' operand  */
#line 2660 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7051 "parser.cpp"
    break;

  case 361: /* function_expr: operand '%' operand  */
#line 2668 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7064 "parser.cpp"
    break;

  case 362: /* function_expr: operand '=' operand  */
#line 2676 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7077 "parser.cpp"
    break;

  case 363: /* function_expr: operand EQUAL operand  */
#line 2684 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7090 "parser.cpp"
    break;

  case 364: /* function_expr: operand NOT_EQ operand  */
#line 2692 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7103 "parser.cpp"
    break;

  case 365: /* function_expr: operand '<' operand  */
#line 2700 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7116 "parser.cpp"
    break;

  case 366: /* function_expr: operand '>' operand  */
#line 2708 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7129 "parser.cpp"
    break;

  case 367: /* function_expr: operand LESS_EQ operand  */
#line 2716 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7142 "parser.cpp"
    break;

  case 368: /* function_expr: operand GREATER_EQ operand  */
#line 2724 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7155 "parser.cpp"
    break;

  case 369: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2732 "parser.y"
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
#line 7190 "parser.cpp"
    break;

  case 370: /* function_expr: operand LIKE operand  */
#line 2762 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7203 "parser.cpp"
    break;

  case 371: /* function_expr: operand NOT LIKE operand  */
#line 2770 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7216 "parser.cpp"
    break;

  case 372: /* conjunction_expr: expr AND expr  */
#line 2779 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7229 "parser.cpp"
    break;

  case 373: /* conjunction_expr: expr OR expr  */
#line 2787 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7242 "parser.cpp"
    break;

  case 374: /* between_expr: operand BETWEEN operand AND operand  */
#line 2796 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7254 "parser.cpp"
    break;

  case 375: /* in_expr: operand IN '(' expr_array ')'  */
#line 2804 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7265 "parser.cpp"
    break;

  case 376: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2810 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7276 "parser.cpp"
    break;

  case 377: /* case_expr: CASE expr case_check_array END  */
#line 2817 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7287 "parser.cpp"
    break;

  case 378: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2823 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7299 "parser.cpp"
    break;

  case 379: /* case_expr: CASE case_check_array END  */
#line 2830 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7309 "parser.cpp"
    break;

  case 380: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2835 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7320 "parser.cpp"
    break;

  case 381: /* case_check_array: WHEN expr THEN expr  */
#line 2842 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7332 "parser.cpp"
    break;

  case 382: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2849 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7344 "parser.cpp"
    break;

  case 383: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2857 "parser.y"
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
#line 7372 "parser.cpp"
    break;

  case 384: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2881 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7383 "parser.cpp"
    break;

  case 385: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2887 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7394 "parser.cpp"
    break;

  case 386: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2893 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7406 "parser.cpp"
    break;

  case 387: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2900 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7418 "parser.cpp"
    break;

  case 388: /* column_expr: IDENTIFIER  */
#line 2908 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7430 "parser.cpp"
    break;

  case 389: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2915 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7442 "parser.cpp"
    break;

  case 390: /* column_expr: '*'  */
#line 2922 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7452 "parser.cpp"
    break;

  case 391: /* column_expr: column_expr '.' '*'  */
#line 2927 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7466 "parser.cpp"
    break;

  case 392: /* constant_expr: STRING  */
#line 2937 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7476 "parser.cpp"
    break;

  case 393: /* constant_expr: TRUE  */
#line 2942 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7486 "parser.cpp"
    break;

  case 394: /* constant_expr: FALSE  */
#line 2947 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7496 "parser.cpp"
    break;

  case 395: /* constant_expr: DOUBLE_VALUE  */
#line 2952 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7506 "parser.cpp"
    break;

  case 396: /* constant_expr: LONG_VALUE  */
#line 2957 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7516 "parser.cpp"
    break;

  case 397: /* constant_expr: DATE STRING  */
#line 2962 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7526 "parser.cpp"
    break;

  case 398: /* constant_expr: TIME STRING  */
#line 2967 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7536 "parser.cpp"
    break;

  case 399: /* constant_expr: DATETIME STRING  */
#line 2972 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7546 "parser.cpp"
    break;

  case 400: /* constant_expr: TIMESTAMP STRING  */
#line 2977 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7556 "parser.cpp"
    break;

  case 401: /* constant_expr: INTERVAL interval_expr  */
#line 2982 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7564 "parser.cpp"
    break;

  case 402: /* constant_expr: interval_expr  */
#line 2985 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7572 "parser.cpp"
    break;

  case 403: /* constant_expr: common_array_expr  */
#line 2988 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7580 "parser.cpp"
    break;

  case 404: /* common_array_expr: array_expr  */
#line 2992 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7588 "parser.cpp"
    break;

  case 405: /* common_array_expr: subarray_array_expr  */
#line 2995 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7596 "parser.cpp"
    break;

  case 406: /* common_array_expr: sparse_array_expr  */
#line 2998 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7604 "parser.cpp"
    break;

  case 407: /* common_array_expr: empty_array_expr  */
#line 3001 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7612 "parser.cpp"
    break;

  case 408: /* common_sparse_array_expr: sparse_array_expr  */
#line 3005 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7620 "parser.cpp"
    break;

  case 409: /* common_sparse_array_expr: array_expr  */
#line 3008 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7628 "parser.cpp"
    break;

  case 410: /* common_sparse_array_expr: empty_array_expr  */
#line 3011 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7636 "parser.cpp"
    break;

  case 411: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3015 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7644 "parser.cpp"
    break;

  case 412: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3019 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7654 "parser.cpp"
    break;

  case 413: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3024 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7663 "parser.cpp"
    break;

  case 414: /* sparse_array_expr: long_sparse_array_expr  */
#line 3029 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7671 "parser.cpp"
    break;

  case 415: /* sparse_array_expr: double_sparse_array_expr  */
#line 3032 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7679 "parser.cpp"
    break;

  case 416: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3036 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7687 "parser.cpp"
    break;

  case 417: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3040 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7699 "parser.cpp"
    break;

  case 418: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3047 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7710 "parser.cpp"
    break;

  case 419: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3054 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7718 "parser.cpp"
    break;

  case 420: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3058 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7730 "parser.cpp"
    break;

  case 421: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3065 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7741 "parser.cpp"
    break;

  case 422: /* empty_array_expr: '[' ']'  */
#line 3072 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7749 "parser.cpp"
    break;

  case 423: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3076 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7757 "parser.cpp"
    break;

  case 424: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3080 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7765 "parser.cpp"
    break;

  case 425: /* array_expr: long_array_expr  */
#line 3084 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7773 "parser.cpp"
    break;

  case 426: /* array_expr: double_array_expr  */
#line 3087 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7781 "parser.cpp"
    break;

  case 427: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3091 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7789 "parser.cpp"
    break;

  case 428: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3095 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7799 "parser.cpp"
    break;

  case 429: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3100 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7808 "parser.cpp"
    break;

  case 430: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3105 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7816 "parser.cpp"
    break;

  case 431: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3109 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7826 "parser.cpp"
    break;

  case 432: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3114 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7835 "parser.cpp"
    break;

  case 433: /* interval_expr: LONG_VALUE SECONDS  */
#line 3119 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7846 "parser.cpp"
    break;

  case 434: /* interval_expr: LONG_VALUE SECOND  */
#line 3125 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7857 "parser.cpp"
    break;

  case 435: /* interval_expr: LONG_VALUE MINUTES  */
#line 3131 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7868 "parser.cpp"
    break;

  case 436: /* interval_expr: LONG_VALUE MINUTE  */
#line 3137 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7879 "parser.cpp"
    break;

  case 437: /* interval_expr: LONG_VALUE HOURS  */
#line 3143 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7890 "parser.cpp"
    break;

  case 438: /* interval_expr: LONG_VALUE HOUR  */
#line 3149 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7901 "parser.cpp"
    break;

  case 439: /* interval_expr: LONG_VALUE DAYS  */
#line 3155 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7912 "parser.cpp"
    break;

  case 440: /* interval_expr: LONG_VALUE DAY  */
#line 3161 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7923 "parser.cpp"
    break;

  case 441: /* interval_expr: LONG_VALUE MONTHS  */
#line 3167 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7934 "parser.cpp"
    break;

  case 442: /* interval_expr: LONG_VALUE MONTH  */
#line 3173 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7945 "parser.cpp"
    break;

  case 443: /* interval_expr: LONG_VALUE YEARS  */
#line 3179 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7956 "parser.cpp"
    break;

  case 444: /* interval_expr: LONG_VALUE YEAR  */
#line 3185 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7967 "parser.cpp"
    break;

  case 445: /* copy_option_list: copy_option  */
#line 3196 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7976 "parser.cpp"
    break;

  case 446: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3200 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7985 "parser.cpp"
    break;

  case 447: /* copy_option: FORMAT IDENTIFIER  */
#line 3205 "parser.y"
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
#line 8020 "parser.cpp"
    break;

  case 448: /* copy_option: DELIMITER STRING  */
#line 3235 "parser.y"
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
#line 8035 "parser.cpp"
    break;

  case 449: /* copy_option: HEADER  */
#line 3245 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8045 "parser.cpp"
    break;

  case 450: /* copy_option: OFFSET LONG_VALUE  */
#line 3250 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8055 "parser.cpp"
    break;

  case 451: /* copy_option: LIMIT LONG_VALUE  */
#line 3255 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8065 "parser.cpp"
    break;

  case 452: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3260 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8075 "parser.cpp"
    break;

  case 453: /* file_path: STRING  */
#line 3266 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8083 "parser.cpp"
    break;

  case 454: /* if_exists: IF EXISTS  */
#line 3270 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8089 "parser.cpp"
    break;

  case 455: /* if_exists: %empty  */
#line 3271 "parser.y"
  { (yyval.bool_value) = false; }
#line 8095 "parser.cpp"
    break;

  case 456: /* if_not_exists: IF NOT EXISTS  */
#line 3273 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8101 "parser.cpp"
    break;

  case 457: /* if_not_exists: %empty  */
#line 3274 "parser.y"
  { (yyval.bool_value) = false; }
#line 8107 "parser.cpp"
    break;

  case 460: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3289 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8120 "parser.cpp"
    break;

  case 461: /* if_not_exists_info: %empty  */
#line 3297 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8128 "parser.cpp"
    break;

  case 462: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3301 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8136 "parser.cpp"
    break;

  case 463: /* with_index_param_list: %empty  */
#line 3304 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8144 "parser.cpp"
    break;

  case 464: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3308 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8152 "parser.cpp"
    break;

  case 465: /* optional_table_properties_list: %empty  */
#line 3311 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8160 "parser.cpp"
    break;

  case 466: /* index_param_list: index_param  */
#line 3315 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8169 "parser.cpp"
    break;

  case 467: /* index_param_list: index_param_list ',' index_param  */
#line 3319 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8178 "parser.cpp"
    break;

  case 468: /* index_param: IDENTIFIER  */
#line 3324 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8189 "parser.cpp"
    break;

  case 469: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3330 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8204 "parser.cpp"
    break;

  case 470: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3340 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8216 "parser.cpp"
    break;

  case 471: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3347 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8228 "parser.cpp"
    break;

  case 472: /* index_info_list: index_info_list_one_pack  */
#line 3358 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8237 "parser.cpp"
    break;

  case 473: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3362 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8249 "parser.cpp"
    break;

  case 474: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3370 "parser.y"
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
#line 8308 "parser.cpp"
    break;

  case 475: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3424 "parser.y"
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
#line 8326 "parser.cpp"
    break;


#line 8330 "parser.cpp"

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

#line 3438 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

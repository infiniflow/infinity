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
  YYSYMBOL_IGNORE = 126,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 127,                  /* PRIMARY  */
  YYSYMBOL_KEY = 128,                      /* KEY  */
  YYSYMBOL_UNIQUE = 129,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 130,                 /* NULLABLE  */
  YYSYMBOL_IS = 131,                       /* IS  */
  YYSYMBOL_DEFAULT = 132,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 133,                     /* TRUE  */
  YYSYMBOL_FALSE = 134,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 135,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 136,                   /* SECOND  */
  YYSYMBOL_SECONDS = 137,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 138,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 139,                  /* MINUTES  */
  YYSYMBOL_HOUR = 140,                     /* HOUR  */
  YYSYMBOL_HOURS = 141,                    /* HOURS  */
  YYSYMBOL_DAY = 142,                      /* DAY  */
  YYSYMBOL_DAYS = 143,                     /* DAYS  */
  YYSYMBOL_MONTH = 144,                    /* MONTH  */
  YYSYMBOL_MONTHS = 145,                   /* MONTHS  */
  YYSYMBOL_YEAR = 146,                     /* YEAR  */
  YYSYMBOL_YEARS = 147,                    /* YEARS  */
  YYSYMBOL_EQUAL = 148,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 149,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 150,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 151,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 152,                  /* BETWEEN  */
  YYSYMBOL_AND = 153,                      /* AND  */
  YYSYMBOL_OR = 154,                       /* OR  */
  YYSYMBOL_EXTRACT = 155,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 156,                     /* LIKE  */
  YYSYMBOL_DATA = 157,                     /* DATA  */
  YYSYMBOL_LOG = 158,                      /* LOG  */
  YYSYMBOL_BUFFER = 159,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 160,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 161,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 162,                 /* MEMINDEX  */
  YYSYMBOL_USING = 163,                    /* USING  */
  YYSYMBOL_SESSION = 164,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 165,                   /* GLOBAL  */
  YYSYMBOL_OFF = 166,                      /* OFF  */
  YYSYMBOL_EXPORT = 167,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 168,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 169,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 170,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 171,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 172,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 173,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 174,                    /* DELTA  */
  YYSYMBOL_LOGS = 175,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 176,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 177,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 178,                   /* SEARCH  */
  YYSYMBOL_MATCH = 179,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 180,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 181,                    /* QUERY  */
  YYSYMBOL_QUERIES = 182,                  /* QUERIES  */
  YYSYMBOL_FUSION = 183,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 184,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 185,                    /* ADMIN  */
  YYSYMBOL_PERSISTENCE = 186,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 187,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 188,                  /* OBJECTS  */
  YYSYMBOL_FILES = 189,                    /* FILES  */
  YYSYMBOL_NUMBER = 190,                   /* NUMBER  */
  YYSYMBOL_191_ = 191,                     /* '='  */
  YYSYMBOL_192_ = 192,                     /* '<'  */
  YYSYMBOL_193_ = 193,                     /* '>'  */
  YYSYMBOL_194_ = 194,                     /* '+'  */
  YYSYMBOL_195_ = 195,                     /* '-'  */
  YYSYMBOL_196_ = 196,                     /* '*'  */
  YYSYMBOL_197_ = 197,                     /* '/'  */
  YYSYMBOL_198_ = 198,                     /* '%'  */
  YYSYMBOL_199_ = 199,                     /* '['  */
  YYSYMBOL_200_ = 200,                     /* ']'  */
  YYSYMBOL_201_ = 201,                     /* '('  */
  YYSYMBOL_202_ = 202,                     /* ')'  */
  YYSYMBOL_203_ = 203,                     /* '.'  */
  YYSYMBOL_204_ = 204,                     /* ';'  */
  YYSYMBOL_205_ = 205,                     /* ','  */
  YYSYMBOL_206_ = 206,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 207,                 /* $accept  */
  YYSYMBOL_input_pattern = 208,            /* input_pattern  */
  YYSYMBOL_statement_list = 209,           /* statement_list  */
  YYSYMBOL_statement = 210,                /* statement  */
  YYSYMBOL_explainable_statement = 211,    /* explainable_statement  */
  YYSYMBOL_create_statement = 212,         /* create_statement  */
  YYSYMBOL_table_element_array = 213,      /* table_element_array  */
  YYSYMBOL_table_element = 214,            /* table_element  */
  YYSYMBOL_table_column = 215,             /* table_column  */
  YYSYMBOL_column_type = 216,              /* column_type  */
  YYSYMBOL_column_constraints = 217,       /* column_constraints  */
  YYSYMBOL_column_constraint = 218,        /* column_constraint  */
  YYSYMBOL_default_expr = 219,             /* default_expr  */
  YYSYMBOL_table_constraint = 220,         /* table_constraint  */
  YYSYMBOL_identifier_array = 221,         /* identifier_array  */
  YYSYMBOL_delete_statement = 222,         /* delete_statement  */
  YYSYMBOL_insert_statement = 223,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 224, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 225,        /* explain_statement  */
  YYSYMBOL_explain_type = 226,             /* explain_type  */
  YYSYMBOL_update_statement = 227,         /* update_statement  */
  YYSYMBOL_update_expr_array = 228,        /* update_expr_array  */
  YYSYMBOL_update_expr = 229,              /* update_expr  */
  YYSYMBOL_drop_statement = 230,           /* drop_statement  */
  YYSYMBOL_copy_statement = 231,           /* copy_statement  */
  YYSYMBOL_select_statement = 232,         /* select_statement  */
  YYSYMBOL_select_with_paren = 233,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 234,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 235, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 236, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 237, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 238,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 239,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 240,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 241,            /* order_by_type  */
  YYSYMBOL_limit_expr = 242,               /* limit_expr  */
  YYSYMBOL_offset_expr = 243,              /* offset_expr  */
  YYSYMBOL_distinct = 244,                 /* distinct  */
  YYSYMBOL_from_clause = 245,              /* from_clause  */
  YYSYMBOL_search_clause = 246,            /* search_clause  */
  YYSYMBOL_where_clause = 247,             /* where_clause  */
  YYSYMBOL_having_clause = 248,            /* having_clause  */
  YYSYMBOL_group_by_clause = 249,          /* group_by_clause  */
  YYSYMBOL_set_operator = 250,             /* set_operator  */
  YYSYMBOL_table_reference = 251,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 252,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 253,     /* table_reference_name  */
  YYSYMBOL_table_name = 254,               /* table_name  */
  YYSYMBOL_table_alias = 255,              /* table_alias  */
  YYSYMBOL_with_clause = 256,              /* with_clause  */
  YYSYMBOL_with_expr_list = 257,           /* with_expr_list  */
  YYSYMBOL_with_expr = 258,                /* with_expr  */
  YYSYMBOL_join_clause = 259,              /* join_clause  */
  YYSYMBOL_join_type = 260,                /* join_type  */
  YYSYMBOL_show_statement = 261,           /* show_statement  */
  YYSYMBOL_flush_statement = 262,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 263,       /* optimize_statement  */
  YYSYMBOL_command_statement = 264,        /* command_statement  */
  YYSYMBOL_compact_statement = 265,        /* compact_statement  */
  YYSYMBOL_admin_statement = 266,          /* admin_statement  */
  YYSYMBOL_expr_array = 267,               /* expr_array  */
  YYSYMBOL_expr_array_list = 268,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 269,               /* expr_alias  */
  YYSYMBOL_expr = 270,                     /* expr  */
  YYSYMBOL_operand = 271,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 272, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 273,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 274,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 275,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 276,          /* match_text_expr  */
  YYSYMBOL_query_expr = 277,               /* query_expr  */
  YYSYMBOL_fusion_expr = 278,              /* fusion_expr  */
  YYSYMBOL_sub_search = 279,               /* sub_search  */
  YYSYMBOL_sub_search_array = 280,         /* sub_search_array  */
  YYSYMBOL_function_expr = 281,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 282,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 283,             /* between_expr  */
  YYSYMBOL_in_expr = 284,                  /* in_expr  */
  YYSYMBOL_case_expr = 285,                /* case_expr  */
  YYSYMBOL_case_check_array = 286,         /* case_check_array  */
  YYSYMBOL_cast_expr = 287,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 288,            /* subquery_expr  */
  YYSYMBOL_column_expr = 289,              /* column_expr  */
  YYSYMBOL_constant_expr = 290,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 291,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 292, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 293,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 294, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 295,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 296,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 297, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 298, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 299, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 300,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 301,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 302,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 303,               /* array_expr  */
  YYSYMBOL_long_array_expr = 304,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 305, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 306,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 307, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 308,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 309,         /* copy_option_list  */
  YYSYMBOL_copy_option = 310,              /* copy_option  */
  YYSYMBOL_file_path = 311,                /* file_path  */
  YYSYMBOL_if_exists = 312,                /* if_exists  */
  YYSYMBOL_if_not_exists = 313,            /* if_not_exists  */
  YYSYMBOL_semicolon = 314,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 315,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 316,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 317, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 318,         /* index_param_list  */
  YYSYMBOL_index_param = 319,              /* index_param  */
  YYSYMBOL_index_info = 320                /* index_info  */
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
#define YYLAST   1291

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  207
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  477
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1028

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   445


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
       2,     2,     2,     2,     2,     2,     2,   198,     2,     2,
     201,   202,   196,   194,   205,   195,   203,   197,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   206,   204,
     192,   191,   193,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   199,     2,   200,     2,     2,     2,     2,     2,     2,
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
     185,   186,   187,   188,   189,   190
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   494,   494,   498,   504,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   544,   561,   577,   606,   622,   640,   669,   673,   679,
     682,   689,   739,   775,   776,   777,   778,   779,   780,   781,
     782,   783,   784,   785,   786,   787,   788,   789,   790,   791,
     792,   793,   794,   795,   798,   800,   801,   802,   803,   806,
     807,   808,   809,   810,   811,   812,   813,   814,   815,   816,
     817,   818,   819,   820,   821,   822,   823,   824,   825,   826,
     827,   828,   829,   830,   831,   832,   833,   834,   835,   836,
     837,   838,   839,   840,   841,   842,   843,   844,   845,   846,
     847,   848,   849,   850,   851,   852,   853,   854,   855,   856,
     857,   858,   859,   860,   879,   883,   893,   896,   899,   902,
     906,   909,   914,   919,   926,   932,   942,   958,   992,  1005,
    1008,  1015,  1021,  1024,  1027,  1030,  1033,  1036,  1039,  1042,
    1049,  1062,  1066,  1071,  1084,  1097,  1112,  1127,  1142,  1165,
    1218,  1273,  1324,  1327,  1330,  1339,  1349,  1352,  1356,  1361,
    1383,  1386,  1391,  1407,  1410,  1414,  1418,  1423,  1429,  1432,
    1435,  1439,  1443,  1445,  1449,  1451,  1454,  1458,  1461,  1465,
    1470,  1474,  1477,  1481,  1484,  1488,  1491,  1495,  1498,  1501,
    1504,  1512,  1515,  1530,  1530,  1532,  1546,  1555,  1560,  1569,
    1574,  1579,  1585,  1592,  1595,  1599,  1602,  1607,  1619,  1626,
    1640,  1643,  1646,  1649,  1652,  1655,  1658,  1664,  1668,  1672,
    1676,  1680,  1687,  1691,  1695,  1699,  1703,  1708,  1712,  1717,
    1721,  1725,  1731,  1737,  1743,  1754,  1765,  1776,  1788,  1800,
    1813,  1827,  1838,  1852,  1868,  1885,  1889,  1893,  1897,  1901,
    1905,  1915,  1919,  1923,  1931,  1942,  1965,  1971,  1976,  1982,
    1988,  1996,  2002,  2008,  2014,  2020,  2028,  2034,  2040,  2046,
    2052,  2060,  2066,  2073,  2086,  2090,  2095,  2101,  2108,  2116,
    2125,  2135,  2145,  2156,  2167,  2179,  2191,  2201,  2212,  2224,
    2237,  2241,  2246,  2251,  2262,  2266,  2271,  2275,  2302,  2308,
    2312,  2313,  2314,  2315,  2316,  2318,  2321,  2327,  2330,  2331,
    2332,  2333,  2334,  2335,  2336,  2337,  2338,  2339,  2341,  2344,
    2350,  2369,  2414,  2452,  2494,  2540,  2561,  2581,  2599,  2617,
    2625,  2636,  2642,  2651,  2657,  2669,  2672,  2675,  2678,  2681,
    2684,  2688,  2692,  2697,  2705,  2713,  2722,  2729,  2736,  2743,
    2750,  2757,  2765,  2773,  2781,  2789,  2797,  2805,  2813,  2821,
    2829,  2837,  2845,  2853,  2883,  2891,  2900,  2908,  2917,  2925,
    2931,  2938,  2944,  2951,  2956,  2963,  2970,  2978,  3002,  3008,
    3014,  3021,  3029,  3036,  3043,  3048,  3058,  3063,  3068,  3073,
    3078,  3083,  3088,  3093,  3098,  3103,  3106,  3109,  3113,  3116,
    3119,  3122,  3126,  3129,  3132,  3136,  3140,  3145,  3150,  3153,
    3157,  3161,  3168,  3175,  3179,  3186,  3193,  3197,  3201,  3205,
    3208,  3212,  3216,  3221,  3226,  3230,  3235,  3240,  3246,  3252,
    3258,  3264,  3270,  3276,  3282,  3288,  3294,  3300,  3306,  3317,
    3321,  3326,  3356,  3366,  3371,  3376,  3381,  3387,  3391,  3392,
    3394,  3395,  3397,  3398,  3410,  3418,  3422,  3425,  3429,  3432,
    3436,  3440,  3445,  3451,  3461,  3469,  3480,  3511
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
  "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE",
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
  "index_info", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-531)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-466)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     618,   257,    27,   337,    86,    20,    86,   132,   938,   579,
      70,   403,   139,   239,   250,   -18,   190,   -37,   263,    63,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,   201,  -531,
    -531,   271,  -531,  -531,  -531,  -531,  -531,  -531,   195,   195,
     195,   195,    38,    86,   221,   221,   221,   221,   221,   104,
     309,    86,   -32,   336,   349,   357,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,   331,   366,    86,  -531,  -531,  -531,  -531,
    -531,   367,  -531,   222,   227,  -531,   380,  -531,   210,  -531,
    -531,   265,  -531,   393,    86,  -531,  -531,  -531,  -531,   -47,
    -531,   389,   182,  -531,   429,    80,   246,   259,  -531,    67,
    -531,   438,  -531,  -531,     2,   401,  -531,   410,   411,   469,
      86,    86,    86,   487,   436,   313,   430,   553,    86,    86,
      86,   555,   573,   575,   519,   592,   592,   471,    56,    60,
      82,  -531,  -531,  -531,  -531,  -531,  -531,  -531,   201,  -531,
    -531,  -531,  -531,  -531,  -531,   329,  -531,  -531,   595,  -531,
     598,  -531,  -531,   596,   599,  -531,  -531,  -531,    86,   409,
     250,   592,   606,  -531,  -531,   607,  -531,  -531,  -531,  -531,
       2,  -531,  -531,  -531,   471,   559,   548,   543,  -531,   -31,
    -531,   313,  -531,    86,   617,    15,  -531,  -531,  -531,  -531,
    -531,   558,  -531,   444,   -33,  -531,   471,  -531,  -531,   556,
     557,   437,  -531,  -531,   397,   513,   439,   441,   288,   647,
     649,   651,   657,  -531,  -531,   654,   468,   208,   473,   474,
     587,   587,  -531,    13,   330,    -8,  -531,    -9,   641,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,   475,  -531,  -531,  -531,    31,  -531,  -531,    44,
    -531,    45,  -531,  -531,  -531,    73,  -531,    76,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,   673,   671,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,   604,   271,  -531,  -531,   147,   680,   477,
     485,   -45,   471,   471,   622,  -531,   -37,    66,   640,   510,
    -531,    98,   512,  -531,    86,   471,   575,  -531,     5,   514,
     516,   125,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,   587,   518,   372,   652,   471,   471,
     -38,   261,  -531,  -531,  -531,  -531,   397,  -531,   727,   533,
     534,   535,   536,   729,   741,   388,   388,  -531,   540,  -531,
    -531,  -531,  -531,   549,   112,   668,   471,   749,   471,   471,
     -13,   561,   -22,   587,   587,   587,   587,   587,   587,   587,
     587,   587,   587,   587,   587,   587,   587,    21,  -531,   560,
    -531,   750,  -531,   751,  -531,   752,  -531,   758,   718,   241,
     566,  -531,   567,   767,  -531,    39,  -531,  -531,     6,   593,
     569,  -531,    -2,     5,   471,  -531,   201,   765,   648,   574,
     107,  -531,  -531,  -531,   -37,   774,  -531,  -531,   775,   471,
     589,  -531,     5,  -531,    40,    40,   471,  -531,   120,   652,
     623,   578,    -5,   108,   264,  -531,   471,   471,   706,   471,
     791,    22,   471,   160,   202,   407,  -531,  -531,   592,  -531,
    -531,  -531,   643,   597,   587,   330,   669,  -531,   656,   656,
     258,   258,   532,   656,   656,   258,   258,   388,   388,  -531,
    -531,  -531,  -531,  -531,  -531,   594,  -531,   602,  -531,  -531,
    -531,   795,   796,  -531,   806,  -531,  -531,   804,  -531,   -37,
     613,   901,  -531,    96,  -531,   203,   519,   471,  -531,  -531,
    -531,     5,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,   610,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,   621,   624,   625,   629,
     630,   155,   639,   617,   813,    66,   201,   642,  -531,   223,
     644,   818,   836,   819,   843,  -531,   866,   234,  -531,   235,
     240,  -531,   677,  -531,   765,   471,  -531,   471,   -40,   117,
     587,   -48,   675,  -531,  -149,    34,  -531,   878,  -531,   879,
    -531,  -531,   807,   330,   656,   684,   255,  -531,   587,   890,
     892,   847,   852,   710,   268,  -531,   897,    12,     6,   845,
    -531,  -531,  -531,  -531,  -531,  -531,   854,  -531,   911,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,   712,   864,  -531,
     913,   722,   800,   817,   834,   851,   790,   802,  -531,  -531,
     145,  -531,   799,   617,   276,   733,  -531,  -531,   772,  -531,
     471,  -531,  -531,  -531,  -531,  -531,  -531,    40,  -531,  -531,
    -531,   734,     5,     1,  -531,   471,   508,   738,   943,   560,
     753,   746,   763,   768,   766,   286,  -531,  -531,   372,   945,
     947,   421,  -531,   806,    43,    96,   901,     6,     6,   769,
     203,   899,   915,   290,   777,   778,   779,   780,   781,   782,
     783,   784,   785,   886,   786,   787,   788,   789,   792,   793,
     794,   797,   798,   801,   887,   803,   805,   808,   809,   810,
     811,   812,   814,   815,   816,   902,   820,   821,   822,   823,
     824,   825,   826,   827,   828,   829,   903,   830,   831,   832,
     833,   835,   837,   838,   839,   840,   841,   907,   842,  -531,
    -531,    33,  -531,  -531,  -531,   292,  -531,   806,   998,   297,
    -531,  -531,  -531,     5,  -531,   545,   844,   299,   846,    16,
     848,  -531,  -531,  -531,  -531,  -531,    40,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,   999,  -531,  -531,  -531,   940,
     617,  -531,   471,   471,  -531,  -531,  1001,  1003,  1005,  1006,
    1012,  1016,  1017,  1018,  1033,  1035,   849,  1042,  1044,  1046,
    1049,  1050,  1051,  1052,  1053,  1054,  1055,   857,  1057,  1058,
    1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,   868,  1068,
    1069,  1070,  1071,  1072,  1073,  1074,  1075,  1076,  1077,   880,
    1078,  1080,  1081,  1082,  1083,  1084,  1085,  1086,  1087,  1088,
     891,  1089,  -531,  -531,   303,   604,  -531,  -531,  1093,  -531,
    1094,  1095,  1096,   305,  1097,   471,   310,   896,     5,   900,
     904,   905,   906,   908,   909,   910,   912,   914,   916,  1098,
     917,   918,   919,   920,   921,   922,   923,   924,   925,   926,
    1099,   927,   928,   929,   930,   931,   932,   933,   934,   935,
     936,  1103,   937,   939,   941,   942,   944,   946,   948,   949,
     950,   951,  1107,   952,   953,   954,   955,   956,   957,   958,
     959,   960,   961,  1109,   962,  -531,  -531,   963,   964,   965,
     325,  -531,   351,     5,  -531,  -531,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,   967,  -531,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,   969,  -531,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,   970,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,   971,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,  -531,   972,
    -531,  1113,  -531,  1136,   604,  1139,  1141,  1143,  -531,  -531,
    -531,  -531,  -531,  -531,  -531,  -531,   343,   973,  -531,   974,
    1159,   505,   604,  1161,  1138,   975,   -25,   525,  1169,  -531,
    -531,   977,  -531,  -531,  1140,  1142,  -531,  1174,  -531,  1130,
     -23,  -531,   982,  -531,  -531,  1144,  1145,  -531,  1182,  -531,
     987,   988,  1186,   604,   989,  -531,   604,  -531
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     214,     0,     0,     0,     0,     0,     0,     0,   149,     0,
       0,     0,     0,     0,     0,     0,     0,   214,     0,   463,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   163,
     162,     0,     8,    14,    15,    16,    17,    18,   461,   461,
     461,   461,   461,     0,   459,   459,   459,   459,   459,   207,
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
      32,   140,   464,     0,     0,   214,   458,   154,   156,   155,
     157,     0,   208,     0,   192,   151,     0,   136,   457,     0,
       0,   392,   396,   399,   400,     0,     0,     0,     0,     0,
       0,     0,     0,   397,   398,     0,     0,     0,     0,     0,
       0,     0,   394,     0,   214,     0,   304,   309,   310,   324,
     322,   325,   323,   326,   327,   319,   314,   313,   312,   320,
     321,   311,   318,   317,   407,   409,     0,   410,   418,     0,
     419,     0,   411,   408,   429,     0,   430,     0,   406,   270,
     272,   271,   268,   269,   275,   277,   276,   273,   274,   280,
     282,   281,   278,   279,     0,     0,   246,   245,   251,   241,
     242,   236,   260,   467,     0,   216,   267,   301,   285,     0,
       0,   188,     0,     0,   184,   460,   214,     0,     0,     0,
     134,     0,     0,   138,     0,     0,     0,   150,   191,     0,
       0,     0,   438,   437,   440,   439,   442,   441,   444,   443,
     446,   445,   448,   447,     0,     0,   358,   214,     0,     0,
       0,     0,   401,   402,   403,   404,     0,   405,     0,     0,
       0,     0,     0,     0,     0,   360,   359,   435,   432,   426,
     416,   421,   424,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   415,     0,
     420,     0,   423,     0,   431,     0,   434,     0,   252,   247,
       0,   265,     0,     0,   302,     0,   171,   170,     0,   190,
     173,   175,   180,   181,     0,   169,    34,     0,     0,     0,
       0,    37,    39,    40,   214,     0,    36,   139,     0,     0,
     137,   158,   153,   152,     0,     0,     0,   353,     0,   214,
       0,     0,     0,     0,     0,   383,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   316,   315,     0,   305,
     308,   376,   377,     0,     0,   214,     0,   357,   367,   368,
     371,   372,     0,   374,   366,   369,   370,   362,   361,   363,
     364,   365,   393,   395,   417,     0,   422,     0,   425,   433,
     436,     0,     0,   248,     0,   217,   303,     0,   286,   214,
     187,   201,   203,   212,   204,     0,   192,     0,   178,   179,
     177,   183,    43,    46,    47,    44,    45,    48,    49,    65,
      50,    52,    51,    68,    55,    56,    57,    53,    54,    58,
      59,    60,    61,    62,    63,    64,     0,     0,     0,     0,
       0,   467,     0,     0,   469,     0,    35,     0,   135,     0,
       0,     0,     0,     0,     0,   453,     0,     0,   449,     0,
       0,   354,     0,   388,     0,     0,   381,     0,     0,     0,
       0,     0,     0,   392,     0,     0,   341,     0,   343,     0,
     428,   427,     0,   214,   375,     0,     0,   356,     0,     0,
       0,   253,   249,   472,     0,   470,   287,     0,     0,     0,
     221,   222,   223,   224,   220,   225,     0,   210,     0,   205,
     347,   345,   348,   346,   349,   350,   351,   189,   196,   176,
       0,     0,     0,     0,     0,     0,     0,     0,   127,   128,
     131,   124,   131,     0,     0,     0,    33,    38,   477,   306,
       0,   455,   454,   452,   451,   456,   161,     0,   159,   355,
     389,     0,   385,     0,   384,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   390,   379,   378,     0,
       0,     0,   466,     0,     0,   212,   202,     0,     0,   209,
       0,     0,   194,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   129,
     126,     0,   125,    42,    41,     0,   133,     0,     0,     0,
     450,   387,   382,   386,   373,     0,     0,     0,     0,     0,
       0,   412,   414,   413,   342,   344,     0,   391,   380,   254,
     250,   473,   475,   474,   471,     0,   288,   206,   218,     0,
       0,   352,     0,     0,   172,    67,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   130,   132,     0,   467,   307,   432,     0,   339,
       0,     0,     0,     0,   289,     0,     0,   195,   193,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   468,   476,     0,     0,     0,
       0,   160,     0,   219,   211,    66,    72,    73,    70,    71,
      74,    75,    76,    77,    78,     0,    69,   105,   106,   103,
     104,   107,   108,   109,   110,   111,     0,   102,    83,    84,
      81,    82,    85,    86,    87,    88,    89,     0,    80,   116,
     117,   114,   115,   118,   119,   120,   121,   122,     0,   113,
      94,    95,    92,    93,    96,    97,    98,    99,   100,     0,
      91,     0,   340,     0,   467,     0,     0,     0,   291,   290,
     296,    79,   112,    90,   123,   101,     0,   329,   338,     0,
     297,   292,   467,     0,     0,     0,   467,     0,     0,   293,
     334,     0,   328,   330,     0,     0,   337,     0,   298,   294,
     467,   336,     0,   299,   295,     0,     0,   333,     0,   332,
       0,     0,     0,   467,     0,   335,   467,   331
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -531,  -531,  -531,  1100,  -531,  1129,  -531,   658,  -531,   646,
    -531,   576,   572,  -531,  -519,  1132,  1134,  1020,  -531,  -531,
    1135,  -531,   898,  1146,  1147,   -59,  1185,   -14,   966,  1036,
     -63,  -531,  -531,   708,  -531,  -531,  -531,  -531,  -531,  -531,
    -186,  -531,  -531,  -531,  -531,   615,   -73,     7,   542,  -531,
    -531,  1048,  -531,  -531,  1148,  1149,  1150,  1151,  1152,  -531,
    -172,  -531,   860,  -196,  -190,  -531,  -479,  -472,  -468,  -466,
    -455,  -451,   547,  -531,  -531,  -531,  -531,  -531,  -531,   888,
    -531,  -531,   850,   488,  -218,  -531,  -531,  -531,   570,  -531,
    -531,  -531,  -531,   571,   853,   855,   -98,  -531,  -531,  -531,
    -531,  1007,  -419,   586,  -119,   270,   378,  -531,  -531,  -530,
    -531,   489,   562,  -531
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   131,    21,   410,   411,   412,   531,
     620,   621,   733,   413,   301,    22,    23,   185,    24,    63,
      25,   194,   195,    26,    27,    28,    29,    30,   106,   171,
     107,   176,   400,   401,   500,   294,   405,   174,   399,   496,
     197,   774,   672,   104,   490,   491,   492,   493,   599,    31,
      92,    93,   494,   596,    32,    33,    34,    35,    36,    37,
     225,   420,   226,   227,   228,   995,   229,   230,   231,   232,
     233,   234,   606,   607,   235,   236,   237,   238,   239,   331,
     240,   241,   242,   243,   244,   750,   245,   246,   247,   248,
     249,   250,   251,   252,   351,   352,   253,   254,   255,   256,
     257,   258,   547,   548,   199,   117,   109,   100,   114,   391,
     626,   584,   585,   416
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     308,   622,   291,    97,   138,   350,   549,   200,   307,    49,
     105,    50,   330,    52,   624,   326,   600,   158,   347,   348,
      90,   347,   348,   601,   472,   563,   296,   602,   354,   603,
     345,   346,   101,   398,   102,   196,   103,   202,   203,   204,
     604,   172,   286,    14,   605,   644,   125,   126,   357,   302,
     115,   329,   554,   456,   377,   390,   649,   390,   124,   259,
      43,   260,   261,   264,   453,   265,   266,  -462,   487,   407,
     498,   499,   145,   765,     1,   766,     2,     3,     4,     5,
       6,     7,     8,     9,   488,   269,   742,   270,   271,    49,
      10,   157,    11,    12,    13,    14,   402,   403,    51,   597,
      84,  1004,  -465,  1015,   735,   358,   359,   290,   457,   422,
     541,   542,   108,   358,   359,   358,   359,   179,   180,   181,
     262,   543,   544,   545,   267,   188,   189,   190,   201,   202,
     203,   204,   432,   433,   326,   209,   210,   211,  1005,   428,
    1016,   212,    88,   454,   358,   359,   272,    14,   358,   359,
      94,   358,   359,   598,   358,   359,   122,   647,   358,   359,
     356,   474,   451,   452,    17,   283,   213,   214,   215,   127,
     297,   303,   306,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   469,   470,   471,   358,   359,   393,
     299,   600,   426,   408,   355,   409,   555,   356,   601,   394,
     205,   206,   602,   170,   603,   645,    95,   489,   501,   207,
     353,   208,   223,   349,   665,   604,   349,   473,   222,   605,
     616,   101,   263,   102,   546,   103,   268,   209,   210,   211,
     616,   378,   223,   212,    15,   390,   379,   406,   162,   650,
     558,   559,    89,   561,   380,   382,   565,   539,   273,   381,
     383,   846,    16,    91,   550,   163,   164,   165,   213,   214,
     215,   358,   359,    98,   574,   358,   359,    99,    17,   108,
     358,   359,   617,   384,   618,   619,   386,   731,   385,   105,
     216,   387,   617,   576,   618,   619,    38,    39,    40,   482,
     483,   201,   202,   203,   204,   116,    53,    54,    41,    42,
     417,   402,    55,   418,   217,   906,   218,   122,   219,   534,
     608,   421,   535,   431,   447,   118,   119,   120,   121,   220,
     221,   222,   551,   123,   223,   356,   224,   427,   339,   572,
     340,   341,   342,   201,   202,   203,   204,   843,     1,   128,
       2,     3,     4,     5,     6,     7,   435,     9,   436,   556,
     437,   557,   129,   437,    10,   536,    11,    12,    13,   642,
     130,   643,   566,   205,   206,   567,    44,    45,    46,   144,
     646,   274,   207,   146,   208,   275,   276,   329,    47,    48,
     277,   278,   217,   151,   218,   152,   219,   160,   658,   362,
     209,   210,   211,   976,   147,   148,   212,   977,   978,   149,
     150,   655,   979,   980,   568,   205,   206,   569,  -466,  -466,
      14,    14,   570,   571,   207,   552,   208,   110,   111,   112,
     113,   213,   214,   215,   761,   629,   762,   763,   356,   153,
     587,   748,   209,   210,   211,   161,   636,   638,   212,   637,
     637,   575,   639,   216,   988,   356,   159,   430,   166,   743,
    -466,  -466,   372,   373,   374,   375,   376,   657,   739,   169,
     356,   167,  1000,   213,   214,   215,  1006,   217,   173,   218,
     662,   219,   178,   663,   201,   202,   203,   204,   736,   175,
    1017,   418,   220,   221,   222,   216,   177,   223,   758,   224,
     182,   356,   775,  1025,   833,   776,  1027,   418,    15,   836,
     183,   839,   356,   362,   840,   905,   186,   911,   663,   217,
     637,   218,   914,   219,   184,   418,   201,   202,   203,   204,
     363,   364,   365,   366,   220,   221,   222,   974,   368,   223,
     975,   224,    17,   312,   313,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   992,   205,   206,   993,   746,
     347,   837,   753,   998,   999,   207,   187,   208,   191,   654,
      85,    86,    87,   369,   370,   371,   372,   373,   374,   375,
     376,  1007,  1008,   209,   210,   211,   192,   848,   193,   212,
     154,   155,   156,   430,   374,   375,   376,   196,   324,   325,
     201,   202,   203,   204,   768,   769,   198,   207,   279,   208,
     847,   280,   281,   282,   213,   214,   215,   430,    64,    65,
     284,    66,   287,   288,   292,   209,   210,   211,   293,   295,
     300,   212,   304,    67,    68,     1,   216,     2,     3,     4,
       5,     6,     7,     8,     9,   305,   309,   310,   311,   362,
     327,    10,   328,    11,    12,    13,   213,   214,   215,   913,
     217,   332,   218,   333,   219,   334,   363,   364,   365,   366,
     336,   335,   324,   362,   368,   220,   221,   222,   216,   338,
     223,   207,   224,   208,   343,   344,   388,   389,   377,   396,
     363,   364,   365,   366,   390,   578,   395,   397,   368,   209,
     210,   211,   217,   404,   218,   212,   219,   414,    14,   369,
     370,   371,   372,   373,   374,   375,   376,   220,   221,   222,
     744,   415,   223,   419,   224,   424,   360,   425,   361,   429,
     213,   214,   215,   369,   370,   371,   372,   373,   374,   375,
     376,   438,    14,   443,   439,   440,   441,   442,    69,    70,
      71,    72,   216,    73,    74,   444,   445,   448,    75,    76,
      77,   446,   450,    78,    79,    80,   475,   477,   479,   223,
      81,    82,   455,   480,   481,    83,   217,   484,   218,   485,
     219,   495,   362,   486,   497,   533,   532,   537,   538,   454,
     553,   220,   221,   222,   560,    15,   223,   362,   224,   363,
     364,   365,   366,   367,   540,   562,   358,   368,   573,   577,
     579,   581,   582,    16,  -466,  -466,   365,   366,   580,   583,
     586,   610,  -466,   674,   675,   676,   677,   678,   588,    17,
     679,   680,   611,   633,   631,   612,   613,   681,   682,   683,
     614,   615,   369,   370,   371,   372,   373,   374,   375,   376,
     623,   625,   632,   684,   628,   630,   634,  -466,   370,   371,
     372,   373,   374,   375,   376,   502,   503,   504,   505,   506,
     507,   508,   509,   510,   511,   512,   513,   514,   515,   516,
     517,   518,   635,   519,   520,   521,   522,   523,   524,   640,
     648,   525,   651,   652,   526,   527,   656,   653,   528,   529,
     530,   685,   686,   687,   688,   689,   571,   570,   690,   691,
     659,   661,   660,   664,   667,   692,   693,   694,   696,   697,
     698,   699,   700,   668,   669,   701,   702,   670,   671,   673,
     729,   695,   703,   704,   705,   707,   708,   709,   710,   711,
     730,   731,   712,   713,   737,   738,   741,   745,   706,   714,
     715,   716,   718,   719,   720,   721,   722,   747,   754,   723,
     724,   759,   749,   760,   772,   717,   725,   726,   727,   589,
    -226,   590,   591,   592,   593,   755,   594,   595,   757,   756,
     770,   773,   728,    56,    57,    58,    59,    60,    61,   786,
     797,    62,   777,   778,   779,   780,   781,   782,   783,   784,
     785,   787,   788,   789,   790,   808,   819,   791,   792,   793,
     830,   835,   794,   795,   845,   844,   796,   849,   798,   850,
     799,   851,   852,   800,   801,   802,   803,   804,   853,   805,
     806,   807,   854,   855,   856,   809,   810,   811,   812,   813,
     814,   815,   816,   817,   818,   820,   821,   822,   823,   857,
     824,   858,   825,   826,   827,   828,   829,   831,   860,   838,
     861,   841,   862,   842,   859,   863,   864,   865,   866,   867,
     868,   869,   870,   871,   872,   873,   874,   875,   876,   877,
     878,   879,   880,   881,   882,   883,   884,   885,   886,   887,
     888,   889,   890,   891,   893,   892,   894,   895,   896,   897,
     898,   899,   900,   901,   902,   904,   903,   907,   908,   909,
     910,   356,   915,   912,   925,   936,   916,   917,   918,   947,
     919,   920,   921,   958,   922,   969,   923,   986,   924,   926,
     927,   928,   929,   930,   931,   932,   933,   934,   935,   937,
     938,   939,   940,   941,   942,   943,   944,   945,   946,   948,
     987,   949,  1002,   950,   951,   989,   952,   990,   953,   991,
     954,   955,   956,   957,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   970,   997,   972,  1001,   971,   981,
     973,   982,   983,   984,   985,  1009,   996,  1003,   994,  1010,
    1013,  1014,  1011,  1018,  1012,  1021,  1019,  1020,  1022,  1024,
    1023,  1026,   132,   627,   734,   133,   732,   134,   135,   168,
     641,   298,    96,   666,   423,   609,   289,   767,   285,   136,
     137,   139,   140,   141,   142,   143,   449,   771,   434,   832,
     751,   752,   337,   740,     0,   764,   834,     0,     0,     0,
       0,     0,     0,     0,   476,     0,     0,     0,   478,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     392,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   564
};

static const yytype_int16 yycheck[] =
{
     196,   531,   174,    17,    63,   223,   425,   126,   194,     3,
       8,     4,   208,     6,   533,   205,   495,    64,     5,     6,
      13,     5,     6,   495,     3,     3,    57,   495,   224,   495,
     220,   221,    20,    78,    22,    68,    24,     4,     5,     6,
     495,   104,   161,    80,   495,    85,    78,    79,    57,    34,
      43,    89,    57,    75,   203,    80,   205,    80,    51,     3,
      33,     5,     6,     3,    77,     5,     6,     0,    29,     3,
      72,    73,    65,    30,     7,    32,     9,    10,    11,    12,
      13,    14,    15,    16,    45,     3,    85,     5,     6,     3,
      23,    84,    25,    26,    27,    80,   292,   293,    78,     3,
      30,   126,    64,   126,   623,   153,   154,   170,   130,   305,
      70,    71,    74,   153,   154,   153,   154,   110,   111,   112,
      64,    81,    82,    83,    64,   118,   119,   120,     3,     4,
       5,     6,   328,   329,   324,   102,   103,   104,   163,   311,
     163,   108,     3,   156,   153,   154,    64,    80,   153,   154,
     168,   153,   154,    57,   153,   154,   203,   205,   153,   154,
     205,   379,   358,   359,   201,   158,   133,   134,   135,   201,
     201,   185,   205,   363,   364,   365,   366,   367,   368,   369,
     370,   371,   372,   373,   374,   375,   376,   153,   154,    42,
     183,   670,    67,   127,   202,   129,    88,   205,   670,    52,
      75,    76,   670,   201,   670,    88,    16,   201,   404,    84,
     224,    86,   199,   200,   202,   670,   200,   196,   196,   670,
      75,    20,   166,    22,   184,    24,   166,   102,   103,   104,
      75,   200,   199,   108,   167,    80,   205,   296,   158,   205,
     436,   437,     3,   439,   200,   200,   442,   419,   166,   205,
     205,   770,   185,     3,   426,   175,   176,   177,   133,   134,
     135,   153,   154,     0,   454,   153,   154,   204,   201,    74,
     153,   154,   127,   200,   129,   130,   200,   132,   205,     8,
     155,   205,   127,   455,   129,   130,    29,    30,    31,    48,
      49,     3,     4,     5,     6,    74,   164,   165,    41,    42,
     202,   497,   170,   205,   179,   835,   181,   203,   183,   202,
     496,   304,   205,   327,   202,    45,    46,    47,    48,   194,
     195,   196,   202,    14,   199,   205,   201,   202,   120,   448,
     122,   123,   124,     3,     4,     5,     6,   756,     7,     3,
       9,    10,    11,    12,    13,    14,    85,    16,    87,    85,
      89,    87,     3,    89,    23,   414,    25,    26,    27,   555,
       3,   557,   202,    75,    76,   205,    29,    30,    31,     3,
     560,    42,    84,     6,    86,    46,    47,    89,    41,    42,
      51,    52,   179,     3,   181,   175,   183,   205,   578,   131,
     102,   103,   104,    42,   172,   173,   108,    46,    47,   172,
     173,   573,    51,    52,   202,    75,    76,   205,   150,   151,
      80,    80,     5,     6,    84,   429,    86,    39,    40,    41,
      42,   133,   134,   135,     3,   202,     5,     6,   205,   164,
     489,   649,   102,   103,   104,     6,   202,   202,   108,   205,
     205,   455,   202,   155,   974,   205,    57,    75,   202,   645,
     192,   193,   194,   195,   196,   197,   198,   202,   630,    21,
     205,   202,   992,   133,   134,   135,   996,   179,    67,   181,
     202,   183,     3,   205,     3,     4,     5,     6,   202,    69,
    1010,   205,   194,   195,   196,   155,    75,   199,   202,   201,
       3,   205,   202,  1023,   202,   205,  1026,   205,   167,   202,
      64,   202,   205,   131,   205,   202,    76,   202,   205,   179,
     205,   181,   202,   183,   201,   205,     3,     4,     5,     6,
     148,   149,   150,   151,   194,   195,   196,   202,   156,   199,
     205,   201,   201,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   202,    75,    76,   205,   647,
       5,     6,   650,    48,    49,    84,     3,    86,     3,   573,
     157,   158,   159,   191,   192,   193,   194,   195,   196,   197,
     198,    46,    47,   102,   103,   104,     3,   773,     3,   108,
     187,   188,   189,    75,   196,   197,   198,    68,    75,    76,
       3,     4,     5,     6,   667,   668,     4,    84,     3,    86,
     772,     3,     6,     4,   133,   134,   135,    75,    29,    30,
     201,    32,     6,     6,    55,   102,   103,   104,    70,    76,
       3,   108,    64,    44,    45,     7,   155,     9,    10,    11,
      12,    13,    14,    15,    16,   191,    80,    80,   201,   131,
     201,    23,   201,    25,    26,    27,   133,   134,   135,   845,
     179,     4,   181,     4,   183,     4,   148,   149,   150,   151,
       6,     4,    75,   131,   156,   194,   195,   196,   155,   201,
     199,    84,   201,    86,   201,   201,     3,     6,   203,   202,
     148,   149,   150,   151,    80,   153,     6,   202,   156,   102,
     103,   104,   179,    71,   181,   108,   183,    57,    80,   191,
     192,   193,   194,   195,   196,   197,   198,   194,   195,   196,
     202,   201,   199,   201,   201,   201,    75,   201,    77,   201,
     133,   134,   135,   191,   192,   193,   194,   195,   196,   197,
     198,     4,    80,     4,   201,   201,   201,   201,   159,   160,
     161,   162,   155,   164,   165,     4,   206,    79,   169,   170,
     171,   202,     3,   174,   175,   176,     6,     6,     6,   199,
     181,   182,   201,     5,    46,   186,   179,   201,   181,   202,
     183,   178,   131,     6,   205,   201,   128,     3,     3,   156,
     202,   194,   195,   196,    78,   167,   199,   131,   201,   148,
     149,   150,   151,   152,   205,     4,   153,   156,   201,   130,
     206,     6,     6,   185,   148,   149,   150,   151,   206,     3,
       6,   201,   156,    91,    92,    93,    94,    95,   205,   201,
      98,    99,   201,     4,     6,   201,   201,   105,   106,   107,
     201,   201,   191,   192,   193,   194,   195,   196,   197,   198,
     201,    28,     6,   121,   202,   201,     3,   191,   192,   193,
     194,   195,   196,   197,   198,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,     6,   108,   109,   110,   111,   112,   113,   202,
     205,   116,     4,     4,   119,   120,   202,    80,   123,   124,
     125,    91,    92,    93,    94,    95,     6,     5,    98,    99,
      53,   191,    50,     6,    59,   105,   106,   107,    91,    92,
      93,    94,    95,    59,     3,    98,    99,   205,    54,     6,
     130,   121,   105,   106,   107,    91,    92,    93,    94,    95,
     128,   132,    98,    99,   201,   163,   202,   199,   121,   105,
     106,   107,    91,    92,    93,    94,    95,     4,   202,    98,
      99,     6,   199,     6,    55,   121,   105,   106,   107,    58,
      59,    60,    61,    62,    63,   202,    65,    66,   202,   201,
     201,    56,   121,    35,    36,    37,    38,    39,    40,    93,
      93,    43,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,    93,    93,   205,   205,   205,
      93,     3,   205,   205,    64,     6,   205,     6,   205,     6,
     205,     6,     6,   205,   205,   205,   205,   205,     6,   205,
     205,   205,     6,     6,     6,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,     6,
     205,     6,   205,   205,   205,   205,   205,   205,     6,   205,
       6,   205,     6,   205,   205,     6,     6,     6,     6,     6,
       6,     6,   205,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   205,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   205,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   205,     4,     4,     4,
       4,   205,   202,     6,     6,     6,   202,   202,   202,     6,
     202,   202,   202,     6,   202,     6,   202,     4,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
       4,   202,     4,   202,   202,     6,   202,     6,   202,     6,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,     6,   202,     6,   205,   202,
     205,   202,   202,   202,   202,     6,   202,   202,   205,   202,
       6,    51,    42,   201,    42,     3,    42,    42,   201,     3,
     202,   202,    63,   535,   622,    63,   620,    63,    63,    99,
     554,   181,    17,   588,   306,   497,   170,   665,   160,    63,
      63,    63,    63,    63,    63,    63,   356,   670,   330,   731,
     650,   650,   215,   637,    -1,   663,   737,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   381,    -1,    -1,    -1,   383,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     284,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   441
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   167,   185,   201,   208,   209,
     210,   212,   222,   223,   225,   227,   230,   231,   232,   233,
     234,   256,   261,   262,   263,   264,   265,   266,    29,    30,
      31,    41,    42,    33,    29,    30,    31,    41,    42,     3,
     254,    78,   254,   164,   165,   170,    35,    36,    37,    38,
      39,    40,    43,   226,    29,    30,    32,    44,    45,   159,
     160,   161,   162,   164,   165,   169,   170,   171,   174,   175,
     176,   181,   182,   186,    30,   157,   158,   159,     3,     3,
     254,     3,   257,   258,   168,    16,   233,   234,     0,   204,
     314,    20,    22,    24,   250,     8,   235,   237,    74,   313,
     313,   313,   313,   313,   315,   254,    74,   312,   312,   312,
     312,   312,   203,    14,   254,    78,    79,   201,     3,     3,
       3,   211,   212,   222,   223,   227,   230,   231,   232,   261,
     262,   263,   264,   265,     3,   254,     6,   172,   173,   172,
     173,     3,   175,   164,   187,   188,   189,   254,    64,    57,
     205,     6,   158,   175,   176,   177,   202,   202,   210,    21,
     201,   236,   237,    67,   244,    69,   238,    75,     3,   254,
     254,   254,     3,    64,   201,   224,    76,     3,   254,   254,
     254,     3,     3,     3,   228,   229,    68,   247,     4,   311,
     311,     3,     4,     5,     6,    75,    76,    84,    86,   102,
     103,   104,   108,   133,   134,   135,   155,   179,   181,   183,
     194,   195,   196,   199,   201,   267,   269,   270,   271,   273,
     274,   275,   276,   277,   278,   281,   282,   283,   284,   285,
     287,   288,   289,   290,   291,   293,   294,   295,   296,   297,
     298,   299,   300,   303,   304,   305,   306,   307,   308,     3,
       5,     6,    64,   166,     3,     5,     6,    64,   166,     3,
       5,     6,    64,   166,    42,    46,    47,    51,    52,     3,
       3,     6,     4,   254,   201,   258,   311,     6,     6,   236,
     237,   267,    55,    70,   242,    76,    57,   201,   224,   254,
       3,   221,    34,   234,    64,   191,   205,   247,   270,    80,
      80,   201,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,    75,    76,   271,   201,   201,    89,
     270,   286,     4,     4,     4,     4,     6,   308,   201,   120,
     122,   123,   124,   201,   201,   271,   271,     5,     6,   200,
     291,   301,   302,   234,   270,   202,   205,    57,   153,   154,
      75,    77,   131,   148,   149,   150,   151,   152,   156,   191,
     192,   193,   194,   195,   196,   197,   198,   203,   200,   205,
     200,   205,   200,   205,   200,   205,   200,   205,     3,     6,
      80,   316,   235,    42,    52,     6,   202,   202,    78,   245,
     239,   240,   270,   270,    71,   243,   232,     3,   127,   129,
     213,   214,   215,   220,    57,   201,   320,   202,   205,   201,
     268,   254,   270,   229,   201,   201,    67,   202,   267,   201,
      75,   234,   270,   270,   286,    85,    87,    89,     4,   201,
     201,   201,   201,     4,     4,   206,   202,   202,    79,   269,
       3,   270,   270,    77,   156,   201,    75,   130,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,     3,   196,   291,     6,   301,     6,   302,     6,
       5,    46,    48,    49,   201,   202,     6,    29,    45,   201,
     251,   252,   253,   254,   259,   178,   246,   205,    72,    73,
     241,   270,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   108,
     109,   110,   111,   112,   113,   116,   119,   120,   123,   124,
     125,   216,   128,   201,   202,   205,   232,     3,     3,   267,
     205,    70,    71,    81,    82,    83,   184,   309,   310,   309,
     267,   202,   234,   202,    57,    88,    85,    87,   270,   270,
      78,   270,     4,     3,   289,   270,   202,   205,   202,   205,
       5,     6,   311,   201,   271,   234,   267,   130,   153,   206,
     206,     6,     6,     3,   318,   319,     6,   232,   205,    58,
      60,    61,    62,    63,    65,    66,   260,     3,    57,   255,
     273,   274,   275,   276,   277,   278,   279,   280,   247,   240,
     201,   201,   201,   201,   201,   201,    75,   127,   129,   130,
     217,   218,   316,   201,   221,    28,   317,   214,   202,   202,
     201,     6,     6,     4,     3,     6,   202,   205,   202,   202,
     202,   216,   270,   270,    85,    88,   271,   205,   205,   205,
     205,     4,     4,    80,   234,   267,   202,   202,   271,    53,
      50,   191,   202,   205,     6,   202,   252,    59,    59,     3,
     205,    54,   249,     6,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   121,    91,    92,    93,    94,    95,
      98,    99,   105,   106,   107,   121,    91,    92,    93,    94,
      95,    98,    99,   105,   106,   107,   121,    91,    92,    93,
      94,    95,    98,    99,   105,   106,   107,   121,    91,    92,
      93,    94,    95,    98,    99,   105,   106,   107,   121,   130,
     128,   132,   218,   219,   219,   221,   202,   201,   163,   267,
     310,   202,    85,   270,   202,   199,   303,     4,   291,   199,
     292,   295,   300,   303,   202,   202,   201,   202,   202,     6,
       6,     3,     5,     6,   319,    30,    32,   255,   253,   253,
     201,   279,    55,    56,   248,   202,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,    93,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,    93,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,    93,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,    93,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
      93,   205,   290,   202,   318,     3,   202,     6,   205,   202,
     205,   205,   205,   309,     6,    64,   221,   267,   270,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   205,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     205,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   205,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   205,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   205,     6,   202,   316,     4,     4,     4,
       4,   202,     6,   270,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,     6,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,     6,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,     6,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,     6,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,     6,
     202,   205,   202,   205,   202,   205,    42,    46,    47,    51,
      52,   202,   202,   202,   202,   202,     4,     4,   316,     6,
       6,     6,   202,   205,   205,   272,   202,     6,    48,    49,
     316,     6,     4,   202,   126,   163,   316,    46,    47,     6,
     202,    42,    42,     6,    51,   126,   163,   316,   201,    42,
      42,     3,   201,   202,     3,   316,   202,   316
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   207,   208,   209,   209,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   212,   212,   212,   212,   212,   212,   213,   213,   214,
     214,   215,   215,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   217,   217,   218,   218,   218,   218,
     219,   219,   220,   220,   221,   221,   222,   223,   223,   224,
     224,   225,   226,   226,   226,   226,   226,   226,   226,   226,
     227,   228,   228,   229,   230,   230,   230,   230,   230,   231,
     231,   231,   232,   232,   232,   232,   233,   233,   234,   235,
     236,   236,   237,   238,   238,   239,   239,   240,   241,   241,
     241,   242,   242,   243,   243,   244,   244,   245,   245,   246,
     246,   247,   247,   248,   248,   249,   249,   250,   250,   250,
     250,   251,   251,   252,   252,   253,   253,   254,   254,   255,
     255,   255,   255,   256,   256,   257,   257,   258,   259,   259,
     260,   260,   260,   260,   260,   260,   260,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   262,   262,   262,   263,   263,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   265,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   267,   267,   268,   268,   269,   269,
     270,   270,   270,   270,   270,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   272,   272,
     273,   274,   274,   274,   274,   275,   275,   275,   275,   276,
     276,   277,   277,   278,   278,   279,   279,   279,   279,   279,
     279,   280,   280,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   281,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   281,   281,   281,   281,   282,   282,   283,   284,
     284,   285,   285,   285,   285,   286,   286,   287,   288,   288,
     288,   288,   289,   289,   289,   289,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   291,   291,
     291,   291,   292,   292,   292,   293,   294,   294,   295,   295,
     296,   297,   297,   298,   299,   299,   300,   301,   302,   303,
     303,   304,   305,   305,   306,   307,   307,   308,   308,   308,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   309,
     309,   310,   310,   310,   310,   310,   310,   311,   312,   312,
     313,   313,   314,   314,   315,   315,   316,   316,   317,   317,
     318,   318,   319,   319,   319,   319,   320,   320
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
      12,    19,    15,    14,    12,    17,    13,    12,    10,     7,
       9,     4,     6,     4,     6,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     4,     5,     4,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     6,     3,     4,     3,     3,     5,     5,
       6,     4,     6,     3,     5,     4,     5,     6,     4,     5,
       5,     6,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     3,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     3,     3,     1,
       1,     2,     2,     3,     2,     2,     3,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     1,
       3,     2,     2,     1,     2,     2,     2,     1,     2,     0,
       3,     0,     1,     0,     2,     0,     4,     0,     4,     0,
       1,     3,     1,     3,     3,     3,     6,     3
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
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2302 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2310 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 223 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2324 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 213 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2338 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2349 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2357 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2366 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2375 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 273 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2389 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2400 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2410 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2420 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2430 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2450 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2460 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2488 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2498 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2506 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2514 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2570 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2579 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 293 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2610 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 283 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2624 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 283 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2638 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2648 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2657 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2671 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 243 "parser.y"
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
#line 2688 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2696 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2704 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2712 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2720 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2728 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2736 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2744 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2752 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2760 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2776 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2790 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2798 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2814 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2830 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 354 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2843 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2851 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2859 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2867 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2875 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2883 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2891 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2899 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2907 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2915 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2923 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2931 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2939 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2947 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2955 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2963 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2971 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2979 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2987 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2995 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3003 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3011 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3019 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3027 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3038 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3052 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3077 "parser.cpp"
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

#line 3185 "parser.cpp"

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
#line 494 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3400 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 498 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3411 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 504 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3422 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 511 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3428 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3434 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3440 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3446 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3452 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3458 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3464 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3470 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3476 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 520 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3482 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 521 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3488 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 522 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3494 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3500 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 524 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3506 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 526 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3512 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3518 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 528 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3524 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3530 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3536 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3542 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3548 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3554 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3560 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 535 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3566 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3572 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 537 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3578 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 544 "parser.y"
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
#line 3598 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 561 "parser.y"
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
#line 3616 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 577 "parser.y"
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
#line 3649 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 606 "parser.y"
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
#line 3669 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 622 "parser.y"
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
#line 3690 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 640 "parser.y"
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

    create_index_info->index_info_ = (yyvsp[0].index_info_t);

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3723 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 669 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3732 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 673 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3741 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 679 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3749 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 682 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3757 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 689 "parser.y"
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
#line 3812 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 739 "parser.y"
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
#line 3851 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 775 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3857 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 776 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3863 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 777 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3869 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3875 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 779 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3881 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 780 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3887 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 781 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3893 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 782 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3899 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 783 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3905 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 784 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3911 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 785 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3917 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 786 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3923 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 787 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3929 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 788 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3935 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 789 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3941 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 790 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3947 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3953 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 792 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3959 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3965 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 794 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3971 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 795 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3977 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 798 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3983 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 800 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3989 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 801 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3995 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 802 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4001 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 803 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4007 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 806 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4013 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4019 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4025 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4031 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4037 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4043 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4049 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4055 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4061 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4067 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4073 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4079 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4085 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4091 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4097 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4103 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4109 "parser.cpp"
    break;

  case 86: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4115 "parser.cpp"
    break;

  case 87: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4121 "parser.cpp"
    break;

  case 88: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4127 "parser.cpp"
    break;

  case 89: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4133 "parser.cpp"
    break;

  case 90: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4139 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4145 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4151 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4157 "parser.cpp"
    break;

  case 94: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4163 "parser.cpp"
    break;

  case 95: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4169 "parser.cpp"
    break;

  case 96: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4175 "parser.cpp"
    break;

  case 97: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4181 "parser.cpp"
    break;

  case 98: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4187 "parser.cpp"
    break;

  case 99: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4193 "parser.cpp"
    break;

  case 100: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4199 "parser.cpp"
    break;

  case 101: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4205 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4211 "parser.cpp"
    break;

  case 103: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4217 "parser.cpp"
    break;

  case 104: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4223 "parser.cpp"
    break;

  case 105: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4229 "parser.cpp"
    break;

  case 106: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4235 "parser.cpp"
    break;

  case 107: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4241 "parser.cpp"
    break;

  case 108: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4247 "parser.cpp"
    break;

  case 109: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4253 "parser.cpp"
    break;

  case 110: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4259 "parser.cpp"
    break;

  case 111: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4265 "parser.cpp"
    break;

  case 112: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4271 "parser.cpp"
    break;

  case 113: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4277 "parser.cpp"
    break;

  case 114: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4283 "parser.cpp"
    break;

  case 115: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4289 "parser.cpp"
    break;

  case 116: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4295 "parser.cpp"
    break;

  case 117: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4301 "parser.cpp"
    break;

  case 118: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4307 "parser.cpp"
    break;

  case 119: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4313 "parser.cpp"
    break;

  case 120: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4319 "parser.cpp"
    break;

  case 121: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4325 "parser.cpp"
    break;

  case 122: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4331 "parser.cpp"
    break;

  case 123: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4337 "parser.cpp"
    break;

  case 124: /* column_constraints: column_constraint  */
#line 879 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4346 "parser.cpp"
    break;

  case 125: /* column_constraints: column_constraints column_constraint  */
#line 883 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4360 "parser.cpp"
    break;

  case 126: /* column_constraint: PRIMARY KEY  */
#line 893 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4368 "parser.cpp"
    break;

  case 127: /* column_constraint: UNIQUE  */
#line 896 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4376 "parser.cpp"
    break;

  case 128: /* column_constraint: NULLABLE  */
#line 899 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4384 "parser.cpp"
    break;

  case 129: /* column_constraint: NOT NULLABLE  */
#line 902 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4392 "parser.cpp"
    break;

  case 130: /* default_expr: DEFAULT constant_expr  */
#line 906 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4400 "parser.cpp"
    break;

  case 131: /* default_expr: %empty  */
#line 909 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4408 "parser.cpp"
    break;

  case 132: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 914 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4418 "parser.cpp"
    break;

  case 133: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 919 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4428 "parser.cpp"
    break;

  case 134: /* identifier_array: IDENTIFIER  */
#line 926 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4439 "parser.cpp"
    break;

  case 135: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 932 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4450 "parser.cpp"
    break;

  case 136: /* delete_statement: DELETE FROM table_name where_clause  */
#line 942 "parser.y"
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
#line 4467 "parser.cpp"
    break;

  case 137: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 958 "parser.y"
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
#line 4506 "parser.cpp"
    break;

  case 138: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 992 "parser.y"
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
#line 4523 "parser.cpp"
    break;

  case 139: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1005 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4531 "parser.cpp"
    break;

  case 140: /* optional_identifier_array: %empty  */
#line 1008 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4539 "parser.cpp"
    break;

  case 141: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1015 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4549 "parser.cpp"
    break;

  case 142: /* explain_type: ANALYZE  */
#line 1021 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4557 "parser.cpp"
    break;

  case 143: /* explain_type: AST  */
#line 1024 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4565 "parser.cpp"
    break;

  case 144: /* explain_type: RAW  */
#line 1027 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4573 "parser.cpp"
    break;

  case 145: /* explain_type: LOGICAL  */
#line 1030 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4581 "parser.cpp"
    break;

  case 146: /* explain_type: PHYSICAL  */
#line 1033 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4589 "parser.cpp"
    break;

  case 147: /* explain_type: PIPELINE  */
#line 1036 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4597 "parser.cpp"
    break;

  case 148: /* explain_type: FRAGMENT  */
#line 1039 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4605 "parser.cpp"
    break;

  case 149: /* explain_type: %empty  */
#line 1042 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4613 "parser.cpp"
    break;

  case 150: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1049 "parser.y"
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
#line 4630 "parser.cpp"
    break;

  case 151: /* update_expr_array: update_expr  */
#line 1062 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4639 "parser.cpp"
    break;

  case 152: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1066 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4648 "parser.cpp"
    break;

  case 153: /* update_expr: IDENTIFIER '=' expr  */
#line 1071 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4660 "parser.cpp"
    break;

  case 154: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1084 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4676 "parser.cpp"
    break;

  case 155: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1097 "parser.y"
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
#line 4694 "parser.cpp"
    break;

  case 156: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1112 "parser.y"
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
#line 4712 "parser.cpp"
    break;

  case 157: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1127 "parser.y"
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
#line 4730 "parser.cpp"
    break;

  case 158: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1142 "parser.y"
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
#line 4753 "parser.cpp"
    break;

  case 159: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1165 "parser.y"
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
#line 4811 "parser.cpp"
    break;

  case 160: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1218 "parser.y"
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
#line 4871 "parser.cpp"
    break;

  case 161: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1273 "parser.y"
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
#line 4923 "parser.cpp"
    break;

  case 162: /* select_statement: select_without_paren  */
#line 1324 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4931 "parser.cpp"
    break;

  case 163: /* select_statement: select_with_paren  */
#line 1327 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4939 "parser.cpp"
    break;

  case 164: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
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
#line 4953 "parser.cpp"
    break;

  case 165: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1339 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4967 "parser.cpp"
    break;

  case 166: /* select_with_paren: '(' select_without_paren ')'  */
#line 1349 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4975 "parser.cpp"
    break;

  case 167: /* select_with_paren: '(' select_with_paren ')'  */
#line 1352 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4983 "parser.cpp"
    break;

  case 168: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1356 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4992 "parser.cpp"
    break;

  case 169: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1361 "parser.y"
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
#line 5018 "parser.cpp"
    break;

  case 170: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1383 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5026 "parser.cpp"
    break;

  case 171: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1386 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5034 "parser.cpp"
    break;

  case 172: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1391 "parser.y"
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
#line 5054 "parser.cpp"
    break;

  case 173: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1407 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5062 "parser.cpp"
    break;

  case 174: /* order_by_clause: %empty  */
#line 1410 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5070 "parser.cpp"
    break;

  case 175: /* order_by_expr_list: order_by_expr  */
#line 1414 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5079 "parser.cpp"
    break;

  case 176: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1418 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5088 "parser.cpp"
    break;

  case 177: /* order_by_expr: expr order_by_type  */
#line 1423 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5098 "parser.cpp"
    break;

  case 178: /* order_by_type: ASC  */
#line 1429 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5106 "parser.cpp"
    break;

  case 179: /* order_by_type: DESC  */
#line 1432 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5114 "parser.cpp"
    break;

  case 180: /* order_by_type: %empty  */
#line 1435 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5122 "parser.cpp"
    break;

  case 181: /* limit_expr: LIMIT expr  */
#line 1439 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5130 "parser.cpp"
    break;

  case 182: /* limit_expr: %empty  */
#line 1443 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5136 "parser.cpp"
    break;

  case 183: /* offset_expr: OFFSET expr  */
#line 1445 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5144 "parser.cpp"
    break;

  case 184: /* offset_expr: %empty  */
#line 1449 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5150 "parser.cpp"
    break;

  case 185: /* distinct: DISTINCT  */
#line 1451 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5158 "parser.cpp"
    break;

  case 186: /* distinct: %empty  */
#line 1454 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5166 "parser.cpp"
    break;

  case 187: /* from_clause: FROM table_reference  */
#line 1458 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5174 "parser.cpp"
    break;

  case 188: /* from_clause: %empty  */
#line 1461 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5182 "parser.cpp"
    break;

  case 189: /* search_clause: SEARCH sub_search_array  */
#line 1465 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5192 "parser.cpp"
    break;

  case 190: /* search_clause: %empty  */
#line 1470 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5200 "parser.cpp"
    break;

  case 191: /* where_clause: WHERE expr  */
#line 1474 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5208 "parser.cpp"
    break;

  case 192: /* where_clause: %empty  */
#line 1477 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5216 "parser.cpp"
    break;

  case 193: /* having_clause: HAVING expr  */
#line 1481 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5224 "parser.cpp"
    break;

  case 194: /* having_clause: %empty  */
#line 1484 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5232 "parser.cpp"
    break;

  case 195: /* group_by_clause: GROUP BY expr_array  */
#line 1488 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5240 "parser.cpp"
    break;

  case 196: /* group_by_clause: %empty  */
#line 1491 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5248 "parser.cpp"
    break;

  case 197: /* set_operator: UNION  */
#line 1495 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5256 "parser.cpp"
    break;

  case 198: /* set_operator: UNION ALL  */
#line 1498 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5264 "parser.cpp"
    break;

  case 199: /* set_operator: INTERSECT  */
#line 1501 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5272 "parser.cpp"
    break;

  case 200: /* set_operator: EXCEPT  */
#line 1504 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5280 "parser.cpp"
    break;

  case 201: /* table_reference: table_reference_unit  */
#line 1512 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5288 "parser.cpp"
    break;

  case 202: /* table_reference: table_reference ',' table_reference_unit  */
#line 1515 "parser.y"
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
#line 5306 "parser.cpp"
    break;

  case 205: /* table_reference_name: table_name table_alias  */
#line 1532 "parser.y"
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
#line 5324 "parser.cpp"
    break;

  case 206: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1546 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5335 "parser.cpp"
    break;

  case 207: /* table_name: IDENTIFIER  */
#line 1555 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5345 "parser.cpp"
    break;

  case 208: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1560 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5357 "parser.cpp"
    break;

  case 209: /* table_alias: AS IDENTIFIER  */
#line 1569 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5367 "parser.cpp"
    break;

  case 210: /* table_alias: IDENTIFIER  */
#line 1574 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5377 "parser.cpp"
    break;

  case 211: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1579 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5388 "parser.cpp"
    break;

  case 212: /* table_alias: %empty  */
#line 1585 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5396 "parser.cpp"
    break;

  case 213: /* with_clause: WITH with_expr_list  */
#line 1592 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5404 "parser.cpp"
    break;

  case 214: /* with_clause: %empty  */
#line 1595 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5412 "parser.cpp"
    break;

  case 215: /* with_expr_list: with_expr  */
#line 1599 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5421 "parser.cpp"
    break;

  case 216: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1602 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5430 "parser.cpp"
    break;

  case 217: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1607 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5442 "parser.cpp"
    break;

  case 218: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1619 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5454 "parser.cpp"
    break;

  case 219: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1626 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5467 "parser.cpp"
    break;

  case 220: /* join_type: INNER  */
#line 1640 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5475 "parser.cpp"
    break;

  case 221: /* join_type: LEFT  */
#line 1643 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5483 "parser.cpp"
    break;

  case 222: /* join_type: RIGHT  */
#line 1646 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5491 "parser.cpp"
    break;

  case 223: /* join_type: OUTER  */
#line 1649 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5499 "parser.cpp"
    break;

  case 224: /* join_type: FULL  */
#line 1652 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5507 "parser.cpp"
    break;

  case 225: /* join_type: CROSS  */
#line 1655 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5515 "parser.cpp"
    break;

  case 226: /* join_type: %empty  */
#line 1658 "parser.y"
                {
}
#line 5522 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW DATABASES  */
#line 1664 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5531 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW TABLES  */
#line 1668 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5540 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW VIEWS  */
#line 1672 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5549 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW CONFIGS  */
#line 1676 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5558 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1680 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5570 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW PROFILES  */
#line 1687 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5579 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW BUFFER  */
#line 1691 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5588 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW MEMINDEX  */
#line 1695 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5597 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW QUERIES  */
#line 1699 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5606 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1703 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5616 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW TRANSACTIONS  */
#line 1708 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5625 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1712 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5635 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW SESSION VARIABLES  */
#line 1717 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5644 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1721 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5653 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1725 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5664 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1731 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5675 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1737 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5686 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLE table_name  */
#line 1743 "parser.y"
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
#line 5702 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1754 "parser.y"
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
#line 5718 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1765 "parser.y"
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
#line 5734 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1776 "parser.y"
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
#line 5751 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1788 "parser.y"
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
#line 5768 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1800 "parser.y"
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
#line 5786 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1813 "parser.y"
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
#line 5805 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1827 "parser.y"
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
#line 5821 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1838 "parser.y"
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
#line 5840 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1852 "parser.y"
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
#line 5861 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1868 "parser.y"
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
#line 5883 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW LOGS  */
#line 1885 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5892 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW DELTA LOGS  */
#line 1889 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5901 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW CATALOGS  */
#line 1893 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5910 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1897 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 5919 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1901 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 5928 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1905 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 5939 "parser.cpp"
    break;

  case 261: /* flush_statement: FLUSH DATA  */
#line 1915 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5948 "parser.cpp"
    break;

  case 262: /* flush_statement: FLUSH LOG  */
#line 1919 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5957 "parser.cpp"
    break;

  case 263: /* flush_statement: FLUSH BUFFER  */
#line 1923 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5966 "parser.cpp"
    break;

  case 264: /* optimize_statement: OPTIMIZE table_name  */
#line 1931 "parser.y"
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
#line 5981 "parser.cpp"
    break;

  case 265: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1942 "parser.y"
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
#line 6005 "parser.cpp"
    break;

  case 266: /* command_statement: USE IDENTIFIER  */
#line 1965 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6016 "parser.cpp"
    break;

  case 267: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1971 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6026 "parser.cpp"
    break;

  case 268: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1976 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6037 "parser.cpp"
    break;

  case 269: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1982 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6048 "parser.cpp"
    break;

  case 270: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1988 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6061 "parser.cpp"
    break;

  case 271: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1996 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6072 "parser.cpp"
    break;

  case 272: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2002 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6083 "parser.cpp"
    break;

  case 273: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2008 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6094 "parser.cpp"
    break;

  case 274: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2014 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6105 "parser.cpp"
    break;

  case 275: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2020 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6118 "parser.cpp"
    break;

  case 276: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2028 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6129 "parser.cpp"
    break;

  case 277: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2034 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6140 "parser.cpp"
    break;

  case 278: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2040 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6151 "parser.cpp"
    break;

  case 279: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2046 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6162 "parser.cpp"
    break;

  case 280: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2052 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6175 "parser.cpp"
    break;

  case 281: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2060 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6186 "parser.cpp"
    break;

  case 282: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2066 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6197 "parser.cpp"
    break;

  case 283: /* compact_statement: COMPACT TABLE table_name  */
#line 2073 "parser.y"
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
#line 6214 "parser.cpp"
    break;

  case 284: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2086 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6223 "parser.cpp"
    break;

  case 285: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2090 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6233 "parser.cpp"
    break;

  case 286: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2095 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6244 "parser.cpp"
    break;

  case 287: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2101 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6256 "parser.cpp"
    break;

  case 288: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2108 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6269 "parser.cpp"
    break;

  case 289: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2116 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6283 "parser.cpp"
    break;

  case 290: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2125 "parser.y"
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
#line 6298 "parser.cpp"
    break;

  case 291: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2135 "parser.y"
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
#line 6313 "parser.cpp"
    break;

  case 292: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2145 "parser.y"
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
#line 6329 "parser.cpp"
    break;

  case 293: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2156 "parser.y"
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
#line 6345 "parser.cpp"
    break;

  case 294: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2167 "parser.y"
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
#line 6362 "parser.cpp"
    break;

  case 295: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2179 "parser.y"
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
#line 6379 "parser.cpp"
    break;

  case 296: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2191 "parser.y"
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
#line 6394 "parser.cpp"
    break;

  case 297: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2201 "parser.y"
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
#line 6410 "parser.cpp"
    break;

  case 298: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2212 "parser.y"
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
#line 6427 "parser.cpp"
    break;

  case 299: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2224 "parser.y"
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
#line 6445 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW LOGS  */
#line 2237 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6454 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2241 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6464 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2246 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6474 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2251 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6485 "parser.cpp"
    break;

  case 304: /* expr_array: expr_alias  */
#line 2262 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6494 "parser.cpp"
    break;

  case 305: /* expr_array: expr_array ',' expr_alias  */
#line 2266 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6503 "parser.cpp"
    break;

  case 306: /* expr_array_list: '(' expr_array ')'  */
#line 2271 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6512 "parser.cpp"
    break;

  case 307: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2275 "parser.y"
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
#line 6532 "parser.cpp"
    break;

  case 308: /* expr_alias: expr AS IDENTIFIER  */
#line 2302 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6543 "parser.cpp"
    break;

  case 309: /* expr_alias: expr  */
#line 2308 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6551 "parser.cpp"
    break;

  case 315: /* operand: '(' expr ')'  */
#line 2318 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6559 "parser.cpp"
    break;

  case 316: /* operand: '(' select_without_paren ')'  */
#line 2321 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6570 "parser.cpp"
    break;

  case 317: /* operand: constant_expr  */
#line 2327 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6578 "parser.cpp"
    break;

  case 328: /* extra_match_tensor_option: ',' STRING  */
#line 2341 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6586 "parser.cpp"
    break;

  case 329: /* extra_match_tensor_option: %empty  */
#line 2344 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6594 "parser.cpp"
    break;

  case 330: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2350 "parser.y"
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
#line 6615 "parser.cpp"
    break;

  case 331: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2369 "parser.y"
                                                                                                                                                       {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-15].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-9].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-9].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-11].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-11].str_value), (yyvsp[-13].const_expr_t));
    if (!check) {
        goto Error1;
    }
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    delete (yyvsp[-13].const_expr_t);

    match_vector_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    match_vector_expr->topn_ = (yyvsp[-7].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-13].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 6664 "parser.cpp"
    break;

  case 332: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2414 "parser.y"
                                                                                           {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-11].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-5].str_value));
    if (!check) {
        goto Error2;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-7].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-7].str_value), (yyvsp[-9].const_expr_t));
    if (!check) {
        goto Error2;
    }
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-3].long_value);
    match_vector_expr->ignore_index_ = true;
    goto Return2;
Error2:
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 6706 "parser.cpp"
    break;

  case 333: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2452 "parser.y"
                                                                                                    {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-4].str_value));
    if (!check) {
        goto Error3;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    if (!check) {
        goto Error3;
    }
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return3;
Error3:
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
Return3:
    ;
}
#line 6752 "parser.cpp"
    break;

  case 334: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2494 "parser.y"
                                                                                     {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-8].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-2].str_value));
    if (!check) {
        goto Error4;
    }

    // vector search data type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    if (!check) {
        goto Error4;
    }
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return4;

Error4:
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
Return4:
    ;
}
#line 6799 "parser.cpp"
    break;

  case 335: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2540 "parser.y"
                                                                                                                                                         {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-13].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-11].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-9].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-9].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-7].long_value), (yyvsp[0].with_index_param_list_t));

    match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
}
#line 6824 "parser.cpp"
    break;

  case 336: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2561 "parser.y"
                                                                                              {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-9].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-7].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-5].str_value));

    // topn and options
    match_sparse_expr->topn_ = (yyvsp[-3].long_value);

    match_sparse_expr->ignore_index_ = true;
}
#line 6848 "parser.cpp"
    break;

  case 337: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2581 "parser.y"
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
#line 6870 "parser.cpp"
    break;

  case 338: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2599 "parser.y"
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
#line 6892 "parser.cpp"
    break;

  case 339: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2617 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6905 "parser.cpp"
    break;

  case 340: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2625 "parser.y"
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
#line 6920 "parser.cpp"
    break;

  case 341: /* query_expr: QUERY '(' STRING ')'  */
#line 2636 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6931 "parser.cpp"
    break;

  case 342: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2642 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6944 "parser.cpp"
    break;

  case 343: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2651 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6955 "parser.cpp"
    break;

  case 344: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2657 "parser.y"
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
#line 6971 "parser.cpp"
    break;

  case 345: /* sub_search: match_vector_expr  */
#line 2669 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6979 "parser.cpp"
    break;

  case 346: /* sub_search: match_text_expr  */
#line 2672 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6987 "parser.cpp"
    break;

  case 347: /* sub_search: match_tensor_expr  */
#line 2675 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6995 "parser.cpp"
    break;

  case 348: /* sub_search: match_sparse_expr  */
#line 2678 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7003 "parser.cpp"
    break;

  case 349: /* sub_search: query_expr  */
#line 2681 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7011 "parser.cpp"
    break;

  case 350: /* sub_search: fusion_expr  */
#line 2684 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7019 "parser.cpp"
    break;

  case 351: /* sub_search_array: sub_search  */
#line 2688 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7028 "parser.cpp"
    break;

  case 352: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2692 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7037 "parser.cpp"
    break;

  case 353: /* function_expr: IDENTIFIER '(' ')'  */
#line 2697 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7050 "parser.cpp"
    break;

  case 354: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2705 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7063 "parser.cpp"
    break;

  case 355: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2713 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7077 "parser.cpp"
    break;

  case 356: /* function_expr: operand IS NOT NULLABLE  */
#line 2722 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7089 "parser.cpp"
    break;

  case 357: /* function_expr: operand IS NULLABLE  */
#line 2729 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7101 "parser.cpp"
    break;

  case 358: /* function_expr: NOT operand  */
#line 2736 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7113 "parser.cpp"
    break;

  case 359: /* function_expr: '-' operand  */
#line 2743 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7125 "parser.cpp"
    break;

  case 360: /* function_expr: '+' operand  */
#line 2750 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7137 "parser.cpp"
    break;

  case 361: /* function_expr: operand '-' operand  */
#line 2757 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7150 "parser.cpp"
    break;

  case 362: /* function_expr: operand '+' operand  */
#line 2765 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7163 "parser.cpp"
    break;

  case 363: /* function_expr: operand '*' operand  */
#line 2773 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7176 "parser.cpp"
    break;

  case 364: /* function_expr: operand '/' operand  */
#line 2781 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7189 "parser.cpp"
    break;

  case 365: /* function_expr: operand '%' operand  */
#line 2789 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7202 "parser.cpp"
    break;

  case 366: /* function_expr: operand '=' operand  */
#line 2797 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7215 "parser.cpp"
    break;

  case 367: /* function_expr: operand EQUAL operand  */
#line 2805 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7228 "parser.cpp"
    break;

  case 368: /* function_expr: operand NOT_EQ operand  */
#line 2813 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7241 "parser.cpp"
    break;

  case 369: /* function_expr: operand '<' operand  */
#line 2821 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7254 "parser.cpp"
    break;

  case 370: /* function_expr: operand '>' operand  */
#line 2829 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7267 "parser.cpp"
    break;

  case 371: /* function_expr: operand LESS_EQ operand  */
#line 2837 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7280 "parser.cpp"
    break;

  case 372: /* function_expr: operand GREATER_EQ operand  */
#line 2845 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7293 "parser.cpp"
    break;

  case 373: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2853 "parser.y"
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
#line 7328 "parser.cpp"
    break;

  case 374: /* function_expr: operand LIKE operand  */
#line 2883 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7341 "parser.cpp"
    break;

  case 375: /* function_expr: operand NOT LIKE operand  */
#line 2891 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7354 "parser.cpp"
    break;

  case 376: /* conjunction_expr: expr AND expr  */
#line 2900 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7367 "parser.cpp"
    break;

  case 377: /* conjunction_expr: expr OR expr  */
#line 2908 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7380 "parser.cpp"
    break;

  case 378: /* between_expr: operand BETWEEN operand AND operand  */
#line 2917 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7392 "parser.cpp"
    break;

  case 379: /* in_expr: operand IN '(' expr_array ')'  */
#line 2925 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7403 "parser.cpp"
    break;

  case 380: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2931 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7414 "parser.cpp"
    break;

  case 381: /* case_expr: CASE expr case_check_array END  */
#line 2938 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7425 "parser.cpp"
    break;

  case 382: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2944 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7437 "parser.cpp"
    break;

  case 383: /* case_expr: CASE case_check_array END  */
#line 2951 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7447 "parser.cpp"
    break;

  case 384: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2956 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7458 "parser.cpp"
    break;

  case 385: /* case_check_array: WHEN expr THEN expr  */
#line 2963 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7470 "parser.cpp"
    break;

  case 386: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2970 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7482 "parser.cpp"
    break;

  case 387: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2978 "parser.y"
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
#line 7510 "parser.cpp"
    break;

  case 388: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3002 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7521 "parser.cpp"
    break;

  case 389: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3008 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7532 "parser.cpp"
    break;

  case 390: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3014 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7544 "parser.cpp"
    break;

  case 391: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3021 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7556 "parser.cpp"
    break;

  case 392: /* column_expr: IDENTIFIER  */
#line 3029 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7568 "parser.cpp"
    break;

  case 393: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3036 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7580 "parser.cpp"
    break;

  case 394: /* column_expr: '*'  */
#line 3043 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7590 "parser.cpp"
    break;

  case 395: /* column_expr: column_expr '.' '*'  */
#line 3048 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7604 "parser.cpp"
    break;

  case 396: /* constant_expr: STRING  */
#line 3058 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7614 "parser.cpp"
    break;

  case 397: /* constant_expr: TRUE  */
#line 3063 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7624 "parser.cpp"
    break;

  case 398: /* constant_expr: FALSE  */
#line 3068 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7634 "parser.cpp"
    break;

  case 399: /* constant_expr: DOUBLE_VALUE  */
#line 3073 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7644 "parser.cpp"
    break;

  case 400: /* constant_expr: LONG_VALUE  */
#line 3078 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7654 "parser.cpp"
    break;

  case 401: /* constant_expr: DATE STRING  */
#line 3083 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7664 "parser.cpp"
    break;

  case 402: /* constant_expr: TIME STRING  */
#line 3088 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7674 "parser.cpp"
    break;

  case 403: /* constant_expr: DATETIME STRING  */
#line 3093 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7684 "parser.cpp"
    break;

  case 404: /* constant_expr: TIMESTAMP STRING  */
#line 3098 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7694 "parser.cpp"
    break;

  case 405: /* constant_expr: INTERVAL interval_expr  */
#line 3103 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7702 "parser.cpp"
    break;

  case 406: /* constant_expr: interval_expr  */
#line 3106 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7710 "parser.cpp"
    break;

  case 407: /* constant_expr: common_array_expr  */
#line 3109 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7718 "parser.cpp"
    break;

  case 408: /* common_array_expr: array_expr  */
#line 3113 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7726 "parser.cpp"
    break;

  case 409: /* common_array_expr: subarray_array_expr  */
#line 3116 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7734 "parser.cpp"
    break;

  case 410: /* common_array_expr: sparse_array_expr  */
#line 3119 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7742 "parser.cpp"
    break;

  case 411: /* common_array_expr: empty_array_expr  */
#line 3122 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7750 "parser.cpp"
    break;

  case 412: /* common_sparse_array_expr: sparse_array_expr  */
#line 3126 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7758 "parser.cpp"
    break;

  case 413: /* common_sparse_array_expr: array_expr  */
#line 3129 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7766 "parser.cpp"
    break;

  case 414: /* common_sparse_array_expr: empty_array_expr  */
#line 3132 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7774 "parser.cpp"
    break;

  case 415: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3136 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7782 "parser.cpp"
    break;

  case 416: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3140 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7792 "parser.cpp"
    break;

  case 417: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3145 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7801 "parser.cpp"
    break;

  case 418: /* sparse_array_expr: long_sparse_array_expr  */
#line 3150 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7809 "parser.cpp"
    break;

  case 419: /* sparse_array_expr: double_sparse_array_expr  */
#line 3153 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7817 "parser.cpp"
    break;

  case 420: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3157 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7825 "parser.cpp"
    break;

  case 421: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3161 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7837 "parser.cpp"
    break;

  case 422: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3168 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7848 "parser.cpp"
    break;

  case 423: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3175 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7856 "parser.cpp"
    break;

  case 424: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3179 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7868 "parser.cpp"
    break;

  case 425: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3186 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7879 "parser.cpp"
    break;

  case 426: /* empty_array_expr: '[' ']'  */
#line 3193 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7887 "parser.cpp"
    break;

  case 427: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3197 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7895 "parser.cpp"
    break;

  case 428: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3201 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7903 "parser.cpp"
    break;

  case 429: /* array_expr: long_array_expr  */
#line 3205 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7911 "parser.cpp"
    break;

  case 430: /* array_expr: double_array_expr  */
#line 3208 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7919 "parser.cpp"
    break;

  case 431: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3212 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7927 "parser.cpp"
    break;

  case 432: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3216 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7937 "parser.cpp"
    break;

  case 433: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3221 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7946 "parser.cpp"
    break;

  case 434: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3226 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7954 "parser.cpp"
    break;

  case 435: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3230 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7964 "parser.cpp"
    break;

  case 436: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3235 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7973 "parser.cpp"
    break;

  case 437: /* interval_expr: LONG_VALUE SECONDS  */
#line 3240 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7984 "parser.cpp"
    break;

  case 438: /* interval_expr: LONG_VALUE SECOND  */
#line 3246 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7995 "parser.cpp"
    break;

  case 439: /* interval_expr: LONG_VALUE MINUTES  */
#line 3252 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8006 "parser.cpp"
    break;

  case 440: /* interval_expr: LONG_VALUE MINUTE  */
#line 3258 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8017 "parser.cpp"
    break;

  case 441: /* interval_expr: LONG_VALUE HOURS  */
#line 3264 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8028 "parser.cpp"
    break;

  case 442: /* interval_expr: LONG_VALUE HOUR  */
#line 3270 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8039 "parser.cpp"
    break;

  case 443: /* interval_expr: LONG_VALUE DAYS  */
#line 3276 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8050 "parser.cpp"
    break;

  case 444: /* interval_expr: LONG_VALUE DAY  */
#line 3282 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8061 "parser.cpp"
    break;

  case 445: /* interval_expr: LONG_VALUE MONTHS  */
#line 3288 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8072 "parser.cpp"
    break;

  case 446: /* interval_expr: LONG_VALUE MONTH  */
#line 3294 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8083 "parser.cpp"
    break;

  case 447: /* interval_expr: LONG_VALUE YEARS  */
#line 3300 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8094 "parser.cpp"
    break;

  case 448: /* interval_expr: LONG_VALUE YEAR  */
#line 3306 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8105 "parser.cpp"
    break;

  case 449: /* copy_option_list: copy_option  */
#line 3317 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8114 "parser.cpp"
    break;

  case 450: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3321 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8123 "parser.cpp"
    break;

  case 451: /* copy_option: FORMAT IDENTIFIER  */
#line 3326 "parser.y"
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
#line 8158 "parser.cpp"
    break;

  case 452: /* copy_option: DELIMITER STRING  */
#line 3356 "parser.y"
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
#line 8173 "parser.cpp"
    break;

  case 453: /* copy_option: HEADER  */
#line 3366 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8183 "parser.cpp"
    break;

  case 454: /* copy_option: OFFSET LONG_VALUE  */
#line 3371 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8193 "parser.cpp"
    break;

  case 455: /* copy_option: LIMIT LONG_VALUE  */
#line 3376 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8203 "parser.cpp"
    break;

  case 456: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3381 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8213 "parser.cpp"
    break;

  case 457: /* file_path: STRING  */
#line 3387 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8221 "parser.cpp"
    break;

  case 458: /* if_exists: IF EXISTS  */
#line 3391 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8227 "parser.cpp"
    break;

  case 459: /* if_exists: %empty  */
#line 3392 "parser.y"
  { (yyval.bool_value) = false; }
#line 8233 "parser.cpp"
    break;

  case 460: /* if_not_exists: IF NOT EXISTS  */
#line 3394 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8239 "parser.cpp"
    break;

  case 461: /* if_not_exists: %empty  */
#line 3395 "parser.y"
  { (yyval.bool_value) = false; }
#line 8245 "parser.cpp"
    break;

  case 464: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3410 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8258 "parser.cpp"
    break;

  case 465: /* if_not_exists_info: %empty  */
#line 3418 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8266 "parser.cpp"
    break;

  case 466: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3422 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8274 "parser.cpp"
    break;

  case 467: /* with_index_param_list: %empty  */
#line 3425 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8282 "parser.cpp"
    break;

  case 468: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3429 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8290 "parser.cpp"
    break;

  case 469: /* optional_table_properties_list: %empty  */
#line 3432 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8298 "parser.cpp"
    break;

  case 470: /* index_param_list: index_param  */
#line 3436 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8307 "parser.cpp"
    break;

  case 471: /* index_param_list: index_param_list ',' index_param  */
#line 3440 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8316 "parser.cpp"
    break;

  case 472: /* index_param: IDENTIFIER  */
#line 3445 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8327 "parser.cpp"
    break;

  case 473: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3451 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8342 "parser.cpp"
    break;

  case 474: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3461 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8355 "parser.cpp"
    break;

  case 475: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3469 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8368 "parser.cpp"
    break;

  case 476: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3480 "parser.y"
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
        free((yyvsp[-4].str_value));
        delete (yyvsp[0].with_index_param_list_t);
        yyerror(&yyloc, scanner, result, "Unknown index type");
        YYERROR;
    }
    free((yyvsp[-1].str_value));

    (yyval.index_info_t) = new infinity::IndexInfo();

    (yyval.index_info_t)->index_type_ = index_type;
    (yyval.index_info_t)->column_name_ = (yyvsp[-4].str_value);
    (yyval.index_info_t)->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-4].str_value));
}
#line 8404 "parser.cpp"
    break;

  case 477: /* index_info: '(' IDENTIFIER ')'  */
#line 3511 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8415 "parser.cpp"
    break;


#line 8419 "parser.cpp"

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

#line 3518 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

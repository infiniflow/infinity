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
  YYSYMBOL_LOCK = 24,                      /* LOCK  */
  YYSYMBOL_UNLOCK = 25,                    /* UNLOCK  */
  YYSYMBOL_ADD = 26,                       /* ADD  */
  YYSYMBOL_RENAME = 27,                    /* RENAME  */
  YYSYMBOL_EXCEPT = 28,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 29,                     /* FLUSH  */
  YYSYMBOL_USE = 30,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 31,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 32,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 33,                  /* DATABASE  */
  YYSYMBOL_TABLE = 34,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 35,                /* COLLECTION  */
  YYSYMBOL_TABLES = 36,                    /* TABLES  */
  YYSYMBOL_INTO = 37,                      /* INTO  */
  YYSYMBOL_VALUES = 38,                    /* VALUES  */
  YYSYMBOL_AST = 39,                       /* AST  */
  YYSYMBOL_PIPELINE = 40,                  /* PIPELINE  */
  YYSYMBOL_RAW = 41,                       /* RAW  */
  YYSYMBOL_LOGICAL = 42,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 43,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 44,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 45,                      /* VIEW  */
  YYSYMBOL_INDEX = 46,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 47,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 48,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 49,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 50,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 51,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 52,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 53,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 54,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 55,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 56,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 57,                     /* CHUNK  */
  YYSYMBOL_GROUP = 58,                     /* GROUP  */
  YYSYMBOL_BY = 59,                        /* BY  */
  YYSYMBOL_HAVING = 60,                    /* HAVING  */
  YYSYMBOL_AS = 61,                        /* AS  */
  YYSYMBOL_NATURAL = 62,                   /* NATURAL  */
  YYSYMBOL_JOIN = 63,                      /* JOIN  */
  YYSYMBOL_LEFT = 64,                      /* LEFT  */
  YYSYMBOL_RIGHT = 65,                     /* RIGHT  */
  YYSYMBOL_OUTER = 66,                     /* OUTER  */
  YYSYMBOL_FULL = 67,                      /* FULL  */
  YYSYMBOL_ON = 68,                        /* ON  */
  YYSYMBOL_INNER = 69,                     /* INNER  */
  YYSYMBOL_CROSS = 70,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 71,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 72,                     /* WHERE  */
  YYSYMBOL_ORDER = 73,                     /* ORDER  */
  YYSYMBOL_LIMIT = 74,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 75,                    /* OFFSET  */
  YYSYMBOL_ASC = 76,                       /* ASC  */
  YYSYMBOL_DESC = 77,                      /* DESC  */
  YYSYMBOL_IF = 78,                        /* IF  */
  YYSYMBOL_NOT = 79,                       /* NOT  */
  YYSYMBOL_EXISTS = 80,                    /* EXISTS  */
  YYSYMBOL_IN = 81,                        /* IN  */
  YYSYMBOL_FROM = 82,                      /* FROM  */
  YYSYMBOL_TO = 83,                        /* TO  */
  YYSYMBOL_WITH = 84,                      /* WITH  */
  YYSYMBOL_DELIMITER = 85,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 86,                    /* FORMAT  */
  YYSYMBOL_HEADER = 87,                    /* HEADER  */
  YYSYMBOL_CAST = 88,                      /* CAST  */
  YYSYMBOL_END = 89,                       /* END  */
  YYSYMBOL_CASE = 90,                      /* CASE  */
  YYSYMBOL_ELSE = 91,                      /* ELSE  */
  YYSYMBOL_THEN = 92,                      /* THEN  */
  YYSYMBOL_WHEN = 93,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 94,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 95,                   /* INTEGER  */
  YYSYMBOL_INT = 96,                       /* INT  */
  YYSYMBOL_TINYINT = 97,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 98,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 99,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 100,                  /* HUGEINT  */
  YYSYMBOL_VARCHAR = 101,                  /* VARCHAR  */
  YYSYMBOL_FLOAT = 102,                    /* FLOAT  */
  YYSYMBOL_DOUBLE = 103,                   /* DOUBLE  */
  YYSYMBOL_REAL = 104,                     /* REAL  */
  YYSYMBOL_DECIMAL = 105,                  /* DECIMAL  */
  YYSYMBOL_DATE = 106,                     /* DATE  */
  YYSYMBOL_TIME = 107,                     /* TIME  */
  YYSYMBOL_DATETIME = 108,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 109,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 110,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 111,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 112,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 113,                     /* UUID  */
  YYSYMBOL_POINT = 114,                    /* POINT  */
  YYSYMBOL_LINE = 115,                     /* LINE  */
  YYSYMBOL_LSEG = 116,                     /* LSEG  */
  YYSYMBOL_BOX = 117,                      /* BOX  */
  YYSYMBOL_PATH = 118,                     /* PATH  */
  YYSYMBOL_POLYGON = 119,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 120,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 121,                     /* BLOB  */
  YYSYMBOL_BITMAP = 122,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 123,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 124,                   /* VECTOR  */
  YYSYMBOL_BIT = 125,                      /* BIT  */
  YYSYMBOL_TEXT = 126,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 127,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 128,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 129,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 130,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 131,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 132,                  /* PRIMARY  */
  YYSYMBOL_KEY = 133,                      /* KEY  */
  YYSYMBOL_UNIQUE = 134,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 135,                 /* NULLABLE  */
  YYSYMBOL_IS = 136,                       /* IS  */
  YYSYMBOL_DEFAULT = 137,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 138,                     /* TRUE  */
  YYSYMBOL_FALSE = 139,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 140,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 141,                   /* SECOND  */
  YYSYMBOL_SECONDS = 142,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 143,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 144,                  /* MINUTES  */
  YYSYMBOL_HOUR = 145,                     /* HOUR  */
  YYSYMBOL_HOURS = 146,                    /* HOURS  */
  YYSYMBOL_DAY = 147,                      /* DAY  */
  YYSYMBOL_DAYS = 148,                     /* DAYS  */
  YYSYMBOL_MONTH = 149,                    /* MONTH  */
  YYSYMBOL_MONTHS = 150,                   /* MONTHS  */
  YYSYMBOL_YEAR = 151,                     /* YEAR  */
  YYSYMBOL_YEARS = 152,                    /* YEARS  */
  YYSYMBOL_EQUAL = 153,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 154,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 155,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 156,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 157,                  /* BETWEEN  */
  YYSYMBOL_AND = 158,                      /* AND  */
  YYSYMBOL_OR = 159,                       /* OR  */
  YYSYMBOL_EXTRACT = 160,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 161,                     /* LIKE  */
  YYSYMBOL_DATA = 162,                     /* DATA  */
  YYSYMBOL_LOG = 163,                      /* LOG  */
  YYSYMBOL_BUFFER = 164,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 165,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 166,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 167,                 /* MEMINDEX  */
  YYSYMBOL_USING = 168,                    /* USING  */
  YYSYMBOL_SESSION = 169,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 170,                   /* GLOBAL  */
  YYSYMBOL_OFF = 171,                      /* OFF  */
  YYSYMBOL_EXPORT = 172,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 173,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 174,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 175,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 176,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 177,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 178,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 179,                    /* DELTA  */
  YYSYMBOL_LOGS = 180,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 181,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 182,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 183,                   /* SEARCH  */
  YYSYMBOL_MATCH = 184,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 185,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 186,                    /* QUERY  */
  YYSYMBOL_QUERIES = 187,                  /* QUERIES  */
  YYSYMBOL_FUSION = 188,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 189,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 190,                    /* ADMIN  */
  YYSYMBOL_PERSISTENCE = 191,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 192,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 193,                  /* OBJECTS  */
  YYSYMBOL_FILES = 194,                    /* FILES  */
  YYSYMBOL_MEMORY = 195,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 196,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 197,                   /* NUMBER  */
  YYSYMBOL_198_ = 198,                     /* '='  */
  YYSYMBOL_199_ = 199,                     /* '<'  */
  YYSYMBOL_200_ = 200,                     /* '>'  */
  YYSYMBOL_201_ = 201,                     /* '+'  */
  YYSYMBOL_202_ = 202,                     /* '-'  */
  YYSYMBOL_203_ = 203,                     /* '*'  */
  YYSYMBOL_204_ = 204,                     /* '/'  */
  YYSYMBOL_205_ = 205,                     /* '%'  */
  YYSYMBOL_206_ = 206,                     /* '['  */
  YYSYMBOL_207_ = 207,                     /* ']'  */
  YYSYMBOL_208_ = 208,                     /* '('  */
  YYSYMBOL_209_ = 209,                     /* ')'  */
  YYSYMBOL_210_ = 210,                     /* '.'  */
  YYSYMBOL_211_ = 211,                     /* ';'  */
  YYSYMBOL_212_ = 212,                     /* ','  */
  YYSYMBOL_213_ = 213,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 214,                 /* $accept  */
  YYSYMBOL_input_pattern = 215,            /* input_pattern  */
  YYSYMBOL_statement_list = 216,           /* statement_list  */
  YYSYMBOL_statement = 217,                /* statement  */
  YYSYMBOL_explainable_statement = 218,    /* explainable_statement  */
  YYSYMBOL_create_statement = 219,         /* create_statement  */
  YYSYMBOL_table_element_array = 220,      /* table_element_array  */
  YYSYMBOL_table_element = 221,            /* table_element  */
  YYSYMBOL_table_column = 222,             /* table_column  */
  YYSYMBOL_column_type = 223,              /* column_type  */
  YYSYMBOL_column_constraints = 224,       /* column_constraints  */
  YYSYMBOL_column_constraint = 225,        /* column_constraint  */
  YYSYMBOL_default_expr = 226,             /* default_expr  */
  YYSYMBOL_table_constraint = 227,         /* table_constraint  */
  YYSYMBOL_identifier_array = 228,         /* identifier_array  */
  YYSYMBOL_delete_statement = 229,         /* delete_statement  */
  YYSYMBOL_insert_statement = 230,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 231, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 232,        /* explain_statement  */
  YYSYMBOL_explain_type = 233,             /* explain_type  */
  YYSYMBOL_update_statement = 234,         /* update_statement  */
  YYSYMBOL_update_expr_array = 235,        /* update_expr_array  */
  YYSYMBOL_update_expr = 236,              /* update_expr  */
  YYSYMBOL_drop_statement = 237,           /* drop_statement  */
  YYSYMBOL_copy_statement = 238,           /* copy_statement  */
  YYSYMBOL_select_statement = 239,         /* select_statement  */
  YYSYMBOL_select_with_paren = 240,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 241,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 242, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 243, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 244, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 245,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 246,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 247,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 248,            /* order_by_type  */
  YYSYMBOL_limit_expr = 249,               /* limit_expr  */
  YYSYMBOL_offset_expr = 250,              /* offset_expr  */
  YYSYMBOL_distinct = 251,                 /* distinct  */
  YYSYMBOL_from_clause = 252,              /* from_clause  */
  YYSYMBOL_search_clause = 253,            /* search_clause  */
  YYSYMBOL_where_clause = 254,             /* where_clause  */
  YYSYMBOL_having_clause = 255,            /* having_clause  */
  YYSYMBOL_group_by_clause = 256,          /* group_by_clause  */
  YYSYMBOL_set_operator = 257,             /* set_operator  */
  YYSYMBOL_table_reference = 258,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 259,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 260,     /* table_reference_name  */
  YYSYMBOL_table_name = 261,               /* table_name  */
  YYSYMBOL_table_alias = 262,              /* table_alias  */
  YYSYMBOL_with_clause = 263,              /* with_clause  */
  YYSYMBOL_with_expr_list = 264,           /* with_expr_list  */
  YYSYMBOL_with_expr = 265,                /* with_expr  */
  YYSYMBOL_join_clause = 266,              /* join_clause  */
  YYSYMBOL_join_type = 267,                /* join_type  */
  YYSYMBOL_show_statement = 268,           /* show_statement  */
  YYSYMBOL_flush_statement = 269,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 270,       /* optimize_statement  */
  YYSYMBOL_command_statement = 271,        /* command_statement  */
  YYSYMBOL_compact_statement = 272,        /* compact_statement  */
  YYSYMBOL_admin_statement = 273,          /* admin_statement  */
  YYSYMBOL_alter_statement = 274,          /* alter_statement  */
  YYSYMBOL_expr_array = 275,               /* expr_array  */
  YYSYMBOL_expr_array_list = 276,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 277,               /* expr_alias  */
  YYSYMBOL_expr = 278,                     /* expr  */
  YYSYMBOL_operand = 279,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 280, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 281,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 282,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 283,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 284,          /* match_text_expr  */
  YYSYMBOL_query_expr = 285,               /* query_expr  */
  YYSYMBOL_fusion_expr = 286,              /* fusion_expr  */
  YYSYMBOL_sub_search = 287,               /* sub_search  */
  YYSYMBOL_sub_search_array = 288,         /* sub_search_array  */
  YYSYMBOL_function_expr = 289,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 290,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 291,             /* between_expr  */
  YYSYMBOL_in_expr = 292,                  /* in_expr  */
  YYSYMBOL_case_expr = 293,                /* case_expr  */
  YYSYMBOL_case_check_array = 294,         /* case_check_array  */
  YYSYMBOL_cast_expr = 295,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 296,            /* subquery_expr  */
  YYSYMBOL_column_expr = 297,              /* column_expr  */
  YYSYMBOL_constant_expr = 298,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 299,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 300, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 301,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 302, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 303,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 304,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 305, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 306, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 307, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 308,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 309,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 310,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 311,               /* array_expr  */
  YYSYMBOL_long_array_expr = 312,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 313, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 314,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 315, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 316,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 317,         /* copy_option_list  */
  YYSYMBOL_copy_option = 318,              /* copy_option  */
  YYSYMBOL_file_path = 319,                /* file_path  */
  YYSYMBOL_if_exists = 320,                /* if_exists  */
  YYSYMBOL_if_not_exists = 321,            /* if_not_exists  */
  YYSYMBOL_semicolon = 322,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 323,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 324,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 325, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 326,         /* index_param_list  */
  YYSYMBOL_index_param = 327,              /* index_param  */
  YYSYMBOL_index_info = 328                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 457 "parser.cpp"

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
#define YYFINAL  106
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1317

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  214
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  500
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1105

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   452


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
       2,     2,     2,     2,     2,     2,     2,   205,     2,     2,
     208,   209,   203,   201,   212,   202,   210,   204,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   213,   211,
     199,   198,   200,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   206,     2,   207,     2,     2,     2,     2,     2,     2,
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
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   495,   495,   499,   505,   512,   513,   514,   515,   516,
     517,   518,   519,   520,   521,   522,   523,   524,   525,   526,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     538,   539,   546,   563,   579,   608,   624,   642,   671,   675,
     681,   684,   691,   742,   781,   782,   783,   784,   785,   786,
     787,   788,   789,   790,   791,   792,   793,   794,   795,   796,
     797,   798,   799,   800,   801,   804,   806,   807,   808,   809,
     812,   813,   814,   815,   816,   817,   818,   819,   820,   821,
     822,   823,   824,   825,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   836,   837,   838,   839,   840,   841,
     842,   843,   844,   845,   846,   847,   848,   849,   850,   851,
     852,   853,   854,   855,   856,   857,   858,   859,   860,   861,
     862,   863,   864,   865,   866,   867,   868,   869,   870,   871,
     872,   873,   874,   875,   876,   877,   896,   900,   910,   913,
     916,   919,   923,   926,   931,   936,   943,   949,   959,   975,
    1009,  1022,  1025,  1032,  1038,  1041,  1044,  1047,  1050,  1053,
    1056,  1059,  1066,  1079,  1083,  1088,  1101,  1114,  1129,  1144,
    1159,  1182,  1235,  1290,  1341,  1344,  1347,  1356,  1366,  1369,
    1373,  1378,  1400,  1403,  1408,  1424,  1427,  1431,  1435,  1440,
    1446,  1449,  1452,  1456,  1460,  1462,  1466,  1468,  1471,  1475,
    1478,  1482,  1487,  1491,  1494,  1498,  1501,  1505,  1508,  1512,
    1515,  1518,  1521,  1529,  1532,  1547,  1547,  1549,  1563,  1572,
    1577,  1586,  1591,  1596,  1602,  1609,  1612,  1616,  1619,  1624,
    1636,  1643,  1657,  1660,  1663,  1666,  1669,  1672,  1675,  1681,
    1685,  1689,  1693,  1697,  1704,  1708,  1712,  1716,  1720,  1725,
    1729,  1734,  1738,  1742,  1748,  1754,  1760,  1771,  1782,  1793,
    1805,  1817,  1830,  1844,  1855,  1869,  1885,  1902,  1906,  1910,
    1914,  1918,  1922,  1928,  1932,  1936,  1944,  1948,  1952,  1960,
    1971,  1994,  2000,  2005,  2011,  2017,  2025,  2031,  2037,  2043,
    2049,  2057,  2063,  2069,  2075,  2081,  2089,  2095,  2101,  2110,
    2120,  2133,  2137,  2142,  2148,  2155,  2163,  2172,  2182,  2192,
    2203,  2214,  2226,  2238,  2248,  2259,  2271,  2284,  2288,  2293,
    2298,  2305,  2311,  2316,  2322,  2327,  2340,  2344,  2349,  2353,
    2386,  2392,  2396,  2397,  2398,  2399,  2400,  2402,  2405,  2411,
    2414,  2415,  2416,  2417,  2418,  2419,  2420,  2421,  2422,  2423,
    2425,  2428,  2434,  2453,  2498,  2536,  2578,  2624,  2645,  2665,
    2683,  2701,  2709,  2720,  2726,  2735,  2741,  2753,  2756,  2759,
    2762,  2765,  2768,  2772,  2776,  2781,  2789,  2797,  2806,  2813,
    2820,  2827,  2834,  2841,  2849,  2857,  2865,  2873,  2881,  2889,
    2897,  2905,  2913,  2921,  2929,  2937,  2967,  2975,  2984,  2992,
    3001,  3009,  3015,  3022,  3028,  3035,  3040,  3047,  3054,  3062,
    3089,  3095,  3101,  3108,  3116,  3123,  3130,  3135,  3145,  3150,
    3155,  3160,  3165,  3170,  3175,  3180,  3185,  3190,  3193,  3196,
    3200,  3203,  3206,  3209,  3213,  3216,  3219,  3223,  3227,  3232,
    3237,  3240,  3244,  3248,  3255,  3262,  3266,  3273,  3280,  3284,
    3288,  3292,  3295,  3299,  3303,  3308,  3313,  3317,  3322,  3327,
    3333,  3339,  3345,  3351,  3357,  3363,  3369,  3375,  3381,  3387,
    3393,  3404,  3408,  3413,  3444,  3454,  3459,  3464,  3469,  3475,
    3479,  3480,  3482,  3483,  3485,  3486,  3498,  3506,  3510,  3513,
    3517,  3520,  3524,  3528,  3533,  3539,  3549,  3559,  3567,  3578,
    3609
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
  "PREPARE", "UNION", "ALL", "INTERSECT", "COMPACT", "LOCK", "UNLOCK",
  "ADD", "RENAME", "EXCEPT", "FLUSH", "USE", "OPTIMIZE", "PROPERTIES",
  "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "AST",
  "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "FRAGMENT", "VIEW", "INDEX",
  "ANALYZE", "VIEWS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK",
  "BLOCKS", "COLUMN", "COLUMNS", "INDEXES", "CHUNK", "GROUP", "BY",
  "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL",
  "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET",
  "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH",
  "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE", "ELSE", "THEN",
  "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT",
  "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE",
  "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP",
  "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE",
  "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT", "MULTIVECTOR",
  "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN",
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION",
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
  "compact_statement", "admin_statement", "alter_statement", "expr_array",
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
  "index_info", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-557)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-488)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     738,   388,     2,   435,    40,    -7,    40,   -79,   569,   683,
      48,    85,    99,   106,   235,   144,   209,   230,    78,   259,
     -33,   284,    86,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,   225,  -557,  -557,   285,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,   224,   224,   224,   224,    36,    40,   231,   231,
     231,   231,   231,   126,   302,    40,   -46,   345,   354,   370,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,   147,   372,    40,
    -557,  -557,  -557,  -557,  -557,   373,  -557,    91,   226,  -557,
     406,  -557,   255,  -557,  -557,   246,  -557,   237,    65,    40,
      40,    40,    40,  -557,  -557,  -557,  -557,   -37,  -557,   376,
     253,  -557,   461,   149,   274,   281,  -557,    54,  -557,   425,
    -557,  -557,     7,   464,  -557,   456,   432,   536,    40,    40,
      40,   538,   477,   361,   473,   564,    40,    40,    40,   577,
     599,   601,   543,   615,   615,   493,   129,   139,   143,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,   225,  -557,  -557,  -557,
    -557,  -557,  -557,   294,  -557,  -557,   624,  -557,   638,  -557,
    -557,   637,   654,  -557,  -557,  -557,  -557,   247,  -557,  -557,
    -557,    40,   457,   230,   615,   661,  -557,  -557,   664,  -557,
    -557,  -557,  -557,     7,  -557,  -557,  -557,   493,   612,   598,
     593,  -557,   -14,  -557,   361,  -557,    40,   671,   114,  -557,
    -557,  -557,  -557,  -557,   607,  -557,   478,   -43,  -557,   493,
    -557,  -557,   594,   596,   483,  -557,  -557,   443,   618,   492,
     494,   348,   688,   699,   700,   701,  -557,  -557,   708,   499,
     234,   510,   512,   706,   706,  -557,    15,   458,    82,  -557,
     -31,   634,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,   511,  -557,  -557,  -557,  -150,
    -557,  -557,   -96,  -557,   -92,  -557,  -557,  -557,    70,  -557,
      83,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,   719,   717,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,   673,   674,   675,     5,
     650,   285,  -557,  -557,   183,   727,   526,   527,   -47,   493,
     493,   662,  -557,   -33,   100,   677,   531,  -557,    92,   532,
    -557,    40,   493,   601,  -557,   261,   533,   534,   199,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,   706,   535,   167,   660,   493,   493,   113,   289,  -557,
    -557,  -557,  -557,   443,  -557,   742,   551,   552,   557,   558,
     760,   767,   431,   431,  -557,   559,  -557,  -557,  -557,  -557,
     565,   -85,   690,   493,   772,   493,   493,   -36,   568,   -23,
     706,   706,   706,   706,   706,   706,   706,   706,   706,   706,
     706,   706,   706,   706,    22,  -557,   571,  -557,   773,  -557,
     774,  -557,   775,  -557,   777,   733,   448,   781,   783,   783,
     789,   790,   589,  -557,   590,   792,  -557,    27,  -557,  -557,
       8,   617,   591,  -557,   159,   261,   493,  -557,   225,   874,
     668,   597,   123,  -557,  -557,  -557,   -33,   804,  -557,  -557,
     805,   493,   603,  -557,   261,  -557,    12,    12,   493,  -557,
     152,   660,   648,   602,    -8,   -78,   400,  -557,   493,   493,
     728,   493,   812,    24,   493,   172,   177,   556,  -557,  -557,
     615,  -557,  -557,  -557,   659,   619,   706,   458,   693,  -557,
     323,   323,   191,   191,   762,   323,   323,   191,   191,   431,
     431,  -557,  -557,  -557,  -557,  -557,  -557,   610,  -557,   616,
    -557,  -557,  -557,   819,   824,  -557,  -557,  -557,  -557,   748,
    -557,   837,  -557,  -557,   836,  -557,   -33,   631,   440,  -557,
      49,  -557,   328,   543,   493,  -557,  -557,  -557,   261,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
     643,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,   646,   647,   652,   653,   657,   663,   164,
     665,   671,   835,   100,   225,   666,  -557,   233,   669,   850,
     862,   868,   873,  -557,   880,   273,  -557,   283,   306,  -557,
     670,  -557,   874,   493,  -557,   493,   -41,   108,   706,   101,
     679,  -557,   -57,   122,  -557,   876,  -557,   883,  -557,  -557,
     809,   458,   323,   691,   321,  -557,   706,   893,   896,   845,
     849,   901,   707,   331,  -557,   900,     4,     8,   856,  -557,
    -557,  -557,  -557,  -557,  -557,   858,  -557,   919,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,   712,   855,  -557,   920,
     315,   786,   834,   910,   927,   944,   799,   794,  -557,  -557,
     151,  -557,   788,   671,   343,   730,  -557,  -557,   771,  -557,
     493,  -557,  -557,  -557,  -557,  -557,  -557,    12,  -557,  -557,
    -557,   726,   261,    79,  -557,   493,   484,   734,   937,   571,
     736,   739,   740,   743,   741,   359,  -557,  -557,   167,   941,
     946,  -557,   447,  -557,   837,   307,    49,   440,     8,     8,
     745,   328,   895,   897,   365,   744,   746,   780,   784,   787,
     798,   802,   803,   806,   888,   815,   816,   820,   821,   822,
     832,   833,   838,   839,   844,   898,   846,   847,   848,   851,
     852,   853,   854,   859,   860,   861,   903,   863,   864,   865,
     866,   867,   869,   870,   871,   872,   875,   914,   877,   878,
     879,   881,   882,   884,   885,   886,   887,   889,   934,   890,
     891,   892,   894,   899,   902,   904,   905,   906,   907,   951,
     908,  -557,  -557,   102,  -557,  -557,  -557,   366,  -557,   837,
     952,   367,  -557,  -557,  -557,   261,  -557,   649,   909,   394,
     911,    17,   912,  -557,  -557,  -557,  -557,  -557,    12,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,   987,  -557,
    -557,  -557,   948,   671,  -557,   493,   493,  -557,  -557,  1011,
    1043,  1051,  1055,  1056,  1061,  1062,  1064,  1068,  1074,   913,
    1079,  1080,  1082,  1086,  1089,  1094,  1099,  1101,  1102,  1103,
     915,  1104,  1106,  1107,  1109,  1116,  1120,  1122,  1123,  1124,
    1125,   921,  1126,  1128,  1129,  1130,  1131,  1132,  1133,  1134,
    1135,  1136,   931,  1138,  1139,  1140,  1141,  1142,  1143,  1144,
    1145,  1146,  1147,   942,  1149,  1150,  1151,  1152,  1153,  1154,
    1155,  1156,  1157,  1158,   953,  1160,  -557,  -557,   405,   650,
    -557,  -557,  1163,  -557,  1164,  1165,  1166,   416,  1167,   493,
     417,   959,   261,   963,   965,   966,   967,   968,   969,   970,
     971,   972,   973,  1177,   975,   976,   977,   978,   979,   980,
     981,   982,   983,   984,  1188,   986,   988,   989,   990,   991,
     992,   993,   994,   995,   996,  1190,   997,   998,   999,  1000,
    1001,  1002,  1003,  1004,  1005,  1006,  1210,  1008,  1009,  1010,
    1012,  1013,  1014,  1015,  1016,  1017,  1018,  1214,  1019,  1020,
    1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,  1232,  1030,
    -557,  -557,  1029,  1031,  1032,   438,  -557,   393,   261,  -557,
    -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    1033,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  1034,  -557,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  1036,  -557,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  1037,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  1038,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,  -557,  -557,  1039,  -557,  1245,  -557,  1246,
     650,  1247,  1248,  1249,  -557,  -557,  -557,  -557,  -557,  -557,
    -557,  -557,  -557,   439,  1040,  -557,  1042,  1250,   467,   650,
    1251,  1254,  1050,   -42,   606,  1255,  -557,  -557,  1053,  -557,
    -557,  1217,  1218,  -557,  1259,  -557,  1205,   -29,  -557,  1058,
    -557,  -557,  1221,  1222,  -557,  1266,  -557,  1063,  1065,  1267,
     650,  1066,  -557,   650,  -557
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     226,     0,     0,     0,     0,     0,     0,     0,   161,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,   485,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   175,   174,     0,     8,    14,    15,    16,    17,
      18,    19,   483,   483,   483,   483,   483,     0,   481,   481,
     481,   481,   481,   219,     0,     0,     0,     0,     0,     0,
     155,   159,   156,   157,   158,   160,   154,   226,     0,     0,
     240,   241,   239,   245,   249,     0,   246,     0,     0,   242,
       0,   244,     0,   267,   269,     0,   247,     0,   273,     0,
       0,     0,     0,   276,   277,   278,   281,   219,   279,     0,
     225,   227,     0,     0,     0,     0,     1,   226,     2,   209,
     211,   212,     0,   198,   180,   186,     0,     0,     0,     0,
       0,     0,     0,   152,     0,     0,     0,     0,     0,     0,
       0,     0,   204,     0,     0,     0,     0,     0,     0,   153,
      20,    25,    27,    26,    21,    22,    24,    23,    28,    29,
      30,    31,   255,   256,   250,   251,     0,   252,     0,   243,
     268,     0,     0,   271,   270,   274,   275,     0,   300,   298,
     299,     0,     0,     0,     0,     0,   317,   301,     0,   179,
     178,     4,   210,     0,   176,   177,   197,     0,     0,   194,
       0,    32,     0,    33,   152,   486,     0,     0,   226,   480,
     166,   168,   167,   169,     0,   220,     0,   204,   163,     0,
     148,   479,     0,     0,   414,   418,   421,   422,     0,     0,
       0,     0,     0,     0,     0,     0,   419,   420,     0,     0,
       0,     0,     0,     0,     0,   416,     0,   226,     0,   326,
     331,   332,   346,   344,   347,   345,   348,   349,   341,   336,
     335,   334,   342,   343,   333,   340,   339,   429,   431,     0,
     432,   440,     0,   441,     0,   433,   430,   451,     0,   452,
       0,   428,   285,   287,   286,   283,   284,   290,   292,   291,
     288,   289,   295,   297,   296,   293,   294,     0,     0,   258,
     257,   263,   253,   254,   248,   272,     0,     0,     0,     0,
     489,     0,   228,   282,   318,   302,     0,     0,   200,     0,
       0,   196,   482,   226,     0,     0,     0,   146,     0,     0,
     150,     0,     0,     0,   162,   203,     0,     0,     0,   460,
     459,   462,   461,   464,   463,   466,   465,   468,   467,   470,
     469,     0,     0,   380,   226,     0,     0,     0,     0,   423,
     424,   425,   426,     0,   427,     0,     0,     0,     0,     0,
       0,     0,   382,   381,   457,   454,   448,   438,   443,   446,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   437,     0,   442,     0,   445,
       0,   453,     0,   456,     0,   264,   259,     0,     0,     0,
       0,     0,     0,   280,     0,     0,   319,     0,   183,   182,
       0,   202,   185,   187,   192,   193,     0,   181,    35,     0,
       0,     0,     0,    38,    40,    41,   226,     0,    37,   151,
       0,     0,   149,   170,   165,   164,     0,     0,     0,   375,
       0,   226,     0,     0,     0,     0,     0,   405,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   338,   337,
       0,   327,   330,   398,   399,     0,     0,   226,     0,   379,
     389,   390,   393,   394,     0,   396,   388,   391,   392,   384,
     383,   385,   386,   387,   415,   417,   439,     0,   444,     0,
     447,   455,   458,     0,     0,   260,   323,   324,   322,     0,
     321,     0,   229,   320,     0,   303,   226,   199,   213,   215,
     224,   216,     0,   204,     0,   190,   191,   189,   195,    44,
      47,    48,    45,    46,    49,    50,    66,    51,    53,    52,
      69,    56,    57,    58,    54,    55,    59,    60,    61,    62,
      63,    64,    65,     0,     0,     0,     0,     0,     0,   489,
       0,     0,   491,     0,    36,     0,   147,     0,     0,     0,
       0,     0,     0,   475,     0,     0,   471,     0,     0,   376,
       0,   410,     0,     0,   403,     0,     0,     0,     0,     0,
       0,   414,     0,     0,   363,     0,   365,     0,   450,   449,
       0,   226,   397,     0,     0,   378,     0,     0,     0,   265,
     261,     0,   494,     0,   492,   304,     0,     0,     0,   233,
     234,   235,   236,   232,   237,     0,   222,     0,   217,   369,
     367,   370,   368,   371,   372,   373,   201,   208,   188,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   139,   140,
     143,   136,   143,     0,     0,     0,    34,    39,   500,   328,
       0,   477,   476,   474,   473,   478,   173,     0,   171,   377,
     411,     0,   407,     0,   406,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   412,   401,   400,     0,
       0,   325,     0,   488,     0,     0,   224,   214,     0,     0,
     221,     0,     0,   206,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,   138,     0,   137,    43,    42,     0,   145,     0,
       0,     0,   472,   409,   404,   408,   395,     0,     0,     0,
       0,     0,     0,   434,   436,   435,   364,   366,     0,   413,
     402,   266,   262,   495,   496,   498,   497,   493,     0,   305,
     218,   230,     0,     0,   374,     0,     0,   184,    68,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   142,   144,     0,   489,
     329,   454,     0,   361,     0,     0,     0,     0,   306,     0,
       0,   207,   205,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     490,   499,     0,     0,     0,     0,   172,     0,   231,   223,
      67,    73,    74,    71,    72,    75,    76,    77,    78,    79,
       0,    70,   117,   118,   115,   116,   119,   120,   121,   122,
     123,     0,   114,    84,    85,    82,    83,    86,    87,    88,
      89,    90,     0,    81,    95,    96,    93,    94,    97,    98,
      99,   100,   101,     0,    92,   128,   129,   126,   127,   130,
     131,   132,   133,   134,     0,   125,   106,   107,   104,   105,
     108,   109,   110,   111,   112,     0,   103,     0,   362,     0,
     489,     0,     0,     0,   308,   307,   313,    80,   124,    91,
     102,   135,   113,     0,   351,   360,     0,   314,   309,   489,
       0,     0,     0,   489,     0,     0,   310,   356,     0,   350,
     352,     0,     0,   359,     0,   315,   311,   489,   358,     0,
     316,   312,     0,     0,   355,     0,   354,     0,     0,     0,
     489,     0,   357,   489,   353
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -557,  -557,  -557,  1169,  -557,  1206,  -557,   709,   254,   695,
    -557,   628,   627,  -557,  -548,  1213,  1215,  1087,  -557,  -557,
    1216,  -557,   961,  1219,  1220,   -59,  1265,   -19,  1007,  1105,
     -74,  -557,  -557,   765,  -557,  -557,  -557,  -557,  -557,  -557,
    -197,  -557,  -557,  -557,  -557,   676,   -30,     3,   595,  -557,
    -557,  1117,  -557,  -557,  1225,  1227,  1228,  1229,  1230,  -557,
    -557,  -185,  -557,   925,  -209,  -200,  -557,  -506,  -505,  -482,
    -481,  -478,  -473,   600,  -557,  -557,  -557,  -557,  -557,  -557,
     955,  -557,  -557,   840,   537,  -232,  -557,  -557,  -557,   620,
    -557,  -557,  -557,  -557,   625,   916,   917,   -28,  -557,  -557,
    -557,  -557,  1071,  -442,   639,  -128,   422,   514,  -557,  -557,
    -556,  -557,   525,   613,  -557
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,   139,    24,   432,   433,   434,   559,
     650,   651,   775,   435,   318,    25,    26,   198,    27,    67,
      28,   207,   208,    29,    30,    31,    32,    33,   114,   184,
     115,   189,   422,   423,   527,   311,   427,   187,   421,   523,
     210,   817,   703,   112,   517,   518,   519,   520,   628,    34,
     100,   101,   521,   625,    35,    36,    37,    38,    39,    40,
      41,   238,   442,   239,   240,   241,  1072,   242,   243,   244,
     245,   246,   247,   635,   636,   248,   249,   250,   251,   252,
     348,   253,   254,   255,   256,   257,   792,   258,   259,   260,
     261,   262,   263,   264,   265,   368,   369,   266,   267,   268,
     269,   270,   271,   575,   576,   212,   125,   117,   108,   122,
     413,   656,   613,   614,   438
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     325,   105,   308,   652,   367,   577,   213,    54,   146,    56,
     324,    53,   347,   654,   583,   113,   629,   630,   343,    98,
     364,   365,   364,   365,   109,   494,   110,   591,   371,   209,
     374,   171,   111,   362,   363,   420,   133,   134,   185,    47,
     631,   632,   412,    53,   633,   475,   303,   313,   674,   634,
     123,    17,   626,   582,  -484,   412,   478,   395,   132,   410,
     514,     1,   396,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   153,   375,   376,    55,   515,    11,    12,    13,
     375,   376,    89,    14,    15,    16,   569,   570,   411,  1081,
      57,    58,   167,   168,   169,   170,    59,   571,   572,   573,
     424,   425,  1092,   429,  -487,   777,   215,   216,   217,   307,
     627,   397,   479,   444,   116,   399,   398,   375,   376,    90,
     400,   192,   193,   194,   469,   476,  1082,   375,   376,   201,
     202,   203,   272,    91,   273,   274,   454,   455,    17,  1093,
      92,   343,   277,   450,   278,   279,   282,    96,   283,   284,
     375,   376,   319,   394,     1,   679,     2,     3,     4,     5,
       6,     7,   135,     9,   496,   373,   473,   474,   784,   323,
      11,    12,    13,   130,   300,    20,    14,    15,    16,   320,
     480,   481,   482,   483,   484,   485,   486,   487,   488,   489,
     490,   491,   492,   493,   314,   629,   630,   275,    17,   316,
     675,   574,   214,   215,   216,   217,   346,   280,   222,   223,
     224,   285,    97,   696,   225,   183,   516,   528,   370,   631,
     632,   236,   366,   633,   366,   495,    18,   235,   634,   415,
     646,    17,   430,    99,   431,   525,   526,   375,   376,   416,
     226,   227,   228,   646,    19,   109,   452,   110,   412,   586,
     587,   102,   589,   111,   428,   593,   567,   296,   165,   375,
     376,   166,    20,   578,   297,   900,   375,   376,   155,   156,
     448,   375,   376,   298,   299,   103,   602,   401,   218,   219,
     375,   376,   402,   647,   106,   648,   649,   220,   773,   221,
     403,   372,   604,   113,   373,   404,   647,   107,   648,   649,
     276,   439,   116,   379,   440,   222,   223,   224,   236,   124,
     281,   225,   175,   677,   286,   424,   131,   375,   376,    18,
     380,   381,   382,   383,   443,   453,   637,   379,   385,   176,
     177,   178,   562,   971,   680,   563,   130,   226,   227,   228,
     287,   808,   600,   809,   288,   289,  -488,  -488,   136,   290,
     291,   214,   215,   216,   217,    20,   897,   137,   356,   229,
     357,   579,   358,   359,   373,   386,   387,   388,   389,   390,
     391,   392,   393,   138,   672,   152,   673,   564,   457,   154,
     458,   594,   459,   230,   595,   231,   596,   232,   676,   597,
    -488,  -488,   389,   390,   391,   392,   393,    93,    94,    95,
     233,   234,   235,   157,   158,   236,   688,   237,   449,   159,
     705,   706,   707,   708,   709,   161,   685,   710,   711,   375,
     376,    42,    43,    44,   712,   713,   714,   218,   219,   162,
     163,   164,   580,    45,    46,   160,   220,   172,   221,  1052,
     715,   346,   659,  1053,  1054,   373,   182,   790,  1055,  1056,
     803,   804,   805,   806,   222,   223,   224,   616,   603,   379,
     225,   214,   215,   216,   217,   173,   785,   174,    48,    49,
      50,   126,   127,   128,   129,   781,  -488,  -488,   382,   383,
      51,    52,   666,   179,  -488,   667,   226,   227,   228,   584,
     180,   585,   668,   459,  1065,   667,   214,   215,   216,   217,
     504,   505,   618,  -238,   619,   620,   621,   622,   229,   623,
     624,   190,   230,  1077,   231,   669,   232,  1083,   373,  1075,
    1076,  -488,   387,   388,   389,   390,   391,   392,   393,   188,
     687,  1094,   230,   373,   231,   186,   232,   218,   219,   191,
     693,   195,    17,   694,  1102,   196,   220,  1104,   221,   233,
     234,   235,   778,   199,   236,   440,   237,   118,   119,   120,
     121,   598,   599,   452,   222,   223,   224,   200,   800,   197,
     225,   373,   218,   219,   818,   887,   890,   819,   440,   373,
     204,   220,   684,   221,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   226,   227,   228,   222,
     223,   224,   205,   893,   206,   225,   894,   902,    60,    61,
      62,    63,    64,    65,   970,   209,    66,   694,   229,   211,
     379,   214,   215,   216,   217,   976,   979,   292,   667,   440,
     901,   226,   227,   228,   391,   392,   393,   380,   381,   382,
     383,   293,   230,   294,   231,   385,   232,  1050,  1069,   788,
    1051,  1070,   795,   229,   364,   891,  1084,  1085,   295,   233,
     234,   235,   507,   508,   236,   301,   237,   304,   811,   812,
     305,   309,   310,   312,   317,   321,   322,   230,   326,   231,
     327,   232,   386,   387,   388,   389,   390,   391,   392,   393,
     978,   328,   349,   786,   233,   234,   235,   341,   342,   236,
     344,   237,   345,   350,   351,   352,   220,   355,   221,   214,
     215,   216,   217,   377,   353,   378,    68,    69,   360,    70,
     361,   394,   405,   406,   222,   223,   224,   407,   408,   409,
     225,    71,    72,   417,   412,   418,   419,   426,   436,   437,
     441,   446,   447,   451,    17,     1,   460,     2,     3,     4,
       5,     6,     7,     8,     9,    10,   226,   227,   228,   461,
     462,    11,    12,    13,   465,   463,   464,    14,    15,    16,
     379,   466,   467,   470,   468,   472,   477,   236,   229,   497,
     499,   501,   502,   503,   506,   341,   429,   380,   381,   382,
     383,   384,   509,   510,   220,   385,   221,   511,   513,   512,
     522,   560,   230,   524,   231,   561,   232,   565,   566,   476,
     588,   581,   222,   223,   224,   568,   590,   375,   225,   233,
     234,   235,    17,   607,   236,   609,   237,   601,   605,   608,
     610,   611,   386,   387,   388,   389,   390,   391,   392,   393,
     612,   452,   615,   617,   226,   227,   228,    73,    74,    75,
      76,   639,    77,    78,   640,   641,   661,    79,    80,    81,
     642,   643,    82,    83,    84,   644,   229,   655,   662,    85,
      86,   645,   663,   653,    87,   658,   664,   660,    88,   670,
     681,   716,   717,   718,   719,   720,   665,   682,   721,   722,
     230,   678,   231,   683,   232,   723,   724,   725,   379,   599,
     686,   598,   689,   690,   691,   692,   695,   233,   234,   235,
      18,   726,   236,   702,   237,   380,   381,   382,   383,   698,
     606,   699,   700,   385,   701,   773,   704,   772,    19,   727,
     728,   729,   730,   731,   771,   783,   732,   733,   779,   780,
     787,   789,   791,   734,   735,   736,    20,   801,   796,   797,
     799,   798,   802,   813,   815,   889,   820,   816,   821,   737,
     386,   387,   388,   389,   390,   391,   392,   393,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   829,   546,   547,   548,   549,
     550,   551,   822,   898,   552,   840,   823,   553,   554,   824,
     851,   555,   556,   557,   558,   738,   739,   740,   741,   742,
     825,   862,   743,   744,   826,   827,   899,   903,   828,   745,
     746,   747,   749,   750,   751,   752,   753,   830,   831,   754,
     755,   873,   832,   833,   834,   748,   756,   757,   758,   760,
     761,   762,   763,   764,   835,   836,   765,   766,   884,   904,
     837,   838,   759,   767,   768,   769,   839,   905,   841,   842,
     843,   906,   907,   844,   845,   846,   847,   908,   909,   770,
     910,   848,   849,   850,   911,   852,   853,   854,   855,   856,
     912,   857,   858,   859,   860,   914,   915,   861,   916,   863,
     864,   865,   917,   866,   867,   918,   868,   869,   870,   871,
     919,   872,   874,   875,   876,   920,   877,   921,   922,   923,
     925,   878,   926,   927,   879,   928,   880,   881,   882,   883,
     885,   892,   929,   895,   896,   913,   930,   924,   931,   932,
     933,   934,   936,   935,   937,   938,   939,   940,   941,   942,
     943,   944,   945,   946,   947,   948,   949,   950,   951,   952,
     953,   954,   955,   956,   957,   958,   959,   960,   961,   962,
     963,   964,   965,   966,   967,   968,   969,   972,   973,   974,
     975,   373,   980,   977,   981,   982,   983,   984,   985,   986,
     987,   988,   989,   990,   991,   992,   993,   994,   995,   996,
     997,   998,   999,  1000,  1001,  1002,  1012,  1003,  1004,  1005,
    1006,  1007,  1008,  1009,  1010,  1011,  1013,  1014,  1015,  1016,
    1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,
    1034,  1027,  1028,  1029,  1030,  1031,  1032,  1033,  1035,  1036,
    1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,
    1048,  1047,  1057,  1058,  1049,  1059,  1060,  1061,  1062,  1063,
    1064,  1073,  1071,  1066,  1067,  1068,  1074,  1078,  1079,  1080,
    1091,  1086,  1087,  1088,  1089,  1090,  1095,  1096,  1097,  1098,
    1101,  1099,   657,   140,  1100,  1103,   181,   671,   774,   776,
     141,   315,   142,   143,   445,   104,   144,   145,   306,   638,
     302,   810,   147,   697,   148,   149,   150,   151,   471,   354,
     793,   814,   456,   592,   888,   794,   782,   807,   414,     0,
     886,     0,     0,     0,   498,     0,     0,   500
};

static const yytype_int16 yycheck[] =
{
     209,    20,   187,   559,   236,   447,   134,     4,    67,     6,
     207,     3,   221,   561,    92,     8,   522,   522,   218,    16,
       5,     6,     5,     6,    20,     3,    22,     3,   237,    72,
      61,    68,    28,   233,   234,    82,    82,    83,   112,    37,
     522,   522,    84,     3,   522,    81,   174,    61,    89,   522,
      47,    84,     3,    61,     0,    84,    79,   207,    55,    54,
      33,     7,   212,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    69,   158,   159,    82,    49,    23,    24,    25,
     158,   159,    34,    29,    30,    31,    74,    75,    83,   131,
     169,   170,    89,    90,    91,    92,   175,    85,    86,    87,
     309,   310,   131,     3,    68,   653,     4,     5,     6,   183,
      61,   207,   135,   322,    78,   207,   212,   158,   159,    34,
     212,   118,   119,   120,   209,   161,   168,   158,   159,   126,
     127,   128,     3,    34,     5,     6,   345,   346,    84,   168,
      34,   341,     3,   328,     5,     6,     3,     3,     5,     6,
     158,   159,    38,   210,     7,   212,     9,    10,    11,    12,
      13,    14,   208,    16,   396,   212,   375,   376,    89,   212,
      23,    24,    25,   210,   171,   208,    29,    30,    31,   198,
     380,   381,   382,   383,   384,   385,   386,   387,   388,   389,
     390,   391,   392,   393,   208,   701,   701,    68,    84,   196,
      92,   189,     3,     4,     5,     6,    93,    68,   106,   107,
     108,    68,     3,   209,   112,   208,   208,   426,   237,   701,
     701,   206,   207,   701,   207,   203,   172,   203,   701,    46,
      79,    84,   132,     3,   134,    76,    77,   158,   159,    56,
     138,   139,   140,    79,   190,    20,    79,    22,    84,   458,
     459,   173,   461,    28,   313,   464,   441,    10,   193,   158,
     159,   196,   208,   448,    17,   813,   158,   159,   177,   178,
      71,   158,   159,    26,    27,    16,   476,   207,    79,    80,
     158,   159,   212,   132,     0,   134,   135,    88,   137,    90,
     207,   209,   477,     8,   212,   212,   132,   211,   134,   135,
     171,   209,    78,   136,   212,   106,   107,   108,   206,    78,
     171,   112,   163,   212,   171,   524,    14,   158,   159,   172,
     153,   154,   155,   156,   321,   344,   523,   136,   161,   180,
     181,   182,   209,   889,   212,   212,   210,   138,   139,   140,
      46,    34,   470,    36,    50,    51,   155,   156,     3,    55,
      56,     3,     4,     5,     6,   208,   798,     3,   124,   160,
     126,   209,   128,   129,   212,   198,   199,   200,   201,   202,
     203,   204,   205,     3,   583,     3,   585,   436,    89,     6,
      91,   209,    93,   184,   212,   186,   209,   188,   588,   212,
     199,   200,   201,   202,   203,   204,   205,   162,   163,   164,
     201,   202,   203,   177,   178,   206,   606,   208,   209,     3,
      95,    96,    97,    98,    99,   169,   601,   102,   103,   158,
     159,    33,    34,    35,   109,   110,   111,    79,    80,   192,
     193,   194,   451,    45,    46,   180,    88,    61,    90,    46,
     125,    93,   209,    50,    51,   212,    21,   679,    55,    56,
       3,     4,     5,     6,   106,   107,   108,   516,   477,   136,
     112,     3,     4,     5,     6,   212,   675,     6,    33,    34,
      35,    49,    50,    51,    52,   660,   153,   154,   155,   156,
      45,    46,   209,   209,   161,   212,   138,   139,   140,    89,
     209,    91,   209,    93,  1050,   212,     3,     4,     5,     6,
      52,    53,    62,    63,    64,    65,    66,    67,   160,    69,
      70,    79,   184,  1069,   186,   209,   188,  1073,   212,    52,
      53,   198,   199,   200,   201,   202,   203,   204,   205,    73,
     209,  1087,   184,   212,   186,    71,   188,    79,    80,     3,
     209,     3,    84,   212,  1100,    68,    88,  1103,    90,   201,
     202,   203,   209,    80,   206,   212,   208,    43,    44,    45,
      46,     5,     6,    79,   106,   107,   108,     3,   209,   208,
     112,   212,    79,    80,   209,   209,   209,   212,   212,   212,
       3,    88,   601,    90,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   138,   139,   140,   106,
     107,   108,     3,   209,     3,   112,   212,   816,    39,    40,
      41,    42,    43,    44,   209,    72,    47,   212,   160,     4,
     136,     3,     4,     5,     6,   209,   209,     3,   212,   212,
     815,   138,   139,   140,   203,   204,   205,   153,   154,   155,
     156,     3,   184,     6,   186,   161,   188,   209,   209,   677,
     212,   212,   680,   160,     5,     6,    50,    51,     4,   201,
     202,   203,   408,   409,   206,   208,   208,     6,   698,   699,
       6,    59,    74,    80,     3,    68,   198,   184,    84,   186,
      84,   188,   198,   199,   200,   201,   202,   203,   204,   205,
     899,   208,     4,   209,   201,   202,   203,    79,    80,   206,
     208,   208,   208,     4,     4,     4,    88,   208,    90,     3,
       4,     5,     6,    79,     6,    81,    33,    34,   208,    36,
     208,   210,     3,     6,   106,   107,   108,    54,    54,    54,
     112,    48,    49,     6,    84,   209,   209,    75,    61,   208,
     208,   208,   208,   208,    84,     7,     4,     9,    10,    11,
      12,    13,    14,    15,    16,    17,   138,   139,   140,   208,
     208,    23,    24,    25,     4,   208,   208,    29,    30,    31,
     136,     4,   213,    83,   209,     3,   208,   206,   160,     6,
       6,     6,     5,    50,     3,    79,     3,   153,   154,   155,
     156,   157,     3,     3,    88,   161,    90,   208,     6,   209,
     183,   133,   184,   212,   186,   208,   188,     3,     3,   161,
      82,   209,   106,   107,   108,   212,     4,   158,   112,   201,
     202,   203,    84,   213,   206,     6,   208,   208,   135,   213,
       6,    83,   198,   199,   200,   201,   202,   203,   204,   205,
       3,    79,     6,   212,   138,   139,   140,   164,   165,   166,
     167,   208,   169,   170,   208,   208,     6,   174,   175,   176,
     208,   208,   179,   180,   181,   208,   160,    32,     6,   186,
     187,   208,     4,   208,   191,   209,     3,   208,   195,   209,
       4,    95,    96,    97,    98,    99,     6,     4,   102,   103,
     184,   212,   186,    84,   188,   109,   110,   111,   136,     6,
     209,     5,    57,    54,     3,   198,     6,   201,   202,   203,
     172,   125,   206,    58,   208,   153,   154,   155,   156,    63,
     158,    63,     3,   161,   212,   137,     6,   133,   190,    95,
      96,    97,    98,    99,   135,   209,   102,   103,   208,   168,
     206,     4,   206,   109,   110,   111,   208,     6,   209,   209,
     209,   208,     6,   208,    59,     3,   212,    60,   212,   125,
     198,   199,   200,   201,   202,   203,   204,   205,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,    97,   112,   113,   114,   115,
     116,   117,   212,     6,   120,    97,   212,   123,   124,   212,
      97,   127,   128,   129,   130,    95,    96,    97,    98,    99,
     212,    97,   102,   103,   212,   212,    68,     6,   212,   109,
     110,   111,    95,    96,    97,    98,    99,   212,   212,   102,
     103,    97,   212,   212,   212,   125,   109,   110,   111,    95,
      96,    97,    98,    99,   212,   212,   102,   103,    97,     6,
     212,   212,   125,   109,   110,   111,   212,     6,   212,   212,
     212,     6,     6,   212,   212,   212,   212,     6,     6,   125,
       6,   212,   212,   212,     6,   212,   212,   212,   212,   212,
       6,   212,   212,   212,   212,     6,     6,   212,     6,   212,
     212,   212,     6,   212,   212,     6,   212,   212,   212,   212,
       6,   212,   212,   212,   212,     6,   212,     6,     6,     6,
       6,   212,     6,     6,   212,     6,   212,   212,   212,   212,
     212,   212,     6,   212,   212,   212,     6,   212,     6,     6,
       6,     6,     6,   212,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   212,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   212,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   212,     6,     4,     4,     4,
       4,   212,   209,     6,   209,   209,   209,   209,   209,   209,
     209,   209,   209,     6,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,     6,   209,     6,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,     6,   209,   209,   209,
       6,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,     6,   209,
     209,   212,   209,   209,   212,   209,   209,   209,   209,     4,
       4,   209,   212,     6,     6,     6,     6,     6,     4,   209,
      55,     6,   209,    46,    46,     6,   208,    46,    46,     3,
       3,   208,   563,    67,   209,   209,   107,   582,   650,   652,
      67,   194,    67,    67,   323,    20,    67,    67,   183,   524,
     173,   696,    67,   617,    67,    67,    67,    67,   373,   228,
     680,   701,   347,   463,   779,   680,   667,   694,   301,    -1,
     773,    -1,    -1,    -1,   398,    -1,    -1,   400
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    84,   172,   190,
     208,   215,   216,   217,   219,   229,   230,   232,   234,   237,
     238,   239,   240,   241,   263,   268,   269,   270,   271,   272,
     273,   274,    33,    34,    35,    45,    46,    37,    33,    34,
      35,    45,    46,     3,   261,    82,   261,   169,   170,   175,
      39,    40,    41,    42,    43,    44,    47,   233,    33,    34,
      36,    48,    49,   164,   165,   166,   167,   169,   170,   174,
     175,   176,   179,   180,   181,   186,   187,   191,   195,    34,
      34,    34,    34,   162,   163,   164,     3,     3,   261,     3,
     264,   265,   173,    16,   240,   241,     0,   211,   322,    20,
      22,    28,   257,     8,   242,   244,    78,   321,   321,   321,
     321,   321,   323,   261,    78,   320,   320,   320,   320,   320,
     210,    14,   261,    82,    83,   208,     3,     3,     3,   218,
     219,   229,   230,   234,   237,   238,   239,   268,   269,   270,
     271,   272,     3,   261,     6,   177,   178,   177,   178,     3,
     180,   169,   192,   193,   194,   193,   196,   261,   261,   261,
     261,    68,    61,   212,     6,   163,   180,   181,   182,   209,
     209,   217,    21,   208,   243,   244,    71,   251,    73,   245,
      79,     3,   261,   261,   261,     3,    68,   208,   231,    80,
       3,   261,   261,   261,     3,     3,     3,   235,   236,    72,
     254,     4,   319,   319,     3,     4,     5,     6,    79,    80,
      88,    90,   106,   107,   108,   112,   138,   139,   140,   160,
     184,   186,   188,   201,   202,   203,   206,   208,   275,   277,
     278,   279,   281,   282,   283,   284,   285,   286,   289,   290,
     291,   292,   293,   295,   296,   297,   298,   299,   301,   302,
     303,   304,   305,   306,   307,   308,   311,   312,   313,   314,
     315,   316,     3,     5,     6,    68,   171,     3,     5,     6,
      68,   171,     3,     5,     6,    68,   171,    46,    50,    51,
      55,    56,     3,     3,     6,     4,    10,    17,    26,    27,
     261,   208,   265,   319,     6,     6,   243,   244,   275,    59,
      74,   249,    80,    61,   208,   231,   261,     3,   228,    38,
     241,    68,   198,   212,   254,   278,    84,    84,   208,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,    79,    80,   279,   208,   208,    93,   278,   294,     4,
       4,     4,     4,     6,   316,   208,   124,   126,   128,   129,
     208,   208,   279,   279,     5,     6,   207,   299,   309,   310,
     241,   278,   209,   212,    61,   158,   159,    79,    81,   136,
     153,   154,   155,   156,   157,   161,   198,   199,   200,   201,
     202,   203,   204,   205,   210,   207,   212,   207,   212,   207,
     212,   207,   212,   207,   212,     3,     6,    54,    54,    54,
      54,    83,    84,   324,   242,    46,    56,     6,   209,   209,
      82,   252,   246,   247,   278,   278,    75,   250,   239,     3,
     132,   134,   220,   221,   222,   227,    61,   208,   328,   209,
     212,   208,   276,   261,   278,   236,   208,   208,    71,   209,
     275,   208,    79,   241,   278,   278,   294,    89,    91,    93,
       4,   208,   208,   208,   208,     4,     4,   213,   209,   209,
      83,   277,     3,   278,   278,    81,   161,   208,    79,   135,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,     3,   203,   299,     6,   309,     6,
     310,     6,     5,    50,    52,    53,     3,   222,   222,     3,
       3,   208,   209,     6,    33,    49,   208,   258,   259,   260,
     261,   266,   183,   253,   212,    76,    77,   248,   278,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   112,   113,   114,   115,
     116,   117,   120,   123,   124,   127,   128,   129,   130,   223,
     133,   208,   209,   212,   239,     3,     3,   275,   212,    74,
      75,    85,    86,    87,   189,   317,   318,   317,   275,   209,
     241,   209,    61,    92,    89,    91,   278,   278,    82,   278,
       4,     3,   297,   278,   209,   212,   209,   212,     5,     6,
     319,   208,   279,   241,   275,   135,   158,   213,   213,     6,
       6,    83,     3,   326,   327,     6,   239,   212,    62,    64,
      65,    66,    67,    69,    70,   267,     3,    61,   262,   281,
     282,   283,   284,   285,   286,   287,   288,   254,   247,   208,
     208,   208,   208,   208,   208,   208,    79,   132,   134,   135,
     224,   225,   324,   208,   228,    32,   325,   221,   209,   209,
     208,     6,     6,     4,     3,     6,   209,   212,   209,   209,
     209,   223,   278,   278,    89,    92,   279,   212,   212,   212,
     212,     4,     4,    84,   241,   275,   209,   209,   279,    57,
      54,     3,   198,   209,   212,     6,   209,   259,    63,    63,
       3,   212,    58,   256,     6,    95,    96,    97,    98,    99,
     102,   103,   109,   110,   111,   125,    95,    96,    97,    98,
      99,   102,   103,   109,   110,   111,   125,    95,    96,    97,
      98,    99,   102,   103,   109,   110,   111,   125,    95,    96,
      97,    98,    99,   102,   103,   109,   110,   111,   125,    95,
      96,    97,    98,    99,   102,   103,   109,   110,   111,   125,
      95,    96,    97,    98,    99,   102,   103,   109,   110,   111,
     125,   135,   133,   137,   225,   226,   226,   228,   209,   208,
     168,   275,   318,   209,    89,   278,   209,   206,   311,     4,
     299,   206,   300,   303,   308,   311,   209,   209,   208,   209,
     209,     6,     6,     3,     4,     5,     6,   327,    34,    36,
     262,   260,   260,   208,   287,    59,    60,   255,   209,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,    97,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
      97,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,    97,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,    97,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,    97,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,    97,   212,   298,   209,   326,     3,
     209,     6,   212,   209,   212,   212,   212,   317,     6,    68,
     228,   275,   278,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   212,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   212,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   212,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   212,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   212,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   212,     6,
     209,   324,     4,     4,     4,     4,   209,     6,   278,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
       6,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,     6,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,     6,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,     6,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,     6,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,     6,   209,   212,   209,   212,
     209,   212,    46,    50,    51,    55,    56,   209,   209,   209,
     209,   209,   209,     4,     4,   324,     6,     6,     6,   209,
     212,   212,   280,   209,     6,    52,    53,   324,     6,     4,
     209,   131,   168,   324,    50,    51,     6,   209,    46,    46,
       6,    55,   131,   168,   324,   208,    46,    46,     3,   208,
     209,     3,   324,   209,   324
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   214,   215,   216,   216,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   219,   219,   219,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   224,   224,   225,   225,
     225,   225,   226,   226,   227,   227,   228,   228,   229,   230,
     230,   231,   231,   232,   233,   233,   233,   233,   233,   233,
     233,   233,   234,   235,   235,   236,   237,   237,   237,   237,
     237,   238,   238,   238,   239,   239,   239,   239,   240,   240,
     241,   242,   243,   243,   244,   245,   245,   246,   246,   247,
     248,   248,   248,   249,   249,   250,   250,   251,   251,   252,
     252,   253,   253,   254,   254,   255,   255,   256,   256,   257,
     257,   257,   257,   258,   258,   259,   259,   260,   260,   261,
     261,   262,   262,   262,   262,   263,   263,   264,   264,   265,
     266,   266,   267,   267,   267,   267,   267,   267,   267,   268,
     268,   268,   268,   268,   268,   268,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   269,   269,   269,   270,
     270,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     272,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   274,   274,   274,   274,   274,   275,   275,   276,   276,
     277,   277,   278,   278,   278,   278,   278,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     280,   280,   281,   282,   282,   282,   282,   283,   283,   283,
     283,   284,   284,   285,   285,   286,   286,   287,   287,   287,
     287,   287,   287,   288,   288,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   290,   290,
     291,   292,   292,   293,   293,   293,   293,   294,   294,   295,
     296,   296,   296,   296,   297,   297,   297,   297,   298,   298,
     298,   298,   298,   298,   298,   298,   298,   298,   298,   298,
     299,   299,   299,   299,   300,   300,   300,   301,   302,   302,
     303,   303,   304,   305,   305,   306,   307,   307,   308,   309,
     310,   311,   311,   312,   313,   313,   314,   315,   315,   316,
     316,   316,   316,   316,   316,   316,   316,   316,   316,   316,
     316,   317,   317,   318,   318,   318,   318,   318,   318,   319,
     320,   320,   321,   321,   322,   322,   323,   323,   324,   324,
     325,   325,   326,   326,   327,   327,   327,   327,   327,   328,
     328
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     8,     6,     7,     6,     1,     3,
       1,     1,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     4,     1,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     1,     2,     2,     1,
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
       2,     2,     2,     3,     2,     2,     2,     2,     4,     2,
       3,     3,     3,     4,     4,     3,     3,     4,     4,     5,
       6,     7,     9,     4,     5,     7,     9,     2,     3,     2,
       3,     3,     4,     2,     3,     3,     2,     2,     2,     2,
       5,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     3,     3,
       3,     3,     4,     6,     7,     9,    10,    12,    12,    13,
      14,    15,    16,    12,    13,    15,    16,     3,     4,     5,
       6,     6,     6,     6,     6,     8,     1,     3,     3,     5,
       3,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     0,    12,    19,    15,    14,    12,    17,    13,    12,
      10,     7,     9,     4,     6,     4,     6,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     4,     5,     4,     3,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     6,     3,     4,     3,     3,
       5,     5,     6,     4,     6,     3,     5,     4,     5,     6,
       4,     5,     5,     6,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     3,
       1,     1,     2,     2,     3,     2,     2,     3,     2,     3,
       3,     1,     1,     2,     2,     3,     2,     2,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     3,     2,     2,     1,     2,     2,     2,     1,
       2,     0,     3,     0,     1,     0,     2,     0,     4,     0,
       4,     0,     1,     3,     1,     3,     3,     3,     3,     6,
       3
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
#line 2349 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2357 "parser.cpp"
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
#line 2371 "parser.cpp"
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
#line 2385 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2396 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2404 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2413 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2422 "parser.cpp"
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
#line 2436 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2447 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2457 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2467 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2477 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2487 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2497 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2507 "parser.cpp"
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
#line 2521 "parser.cpp"
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
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2570 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2578 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2586 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2594 "parser.cpp"
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
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2617 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2626 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2635 "parser.cpp"
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
#line 2648 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2657 "parser.cpp"
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
#line 2671 "parser.cpp"
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
#line 2685 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2695 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2704 "parser.cpp"
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
#line 2718 "parser.cpp"
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
#line 2735 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2743 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2751 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2791 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2807 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2815 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2823 "parser.cpp"
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
#line 2837 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2845 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2853 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2861 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2869 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2877 "parser.cpp"
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
#line 2890 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2898 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2906 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2914 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2922 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2930 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2938 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2946 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2954 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2962 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2970 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2978 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2986 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2994 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3002 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3010 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3018 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3026 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3034 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3042 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3050 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3058 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3074 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3085 "parser.cpp"
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
#line 3099 "parser.cpp"
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
#line 3113 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3124 "parser.cpp"
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

#line 3232 "parser.cpp"

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
#line 495 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3447 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 499 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3458 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 505 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3469 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 512 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3475 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3481 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3487 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3493 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3499 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3505 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3511 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3517 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3523 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 521 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3529 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 522 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3535 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3541 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 524 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3547 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 525 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3553 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 526 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3559 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 528 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3565 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3571 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3577 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3583 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3589 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3595 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3601 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3607 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3613 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 537 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3619 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 538 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3625 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3631 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
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
#line 3651 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
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
#line 3669 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
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
#line 3702 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
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
#line 3722 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
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
#line 3743 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
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

    create_index_info->index_info_ = (yyvsp[0].index_info_t);

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3776 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 671 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3785 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 675 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3794 "parser.cpp"
    break;

  case 40: /* table_element: table_column  */
#line 681 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3802 "parser.cpp"
    break;

  case 41: /* table_element: table_constraint  */
#line 684 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3810 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
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
        case infinity::LogicalType::kEmbedding:
        case infinity::LogicalType::kMultiVector:
        case infinity::LogicalType::kTensor:
        case infinity::LogicalType::kTensorArray: {
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
#line 3866 "parser.cpp"
    break;

  case 43: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
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
        case infinity::LogicalType::kEmbedding:
        case infinity::LogicalType::kMultiVector:
        case infinity::LogicalType::kTensor:
        case infinity::LogicalType::kTensorArray: {
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
#line 3908 "parser.cpp"
    break;

  case 44: /* column_type: BOOLEAN  */
#line 781 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3914 "parser.cpp"
    break;

  case 45: /* column_type: TINYINT  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3920 "parser.cpp"
    break;

  case 46: /* column_type: SMALLINT  */
#line 783 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3926 "parser.cpp"
    break;

  case 47: /* column_type: INTEGER  */
#line 784 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3932 "parser.cpp"
    break;

  case 48: /* column_type: INT  */
#line 785 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3938 "parser.cpp"
    break;

  case 49: /* column_type: BIGINT  */
#line 786 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3944 "parser.cpp"
    break;

  case 50: /* column_type: HUGEINT  */
#line 787 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3950 "parser.cpp"
    break;

  case 51: /* column_type: FLOAT  */
#line 788 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3956 "parser.cpp"
    break;

  case 52: /* column_type: REAL  */
#line 789 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3962 "parser.cpp"
    break;

  case 53: /* column_type: DOUBLE  */
#line 790 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3968 "parser.cpp"
    break;

  case 54: /* column_type: FLOAT16  */
#line 791 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3974 "parser.cpp"
    break;

  case 55: /* column_type: BFLOAT16  */
#line 792 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3980 "parser.cpp"
    break;

  case 56: /* column_type: DATE  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3986 "parser.cpp"
    break;

  case 57: /* column_type: TIME  */
#line 794 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3992 "parser.cpp"
    break;

  case 58: /* column_type: DATETIME  */
#line 795 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3998 "parser.cpp"
    break;

  case 59: /* column_type: TIMESTAMP  */
#line 796 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4004 "parser.cpp"
    break;

  case 60: /* column_type: UUID  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4010 "parser.cpp"
    break;

  case 61: /* column_type: POINT  */
#line 798 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4016 "parser.cpp"
    break;

  case 62: /* column_type: LINE  */
#line 799 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4022 "parser.cpp"
    break;

  case 63: /* column_type: LSEG  */
#line 800 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4028 "parser.cpp"
    break;

  case 64: /* column_type: BOX  */
#line 801 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4034 "parser.cpp"
    break;

  case 65: /* column_type: CIRCLE  */
#line 804 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4040 "parser.cpp"
    break;

  case 66: /* column_type: VARCHAR  */
#line 806 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4046 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4052 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 808 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4058 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL  */
#line 809 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4064 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4070 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4076 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4082 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4088 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4094 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4100 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4106 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4112 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4118 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4124 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4130 "parser.cpp"
    break;

  case 81: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4136 "parser.cpp"
    break;

  case 82: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4142 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4148 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4154 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4160 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4166 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4172 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4178 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4184 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4190 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4196 "parser.cpp"
    break;

  case 92: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4202 "parser.cpp"
    break;

  case 93: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4208 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4214 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4220 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4226 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4232 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4238 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4244 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4250 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4256 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4262 "parser.cpp"
    break;

  case 103: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4268 "parser.cpp"
    break;

  case 104: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4274 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4280 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4286 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4292 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4298 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4304 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4310 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4316 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4322 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4328 "parser.cpp"
    break;

  case 114: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4334 "parser.cpp"
    break;

  case 115: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4340 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4346 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4352 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4358 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4364 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4370 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4376 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4382 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4388 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4394 "parser.cpp"
    break;

  case 125: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4400 "parser.cpp"
    break;

  case 126: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4406 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4412 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4418 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4424 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4430 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4436 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4442 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4448 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4454 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4460 "parser.cpp"
    break;

  case 136: /* column_constraints: column_constraint  */
#line 896 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4469 "parser.cpp"
    break;

  case 137: /* column_constraints: column_constraints column_constraint  */
#line 900 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4483 "parser.cpp"
    break;

  case 138: /* column_constraint: PRIMARY KEY  */
#line 910 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4491 "parser.cpp"
    break;

  case 139: /* column_constraint: UNIQUE  */
#line 913 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4499 "parser.cpp"
    break;

  case 140: /* column_constraint: NULLABLE  */
#line 916 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4507 "parser.cpp"
    break;

  case 141: /* column_constraint: NOT NULLABLE  */
#line 919 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4515 "parser.cpp"
    break;

  case 142: /* default_expr: DEFAULT constant_expr  */
#line 923 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4523 "parser.cpp"
    break;

  case 143: /* default_expr: %empty  */
#line 926 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4531 "parser.cpp"
    break;

  case 144: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 931 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4541 "parser.cpp"
    break;

  case 145: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 936 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4551 "parser.cpp"
    break;

  case 146: /* identifier_array: IDENTIFIER  */
#line 943 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4562 "parser.cpp"
    break;

  case 147: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 949 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4573 "parser.cpp"
    break;

  case 148: /* delete_statement: DELETE FROM table_name where_clause  */
#line 959 "parser.y"
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
#line 4590 "parser.cpp"
    break;

  case 149: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 975 "parser.y"
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
#line 4629 "parser.cpp"
    break;

  case 150: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1009 "parser.y"
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
#line 4646 "parser.cpp"
    break;

  case 151: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1022 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4654 "parser.cpp"
    break;

  case 152: /* optional_identifier_array: %empty  */
#line 1025 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4662 "parser.cpp"
    break;

  case 153: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1032 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4672 "parser.cpp"
    break;

  case 154: /* explain_type: ANALYZE  */
#line 1038 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4680 "parser.cpp"
    break;

  case 155: /* explain_type: AST  */
#line 1041 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4688 "parser.cpp"
    break;

  case 156: /* explain_type: RAW  */
#line 1044 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4696 "parser.cpp"
    break;

  case 157: /* explain_type: LOGICAL  */
#line 1047 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4704 "parser.cpp"
    break;

  case 158: /* explain_type: PHYSICAL  */
#line 1050 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4712 "parser.cpp"
    break;

  case 159: /* explain_type: PIPELINE  */
#line 1053 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4720 "parser.cpp"
    break;

  case 160: /* explain_type: FRAGMENT  */
#line 1056 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4728 "parser.cpp"
    break;

  case 161: /* explain_type: %empty  */
#line 1059 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4736 "parser.cpp"
    break;

  case 162: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1066 "parser.y"
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
#line 4753 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr  */
#line 1079 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4762 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1083 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4771 "parser.cpp"
    break;

  case 165: /* update_expr: IDENTIFIER '=' expr  */
#line 1088 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4783 "parser.cpp"
    break;

  case 166: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1101 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4799 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1114 "parser.y"
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
#line 4817 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1129 "parser.y"
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
#line 4835 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1144 "parser.y"
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
#line 4853 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1159 "parser.y"
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
#line 4876 "parser.cpp"
    break;

  case 171: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1182 "parser.y"
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
#line 4934 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1235 "parser.y"
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
#line 4994 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1290 "parser.y"
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
#line 5046 "parser.cpp"
    break;

  case 174: /* select_statement: select_without_paren  */
#line 1341 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5054 "parser.cpp"
    break;

  case 175: /* select_statement: select_with_paren  */
#line 1344 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5062 "parser.cpp"
    break;

  case 176: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1347 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5076 "parser.cpp"
    break;

  case 177: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1356 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5090 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_without_paren ')'  */
#line 1366 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5098 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_with_paren ')'  */
#line 1369 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5106 "parser.cpp"
    break;

  case 180: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1373 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5115 "parser.cpp"
    break;

  case 181: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1378 "parser.y"
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
#line 5141 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1400 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5149 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1403 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5157 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1408 "parser.y"
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
#line 5177 "parser.cpp"
    break;

  case 185: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1424 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5185 "parser.cpp"
    break;

  case 186: /* order_by_clause: %empty  */
#line 1427 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5193 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr  */
#line 1431 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5202 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1435 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5211 "parser.cpp"
    break;

  case 189: /* order_by_expr: expr order_by_type  */
#line 1440 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5221 "parser.cpp"
    break;

  case 190: /* order_by_type: ASC  */
#line 1446 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5229 "parser.cpp"
    break;

  case 191: /* order_by_type: DESC  */
#line 1449 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5237 "parser.cpp"
    break;

  case 192: /* order_by_type: %empty  */
#line 1452 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5245 "parser.cpp"
    break;

  case 193: /* limit_expr: LIMIT expr  */
#line 1456 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5253 "parser.cpp"
    break;

  case 194: /* limit_expr: %empty  */
#line 1460 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5259 "parser.cpp"
    break;

  case 195: /* offset_expr: OFFSET expr  */
#line 1462 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5267 "parser.cpp"
    break;

  case 196: /* offset_expr: %empty  */
#line 1466 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5273 "parser.cpp"
    break;

  case 197: /* distinct: DISTINCT  */
#line 1468 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5281 "parser.cpp"
    break;

  case 198: /* distinct: %empty  */
#line 1471 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5289 "parser.cpp"
    break;

  case 199: /* from_clause: FROM table_reference  */
#line 1475 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5297 "parser.cpp"
    break;

  case 200: /* from_clause: %empty  */
#line 1478 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5305 "parser.cpp"
    break;

  case 201: /* search_clause: SEARCH sub_search_array  */
#line 1482 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5315 "parser.cpp"
    break;

  case 202: /* search_clause: %empty  */
#line 1487 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5323 "parser.cpp"
    break;

  case 203: /* where_clause: WHERE expr  */
#line 1491 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5331 "parser.cpp"
    break;

  case 204: /* where_clause: %empty  */
#line 1494 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5339 "parser.cpp"
    break;

  case 205: /* having_clause: HAVING expr  */
#line 1498 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5347 "parser.cpp"
    break;

  case 206: /* having_clause: %empty  */
#line 1501 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5355 "parser.cpp"
    break;

  case 207: /* group_by_clause: GROUP BY expr_array  */
#line 1505 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5363 "parser.cpp"
    break;

  case 208: /* group_by_clause: %empty  */
#line 1508 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5371 "parser.cpp"
    break;

  case 209: /* set_operator: UNION  */
#line 1512 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5379 "parser.cpp"
    break;

  case 210: /* set_operator: UNION ALL  */
#line 1515 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5387 "parser.cpp"
    break;

  case 211: /* set_operator: INTERSECT  */
#line 1518 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5395 "parser.cpp"
    break;

  case 212: /* set_operator: EXCEPT  */
#line 1521 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5403 "parser.cpp"
    break;

  case 213: /* table_reference: table_reference_unit  */
#line 1529 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5411 "parser.cpp"
    break;

  case 214: /* table_reference: table_reference ',' table_reference_unit  */
#line 1532 "parser.y"
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
#line 5429 "parser.cpp"
    break;

  case 217: /* table_reference_name: table_name table_alias  */
#line 1549 "parser.y"
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
#line 5447 "parser.cpp"
    break;

  case 218: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1563 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5458 "parser.cpp"
    break;

  case 219: /* table_name: IDENTIFIER  */
#line 1572 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5468 "parser.cpp"
    break;

  case 220: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1577 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5480 "parser.cpp"
    break;

  case 221: /* table_alias: AS IDENTIFIER  */
#line 1586 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5490 "parser.cpp"
    break;

  case 222: /* table_alias: IDENTIFIER  */
#line 1591 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5500 "parser.cpp"
    break;

  case 223: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1596 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5511 "parser.cpp"
    break;

  case 224: /* table_alias: %empty  */
#line 1602 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5519 "parser.cpp"
    break;

  case 225: /* with_clause: WITH with_expr_list  */
#line 1609 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5527 "parser.cpp"
    break;

  case 226: /* with_clause: %empty  */
#line 1612 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5535 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr  */
#line 1616 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5544 "parser.cpp"
    break;

  case 228: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1619 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5553 "parser.cpp"
    break;

  case 229: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1624 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5565 "parser.cpp"
    break;

  case 230: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1636 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5577 "parser.cpp"
    break;

  case 231: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1643 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5590 "parser.cpp"
    break;

  case 232: /* join_type: INNER  */
#line 1657 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5598 "parser.cpp"
    break;

  case 233: /* join_type: LEFT  */
#line 1660 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5606 "parser.cpp"
    break;

  case 234: /* join_type: RIGHT  */
#line 1663 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5614 "parser.cpp"
    break;

  case 235: /* join_type: OUTER  */
#line 1666 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5622 "parser.cpp"
    break;

  case 236: /* join_type: FULL  */
#line 1669 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5630 "parser.cpp"
    break;

  case 237: /* join_type: CROSS  */
#line 1672 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5638 "parser.cpp"
    break;

  case 238: /* join_type: %empty  */
#line 1675 "parser.y"
                {
}
#line 5645 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW DATABASES  */
#line 1681 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5654 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW TABLES  */
#line 1685 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5663 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW VIEWS  */
#line 1689 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5672 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW CONFIGS  */
#line 1693 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5681 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1697 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5693 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW PROFILES  */
#line 1704 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5702 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW BUFFER  */
#line 1708 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5711 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW MEMINDEX  */
#line 1712 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5720 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERIES  */
#line 1716 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5729 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1720 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5739 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTIONS  */
#line 1725 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5748 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1729 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5758 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW SESSION VARIABLES  */
#line 1734 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5767 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1738 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5776 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1742 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5787 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1748 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5798 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1754 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5809 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TABLE table_name  */
#line 1760 "parser.y"
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
#line 5825 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1771 "parser.y"
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
#line 5841 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1782 "parser.y"
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
#line 5857 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1793 "parser.y"
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
#line 5874 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1805 "parser.y"
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
#line 5891 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1817 "parser.y"
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
#line 5909 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1830 "parser.y"
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
#line 5928 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1844 "parser.y"
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
#line 5944 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1855 "parser.y"
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
#line 5963 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1869 "parser.y"
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
#line 5984 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1885 "parser.y"
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
#line 6006 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW LOGS  */
#line 1902 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6015 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW DELTA LOGS  */
#line 1906 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6024 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW CATALOGS  */
#line 1910 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6033 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1914 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6042 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1918 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6051 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1922 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6062 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY  */
#line 1928 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6071 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1932 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6080 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1936 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6089 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH DATA  */
#line 1944 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6098 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH LOG  */
#line 1948 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6107 "parser.cpp"
    break;

  case 278: /* flush_statement: FLUSH BUFFER  */
#line 1952 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6116 "parser.cpp"
    break;

  case 279: /* optimize_statement: OPTIMIZE table_name  */
#line 1960 "parser.y"
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
#line 6131 "parser.cpp"
    break;

  case 280: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1971 "parser.y"
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
#line 6155 "parser.cpp"
    break;

  case 281: /* command_statement: USE IDENTIFIER  */
#line 1994 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6166 "parser.cpp"
    break;

  case 282: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 2000 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6176 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2005 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6187 "parser.cpp"
    break;

  case 284: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2011 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6198 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2017 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6211 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2025 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6222 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2031 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6233 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2037 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6244 "parser.cpp"
    break;

  case 289: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2043 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6255 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2049 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6268 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2057 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6279 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2063 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6290 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2069 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6301 "parser.cpp"
    break;

  case 294: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2075 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6312 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2081 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6325 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2089 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6336 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2095 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6347 "parser.cpp"
    break;

  case 298: /* command_statement: LOCK TABLE table_name  */
#line 2101 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6361 "parser.cpp"
    break;

  case 299: /* command_statement: UNLOCK TABLE table_name  */
#line 2110 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6375 "parser.cpp"
    break;

  case 300: /* compact_statement: COMPACT TABLE table_name  */
#line 2120 "parser.y"
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
#line 6392 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2133 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6401 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2137 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6411 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2142 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6422 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2148 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6434 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2155 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6447 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2163 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6461 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2172 "parser.y"
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
#line 6476 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2182 "parser.y"
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
#line 6491 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2192 "parser.y"
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
#line 6507 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2203 "parser.y"
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
#line 6523 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2214 "parser.y"
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
#line 6540 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2226 "parser.y"
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
#line 6557 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2238 "parser.y"
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
#line 6572 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2248 "parser.y"
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
#line 6588 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2259 "parser.y"
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
#line 6605 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2271 "parser.y"
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
#line 6623 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOGS  */
#line 2284 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6632 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2288 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6642 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2293 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6652 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2298 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6663 "parser.cpp"
    break;

  case 321: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2305 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6674 "parser.cpp"
    break;

  case 322: /* alter_statement: ALTER TABLE table_name ADD COLUMN table_column  */
#line 2311 "parser.y"
                                                 {
    auto *ret = new infinity::AddColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_def_ = (yyvsp[0].table_column_t);
}
#line 6684 "parser.cpp"
    break;

  case 323: /* alter_statement: ALTER TABLE table_name DROP COLUMN IDENTIFIER  */
#line 2316 "parser.y"
                                                {
    auto *ret = new infinity::DropColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6695 "parser.cpp"
    break;

  case 324: /* alter_statement: ALTER TABLE table_name ALTER COLUMN table_column  */
#line 2322 "parser.y"
                                                   {
    auto *ret = new infinity::AlterColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_def_ = (yyvsp[0].table_column_t);
}
#line 6705 "parser.cpp"
    break;

  case 325: /* alter_statement: ALTER TABLE table_name RENAME COLUMN IDENTIFIER TO IDENTIFIER  */
#line 2327 "parser.y"
                                                                {
    auto *ret = new infinity::RenameColumnStatement((yyvsp[-5].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    ret->new_column_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6718 "parser.cpp"
    break;

  case 326: /* expr_array: expr_alias  */
#line 2340 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6727 "parser.cpp"
    break;

  case 327: /* expr_array: expr_array ',' expr_alias  */
#line 2344 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6736 "parser.cpp"
    break;

  case 328: /* expr_array_list: '(' expr_array ')'  */
#line 2349 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6745 "parser.cpp"
    break;

  case 329: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2353 "parser.y"
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
        (yyvsp[-4].expr_array_list_t) = nullptr;
        for (auto ptr : *(yyvsp[-1].expr_array_t)) {
            delete ptr;
        }
        delete (yyvsp[-1].expr_array_t);
        (yyvsp[-1].expr_array_t) = nullptr;
        YYERROR;
    }
    (yyvsp[-4].expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
    (yyval.expr_array_list_t) = (yyvsp[-4].expr_array_list_t);
}
#line 6771 "parser.cpp"
    break;

  case 330: /* expr_alias: expr AS IDENTIFIER  */
#line 2386 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6782 "parser.cpp"
    break;

  case 331: /* expr_alias: expr  */
#line 2392 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6790 "parser.cpp"
    break;

  case 337: /* operand: '(' expr ')'  */
#line 2402 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6798 "parser.cpp"
    break;

  case 338: /* operand: '(' select_without_paren ')'  */
#line 2405 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6809 "parser.cpp"
    break;

  case 339: /* operand: constant_expr  */
#line 2411 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6817 "parser.cpp"
    break;

  case 350: /* extra_match_tensor_option: ',' STRING  */
#line 2425 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6825 "parser.cpp"
    break;

  case 351: /* extra_match_tensor_option: %empty  */
#line 2428 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6833 "parser.cpp"
    break;

  case 352: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2434 "parser.y"
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
#line 6854 "parser.cpp"
    break;

  case 353: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2453 "parser.y"
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
#line 6903 "parser.cpp"
    break;

  case 354: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2498 "parser.y"
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
#line 6945 "parser.cpp"
    break;

  case 355: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2536 "parser.y"
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
#line 6991 "parser.cpp"
    break;

  case 356: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2578 "parser.y"
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
#line 7038 "parser.cpp"
    break;

  case 357: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2624 "parser.y"
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
#line 7063 "parser.cpp"
    break;

  case 358: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2645 "parser.y"
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
#line 7087 "parser.cpp"
    break;

  case 359: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2665 "parser.y"
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
#line 7109 "parser.cpp"
    break;

  case 360: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2683 "parser.y"
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
#line 7131 "parser.cpp"
    break;

  case 361: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2701 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7144 "parser.cpp"
    break;

  case 362: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2709 "parser.y"
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
#line 7159 "parser.cpp"
    break;

  case 363: /* query_expr: QUERY '(' STRING ')'  */
#line 2720 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7170 "parser.cpp"
    break;

  case 364: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2726 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7183 "parser.cpp"
    break;

  case 365: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2735 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7194 "parser.cpp"
    break;

  case 366: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2741 "parser.y"
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
#line 7210 "parser.cpp"
    break;

  case 367: /* sub_search: match_vector_expr  */
#line 2753 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7218 "parser.cpp"
    break;

  case 368: /* sub_search: match_text_expr  */
#line 2756 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7226 "parser.cpp"
    break;

  case 369: /* sub_search: match_tensor_expr  */
#line 2759 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7234 "parser.cpp"
    break;

  case 370: /* sub_search: match_sparse_expr  */
#line 2762 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7242 "parser.cpp"
    break;

  case 371: /* sub_search: query_expr  */
#line 2765 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7250 "parser.cpp"
    break;

  case 372: /* sub_search: fusion_expr  */
#line 2768 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7258 "parser.cpp"
    break;

  case 373: /* sub_search_array: sub_search  */
#line 2772 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7267 "parser.cpp"
    break;

  case 374: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2776 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7276 "parser.cpp"
    break;

  case 375: /* function_expr: IDENTIFIER '(' ')'  */
#line 2781 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7289 "parser.cpp"
    break;

  case 376: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2789 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7302 "parser.cpp"
    break;

  case 377: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2797 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7316 "parser.cpp"
    break;

  case 378: /* function_expr: operand IS NOT NULLABLE  */
#line 2806 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7328 "parser.cpp"
    break;

  case 379: /* function_expr: operand IS NULLABLE  */
#line 2813 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7340 "parser.cpp"
    break;

  case 380: /* function_expr: NOT operand  */
#line 2820 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7352 "parser.cpp"
    break;

  case 381: /* function_expr: '-' operand  */
#line 2827 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7364 "parser.cpp"
    break;

  case 382: /* function_expr: '+' operand  */
#line 2834 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7376 "parser.cpp"
    break;

  case 383: /* function_expr: operand '-' operand  */
#line 2841 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7389 "parser.cpp"
    break;

  case 384: /* function_expr: operand '+' operand  */
#line 2849 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7402 "parser.cpp"
    break;

  case 385: /* function_expr: operand '*' operand  */
#line 2857 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7415 "parser.cpp"
    break;

  case 386: /* function_expr: operand '/' operand  */
#line 2865 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7428 "parser.cpp"
    break;

  case 387: /* function_expr: operand '%' operand  */
#line 2873 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7441 "parser.cpp"
    break;

  case 388: /* function_expr: operand '=' operand  */
#line 2881 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7454 "parser.cpp"
    break;

  case 389: /* function_expr: operand EQUAL operand  */
#line 2889 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7467 "parser.cpp"
    break;

  case 390: /* function_expr: operand NOT_EQ operand  */
#line 2897 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7480 "parser.cpp"
    break;

  case 391: /* function_expr: operand '<' operand  */
#line 2905 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7493 "parser.cpp"
    break;

  case 392: /* function_expr: operand '>' operand  */
#line 2913 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7506 "parser.cpp"
    break;

  case 393: /* function_expr: operand LESS_EQ operand  */
#line 2921 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7519 "parser.cpp"
    break;

  case 394: /* function_expr: operand GREATER_EQ operand  */
#line 2929 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7532 "parser.cpp"
    break;

  case 395: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2937 "parser.y"
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
#line 7567 "parser.cpp"
    break;

  case 396: /* function_expr: operand LIKE operand  */
#line 2967 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7580 "parser.cpp"
    break;

  case 397: /* function_expr: operand NOT LIKE operand  */
#line 2975 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7593 "parser.cpp"
    break;

  case 398: /* conjunction_expr: expr AND expr  */
#line 2984 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7606 "parser.cpp"
    break;

  case 399: /* conjunction_expr: expr OR expr  */
#line 2992 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7619 "parser.cpp"
    break;

  case 400: /* between_expr: operand BETWEEN operand AND operand  */
#line 3001 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7631 "parser.cpp"
    break;

  case 401: /* in_expr: operand IN '(' expr_array ')'  */
#line 3009 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7642 "parser.cpp"
    break;

  case 402: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3015 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7653 "parser.cpp"
    break;

  case 403: /* case_expr: CASE expr case_check_array END  */
#line 3022 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7664 "parser.cpp"
    break;

  case 404: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3028 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7676 "parser.cpp"
    break;

  case 405: /* case_expr: CASE case_check_array END  */
#line 3035 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7686 "parser.cpp"
    break;

  case 406: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3040 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7697 "parser.cpp"
    break;

  case 407: /* case_check_array: WHEN expr THEN expr  */
#line 3047 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7709 "parser.cpp"
    break;

  case 408: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3054 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7721 "parser.cpp"
    break;

  case 409: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3062 "parser.y"
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
        case infinity::LogicalType::kEmbedding:
        case infinity::LogicalType::kMultiVector:
        case infinity::LogicalType::kTensor:
        case infinity::LogicalType::kTensorArray: {
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
#line 7752 "parser.cpp"
    break;

  case 410: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3089 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7763 "parser.cpp"
    break;

  case 411: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3095 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7774 "parser.cpp"
    break;

  case 412: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3101 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7786 "parser.cpp"
    break;

  case 413: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3108 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7798 "parser.cpp"
    break;

  case 414: /* column_expr: IDENTIFIER  */
#line 3116 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7810 "parser.cpp"
    break;

  case 415: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3123 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7822 "parser.cpp"
    break;

  case 416: /* column_expr: '*'  */
#line 3130 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7832 "parser.cpp"
    break;

  case 417: /* column_expr: column_expr '.' '*'  */
#line 3135 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7846 "parser.cpp"
    break;

  case 418: /* constant_expr: STRING  */
#line 3145 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7856 "parser.cpp"
    break;

  case 419: /* constant_expr: TRUE  */
#line 3150 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7866 "parser.cpp"
    break;

  case 420: /* constant_expr: FALSE  */
#line 3155 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7876 "parser.cpp"
    break;

  case 421: /* constant_expr: DOUBLE_VALUE  */
#line 3160 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7886 "parser.cpp"
    break;

  case 422: /* constant_expr: LONG_VALUE  */
#line 3165 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7896 "parser.cpp"
    break;

  case 423: /* constant_expr: DATE STRING  */
#line 3170 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7906 "parser.cpp"
    break;

  case 424: /* constant_expr: TIME STRING  */
#line 3175 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7916 "parser.cpp"
    break;

  case 425: /* constant_expr: DATETIME STRING  */
#line 3180 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7926 "parser.cpp"
    break;

  case 426: /* constant_expr: TIMESTAMP STRING  */
#line 3185 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7936 "parser.cpp"
    break;

  case 427: /* constant_expr: INTERVAL interval_expr  */
#line 3190 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7944 "parser.cpp"
    break;

  case 428: /* constant_expr: interval_expr  */
#line 3193 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7952 "parser.cpp"
    break;

  case 429: /* constant_expr: common_array_expr  */
#line 3196 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7960 "parser.cpp"
    break;

  case 430: /* common_array_expr: array_expr  */
#line 3200 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7968 "parser.cpp"
    break;

  case 431: /* common_array_expr: subarray_array_expr  */
#line 3203 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7976 "parser.cpp"
    break;

  case 432: /* common_array_expr: sparse_array_expr  */
#line 3206 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7984 "parser.cpp"
    break;

  case 433: /* common_array_expr: empty_array_expr  */
#line 3209 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7992 "parser.cpp"
    break;

  case 434: /* common_sparse_array_expr: sparse_array_expr  */
#line 3213 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8000 "parser.cpp"
    break;

  case 435: /* common_sparse_array_expr: array_expr  */
#line 3216 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8008 "parser.cpp"
    break;

  case 436: /* common_sparse_array_expr: empty_array_expr  */
#line 3219 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8016 "parser.cpp"
    break;

  case 437: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3223 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8024 "parser.cpp"
    break;

  case 438: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3227 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8034 "parser.cpp"
    break;

  case 439: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3232 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8043 "parser.cpp"
    break;

  case 440: /* sparse_array_expr: long_sparse_array_expr  */
#line 3237 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8051 "parser.cpp"
    break;

  case 441: /* sparse_array_expr: double_sparse_array_expr  */
#line 3240 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8059 "parser.cpp"
    break;

  case 442: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3244 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8067 "parser.cpp"
    break;

  case 443: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3248 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8079 "parser.cpp"
    break;

  case 444: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3255 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8090 "parser.cpp"
    break;

  case 445: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3262 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8098 "parser.cpp"
    break;

  case 446: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3266 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8110 "parser.cpp"
    break;

  case 447: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3273 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8121 "parser.cpp"
    break;

  case 448: /* empty_array_expr: '[' ']'  */
#line 3280 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8129 "parser.cpp"
    break;

  case 449: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3284 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8137 "parser.cpp"
    break;

  case 450: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3288 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8145 "parser.cpp"
    break;

  case 451: /* array_expr: long_array_expr  */
#line 3292 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8153 "parser.cpp"
    break;

  case 452: /* array_expr: double_array_expr  */
#line 3295 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8161 "parser.cpp"
    break;

  case 453: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3299 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8169 "parser.cpp"
    break;

  case 454: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3303 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8179 "parser.cpp"
    break;

  case 455: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3308 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8188 "parser.cpp"
    break;

  case 456: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3313 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8196 "parser.cpp"
    break;

  case 457: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3317 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8206 "parser.cpp"
    break;

  case 458: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3322 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8215 "parser.cpp"
    break;

  case 459: /* interval_expr: LONG_VALUE SECONDS  */
#line 3327 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8226 "parser.cpp"
    break;

  case 460: /* interval_expr: LONG_VALUE SECOND  */
#line 3333 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8237 "parser.cpp"
    break;

  case 461: /* interval_expr: LONG_VALUE MINUTES  */
#line 3339 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8248 "parser.cpp"
    break;

  case 462: /* interval_expr: LONG_VALUE MINUTE  */
#line 3345 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8259 "parser.cpp"
    break;

  case 463: /* interval_expr: LONG_VALUE HOURS  */
#line 3351 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8270 "parser.cpp"
    break;

  case 464: /* interval_expr: LONG_VALUE HOUR  */
#line 3357 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8281 "parser.cpp"
    break;

  case 465: /* interval_expr: LONG_VALUE DAYS  */
#line 3363 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8292 "parser.cpp"
    break;

  case 466: /* interval_expr: LONG_VALUE DAY  */
#line 3369 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8303 "parser.cpp"
    break;

  case 467: /* interval_expr: LONG_VALUE MONTHS  */
#line 3375 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8314 "parser.cpp"
    break;

  case 468: /* interval_expr: LONG_VALUE MONTH  */
#line 3381 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8325 "parser.cpp"
    break;

  case 469: /* interval_expr: LONG_VALUE YEARS  */
#line 3387 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8336 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE YEAR  */
#line 3393 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8347 "parser.cpp"
    break;

  case 471: /* copy_option_list: copy_option  */
#line 3404 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8356 "parser.cpp"
    break;

  case 472: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3408 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8365 "parser.cpp"
    break;

  case 473: /* copy_option: FORMAT IDENTIFIER  */
#line 3413 "parser.y"
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
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 8401 "parser.cpp"
    break;

  case 474: /* copy_option: DELIMITER STRING  */
#line 3444 "parser.y"
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
#line 8416 "parser.cpp"
    break;

  case 475: /* copy_option: HEADER  */
#line 3454 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8426 "parser.cpp"
    break;

  case 476: /* copy_option: OFFSET LONG_VALUE  */
#line 3459 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8436 "parser.cpp"
    break;

  case 477: /* copy_option: LIMIT LONG_VALUE  */
#line 3464 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8446 "parser.cpp"
    break;

  case 478: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3469 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8456 "parser.cpp"
    break;

  case 479: /* file_path: STRING  */
#line 3475 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8464 "parser.cpp"
    break;

  case 480: /* if_exists: IF EXISTS  */
#line 3479 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8470 "parser.cpp"
    break;

  case 481: /* if_exists: %empty  */
#line 3480 "parser.y"
  { (yyval.bool_value) = false; }
#line 8476 "parser.cpp"
    break;

  case 482: /* if_not_exists: IF NOT EXISTS  */
#line 3482 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8482 "parser.cpp"
    break;

  case 483: /* if_not_exists: %empty  */
#line 3483 "parser.y"
  { (yyval.bool_value) = false; }
#line 8488 "parser.cpp"
    break;

  case 486: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3498 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8501 "parser.cpp"
    break;

  case 487: /* if_not_exists_info: %empty  */
#line 3506 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8509 "parser.cpp"
    break;

  case 488: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3510 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8517 "parser.cpp"
    break;

  case 489: /* with_index_param_list: %empty  */
#line 3513 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8525 "parser.cpp"
    break;

  case 490: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3517 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8533 "parser.cpp"
    break;

  case 491: /* optional_table_properties_list: %empty  */
#line 3520 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8541 "parser.cpp"
    break;

  case 492: /* index_param_list: index_param  */
#line 3524 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8550 "parser.cpp"
    break;

  case 493: /* index_param_list: index_param_list ',' index_param  */
#line 3528 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8559 "parser.cpp"
    break;

  case 494: /* index_param: IDENTIFIER  */
#line 3533 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8570 "parser.cpp"
    break;

  case 495: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3539 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8585 "parser.cpp"
    break;

  case 496: /* index_param: IDENTIFIER '=' STRING  */
#line 3549 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8600 "parser.cpp"
    break;

  case 497: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3559 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8613 "parser.cpp"
    break;

  case 498: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3567 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8626 "parser.cpp"
    break;

  case 499: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3578 "parser.y"
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
#line 8662 "parser.cpp"
    break;

  case 500: /* index_info: '(' IDENTIFIER ')'  */
#line 3609 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8673 "parser.cpp"
    break;


#line 8677 "parser.cpp"

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

#line 3616 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

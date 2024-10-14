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
  YYSYMBOL_VIEW = 39,                      /* VIEW  */
  YYSYMBOL_INDEX = 40,                     /* INDEX  */
  YYSYMBOL_VIEWS = 41,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 42,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 43,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 44,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 45,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 46,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 47,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 48,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 49,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 50,                     /* CHUNK  */
  YYSYMBOL_GROUP = 51,                     /* GROUP  */
  YYSYMBOL_BY = 52,                        /* BY  */
  YYSYMBOL_HAVING = 53,                    /* HAVING  */
  YYSYMBOL_AS = 54,                        /* AS  */
  YYSYMBOL_NATURAL = 55,                   /* NATURAL  */
  YYSYMBOL_JOIN = 56,                      /* JOIN  */
  YYSYMBOL_LEFT = 57,                      /* LEFT  */
  YYSYMBOL_RIGHT = 58,                     /* RIGHT  */
  YYSYMBOL_OUTER = 59,                     /* OUTER  */
  YYSYMBOL_FULL = 60,                      /* FULL  */
  YYSYMBOL_ON = 61,                        /* ON  */
  YYSYMBOL_INNER = 62,                     /* INNER  */
  YYSYMBOL_CROSS = 63,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 64,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 65,                     /* WHERE  */
  YYSYMBOL_ORDER = 66,                     /* ORDER  */
  YYSYMBOL_LIMIT = 67,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 68,                    /* OFFSET  */
  YYSYMBOL_ASC = 69,                       /* ASC  */
  YYSYMBOL_DESC = 70,                      /* DESC  */
  YYSYMBOL_IF = 71,                        /* IF  */
  YYSYMBOL_NOT = 72,                       /* NOT  */
  YYSYMBOL_EXISTS = 73,                    /* EXISTS  */
  YYSYMBOL_IN = 74,                        /* IN  */
  YYSYMBOL_FROM = 75,                      /* FROM  */
  YYSYMBOL_TO = 76,                        /* TO  */
  YYSYMBOL_WITH = 77,                      /* WITH  */
  YYSYMBOL_DELIMITER = 78,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 79,                    /* FORMAT  */
  YYSYMBOL_HEADER = 80,                    /* HEADER  */
  YYSYMBOL_HIGHLIGHT = 81,                 /* HIGHLIGHT  */
  YYSYMBOL_CAST = 82,                      /* CAST  */
  YYSYMBOL_END = 83,                       /* END  */
  YYSYMBOL_CASE = 84,                      /* CASE  */
  YYSYMBOL_ELSE = 85,                      /* ELSE  */
  YYSYMBOL_THEN = 86,                      /* THEN  */
  YYSYMBOL_WHEN = 87,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 88,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 89,                   /* INTEGER  */
  YYSYMBOL_INT = 90,                       /* INT  */
  YYSYMBOL_TINYINT = 91,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 92,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 93,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 94,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 95,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 96,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 97,                    /* DOUBLE  */
  YYSYMBOL_REAL = 98,                      /* REAL  */
  YYSYMBOL_DECIMAL = 99,                   /* DECIMAL  */
  YYSYMBOL_DATE = 100,                     /* DATE  */
  YYSYMBOL_TIME = 101,                     /* TIME  */
  YYSYMBOL_DATETIME = 102,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 103,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 104,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 105,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 106,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 107,                     /* UUID  */
  YYSYMBOL_POINT = 108,                    /* POINT  */
  YYSYMBOL_LINE = 109,                     /* LINE  */
  YYSYMBOL_LSEG = 110,                     /* LSEG  */
  YYSYMBOL_BOX = 111,                      /* BOX  */
  YYSYMBOL_PATH = 112,                     /* PATH  */
  YYSYMBOL_POLYGON = 113,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 114,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 115,                     /* BLOB  */
  YYSYMBOL_BITMAP = 116,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 117,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 118,                   /* VECTOR  */
  YYSYMBOL_BIT = 119,                      /* BIT  */
  YYSYMBOL_TEXT = 120,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 121,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 122,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 123,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 124,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 125,                   /* IGNORE  */
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
  YYSYMBOL_LEADER = 184,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 185,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 186,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 187,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 188,               /* STANDALONE  */
  YYSYMBOL_NODES = 189,                    /* NODES  */
  YYSYMBOL_NODE = 190,                     /* NODE  */
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
  YYSYMBOL_column_def_array = 221,         /* column_def_array  */
  YYSYMBOL_table_element = 222,            /* table_element  */
  YYSYMBOL_table_column = 223,             /* table_column  */
  YYSYMBOL_column_type = 224,              /* column_type  */
  YYSYMBOL_column_constraints = 225,       /* column_constraints  */
  YYSYMBOL_column_constraint = 226,        /* column_constraint  */
  YYSYMBOL_default_expr = 227,             /* default_expr  */
  YYSYMBOL_table_constraint = 228,         /* table_constraint  */
  YYSYMBOL_identifier_array = 229,         /* identifier_array  */
  YYSYMBOL_delete_statement = 230,         /* delete_statement  */
  YYSYMBOL_insert_statement = 231,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 232, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 233,        /* explain_statement  */
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
  YYSYMBOL_highlight_clause = 252,         /* highlight_clause  */
  YYSYMBOL_from_clause = 253,              /* from_clause  */
  YYSYMBOL_search_clause = 254,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 255, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 256,             /* where_clause  */
  YYSYMBOL_having_clause = 257,            /* having_clause  */
  YYSYMBOL_group_by_clause = 258,          /* group_by_clause  */
  YYSYMBOL_set_operator = 259,             /* set_operator  */
  YYSYMBOL_table_reference = 260,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 261,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 262,     /* table_reference_name  */
  YYSYMBOL_table_name = 263,               /* table_name  */
  YYSYMBOL_table_alias = 264,              /* table_alias  */
  YYSYMBOL_with_clause = 265,              /* with_clause  */
  YYSYMBOL_with_expr_list = 266,           /* with_expr_list  */
  YYSYMBOL_with_expr = 267,                /* with_expr  */
  YYSYMBOL_join_clause = 268,              /* join_clause  */
  YYSYMBOL_join_type = 269,                /* join_type  */
  YYSYMBOL_show_statement = 270,           /* show_statement  */
  YYSYMBOL_flush_statement = 271,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 272,       /* optimize_statement  */
  YYSYMBOL_command_statement = 273,        /* command_statement  */
  YYSYMBOL_compact_statement = 274,        /* compact_statement  */
  YYSYMBOL_admin_statement = 275,          /* admin_statement  */
  YYSYMBOL_alter_statement = 276,          /* alter_statement  */
  YYSYMBOL_expr_array = 277,               /* expr_array  */
  YYSYMBOL_expr_array_list = 278,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 279,               /* expr_alias  */
  YYSYMBOL_expr = 280,                     /* expr  */
  YYSYMBOL_operand = 281,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 282,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 283,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 284,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 285,          /* match_text_expr  */
  YYSYMBOL_query_expr = 286,               /* query_expr  */
  YYSYMBOL_fusion_expr = 287,              /* fusion_expr  */
  YYSYMBOL_sub_search = 288,               /* sub_search  */
  YYSYMBOL_sub_search_array = 289,         /* sub_search_array  */
  YYSYMBOL_function_expr = 290,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 291,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 292,             /* between_expr  */
  YYSYMBOL_in_expr = 293,                  /* in_expr  */
  YYSYMBOL_case_expr = 294,                /* case_expr  */
  YYSYMBOL_case_check_array = 295,         /* case_check_array  */
  YYSYMBOL_cast_expr = 296,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 297,            /* subquery_expr  */
  YYSYMBOL_column_expr = 298,              /* column_expr  */
  YYSYMBOL_constant_expr = 299,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 300,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 301, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 302,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 303, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 304,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 305,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 306, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 307, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 308, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 309,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 310,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 311,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 312,               /* array_expr  */
  YYSYMBOL_long_array_expr = 313,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 314, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 315,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 316, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 317,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 318,         /* copy_option_list  */
  YYSYMBOL_copy_option = 319,              /* copy_option  */
  YYSYMBOL_file_path = 320,                /* file_path  */
  YYSYMBOL_if_exists = 321,                /* if_exists  */
  YYSYMBOL_if_not_exists = 322,            /* if_not_exists  */
  YYSYMBOL_semicolon = 323,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 324,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 325,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 326, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 327,         /* index_param_list  */
  YYSYMBOL_index_param = 328,              /* index_param  */
  YYSYMBOL_index_info = 329                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 458 "parser.cpp"

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
#define YYFINAL  115
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1380

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  214
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  507
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1137

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
       0,   506,   506,   510,   516,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   557,   574,   590,   619,   635,   653,   682,   686,
     691,   695,   701,   704,   711,   762,   801,   802,   803,   804,
     805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   824,   826,   827,
     828,   829,   832,   833,   834,   835,   836,   837,   838,   839,
     840,   841,   842,   843,   844,   845,   846,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   856,   857,   858,   859,
     860,   861,   862,   863,   864,   865,   866,   867,   868,   869,
     870,   871,   872,   873,   874,   875,   876,   877,   878,   879,
     880,   881,   882,   883,   884,   885,   886,   887,   888,   889,
     890,   891,   892,   893,   894,   895,   896,   897,   916,   920,
     930,   933,   936,   939,   943,   946,   951,   956,   963,   969,
     979,   995,  1029,  1042,  1045,  1052,  1064,  1073,  1086,  1090,
    1095,  1108,  1121,  1136,  1151,  1166,  1189,  1242,  1297,  1348,
    1351,  1354,  1363,  1373,  1376,  1380,  1385,  1412,  1415,  1420,
    1437,  1440,  1444,  1448,  1453,  1459,  1462,  1465,  1469,  1473,
    1475,  1479,  1481,  1484,  1488,  1491,  1495,  1498,  1502,  1507,
    1511,  1514,  1518,  1521,  1525,  1528,  1532,  1535,  1539,  1542,
    1545,  1548,  1556,  1559,  1574,  1574,  1576,  1590,  1599,  1604,
    1613,  1618,  1623,  1629,  1636,  1639,  1643,  1646,  1651,  1663,
    1670,  1684,  1687,  1690,  1693,  1696,  1699,  1702,  1708,  1712,
    1716,  1720,  1724,  1731,  1735,  1739,  1743,  1747,  1752,  1756,
    1761,  1765,  1769,  1775,  1781,  1787,  1798,  1809,  1820,  1832,
    1844,  1857,  1871,  1882,  1896,  1912,  1929,  1933,  1937,  1941,
    1945,  1949,  1955,  1959,  1963,  1967,  1977,  1981,  1985,  1993,
    2004,  2027,  2033,  2038,  2044,  2050,  2058,  2064,  2070,  2076,
    2082,  2090,  2096,  2102,  2108,  2114,  2122,  2128,  2134,  2143,
    2153,  2166,  2170,  2175,  2181,  2188,  2196,  2205,  2215,  2225,
    2236,  2247,  2259,  2271,  2281,  2292,  2304,  2317,  2321,  2326,
    2331,  2337,  2341,  2345,  2351,  2355,  2361,  2365,  2370,  2375,
    2382,  2391,  2401,  2410,  2422,  2438,  2442,  2447,  2451,  2484,
    2490,  2494,  2495,  2496,  2497,  2498,  2500,  2503,  2509,  2512,
    2513,  2514,  2515,  2516,  2517,  2518,  2519,  2520,  2521,  2525,
    2543,  2589,  2628,  2671,  2718,  2742,  2765,  2786,  2807,  2816,
    2828,  2835,  2845,  2851,  2863,  2866,  2869,  2872,  2875,  2878,
    2882,  2886,  2891,  2899,  2907,  2916,  2923,  2930,  2937,  2944,
    2951,  2959,  2967,  2975,  2983,  2991,  2999,  3007,  3015,  3023,
    3031,  3039,  3047,  3077,  3085,  3094,  3102,  3111,  3119,  3125,
    3132,  3138,  3145,  3150,  3157,  3164,  3172,  3199,  3205,  3211,
    3218,  3226,  3233,  3240,  3245,  3255,  3260,  3265,  3270,  3275,
    3280,  3285,  3290,  3295,  3300,  3303,  3306,  3310,  3313,  3316,
    3319,  3323,  3326,  3329,  3333,  3337,  3342,  3347,  3350,  3354,
    3358,  3365,  3372,  3376,  3383,  3390,  3394,  3398,  3402,  3405,
    3409,  3413,  3418,  3423,  3427,  3432,  3437,  3443,  3449,  3455,
    3461,  3467,  3473,  3479,  3485,  3491,  3497,  3503,  3514,  3518,
    3523,  3554,  3564,  3569,  3574,  3579,  3585,  3589,  3590,  3592,
    3593,  3595,  3596,  3608,  3616,  3620,  3623,  3627,  3630,  3634,
    3638,  3643,  3649,  3659,  3669,  3677,  3688,  3719
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
  "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "VIEW",
  "INDEX", "VIEWS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS",
  "COLUMN", "COLUMNS", "INDEXES", "CHUNK", "GROUP", "BY", "HAVING", "AS",
  "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER",
  "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC",
  "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT",
  "HEADER", "HIGHLIGHT", "CAST", "END", "CASE", "ELSE", "THEN", "WHEN",
  "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT",
  "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME",
  "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT", "MULTIVECTOR", "TENSOR",
  "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER",
  "FOLLOWER", "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE",
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "':'", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "column_def_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "default_expr", "table_constraint",
  "identifier_array", "delete_statement", "insert_statement",
  "optional_identifier_array", "explain_statement", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "highlight_clause", "from_clause",
  "search_clause", "optional_search_filter_expr", "where_clause",
  "having_clause", "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "flush_statement",
  "optimize_statement", "command_statement", "compact_statement",
  "admin_statement", "alter_statement", "expr_array", "expr_array_list",
  "expr_alias", "expr", "operand", "match_tensor_expr",
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
  "index_info", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-672)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-495)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     731,   445,    14,   472,    63,     3,    63,   144,    70,   678,
      56,    62,   128,   153,   214,   206,   238,   329,   230,    20,
     -13,   351,   210,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    -672,   354,  -672,  -672,   417,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,   356,   356,   356,   356,     4,    63,   385,   385,
     385,   385,   385,   242,   452,    63,    35,   457,   469,   471,
     763,  -672,  -672,  -672,  -672,  -672,  -672,  -672,   354,  -672,
    -672,  -672,  -672,  -672,   278,   473,    63,  -672,  -672,  -672,
    -672,  -672,   488,  -672,   271,   330,  -672,   519,  -672,   359,
    -672,  -672,   538,  -672,   239,   133,    63,    63,    63,    63,
    -672,  -672,  -672,  -672,   -46,  -672,   503,   361,  -672,   557,
     179,   234,   573,   386,   392,  -672,    32,  -672,   578,  -672,
    -672,     9,   541,  -672,   584,   551,   656,    63,    63,    63,
     657,   600,   454,   590,   661,    63,    63,    63,   665,   666,
     668,   608,   682,   682,   464,    82,   139,   196,  -672,   479,
    -672,   316,  -672,  -672,   679,  -672,   686,  -672,  -672,  -672,
     689,  -672,  -672,  -672,  -672,   258,  -672,  -672,  -672,    63,
     486,   329,   682,  -672,   533,  -672,   690,  -672,  -672,   695,
    -672,  -672,   694,  -672,   698,   654,  -672,  -672,  -672,  -672,
       9,  -672,  -672,  -672,   464,   658,   646,   642,  -672,   -16,
    -672,   454,  -672,    63,   713,    54,  -672,  -672,  -672,  -672,
    -672,   663,  -672,   527,   -15,  -672,   464,  -672,  -672,   640,
     641,   521,  -672,  -672,   971,   520,   523,   524,   296,   723,
     729,   730,   735,  -672,  -672,   746,   542,   270,   545,   550,
     603,   603,  -672,    13,   381,   134,  -672,    58,   677,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,   549,  -672,  -672,  -672,   103,  -672,  -672,   111,
    -672,   141,  -672,  -672,  -672,   182,  -672,   199,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,   760,   758,  -672,  -672,  -672,
    -672,  -672,  -672,   719,   720,   692,   688,   417,  -672,  -672,
     765,    49,  -672,   772,  -672,   335,   572,   574,   -21,   464,
     464,   703,  -672,   -13,   118,   736,   581,  -672,   201,   583,
    -672,    63,   464,   668,  -672,   390,   588,   589,   132,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    -672,   603,   591,   757,   721,   464,   464,   136,   331,  -672,
    -672,  -672,  -672,   971,  -672,   791,   592,   593,   595,   602,
     798,   808,   305,   305,  -672,   601,  -672,  -672,  -672,  -672,
     604,   127,   739,   464,   814,   464,   464,    -4,   610,    -1,
     603,   603,   603,   603,   603,   603,   603,   603,   603,   603,
     603,   603,   603,   603,    23,  -672,   613,  -672,   815,  -672,
     816,  -672,   817,  -672,   825,   777,   510,   623,   625,   831,
     627,  -672,   634,  -672,   838,  -672,   288,   687,   691,  -672,
    -672,   464,   779,   643,  -672,    60,   390,   464,  -672,   354,
     896,   732,   648,   208,  -672,  -672,  -672,   -13,   857,  -672,
    -672,   858,   464,   650,  -672,   390,  -672,    46,    46,   464,
    -672,   322,   721,   708,   655,   109,    41,   364,  -672,   464,
     464,   790,   464,   862,    26,   464,   659,   340,   562,  -672,
    -672,   682,  -672,  -672,  -672,   733,   662,   603,   381,   743,
    -672,   778,   778,   718,   718,   744,   778,   778,   718,   718,
     305,   305,  -672,  -672,  -672,  -672,  -672,  -672,   670,  -672,
     671,  -672,  -672,  -672,   880,   882,  -672,   713,   886,  -672,
     887,  -672,  -672,   889,  -672,   894,   897,   697,    10,   724,
     464,  -672,  -672,  -672,   390,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,   702,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,   705,
     707,   716,   722,   726,   727,   165,   728,   713,   870,   118,
     354,   741,  -672,   341,   745,   905,   910,   899,   928,  -672,
     926,   342,  -672,   360,   376,  -672,   742,  -672,   896,   464,
    -672,   464,     8,   122,   603,   -45,   725,  -672,   169,   119,
      68,   754,  -672,   934,  -672,  -672,   863,   381,   778,   755,
     382,  -672,   603,   935,   947,   904,   918,   391,   401,  -672,
     768,   402,  -672,   961,  -672,  -672,   -13,   756,   433,  -672,
     300,  -672,   280,   608,  -672,   963,   587,   932,   949,   966,
     983,  1000,   841,   845,  -672,  -672,   191,  -672,   842,   713,
     403,   766,  -672,  -672,   813,  -672,   464,  -672,  -672,  -672,
    -672,  -672,  -672,    46,  -672,  -672,  -672,   792,   390,    19,
    -672,   464,   431,   802,  1005,   613,   805,   800,   464,  -672,
     806,   818,   807,   415,  -672,  -672,   757,  1021,  1024,  -672,
    -672,   886,   513,  -672,   887,   412,    -6,    10,   975,  -672,
    -672,  -672,  -672,  -672,  -672,   976,  -672,  1030,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,   822,   992,   416,   832,
     835,   836,   837,   848,   849,   852,   853,   854,   959,   855,
     865,   866,   869,   871,   872,   883,   888,   906,   908,   991,
     909,   911,   912,   913,   914,   915,   916,   917,   919,   920,
    1003,   921,   922,   923,   924,   925,   927,   929,   930,   931,
     933,  1007,   936,   937,   938,   939,   940,   941,   942,   943,
     944,   945,  1008,   946,   948,   950,   951,   952,   953,   954,
     955,   956,   957,  1010,   958,  -672,  -672,   149,  -672,  -672,
    -672,   430,  -672,   887,  1119,   435,  -672,  -672,  -672,   390,
    -672,   570,   960,   962,   964,    15,   965,  -672,  -672,  -672,
    1065,   969,   390,  -672,    46,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  1132,  -672,   300,   433,    10,
      10,   967,   280,  1088,  1091,  -672,  1140,  1141,  1153,  1155,
    1165,  1167,  1173,  1174,  1175,  1176,   972,  1177,  1179,  1180,
    1181,  1182,  1183,  1184,  1185,  1186,  1187,   982,  1189,  1190,
    1191,  1192,  1193,  1194,  1195,  1196,  1197,  1198,   993,  1200,
    1201,  1202,  1203,  1204,  1205,  1206,  1207,  1208,  1209,  1004,
    1211,  1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,
    1015,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,  1230,
    1231,  1026,  1233,  -672,  -672,   437,   688,  -672,  -672,  1236,
     101,  1032,  1238,  1239,  -672,   439,  1240,  -672,  -672,  1188,
     713,  -672,   464,   464,  -672,  1035,  1036,  1038,  1039,  1041,
    1042,  1043,  1044,  1045,  1046,  1250,  1048,  1049,  1050,  1051,
    1052,  1053,  1054,  1055,  1056,  1057,  1261,  1059,  1060,  1061,
    1062,  1063,  1064,  1066,  1067,  1068,  1069,  1268,  1070,  1071,
    1072,  1073,  1074,  1075,  1076,  1077,  1078,  1079,  1283,  1081,
    1082,  1083,  1084,  1085,  1086,  1087,  1089,  1090,  1092,  1291,
    1093,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,
    1294,  1103,  -672,  -672,  1104,   800,  -672,  1105,  1106,  -672,
     395,   464,   446,   697,   390,  -672,  -672,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  1110,  -672,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  1111,  -672,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  1112,  -672,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  1113,  -672,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  1114,
    -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    1115,  -672,  1309,  1116,  1310,   100,  1117,  1321,  1322,  -672,
    -672,  -672,   390,  -672,  -672,  -672,  -672,  -672,  -672,  -672,
    1118,  -672,  1120,   800,   688,  1323,   571,   183,  1122,  1311,
    1124,  -672,   594,  1328,  -672,   800,   688,   800,   -10,  1329,
    -672,  1288,  1128,  -672,  1129,  1299,  1300,  -672,  -672,  -672,
      -3,  -672,  -672,  1133,  1302,  1303,  -672,  1341,  -672,  1137,
    1138,  1343,   688,  1139,  -672,   688,  -672
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     225,     0,     0,     0,     0,     0,     0,     0,   225,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     225,     0,   492,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   170,   169,     0,     8,    14,    15,    16,    17,
      18,    19,   490,   490,   490,   490,   490,     0,   488,   488,
     488,   488,   488,   218,     0,     0,     0,     0,     0,     0,
     225,   156,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   239,   240,   238,
     244,   248,     0,   245,     0,     0,   241,     0,   243,     0,
     266,   268,     0,   246,     0,   272,     0,     0,     0,     0,
     276,   277,   278,   281,   218,   279,     0,   224,   226,     0,
       0,     0,     0,     0,     0,     1,   225,     2,   208,   210,
     211,     0,   193,   175,   181,     0,     0,     0,     0,     0,
       0,     0,   154,     0,     0,     0,     0,     0,     0,     0,
       0,   203,     0,     0,     0,     0,     0,     0,   155,     0,
     254,   255,   249,   250,     0,   251,     0,   242,   267,   247,
       0,   270,   269,   273,   274,     0,   300,   298,   299,     0,
       0,     0,     0,   327,     0,   328,     0,   321,   322,     0,
     317,   301,     0,   324,   326,     0,   174,   173,     4,   209,
       0,   171,   172,   192,     0,     0,   189,     0,    32,     0,
      33,   154,   493,     0,     0,   225,   487,   161,   163,   162,
     164,     0,   219,     0,   203,   158,     0,   150,   486,     0,
       0,   421,   425,   428,   429,     0,     0,     0,     0,     0,
       0,     0,     0,   426,   427,     0,     0,     0,     0,     0,
       0,     0,   423,     0,   225,     0,   335,   340,   341,   355,
     353,   356,   354,   357,   358,   350,   345,   344,   343,   351,
     352,   342,   349,   348,   436,   438,     0,   439,   447,     0,
     448,     0,   440,   437,   458,     0,   459,     0,   435,   285,
     287,   286,   283,   284,   290,   292,   291,   288,   289,   295,
     297,   296,   293,   294,   275,     0,     0,   257,   256,   262,
     252,   253,   271,     0,     0,     0,   496,     0,   227,   282,
       0,   318,   323,   302,   325,     0,     0,     0,   195,     0,
       0,   191,   489,   225,     0,     0,     0,   148,     0,     0,
     152,     0,     0,     0,   157,   202,     0,     0,     0,   467,
     466,   469,   468,   471,   470,   473,   472,   475,   474,   477,
     476,     0,     0,   387,   225,     0,     0,     0,     0,   430,
     431,   432,   433,     0,   434,     0,     0,     0,     0,     0,
       0,     0,   389,   388,   464,   461,   455,   445,   450,   453,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   444,     0,   449,     0,   452,
       0,   460,     0,   463,     0,   263,   258,     0,     0,     0,
       0,   280,     0,   329,     0,   319,     0,     0,     0,   178,
     177,     0,   197,   180,   182,   187,   188,     0,   176,    35,
       0,     0,     0,     0,    38,    42,    43,   225,     0,    37,
     153,     0,     0,   151,   165,   160,   159,     0,     0,     0,
     382,     0,   225,     0,     0,     0,     0,     0,   412,     0,
       0,     0,     0,     0,     0,     0,   201,     0,     0,   347,
     346,     0,   336,   339,   405,   406,     0,     0,   225,     0,
     386,   396,   397,   400,   401,     0,   403,   395,   398,   399,
     391,   390,   392,   393,   394,   422,   424,   446,     0,   451,
       0,   454,   462,   465,     0,     0,   259,     0,     0,   332,
       0,   228,   320,     0,   303,     0,     0,   194,     0,   199,
       0,   185,   186,   184,   190,    46,    49,    50,    47,    48,
      51,    52,    68,    53,    55,    54,    71,    58,    59,    60,
      56,    57,    61,    62,    63,    64,    65,    66,    67,     0,
       0,     0,     0,     0,     0,   496,     0,     0,   498,     0,
      36,     0,   149,     0,     0,     0,     0,     0,     0,   482,
       0,     0,   478,     0,     0,   383,     0,   417,     0,     0,
     410,     0,     0,     0,     0,     0,     0,   421,     0,     0,
       0,     0,   372,     0,   457,   456,     0,   225,   404,     0,
       0,   385,     0,     0,     0,   264,   260,     0,     0,    40,
     501,     0,   499,   304,   330,   331,   225,   196,   212,   214,
     223,   215,     0,   203,   183,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   141,   142,   145,   138,   145,     0,
       0,     0,    34,    39,   507,   337,     0,   484,   483,   481,
     480,   485,   168,     0,   166,   384,   418,     0,   414,     0,
     413,     0,     0,     0,     0,     0,     0,   201,     0,   370,
       0,     0,     0,     0,   419,   408,   407,     0,     0,   334,
     333,     0,     0,   495,     0,     0,     0,     0,     0,   232,
     233,   234,   235,   231,   236,     0,   221,     0,   216,   376,
     374,   377,   375,   378,   379,   380,   198,   207,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   143,   140,     0,   139,    45,
      44,     0,   147,     0,     0,     0,   479,   416,   411,   415,
     402,     0,     0,   201,     0,     0,     0,   441,   443,   442,
       0,     0,   200,   373,     0,   420,   409,   265,   261,    41,
     502,   503,   505,   504,   500,     0,   305,   223,   213,     0,
       0,   220,     0,     0,   205,    70,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   144,   146,     0,   496,   338,   461,     0,
       0,     0,     0,     0,   371,     0,   306,   217,   229,     0,
       0,   381,     0,     0,   179,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   497,   506,     0,   201,   368,     0,   201,   167,
       0,     0,     0,   206,   204,    69,    75,    76,    73,    74,
      77,    78,    79,    80,    81,     0,    72,   119,   120,   117,
     118,   121,   122,   123,   124,   125,     0,   116,    86,    87,
      84,    85,    88,    89,    90,    91,    92,     0,    83,    97,
      98,    95,    96,    99,   100,   101,   102,   103,     0,    94,
     130,   131,   128,   129,   132,   133,   134,   135,   136,     0,
     127,   108,   109,   106,   107,   110,   111,   112,   113,   114,
       0,   105,     0,     0,     0,     0,     0,     0,     0,   308,
     307,   313,   230,   222,    82,   126,    93,   104,   137,   115,
     201,   369,     0,   201,   496,   314,   309,     0,     0,     0,
       0,   367,     0,     0,   310,   201,   496,   201,   496,     0,
     315,   311,     0,   363,     0,     0,     0,   366,   316,   312,
     496,   359,   365,     0,     0,     0,   362,     0,   361,     0,
       0,     0,   496,     0,   364,   496,   360
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -672,  -672,  -672,  1234,  1289,   217,  -672,  -672,   782,  -485,
     764,  -672,   709,   706,  -672,  -508,   237,   265,  1152,  -672,
     268,  -672,  1023,   279,   282,    -7,  1337,   -17,  1058,  1168,
     -67,  -672,  -672,   829,  -672,  -672,  -672,  -672,  -672,  -672,
    -672,  -671,  -207,  -672,  -672,  -672,  -672,   664,  -173,    21,
     535,  -672,  -672,  1199,  -672,  -672,   284,   287,   290,   297,
     298,  -672,  -672,  -192,  -672,   977,  -216,  -217,  -602,  -601,
    -597,  -580,  -574,  -563,   531,  -672,  -672,  -672,  -672,  -672,
    -672,  1009,  -672,  -672,   890,   580,  -238,  -672,  -672,  -672,
     693,  -672,  -672,  -672,  -672,   696,   968,   970,  -273,  -672,
    -672,  -672,  -672,  1136,  -448,   710,  -132,   478,   495,  -672,
    -672,  -561,  -672,   575,   680,  -672
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   443,   618,   444,   445,
     565,   646,   647,   789,   446,   328,    25,    26,   205,    27,
      28,   214,   215,    29,    30,    31,    32,    33,   123,   191,
     124,   196,   433,   434,   533,   321,   438,   194,   432,   529,
     633,   601,   217,   924,   834,   121,   627,   628,   629,   630,
     708,    34,   107,   108,   631,   705,    35,    36,    37,    38,
      39,    40,    41,   245,   453,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   715,   716,   255,   256,   257,   258,
     259,   358,   260,   261,   262,   263,   264,   806,   265,   266,
     267,   268,   269,   270,   271,   272,   378,   379,   273,   274,
     275,   276,   277,   278,   581,   582,   219,   134,   126,   117,
     131,   421,   652,   621,   622,   449
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     335,    68,   318,   114,   648,   377,   811,   334,   353,   617,
     583,   220,   357,    53,   118,   169,   119,   122,   374,   375,
     374,   375,   120,   372,   373,    54,   505,    56,   381,   597,
     709,   710,  -491,   619,   110,   711,   111,   105,   323,     1,
     309,     2,     3,     4,     5,     6,     7,     8,     9,    10,
     216,    47,   712,    68,   192,    11,    12,    13,   713,   650,
     431,    14,    15,    16,    17,  -494,    53,   420,   132,   714,
     486,   489,   677,    60,   420,   125,   141,     1,    55,     2,
       3,     4,     5,     6,     7,   279,     9,   280,   281,   424,
      96,   670,   329,    11,    12,    13,    97,   151,   425,    14,
      15,    16,   798,   435,   436,   995,  1093,   385,   386,    17,
     142,   143,   384,   575,   576,  1115,   455,   165,   166,   167,
     168,   440,  1124,   317,   577,   578,   579,   589,   490,   531,
     532,    17,   911,   678,   353,   221,   222,   223,   224,   465,
     466,   791,   284,   282,   285,   286,   461,    17,   199,   200,
     201,   487,  1116,   222,   223,   224,   208,   209,   210,  1125,
     385,   386,    98,   588,   139,   678,   678,   673,   507,   484,
     485,   385,   386,   491,   492,   493,   494,   495,   496,   497,
     498,   499,   500,   501,   502,   503,   504,    99,   330,  1105,
     306,   383,   324,   385,   386,    20,   459,   333,    18,   289,
     287,   290,   291,   827,   225,   226,   819,   112,   671,   103,
     385,   386,   385,   386,   227,    19,   228,   190,   626,   243,
     376,   534,   376,   356,   326,    62,   506,   380,   580,   242,
     709,   710,   229,   230,   231,   711,    18,   642,   232,   527,
      20,   104,   420,   144,   441,    63,   442,   283,   678,   229,
     230,   231,   712,   592,   593,   232,   595,   292,   713,   599,
     573,   385,   386,   642,   233,   234,   235,   584,   303,   714,
     608,   385,   386,    64,   385,   386,    65,    62,    20,   385,
     386,   233,   234,   235,   304,   305,   236,    66,   385,   386,
      67,   643,    69,   644,   645,    70,   610,    63,    71,   221,
     222,   223,   224,   706,   288,    72,    73,    57,    58,   237,
     405,   238,    59,   239,   435,   406,   439,   643,   407,   644,
     645,   523,   787,   408,  1073,    64,   163,  1076,    65,   164,
     524,   676,   106,   240,   241,   242,   480,   464,   243,    66,
     244,   460,    67,   382,    69,   993,   383,    70,   409,   606,
      71,   115,   454,   410,   707,   243,   295,    72,    73,   296,
     297,   293,   173,   174,   298,   299,   915,   175,   225,   226,
     100,   101,   102,   668,   118,   669,   119,   672,   227,   404,
     228,   675,   120,   356,   221,   222,   223,   224,   366,   411,
     367,   176,   368,   369,   412,   686,   229,   230,   231,   109,
     802,   177,   232,   809,   178,   179,   413,   180,   181,   182,
     450,   414,  1002,   451,   468,   683,   469,   568,   470,  1098,
     569,   116,  1100,   183,   184,   122,   717,   125,   233,   234,
     235,   160,   161,   162,  1112,  1077,  1114,   804,  1078,  1079,
     570,   153,   154,  1080,  1081,   586,   825,   590,   826,   591,
     236,   470,   139,   225,   226,   799,   133,   237,    17,   238,
     145,   239,   812,   227,   795,   228,   140,   221,   222,   223,
     224,   609,   146,   237,   147,   238,   150,   239,    42,    43,
      44,   229,   230,   231,    45,    46,   149,   232,   698,  -237,
     699,   700,   701,   702,   152,   703,   704,   240,   241,   242,
     155,   156,   243,   463,   244,    48,    49,    50,   401,   402,
     403,    51,    52,   233,   234,   235,   820,   821,   822,   823,
     427,   428,   157,   221,   222,   223,   224,   135,   136,   137,
     138,   585,   158,  1101,   383,   236,   225,   226,   127,   128,
     129,   130,   385,   386,   159,  1113,   227,  1117,   228,   602,
     655,   662,   603,   383,   663,   515,   516,   170,   237,  1126,
     238,   389,   239,   172,   229,   230,   231,   604,   605,   664,
     232,  1134,   663,   171,  1136,   374,   908,   185,   390,   391,
     392,   393,   240,   241,   242,   665,   395,   243,   383,   244,
     682,   685,   351,   352,   383,   186,   233,   234,   235,   189,
     689,   187,   227,   451,   228,   193,   221,   222,   223,   224,
     690,   693,   792,   691,   694,   451,  1103,  1104,   236,   696,
     229,   230,   231,   197,   816,   835,   232,   383,   836,   396,
     397,   398,   399,   400,   401,   402,   403,  1109,  1110,   904,
     800,   237,   451,   238,   907,   239,   992,   383,   999,   694,
     195,   663,   233,   234,   235,  1083,   918,   919,   451,   198,
     202,   203,   204,   206,   207,   240,   241,   242,   211,   212,
     243,   213,   244,   216,   236,   351,   719,   720,   721,   722,
     723,    74,   300,   724,   725,   227,   218,   228,   294,   301,
     726,   727,   728,   302,   307,   310,   311,   237,   312,   238,
     313,   239,   314,   229,   230,   231,   729,  1004,   315,   232,
     319,    75,    76,   320,    77,   322,   327,   336,   337,    78,
      79,   240,   241,   242,   331,   332,   243,   359,   244,   338,
    1003,   354,   355,   360,   361,   233,   234,   235,     1,   362,
       2,     3,     4,     5,     6,     7,     8,     9,    10,   387,
     365,   388,   363,   370,    11,    12,    13,   236,   371,   404,
      14,    15,    16,   415,   416,   420,   417,   418,   419,   423,
       1,   437,     2,     3,     4,     5,     6,     7,   426,     9,
     237,   429,   238,   430,   239,  1082,    11,    12,    13,   448,
     447,   452,    14,    15,    16,   471,   457,   458,    17,   462,
     472,   473,   476,   474,   240,   241,   242,   389,    17,   243,
     475,   244,   477,   479,   478,   481,   463,   483,   488,   243,
     514,   508,   510,   512,   390,   391,   392,   393,   394,   463,
     513,   517,   395,   518,   519,   520,    80,    81,    82,    83,
      17,    84,    85,   521,   522,    86,    87,    88,   389,   525,
      89,    90,    91,   526,   528,   530,   567,    92,    93,   566,
     571,   572,   574,   487,   587,   594,   596,  -495,  -495,    94,
     607,   600,   611,    95,   389,   396,   397,   398,   399,   400,
     401,   402,   403,   613,   614,   385,   615,   389,   616,   440,
     620,   390,   391,   392,   393,   623,   612,    18,   624,   395,
     632,   625,   651,   659,   390,   391,   392,   393,   389,   383,
     635,   657,   395,   636,    19,   637,   658,  -495,  -495,   399,
     400,   401,   402,   403,   638,  -495,  -495,   392,   393,    18,
     639,   660,   661,  -495,   640,   641,   649,   674,   680,    20,
     681,   605,   396,   397,   398,   399,   400,   401,   402,   403,
     654,   666,   604,   656,   687,   396,   397,   398,   399,   400,
     401,   402,   403,   679,   684,   688,   692,   695,   697,   718,
     785,    20,   786,   787,   793,   794,  -495,   397,   398,   399,
     400,   401,   402,   403,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   797,   552,   553,   554,   555,   556,   557,   801,   803,
     558,   805,   810,   559,   560,   813,   815,   561,   562,   563,
     564,   730,   731,   732,   733,   734,   814,   817,   735,   736,
     818,   829,   830,   831,   832,   737,   738,   739,   741,   742,
     743,   744,   745,   833,   837,   746,   747,   838,   839,   840,
     846,   740,   748,   749,   750,   752,   753,   754,   755,   756,
     841,   842,   757,   758,   843,   844,   845,   847,   751,   759,
     760,   761,   763,   764,   765,   766,   767,   848,   849,   768,
     769,   850,   857,   851,   852,   762,   770,   771,   772,   774,
     775,   776,   777,   778,   868,   853,   779,   780,   879,   890,
     854,   901,   773,   781,   782,   783,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   855,   784,
     856,   858,   906,   859,   860,   861,   862,   863,   864,   865,
     678,   866,   867,   869,   870,   871,   872,   873,   916,   874,
     922,   875,   876,   877,   923,   878,   925,   926,   880,   881,
     882,   883,   884,   885,   886,   887,   888,   889,   891,   927,
     892,   928,   893,   894,   895,   896,   897,   898,   899,   900,
     902,   929,   909,   930,   910,   920,   912,   913,   914,   931,
     932,   933,   934,   936,   935,   937,   938,   939,   940,   941,
     942,   943,   944,   945,   946,   947,   948,   949,   950,   951,
     952,   953,   954,   955,   956,   957,   958,   959,   960,   961,
     962,   963,   964,   965,   966,   967,   968,   969,   970,   971,
     972,   973,   974,   975,   976,   977,   978,   979,   980,   981,
     982,   983,   984,   985,   986,   987,   988,   989,   990,   991,
     994,   996,   997,   998,  1005,  1006,  1000,  1007,  1008,  1001,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,
    1029,  1030,  1031,  1032,  1037,  1033,  1034,  1035,  1036,  1038,
    1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,
    1049,  1050,  1051,  1052,  1053,  1054,  1055,  1059,  1056,  1057,
    1070,  1058,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,
    1068,  1069,  1071,  1090,  1092,  1107,  1072,  1074,  1075,  1084,
    1085,  1086,  1087,  1088,  1089,  1091,  1094,  1095,  1096,  1102,
    1097,  1106,  1099,  1108,  1111,  1118,  1119,  1120,  1121,  1122,
    1123,  1127,  1128,  1129,  1130,  1131,  1133,  1132,  1135,   148,
     188,   653,   667,   325,   790,   788,   456,   113,   316,   634,
     482,   828,   917,   921,   598,   422,   467,   903,   905,   807,
     308,   364,   808,   796,   824,     0,   509,     0,     0,     0,
     511
};

static const yytype_int16 yycheck[] =
{
     216,     8,   194,    20,   565,   243,   677,   214,   225,   517,
     458,   143,   228,     3,    20,    61,    22,     8,     5,     6,
       5,     6,    28,   240,   241,     4,     3,     6,   244,     3,
     632,   632,     0,   518,    14,   632,    16,    16,    54,     7,
     172,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      65,    37,   632,    60,   121,    23,    24,    25,   632,   567,
      81,    29,    30,    31,    77,    61,     3,    77,    47,   632,
      74,    72,     4,     3,    77,    71,    55,     7,    75,     9,
      10,    11,    12,    13,    14,     3,    16,     5,     6,    40,
      34,    83,    38,    23,    24,    25,    34,    76,    49,    29,
      30,    31,    83,   319,   320,     4,     6,   152,   153,    77,
      75,    76,    54,    67,    68,   125,   332,    96,    97,    98,
      99,     3,   125,   190,    78,    79,    80,    86,   129,    69,
      70,    77,   803,    65,   351,     3,     4,     5,     6,   355,
     356,   649,     3,    61,     5,     6,   338,    77,   127,   128,
     129,   155,   162,     4,     5,     6,   135,   136,   137,   162,
     152,   153,    34,    54,   210,    65,    65,   212,   406,   385,
     386,   152,   153,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   403,    34,   205,     6,
     169,   212,   208,   152,   153,   208,    64,   212,   166,     3,
      61,     5,     6,   209,    72,    73,   691,   187,    86,     3,
     152,   153,   152,   153,    82,   183,    84,   208,   208,   206,
     207,   437,   207,    87,   203,     8,   203,   244,   182,   203,
     832,   832,   100,   101,   102,   832,   166,    72,   106,   431,
     208,     3,    77,   208,   126,     8,   128,   165,    65,   100,
     101,   102,   832,   469,   470,   106,   472,    61,   832,   475,
     452,   152,   153,    72,   132,   133,   134,   459,    10,   832,
     487,   152,   153,     8,   152,   153,     8,    60,   208,   152,
     153,   132,   133,   134,    26,    27,   154,     8,   152,   153,
       8,   126,     8,   128,   129,     8,   488,    60,     8,     3,
       4,     5,     6,     3,   165,     8,     8,   163,   164,   177,
     207,   179,   168,   181,   530,   212,   323,   126,   207,   128,
     129,    33,   131,   212,   995,    60,   193,   998,    60,   196,
      42,   212,     3,   201,   202,   203,   209,   354,   206,    60,
     208,   209,    60,   209,    60,   906,   212,    60,   207,   481,
      60,     0,   331,   212,    54,   206,    40,    60,    60,    43,
      44,   165,   183,   184,    48,    49,   814,   188,    72,    73,
     156,   157,   158,   589,    20,   591,    22,   594,    82,   210,
      84,   212,    28,    87,     3,     4,     5,     6,   118,   207,
     120,   157,   122,   123,   212,   612,   100,   101,   102,   169,
     673,   167,   106,   676,   170,   171,   207,   173,   174,   175,
     209,   212,   920,   212,    83,   607,    85,   209,    87,  1090,
     212,   211,  1093,   189,   190,     8,   633,    71,   132,   133,
     134,   192,   193,   194,  1105,    40,  1107,   675,    43,    44,
     447,   170,   171,    48,    49,   462,    34,    83,    36,    85,
     154,    87,   210,    72,    73,   671,    71,   177,    77,   179,
       3,   181,   678,    82,   656,    84,    14,     3,     4,     5,
       6,   488,     3,   177,     3,   179,     3,   181,    33,    34,
      35,   100,   101,   102,    39,    40,   208,   106,    55,    56,
      57,    58,    59,    60,     6,    62,    63,   201,   202,   203,
     170,   171,   206,    72,   208,    33,    34,    35,   203,   204,
     205,    39,    40,   132,   133,   134,     3,     4,     5,     6,
     185,   186,     3,     3,     4,     5,     6,    49,    50,    51,
      52,   209,   173,  1094,   212,   154,    72,    73,    43,    44,
      45,    46,   152,   153,     6,  1106,    82,  1108,    84,   209,
     209,   209,   212,   212,   212,    45,    46,    54,   177,  1120,
     179,   130,   181,     6,   100,   101,   102,     5,     6,   209,
     106,  1132,   212,   212,  1135,     5,     6,     4,   147,   148,
     149,   150,   201,   202,   203,   209,   155,   206,   212,   208,
     607,   209,    72,    73,   212,   209,   132,   133,   134,    21,
     209,   209,    82,   212,    84,    64,     3,     4,     5,     6,
     209,   209,   209,   212,   212,   212,    45,    46,   154,   626,
     100,   101,   102,    72,   209,   209,   106,   212,   212,   198,
     199,   200,   201,   202,   203,   204,   205,    43,    44,   209,
     209,   177,   212,   179,   209,   181,   209,   212,   209,   212,
      66,   212,   132,   133,   134,   209,   829,   830,   212,     3,
       3,    61,   208,    73,     3,   201,   202,   203,     3,     3,
     206,     3,   208,    65,   154,    72,    89,    90,    91,    92,
      93,     3,     3,    96,    97,    82,     4,    84,   209,     3,
     103,   104,   105,     4,   208,   162,     6,   177,     3,   179,
       6,   181,     4,   100,   101,   102,   119,   923,    54,   106,
      52,    33,    34,    67,    36,    73,     3,    77,    77,    41,
      42,   201,   202,   203,    61,   198,   206,     4,   208,   208,
     922,   208,   208,     4,     4,   132,   133,   134,     7,     4,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    72,
     208,    74,     6,   208,    23,    24,    25,   154,   208,   210,
      29,    30,    31,     3,     6,    77,    47,    47,    76,     4,
       7,    68,     9,    10,    11,    12,    13,    14,     6,    16,
     177,   209,   179,   209,   181,  1001,    23,    24,    25,   208,
      54,   208,    29,    30,    31,     4,   208,   208,    77,   208,
     208,   208,     4,   208,   201,   202,   203,   130,    77,   206,
     208,   208,     4,   209,   213,    76,    72,     3,   208,   206,
      43,     6,     6,     6,   147,   148,   149,   150,   151,    72,
       5,   208,   155,   208,     3,   208,   158,   159,   160,   161,
      77,   163,   164,   209,     6,   167,   168,   169,   130,   162,
     172,   173,   174,   162,    75,   212,   208,   179,   180,   127,
       3,     3,   212,   155,   209,    75,     4,   149,   150,   191,
     208,   212,   129,   195,   130,   198,   199,   200,   201,   202,
     203,   204,   205,   213,   213,   152,     6,   130,     6,     3,
       3,   147,   148,   149,   150,     6,   152,   166,     4,   155,
     176,     4,    32,     4,   147,   148,   149,   150,   130,   212,
     208,     6,   155,   208,   183,   208,     6,   199,   200,   201,
     202,   203,   204,   205,   208,   147,   148,   149,   150,   166,
     208,     3,     6,   155,   208,   208,   208,   212,     4,   208,
      77,     6,   198,   199,   200,   201,   202,   203,   204,   205,
     209,   209,     5,   208,    50,   198,   199,   200,   201,   202,
     203,   204,   205,   209,   209,    47,   198,     6,   212,     6,
     129,   208,   127,   131,   208,   162,   198,   199,   200,   201,
     202,   203,   204,   205,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   209,   106,   107,   108,   109,   110,   111,   206,     4,
     114,   206,   212,   117,   118,   209,   209,   121,   122,   123,
     124,    89,    90,    91,    92,    93,   208,     6,    96,    97,
       6,    56,    56,     3,   212,   103,   104,   105,    89,    90,
      91,    92,    93,    51,   212,    96,    97,   212,   212,   212,
      91,   119,   103,   104,   105,    89,    90,    91,    92,    93,
     212,   212,    96,    97,   212,   212,   212,   212,   119,   103,
     104,   105,    89,    90,    91,    92,    93,   212,   212,    96,
      97,   212,    91,   212,   212,   119,   103,   104,   105,    89,
      90,    91,    92,    93,    91,   212,    96,    97,    91,    91,
     212,    91,   119,   103,   104,   105,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   212,   119,
     212,   212,     3,   212,   212,   212,   212,   212,   212,   212,
      65,   212,   212,   212,   212,   212,   212,   212,     6,   212,
      52,   212,   212,   212,    53,   212,     6,     6,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,     6,
     212,     6,   212,   212,   212,   212,   212,   212,   212,   212,
     212,     6,   212,     6,   212,   208,   212,   212,   209,     6,
       6,     6,     6,     6,   212,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   212,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   212,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   212,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   212,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   212,     6,
       4,   209,     4,     4,   209,   209,     6,   209,   209,    61,
     209,   209,   209,   209,   209,   209,     6,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     6,   209,   209,
     209,   209,   209,   209,     6,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,     6,
     209,   209,   209,   209,   209,   209,   209,     6,   209,   209,
       6,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,     4,     4,     4,   212,   212,   212,   209,
     209,   209,   209,   209,   209,   209,   209,     6,     6,     6,
     212,   209,   212,   209,     6,     6,    48,   209,   209,    40,
      40,   208,    40,    40,     3,   208,     3,   209,   209,    60,
     116,   569,   588,   201,   648,   646,   333,    20,   190,   530,
     383,   697,   827,   832,   474,   307,   357,   787,   793,   676,
     171,   235,   676,   663,   694,    -1,   408,    -1,    -1,    -1,
     410
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    77,   166,   183,
     208,   215,   216,   217,   219,   230,   231,   233,   234,   237,
     238,   239,   240,   241,   265,   270,   271,   272,   273,   274,
     275,   276,    33,    34,    35,    39,    40,    37,    33,    34,
      35,    39,    40,     3,   263,    75,   263,   163,   164,   168,
       3,   218,   219,   230,   231,   234,   237,   238,   239,   270,
     271,   272,   273,   274,     3,    33,    34,    36,    41,    42,
     158,   159,   160,   161,   163,   164,   167,   168,   169,   172,
     173,   174,   179,   180,   191,   195,    34,    34,    34,    34,
     156,   157,   158,     3,     3,   263,     3,   266,   267,   169,
      14,    16,   187,   240,   241,     0,   211,   323,    20,    22,
      28,   259,     8,   242,   244,    71,   322,   322,   322,   322,
     322,   324,   263,    71,   321,   321,   321,   321,   321,   210,
      14,   263,    75,    76,   208,     3,     3,     3,   218,   208,
       3,   263,     6,   170,   171,   170,   171,     3,   173,     6,
     192,   193,   194,   193,   196,   263,   263,   263,   263,    61,
      54,   212,     6,   183,   184,   188,   157,   167,   170,   171,
     173,   174,   175,   189,   190,     4,   209,   209,   217,    21,
     208,   243,   244,    64,   251,    66,   245,    72,     3,   263,
     263,   263,     3,    61,   208,   232,    73,     3,   263,   263,
     263,     3,     3,     3,   235,   236,    65,   256,     4,   320,
     320,     3,     4,     5,     6,    72,    73,    82,    84,   100,
     101,   102,   106,   132,   133,   134,   154,   177,   179,   181,
     201,   202,   203,   206,   208,   277,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   290,   291,   292,   293,   294,
     296,   297,   298,   299,   300,   302,   303,   304,   305,   306,
     307,   308,   309,   312,   313,   314,   315,   316,   317,     3,
       5,     6,    61,   165,     3,     5,     6,    61,   165,     3,
       5,     6,    61,   165,   209,    40,    43,    44,    48,    49,
       3,     3,     4,    10,    26,    27,   263,   208,   267,   320,
     162,     6,     3,     6,     4,    54,   243,   244,   277,    52,
      67,   249,    73,    54,   208,   232,   263,     3,   229,    38,
     241,    61,   198,   212,   256,   280,    77,    77,   208,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,    72,    73,   281,   208,   208,    87,   280,   295,     4,
       4,     4,     4,     6,   317,   208,   118,   120,   122,   123,
     208,   208,   281,   281,     5,     6,   207,   300,   310,   311,
     241,   280,   209,   212,    54,   152,   153,    72,    74,   130,
     147,   148,   149,   150,   151,   155,   198,   199,   200,   201,
     202,   203,   204,   205,   210,   207,   212,   207,   212,   207,
     212,   207,   212,   207,   212,     3,     6,    47,    47,    76,
      77,   325,   242,     4,    40,    49,     6,   185,   186,   209,
     209,    81,   252,   246,   247,   280,   280,    68,   250,   239,
       3,   126,   128,   220,   222,   223,   228,    54,   208,   329,
     209,   212,   208,   278,   263,   280,   236,   208,   208,    64,
     209,   277,   208,    72,   241,   280,   280,   295,    83,    85,
      87,     4,   208,   208,   208,   208,     4,     4,   213,   209,
     209,    76,   279,     3,   280,   280,    74,   155,   208,    72,
     129,   281,   281,   281,   281,   281,   281,   281,   281,   281,
     281,   281,   281,   281,   281,     3,   203,   300,     6,   310,
       6,   311,     6,     5,    43,    45,    46,   208,   208,     3,
     208,   209,     6,    33,    42,   162,   162,   277,    75,   253,
     212,    69,    70,   248,   280,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   106,   107,   108,   109,   110,   111,   114,   117,
     118,   121,   122,   123,   124,   224,   127,   208,   209,   212,
     239,     3,     3,   277,   212,    67,    68,    78,    79,    80,
     182,   318,   319,   318,   277,   209,   241,   209,    54,    86,
      83,    85,   280,   280,    75,   280,     4,     3,   298,   280,
     212,   255,   209,   212,     5,     6,   320,   208,   281,   241,
     277,   129,   152,   213,   213,     6,     6,   229,   221,   223,
       3,   327,   328,     6,     4,     4,   208,   260,   261,   262,
     263,   268,   176,   254,   247,   208,   208,   208,   208,   208,
     208,   208,    72,   126,   128,   129,   225,   226,   325,   208,
     229,    32,   326,   222,   209,   209,   208,     6,     6,     4,
       3,     6,   209,   212,   209,   209,   209,   224,   280,   280,
      83,    86,   281,   212,   212,   212,   212,     4,    65,   209,
       4,    77,   241,   277,   209,   209,   281,    50,    47,   209,
     209,   212,   198,   209,   212,     6,   239,   212,    55,    57,
      58,    59,    60,    62,    63,   269,     3,    54,   264,   282,
     283,   284,   285,   286,   287,   288,   289,   256,     6,    89,
      90,    91,    92,    93,    96,    97,   103,   104,   105,   119,
      89,    90,    91,    92,    93,    96,    97,   103,   104,   105,
     119,    89,    90,    91,    92,    93,    96,    97,   103,   104,
     105,   119,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   119,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   119,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   119,   129,   127,   131,   226,   227,
     227,   229,   209,   208,   162,   277,   319,   209,    83,   280,
     209,   206,   312,     4,   300,   206,   301,   304,   309,   312,
     212,   255,   280,   209,   208,   209,   209,     6,     6,   223,
       3,     4,     5,     6,   328,    34,    36,   209,   261,    56,
      56,     3,   212,    51,   258,   209,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,    91,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,    91,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,    91,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,    91,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
      91,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,    91,   212,   299,   209,   327,     3,   209,     6,   212,
     212,   255,   212,   212,   209,   318,     6,   264,   262,   262,
     208,   288,    52,    53,   257,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   212,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   212,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   212,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   212,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   212,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     212,     6,   209,   325,     4,     4,   209,     4,     4,   209,
       6,    61,   229,   277,   280,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,     6,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,     6,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     6,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,     6,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,     6,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
       6,   209,   212,   255,   212,   212,   255,    40,    43,    44,
      48,    49,   280,   209,   209,   209,   209,   209,   209,   209,
       4,   209,     4,     6,   209,     6,     6,   212,   255,   212,
     255,   325,     6,    45,    46,     6,   209,     4,   209,    43,
      44,     6,   255,   325,   255,   125,   162,   325,     6,    48,
     209,   209,    40,    40,   125,   162,   325,   208,    40,    40,
       3,   208,   209,     3,   325,   209,   325
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   214,   215,   216,   216,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   219,   219,   219,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   223,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   225,   225,
     226,   226,   226,   226,   227,   227,   228,   228,   229,   229,
     230,   231,   231,   232,   232,   233,   233,   234,   235,   235,
     236,   237,   237,   237,   237,   237,   238,   238,   238,   239,
     239,   239,   239,   240,   240,   241,   242,   243,   243,   244,
     245,   245,   246,   246,   247,   248,   248,   248,   249,   249,
     250,   250,   251,   251,   252,   252,   253,   253,   254,   254,
     255,   255,   256,   256,   257,   257,   258,   258,   259,   259,
     259,   259,   260,   260,   261,   261,   262,   262,   263,   263,
     264,   264,   264,   264,   265,   265,   266,   266,   267,   268,
     268,   269,   269,   269,   269,   269,   269,   269,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   271,   271,   271,   272,
     272,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     274,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   276,   276,   276,   277,   277,   278,   278,   279,
     279,   280,   280,   280,   280,   280,   281,   281,   281,   281,
     281,   281,   281,   281,   281,   281,   281,   281,   281,   282,
     283,   283,   283,   283,   284,   284,   284,   284,   285,   285,
     286,   286,   287,   287,   288,   288,   288,   288,   288,   288,
     289,   289,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   291,   291,   292,   293,   293,
     294,   294,   294,   294,   295,   295,   296,   297,   297,   297,
     297,   298,   298,   298,   298,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   300,   300,   300,
     300,   301,   301,   301,   302,   303,   303,   304,   304,   305,
     306,   306,   307,   308,   308,   309,   310,   311,   312,   312,
     313,   314,   314,   315,   316,   316,   317,   317,   317,   317,
     317,   317,   317,   317,   317,   317,   317,   317,   318,   318,
     319,   319,   319,   319,   319,   319,   320,   321,   321,   322,
     322,   323,   323,   324,   324,   325,   325,   326,   326,   327,
     327,   328,   328,   328,   328,   328,   329,   329
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     8,     6,     7,     6,     1,     3,
       1,     3,     1,     1,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       4,     1,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     1,     2,
       2,     1,     1,     2,     2,     0,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     2,     5,     1,     3,
       3,     4,     4,     4,     4,     6,     8,    11,     8,     1,
       1,     3,     3,     3,     3,     2,     4,     3,     3,     9,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     2,     0,     2,     0,     3,     0,     1,     2,
       1,     1,     1,     3,     1,     1,     2,     4,     1,     3,
       2,     1,     5,     0,     2,     0,     1,     3,     5,     4,
       6,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     2,     3,     2,     2,     2,     2,     3,     2,     3,
       3,     3,     4,     4,     3,     3,     4,     4,     5,     6,
       7,     9,     4,     5,     7,     9,     2,     3,     2,     3,
       3,     4,     2,     3,     3,     4,     2,     2,     2,     2,
       5,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     3,     3,
       3,     3,     4,     6,     7,     9,    10,    12,    12,    13,
      14,    15,    16,    12,    13,    15,    16,     3,     4,     5,
       6,     3,     3,     4,     3,     4,     3,     3,     3,     5,
       7,     7,     6,     8,     8,     1,     3,     3,     5,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,    14,
      20,    16,    15,    13,    18,    14,    13,    11,     8,    10,
       5,     7,     4,     6,     1,     1,     1,     1,     1,     1,
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
       3,     1,     3,     3,     3,     3,     6,     3
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
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2374 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2382 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 234 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2396 "parser.cpp"
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
#line 2410 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 224 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2424 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2435 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2443 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2452 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2461 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 284 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2475 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2486 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2496 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2506 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2526 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2536 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2546 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2574 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2584 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2592 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 244 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2623 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2639 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2647 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2655 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 244 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2669 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2678 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2687 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2696 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 304 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2709 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 294 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2732 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 294 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2746 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2756 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2765 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 244 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2779 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 254 "parser.y"
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
#line 2796 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2804 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2812 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 244 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2890 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2898 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2906 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2914 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2922 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2930 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 365 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2943 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2951 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2959 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3007 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3015 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3023 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3055 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3063 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3071 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3079 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3138 "parser.cpp"
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
#line 3152 "parser.cpp"
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
#line 3166 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3177 "parser.cpp"
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

#line 3285 "parser.cpp"

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
#line 506 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3500 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 510 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3511 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 516 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3522 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 523 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3528 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3534 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3540 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3546 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3552 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3558 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3564 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3570 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3576 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 532 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3582 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 533 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3588 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3594 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3600 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3606 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3612 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 539 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3618 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3624 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3630 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3636 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3642 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3648 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3654 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3660 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3666 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3672 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3678 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3684 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 557 "parser.y"
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
#line 3704 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 574 "parser.y"
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
#line 3722 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 590 "parser.y"
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
#line 3755 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 619 "parser.y"
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
#line 3775 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 635 "parser.y"
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
#line 3796 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 653 "parser.y"
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
#line 3829 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 682 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3838 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 686 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3847 "parser.cpp"
    break;

  case 40: /* column_def_array: table_column  */
#line 691 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3856 "parser.cpp"
    break;

  case 41: /* column_def_array: column_def_array ',' table_column  */
#line 695 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3865 "parser.cpp"
    break;

  case 42: /* table_element: table_column  */
#line 701 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3873 "parser.cpp"
    break;

  case 43: /* table_element: table_constraint  */
#line 704 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3881 "parser.cpp"
    break;

  case 44: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 711 "parser.y"
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
#line 3937 "parser.cpp"
    break;

  case 45: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 762 "parser.y"
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
#line 3979 "parser.cpp"
    break;

  case 46: /* column_type: BOOLEAN  */
#line 801 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3985 "parser.cpp"
    break;

  case 47: /* column_type: TINYINT  */
#line 802 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3991 "parser.cpp"
    break;

  case 48: /* column_type: SMALLINT  */
#line 803 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3997 "parser.cpp"
    break;

  case 49: /* column_type: INTEGER  */
#line 804 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4003 "parser.cpp"
    break;

  case 50: /* column_type: INT  */
#line 805 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4009 "parser.cpp"
    break;

  case 51: /* column_type: BIGINT  */
#line 806 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4015 "parser.cpp"
    break;

  case 52: /* column_type: HUGEINT  */
#line 807 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4021 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT  */
#line 808 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4027 "parser.cpp"
    break;

  case 54: /* column_type: REAL  */
#line 809 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4033 "parser.cpp"
    break;

  case 55: /* column_type: DOUBLE  */
#line 810 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4039 "parser.cpp"
    break;

  case 56: /* column_type: FLOAT16  */
#line 811 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4045 "parser.cpp"
    break;

  case 57: /* column_type: BFLOAT16  */
#line 812 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4051 "parser.cpp"
    break;

  case 58: /* column_type: DATE  */
#line 813 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4057 "parser.cpp"
    break;

  case 59: /* column_type: TIME  */
#line 814 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4063 "parser.cpp"
    break;

  case 60: /* column_type: DATETIME  */
#line 815 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4069 "parser.cpp"
    break;

  case 61: /* column_type: TIMESTAMP  */
#line 816 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4075 "parser.cpp"
    break;

  case 62: /* column_type: UUID  */
#line 817 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4081 "parser.cpp"
    break;

  case 63: /* column_type: POINT  */
#line 818 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4087 "parser.cpp"
    break;

  case 64: /* column_type: LINE  */
#line 819 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4093 "parser.cpp"
    break;

  case 65: /* column_type: LSEG  */
#line 820 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4099 "parser.cpp"
    break;

  case 66: /* column_type: BOX  */
#line 821 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4105 "parser.cpp"
    break;

  case 67: /* column_type: CIRCLE  */
#line 824 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4111 "parser.cpp"
    break;

  case 68: /* column_type: VARCHAR  */
#line 826 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4117 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4123 "parser.cpp"
    break;

  case 70: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 828 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4129 "parser.cpp"
    break;

  case 71: /* column_type: DECIMAL  */
#line 829 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4135 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4141 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4147 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4153 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4159 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4165 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4171 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4177 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4183 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4189 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4195 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4201 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4207 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4213 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4219 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4225 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4231 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4237 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4243 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4249 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4255 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4261 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4267 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4273 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4279 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4285 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4291 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4297 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4303 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4309 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4315 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4321 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4327 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4333 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4339 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4345 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4351 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4357 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4363 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4369 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4375 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4381 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4387 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4393 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4399 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4405 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4411 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 878 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4417 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 879 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4423 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 880 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4429 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 881 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4435 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 882 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4441 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 883 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4447 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 884 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4453 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 885 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4459 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 886 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4465 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 887 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4471 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 888 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4477 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 889 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4483 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 890 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4489 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 891 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4495 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 892 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4501 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 893 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4507 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 894 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4513 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 895 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4519 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 896 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4525 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 897 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4531 "parser.cpp"
    break;

  case 138: /* column_constraints: column_constraint  */
#line 916 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4540 "parser.cpp"
    break;

  case 139: /* column_constraints: column_constraints column_constraint  */
#line 920 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4554 "parser.cpp"
    break;

  case 140: /* column_constraint: PRIMARY KEY  */
#line 930 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4562 "parser.cpp"
    break;

  case 141: /* column_constraint: UNIQUE  */
#line 933 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4570 "parser.cpp"
    break;

  case 142: /* column_constraint: NULLABLE  */
#line 936 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4578 "parser.cpp"
    break;

  case 143: /* column_constraint: NOT NULLABLE  */
#line 939 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4586 "parser.cpp"
    break;

  case 144: /* default_expr: DEFAULT constant_expr  */
#line 943 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4594 "parser.cpp"
    break;

  case 145: /* default_expr: %empty  */
#line 946 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4602 "parser.cpp"
    break;

  case 146: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 951 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4612 "parser.cpp"
    break;

  case 147: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 956 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4622 "parser.cpp"
    break;

  case 148: /* identifier_array: IDENTIFIER  */
#line 963 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4633 "parser.cpp"
    break;

  case 149: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 969 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4644 "parser.cpp"
    break;

  case 150: /* delete_statement: DELETE FROM table_name where_clause  */
#line 979 "parser.y"
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
#line 4661 "parser.cpp"
    break;

  case 151: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 995 "parser.y"
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
#line 4700 "parser.cpp"
    break;

  case 152: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1029 "parser.y"
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
#line 4717 "parser.cpp"
    break;

  case 153: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1042 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4725 "parser.cpp"
    break;

  case 154: /* optional_identifier_array: %empty  */
#line 1045 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4733 "parser.cpp"
    break;

  case 155: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1052 "parser.y"
                                                             {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    ParserHelper::ToLower((yyvsp[-1].str_value));
    if(!strcmp((yyvsp[-1].str_value), "analyze")) (yyval.explain_stmt)->type_ = infinity::ExplainType::kAnalyze;
    else if(!strcmp((yyvsp[-1].str_value), "ast")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kAst;
    else if(!strcmp((yyvsp[-1].str_value), "raw")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kUnOpt;
    else if(!strcmp((yyvsp[-1].str_value), "logical")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kOpt;
    else if(!strcmp((yyvsp[-1].str_value), "physical")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    else if(!strcmp((yyvsp[-1].str_value), "pipeline")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kPipeline;
    else if(!strcmp((yyvsp[-1].str_value), "fragment")) (yyval.explain_stmt)->type_ =infinity::ExplainType::kFragment;
    free((yyvsp[-1].str_value));
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4751 "parser.cpp"
    break;

  case 156: /* explain_statement: EXPLAIN explainable_statement  */
#line 1064 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4761 "parser.cpp"
    break;

  case 157: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1073 "parser.y"
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
#line 4778 "parser.cpp"
    break;

  case 158: /* update_expr_array: update_expr  */
#line 1086 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4787 "parser.cpp"
    break;

  case 159: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1090 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4796 "parser.cpp"
    break;

  case 160: /* update_expr: IDENTIFIER '=' expr  */
#line 1095 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4808 "parser.cpp"
    break;

  case 161: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1108 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4824 "parser.cpp"
    break;

  case 162: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1121 "parser.y"
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
#line 4842 "parser.cpp"
    break;

  case 163: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1136 "parser.y"
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
#line 4860 "parser.cpp"
    break;

  case 164: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1151 "parser.y"
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
#line 4878 "parser.cpp"
    break;

  case 165: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1166 "parser.y"
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
#line 4901 "parser.cpp"
    break;

  case 166: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1189 "parser.y"
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
#line 4959 "parser.cpp"
    break;

  case 167: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1242 "parser.y"
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
#line 5019 "parser.cpp"
    break;

  case 168: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1297 "parser.y"
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
#line 5071 "parser.cpp"
    break;

  case 169: /* select_statement: select_without_paren  */
#line 1348 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5079 "parser.cpp"
    break;

  case 170: /* select_statement: select_with_paren  */
#line 1351 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5087 "parser.cpp"
    break;

  case 171: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1354 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5101 "parser.cpp"
    break;

  case 172: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1363 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5115 "parser.cpp"
    break;

  case 173: /* select_with_paren: '(' select_without_paren ')'  */
#line 1373 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5123 "parser.cpp"
    break;

  case 174: /* select_with_paren: '(' select_with_paren ')'  */
#line 1376 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5131 "parser.cpp"
    break;

  case 175: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1380 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5140 "parser.cpp"
    break;

  case 176: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1385 "parser.y"
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
        if ((yyvsp[-2].order_by_expr_list_t)) {
            for (auto ptr : *((yyvsp[-2].order_by_expr_list_t))) {
                delete ptr;
            }
            delete (yyvsp[-2].order_by_expr_list_t);
        }
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
#line 5171 "parser.cpp"
    break;

  case 177: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1412 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5179 "parser.cpp"
    break;

  case 178: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1415 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5187 "parser.cpp"
    break;

  case 179: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1420 "parser.y"
                                                                                                                 {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-7].bool_value);
    (yyval.select_stmt)->select_list_ = (yyvsp[-6].expr_array_t);
    (yyval.select_stmt)->highlight_list_ = (yyvsp[-5].expr_array_t);
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
#line 5208 "parser.cpp"
    break;

  case 180: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1437 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5216 "parser.cpp"
    break;

  case 181: /* order_by_clause: %empty  */
#line 1440 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5224 "parser.cpp"
    break;

  case 182: /* order_by_expr_list: order_by_expr  */
#line 1444 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5233 "parser.cpp"
    break;

  case 183: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1448 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5242 "parser.cpp"
    break;

  case 184: /* order_by_expr: expr order_by_type  */
#line 1453 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5252 "parser.cpp"
    break;

  case 185: /* order_by_type: ASC  */
#line 1459 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5260 "parser.cpp"
    break;

  case 186: /* order_by_type: DESC  */
#line 1462 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5268 "parser.cpp"
    break;

  case 187: /* order_by_type: %empty  */
#line 1465 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5276 "parser.cpp"
    break;

  case 188: /* limit_expr: LIMIT expr  */
#line 1469 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5284 "parser.cpp"
    break;

  case 189: /* limit_expr: %empty  */
#line 1473 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5290 "parser.cpp"
    break;

  case 190: /* offset_expr: OFFSET expr  */
#line 1475 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5298 "parser.cpp"
    break;

  case 191: /* offset_expr: %empty  */
#line 1479 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5304 "parser.cpp"
    break;

  case 192: /* distinct: DISTINCT  */
#line 1481 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5312 "parser.cpp"
    break;

  case 193: /* distinct: %empty  */
#line 1484 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5320 "parser.cpp"
    break;

  case 194: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1488 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5328 "parser.cpp"
    break;

  case 195: /* highlight_clause: %empty  */
#line 1491 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5336 "parser.cpp"
    break;

  case 196: /* from_clause: FROM table_reference  */
#line 1495 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5344 "parser.cpp"
    break;

  case 197: /* from_clause: %empty  */
#line 1498 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5352 "parser.cpp"
    break;

  case 198: /* search_clause: SEARCH sub_search_array  */
#line 1502 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5362 "parser.cpp"
    break;

  case 199: /* search_clause: %empty  */
#line 1507 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5370 "parser.cpp"
    break;

  case 200: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1511 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5378 "parser.cpp"
    break;

  case 201: /* optional_search_filter_expr: %empty  */
#line 1514 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5386 "parser.cpp"
    break;

  case 202: /* where_clause: WHERE expr  */
#line 1518 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5394 "parser.cpp"
    break;

  case 203: /* where_clause: %empty  */
#line 1521 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5402 "parser.cpp"
    break;

  case 204: /* having_clause: HAVING expr  */
#line 1525 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5410 "parser.cpp"
    break;

  case 205: /* having_clause: %empty  */
#line 1528 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5418 "parser.cpp"
    break;

  case 206: /* group_by_clause: GROUP BY expr_array  */
#line 1532 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5426 "parser.cpp"
    break;

  case 207: /* group_by_clause: %empty  */
#line 1535 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5434 "parser.cpp"
    break;

  case 208: /* set_operator: UNION  */
#line 1539 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5442 "parser.cpp"
    break;

  case 209: /* set_operator: UNION ALL  */
#line 1542 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5450 "parser.cpp"
    break;

  case 210: /* set_operator: INTERSECT  */
#line 1545 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5458 "parser.cpp"
    break;

  case 211: /* set_operator: EXCEPT  */
#line 1548 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5466 "parser.cpp"
    break;

  case 212: /* table_reference: table_reference_unit  */
#line 1556 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5474 "parser.cpp"
    break;

  case 213: /* table_reference: table_reference ',' table_reference_unit  */
#line 1559 "parser.y"
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
#line 5492 "parser.cpp"
    break;

  case 216: /* table_reference_name: table_name table_alias  */
#line 1576 "parser.y"
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
#line 5510 "parser.cpp"
    break;

  case 217: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1590 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5521 "parser.cpp"
    break;

  case 218: /* table_name: IDENTIFIER  */
#line 1599 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5531 "parser.cpp"
    break;

  case 219: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1604 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5543 "parser.cpp"
    break;

  case 220: /* table_alias: AS IDENTIFIER  */
#line 1613 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5553 "parser.cpp"
    break;

  case 221: /* table_alias: IDENTIFIER  */
#line 1618 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5563 "parser.cpp"
    break;

  case 222: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1623 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5574 "parser.cpp"
    break;

  case 223: /* table_alias: %empty  */
#line 1629 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5582 "parser.cpp"
    break;

  case 224: /* with_clause: WITH with_expr_list  */
#line 1636 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5590 "parser.cpp"
    break;

  case 225: /* with_clause: %empty  */
#line 1639 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5598 "parser.cpp"
    break;

  case 226: /* with_expr_list: with_expr  */
#line 1643 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5607 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1646 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5616 "parser.cpp"
    break;

  case 228: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1651 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5628 "parser.cpp"
    break;

  case 229: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1663 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5640 "parser.cpp"
    break;

  case 230: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1670 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5653 "parser.cpp"
    break;

  case 231: /* join_type: INNER  */
#line 1684 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5661 "parser.cpp"
    break;

  case 232: /* join_type: LEFT  */
#line 1687 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5669 "parser.cpp"
    break;

  case 233: /* join_type: RIGHT  */
#line 1690 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5677 "parser.cpp"
    break;

  case 234: /* join_type: OUTER  */
#line 1693 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5685 "parser.cpp"
    break;

  case 235: /* join_type: FULL  */
#line 1696 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5693 "parser.cpp"
    break;

  case 236: /* join_type: CROSS  */
#line 1699 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5701 "parser.cpp"
    break;

  case 237: /* join_type: %empty  */
#line 1702 "parser.y"
                {
}
#line 5708 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW DATABASES  */
#line 1708 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5717 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLES  */
#line 1712 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5726 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW VIEWS  */
#line 1716 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5735 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW CONFIGS  */
#line 1720 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5744 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1724 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5756 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW PROFILES  */
#line 1731 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5765 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW BUFFER  */
#line 1735 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5774 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW MEMINDEX  */
#line 1739 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5783 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW QUERIES  */
#line 1743 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5792 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1747 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5802 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TRANSACTIONS  */
#line 1752 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5811 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1756 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5821 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW SESSION VARIABLES  */
#line 1761 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5830 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1765 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5839 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1769 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5850 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1775 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5861 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1781 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5872 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW TABLE table_name  */
#line 1787 "parser.y"
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
#line 5888 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1798 "parser.y"
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
#line 5904 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1809 "parser.y"
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
#line 5920 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1820 "parser.y"
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
#line 5937 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1832 "parser.y"
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
#line 5954 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1844 "parser.y"
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
#line 5972 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1857 "parser.y"
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
#line 5991 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1871 "parser.y"
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
#line 6007 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1882 "parser.y"
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
#line 6026 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1896 "parser.y"
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
#line 6047 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1912 "parser.y"
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
#line 6069 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW LOGS  */
#line 1929 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6078 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DELTA LOGS  */
#line 1933 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6087 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW CATALOGS  */
#line 1937 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6096 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1941 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6105 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1945 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6114 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1949 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6125 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW MEMORY  */
#line 1955 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6134 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1959 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6143 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1963 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6152 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 1967 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6163 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH DATA  */
#line 1977 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6172 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH LOG  */
#line 1981 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6181 "parser.cpp"
    break;

  case 278: /* flush_statement: FLUSH BUFFER  */
#line 1985 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6190 "parser.cpp"
    break;

  case 279: /* optimize_statement: OPTIMIZE table_name  */
#line 1993 "parser.y"
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
#line 6205 "parser.cpp"
    break;

  case 280: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2004 "parser.y"
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
#line 6229 "parser.cpp"
    break;

  case 281: /* command_statement: USE IDENTIFIER  */
#line 2027 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6240 "parser.cpp"
    break;

  case 282: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2033 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6250 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2038 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6261 "parser.cpp"
    break;

  case 284: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2044 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6272 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2050 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6285 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2058 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6296 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2064 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6307 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2070 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6318 "parser.cpp"
    break;

  case 289: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2076 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6329 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2082 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6342 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2090 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6353 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2096 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6364 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2102 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6375 "parser.cpp"
    break;

  case 294: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2108 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6386 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2114 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6399 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2122 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6410 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2128 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6421 "parser.cpp"
    break;

  case 298: /* command_statement: LOCK TABLE table_name  */
#line 2134 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6435 "parser.cpp"
    break;

  case 299: /* command_statement: UNLOCK TABLE table_name  */
#line 2143 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6449 "parser.cpp"
    break;

  case 300: /* compact_statement: COMPACT TABLE table_name  */
#line 2153 "parser.y"
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
#line 6466 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2166 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6475 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2170 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6485 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2175 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6496 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2181 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6508 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2188 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6521 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2196 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6535 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2205 "parser.y"
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
#line 6550 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2215 "parser.y"
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
#line 6565 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2225 "parser.y"
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
#line 6581 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2236 "parser.y"
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
#line 6597 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2247 "parser.y"
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
#line 6614 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2259 "parser.y"
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
#line 6631 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2271 "parser.y"
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
#line 6646 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2281 "parser.y"
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
#line 6662 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2292 "parser.y"
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
#line 6679 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2304 "parser.y"
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
#line 6697 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOGS  */
#line 2317 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6706 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2321 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6716 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2326 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6726 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2331 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6737 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2337 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6746 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2341 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6755 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2345 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6766 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW NODES  */
#line 2351 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6775 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2355 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6786 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW NODE  */
#line 2361 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6795 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SET ADMIN  */
#line 2365 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6805 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SET STANDALONE  */
#line 2370 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6815 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2375 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6827 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2382 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6841 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2391 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6855 "parser.cpp"
    break;

  case 332: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2401 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 6869 "parser.cpp"
    break;

  case 333: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2410 "parser.y"
                                                             {
    auto *ret = new infinity::AddColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;

    for (infinity::ColumnDef*& column_def : *(yyvsp[-1].column_def_array_t)) {
        ret->column_defs_.emplace_back(column_def);
    }
    delete (yyvsp[-1].column_def_array_t);
    free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);
}
#line 6886 "parser.cpp"
    break;

  case 334: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2422 "parser.y"
                                                              {
    auto *ret = new infinity::DropColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    for (std::string &column_name : *(yyvsp[-1].identifier_array_t)) {
        ret->column_names_.emplace_back(std::move(column_name));
    }
    delete (yyvsp[-1].identifier_array_t);
    free((yyvsp[-5].table_name_t)->schema_name_ptr_);
    free((yyvsp[-5].table_name_t)->table_name_ptr_);
    delete (yyvsp[-5].table_name_t);
}
#line 6902 "parser.cpp"
    break;

  case 335: /* expr_array: expr_alias  */
#line 2438 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6911 "parser.cpp"
    break;

  case 336: /* expr_array: expr_array ',' expr_alias  */
#line 2442 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6920 "parser.cpp"
    break;

  case 337: /* expr_array_list: '(' expr_array ')'  */
#line 2447 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6929 "parser.cpp"
    break;

  case 338: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2451 "parser.y"
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
#line 6955 "parser.cpp"
    break;

  case 339: /* expr_alias: expr AS IDENTIFIER  */
#line 2484 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6966 "parser.cpp"
    break;

  case 340: /* expr_alias: expr  */
#line 2490 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6974 "parser.cpp"
    break;

  case 346: /* operand: '(' expr ')'  */
#line 2500 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6982 "parser.cpp"
    break;

  case 347: /* operand: '(' select_without_paren ')'  */
#line 2503 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6993 "parser.cpp"
    break;

  case 348: /* operand: constant_expr  */
#line 2509 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7001 "parser.cpp"
    break;

  case 359: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2525 "parser.y"
                                                                                                                                        {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-10].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-4].str_value));
    // search options
    match_tensor_expr->SetExtraOptions((yyvsp[-2].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-1].expr_t));
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7021 "parser.cpp"
    break;

  case 360: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2543 "parser.y"
                                                                                                                                                                                   {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-16].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-10].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-10].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-12].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-12].str_value), (yyvsp[-14].const_expr_t));
    if (!check) {
        goto Error1;
    }
    free((yyvsp[-12].str_value));
    free((yyvsp[-10].str_value));
    delete (yyvsp[-14].const_expr_t);

    match_vector_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    match_vector_expr->topn_ = (yyvsp[-8].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-7].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-12].str_value));
    free((yyvsp[-10].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-14].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 7071 "parser.cpp"
    break;

  case 361: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2589 "parser.y"
                                                                                                                       {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-12].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-6].str_value));
    if (!check) {
        goto Error2;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-8].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-8].str_value), (yyvsp[-10].const_expr_t));
    if (!check) {
        goto Error2;
    }
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));
    delete (yyvsp[-10].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-4].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-3].expr_t));
    match_vector_expr->ignore_index_ = true;
    goto Return2;
Error2:
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));
    delete (yyvsp[-10].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 7114 "parser.cpp"
    break;

  case 362: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2628 "parser.y"
                                                                                                                                {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-11].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-5].str_value));
    if (!check) {
        goto Error3;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-7].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-7].str_value), (yyvsp[-9].const_expr_t));
    if (!check) {
        goto Error3;
    }
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-3].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-2].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return3;
Error3:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return3:
    ;
}
#line 7161 "parser.cpp"
    break;

  case 363: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2671 "parser.y"
                                                                                                                 {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-9].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-3].str_value));
    if (!check) {
        goto Error4;
    }

    // vector search data type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-5].str_value), (yyvsp[-7].const_expr_t));
    if (!check) {
        goto Error4;
    }
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    delete (yyvsp[-7].const_expr_t);

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->filter_expr_.reset((yyvsp[-2].expr_t));
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return4;

Error4:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    delete (yyvsp[-7].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return4:
    ;
}
#line 7209 "parser.cpp"
    break;

  case 364: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2718 "parser.y"
                                                                                                                                                                                     {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-14].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-12].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-10].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-10].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-8].long_value), (yyvsp[0].with_index_param_list_t));

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-7].expr_t));

    match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
}
#line 7237 "parser.cpp"
    break;

  case 365: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2742 "parser.y"
                                                                                                                          {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-10].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-8].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-6].str_value));

    // topn and options
    match_sparse_expr->topn_ = (yyvsp[-4].long_value);

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-3].expr_t));

    match_sparse_expr->ignore_index_ = true;
}
#line 7264 "parser.cpp"
    break;

  case 366: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2765 "parser.y"
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

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-2].expr_t));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-3].long_value), (yyvsp[0].with_index_param_list_t));
}
#line 7289 "parser.cpp"
    break;

  case 367: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2786 "parser.y"
                                                                                                                    {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-7].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-5].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-3].str_value));

    // optional filter
    match_sparse_expr->SetOptionalFilter((yyvsp[-2].expr_t));

    // topn and options
    match_sparse_expr->SetOptParams(infinity::DEFAULT_MATCH_SPARSE_TOP_N, (yyvsp[0].with_index_param_list_t));
}
#line 7314 "parser.cpp"
    break;

  case 368: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2807 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7328 "parser.cpp"
    break;

  case 369: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2816 "parser.y"
                                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-6].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7344 "parser.cpp"
    break;

  case 370: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2828 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7356 "parser.cpp"
    break;

  case 371: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2835 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7370 "parser.cpp"
    break;

  case 372: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2845 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7381 "parser.cpp"
    break;

  case 373: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2851 "parser.y"
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
#line 7397 "parser.cpp"
    break;

  case 374: /* sub_search: match_vector_expr  */
#line 2863 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7405 "parser.cpp"
    break;

  case 375: /* sub_search: match_text_expr  */
#line 2866 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7413 "parser.cpp"
    break;

  case 376: /* sub_search: match_tensor_expr  */
#line 2869 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7421 "parser.cpp"
    break;

  case 377: /* sub_search: match_sparse_expr  */
#line 2872 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7429 "parser.cpp"
    break;

  case 378: /* sub_search: query_expr  */
#line 2875 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7437 "parser.cpp"
    break;

  case 379: /* sub_search: fusion_expr  */
#line 2878 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7445 "parser.cpp"
    break;

  case 380: /* sub_search_array: sub_search  */
#line 2882 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7454 "parser.cpp"
    break;

  case 381: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2886 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7463 "parser.cpp"
    break;

  case 382: /* function_expr: IDENTIFIER '(' ')'  */
#line 2891 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7476 "parser.cpp"
    break;

  case 383: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2899 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7489 "parser.cpp"
    break;

  case 384: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2907 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7503 "parser.cpp"
    break;

  case 385: /* function_expr: operand IS NOT NULLABLE  */
#line 2916 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7515 "parser.cpp"
    break;

  case 386: /* function_expr: operand IS NULLABLE  */
#line 2923 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7527 "parser.cpp"
    break;

  case 387: /* function_expr: NOT operand  */
#line 2930 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7539 "parser.cpp"
    break;

  case 388: /* function_expr: '-' operand  */
#line 2937 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7551 "parser.cpp"
    break;

  case 389: /* function_expr: '+' operand  */
#line 2944 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7563 "parser.cpp"
    break;

  case 390: /* function_expr: operand '-' operand  */
#line 2951 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7576 "parser.cpp"
    break;

  case 391: /* function_expr: operand '+' operand  */
#line 2959 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7589 "parser.cpp"
    break;

  case 392: /* function_expr: operand '*' operand  */
#line 2967 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7602 "parser.cpp"
    break;

  case 393: /* function_expr: operand '/' operand  */
#line 2975 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7615 "parser.cpp"
    break;

  case 394: /* function_expr: operand '%' operand  */
#line 2983 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7628 "parser.cpp"
    break;

  case 395: /* function_expr: operand '=' operand  */
#line 2991 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7641 "parser.cpp"
    break;

  case 396: /* function_expr: operand EQUAL operand  */
#line 2999 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7654 "parser.cpp"
    break;

  case 397: /* function_expr: operand NOT_EQ operand  */
#line 3007 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7667 "parser.cpp"
    break;

  case 398: /* function_expr: operand '<' operand  */
#line 3015 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7680 "parser.cpp"
    break;

  case 399: /* function_expr: operand '>' operand  */
#line 3023 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7693 "parser.cpp"
    break;

  case 400: /* function_expr: operand LESS_EQ operand  */
#line 3031 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7706 "parser.cpp"
    break;

  case 401: /* function_expr: operand GREATER_EQ operand  */
#line 3039 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7719 "parser.cpp"
    break;

  case 402: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3047 "parser.y"
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
#line 7754 "parser.cpp"
    break;

  case 403: /* function_expr: operand LIKE operand  */
#line 3077 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7767 "parser.cpp"
    break;

  case 404: /* function_expr: operand NOT LIKE operand  */
#line 3085 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7780 "parser.cpp"
    break;

  case 405: /* conjunction_expr: expr AND expr  */
#line 3094 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7793 "parser.cpp"
    break;

  case 406: /* conjunction_expr: expr OR expr  */
#line 3102 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7806 "parser.cpp"
    break;

  case 407: /* between_expr: operand BETWEEN operand AND operand  */
#line 3111 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7818 "parser.cpp"
    break;

  case 408: /* in_expr: operand IN '(' expr_array ')'  */
#line 3119 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7829 "parser.cpp"
    break;

  case 409: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3125 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7840 "parser.cpp"
    break;

  case 410: /* case_expr: CASE expr case_check_array END  */
#line 3132 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7851 "parser.cpp"
    break;

  case 411: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3138 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7863 "parser.cpp"
    break;

  case 412: /* case_expr: CASE case_check_array END  */
#line 3145 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7873 "parser.cpp"
    break;

  case 413: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3150 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7884 "parser.cpp"
    break;

  case 414: /* case_check_array: WHEN expr THEN expr  */
#line 3157 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7896 "parser.cpp"
    break;

  case 415: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3164 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7908 "parser.cpp"
    break;

  case 416: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3172 "parser.y"
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
#line 7939 "parser.cpp"
    break;

  case 417: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3199 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7950 "parser.cpp"
    break;

  case 418: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3205 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7961 "parser.cpp"
    break;

  case 419: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3211 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7973 "parser.cpp"
    break;

  case 420: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3218 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7985 "parser.cpp"
    break;

  case 421: /* column_expr: IDENTIFIER  */
#line 3226 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7997 "parser.cpp"
    break;

  case 422: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3233 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8009 "parser.cpp"
    break;

  case 423: /* column_expr: '*'  */
#line 3240 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8019 "parser.cpp"
    break;

  case 424: /* column_expr: column_expr '.' '*'  */
#line 3245 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8033 "parser.cpp"
    break;

  case 425: /* constant_expr: STRING  */
#line 3255 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8043 "parser.cpp"
    break;

  case 426: /* constant_expr: TRUE  */
#line 3260 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8053 "parser.cpp"
    break;

  case 427: /* constant_expr: FALSE  */
#line 3265 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8063 "parser.cpp"
    break;

  case 428: /* constant_expr: DOUBLE_VALUE  */
#line 3270 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8073 "parser.cpp"
    break;

  case 429: /* constant_expr: LONG_VALUE  */
#line 3275 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8083 "parser.cpp"
    break;

  case 430: /* constant_expr: DATE STRING  */
#line 3280 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8093 "parser.cpp"
    break;

  case 431: /* constant_expr: TIME STRING  */
#line 3285 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8103 "parser.cpp"
    break;

  case 432: /* constant_expr: DATETIME STRING  */
#line 3290 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8113 "parser.cpp"
    break;

  case 433: /* constant_expr: TIMESTAMP STRING  */
#line 3295 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8123 "parser.cpp"
    break;

  case 434: /* constant_expr: INTERVAL interval_expr  */
#line 3300 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8131 "parser.cpp"
    break;

  case 435: /* constant_expr: interval_expr  */
#line 3303 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8139 "parser.cpp"
    break;

  case 436: /* constant_expr: common_array_expr  */
#line 3306 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8147 "parser.cpp"
    break;

  case 437: /* common_array_expr: array_expr  */
#line 3310 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8155 "parser.cpp"
    break;

  case 438: /* common_array_expr: subarray_array_expr  */
#line 3313 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8163 "parser.cpp"
    break;

  case 439: /* common_array_expr: sparse_array_expr  */
#line 3316 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8171 "parser.cpp"
    break;

  case 440: /* common_array_expr: empty_array_expr  */
#line 3319 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8179 "parser.cpp"
    break;

  case 441: /* common_sparse_array_expr: sparse_array_expr  */
#line 3323 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8187 "parser.cpp"
    break;

  case 442: /* common_sparse_array_expr: array_expr  */
#line 3326 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8195 "parser.cpp"
    break;

  case 443: /* common_sparse_array_expr: empty_array_expr  */
#line 3329 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8203 "parser.cpp"
    break;

  case 444: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3333 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8211 "parser.cpp"
    break;

  case 445: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3337 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8221 "parser.cpp"
    break;

  case 446: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3342 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8230 "parser.cpp"
    break;

  case 447: /* sparse_array_expr: long_sparse_array_expr  */
#line 3347 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8238 "parser.cpp"
    break;

  case 448: /* sparse_array_expr: double_sparse_array_expr  */
#line 3350 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8246 "parser.cpp"
    break;

  case 449: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3354 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8254 "parser.cpp"
    break;

  case 450: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3358 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8266 "parser.cpp"
    break;

  case 451: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3365 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8277 "parser.cpp"
    break;

  case 452: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3372 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8285 "parser.cpp"
    break;

  case 453: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3376 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8297 "parser.cpp"
    break;

  case 454: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3383 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8308 "parser.cpp"
    break;

  case 455: /* empty_array_expr: '[' ']'  */
#line 3390 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8316 "parser.cpp"
    break;

  case 456: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3394 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8324 "parser.cpp"
    break;

  case 457: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3398 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8332 "parser.cpp"
    break;

  case 458: /* array_expr: long_array_expr  */
#line 3402 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8340 "parser.cpp"
    break;

  case 459: /* array_expr: double_array_expr  */
#line 3405 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8348 "parser.cpp"
    break;

  case 460: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3409 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8356 "parser.cpp"
    break;

  case 461: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3413 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8366 "parser.cpp"
    break;

  case 462: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3418 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8375 "parser.cpp"
    break;

  case 463: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3423 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8383 "parser.cpp"
    break;

  case 464: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3427 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8393 "parser.cpp"
    break;

  case 465: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3432 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8402 "parser.cpp"
    break;

  case 466: /* interval_expr: LONG_VALUE SECONDS  */
#line 3437 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8413 "parser.cpp"
    break;

  case 467: /* interval_expr: LONG_VALUE SECOND  */
#line 3443 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8424 "parser.cpp"
    break;

  case 468: /* interval_expr: LONG_VALUE MINUTES  */
#line 3449 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8435 "parser.cpp"
    break;

  case 469: /* interval_expr: LONG_VALUE MINUTE  */
#line 3455 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8446 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE HOURS  */
#line 3461 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8457 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE HOUR  */
#line 3467 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8468 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE DAYS  */
#line 3473 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8479 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE DAY  */
#line 3479 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8490 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE MONTHS  */
#line 3485 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8501 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE MONTH  */
#line 3491 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8512 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE YEARS  */
#line 3497 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8523 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE YEAR  */
#line 3503 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8534 "parser.cpp"
    break;

  case 478: /* copy_option_list: copy_option  */
#line 3514 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8543 "parser.cpp"
    break;

  case 479: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3518 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8552 "parser.cpp"
    break;

  case 480: /* copy_option: FORMAT IDENTIFIER  */
#line 3523 "parser.y"
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
#line 8588 "parser.cpp"
    break;

  case 481: /* copy_option: DELIMITER STRING  */
#line 3554 "parser.y"
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
#line 8603 "parser.cpp"
    break;

  case 482: /* copy_option: HEADER  */
#line 3564 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8613 "parser.cpp"
    break;

  case 483: /* copy_option: OFFSET LONG_VALUE  */
#line 3569 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8623 "parser.cpp"
    break;

  case 484: /* copy_option: LIMIT LONG_VALUE  */
#line 3574 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8633 "parser.cpp"
    break;

  case 485: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3579 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8643 "parser.cpp"
    break;

  case 486: /* file_path: STRING  */
#line 3585 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8651 "parser.cpp"
    break;

  case 487: /* if_exists: IF EXISTS  */
#line 3589 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8657 "parser.cpp"
    break;

  case 488: /* if_exists: %empty  */
#line 3590 "parser.y"
  { (yyval.bool_value) = false; }
#line 8663 "parser.cpp"
    break;

  case 489: /* if_not_exists: IF NOT EXISTS  */
#line 3592 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8669 "parser.cpp"
    break;

  case 490: /* if_not_exists: %empty  */
#line 3593 "parser.y"
  { (yyval.bool_value) = false; }
#line 8675 "parser.cpp"
    break;

  case 493: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3608 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8688 "parser.cpp"
    break;

  case 494: /* if_not_exists_info: %empty  */
#line 3616 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8696 "parser.cpp"
    break;

  case 495: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3620 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8704 "parser.cpp"
    break;

  case 496: /* with_index_param_list: %empty  */
#line 3623 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8712 "parser.cpp"
    break;

  case 497: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3627 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8720 "parser.cpp"
    break;

  case 498: /* optional_table_properties_list: %empty  */
#line 3630 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8728 "parser.cpp"
    break;

  case 499: /* index_param_list: index_param  */
#line 3634 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8737 "parser.cpp"
    break;

  case 500: /* index_param_list: index_param_list ',' index_param  */
#line 3638 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8746 "parser.cpp"
    break;

  case 501: /* index_param: IDENTIFIER  */
#line 3643 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8757 "parser.cpp"
    break;

  case 502: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3649 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8772 "parser.cpp"
    break;

  case 503: /* index_param: IDENTIFIER '=' STRING  */
#line 3659 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8787 "parser.cpp"
    break;

  case 504: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3669 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8800 "parser.cpp"
    break;

  case 505: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3677 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8813 "parser.cpp"
    break;

  case 506: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3688 "parser.y"
                                                                       {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "bmp") == 0) {
        index_type = infinity::IndexType::kBMP;
    } else if (strcmp((yyvsp[-1].str_value), "ivf") == 0) {
        index_type = infinity::IndexType::kIVF;
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
#line 8849 "parser.cpp"
    break;

  case 507: /* index_info: '(' IDENTIFIER ')'  */
#line 3719 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8860 "parser.cpp"
    break;


#line 8864 "parser.cpp"

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

#line 3726 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

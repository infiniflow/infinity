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
  YYSYMBOL_SYSTEM = 51,                    /* SYSTEM  */
  YYSYMBOL_GROUP = 52,                     /* GROUP  */
  YYSYMBOL_BY = 53,                        /* BY  */
  YYSYMBOL_HAVING = 54,                    /* HAVING  */
  YYSYMBOL_AS = 55,                        /* AS  */
  YYSYMBOL_NATURAL = 56,                   /* NATURAL  */
  YYSYMBOL_JOIN = 57,                      /* JOIN  */
  YYSYMBOL_LEFT = 58,                      /* LEFT  */
  YYSYMBOL_RIGHT = 59,                     /* RIGHT  */
  YYSYMBOL_OUTER = 60,                     /* OUTER  */
  YYSYMBOL_FULL = 61,                      /* FULL  */
  YYSYMBOL_ON = 62,                        /* ON  */
  YYSYMBOL_INNER = 63,                     /* INNER  */
  YYSYMBOL_CROSS = 64,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 65,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 66,                     /* WHERE  */
  YYSYMBOL_ORDER = 67,                     /* ORDER  */
  YYSYMBOL_LIMIT = 68,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 69,                    /* OFFSET  */
  YYSYMBOL_ASC = 70,                       /* ASC  */
  YYSYMBOL_DESC = 71,                      /* DESC  */
  YYSYMBOL_IF = 72,                        /* IF  */
  YYSYMBOL_NOT = 73,                       /* NOT  */
  YYSYMBOL_EXISTS = 74,                    /* EXISTS  */
  YYSYMBOL_IN = 75,                        /* IN  */
  YYSYMBOL_FROM = 76,                      /* FROM  */
  YYSYMBOL_TO = 77,                        /* TO  */
  YYSYMBOL_WITH = 78,                      /* WITH  */
  YYSYMBOL_DELIMITER = 79,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 80,                    /* FORMAT  */
  YYSYMBOL_HEADER = 81,                    /* HEADER  */
  YYSYMBOL_HIGHLIGHT = 82,                 /* HIGHLIGHT  */
  YYSYMBOL_CAST = 83,                      /* CAST  */
  YYSYMBOL_END = 84,                       /* END  */
  YYSYMBOL_CASE = 85,                      /* CASE  */
  YYSYMBOL_ELSE = 86,                      /* ELSE  */
  YYSYMBOL_THEN = 87,                      /* THEN  */
  YYSYMBOL_WHEN = 88,                      /* WHEN  */
  YYSYMBOL_UNNEST = 89,                    /* UNNEST  */
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
  YYSYMBOL_ARRAY = 119,                    /* ARRAY  */
  YYSYMBOL_TUPLE = 120,                    /* TUPLE  */
  YYSYMBOL_EMBEDDING = 121,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 122,                   /* VECTOR  */
  YYSYMBOL_MULTIVECTOR = 123,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 124,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 125,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 126,              /* TENSORARRAY  */
  YYSYMBOL_BIT = 127,                      /* BIT  */
  YYSYMBOL_TEXT = 128,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 129,                  /* PRIMARY  */
  YYSYMBOL_KEY = 130,                      /* KEY  */
  YYSYMBOL_UNIQUE = 131,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 132,                 /* NULLABLE  */
  YYSYMBOL_IS = 133,                       /* IS  */
  YYSYMBOL_DEFAULT = 134,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 135,                  /* COMMENT  */
  YYSYMBOL_IGNORE = 136,                   /* IGNORE  */
  YYSYMBOL_TRUE = 137,                     /* TRUE  */
  YYSYMBOL_FALSE = 138,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 139,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 140,                   /* SECOND  */
  YYSYMBOL_SECONDS = 141,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 142,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 143,                  /* MINUTES  */
  YYSYMBOL_HOUR = 144,                     /* HOUR  */
  YYSYMBOL_HOURS = 145,                    /* HOURS  */
  YYSYMBOL_DAY = 146,                      /* DAY  */
  YYSYMBOL_DAYS = 147,                     /* DAYS  */
  YYSYMBOL_MONTH = 148,                    /* MONTH  */
  YYSYMBOL_MONTHS = 149,                   /* MONTHS  */
  YYSYMBOL_YEAR = 150,                     /* YEAR  */
  YYSYMBOL_YEARS = 151,                    /* YEARS  */
  YYSYMBOL_EQUAL = 152,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 153,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 154,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 155,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 156,                  /* BETWEEN  */
  YYSYMBOL_AND = 157,                      /* AND  */
  YYSYMBOL_OR = 158,                       /* OR  */
  YYSYMBOL_EXTRACT = 159,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 160,                     /* LIKE  */
  YYSYMBOL_DATA = 161,                     /* DATA  */
  YYSYMBOL_LOG = 162,                      /* LOG  */
  YYSYMBOL_BUFFER = 163,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 164,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 165,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 166,                 /* MEMINDEX  */
  YYSYMBOL_USING = 167,                    /* USING  */
  YYSYMBOL_SESSION = 168,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 169,                   /* GLOBAL  */
  YYSYMBOL_OFF = 170,                      /* OFF  */
  YYSYMBOL_EXPORT = 171,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 172,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 173,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 174,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 175,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 176,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 177,                    /* DELTA  */
  YYSYMBOL_LOGS = 178,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 179,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 180,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 181,                   /* SEARCH  */
  YYSYMBOL_MATCH = 182,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 183,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 184,                    /* QUERY  */
  YYSYMBOL_QUERIES = 185,                  /* QUERIES  */
  YYSYMBOL_FUSION = 186,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 187,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 188,                    /* ADMIN  */
  YYSYMBOL_LEADER = 189,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 190,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 191,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 192,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 193,               /* STANDALONE  */
  YYSYMBOL_NODES = 194,                    /* NODES  */
  YYSYMBOL_NODE = 195,                     /* NODE  */
  YYSYMBOL_REMOVE = 196,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 197,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 198,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 199,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 200,                  /* RESTORE  */
  YYSYMBOL_PERSISTENCE = 201,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 202,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 203,                  /* OBJECTS  */
  YYSYMBOL_FILES = 204,                    /* FILES  */
  YYSYMBOL_MEMORY = 205,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 206,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 207,                  /* HISTORY  */
  YYSYMBOL_CURRENTDATE = 208,              /* CURRENTDATE  */
  YYSYMBOL_NUMBER = 209,                   /* NUMBER  */
  YYSYMBOL_210_ = 210,                     /* '='  */
  YYSYMBOL_211_ = 211,                     /* '<'  */
  YYSYMBOL_212_ = 212,                     /* '>'  */
  YYSYMBOL_213_ = 213,                     /* '+'  */
  YYSYMBOL_214_ = 214,                     /* '-'  */
  YYSYMBOL_215_ = 215,                     /* '*'  */
  YYSYMBOL_216_ = 216,                     /* '/'  */
  YYSYMBOL_217_ = 217,                     /* '%'  */
  YYSYMBOL_218_ = 218,                     /* '['  */
  YYSYMBOL_219_ = 219,                     /* ']'  */
  YYSYMBOL_220_ = 220,                     /* '('  */
  YYSYMBOL_221_ = 221,                     /* ')'  */
  YYSYMBOL_222_ = 222,                     /* '.'  */
  YYSYMBOL_223_ = 223,                     /* ';'  */
  YYSYMBOL_224_ = 224,                     /* ','  */
  YYSYMBOL_225_ = 225,                     /* '}'  */
  YYSYMBOL_226_ = 226,                     /* '{'  */
  YYSYMBOL_227_ = 227,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 228,                 /* $accept  */
  YYSYMBOL_input_pattern = 229,            /* input_pattern  */
  YYSYMBOL_statement_list = 230,           /* statement_list  */
  YYSYMBOL_statement = 231,                /* statement  */
  YYSYMBOL_explainable_statement = 232,    /* explainable_statement  */
  YYSYMBOL_create_statement = 233,         /* create_statement  */
  YYSYMBOL_table_element_array = 234,      /* table_element_array  */
  YYSYMBOL_column_def_array = 235,         /* column_def_array  */
  YYSYMBOL_table_element = 236,            /* table_element  */
  YYSYMBOL_table_column = 237,             /* table_column  */
  YYSYMBOL_column_type_array = 238,        /* column_type_array  */
  YYSYMBOL_column_type = 239,              /* column_type  */
  YYSYMBOL_column_constraints = 240,       /* column_constraints  */
  YYSYMBOL_column_constraint = 241,        /* column_constraint  */
  YYSYMBOL_default_expr = 242,             /* default_expr  */
  YYSYMBOL_table_constraint = 243,         /* table_constraint  */
  YYSYMBOL_identifier_array = 244,         /* identifier_array  */
  YYSYMBOL_delete_statement = 245,         /* delete_statement  */
  YYSYMBOL_insert_statement = 246,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 247, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 248,        /* explain_statement  */
  YYSYMBOL_update_statement = 249,         /* update_statement  */
  YYSYMBOL_update_expr_array = 250,        /* update_expr_array  */
  YYSYMBOL_update_expr = 251,              /* update_expr  */
  YYSYMBOL_drop_statement = 252,           /* drop_statement  */
  YYSYMBOL_copy_statement = 253,           /* copy_statement  */
  YYSYMBOL_select_statement = 254,         /* select_statement  */
  YYSYMBOL_select_with_paren = 255,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 256,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 257, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 258, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 259, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 260,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 261,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 262,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 263,            /* order_by_type  */
  YYSYMBOL_limit_expr = 264,               /* limit_expr  */
  YYSYMBOL_offset_expr = 265,              /* offset_expr  */
  YYSYMBOL_distinct = 266,                 /* distinct  */
  YYSYMBOL_unnest_clause = 267,            /* unnest_clause  */
  YYSYMBOL_highlight_clause = 268,         /* highlight_clause  */
  YYSYMBOL_from_clause = 269,              /* from_clause  */
  YYSYMBOL_search_clause = 270,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 271, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 272,             /* where_clause  */
  YYSYMBOL_having_clause = 273,            /* having_clause  */
  YYSYMBOL_group_by_clause = 274,          /* group_by_clause  */
  YYSYMBOL_set_operator = 275,             /* set_operator  */
  YYSYMBOL_table_reference = 276,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 277,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 278,     /* table_reference_name  */
  YYSYMBOL_table_name = 279,               /* table_name  */
  YYSYMBOL_table_alias = 280,              /* table_alias  */
  YYSYMBOL_with_clause = 281,              /* with_clause  */
  YYSYMBOL_with_expr_list = 282,           /* with_expr_list  */
  YYSYMBOL_with_expr = 283,                /* with_expr  */
  YYSYMBOL_join_clause = 284,              /* join_clause  */
  YYSYMBOL_join_type = 285,                /* join_type  */
  YYSYMBOL_show_statement = 286,           /* show_statement  */
  YYSYMBOL_flush_statement = 287,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 288,       /* optimize_statement  */
  YYSYMBOL_command_statement = 289,        /* command_statement  */
  YYSYMBOL_compact_statement = 290,        /* compact_statement  */
  YYSYMBOL_admin_statement = 291,          /* admin_statement  */
  YYSYMBOL_alter_statement = 292,          /* alter_statement  */
  YYSYMBOL_expr_array = 293,               /* expr_array  */
  YYSYMBOL_insert_row_list = 294,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 295,               /* expr_alias  */
  YYSYMBOL_expr = 296,                     /* expr  */
  YYSYMBOL_operand = 297,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 298,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 299,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 300,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 301,          /* match_text_expr  */
  YYSYMBOL_query_expr = 302,               /* query_expr  */
  YYSYMBOL_fusion_expr = 303,              /* fusion_expr  */
  YYSYMBOL_sub_search = 304,               /* sub_search  */
  YYSYMBOL_sub_search_array = 305,         /* sub_search_array  */
  YYSYMBOL_function_expr = 306,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 307,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 308,             /* between_expr  */
  YYSYMBOL_in_expr = 309,                  /* in_expr  */
  YYSYMBOL_case_expr = 310,                /* case_expr  */
  YYSYMBOL_case_check_array = 311,         /* case_check_array  */
  YYSYMBOL_cast_expr = 312,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 313,            /* subquery_expr  */
  YYSYMBOL_column_expr = 314,              /* column_expr  */
  YYSYMBOL_constant_expr = 315,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 316,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 317, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 318,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 319, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 320,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 321,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 322, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 323, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 324, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 325,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 326,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 327, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 328,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 329,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 330,               /* array_expr  */
  YYSYMBOL_long_array_expr = 331,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 332, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 333,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 334, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 335,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 336,         /* copy_option_list  */
  YYSYMBOL_copy_option = 337,              /* copy_option  */
  YYSYMBOL_file_path = 338,                /* file_path  */
  YYSYMBOL_if_exists = 339,                /* if_exists  */
  YYSYMBOL_if_not_exists = 340,            /* if_not_exists  */
  YYSYMBOL_semicolon = 341,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 342,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 343,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 344, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 345,         /* index_param_list  */
  YYSYMBOL_index_param = 346,              /* index_param  */
  YYSYMBOL_index_info = 347                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 476 "parser.cpp"

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
#define YYFINAL  127
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1578

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  228
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  552
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1256

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   464


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
       2,     2,     2,     2,     2,     2,     2,   217,     2,     2,
     220,   221,   215,   213,   224,   214,   222,   216,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   227,   223,
     211,   210,   212,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   218,     2,   219,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   226,     2,   225,     2,     2,     2,     2,
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
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   517,   517,   521,   527,   534,   535,   536,   537,   538,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,   561,   568,   585,   602,   618,   647,   662,   694,   712,
     730,   758,   789,   793,   798,   802,   808,   811,   818,   838,
     860,   884,   910,   914,   920,   921,   922,   923,   924,   925,
     926,   927,   928,   929,   930,   931,   932,   933,   934,   935,
     936,   937,   938,   939,   940,   943,   945,   946,   947,   948,
     951,   952,   953,   954,   955,   956,   957,   958,   959,   960,
     961,   962,   963,   964,   965,   966,   967,   968,   969,   970,
     971,   972,   973,   974,   975,   976,   977,   978,   979,   980,
     981,   982,   983,   984,   985,   986,   987,   988,   989,   990,
     991,   992,   993,   994,   995,   996,   997,   998,   999,  1000,
    1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,
    1011,  1012,  1013,  1014,  1015,  1016,  1017,  1021,  1044,  1048,
    1058,  1061,  1064,  1067,  1071,  1074,  1079,  1084,  1091,  1097,
    1107,  1123,  1161,  1177,  1180,  1187,  1199,  1208,  1221,  1225,
    1230,  1243,  1256,  1271,  1286,  1301,  1324,  1377,  1432,  1483,
    1486,  1489,  1498,  1508,  1511,  1515,  1520,  1547,  1550,  1555,
    1573,  1576,  1580,  1584,  1589,  1595,  1598,  1601,  1605,  1609,
    1611,  1615,  1617,  1620,  1624,  1627,  1631,  1634,  1638,  1641,
    1645,  1650,  1654,  1657,  1661,  1664,  1668,  1671,  1675,  1678,
    1682,  1685,  1688,  1691,  1699,  1702,  1717,  1717,  1719,  1733,
    1742,  1747,  1756,  1761,  1766,  1772,  1779,  1782,  1786,  1789,
    1794,  1806,  1813,  1827,  1830,  1833,  1836,  1839,  1842,  1845,
    1851,  1855,  1859,  1863,  1867,  1874,  1878,  1882,  1886,  1890,
    1895,  1899,  1904,  1908,  1912,  1916,  1922,  1928,  1934,  1945,
    1956,  1967,  1979,  1991,  2004,  2018,  2029,  2043,  2059,  2076,
    2080,  2084,  2088,  2092,  2096,  2102,  2106,  2110,  2114,  2120,
    2124,  2134,  2138,  2142,  2150,  2161,  2184,  2190,  2195,  2201,
    2207,  2215,  2221,  2227,  2233,  2239,  2247,  2253,  2259,  2265,
    2271,  2279,  2285,  2291,  2300,  2309,  2317,  2325,  2331,  2337,
    2343,  2350,  2363,  2367,  2372,  2378,  2385,  2393,  2402,  2412,
    2422,  2433,  2444,  2456,  2468,  2478,  2489,  2501,  2514,  2518,
    2523,  2528,  2534,  2538,  2542,  2548,  2552,  2556,  2562,  2568,
    2576,  2582,  2586,  2592,  2596,  2602,  2607,  2612,  2619,  2628,
    2638,  2647,  2659,  2675,  2679,  2684,  2694,  2716,  2722,  2726,
    2727,  2728,  2729,  2730,  2732,  2735,  2741,  2744,  2745,  2746,
    2747,  2748,  2749,  2750,  2751,  2752,  2753,  2757,  2773,  2790,
    2808,  2854,  2893,  2936,  2983,  3007,  3030,  3051,  3072,  3081,
    3092,  3103,  3117,  3124,  3134,  3140,  3152,  3155,  3158,  3161,
    3164,  3167,  3171,  3175,  3180,  3188,  3196,  3205,  3211,  3217,
    3224,  3231,  3238,  3245,  3252,  3259,  3266,  3273,  3280,  3287,
    3294,  3302,  3310,  3318,  3326,  3334,  3342,  3350,  3358,  3366,
    3374,  3382,  3390,  3420,  3428,  3437,  3445,  3454,  3462,  3468,
    3475,  3481,  3488,  3493,  3500,  3507,  3515,  3528,  3534,  3540,
    3547,  3555,  3562,  3569,  3574,  3584,  3589,  3594,  3599,  3604,
    3609,  3614,  3619,  3624,  3629,  3632,  3635,  3638,  3642,  3645,
    3648,  3651,  3655,  3658,  3661,  3665,  3669,  3674,  3679,  3682,
    3686,  3690,  3697,  3704,  3708,  3715,  3722,  3726,  3729,  3733,
    3737,  3742,  3746,  3750,  3753,  3757,  3761,  3766,  3771,  3775,
    3780,  3785,  3791,  3797,  3803,  3809,  3815,  3821,  3827,  3833,
    3839,  3845,  3851,  3862,  3866,  3871,  3902,  3912,  3917,  3922,
    3927,  3933,  3937,  3938,  3940,  3941,  3943,  3944,  3956,  3964,
    3968,  3971,  3975,  3978,  3982,  3986,  3991,  3997,  4007,  4017,
    4025,  4036,  4067
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
  "COLUMN", "COLUMNS", "INDEXES", "CHUNK", "SYSTEM", "GROUP", "BY",
  "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL",
  "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET",
  "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH",
  "DELIMITER", "FORMAT", "HEADER", "HIGHLIGHT", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "UNNEST", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED",
  "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON",
  "CIRCLE", "BLOB", "BITMAP", "ARRAY", "TUPLE", "EMBEDDING", "VECTOR",
  "MULTIVECTOR", "TENSOR", "SPARSE", "TENSORARRAY", "BIT", "TEXT",
  "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "COMMENT",
  "IGNORE", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "TRANSACTIONS",
  "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT",
  "CONFIGS", "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "DELTA",
  "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY",
  "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER",
  "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE", "REMOVE",
  "SNAPSHOT", "SNAPSHOTS", "RECOVER", "RESTORE", "PERSISTENCE", "OBJECT",
  "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "HISTORY", "CURRENTDATE",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "'}'", "'{'", "':'", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "column_def_array",
  "table_element", "table_column", "column_type_array", "column_type",
  "column_constraints", "column_constraint", "default_expr",
  "table_constraint", "identifier_array", "delete_statement",
  "insert_statement", "optional_identifier_array", "explain_statement",
  "update_statement", "update_expr_array", "update_expr", "drop_statement",
  "copy_statement", "select_statement", "select_with_paren",
  "select_without_paren", "select_clause_with_modifier",
  "select_clause_without_modifier_paren", "select_clause_without_modifier",
  "order_by_clause", "order_by_expr_list", "order_by_expr",
  "order_by_type", "limit_expr", "offset_expr", "distinct",
  "unnest_clause", "highlight_clause", "from_clause", "search_clause",
  "optional_search_filter_expr", "where_clause", "having_clause",
  "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "flush_statement",
  "optimize_statement", "command_statement", "compact_statement",
  "admin_statement", "alter_statement", "expr_array", "insert_row_list",
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
  "empty_array_expr", "curly_brackets_expr",
  "unclosed_curly_brackets_expr", "int_sparse_ele", "float_sparse_ele",
  "array_expr", "long_array_expr", "unclosed_long_array_expr",
  "double_array_expr", "unclosed_double_array_expr", "interval_expr",
  "copy_option_list", "copy_option", "file_path", "if_exists",
  "if_not_exists", "semicolon", "if_not_exists_info",
  "with_index_param_list", "optional_table_properties_list",
  "index_param_list", "index_param", "index_info", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-780)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-540)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     947,    98,    16,   303,    62,    -3,    62,   274,   911,   896,
      77,   134,   164,   174,   236,   123,   230,   281,    94,   197,
     422,   -48,    88,   155,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,   396,  -780,  -780,   379,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,   333,   333,   333,   333,    23,   408,    62,
     350,   350,   350,   350,   350,   412,   250,   464,    62,   -25,
     471,   483,   493,   985,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,   396,  -780,  -780,  -780,  -780,  -780,   260,   509,    62,
    -780,  -780,  -780,  -780,  -780,    25,  -780,   355,   465,  -780,
     511,  -780,   341,  -780,  -780,   530,  -780,   543,  -780,   487,
     -96,    62,    62,    62,    62,  -780,  -780,  -780,  -780,   -36,
    -780,   518,   360,  -780,   573,   389,   403,   328,   993,   416,
     616,   427,   548,   429,   431,   409,   434,  -780,   812,  -780,
     639,  -780,  -780,    11,   599,  -780,   603,   601,   673,    62,
      62,    62,   683,   638,   642,   490,   640,   709,    62,    62,
      62,   713,  -780,   723,   724,   662,   725,   725,   529,    87,
     103,   122,  -780,   513,  -780,   449,  -780,  -780,  -780,   727,
    -780,   729,  -780,  -780,  -780,  -780,   731,  -780,  -780,  -780,
    -780,    53,  -780,  -780,  -780,    62,   516,   281,   725,  -780,
     734,  -780,   572,  -780,   735,  -780,  -780,   737,  -780,  -780,
     739,  -780,   742,   745,  -780,   749,   699,   765,   577,   780,
     781,  -780,  -780,  -780,  -780,    11,  -780,  -780,  -780,   529,
     736,   732,   717,   658,   -41,  -780,   490,  -780,    62,    48,
     792,   276,  -780,  -780,  -780,  -780,  -780,   746,  -780,   587,
     -34,  -780,   529,  -780,  -780,   721,   733,   581,  -780,  -780,
     617,   648,   593,   594,    97,   811,   816,   827,   829,  -780,
    -780,   833,   624,   625,   626,   627,   628,   629,   631,   336,
     633,   634,   635,   767,   767,  -780,    43,   479,   151,    85,
    -780,   -22,   967,  -780,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,   636,  -780,  -780,  -780,
     162,  -780,  -780,   176,  -780,   210,  -780,  -780,   366,  -780,
    -780,   257,  -780,   275,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,   854,   853,  -780,  -780,  -780,  -780,  -780,  -780,   818,
     820,   795,   798,   379,  -780,  -780,  -780,   869,    34,  -780,
     871,  -780,  -780,   801,   370,  -780,   875,  -780,  -780,   659,
     660,   -32,   529,   529,   813,  -780,   880,   -48,    52,   830,
     666,   884,   885,  -780,  -780,    92,   669,  -780,    62,   529,
     724,  -780,   477,   671,   672,   329,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,   767,   674,
     999,   815,   529,   529,     1,   194,  -780,  -780,  -780,  -780,
     617,  -780,   529,   529,   529,   529,   529,   529,   891,   676,
     677,   678,   680,   897,   898,   682,   323,   323,  -780,   681,
    -780,  -780,  -780,  -780,   689,   -71,  -780,  -780,   835,   529,
     913,   529,   529,   -38,   708,   139,   767,   767,   767,   767,
     767,   767,   767,   767,   767,   767,   767,   767,   767,   767,
      36,  -780,   701,  -780,   925,  -780,   927,   183,  -780,  -780,
     933,  -780,   938,   901,   602,   726,   728,   942,   730,  -780,
     744,  -780,   946,  -780,   315,   951,   799,   800,  -780,  -780,
    -780,   529,   892,   750,  -780,   -13,   477,   529,  -780,  -780,
      56,  1127,   839,   753,   138,  -780,  -780,  -780,   -48,   976,
     849,  -780,  -780,  -780,   983,   529,   764,  -780,   477,  -780,
     195,   195,   529,  -780,   214,   815,   831,   769,   -11,   -53,
     337,  -780,   529,   529,    49,   109,   150,   187,   189,   192,
     926,   529,    37,   529,  1000,   779,   241,  -780,   690,  -780,
    -780,   725,  -780,  -780,  -780,   848,   786,   767,   479,   879,
    -780,   565,   565,   237,   237,   984,   565,   565,   237,   237,
     323,   323,  -780,  -780,  -780,  -780,  -780,  -780,   790,  -780,
     791,  -780,  -780,  -780,  -780,  1007,  1013,  -780,   792,  1017,
    -780,  1018,  -780,  -780,  1016,  -780,  -780,  1019,  1020,   806,
      10,   944,   529,  -780,  -780,  -780,   477,  1027,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,   817,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,   819,   821,   823,   824,   825,   826,   828,   832,
     288,   834,   792,  1003,    52,   396,   837,  1032,  -780,   266,
     836,  1041,  1043,  1034,  1047,  -780,  1045,   324,  -780,   330,
     335,  -780,   845,  -780,  1127,   529,  -780,   529,   -60,   -16,
    -780,  -780,  -780,  -780,  -780,  -780,   767,   141,  -780,  -128,
     143,   843,   130,   850,  -780,  1049,  -780,  -780,   977,   479,
     565,   855,   354,  -780,   767,  1071,  1073,  1029,  1036,   356,
     374,  -780,   882,   375,  -780,  1090,  -780,  -780,   -48,   878,
     446,  -780,    66,  -780,   529,   917,  -780,  -780,  1093,  1127,
    1127,   551,   711,  1163,  1180,  1203,  1220,   971,   974,  -780,
    -780,   224,  -780,   972,   792,   380,   887,   970,  -780,   941,
    -780,  -780,   529,  -780,  -780,  -780,  -780,  -780,  -780,   195,
    -780,  -780,  -780,   888,   477,   -39,  -780,   529,   874,   894,
     701,   895,  1106,   890,   529,  -780,   903,   905,   907,   384,
    -780,  -780,   999,  1109,  1110,  -780,  -780,  1017,   523,  -780,
    1018,   293,    44,    10,  1069,  -780,  -780,  -780,  -780,  -780,
    -780,  1072,  -780,  1130,  -780,   806,   338,   662,   385,   909,
     386,  -780,   910,   916,   918,   919,   921,   922,   924,   934,
     936,  1056,   937,   939,   940,   950,   952,   962,   965,   968,
     969,   978,  1057,   979,   980,   982,  1010,  1021,  1035,  1039,
    1040,  1042,  1052,  1064,  1053,  1058,  1059,  1060,  1065,  1067,
    1068,  1075,  1076,  1079,  1077,  1080,  1081,  1082,  1092,  1096,
    1097,  1098,  1099,  1100,  1104,  1114,  1105,  1107,  1108,  1111,
    1112,  1113,  1115,  1116,  1117,  1118,  1148,  1119,  -780,  -780,
     183,  -780,  1031,  1125,   415,  -780,  1018,  1158,  1172,   438,
    -780,  -780,  -780,   477,  -780,   697,  1120,  1121,    30,  1122,
    -780,  -780,  -780,  1124,  1142,  1023,   477,  -780,   195,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  1236,
    -780,    66,   446,    10,    10,  1061,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  1126,  1213,  -780,  1261,  -780,  -780,  1127,
    1274,  1282,  1287,  1311,  1327,  1328,  1332,  1343,  1345,  1128,
    1347,  1348,  1349,  1350,  1351,  1352,  1353,  1354,  1355,  1356,
    1139,  1358,  1359,  1360,  1361,  1362,  1363,  1364,  1365,  1366,
    1367,  1150,  1369,  1370,  1371,  1372,  1373,  1374,  1375,  1376,
    1377,  1378,  1161,  1380,  1381,  1382,  1383,  1384,  1385,  1386,
    1387,  1388,  1389,  1173,  1390,  1392,  1393,  1394,  1395,  1396,
    1397,  1398,  1399,  1400,  1183,  1402,  -780,  1405,  1406,  -780,
     459,  -780,   798,  -780,  -780,  1407,  1408,  1409,   265,  1193,
    -780,   460,  1410,  -780,  -780,  1357,   792,   338,  1368,  1379,
    1194,  -780,  1196,  1197,  1199,  1201,  1202,  1204,  1205,  1206,
    1207,  1418,  1208,  1209,  1210,  1211,  1214,  1215,  1216,  1217,
    1218,  1219,  1428,  1221,  1222,  1223,  1224,  1225,  1226,  1227,
    1228,  1229,  1230,  1435,  1231,  1232,  1233,  1234,  1235,  1237,
    1238,  1239,  1240,  1241,  1451,  1242,  1243,  1244,  1245,  1246,
    1247,  1248,  1249,  1250,  1251,  1467,  1253,  1254,  1255,  1256,
    1257,  1258,  1259,  1260,  1262,  1263,  1476,  1264,  -780,  -780,
    -780,  -780,  1265,  1266,  1267,   890,  1319,  -780,   545,   529,
     461,  -780,   529,   529,  -780,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  1271,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  1272,  -780,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  1273,  -780,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  1275,  -780,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  1276,
    -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,  -780,
    1277,  -780,  1483,  1484,    54,  1278,  1279,  1446,  1495,  1496,
    -780,  -780,  -780,   477,  -780,   806,   477,  -780,  -780,  -780,
    -780,  -780,  -780,  1280,  1281,   890,   798,  1336,  1286,  1501,
     661,   163,  1288,  1504,  1289,  -780,  1462,  1508,   665,  1507,
    -780,   890,   798,   890,   -24,  1294,  1295,  1509,  -780,  1469,
    1297,  -780,  1298,  1480,  1481,  -780,  1518,  -780,  -780,  -780,
     -19,   -90,  -780,  1303,  1304,  1486,  1487,  -780,  1488,  1489,
    1521,  -780,  -780,  1310,  -780,  1312,  1313,  1528,  1530,   798,
    1314,  1315,  -780,   798,  -780,  -780
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     237,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   237,     0,   537,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   180,   179,     0,     8,    14,    15,    16,
      17,    18,    19,   535,   535,   535,   535,   535,     0,     0,
     533,   533,   533,   533,   533,     0,   230,     0,     0,     0,
       0,     0,     0,   237,   166,    20,    25,    27,    26,    21,
      22,    24,    23,    28,    29,    30,    31,     0,     0,     0,
     251,   252,   250,   256,   260,     0,   257,     0,     0,   253,
       0,   255,     0,   279,   281,     0,   258,     0,   289,     0,
     285,     0,     0,     0,     0,   291,   292,   293,   296,   230,
     294,     0,   236,   238,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,   237,     2,
     220,   222,   223,     0,   203,   185,   191,     0,     0,     0,
       0,     0,     0,     0,     0,   164,     0,     0,     0,     0,
       0,     0,   318,     0,     0,   215,     0,     0,     0,     0,
       0,     0,   165,     0,   267,   268,   261,   262,   263,     0,
     264,     0,   254,   280,   259,   290,     0,   283,   282,   286,
     287,     0,   321,   313,   314,     0,     0,     0,     0,   345,
       0,   355,     0,   356,     0,   342,   343,     0,   338,   322,
       0,   351,   353,     0,   346,     0,     0,     0,     0,     0,
       0,   184,   183,     4,   221,     0,   181,   182,   202,     0,
       0,   199,     0,    33,     0,    34,   164,   538,     0,     0,
       0,   237,   532,   171,   173,   172,   174,     0,   231,     0,
     215,   168,     0,   160,   531,     0,     0,   461,   465,   468,
     469,     0,     0,     0,     0,     0,     0,     0,     0,   466,
     467,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   418,     0,     0,   463,     0,   237,     0,     0,
     363,   368,   369,   383,   381,   384,   382,   385,   386,   378,
     373,   372,   371,   379,   380,   370,   377,   376,   476,   479,
       0,   480,   488,     0,   489,     0,   481,   477,     0,   478,
     503,     0,   504,     0,   475,   300,   302,   301,   298,   299,
     305,   307,   306,   303,   304,   310,   312,   311,   308,   309,
     288,     0,     0,   270,   269,   275,   265,   266,   284,     0,
       0,     0,   541,     0,   239,   297,   348,     0,   339,   344,
     323,   352,   347,     0,     0,   354,     0,   319,   320,     0,
       0,   207,     0,     0,   201,   534,     0,   237,     0,     0,
       0,     0,     0,   317,   158,     0,     0,   162,     0,     0,
       0,   167,   214,     0,     0,     0,   512,   511,   514,   513,
     516,   515,   518,   517,   520,   519,   522,   521,     0,     0,
     427,   237,     0,     0,     0,     0,   470,   471,   472,   473,
       0,   474,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   429,   428,   509,   506,
     496,   486,   491,   494,     0,     0,   498,   499,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   485,     0,   490,     0,   493,     0,     0,   497,   505,
       0,   508,     0,   276,   271,     0,     0,     0,     0,   295,
       0,   357,     0,   340,     0,     0,     0,     0,   350,   188,
     187,     0,   209,   190,   192,   197,   198,     0,   186,    32,
      36,     0,     0,     0,     0,    42,    46,    47,   237,     0,
      40,   316,   315,   163,     0,     0,   161,   175,   170,   169,
       0,     0,     0,   414,     0,   237,     0,     0,     0,     0,
       0,   452,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   213,     0,   417,     0,   375,
     374,     0,   364,   367,   445,   446,     0,     0,   237,     0,
     426,   436,   437,   440,   441,     0,   443,   435,   438,   439,
     431,   430,   432,   433,   434,   462,   464,   487,     0,   492,
       0,   495,   500,   507,   510,     0,     0,   272,     0,     0,
     360,     0,   240,   341,     0,   324,   349,     0,     0,   206,
       0,   205,     0,   195,   196,   194,   200,     0,    54,    57,
      58,    55,    56,    59,    60,    76,    61,    63,    62,    79,
      66,    67,    68,    64,    65,    69,    70,    71,    72,    73,
      74,    75,     0,     0,     0,     0,     0,     0,     0,     0,
     541,     0,     0,   543,     0,    39,     0,     0,   159,     0,
       0,     0,     0,     0,     0,   527,     0,     0,   523,     0,
       0,   415,     0,   457,     0,     0,   450,     0,     0,     0,
     424,   423,   422,   421,   420,   419,     0,     0,   461,     0,
       0,     0,     0,     0,   404,     0,   502,   501,     0,   237,
     444,     0,     0,   425,     0,     0,     0,   277,   273,     0,
       0,    44,   546,     0,   544,   325,   358,   359,   237,   208,
     224,   226,   235,   227,     0,   211,   193,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   151,
     152,   155,   148,   155,     0,     0,     0,    35,    43,   552,
      41,   365,     0,   529,   528,   526,   525,   530,   178,     0,
     176,   416,   458,     0,   454,     0,   453,     0,     0,     0,
       0,     0,     0,   213,     0,   402,     0,     0,     0,     0,
     459,   448,   447,     0,     0,   362,   361,     0,     0,   540,
       0,     0,     0,     0,     0,   244,   245,   246,   247,   243,
     248,     0,   233,     0,   228,   204,     0,   215,     0,     0,
       0,    52,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   153,   150,
       0,   149,    49,    48,     0,   157,     0,     0,     0,     0,
     524,   456,   451,   455,   442,     0,     0,     0,     0,     0,
     482,   484,   483,   213,     0,     0,   212,   405,     0,   460,
     449,   278,   274,    45,   547,   548,   550,   549,   545,     0,
     326,   235,   225,     0,     0,   232,   408,   406,   409,   407,
     410,   411,   412,   210,   219,    78,     0,   146,   147,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   154,     0,     0,   156,
       0,    37,   541,   366,   506,     0,     0,     0,     0,     0,
     403,     0,   327,   229,   241,     0,     0,     0,     0,   217,
       0,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    51,    50,
     542,   551,     0,     0,   213,   213,   398,   177,     0,     0,
       0,   413,     0,     0,   189,    77,    83,    84,    81,    82,
      85,    86,    87,    88,    89,     0,    80,   127,   128,   125,
     126,   129,   130,   131,   132,   133,     0,   124,    94,    95,
      92,    93,    96,    97,    98,    99,   100,     0,    91,   105,
     106,   103,   104,   107,   108,   109,   110,   111,     0,   102,
     138,   139,   136,   137,   140,   141,   142,   143,   144,     0,
     135,   116,   117,   114,   115,   118,   119,   120,   121,   122,
       0,   113,     0,     0,     0,     0,     0,     0,     0,     0,
     329,   328,   334,   242,   234,   218,   216,    90,   134,   101,
     112,   145,   123,   213,     0,   213,   541,   399,     0,   335,
     330,     0,     0,     0,     0,   397,     0,     0,     0,     0,
     331,   213,   541,   213,   541,     0,     0,     0,   336,   332,
       0,   393,     0,     0,     0,   396,     0,   400,   337,   333,
     541,   387,   395,     0,     0,     0,     0,   392,     0,     0,
       0,   401,   391,     0,   389,     0,     0,     0,     0,   541,
       0,     0,   394,   541,   388,   390
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -780,  -780,  -780,  1411,  1474,    67,  -780,  -780,   899,  -567,
    -780,  -652,  -780,   807,   808,  -780,  -580,   310,   311,  1316,
    -780,   316,  -780,  1160,   317,   320,    -7,  1523,   -17,  1212,
    1330,   -86,  -780,  -780,   945,  -780,  -780,  -780,  -780,  -780,
    -780,  -780,  -780,  -754,  -234,  -780,  -780,  -780,  -780,   763,
    -190,    12,   637,  -780,  -780,  1391,  -780,  -780,   343,   344,
     364,   367,   377,  -780,  -780,  -219,  -780,  1123,  -239,  -231,
    -779,  -775,  -773,  -771,  -769,  -767,   532,  -780,  -780,  -780,
    -780,  -780,  -780,  1146,  -780,  -780,  1009,  -273,  -269,  -780,
    -780,  -780,   793,  -780,  -780,  -780,  -780,   796,  -780,  -780,
    1088,  1087,  -315,  -780,  -780,  -780,  -780,  1299,  -510,   809,
    -147,   514,   525,  -780,  -780,  -638,  -780,   685,   776,  -780
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    22,    23,    24,    64,    25,   504,   700,   505,   506,
     800,   640,   731,   732,   872,   507,   375,    26,    27,   231,
      28,    29,   240,   241,    30,    31,    32,    33,    34,   135,
     216,   136,   221,   493,   494,   605,   364,   498,   219,   715,
     492,   601,   797,   683,   243,  1104,  1019,   133,   709,   710,
     711,   712,   794,    35,   112,   113,   713,   791,    36,    37,
      38,    39,    40,    41,    42,   279,   516,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   922,   923,   289,   290,
     291,   292,   293,   405,   294,   295,   296,   297,   298,   889,
     299,   300,   301,   302,   303,   304,   305,   306,   307,   308,
     432,   433,   309,   310,   311,   312,   313,   314,   657,   658,
     245,   147,   138,   129,   143,   479,   737,   703,   704,   510
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     361,    71,   733,   382,   126,   437,   381,   431,   699,   895,
     246,   659,   753,    56,   367,   404,    57,   916,    59,   134,
     400,   917,   701,   918,   756,   919,   185,   920,   110,   921,
      17,   166,   242,   440,   665,   428,   429,   556,   435,   575,
     678,   345,   426,   427,   664,   882,  1238,   217,   428,   429,
     491,   156,   157,    49,   478,   501,    71,   603,   604,   478,
    1195,   145,   735,   339,   130,    56,   131,   799,   801,   792,
     155,   757,   132,    58,   482,    65,   130,  1239,   131,   340,
     341,   371,   372,   483,   132,  -539,   441,   442,   127,   403,
     315,   165,   316,   317,   460,   137,   760,   441,   442,   373,
     247,   248,   249,   250,   441,   442,   320,   179,   321,   322,
     180,   101,  1223,   181,   182,   183,   184,  1235,   441,   442,
     764,   793,   557,   495,   496,   325,   108,   326,   327,   360,
      65,    43,    44,    45,   763,   441,   442,    46,    47,  1009,
     518,   441,   442,  1224,   441,   442,   441,   442,  1236,   318,
     550,   224,   225,   226,   874,   248,   249,   250,   441,   442,
     234,   235,   236,   528,   529,   323,   524,   400,   102,  1211,
     251,   252,    21,   534,   535,   536,   537,   538,   539,   368,
     253,   502,   254,   503,   328,   403,   153,   248,   249,   250,
     380,   607,   439,   577,   582,   158,   764,   342,   103,   255,
     256,   257,   554,   555,   115,   258,   441,   442,   104,   116,
     903,   117,   559,   118,   377,   561,   562,   563,   564,   565,
     566,   567,   568,   569,   570,   571,   572,   573,   574,   764,
     708,   215,   167,   109,   259,   260,   261,   262,   916,   263,
     370,   264,   917,   265,   918,   266,   919,   267,   920,   430,
     921,   576,   275,   255,   256,   257,   268,   319,   606,   258,
     434,   276,   430,   651,   652,   911,   441,   442,   114,  1095,
     670,   560,   599,   324,   653,   654,   655,  1021,   531,   269,
     532,   270,   533,   271,   111,   255,   256,   257,   259,   260,
     261,   258,   329,   668,   669,    48,   649,   727,   441,   442,
     441,   442,   677,   660,   680,   272,   438,   441,   442,   439,
     273,   274,   275,   513,   376,   276,   514,   277,    66,    67,
     259,   260,   261,   278,    68,    69,   690,   909,    70,   910,
     671,   764,   247,   248,   249,   250,    50,    51,    52,   692,
    1175,  1176,    53,    54,   441,   442,   441,   442,   594,   441,
     442,    72,    73,   728,    17,   729,   730,   595,   870,   643,
     500,   727,   644,   495,  1091,   759,   478,   761,   119,   276,
     445,   672,    74,    66,    67,    75,   436,   278,   128,    68,
      69,   461,   656,    70,   527,    76,   462,   134,  1011,   120,
     517,  -540,  -540,   121,   522,   463,   122,   105,   106,   107,
     464,   276,   251,   252,   688,   137,    72,    73,   673,   278,
     674,   144,   253,   675,   254,   152,   130,   728,   131,   729,
     730,   666,   146,   667,   132,   533,   754,    74,   755,   465,
      75,   255,   256,   257,   466,   661,  1100,   258,   439,  1202,
      76,  1204,    60,    61,   886,   758,   892,    62,  -540,  -540,
     455,   456,   457,   458,   459,   123,   124,  1220,   419,  1222,
     420,   421,   684,   772,   422,   685,   259,   260,   261,   262,
     769,   263,   153,   264,   159,   265,   469,   266,   154,   267,
     163,   470,   247,   248,   249,   250,   160,   741,   268,   331,
     439,   887,   332,   333,   471,   795,   161,   334,   335,   472,
      55,   645,   784,  -249,   785,   786,   787,   788,   662,   789,
     790,   269,   164,   270,   172,   271,   191,   192,   883,   173,
     269,   193,   270,   879,   271,   896,   904,   905,   906,   907,
     168,   169,   247,   248,   249,   250,   174,   272,   457,   458,
     459,   691,   273,   274,   275,   748,   175,   276,   749,   277,
     523,   750,   251,   252,   749,   278,   751,    17,  1205,   439,
     486,   487,   253,   924,   254,   148,   149,   150,   151,   139,
     140,   141,   142,   186,  1221,   771,  1225,   775,   439,   188,
     514,   255,   256,   257,   187,  1178,   189,   258,  1179,  1180,
     467,   468,  1237,  1181,  1182,   776,   779,   996,   777,   780,
     190,   875,   251,   252,   514,   900,   925,   928,   439,   926,
     929,  1252,   253,   205,   254,  1255,   259,   260,   261,   262,
     206,   263,   207,   264,   208,   265,   209,   266,   210,   267,
     211,   255,   256,   257,   441,   442,   999,   258,   268,   514,
     170,   171,   802,   803,   804,   805,   806,   586,   587,   807,
     808,   247,   248,   249,   250,   212,   809,   810,   811,  1003,
     214,   269,   439,   270,   218,   271,   259,   260,   261,   262,
     220,   263,   768,   264,   222,   265,   223,   266,   812,   267,
    1090,  1097,  1184,   780,   749,   514,   227,   272,   268,   176,
     177,   178,   273,   274,   275,   686,   687,   276,   445,   277,
     228,   782,   428,  1004,   229,   278,  1209,  1210,  1217,  1218,
     230,   269,   233,   270,   232,   271,   237,  -540,  -540,   448,
     449,   398,   399,  1014,  1015,  -540,   238,   239,   242,   244,
     336,   253,   337,   254,   330,   338,   343,   272,   346,   347,
     349,   348,   273,   274,   275,   350,   351,   276,   352,   277,
     255,   256,   257,   353,   354,   278,   258,   386,   387,   388,
     389,   390,   391,   392,   393,   394,   395,   396,   397,   355,
     247,   248,   249,   250,   356,  -540,   453,   454,   455,   456,
     457,   458,   459,   357,   358,   259,   260,   261,   262,   362,
     263,   365,   264,   366,   265,   374,   266,   379,   267,   383,
     363,   385,   813,   814,   815,   816,   817,   268,   378,   818,
     819,   384,  -536,   401,   402,   406,   820,   821,   822,     1,
     407,     2,     3,     4,     5,     6,     7,     8,     9,    10,
     269,   408,   270,   409,   271,    11,    12,    13,   823,   410,
     398,    14,    15,    16,   412,   413,   414,   415,   416,   417,
     253,   418,   254,   423,   424,   425,   272,   473,   460,   474,
    1183,   273,   274,   275,  1186,   475,   276,   476,   277,   255,
     256,   257,   477,   481,   278,   258,   478,   484,   485,   488,
     489,   490,   497,  1185,   499,   508,   509,   511,   512,   515,
      17,   520,   521,    17,   525,   540,   541,   542,   543,    77,
     544,   545,   546,   547,   259,   260,   261,   262,   548,   263,
     549,   264,   551,   265,    63,   266,   553,   267,     1,   276,
       2,     3,     4,     5,     6,     7,   268,     9,   558,    78,
      79,   578,    80,   580,    11,    12,    13,    81,    82,   583,
      14,    15,    16,   584,   585,   590,   588,   526,   589,   269,
     591,   270,   593,   271,     1,   596,     2,     3,     4,     5,
       6,     7,     8,     9,    10,   592,   597,   598,   600,   641,
      11,    12,    13,   642,   602,   272,    14,    15,    16,   646,
     273,   274,   275,    18,   647,   276,   648,   277,   650,    17,
     663,   557,     1,   278,     2,     3,     4,     5,     6,     7,
      19,     9,   676,   682,   681,   441,   689,   445,    11,    12,
      13,   693,    20,   697,    14,    15,    16,   695,   696,   698,
     501,   702,   705,   706,   707,    17,   446,   447,   448,   449,
     439,   717,    21,   714,   451,   736,   740,   718,   745,   719,
     443,   720,   444,   721,   722,   723,   724,   743,   725,   744,
     746,   747,   726,   766,   734,   767,   742,   526,   739,    83,
      84,    85,    86,    17,    87,    88,   752,   762,    89,    90,
      91,   765,   526,    92,    93,    94,   770,   687,   686,   773,
      95,    96,    18,   774,   452,   453,   454,   455,   456,   457,
     458,   459,   778,    97,    98,   884,   781,    99,   796,   798,
     445,   100,   783,   868,   869,   877,   870,   876,   878,   881,
     893,    20,   885,   888,   894,   901,   902,   445,    18,   446,
     447,   448,   449,   450,   897,   898,   913,   451,   899,   914,
     927,    21,   445,   915,   930,    19,   446,   447,   448,   449,
     931,   694,   932,   933,   451,   934,   935,    20,   936,   939,
     950,   446,   447,   448,   449,   194,    18,   961,   937,   451,
     938,   940,  1001,   941,   942,   195,   997,    21,   196,   197,
     972,   198,   199,   200,   943,  1002,   944,   452,   453,   454,
     455,   456,   457,   458,   459,    20,   945,   201,   202,   946,
     203,   204,   947,   948,   452,   453,   454,   455,   456,   457,
     458,   459,   949,   951,   952,    21,   953,   983,   764,   452,
     453,   454,   455,   456,   457,   458,   459,   608,   609,   610,
     611,   612,   613,   614,   615,   616,   617,   618,   619,   620,
     621,   622,   623,   624,   954,   625,   626,   627,   628,   629,
     630,   994,  1012,   631,  1010,   955,   632,   633,   634,   635,
     636,   637,   638,   639,   824,   825,   826,   827,   828,   956,
     998,   829,   830,   957,   958,  1018,   959,  1020,   831,   832,
     833,   835,   836,   837,   838,   839,   960,   962,   840,   841,
    1022,  1016,   963,   964,   965,   842,   843,   844,  1023,   966,
     834,   967,   968,  1024,   846,   847,   848,   849,   850,   969,
     970,   851,   852,   971,   973,   974,   975,   845,   853,   854,
     855,   857,   858,   859,   860,   861,   976,  1025,   862,   863,
     977,   978,   979,   980,   981,   864,   865,   866,   982,   984,
     856,   985,   986,  1026,  1027,   987,   988,   989,  1028,   990,
     991,   992,   993,   995,  1005,  1006,  1007,   867,  1008,  1029,
    1017,  1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,
    1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,
    1049,  1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,
    1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,
    1069,  1070,  1071,  1072,  1073,  1074,  1076,  1075,  1077,  1078,
    1079,  1080,  1081,  1082,  1083,  1084,  1085,  1086,  1087,  1088,
    1089,  1092,  1093,  1094,  1096,  1105,  1098,  1106,  1107,  1099,
    1108,  1102,  1109,  1110,  1115,  1111,  1112,  1113,  1114,  1116,
    1117,  1118,  1119,  1103,  1126,  1120,  1121,  1122,  1123,  1124,
    1125,  1137,  1127,  1128,  1129,  1130,  1131,  1132,  1133,  1134,
    1135,  1136,  1138,  1139,  1140,  1141,  1142,  1148,  1143,  1144,
    1145,  1146,  1147,  1149,  1150,  1151,  1152,  1153,  1154,  1155,
    1156,  1157,  1158,  1159,  1160,  1161,  1162,  1163,  1164,  1165,
    1166,  1167,  1170,  1168,  1169,  1171,  1177,  1193,  1194,  1172,
    1173,  1174,  1187,  1188,  1189,  1198,  1190,  1191,  1192,  1196,
    1197,  1199,  1200,  1206,  1201,  1203,  1207,  1208,  1213,  1212,
    1214,  1215,  1216,  1219,  1226,  1228,  1227,  1229,  1230,  1231,
    1232,  1233,  1234,  1240,  1246,  1241,  1242,  1243,  1244,  1245,
    1247,  1250,  1248,  1251,  1249,  1253,  1254,   162,   871,   213,
     519,   873,   369,   738,   125,   359,   912,   716,  1013,  1101,
     530,   679,   579,   581,   890,   480,   908,   891,   880,     0,
     411,  1000,   552,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   344
};

static const yytype_int16 yycheck[] =
{
     219,     8,   640,   242,    21,   278,   240,   276,   588,   763,
     157,   521,   664,     3,    55,   254,     4,   796,     6,     8,
     251,   796,   589,   796,    84,   796,    62,   796,    16,   796,
      78,     6,    66,    55,    87,     5,     6,    75,   277,     3,
       3,   188,   273,   274,    55,    84,   136,   133,     5,     6,
      82,    76,    77,    37,    78,     3,    63,    70,    71,    78,
       6,    49,   642,    10,    20,     3,    22,   719,   720,     3,
      58,    87,    28,    76,    40,     8,    20,   167,    22,    26,
      27,    33,    34,    49,    28,    62,   157,   158,     0,    88,
       3,    79,     5,     6,   222,    72,   224,   157,   158,    51,
       3,     4,     5,     6,   157,   158,     3,   203,     5,     6,
     206,    34,   136,   101,   102,   103,   104,   136,   157,   158,
      66,    55,   160,   362,   363,     3,     3,     5,     6,   215,
      63,    33,    34,    35,     4,   157,   158,    39,    40,   893,
     379,   157,   158,   167,   157,   158,   157,   158,   167,    62,
     221,   139,   140,   141,   734,     4,     5,     6,   157,   158,
     148,   149,   150,   402,   403,    62,   385,   398,    34,     6,
      73,    74,   220,   412,   413,   414,   415,   416,   417,   220,
      83,   129,    85,   131,    62,    88,   222,     4,     5,     6,
     224,   135,   224,   462,   467,   220,    66,   185,    34,   102,
     103,   104,   441,   442,     7,   108,   157,   158,    34,    12,
     777,    14,    73,    16,   231,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,    66,
     220,   220,   207,     3,   137,   138,   139,   140,  1017,   142,
     228,   144,  1017,   146,  1017,   148,  1017,   150,  1017,   219,
    1017,   215,   215,   102,   103,   104,   159,   170,   497,   108,
     277,   218,   219,    68,    69,   221,   157,   158,   174,     4,
     221,   132,   491,   170,    79,    80,    81,   929,    84,   182,
      86,   184,    88,   186,     3,   102,   103,   104,   137,   138,
     139,   108,   170,   532,   533,   197,   515,    73,   157,   158,
     157,   158,   541,   522,   543,   208,   221,   157,   158,   224,
     213,   214,   215,   221,    38,   218,   224,   220,     8,     8,
     137,   138,   139,   226,     8,     8,   557,    34,     8,    36,
     221,    66,     3,     4,     5,     6,    33,    34,    35,   558,
    1094,  1095,    39,    40,   157,   158,   157,   158,    33,   157,
     158,     8,     8,   129,    78,   131,   132,    42,   134,   221,
     367,    73,   224,   602,  1002,   224,    78,   224,   171,   218,
     133,   221,     8,    63,    63,     8,   225,   226,   223,    63,
      63,   219,   187,    63,   401,     8,   224,     8,   898,   192,
     378,   154,   155,   196,    65,   219,   199,   161,   162,   163,
     224,   218,    73,    74,   551,    72,    63,    63,   221,   226,
     221,     3,    83,   221,    85,     3,    20,   129,    22,   131,
     132,    84,    72,    86,    28,    88,   665,    63,   667,   219,
      63,   102,   103,   104,   224,   221,  1016,   108,   224,  1193,
      63,  1195,   168,   169,   759,   676,   761,   173,   211,   212,
     213,   214,   215,   216,   217,    33,    34,  1211,   122,  1213,
     124,   125,   221,   694,   128,   224,   137,   138,   139,   140,
     689,   142,   222,   144,     3,   146,   219,   148,    14,   150,
     220,   224,     3,     4,     5,     6,     3,   221,   159,    40,
     224,   760,    43,    44,   219,   714,     3,    48,    49,   224,
     197,   508,    56,    57,    58,    59,    60,    61,   525,    63,
      64,   182,     3,   184,     3,   186,   188,   189,   757,   178,
     182,   193,   184,   742,   186,   764,     3,     4,     5,     6,
     175,   176,     3,     4,     5,     6,     6,   208,   215,   216,
     217,   558,   213,   214,   215,   221,     3,   218,   224,   220,
     221,   221,    73,    74,   224,   226,   221,    78,  1196,   224,
     190,   191,    83,   797,    85,    51,    52,    53,    54,    44,
      45,    46,    47,    55,  1212,   221,  1214,   221,   224,     6,
     224,   102,   103,   104,   224,    40,   197,   108,    43,    44,
     224,   225,  1230,    48,    49,   221,   221,   870,   224,   224,
     197,   221,    73,    74,   224,   221,   221,   221,   224,   224,
     224,  1249,    83,   197,    85,  1253,   137,   138,   139,   140,
       4,   142,   195,   144,    76,   146,   197,   148,   197,   150,
     221,   102,   103,   104,   157,   158,   221,   108,   159,   224,
     175,   176,    91,    92,    93,    94,    95,    45,    46,    98,
      99,     3,     4,     5,     6,   221,   105,   106,   107,   221,
      21,   182,   224,   184,    65,   186,   137,   138,   139,   140,
      67,   142,   689,   144,    73,   146,     3,   148,   127,   150,
     221,   221,   221,   224,   224,   224,     3,   208,   159,   202,
     203,   204,   213,   214,   215,     5,     6,   218,   133,   220,
      62,   708,     5,     6,    62,   226,    45,    46,    43,    44,
     220,   182,     3,   184,    74,   186,     3,   152,   153,   154,
     155,    73,    74,   913,   914,   160,     3,     3,    66,     4,
       3,    83,     3,    85,   221,     4,   220,   208,     4,   167,
       3,     6,   213,   214,   215,     6,     4,   218,     3,   220,
     102,   103,   104,     4,    55,   226,   108,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,     4,
       3,     4,     5,     6,   197,   210,   211,   212,   213,   214,
     215,   216,   217,     3,     3,   137,   138,   139,   140,    53,
     142,    74,   144,   135,   146,     3,   148,   210,   150,    78,
      68,   220,    91,    92,    93,    94,    95,   159,    62,    98,
      99,    78,     0,   220,   220,     4,   105,   106,   107,     7,
       4,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     182,     4,   184,     4,   186,    23,    24,    25,   127,     6,
      73,    29,    30,    31,   220,   220,   220,   220,   220,   220,
      83,   220,    85,   220,   220,   220,   208,     3,   222,     6,
    1099,   213,   214,   215,  1103,    47,   218,    47,   220,   102,
     103,   104,    77,     4,   226,   108,    78,     6,    77,     4,
     221,   221,    69,  1102,     4,    55,   220,     3,     3,   220,
      78,   220,   220,    78,   220,     4,   220,   220,   220,     3,
     220,     4,     4,   221,   137,   138,   139,   140,   227,   142,
     221,   144,    77,   146,     3,   148,     3,   150,     7,   218,
       9,    10,    11,    12,    13,    14,   159,    16,   220,    33,
      34,     6,    36,     6,    23,    24,    25,    41,    42,     6,
      29,    30,    31,     5,    43,     3,   220,    73,   220,   182,
     220,   184,     6,   186,     7,     4,     9,    10,    11,    12,
      13,    14,    15,    16,    17,   221,   167,   167,    76,   130,
      23,    24,    25,   220,   224,   208,    29,    30,    31,     3,
     213,   214,   215,   171,   135,   218,     3,   220,   224,    78,
     221,   160,     7,   226,     9,    10,    11,    12,    13,    14,
     188,    16,    76,   224,     4,   157,   220,   133,    23,    24,
      25,   132,   200,     6,    29,    30,    31,   227,   227,     6,
       3,     3,     6,     4,     4,    78,   152,   153,   154,   155,
     224,     4,   220,    89,   160,    32,     4,   220,     4,   220,
      73,   220,    75,   220,   220,   220,   220,     6,   220,     6,
       3,     6,   220,     4,   220,    78,   220,    73,   221,   163,
     164,   165,   166,    78,   168,   169,   221,   224,   172,   173,
     174,   221,    73,   177,   178,   179,   221,     6,     5,    50,
     184,   185,   171,    47,   210,   211,   212,   213,   214,   215,
     216,   217,   210,   197,   198,   221,     6,   201,   181,     6,
     133,   205,   224,   132,   130,   135,   134,   220,   167,   221,
       4,   200,   218,   218,   224,     6,     6,   133,   171,   152,
     153,   154,   155,   156,   221,   220,    57,   160,   221,    57,
     221,   220,   133,     3,   224,   188,   152,   153,   154,   155,
     224,   157,   224,   224,   160,   224,   224,   200,   224,    93,
      93,   152,   153,   154,   155,   162,   171,    93,   224,   160,
     224,   224,     4,   224,   224,   172,   135,   220,   175,   176,
      93,   178,   179,   180,   224,     3,   224,   210,   211,   212,
     213,   214,   215,   216,   217,   200,   224,   194,   195,   224,
     197,   198,   224,   224,   210,   211,   212,   213,   214,   215,
     216,   217,   224,   224,   224,   220,   224,    93,    66,   210,
     211,   212,   213,   214,   215,   216,   217,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   224,   108,   109,   110,   111,   112,
     113,    93,     6,   116,   221,   224,   119,   120,   121,   122,
     123,   124,   125,   126,    91,    92,    93,    94,    95,   224,
     135,    98,    99,   224,   224,    52,   224,     6,   105,   106,
     107,    91,    92,    93,    94,    95,   224,   224,    98,    99,
       6,   220,   224,   224,   224,   105,   106,   107,     6,   224,
     127,   224,   224,     6,    91,    92,    93,    94,    95,   224,
     224,    98,    99,   224,   224,   224,   224,   127,   105,   106,
     107,    91,    92,    93,    94,    95,   224,     6,    98,    99,
     224,   224,   224,   224,   224,   105,   106,   107,   224,   224,
     127,   224,   224,     6,     6,   224,   224,   224,     6,   224,
     224,   224,   224,   224,   224,   224,   224,   127,   224,     6,
     224,     6,   224,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   224,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   224,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   224,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   224,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   224,     6,     4,
       4,     4,     4,     4,   221,   221,     6,   221,   221,    62,
     221,    53,   221,   221,     6,   221,   221,   221,   221,   221,
     221,   221,   221,    54,     6,   221,   221,   221,   221,   221,
     221,     6,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,     6,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,     6,   221,   221,   221,   221,   221,   221,
     221,   221,     6,   221,   221,   221,   167,     4,     4,   224,
     224,   224,   221,   221,   221,    49,   221,   221,   221,   221,
     221,     6,     6,   167,   224,   224,   220,     6,     4,   221,
     221,    49,     4,     6,   220,     6,   221,    48,   221,   221,
      40,    40,     4,   220,     3,   221,    40,    40,    40,    40,
     220,     3,   220,     3,   221,   221,   221,    63,   731,   128,
     380,   733,   226,   644,    21,   215,   783,   602,   911,  1017,
     404,   542,   464,   466,   761,   343,   780,   761,   749,    -1,
     261,   876,   439,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   187
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    78,   171,   188,
     200,   220,   229,   230,   231,   233,   245,   246,   248,   249,
     252,   253,   254,   255,   256,   281,   286,   287,   288,   289,
     290,   291,   292,    33,    34,    35,    39,    40,   197,    37,
      33,    34,    35,    39,    40,   197,     3,   279,    76,   279,
     168,   169,   173,     3,   232,   233,   245,   246,   249,   252,
     253,   254,   286,   287,   288,   289,   290,     3,    33,    34,
      36,    41,    42,   163,   164,   165,   166,   168,   169,   172,
     173,   174,   177,   178,   179,   184,   185,   197,   198,   201,
     205,    34,    34,    34,    34,   161,   162,   163,     3,     3,
     279,     3,   282,   283,   174,     7,    12,    14,    16,   171,
     192,   196,   199,    33,    34,   255,   256,     0,   223,   341,
      20,    22,    28,   275,     8,   257,   259,    72,   340,   340,
     340,   340,   340,   342,     3,   279,    72,   339,   339,   339,
     339,   339,     3,   222,    14,   279,    76,    77,   220,     3,
       3,     3,   232,   220,     3,   279,     6,   207,   175,   176,
     175,   176,     3,   178,     6,     3,   202,   203,   204,   203,
     206,   279,   279,   279,   279,    62,    55,   224,     6,   197,
     197,   188,   189,   193,   162,   172,   175,   176,   178,   179,
     180,   194,   195,   197,   198,   197,     4,   195,    76,   197,
     197,   221,   221,   231,    21,   220,   258,   259,    65,   266,
      67,   260,    73,     3,   279,   279,   279,     3,    62,    62,
     220,   247,    74,     3,   279,   279,   279,     3,     3,     3,
     250,   251,    66,   272,     4,   338,   338,     3,     4,     5,
       6,    73,    74,    83,    85,   102,   103,   104,   108,   137,
     138,   139,   140,   142,   144,   146,   148,   150,   159,   182,
     184,   186,   208,   213,   214,   215,   218,   220,   226,   293,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   306,
     307,   308,   309,   310,   312,   313,   314,   315,   316,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,   330,
     331,   332,   333,   334,   335,     3,     5,     6,    62,   170,
       3,     5,     6,    62,   170,     3,     5,     6,    62,   170,
     221,    40,    43,    44,    48,    49,     3,     3,     4,    10,
      26,    27,   279,   220,   283,   338,     4,   167,     6,     3,
       6,     4,     3,     4,    55,     4,   197,     3,     3,   258,
     259,   293,    53,    68,   264,    74,   135,    55,   220,   247,
     279,    33,    34,    51,     3,   244,    38,   256,    62,   210,
     224,   272,   296,    78,    78,   220,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,    73,    74,
     297,   220,   220,    88,   296,   311,     4,     4,     4,     4,
       6,   335,   220,   220,   220,   220,   220,   220,   220,   122,
     124,   125,   128,   220,   220,   220,   297,   297,     5,     6,
     219,   316,   328,   329,   256,   296,   225,   315,   221,   224,
      55,   157,   158,    73,    75,   133,   152,   153,   154,   155,
     156,   160,   210,   211,   212,   213,   214,   215,   216,   217,
     222,   219,   224,   219,   224,   219,   224,   224,   225,   219,
     224,   219,   224,     3,     6,    47,    47,    77,    78,   343,
     257,     4,    40,    49,     6,    77,   190,   191,     4,   221,
     221,    82,   268,   261,   262,   296,   296,    69,   265,     4,
     254,     3,   129,   131,   234,   236,   237,   243,    55,   220,
     347,     3,     3,   221,   224,   220,   294,   279,   296,   251,
     220,   220,    65,   221,   293,   220,    73,   256,   296,   296,
     311,    84,    86,    88,   296,   296,   296,   296,   296,   296,
       4,   220,   220,   220,   220,     4,     4,   221,   227,   221,
     221,    77,   295,     3,   296,   296,    75,   160,   220,    73,
     132,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   297,     3,   215,   316,     6,   328,
       6,   329,   315,     6,     5,    43,    45,    46,   220,   220,
       3,   220,   221,     6,    33,    42,     4,   167,   167,   293,
      76,   269,   224,    70,    71,   263,   296,   135,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   108,   109,   110,   111,   112,
     113,   116,   119,   120,   121,   122,   123,   124,   125,   126,
     239,   130,   220,   221,   224,   254,     3,   135,     3,   293,
     224,    68,    69,    79,    80,    81,   187,   336,   337,   336,
     293,   221,   256,   221,    55,    87,    84,    86,   296,   296,
     221,   221,   221,   221,   221,   221,    76,   296,     3,   314,
     296,     4,   224,   271,   221,   224,     5,     6,   338,   220,
     297,   256,   293,   132,   157,   227,   227,     6,     6,   244,
     235,   237,     3,   345,   346,     6,     4,     4,   220,   276,
     277,   278,   279,   284,    89,   267,   262,     4,   220,   220,
     220,   220,   220,   220,   220,   220,   220,    73,   129,   131,
     132,   240,   241,   343,   220,   244,    32,   344,   236,   221,
       4,   221,   220,     6,     6,     4,     3,     6,   221,   224,
     221,   221,   221,   239,   296,   296,    84,    87,   297,   224,
     224,   224,   224,     4,    66,   221,     4,    78,   256,   293,
     221,   221,   297,    50,    47,   221,   221,   224,   210,   221,
     224,     6,   254,   224,    56,    58,    59,    60,    61,    63,
      64,   285,     3,    55,   280,   293,   181,   270,     6,   239,
     238,   239,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   127,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   127,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   127,    91,    92,    93,    94,    95,
      98,    99,   105,   106,   107,   127,    91,    92,    93,    94,
      95,    98,    99,   105,   106,   107,   127,    91,    92,    93,
      94,    95,    98,    99,   105,   106,   107,   127,   132,   130,
     134,   241,   242,   242,   244,   221,   220,   135,   167,   293,
     337,   221,    84,   296,   221,   218,   330,   316,   218,   317,
     320,   325,   330,     4,   224,   271,   296,   221,   220,   221,
     221,     6,     6,   237,     3,     4,     5,     6,   346,    34,
      36,   221,   277,    57,    57,     3,   298,   299,   300,   301,
     302,   303,   304,   305,   272,   221,   224,   221,   221,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,    93,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
      93,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,    93,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,    93,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,    93,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,    93,   224,   315,   135,   135,   221,
     345,     4,     3,   221,     6,   224,   224,   224,   224,   271,
     221,   336,     6,   280,   278,   278,   220,   224,    52,   274,
       6,   239,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   224,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   224,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   224,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   224,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   224,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   224,     6,     4,     4,
     221,   343,     4,     4,     4,     4,   221,   221,     6,    62,
     244,   304,    53,    54,   273,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,     6,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,     6,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,     6,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,     6,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,     6,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
       6,   221,   224,   224,   224,   271,   271,   167,    40,    43,
      44,    48,    49,   296,   221,   293,   296,   221,   221,   221,
     221,   221,   221,     4,     4,     6,   221,   221,    49,     6,
       6,   224,   271,   224,   271,   343,   167,   220,     6,    45,
      46,     6,   221,     4,   221,    49,     4,    43,    44,     6,
     271,   343,   271,   136,   167,   343,   220,   221,     6,    48,
     221,   221,    40,    40,     4,   136,   167,   343,   136,   167,
     220,   221,    40,    40,    40,    40,     3,   220,   220,   221,
       3,     3,   343,   221,   221,   343
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   228,   229,   230,   230,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   234,   234,   235,   235,   236,   236,   237,   237,
     237,   237,   238,   238,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   240,   240,
     241,   241,   241,   241,   242,   242,   243,   243,   244,   244,
     245,   246,   246,   247,   247,   248,   248,   249,   250,   250,
     251,   252,   252,   252,   252,   252,   253,   253,   253,   254,
     254,   254,   254,   255,   255,   256,   257,   258,   258,   259,
     260,   260,   261,   261,   262,   263,   263,   263,   264,   264,
     265,   265,   266,   266,   267,   267,   268,   268,   269,   269,
     270,   270,   271,   271,   272,   272,   273,   273,   274,   274,
     275,   275,   275,   275,   276,   276,   277,   277,   278,   278,
     279,   279,   280,   280,   280,   280,   281,   281,   282,   282,
     283,   284,   284,   285,   285,   285,   285,   285,   285,   285,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   287,   287,   287,   288,   288,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   290,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     292,   292,   292,   293,   293,   294,   294,   295,   295,   296,
     296,   296,   296,   296,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   298,   298,   298,
     299,   299,   299,   299,   300,   300,   300,   300,   301,   301,
     301,   301,   302,   302,   303,   303,   304,   304,   304,   304,
     304,   304,   305,   305,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   307,   307,   308,   309,   309,
     310,   310,   310,   310,   311,   311,   312,   313,   313,   313,
     313,   314,   314,   314,   314,   315,   315,   315,   315,   315,
     315,   315,   315,   315,   315,   315,   315,   315,   316,   316,
     316,   316,   317,   317,   317,   318,   319,   319,   320,   320,
     321,   322,   322,   323,   324,   324,   325,   326,   326,   327,
     327,   328,   329,   330,   330,   331,   332,   332,   333,   334,
     334,   335,   335,   335,   335,   335,   335,   335,   335,   335,
     335,   335,   335,   336,   336,   337,   337,   337,   337,   337,
     337,   338,   339,   339,   340,   340,   341,   341,   342,   342,
     343,   343,   344,   344,   345,   345,   346,   346,   346,   346,
     346,   347,   347
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     4,     4,     8,     6,    10,     8,     7,
       6,     8,     1,     3,     1,     3,     1,     1,     4,     4,
       6,     6,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     4,     1,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     4,     4,     1,     2,
       2,     1,     1,     2,     2,     0,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     2,     5,     1,     3,
       3,     4,     4,     4,     4,     6,     8,    11,     8,     1,
       1,     3,     3,     3,     3,     2,     4,     3,     3,    10,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       2,     0,     3,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     3,     2,     2,     2,     2,     3,
       2,     3,     3,     3,     3,     4,     4,     3,     3,     4,
       4,     5,     6,     7,     9,     4,     5,     7,     9,     2,
       3,     2,     3,     3,     4,     2,     3,     3,     4,     2,
       3,     2,     2,     2,     2,     5,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     3,     3,     6,     6,     5,     3,     4,
       4,     3,     3,     4,     6,     7,     9,    10,    12,    12,
      13,    14,    15,    16,    12,    13,    15,    16,     3,     4,
       5,     6,     3,     3,     4,     3,     3,     4,     4,     6,
       5,     3,     4,     3,     4,     3,     3,     5,     7,     7,
       6,     8,     8,     1,     3,     3,     5,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,    14,    19,    16,
      20,    16,    15,    13,    18,    14,    13,    11,     8,    10,
      13,    15,     5,     7,     4,     6,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     4,     5,     3,     1,     4,
       4,     4,     4,     4,     4,     4,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     4,     3,     3,     5,     5,     6,
       4,     6,     3,     5,     4,     5,     6,     4,     5,     5,
       6,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     2,     3,     1,     1,
       2,     2,     3,     2,     2,     3,     2,     2,     2,     2,
       3,     3,     3,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     2,     2,     1,     2,     2,
       2,     1,     2,     0,     3,     0,     1,     0,     2,     0,
       4,     0,     4,     0,     1,     3,     1,     3,     3,     3,
       3,     6,     3
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
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2486 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2494 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 245 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2508 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 225 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2522 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 235 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2536 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 184 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2554 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2565 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2573 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2582 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 292 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 285 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2616 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2626 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2636 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2646 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2656 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2666 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 275 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2690 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 275 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2704 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 355 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2714 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2722 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2730 "parser.cpp"
        break;

    case YYSYMBOL_unnest_clause: /* unnest_clause  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2744 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2758 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2791 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2813 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2831 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2840 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 312 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2853 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2862 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2890 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 361 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2900 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2909 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2923 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 265 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3009 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3017 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3055 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3063 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3071 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 373 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3084 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3100 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3108 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3116 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3124 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3132 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3140 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3148 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3156 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3164 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3172 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3180 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3188 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3196 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3204 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3212 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 382 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3220 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 386 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3228 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3236 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3244 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3252 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3260 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3268 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3276 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3284 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 218 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3295 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 201 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3309 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 201 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3323 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 194 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3334 "parser.cpp"
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

#line 3442 "parser.cpp"

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
#line 517 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3657 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 521 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3668 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 527 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3679 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 534 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3685 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 535 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3691 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3697 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3703 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 538 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3709 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3715 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3721 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3727 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 542 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3733 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 543 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3739 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 544 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3745 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 545 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3751 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3757 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3763 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3769 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 550 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3775 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 551 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3781 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 552 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3787 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3793 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 554 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3799 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 555 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3805 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3811 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3817 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 558 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3823 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 559 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3829 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 560 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3835 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 561 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3841 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 568 "parser.y"
                                                                           {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[-2].str_value));
    create_schema_info->schema_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    if(create_schema_info->schema_name_.empty()) {
        yyerror(&yyloc, scanner, result, "Empty database name is given.");
        YYERROR;
    }

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-3].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    (yyval.create_stmt)->create_info_->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 3863 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 585 "parser.y"
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
#line 3883 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 602 "parser.y"
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
#line 3901 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 618 "parser.y"
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
#line 3934 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 647 "parser.y"
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
#line 3954 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 662 "parser.y"
                                                                                                                  {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    delete (yyvsp[-6].table_name_t);

    for (infinity::TableElement*& element : *(yyvsp[-4].table_element_array_t)) {
        if(element->type_ == infinity::TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((infinity::ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((infinity::TableConstraint*)element);
        }
    }
    delete (yyvsp[-4].table_element_array_t);

    if ((yyvsp[-2].with_index_param_list_t) != nullptr) {
        create_table_info->properties_ = std::move(*(yyvsp[-2].with_index_param_list_t));
        delete (yyvsp[-2].with_index_param_list_t);
    }

    create_table_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = create_table_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-7].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3990 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 694 "parser.y"
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

    create_table_info->conflict_type_ = (yyvsp[-5].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    create_table_info->select_ = (yyvsp[-2].select_stmt);
    create_table_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    (yyval.create_stmt)->create_info_ = create_table_info;
}
#line 4012 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 712 "parser.y"
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
#line 4033 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 730 "parser.y"
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
#line 4066 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 758 "parser.y"
                                                                          {
    std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    create_index_info->index_name_ = (yyvsp[-5].if_not_exists_info_t)->info_;
    if ((yyvsp[-5].if_not_exists_info_t)->exists_) {
        create_index_info->conflict_type_ = (yyvsp[-5].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    } else {
        create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
    }
    delete (yyvsp[-5].if_not_exists_info_t);

    create_index_info->index_info_ = (yyvsp[-2].index_info_t);
    create_index_info->comment_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 4101 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 789 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4110 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 793 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4119 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 798 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4128 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 802 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4137 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 808 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4145 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 811 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4153 "parser.cpp"
    break;

  case 48: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 818 "parser.y"
                                                          {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-1].with_index_param_list_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-2].column_type_t)), index_param_list);
    delete (yyvsp[-2].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-3].str_value));
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), "", std::move(default_expr));
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4178 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 838 "parser.y"
                                                         {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-2].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-2].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-3].str_value));
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), "", std::move(default_expr));
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    (yyval.table_column_t)->constraints_ = std::move(*(yyvsp[-1].column_constraints_t));
    delete (yyvsp[-1].column_constraints_t);
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4205 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 860 "parser.y"
                                                                           {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-3].with_index_param_list_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-4].column_type_t)), index_param_list);
    delete (yyvsp[-4].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-5].str_value));
        free((yyvsp[0].str_value));
        YYERROR;
    }

    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), (yyvsp[0].str_value), std::move(default_expr));
    free((yyvsp[0].str_value));

    ParserHelper::ToLower((yyvsp[-5].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-5].str_value);
    free((yyvsp[-5].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4234 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 884 "parser.y"
                                                                        {
    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-4].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-4].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        free((yyvsp[-5].str_value));
        free((yyvsp[0].str_value));
        delete (yyvsp[-3].column_constraints_t);
        YYERROR;
    }
    (yyval.table_column_t) = new infinity::ColumnDef(std::move(data_type_result), (yyvsp[0].str_value), std::move(default_expr));
    free((yyvsp[0].str_value));

    ParserHelper::ToLower((yyvsp[-5].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-5].str_value);
    (yyval.table_column_t)->constraints_ = std::move(*(yyvsp[-3].column_constraints_t));
    delete (yyvsp[-3].column_constraints_t);
    free((yyvsp[-5].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4264 "parser.cpp"
    break;

  case 52: /* column_type_array: column_type  */
#line 910 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4273 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type_array ',' column_type  */
#line 914 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4282 "parser.cpp"
    break;

  case 54: /* column_type: BOOLEAN  */
#line 920 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4288 "parser.cpp"
    break;

  case 55: /* column_type: TINYINT  */
#line 921 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4294 "parser.cpp"
    break;

  case 56: /* column_type: SMALLINT  */
#line 922 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4300 "parser.cpp"
    break;

  case 57: /* column_type: INTEGER  */
#line 923 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4306 "parser.cpp"
    break;

  case 58: /* column_type: INT  */
#line 924 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4312 "parser.cpp"
    break;

  case 59: /* column_type: BIGINT  */
#line 925 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4318 "parser.cpp"
    break;

  case 60: /* column_type: HUGEINT  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4324 "parser.cpp"
    break;

  case 61: /* column_type: FLOAT  */
#line 927 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4330 "parser.cpp"
    break;

  case 62: /* column_type: REAL  */
#line 928 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4336 "parser.cpp"
    break;

  case 63: /* column_type: DOUBLE  */
#line 929 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4342 "parser.cpp"
    break;

  case 64: /* column_type: FLOAT16  */
#line 930 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4348 "parser.cpp"
    break;

  case 65: /* column_type: BFLOAT16  */
#line 931 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4354 "parser.cpp"
    break;

  case 66: /* column_type: DATE  */
#line 932 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4360 "parser.cpp"
    break;

  case 67: /* column_type: TIME  */
#line 933 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4366 "parser.cpp"
    break;

  case 68: /* column_type: DATETIME  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4372 "parser.cpp"
    break;

  case 69: /* column_type: TIMESTAMP  */
#line 935 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4378 "parser.cpp"
    break;

  case 70: /* column_type: UUID  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4384 "parser.cpp"
    break;

  case 71: /* column_type: POINT  */
#line 937 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4390 "parser.cpp"
    break;

  case 72: /* column_type: LINE  */
#line 938 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4396 "parser.cpp"
    break;

  case 73: /* column_type: LSEG  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4402 "parser.cpp"
    break;

  case 74: /* column_type: BOX  */
#line 940 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4408 "parser.cpp"
    break;

  case 75: /* column_type: CIRCLE  */
#line 943 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4414 "parser.cpp"
    break;

  case 76: /* column_type: VARCHAR  */
#line 945 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4420 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 946 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4426 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 947 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4432 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4438 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4444 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4450 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4456 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4462 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4468 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4474 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4480 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4486 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4492 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4498 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4504 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4510 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4516 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4522 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4528 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4534 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4540 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4546 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4552 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4558 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4564 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4570 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4576 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4582 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4588 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4594 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4600 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4606 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4612 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4618 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4624 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4630 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4636 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4642 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4648 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4654 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4660 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4666 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4672 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4678 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4684 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4690 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4696 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4702 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4708 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4714 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4720 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4726 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4732 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4738 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4744 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4750 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4756 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4762 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4768 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4774 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4780 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4786 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4792 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4798 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4804 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4810 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4816 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4822 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4828 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4834 "parser.cpp"
    break;

  case 146: /* column_type: ARRAY '(' column_type ')'  */
#line 1017 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4843 "parser.cpp"
    break;

  case 147: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1021 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4853 "parser.cpp"
    break;

  case 148: /* column_constraints: column_constraint  */
#line 1044 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4862 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraints column_constraint  */
#line 1048 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4876 "parser.cpp"
    break;

  case 150: /* column_constraint: PRIMARY KEY  */
#line 1058 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4884 "parser.cpp"
    break;

  case 151: /* column_constraint: UNIQUE  */
#line 1061 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4892 "parser.cpp"
    break;

  case 152: /* column_constraint: NULLABLE  */
#line 1064 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4900 "parser.cpp"
    break;

  case 153: /* column_constraint: NOT NULLABLE  */
#line 1067 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4908 "parser.cpp"
    break;

  case 154: /* default_expr: DEFAULT constant_expr  */
#line 1071 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4916 "parser.cpp"
    break;

  case 155: /* default_expr: %empty  */
#line 1074 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4924 "parser.cpp"
    break;

  case 156: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1079 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4934 "parser.cpp"
    break;

  case 157: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1084 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4944 "parser.cpp"
    break;

  case 158: /* identifier_array: IDENTIFIER  */
#line 1091 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4955 "parser.cpp"
    break;

  case 159: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1097 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4966 "parser.cpp"
    break;

  case 160: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1107 "parser.y"
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
#line 4983 "parser.cpp"
    break;

  case 161: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1123 "parser.y"
                                                                                          {
    bool is_error{false};
    for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
        for (const auto &expr : expr_array->values_) {
            if(expr->type_ != infinity::ParsedExprType::kConstant) {
                yyerror(&yyloc, scanner, result, ("Value list has non-constant expression: " + expr->ToString()).c_str());
                is_error = true;
            }
        }
    }
    if(is_error) {
        for (auto expr_array : *(yyvsp[0].insert_row_list_t)) {
            delete (expr_array);
        }
        delete (yyvsp[0].insert_row_list_t);
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
    for (infinity::InsertRowExpr* &expr_ptr : *(yyvsp[0].insert_row_list_t)) {
        if ((yyvsp[-2].identifier_array_t)) {
            expr_ptr->columns_ = *((yyvsp[-2].identifier_array_t));
        }
        (yyval.insert_stmt)->insert_rows_.emplace_back(expr_ptr);
        expr_ptr = nullptr;
    }
    delete (yyvsp[-2].identifier_array_t);
    delete (yyvsp[0].insert_row_list_t);
}
#line 5026 "parser.cpp"
    break;

  case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1161 "parser.y"
                                                                        {
    (yyval.insert_stmt) = new infinity::InsertStatement();
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.insert_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.insert_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    delete (yyvsp[-2].table_name_t);
    if ((yyvsp[-1].identifier_array_t)) {
        (yyval.insert_stmt)->columns_for_select_ = std::move(*((yyvsp[-1].identifier_array_t)));
        delete (yyvsp[-1].identifier_array_t);
    }
    (yyval.insert_stmt)->select_.reset((yyvsp[0].select_stmt));
}
#line 5046 "parser.cpp"
    break;

  case 163: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1177 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5054 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: %empty  */
#line 1180 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5062 "parser.cpp"
    break;

  case 165: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1187 "parser.y"
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
#line 5080 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN explainable_statement  */
#line 1199 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5090 "parser.cpp"
    break;

  case 167: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1208 "parser.y"
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
#line 5107 "parser.cpp"
    break;

  case 168: /* update_expr_array: update_expr  */
#line 1221 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5116 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1225 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5125 "parser.cpp"
    break;

  case 170: /* update_expr: IDENTIFIER '=' expr  */
#line 1230 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5137 "parser.cpp"
    break;

  case 171: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1243 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5153 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1256 "parser.y"
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
#line 5171 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1271 "parser.y"
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
#line 5189 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1286 "parser.y"
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
#line 5207 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1301 "parser.y"
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
#line 5230 "parser.cpp"
    break;

  case 176: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1324 "parser.y"
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
#line 5288 "parser.cpp"
    break;

  case 177: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1377 "parser.y"
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
#line 5348 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1432 "parser.y"
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
#line 5400 "parser.cpp"
    break;

  case 179: /* select_statement: select_without_paren  */
#line 1483 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5408 "parser.cpp"
    break;

  case 180: /* select_statement: select_with_paren  */
#line 1486 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5416 "parser.cpp"
    break;

  case 181: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1489 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5430 "parser.cpp"
    break;

  case 182: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1498 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5444 "parser.cpp"
    break;

  case 183: /* select_with_paren: '(' select_without_paren ')'  */
#line 1508 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5452 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_with_paren ')'  */
#line 1511 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5460 "parser.cpp"
    break;

  case 185: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1515 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5469 "parser.cpp"
    break;

  case 186: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1520 "parser.y"
                                                                                                   {
    if((yyvsp[-1].expr_t) == nullptr and (yyvsp[0].expr_t) != nullptr) {
        delete (yyvsp[-3].select_stmt);
        delete (yyvsp[-2].order_by_expr_list_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Offset expression isn't valid without Limit expression");
        YYERROR;
    }
    if((yyvsp[-3].select_stmt)->search_expr_ != nullptr and ((yyvsp[-2].order_by_expr_list_t) != nullptr /*or $3 != nullptr or $4 != nullptr*/)) {
        delete (yyvsp[-3].select_stmt);
        if ((yyvsp[-2].order_by_expr_list_t)) {
            for (auto ptr : *((yyvsp[-2].order_by_expr_list_t))) {
                delete ptr;
            }
            delete (yyvsp[-2].order_by_expr_list_t);
        }
        delete (yyvsp[-1].expr_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Result modifier(ORDER BY) is conflict with SEARCH expression.");
        YYERROR;
    }
    (yyvsp[-3].select_stmt)->order_by_list_ = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 5500 "parser.cpp"
    break;

  case 187: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1547 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5508 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1550 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5516 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause unnest_clause search_clause where_clause group_by_clause having_clause  */
#line 1555 "parser.y"
                                                                                                                               {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-8].bool_value);
    (yyval.select_stmt)->select_list_ = (yyvsp[-7].expr_array_t);
    (yyval.select_stmt)->highlight_list_ = (yyvsp[-6].expr_array_t);
    (yyval.select_stmt)->table_ref_ = (yyvsp[-5].table_reference_t);
    (yyval.select_stmt)->unnest_expr_list_ = (yyvsp[-4].expr_array_t);
    (yyval.select_stmt)->search_expr_ = (yyvsp[-3].expr_t);
    (yyval.select_stmt)->where_expr_ = (yyvsp[-2].expr_t);
    (yyval.select_stmt)->group_by_list_ = (yyvsp[-1].expr_array_t);
    (yyval.select_stmt)->having_expr_ = (yyvsp[0].expr_t);

    if((yyval.select_stmt)->group_by_list_ == nullptr && (yyval.select_stmt)->having_expr_ != nullptr) {
        yyerror(&yyloc, scanner, result, "HAVING clause should follow after GROUP BY clause");
        YYERROR;
    }
}
#line 5538 "parser.cpp"
    break;

  case 190: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1573 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5546 "parser.cpp"
    break;

  case 191: /* order_by_clause: %empty  */
#line 1576 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5554 "parser.cpp"
    break;

  case 192: /* order_by_expr_list: order_by_expr  */
#line 1580 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5563 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1584 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5572 "parser.cpp"
    break;

  case 194: /* order_by_expr: expr order_by_type  */
#line 1589 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5582 "parser.cpp"
    break;

  case 195: /* order_by_type: ASC  */
#line 1595 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5590 "parser.cpp"
    break;

  case 196: /* order_by_type: DESC  */
#line 1598 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5598 "parser.cpp"
    break;

  case 197: /* order_by_type: %empty  */
#line 1601 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5606 "parser.cpp"
    break;

  case 198: /* limit_expr: LIMIT expr  */
#line 1605 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5614 "parser.cpp"
    break;

  case 199: /* limit_expr: %empty  */
#line 1609 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5620 "parser.cpp"
    break;

  case 200: /* offset_expr: OFFSET expr  */
#line 1611 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5628 "parser.cpp"
    break;

  case 201: /* offset_expr: %empty  */
#line 1615 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5634 "parser.cpp"
    break;

  case 202: /* distinct: DISTINCT  */
#line 1617 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5642 "parser.cpp"
    break;

  case 203: /* distinct: %empty  */
#line 1620 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5650 "parser.cpp"
    break;

  case 204: /* unnest_clause: UNNEST expr_array  */
#line 1624 "parser.y"
                                 {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5658 "parser.cpp"
    break;

  case 205: /* unnest_clause: %empty  */
#line 1627 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5666 "parser.cpp"
    break;

  case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5674 "parser.cpp"
    break;

  case 207: /* highlight_clause: %empty  */
#line 1634 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5682 "parser.cpp"
    break;

  case 208: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5690 "parser.cpp"
    break;

  case 209: /* from_clause: %empty  */
#line 1641 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5698 "parser.cpp"
    break;

  case 210: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5708 "parser.cpp"
    break;

  case 211: /* search_clause: %empty  */
#line 1650 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5716 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5724 "parser.cpp"
    break;

  case 213: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5732 "parser.cpp"
    break;

  case 214: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5740 "parser.cpp"
    break;

  case 215: /* where_clause: %empty  */
#line 1664 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5748 "parser.cpp"
    break;

  case 216: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5756 "parser.cpp"
    break;

  case 217: /* having_clause: %empty  */
#line 1671 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5764 "parser.cpp"
    break;

  case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5772 "parser.cpp"
    break;

  case 219: /* group_by_clause: %empty  */
#line 1678 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5780 "parser.cpp"
    break;

  case 220: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5788 "parser.cpp"
    break;

  case 221: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5796 "parser.cpp"
    break;

  case 222: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5804 "parser.cpp"
    break;

  case 223: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5812 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5820 "parser.cpp"
    break;

  case 225: /* table_reference: table_reference ',' table_reference_unit  */
#line 1702 "parser.y"
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
#line 5838 "parser.cpp"
    break;

  case 228: /* table_reference_name: table_name table_alias  */
#line 1719 "parser.y"
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
#line 5856 "parser.cpp"
    break;

  case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5867 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5877 "parser.cpp"
    break;

  case 231: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1747 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5889 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5899 "parser.cpp"
    break;

  case 233: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5909 "parser.cpp"
    break;

  case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5920 "parser.cpp"
    break;

  case 235: /* table_alias: %empty  */
#line 1772 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5928 "parser.cpp"
    break;

  case 236: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5936 "parser.cpp"
    break;

  case 237: /* with_clause: %empty  */
#line 1782 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5944 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5953 "parser.cpp"
    break;

  case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5962 "parser.cpp"
    break;

  case 240: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1794 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5974 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1806 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5986 "parser.cpp"
    break;

  case 242: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1813 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5999 "parser.cpp"
    break;

  case 243: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6007 "parser.cpp"
    break;

  case 244: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6015 "parser.cpp"
    break;

  case 245: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6023 "parser.cpp"
    break;

  case 246: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6031 "parser.cpp"
    break;

  case 247: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6039 "parser.cpp"
    break;

  case 248: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6047 "parser.cpp"
    break;

  case 249: /* join_type: %empty  */
#line 1845 "parser.y"
                {
}
#line 6054 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DATABASES  */
#line 1851 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6063 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLES  */
#line 1855 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6072 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW VIEWS  */
#line 1859 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6081 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIGS  */
#line 1863 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6090 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1867 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6102 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW PROFILES  */
#line 1874 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6111 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW BUFFER  */
#line 1878 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6120 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW MEMINDEX  */
#line 1882 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6129 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERIES  */
#line 1886 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6138 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1890 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6148 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1895 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6157 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1899 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6167 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1904 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6176 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1908 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6185 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1912 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6194 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1916 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6205 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1922 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6216 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1928 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6227 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name  */
#line 1934 "parser.y"
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
#line 6243 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1945 "parser.y"
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
#line 6259 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1956 "parser.y"
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
#line 6275 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1967 "parser.y"
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
#line 6292 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1979 "parser.y"
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
#line 6309 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1991 "parser.y"
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
#line 6327 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2004 "parser.y"
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
#line 6346 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2018 "parser.y"
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
#line 6362 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2029 "parser.y"
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
#line 6381 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2043 "parser.y"
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
#line 6402 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2059 "parser.y"
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
#line 6424 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW LOGS  */
#line 2076 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6433 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW DELTA LOGS  */
#line 2080 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6442 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOGS  */
#line 2084 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6451 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2088 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6460 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2092 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6469 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2096 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6480 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY  */
#line 2102 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6489 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2106 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6498 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2110 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6507 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2114 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6518 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOTS  */
#line 2120 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6527 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2124 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6538 "parser.cpp"
    break;

  case 291: /* flush_statement: FLUSH DATA  */
#line 2134 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6547 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH LOG  */
#line 2138 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6556 "parser.cpp"
    break;

  case 293: /* flush_statement: FLUSH BUFFER  */
#line 2142 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6565 "parser.cpp"
    break;

  case 294: /* optimize_statement: OPTIMIZE table_name  */
#line 2150 "parser.y"
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
#line 6580 "parser.cpp"
    break;

  case 295: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2161 "parser.y"
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
#line 6604 "parser.cpp"
    break;

  case 296: /* command_statement: USE IDENTIFIER  */
#line 2184 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6615 "parser.cpp"
    break;

  case 297: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2190 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6625 "parser.cpp"
    break;

  case 298: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2195 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6636 "parser.cpp"
    break;

  case 299: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2201 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6647 "parser.cpp"
    break;

  case 300: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2207 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6660 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2215 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6671 "parser.cpp"
    break;

  case 302: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2221 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6682 "parser.cpp"
    break;

  case 303: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2227 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6693 "parser.cpp"
    break;

  case 304: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2233 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6704 "parser.cpp"
    break;

  case 305: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2239 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6717 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2247 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6728 "parser.cpp"
    break;

  case 307: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2253 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6739 "parser.cpp"
    break;

  case 308: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2259 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6750 "parser.cpp"
    break;

  case 309: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2265 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6761 "parser.cpp"
    break;

  case 310: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2271 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6774 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2279 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6785 "parser.cpp"
    break;

  case 312: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2285 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6796 "parser.cpp"
    break;

  case 313: /* command_statement: LOCK TABLE table_name  */
#line 2291 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6810 "parser.cpp"
    break;

  case 314: /* command_statement: UNLOCK TABLE table_name  */
#line 2300 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6824 "parser.cpp"
    break;

  case 315: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2309 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6837 "parser.cpp"
    break;

  case 316: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2317 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6850 "parser.cpp"
    break;

  case 317: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2325 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6861 "parser.cpp"
    break;

  case 318: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2331 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6872 "parser.cpp"
    break;

  case 319: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2337 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6883 "parser.cpp"
    break;

  case 320: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2343 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6894 "parser.cpp"
    break;

  case 321: /* compact_statement: COMPACT TABLE table_name  */
#line 2350 "parser.y"
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
#line 6911 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2363 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6920 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2367 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6930 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2372 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6941 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2378 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6953 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2385 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6966 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2393 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6980 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2402 "parser.y"
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
#line 6995 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2412 "parser.y"
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
#line 7010 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2422 "parser.y"
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
#line 7026 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2433 "parser.y"
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
#line 7042 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2444 "parser.y"
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
#line 7059 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2456 "parser.y"
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
#line 7076 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2468 "parser.y"
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
#line 7091 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2478 "parser.y"
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
#line 7107 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2489 "parser.y"
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
#line 7124 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2501 "parser.y"
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
#line 7142 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOGS  */
#line 2514 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7151 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2518 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7161 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2523 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7171 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2528 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7182 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2534 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7191 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2538 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7200 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2542 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7211 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2548 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7220 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2552 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7229 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2556 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7240 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2562 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7251 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2568 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7264 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2576 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7275 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODES  */
#line 2582 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7284 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2586 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7295 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW NODE  */
#line 2592 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7304 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2596 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7315 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET ADMIN  */
#line 2602 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7325 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET STANDALONE  */
#line 2607 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7335 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2612 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7347 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2619 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7361 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2628 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7375 "parser.cpp"
    break;

  case 360: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2638 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7389 "parser.cpp"
    break;

  case 361: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2647 "parser.y"
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
#line 7406 "parser.cpp"
    break;

  case 362: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2659 "parser.y"
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
#line 7422 "parser.cpp"
    break;

  case 363: /* expr_array: expr_alias  */
#line 2675 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7431 "parser.cpp"
    break;

  case 364: /* expr_array: expr_array ',' expr_alias  */
#line 2679 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7440 "parser.cpp"
    break;

  case 365: /* insert_row_list: '(' expr_array ')'  */
#line 2684 "parser.y"
                                     {
    auto res = std::make_unique<infinity::InsertRowExpr>();
    for (auto* &expr : *(yyvsp[-1].expr_array_t)) {
        res->values_.emplace_back(expr);
        expr = nullptr;
    }
    delete (yyvsp[-1].expr_array_t);
    (yyval.insert_row_list_t) = new std::vector<infinity::InsertRowExpr*>();
    (yyval.insert_row_list_t)->emplace_back(res.release());
}
#line 7455 "parser.cpp"
    break;

  case 366: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2694 "parser.y"
                                         {
    (yyval.insert_row_list_t) = (yyvsp[-4].insert_row_list_t);
    auto res = std::make_unique<infinity::InsertRowExpr>();
    for (auto* &expr : *(yyvsp[-1].expr_array_t)) {
        res->values_.emplace_back(expr);
        expr = nullptr;
    }
    delete (yyvsp[-1].expr_array_t);
    (yyval.insert_row_list_t)->emplace_back(res.release());
}
#line 7470 "parser.cpp"
    break;

  case 367: /* expr_alias: expr AS IDENTIFIER  */
#line 2716 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7481 "parser.cpp"
    break;

  case 368: /* expr_alias: expr  */
#line 2722 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7489 "parser.cpp"
    break;

  case 374: /* operand: '(' expr ')'  */
#line 2732 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7497 "parser.cpp"
    break;

  case 375: /* operand: '(' select_without_paren ')'  */
#line 2735 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7508 "parser.cpp"
    break;

  case 376: /* operand: constant_expr  */
#line 2741 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7516 "parser.cpp"
    break;

  case 387: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2757 "parser.y"
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
#line 7536 "parser.cpp"
    break;

  case 388: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2773 "parser.y"
                                                                                                                                                   {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-15].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-11].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-11].str_value), (yyvsp[-13].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-9].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-9].str_value));
    // search options
    match_tensor_expr->SetExtraOptions((yyvsp[-7].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-6].expr_t));
    match_tensor_expr->index_name_ = (yyvsp[-1].str_value);
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7557 "parser.cpp"
    break;

  case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2790 "parser.y"
                                                                                                                                 {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-12].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-8].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-8].str_value), (yyvsp[-10].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-6].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-6].str_value));
    // search options
    match_tensor_expr->ignore_index_ = true;
    match_tensor_expr->SetExtraOptions((yyvsp[-4].str_value));
    match_tensor_expr->SetOptionalFilter((yyvsp[-3].expr_t));
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7578 "parser.cpp"
    break;

  case 390: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2808 "parser.y"
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
#line 7628 "parser.cpp"
    break;

  case 391: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2854 "parser.y"
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
#line 7671 "parser.cpp"
    break;

  case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2893 "parser.y"
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
#line 7718 "parser.cpp"
    break;

  case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2936 "parser.y"
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
#line 7766 "parser.cpp"
    break;

  case 394: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2983 "parser.y"
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
#line 7794 "parser.cpp"
    break;

  case 395: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3007 "parser.y"
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
#line 7821 "parser.cpp"
    break;

  case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3030 "parser.y"
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
#line 7846 "parser.cpp"
    break;

  case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3051 "parser.y"
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
#line 7871 "parser.cpp"
    break;

  case 398: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3072 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7885 "parser.cpp"
    break;

  case 399: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3081 "parser.y"
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
#line 7901 "parser.cpp"
    break;

  case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3092 "parser.y"
                                                                                                {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-9].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-7].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-6].expr_t));
    match_text_expr->index_names_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-7].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7917 "parser.cpp"
    break;

  case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3103 "parser.y"
                                                                                                           {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-11].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-9].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-7].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-6].expr_t));
    match_text_expr->index_names_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-7].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7935 "parser.cpp"
    break;

  case 402: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3117 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7947 "parser.cpp"
    break;

  case 403: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3124 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7961 "parser.cpp"
    break;

  case 404: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3134 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7972 "parser.cpp"
    break;

  case 405: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3140 "parser.y"
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
#line 7988 "parser.cpp"
    break;

  case 406: /* sub_search: match_vector_expr  */
#line 3152 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7996 "parser.cpp"
    break;

  case 407: /* sub_search: match_text_expr  */
#line 3155 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8004 "parser.cpp"
    break;

  case 408: /* sub_search: match_tensor_expr  */
#line 3158 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8012 "parser.cpp"
    break;

  case 409: /* sub_search: match_sparse_expr  */
#line 3161 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8020 "parser.cpp"
    break;

  case 410: /* sub_search: query_expr  */
#line 3164 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8028 "parser.cpp"
    break;

  case 411: /* sub_search: fusion_expr  */
#line 3167 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8036 "parser.cpp"
    break;

  case 412: /* sub_search_array: sub_search  */
#line 3171 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8045 "parser.cpp"
    break;

  case 413: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3175 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8054 "parser.cpp"
    break;

  case 414: /* function_expr: IDENTIFIER '(' ')'  */
#line 3180 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8067 "parser.cpp"
    break;

  case 415: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3188 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8080 "parser.cpp"
    break;

  case 416: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3196 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8094 "parser.cpp"
    break;

  case 417: /* function_expr: CURRENTDATE '(' ')'  */
#line 3205 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "currentdate";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_t) = func_expr;
}
#line 8105 "parser.cpp"
    break;

  case 418: /* function_expr: CURRENTDATE  */
#line 3211 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "currentdate";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_t) = func_expr;
}
#line 8116 "parser.cpp"
    break;

  case 419: /* function_expr: YEAR '(' expr ')'  */
#line 3217 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8128 "parser.cpp"
    break;

  case 420: /* function_expr: MONTH '(' expr ')'  */
#line 3224 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8140 "parser.cpp"
    break;

  case 421: /* function_expr: DAY '(' expr ')'  */
#line 3231 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8152 "parser.cpp"
    break;

  case 422: /* function_expr: HOUR '(' expr ')'  */
#line 3238 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8164 "parser.cpp"
    break;

  case 423: /* function_expr: MINUTE '(' expr ')'  */
#line 3245 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8176 "parser.cpp"
    break;

  case 424: /* function_expr: SECOND '(' expr ')'  */
#line 3252 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8188 "parser.cpp"
    break;

  case 425: /* function_expr: operand IS NOT NULLABLE  */
#line 3259 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8200 "parser.cpp"
    break;

  case 426: /* function_expr: operand IS NULLABLE  */
#line 3266 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8212 "parser.cpp"
    break;

  case 427: /* function_expr: NOT operand  */
#line 3273 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8224 "parser.cpp"
    break;

  case 428: /* function_expr: '-' operand  */
#line 3280 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8236 "parser.cpp"
    break;

  case 429: /* function_expr: '+' operand  */
#line 3287 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8248 "parser.cpp"
    break;

  case 430: /* function_expr: operand '-' operand  */
#line 3294 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8261 "parser.cpp"
    break;

  case 431: /* function_expr: operand '+' operand  */
#line 3302 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8274 "parser.cpp"
    break;

  case 432: /* function_expr: operand '*' operand  */
#line 3310 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8287 "parser.cpp"
    break;

  case 433: /* function_expr: operand '/' operand  */
#line 3318 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8300 "parser.cpp"
    break;

  case 434: /* function_expr: operand '%' operand  */
#line 3326 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8313 "parser.cpp"
    break;

  case 435: /* function_expr: operand '=' operand  */
#line 3334 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8326 "parser.cpp"
    break;

  case 436: /* function_expr: operand EQUAL operand  */
#line 3342 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8339 "parser.cpp"
    break;

  case 437: /* function_expr: operand NOT_EQ operand  */
#line 3350 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8352 "parser.cpp"
    break;

  case 438: /* function_expr: operand '<' operand  */
#line 3358 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8365 "parser.cpp"
    break;

  case 439: /* function_expr: operand '>' operand  */
#line 3366 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8378 "parser.cpp"
    break;

  case 440: /* function_expr: operand LESS_EQ operand  */
#line 3374 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8391 "parser.cpp"
    break;

  case 441: /* function_expr: operand GREATER_EQ operand  */
#line 3382 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8404 "parser.cpp"
    break;

  case 442: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3390 "parser.y"
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
#line 8439 "parser.cpp"
    break;

  case 443: /* function_expr: operand LIKE operand  */
#line 3420 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8452 "parser.cpp"
    break;

  case 444: /* function_expr: operand NOT LIKE operand  */
#line 3428 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8465 "parser.cpp"
    break;

  case 445: /* conjunction_expr: expr AND expr  */
#line 3437 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8478 "parser.cpp"
    break;

  case 446: /* conjunction_expr: expr OR expr  */
#line 3445 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8491 "parser.cpp"
    break;

  case 447: /* between_expr: operand BETWEEN operand AND operand  */
#line 3454 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8503 "parser.cpp"
    break;

  case 448: /* in_expr: operand IN '(' expr_array ')'  */
#line 3462 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8514 "parser.cpp"
    break;

  case 449: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3468 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8525 "parser.cpp"
    break;

  case 450: /* case_expr: CASE expr case_check_array END  */
#line 3475 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8536 "parser.cpp"
    break;

  case 451: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3481 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8548 "parser.cpp"
    break;

  case 452: /* case_expr: CASE case_check_array END  */
#line 3488 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8558 "parser.cpp"
    break;

  case 453: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3493 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8569 "parser.cpp"
    break;

  case 454: /* case_check_array: WHEN expr THEN expr  */
#line 3500 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8581 "parser.cpp"
    break;

  case 455: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3507 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8593 "parser.cpp"
    break;

  case 456: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3515 "parser.y"
                                            {
    auto [data_type_result, fail_reason] = infinity::ColumnType::GetDataTypeFromColumnType(*((yyvsp[-1].column_type_t)), std::vector<std::unique_ptr<infinity::InitParameter>>{});
    delete (yyvsp[-1].column_type_t);
    if (!data_type_result) {
        yyerror(&yyloc, scanner, result, fail_reason.c_str());
        delete (yyvsp[-3].expr_t);
        YYERROR;
    }
    infinity::CastExpr* cast_expr = new infinity::CastExpr(std::move(*data_type_result));
    cast_expr->expr_ = (yyvsp[-3].expr_t);
    (yyval.expr_t) = cast_expr;
}
#line 8610 "parser.cpp"
    break;

  case 457: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3528 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8621 "parser.cpp"
    break;

  case 458: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3534 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8632 "parser.cpp"
    break;

  case 459: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3540 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8644 "parser.cpp"
    break;

  case 460: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3547 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8656 "parser.cpp"
    break;

  case 461: /* column_expr: IDENTIFIER  */
#line 3555 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8668 "parser.cpp"
    break;

  case 462: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3562 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8680 "parser.cpp"
    break;

  case 463: /* column_expr: '*'  */
#line 3569 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8690 "parser.cpp"
    break;

  case 464: /* column_expr: column_expr '.' '*'  */
#line 3574 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8704 "parser.cpp"
    break;

  case 465: /* constant_expr: STRING  */
#line 3584 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8714 "parser.cpp"
    break;

  case 466: /* constant_expr: TRUE  */
#line 3589 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8724 "parser.cpp"
    break;

  case 467: /* constant_expr: FALSE  */
#line 3594 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8734 "parser.cpp"
    break;

  case 468: /* constant_expr: DOUBLE_VALUE  */
#line 3599 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8744 "parser.cpp"
    break;

  case 469: /* constant_expr: LONG_VALUE  */
#line 3604 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8754 "parser.cpp"
    break;

  case 470: /* constant_expr: DATE STRING  */
#line 3609 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8764 "parser.cpp"
    break;

  case 471: /* constant_expr: TIME STRING  */
#line 3614 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8774 "parser.cpp"
    break;

  case 472: /* constant_expr: DATETIME STRING  */
#line 3619 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8784 "parser.cpp"
    break;

  case 473: /* constant_expr: TIMESTAMP STRING  */
#line 3624 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8794 "parser.cpp"
    break;

  case 474: /* constant_expr: INTERVAL interval_expr  */
#line 3629 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8802 "parser.cpp"
    break;

  case 475: /* constant_expr: interval_expr  */
#line 3632 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8810 "parser.cpp"
    break;

  case 476: /* constant_expr: common_array_expr  */
#line 3635 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8818 "parser.cpp"
    break;

  case 477: /* constant_expr: curly_brackets_expr  */
#line 3638 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8826 "parser.cpp"
    break;

  case 478: /* common_array_expr: array_expr  */
#line 3642 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8834 "parser.cpp"
    break;

  case 479: /* common_array_expr: subarray_array_expr  */
#line 3645 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8842 "parser.cpp"
    break;

  case 480: /* common_array_expr: sparse_array_expr  */
#line 3648 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8850 "parser.cpp"
    break;

  case 481: /* common_array_expr: empty_array_expr  */
#line 3651 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8858 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: sparse_array_expr  */
#line 3655 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8866 "parser.cpp"
    break;

  case 483: /* common_sparse_array_expr: array_expr  */
#line 3658 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8874 "parser.cpp"
    break;

  case 484: /* common_sparse_array_expr: empty_array_expr  */
#line 3661 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8882 "parser.cpp"
    break;

  case 485: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3665 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8890 "parser.cpp"
    break;

  case 486: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3669 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8900 "parser.cpp"
    break;

  case 487: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3674 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8909 "parser.cpp"
    break;

  case 488: /* sparse_array_expr: long_sparse_array_expr  */
#line 3679 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8917 "parser.cpp"
    break;

  case 489: /* sparse_array_expr: double_sparse_array_expr  */
#line 3682 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8925 "parser.cpp"
    break;

  case 490: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3686 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8933 "parser.cpp"
    break;

  case 491: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3690 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8945 "parser.cpp"
    break;

  case 492: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3697 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8956 "parser.cpp"
    break;

  case 493: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3704 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8964 "parser.cpp"
    break;

  case 494: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3708 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8976 "parser.cpp"
    break;

  case 495: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3715 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8987 "parser.cpp"
    break;

  case 496: /* empty_array_expr: '[' ']'  */
#line 3722 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8995 "parser.cpp"
    break;

  case 497: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3726 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9003 "parser.cpp"
    break;

  case 498: /* curly_brackets_expr: '{' '}'  */
#line 3729 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9011 "parser.cpp"
    break;

  case 499: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3733 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9020 "parser.cpp"
    break;

  case 500: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3737 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9029 "parser.cpp"
    break;

  case 501: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3742 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9037 "parser.cpp"
    break;

  case 502: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3746 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9045 "parser.cpp"
    break;

  case 503: /* array_expr: long_array_expr  */
#line 3750 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9053 "parser.cpp"
    break;

  case 504: /* array_expr: double_array_expr  */
#line 3753 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9061 "parser.cpp"
    break;

  case 505: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3757 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9069 "parser.cpp"
    break;

  case 506: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3761 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9079 "parser.cpp"
    break;

  case 507: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3766 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9088 "parser.cpp"
    break;

  case 508: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3771 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9096 "parser.cpp"
    break;

  case 509: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3775 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9106 "parser.cpp"
    break;

  case 510: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3780 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9115 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE SECONDS  */
#line 3785 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9126 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE SECOND  */
#line 3791 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9137 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE MINUTES  */
#line 3797 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9148 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE MINUTE  */
#line 3803 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9159 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE HOURS  */
#line 3809 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9170 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE HOUR  */
#line 3815 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9181 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE DAYS  */
#line 3821 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9192 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE DAY  */
#line 3827 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9203 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE MONTHS  */
#line 3833 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9214 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE MONTH  */
#line 3839 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9225 "parser.cpp"
    break;

  case 521: /* interval_expr: LONG_VALUE YEARS  */
#line 3845 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9236 "parser.cpp"
    break;

  case 522: /* interval_expr: LONG_VALUE YEAR  */
#line 3851 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9247 "parser.cpp"
    break;

  case 523: /* copy_option_list: copy_option  */
#line 3862 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9256 "parser.cpp"
    break;

  case 524: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3866 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9265 "parser.cpp"
    break;

  case 525: /* copy_option: FORMAT IDENTIFIER  */
#line 3871 "parser.y"
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
#line 9301 "parser.cpp"
    break;

  case 526: /* copy_option: DELIMITER STRING  */
#line 3902 "parser.y"
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
#line 9316 "parser.cpp"
    break;

  case 527: /* copy_option: HEADER  */
#line 3912 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9326 "parser.cpp"
    break;

  case 528: /* copy_option: OFFSET LONG_VALUE  */
#line 3917 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9336 "parser.cpp"
    break;

  case 529: /* copy_option: LIMIT LONG_VALUE  */
#line 3922 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9346 "parser.cpp"
    break;

  case 530: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3927 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9356 "parser.cpp"
    break;

  case 531: /* file_path: STRING  */
#line 3933 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9364 "parser.cpp"
    break;

  case 532: /* if_exists: IF EXISTS  */
#line 3937 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9370 "parser.cpp"
    break;

  case 533: /* if_exists: %empty  */
#line 3938 "parser.y"
  { (yyval.bool_value) = false; }
#line 9376 "parser.cpp"
    break;

  case 534: /* if_not_exists: IF NOT EXISTS  */
#line 3940 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9382 "parser.cpp"
    break;

  case 535: /* if_not_exists: %empty  */
#line 3941 "parser.y"
  { (yyval.bool_value) = false; }
#line 9388 "parser.cpp"
    break;

  case 538: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3956 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9401 "parser.cpp"
    break;

  case 539: /* if_not_exists_info: %empty  */
#line 3964 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9409 "parser.cpp"
    break;

  case 540: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3968 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9417 "parser.cpp"
    break;

  case 541: /* with_index_param_list: %empty  */
#line 3971 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9425 "parser.cpp"
    break;

  case 542: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3975 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9433 "parser.cpp"
    break;

  case 543: /* optional_table_properties_list: %empty  */
#line 3978 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9441 "parser.cpp"
    break;

  case 544: /* index_param_list: index_param  */
#line 3982 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9450 "parser.cpp"
    break;

  case 545: /* index_param_list: index_param_list ',' index_param  */
#line 3986 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9459 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER  */
#line 3991 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9470 "parser.cpp"
    break;

  case 547: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3997 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9485 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' STRING  */
#line 4007 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9500 "parser.cpp"
    break;

  case 549: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4017 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9513 "parser.cpp"
    break;

  case 550: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4025 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9526 "parser.cpp"
    break;

  case 551: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4036 "parser.y"
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
#line 9562 "parser.cpp"
    break;

  case 552: /* index_info: '(' IDENTIFIER ')'  */
#line 4067 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9573 "parser.cpp"
    break;


#line 9577 "parser.cpp"

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

#line 4074 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

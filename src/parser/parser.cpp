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
  YYSYMBOL_NUMBER = 208,                   /* NUMBER  */
  YYSYMBOL_209_ = 209,                     /* '='  */
  YYSYMBOL_210_ = 210,                     /* '<'  */
  YYSYMBOL_211_ = 211,                     /* '>'  */
  YYSYMBOL_212_ = 212,                     /* '+'  */
  YYSYMBOL_213_ = 213,                     /* '-'  */
  YYSYMBOL_214_ = 214,                     /* '*'  */
  YYSYMBOL_215_ = 215,                     /* '/'  */
  YYSYMBOL_216_ = 216,                     /* '%'  */
  YYSYMBOL_217_ = 217,                     /* '['  */
  YYSYMBOL_218_ = 218,                     /* ']'  */
  YYSYMBOL_219_ = 219,                     /* '('  */
  YYSYMBOL_220_ = 220,                     /* ')'  */
  YYSYMBOL_221_ = 221,                     /* '.'  */
  YYSYMBOL_222_ = 222,                     /* ';'  */
  YYSYMBOL_223_ = 223,                     /* ','  */
  YYSYMBOL_224_ = 224,                     /* '}'  */
  YYSYMBOL_225_ = 225,                     /* '{'  */
  YYSYMBOL_226_ = 226,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 227,                 /* $accept  */
  YYSYMBOL_input_pattern = 228,            /* input_pattern  */
  YYSYMBOL_statement_list = 229,           /* statement_list  */
  YYSYMBOL_statement = 230,                /* statement  */
  YYSYMBOL_explainable_statement = 231,    /* explainable_statement  */
  YYSYMBOL_create_statement = 232,         /* create_statement  */
  YYSYMBOL_table_element_array = 233,      /* table_element_array  */
  YYSYMBOL_column_def_array = 234,         /* column_def_array  */
  YYSYMBOL_table_element = 235,            /* table_element  */
  YYSYMBOL_table_column = 236,             /* table_column  */
  YYSYMBOL_column_type_array = 237,        /* column_type_array  */
  YYSYMBOL_column_type = 238,              /* column_type  */
  YYSYMBOL_column_constraints = 239,       /* column_constraints  */
  YYSYMBOL_column_constraint = 240,        /* column_constraint  */
  YYSYMBOL_default_expr = 241,             /* default_expr  */
  YYSYMBOL_table_constraint = 242,         /* table_constraint  */
  YYSYMBOL_identifier_array = 243,         /* identifier_array  */
  YYSYMBOL_delete_statement = 244,         /* delete_statement  */
  YYSYMBOL_insert_statement = 245,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 246, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 247,        /* explain_statement  */
  YYSYMBOL_update_statement = 248,         /* update_statement  */
  YYSYMBOL_update_expr_array = 249,        /* update_expr_array  */
  YYSYMBOL_update_expr = 250,              /* update_expr  */
  YYSYMBOL_drop_statement = 251,           /* drop_statement  */
  YYSYMBOL_copy_statement = 252,           /* copy_statement  */
  YYSYMBOL_select_statement = 253,         /* select_statement  */
  YYSYMBOL_select_with_paren = 254,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 255,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 256, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 257, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 258, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 259,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 260,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 261,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 262,            /* order_by_type  */
  YYSYMBOL_limit_expr = 263,               /* limit_expr  */
  YYSYMBOL_offset_expr = 264,              /* offset_expr  */
  YYSYMBOL_distinct = 265,                 /* distinct  */
  YYSYMBOL_unnest_clause = 266,            /* unnest_clause  */
  YYSYMBOL_highlight_clause = 267,         /* highlight_clause  */
  YYSYMBOL_from_clause = 268,              /* from_clause  */
  YYSYMBOL_search_clause = 269,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 270, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 271,             /* where_clause  */
  YYSYMBOL_having_clause = 272,            /* having_clause  */
  YYSYMBOL_group_by_clause = 273,          /* group_by_clause  */
  YYSYMBOL_set_operator = 274,             /* set_operator  */
  YYSYMBOL_table_reference = 275,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 276,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 277,     /* table_reference_name  */
  YYSYMBOL_table_name = 278,               /* table_name  */
  YYSYMBOL_table_alias = 279,              /* table_alias  */
  YYSYMBOL_with_clause = 280,              /* with_clause  */
  YYSYMBOL_with_expr_list = 281,           /* with_expr_list  */
  YYSYMBOL_with_expr = 282,                /* with_expr  */
  YYSYMBOL_join_clause = 283,              /* join_clause  */
  YYSYMBOL_join_type = 284,                /* join_type  */
  YYSYMBOL_show_statement = 285,           /* show_statement  */
  YYSYMBOL_flush_statement = 286,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 287,       /* optimize_statement  */
  YYSYMBOL_command_statement = 288,        /* command_statement  */
  YYSYMBOL_compact_statement = 289,        /* compact_statement  */
  YYSYMBOL_admin_statement = 290,          /* admin_statement  */
  YYSYMBOL_alter_statement = 291,          /* alter_statement  */
  YYSYMBOL_expr_array = 292,               /* expr_array  */
  YYSYMBOL_insert_row_list = 293,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 294,               /* expr_alias  */
  YYSYMBOL_expr = 295,                     /* expr  */
  YYSYMBOL_operand = 296,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 297,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 298,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 299,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 300,          /* match_text_expr  */
  YYSYMBOL_query_expr = 301,               /* query_expr  */
  YYSYMBOL_fusion_expr = 302,              /* fusion_expr  */
  YYSYMBOL_sub_search = 303,               /* sub_search  */
  YYSYMBOL_sub_search_array = 304,         /* sub_search_array  */
  YYSYMBOL_function_expr = 305,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 306,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 307,             /* between_expr  */
  YYSYMBOL_in_expr = 308,                  /* in_expr  */
  YYSYMBOL_case_expr = 309,                /* case_expr  */
  YYSYMBOL_case_check_array = 310,         /* case_check_array  */
  YYSYMBOL_cast_expr = 311,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 312,            /* subquery_expr  */
  YYSYMBOL_column_expr = 313,              /* column_expr  */
  YYSYMBOL_constant_expr = 314,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 315,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 316, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 317,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 318, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 319,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 320,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 321, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 322, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 323, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 324,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 325,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 326, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 327,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 328,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 329,               /* array_expr  */
  YYSYMBOL_long_array_expr = 330,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 331, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 332,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 333, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 334,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 335,         /* copy_option_list  */
  YYSYMBOL_copy_option = 336,              /* copy_option  */
  YYSYMBOL_file_path = 337,                /* file_path  */
  YYSYMBOL_if_exists = 338,                /* if_exists  */
  YYSYMBOL_if_not_exists = 339,            /* if_not_exists  */
  YYSYMBOL_semicolon = 340,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 341,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 342,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 343, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 344,         /* index_param_list  */
  YYSYMBOL_index_param = 345,              /* index_param  */
  YYSYMBOL_index_info = 346                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 475 "parser.cpp"

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
#define YYLAST   1562

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  227
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  550
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1253

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   463


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
       2,     2,     2,     2,     2,     2,     2,   216,     2,     2,
     219,   220,   214,   212,   223,   213,   221,   215,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   226,   222,
     210,   209,   211,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   217,     2,   218,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   225,     2,   224,     2,     2,     2,     2,
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
     205,   206,   207,   208
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
    3164,  3167,  3171,  3175,  3180,  3188,  3196,  3205,  3212,  3219,
    3226,  3233,  3240,  3247,  3254,  3261,  3268,  3275,  3282,  3290,
    3298,  3306,  3314,  3322,  3330,  3338,  3346,  3354,  3362,  3370,
    3378,  3408,  3416,  3425,  3433,  3442,  3450,  3456,  3463,  3469,
    3476,  3481,  3488,  3495,  3503,  3516,  3522,  3528,  3535,  3543,
    3550,  3557,  3562,  3572,  3577,  3582,  3587,  3592,  3597,  3602,
    3607,  3612,  3617,  3620,  3623,  3626,  3630,  3633,  3636,  3639,
    3643,  3646,  3649,  3653,  3657,  3662,  3667,  3670,  3674,  3678,
    3685,  3692,  3696,  3703,  3710,  3714,  3717,  3721,  3725,  3730,
    3734,  3738,  3741,  3745,  3749,  3754,  3759,  3763,  3768,  3773,
    3779,  3785,  3791,  3797,  3803,  3809,  3815,  3821,  3827,  3833,
    3839,  3850,  3854,  3859,  3890,  3900,  3905,  3910,  3915,  3921,
    3925,  3926,  3928,  3929,  3931,  3932,  3944,  3952,  3956,  3959,
    3963,  3966,  3970,  3974,  3979,  3985,  3995,  4005,  4013,  4024,
    4055
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
  "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "HISTORY", "NUMBER", "'='",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('",
  "')'", "'.'", "';'", "','", "'}'", "'{'", "':'", "$accept",
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

#define YYPACT_NINF (-775)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-538)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1015,    39,    43,   101,    62,    27,    62,   207,   990,   935,
      76,    84,   133,   137,   470,   130,   176,   187,    20,   114,
     447,   -38,   211,    31,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,   404,  -775,  -775,   199,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,   196,   196,   196,   196,   225,   260,    62,
     262,   262,   262,   262,   262,   347,   134,   369,    62,   -23,
     392,   399,   407,  1040,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,   404,  -775,  -775,  -775,  -775,  -775,   208,   417,    62,
    -775,  -775,  -775,  -775,  -775,    24,  -775,   -84,   322,  -775,
     428,  -775,   263,  -775,  -775,   449,  -775,   486,  -775,   537,
     353,    62,    62,    62,    62,  -775,  -775,  -775,  -775,   -29,
    -775,   451,   305,  -775,   531,   345,   351,   302,   355,   358,
     573,   384,   505,   405,   425,   418,   420,  -775,   934,  -775,
     583,  -775,  -775,    36,   577,  -775,   579,   571,   650,    62,
      62,    62,   661,   606,   614,   459,   608,   688,    62,    62,
      62,   694,  -775,   701,   710,   651,   723,   723,   586,    82,
      90,    99,  -775,   509,  -775,   435,  -775,  -775,  -775,   728,
    -775,   730,  -775,  -775,  -775,  -775,   733,  -775,  -775,  -775,
    -775,    80,  -775,  -775,  -775,    62,   546,   187,   723,  -775,
     765,  -775,   610,  -775,   772,  -775,  -775,   768,  -775,  -775,
     773,  -775,   776,   778,  -775,   782,   727,   783,   591,   787,
     788,  -775,  -775,  -775,  -775,    36,  -775,  -775,  -775,   586,
     739,   725,   720,   660,   -31,  -775,   459,  -775,    62,   331,
     793,    59,  -775,  -775,  -775,  -775,  -775,   735,  -775,   592,
     -53,  -775,   586,  -775,  -775,   724,   726,   587,  -775,  -775,
     303,   681,   588,   589,    95,   805,   806,   808,   809,  -775,
    -775,   810,   595,   596,   598,   603,   605,   607,   609,   311,
     611,   613,   770,   770,  -775,    12,   497,   180,   273,  -775,
     -26,  1010,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,   612,  -775,  -775,  -775,   151,
    -775,  -775,   167,  -775,   171,  -775,  -775,   341,  -775,  -775,
     239,  -775,   259,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
     836,   838,  -775,  -775,  -775,  -775,  -775,  -775,   798,   799,
     774,   769,   199,  -775,  -775,  -775,   848,   313,  -775,   850,
    -775,  -775,   777,   382,  -775,   854,  -775,  -775,   639,   640,
     -36,   586,   586,   795,  -775,   858,   -38,    73,   811,   649,
     866,   868,  -775,  -775,   309,   656,  -775,    62,   586,   710,
    -775,   430,   657,   658,   326,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,   770,   662,   414,
     801,   586,   586,    -4,   385,  -775,  -775,  -775,  -775,   303,
    -775,   586,   586,   586,   586,   586,   586,   878,   664,   665,
     666,   667,   883,   884,   528,   528,  -775,   663,  -775,  -775,
    -775,  -775,   670,    51,  -775,  -775,   814,   586,   889,   586,
     586,   -43,   677,   -13,   770,   770,   770,   770,   770,   770,
     770,   770,   770,   770,   770,   770,   770,   770,    44,  -775,
     680,  -775,   893,  -775,   895,   269,  -775,  -775,   896,  -775,
     898,   861,   552,   686,   692,   910,   696,  -775,   697,  -775,
     913,  -775,   328,   917,   755,   756,  -775,  -775,  -775,   586,
     849,   704,  -775,   191,   430,   586,  -775,  -775,    29,  1179,
     794,   709,   321,  -775,  -775,  -775,   -38,   923,   796,  -775,
    -775,  -775,   927,   586,   712,  -775,   430,  -775,     2,     2,
     586,  -775,   373,   801,   779,   713,   -10,   -12,   421,  -775,
     586,   586,   108,   132,   147,   158,   164,   168,   856,   586,
      45,   586,   932,   717,   383,   714,  -775,  -775,   723,  -775,
    -775,  -775,   780,   734,   770,   497,   823,  -775,  1052,  1052,
     202,   202,  1042,  1052,  1052,   202,   202,   528,   528,  -775,
    -775,  -775,  -775,  -775,  -775,   716,  -775,   736,  -775,  -775,
    -775,  -775,   954,   955,  -775,   793,   963,  -775,   964,  -775,
    -775,   966,  -775,  -775,   969,   970,   747,    28,   886,   586,
    -775,  -775,  -775,   430,   974,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,   760,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,   761,
     762,   766,   767,   771,   775,   786,   789,   290,   790,   793,
     956,    73,   404,   791,   987,  -775,   396,   797,   986,   992,
    1003,   993,  -775,  1004,   398,  -775,   432,   434,  -775,   803,
    -775,  1179,   586,  -775,   586,   -49,    -1,  -775,  -775,  -775,
    -775,  -775,  -775,   770,   -96,  -775,   238,    70,   812,   116,
     813,  -775,  1013,  -775,  -775,   940,   497,  1052,   816,   438,
    -775,   770,  1028,  1036,   998,   995,   442,   443,  -775,   828,
     450,  -775,  1037,  -775,  -775,   -38,   832,   551,  -775,    74,
    -775,   586,   876,  -775,  -775,  1053,  1179,  1179,   743,   983,
    1032,  1215,  1232,  1255,   926,   930,  -775,  -775,   213,  -775,
     928,   793,   452,   842,   931,  -775,   900,  -775,  -775,   586,
    -775,  -775,  -775,  -775,  -775,  -775,     2,  -775,  -775,  -775,
     852,   430,   -15,  -775,   586,   547,   862,   680,   863,  1069,
     864,   586,  -775,   871,   865,   872,   454,  -775,  -775,   414,
    1080,  1088,  -775,  -775,   963,   520,  -775,   964,   477,    30,
      28,  1038,  -775,  -775,  -775,  -775,  -775,  -775,  1039,  -775,
    1094,  -775,   747,   332,   651,   472,   882,   473,  -775,   888,
     894,   905,   906,   912,   918,   919,   921,   922,  1023,   924,
     925,   929,   933,   937,   944,   945,   946,   948,   949,  1056,
     950,   951,   953,   957,   958,   959,   960,   961,   965,   968,
    1057,   975,   977,   978,   985,   991,   994,  1005,  1006,  1007,
    1008,  1058,  1009,  1012,  1014,  1016,  1018,  1019,  1020,  1021,
    1022,  1024,  1061,  1025,  1026,  1027,  1063,  1070,  1071,  1073,
    1074,  1089,  1092,  1062,  1093,  -775,  -775,   269,  -775,   971,
    1011,   483,  -775,   964,  1117,  1154,   485,  -775,  -775,  -775,
     430,  -775,   741,  1095,  1096,    33,  1105,  -775,  -775,  -775,
    1106,  1111,   938,   430,  -775,     2,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  1172,  -775,    74,   551,
      28,    28,   973,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    1109,  1127,  -775,  1181,  -775,  -775,  1179,  1183,  1187,  1204,
    1207,  1210,  1212,  1221,  1227,  1230,  1110,  1240,  1254,  1305,
    1311,  1328,  1329,  1330,  1334,  1335,  1337,  1121,  1339,  1345,
    1346,  1349,  1350,  1351,  1352,  1357,  1358,  1359,  1143,  1361,
    1362,  1363,  1364,  1365,  1366,  1367,  1368,  1369,  1370,  1155,
    1371,  1373,  1374,  1375,  1377,  1378,  1379,  1380,  1381,  1382,
    1166,  1384,  1385,  1386,  1387,  1388,  1389,  1390,  1391,  1392,
    1393,  1177,  1395,  -775,  1234,  1398,  -775,   492,  -775,   769,
    -775,  -775,  1399,  1400,  1401,   201,  1186,  -775,   498,  1402,
    -775,  -775,  1347,   793,   332,  1354,  1356,  1191,  -775,  1192,
    1193,  1194,  1195,  1196,  1197,  1198,  1199,  1200,  1415,  1202,
    1203,  1205,  1206,  1208,  1209,  1211,  1213,  1214,  1216,  1418,
    1217,  1218,  1219,  1220,  1222,  1223,  1224,  1225,  1226,  1228,
    1421,  1229,  1231,  1233,  1235,  1236,  1237,  1238,  1239,  1241,
    1242,  1424,  1243,  1244,  1245,  1246,  1247,  1248,  1249,  1250,
    1251,  1252,  1426,  1253,  1256,  1257,  1258,  1259,  1260,  1261,
    1262,  1263,  1264,  1429,  1265,  -775,  -775,  -775,  -775,  1266,
    1267,  1268,   864,  1274,  -775,   514,   586,   515,  -775,   586,
     586,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  1272,  -775,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  1273,  -775,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  1275,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  1276,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  1277,  -775,  -775,  -775,
    -775,  -775,  -775,  -775,  -775,  -775,  -775,  1278,  -775,  1443,
    1446,    49,  1279,  1280,  1403,  1448,  1454,  -775,  -775,  -775,
     430,  -775,   747,   430,  -775,  -775,  -775,  -775,  -775,  -775,
    1271,  1281,   864,   769,  1307,  1269,  1469,   703,    58,  1282,
    1482,  1283,  -775,  1438,  1497,   707,  1499,  -775,   864,   769,
     864,   -42,  1287,  1288,  1501,  -775,  1461,  1290,  -775,  1291,
    1472,  1473,  -775,  1510,  -775,  -775,  -775,   128,   144,  -775,
    1296,  1297,  1476,  1478,  -775,  1479,  1480,  1518,  -775,  -775,
    1303,  -775,  1304,  1306,  1521,  1522,   769,  1308,  1309,  -775,
     769,  -775,  -775
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     237,     0,     0,     0,     0,     0,     0,     0,   237,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   237,     0,   535,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   180,   179,     0,     8,    14,    15,    16,
      17,    18,    19,   533,   533,   533,   533,   533,     0,     0,
     531,   531,   531,   531,   531,     0,   230,     0,     0,     0,
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
       0,   199,     0,    33,     0,    34,   164,   536,     0,     0,
       0,   237,   530,   171,   173,   172,   174,     0,   231,     0,
     215,   168,     0,   160,   529,     0,     0,   459,   463,   466,
     467,     0,     0,     0,     0,     0,     0,     0,     0,   464,
     465,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   461,     0,   237,     0,     0,   363,
     368,   369,   383,   381,   384,   382,   385,   386,   378,   373,
     372,   371,   379,   380,   370,   377,   376,   474,   477,     0,
     478,   486,     0,   487,     0,   479,   475,     0,   476,   501,
       0,   502,     0,   473,   300,   302,   301,   298,   299,   305,
     307,   306,   303,   304,   310,   312,   311,   308,   309,   288,
       0,     0,   270,   269,   275,   265,   266,   284,     0,     0,
       0,   539,     0,   239,   297,   348,     0,   339,   344,   323,
     352,   347,     0,     0,   354,     0,   319,   320,     0,     0,
     207,     0,     0,   201,   532,     0,   237,     0,     0,     0,
       0,     0,   317,   158,     0,     0,   162,     0,     0,     0,
     167,   214,     0,     0,     0,   510,   509,   512,   511,   514,
     513,   516,   515,   518,   517,   520,   519,     0,     0,   425,
     237,     0,     0,     0,     0,   468,   469,   470,   471,     0,
     472,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   427,   426,   507,   504,   494,   484,
     489,   492,     0,     0,   496,   497,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   483,
       0,   488,     0,   491,     0,     0,   495,   503,     0,   506,
       0,   276,   271,     0,     0,     0,     0,   295,     0,   357,
       0,   340,     0,     0,     0,     0,   350,   188,   187,     0,
     209,   190,   192,   197,   198,     0,   186,    32,    36,     0,
       0,     0,     0,    42,    46,    47,   237,     0,    40,   316,
     315,   163,     0,     0,   161,   175,   170,   169,     0,     0,
       0,   414,     0,   237,     0,     0,     0,     0,     0,   450,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   213,     0,     0,   375,   374,     0,   364,
     367,   443,   444,     0,     0,   237,     0,   424,   434,   435,
     438,   439,     0,   441,   433,   436,   437,   429,   428,   430,
     431,   432,   460,   462,   485,     0,   490,     0,   493,   498,
     505,   508,     0,     0,   272,     0,     0,   360,     0,   240,
     341,     0,   324,   349,     0,     0,   206,     0,   205,     0,
     195,   196,   194,   200,     0,    54,    57,    58,    55,    56,
      59,    60,    76,    61,    63,    62,    79,    66,    67,    68,
      64,    65,    69,    70,    71,    72,    73,    74,    75,     0,
       0,     0,     0,     0,     0,     0,     0,   539,     0,     0,
     541,     0,    39,     0,     0,   159,     0,     0,     0,     0,
       0,     0,   525,     0,     0,   521,     0,     0,   415,     0,
     455,     0,     0,   448,     0,     0,     0,   422,   421,   420,
     419,   418,   417,     0,     0,   459,     0,     0,     0,     0,
       0,   404,     0,   500,   499,     0,   237,   442,     0,     0,
     423,     0,     0,     0,   277,   273,     0,     0,    44,   544,
       0,   542,   325,   358,   359,   237,   208,   224,   226,   235,
     227,     0,   211,   193,    38,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   151,   152,   155,   148,
     155,     0,     0,     0,    35,    43,   550,    41,   365,     0,
     527,   526,   524,   523,   528,   178,     0,   176,   416,   456,
       0,   452,     0,   451,     0,     0,     0,     0,     0,     0,
     213,     0,   402,     0,     0,     0,     0,   457,   446,   445,
       0,     0,   362,   361,     0,     0,   538,     0,     0,     0,
       0,     0,   244,   245,   246,   247,   243,   248,     0,   233,
       0,   228,   204,     0,   215,     0,     0,     0,    52,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   153,   150,     0,   149,    49,
      48,     0,   157,     0,     0,     0,     0,   522,   454,   449,
     453,   440,     0,     0,     0,     0,     0,   480,   482,   481,
     213,     0,     0,   212,   405,     0,   458,   447,   278,   274,
      45,   545,   546,   548,   547,   543,     0,   326,   235,   225,
       0,     0,   232,   408,   406,   409,   407,   410,   411,   412,
     210,   219,    78,     0,   146,   147,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   154,     0,     0,   156,     0,    37,   539,
     366,   504,     0,     0,     0,     0,     0,   403,     0,   327,
     229,   241,     0,     0,     0,     0,   217,     0,    53,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    51,    50,   540,   549,     0,
       0,   213,   213,   398,   177,     0,     0,     0,   413,     0,
       0,   189,    77,    83,    84,    81,    82,    85,    86,    87,
      88,    89,     0,    80,   127,   128,   125,   126,   129,   130,
     131,   132,   133,     0,   124,    94,    95,    92,    93,    96,
      97,    98,    99,   100,     0,    91,   105,   106,   103,   104,
     107,   108,   109,   110,   111,     0,   102,   138,   139,   136,
     137,   140,   141,   142,   143,   144,     0,   135,   116,   117,
     114,   115,   118,   119,   120,   121,   122,     0,   113,     0,
       0,     0,     0,     0,     0,     0,     0,   329,   328,   334,
     242,   234,   218,   216,    90,   134,   101,   112,   145,   123,
     213,     0,   213,   539,   399,     0,   335,   330,     0,     0,
       0,     0,   397,     0,     0,     0,     0,   331,   213,   539,
     213,   539,     0,     0,     0,   336,   332,     0,   393,     0,
       0,     0,   396,     0,   400,   337,   333,   539,   387,   395,
       0,     0,     0,     0,   392,     0,     0,     0,   401,   391,
       0,   389,     0,     0,     0,     0,   539,     0,     0,   394,
     539,   388,   390
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -775,  -775,  -775,  1404,  1464,   264,  -775,  -775,   890,  -564,
    -775,  -650,  -775,   802,   804,  -775,  -576,   270,   288,  1310,
    -775,   291,  -775,  1156,   295,   316,    -7,  1512,   -19,  1201,
    1322,   -99,  -775,  -775,   939,  -775,  -775,  -775,  -775,  -775,
    -775,  -775,  -775,  -752,  -233,  -775,  -775,  -775,  -775,   759,
    -158,    10,   632,  -775,  -775,  1355,  -775,  -775,   329,   330,
     333,   335,   338,  -775,  -775,  -219,  -775,  1104,  -239,  -231,
    -774,  -772,  -770,  -768,  -766,  -765,   530,  -775,  -775,  -775,
    -775,  -775,  -775,  1142,  -775,  -775,  1017,  -272,  -269,  -775,
    -775,  -775,   792,  -775,  -775,  -775,  -775,   800,  -775,  -775,
    1084,  1083,  -237,  -775,  -775,  -775,  -775,  1292,  -509,   815,
    -145,   532,   604,  -775,  -775,  -633,  -775,   675,   785,  -775
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    22,    23,    24,    64,    25,   502,   697,   503,   504,
     797,   637,   728,   729,   869,   505,   374,    26,    27,   231,
      28,    29,   240,   241,    30,    31,    32,    33,    34,   135,
     216,   136,   221,   491,   492,   602,   363,   496,   219,   712,
     490,   598,   794,   680,   243,  1101,  1016,   133,   706,   707,
     708,   709,   791,    35,   112,   113,   710,   788,    36,    37,
      38,    39,    40,    41,    42,   278,   514,   279,   280,   281,
     282,   283,   284,   285,   286,   287,   919,   920,   288,   289,
     290,   291,   292,   404,   293,   294,   295,   296,   297,   886,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     430,   431,   308,   309,   310,   311,   312,   313,   654,   655,
     245,   147,   138,   129,   143,   477,   734,   700,   701,   508
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     360,    71,   126,   381,   730,   435,   429,   380,   892,   696,
     656,   750,   246,   242,    57,   403,    59,   426,   427,   913,
     399,   914,   698,   915,   366,   916,   110,   917,   918,   438,
     166,    56,   553,   185,   217,   753,   476,   433,   426,   427,
      17,   424,   425,   344,   134,   661,   489,   572,   675,   130,
     130,   131,   131,   156,   157,  1192,    71,   132,   132,   145,
     556,   439,   440,   732,  1208,    56,   796,   798,   155,   879,
     648,   649,    43,    44,    45,   662,   499,   789,    46,    47,
      49,   650,   651,   652,   402,   314,   754,   315,   316,   165,
     338,   168,   169,   319,  1220,   320,   321,   375,   247,   248,
     249,   250,   324,    58,   325,   326,   339,   340,   439,   440,
     101,   181,   182,   183,   184,   761,   359,   554,   102,   557,
     760,   115,   493,   494,   761,  1221,   116,   756,   117,   790,
     118,   439,   440,   108,    50,    51,    52,    17,  1006,   516,
      53,    54,   439,   440,   317,   439,   440,   439,   440,   224,
     225,   226,   322,   439,   440,   871,   439,   440,   234,   235,
     236,   327,   526,   527,   604,   522,   399,   103,   251,   252,
     379,   104,   532,   533,   534,   535,   536,   537,   253,   109,
     254,    21,   761,   402,   248,   249,   250,   437,   367,   653,
     111,   574,   153,   579,   114,   341,   158,   255,   256,   257,
     551,   552,   500,   258,   501,  1092,   476,   134,   439,   440,
     900,   127,   376,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   439,   440,   275,
     428,   167,   259,   260,   261,   262,    48,   263,   369,   264,
     913,   265,   914,   266,   915,   267,   916,   705,   917,   918,
     908,   428,   318,   128,   268,   215,   603,   432,   573,   274,
     323,   600,   601,   144,  1232,   439,   440,   761,   137,   328,
     596,   547,    65,   248,   249,   250,  1018,   269,    66,   270,
    1235,   271,   255,   256,   257,   119,   724,  -537,   258,   439,
     440,   665,   666,   758,   646,  1233,    67,   137,    55,    68,
     674,   657,   677,    69,   439,   440,   120,   272,   273,   274,
     121,  1236,   275,   122,   276,   439,   440,   259,   260,   261,
     277,   439,   440,   687,    70,   439,   440,    65,   667,   247,
     248,   249,   250,    66,   146,   443,   689,    72,    73,  1172,
    1173,    74,   725,    75,   726,   727,    76,   867,   439,   440,
     152,    67,   668,   480,    68,   153,  -538,  -538,    69,   498,
     493,   591,   481,   724,   370,   371,  1088,   669,   476,   459,
     592,   255,   256,   257,   460,    60,    61,   258,   670,    70,
      62,   525,   372,   154,   671,   461,  1008,   515,   672,   463,
     462,   520,    72,    73,   464,   159,    74,   275,    75,   251,
     252,    76,   160,   685,   434,   277,   259,   260,   261,   253,
     161,   254,  -538,  -538,   453,   454,   455,   456,   457,   725,
     164,   726,   727,   751,   130,   752,   131,   163,   255,   256,
     257,   172,   132,   418,   258,   419,   420,  1097,  1199,   421,
    1201,   173,   755,   385,   386,   387,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   174,  1217,   467,  1219,   458,
     769,   757,   468,   259,   260,   261,   262,   766,   263,   529,
     264,   530,   265,   531,   266,   330,   267,   469,   331,   332,
     123,   124,   470,   333,   334,   268,   275,   524,   884,   175,
     191,   192,   792,   436,   277,   193,   437,   170,   171,   642,
     247,   248,   249,   250,   659,   663,   186,   664,   269,   531,
     270,   906,   271,   907,   269,   880,   270,   194,   271,   883,
     876,   889,   893,   901,   902,   903,   904,   195,   187,   511,
     196,   197,   512,   198,   199,   200,   688,   188,   272,   273,
     274,   640,   189,   275,   641,   276,   521,   443,   190,   201,
     202,   277,   203,   204,  1175,   205,   179,  1176,  1177,   180,
    1202,   921,  1178,  1179,   465,   466,   444,   445,   446,   447,
     251,   252,   484,   485,   449,    17,  1218,   206,  1222,   207,
     253,   208,   254,   148,   149,   150,   151,   439,   440,   247,
     248,   249,   250,   658,  1234,   993,   437,   583,   584,   255,
     256,   257,   209,   681,   214,   258,   682,   781,  -249,   782,
     783,   784,   785,  1249,   786,   787,   738,  1252,   745,   437,
     524,   746,   210,   450,   451,   452,   453,   454,   455,   456,
     457,   105,   106,   107,   259,   260,   261,   262,   211,   263,
     212,   264,   218,   265,   222,   266,   220,   267,   139,   140,
     141,   142,   747,   223,   748,   746,   268,   437,   768,   251,
     252,   437,   772,   773,   227,   512,   774,   765,   228,   253,
     776,   254,   872,   777,   897,   512,   229,   437,   230,   269,
     443,   270,   232,   271,   247,   248,   249,   250,   255,   256,
     257,   233,   922,   925,   258,   923,   926,   237,   779,   444,
     445,   446,   447,   996,   238,  1000,   512,   449,   437,   272,
     273,   274,  1087,   239,   275,   777,   276,   242,  1094,   683,
     684,   746,   277,   259,   260,   261,   262,   244,   263,   329,
     264,   335,   265,   336,   266,  1181,   267,   337,   512,   176,
     177,   178,   455,   456,   457,   268,   426,  1001,  1206,  1207,
    1214,  1215,  1011,  1012,   397,   398,   450,   451,   452,   453,
     454,   455,   456,   457,   253,   342,   254,   881,   269,   345,
     270,   348,   271,   247,   248,   249,   250,   346,   347,   349,
     350,   351,   353,   255,   256,   257,   352,   354,   355,   258,
     356,   357,   361,   362,   364,   365,   373,   377,   272,   273,
     274,   378,   382,   275,   383,   276,   384,   400,   401,   405,
     406,   277,   407,   408,   411,   412,   409,   413,   259,   260,
     261,   262,   414,   263,   415,   264,   416,   265,   417,   266,
     422,   267,   423,   458,   799,   800,   801,   802,   803,   471,
     268,   804,   805,   397,   472,   473,   474,   476,   806,   807,
     808,   475,   479,   253,   483,   254,   482,  1180,   486,   487,
     488,  1183,   497,   269,   495,   270,   506,   271,   507,   509,
     809,   510,   255,   256,   257,   513,   518,   519,   258,    17,
    1182,   523,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   548,   550,   272,   273,   274,   555,   275,   275,   575,
     276,   577,   580,   581,   582,   585,   277,   259,   260,   261,
     262,   586,   263,   587,   264,   588,   265,   589,   266,   590,
     267,   593,   594,   595,   638,   597,   643,   599,   639,   268,
     645,   644,   673,   660,  -534,   647,   678,   439,    77,   554,
     679,     1,   692,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   269,   686,   270,   690,   271,    11,    12,    13,
     694,   695,   693,    14,    15,    16,   499,   699,    78,    79,
     437,    80,   702,   703,   704,   711,    81,    82,   714,   715,
     716,   717,   272,   273,   274,   718,   719,   275,   733,   276,
     720,   737,   740,    63,   721,   277,   743,     1,   741,     2,
       3,     4,     5,     6,     7,   722,     9,   742,   723,   731,
     744,   736,    17,    11,    12,    13,   739,   763,   764,    14,
      15,    16,     1,   749,     2,     3,     4,     5,     6,     7,
       8,     9,    10,   762,   684,   759,   767,   775,    11,    12,
      13,   683,   771,   778,    14,    15,    16,     1,   770,     2,
       3,     4,     5,     6,     7,   780,     9,   793,   865,   795,
     866,   873,   867,    11,    12,    13,   874,   875,    17,    14,
      15,    16,   878,   890,   810,   811,   812,   813,   814,   882,
     885,   815,   816,   441,   895,   442,   898,   891,   817,   818,
     819,   894,   896,    17,   899,   910,   911,   912,    83,    84,
      85,    86,   924,    87,    88,    18,   994,    89,    90,    91,
     820,   927,    92,    93,    94,   524,   936,   928,    17,    95,
      96,   998,    19,   821,   822,   823,   824,   825,   929,   930,
     826,   827,    97,    98,    20,   931,    99,   828,   829,   830,
     100,   932,   933,   443,   934,   935,   995,   937,   938,   947,
     958,   969,   939,    21,   980,   991,   940,   999,  1007,   831,
     941,    18,   444,   445,   446,   447,   448,   942,   943,   944,
     449,   945,   946,   948,   949,   443,   950,   761,  1009,  1015,
     951,   952,   953,   954,   955,   443,    18,  1017,   956,  1019,
      20,   957,  1013,  1020,   444,   445,   446,   447,   959,   691,
     960,   961,   449,    19,  -538,  -538,   446,   447,   962,    21,
    1021,    18,  -538,  1022,   963,    20,  1023,   964,  1024,   450,
     451,   452,   453,   454,   455,   456,   457,  1025,   965,   966,
     967,   968,   970,  1026,    21,   971,  1027,   972,  1085,   973,
      20,   974,   975,   976,   977,   978,  1029,   979,   981,   982,
     983,   450,   451,   452,   453,   454,   455,   456,   457,    21,
    1030,  -538,   451,   452,   453,   454,   455,   456,   457,   605,
     606,   607,   608,   609,   610,   611,   612,   613,   614,   615,
     616,   617,   618,   619,   620,   621,   984,   622,   623,   624,
     625,   626,   627,   985,   986,   628,   987,   988,   629,   630,
     631,   632,   633,   634,   635,   636,   832,   833,   834,   835,
     836,  1031,   989,   837,   838,   990,   992,  1032,  1002,  1003,
     839,   840,   841,   843,   844,   845,   846,   847,  1004,  1005,
     848,   849,  1014,  1028,  1033,  1034,  1035,   850,   851,   852,
    1036,  1037,   842,  1038,  1039,  1040,   854,   855,   856,   857,
     858,  1041,  1042,   859,   860,  1043,  1044,  1045,  1046,   853,
     861,   862,   863,  1047,  1048,  1049,  1050,  1051,  1052,  1053,
    1054,  1055,  1056,  1057,  1058,  1059,  1060,  1062,  1061,  1063,
    1064,  1065,   864,  1066,  1067,  1068,  1069,  1070,  1071,  1072,
    1073,  1074,  1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,
    1083,  1084,  1086,  1089,  1090,  1091,  1093,  1099,  1095,  1096,
    1100,  1102,  1103,  1104,  1105,  1106,  1107,  1108,  1109,  1110,
    1111,  1112,  1113,  1114,  1123,  1115,  1116,  1134,  1117,  1118,
    1145,  1119,  1156,  1120,  1121,  1167,  1122,  1124,  1125,  1126,
    1127,  1174,  1128,  1129,  1130,  1131,  1132,  1190,  1133,  1135,
    1191,  1136,  1195,  1137,  1196,  1138,  1139,  1140,  1141,  1142,
    1197,  1143,  1144,  1146,  1147,  1148,  1149,  1150,  1151,  1152,
    1153,  1154,  1155,  1157,  1203,  1205,  1158,  1159,  1160,  1161,
    1162,  1163,  1164,  1165,  1166,  1168,  1210,  1212,  1204,  1169,
    1170,  1171,  1184,  1185,  1198,  1186,  1187,  1188,  1189,  1193,
    1194,  1213,  1209,  1211,  1200,  1216,  1223,  1225,  1224,  1226,
    1227,  1228,  1229,  1230,  1231,  1237,  1239,  1238,  1240,  1241,
    1242,  1243,  1244,  1245,  1247,  1248,  1246,   162,  1250,  1251,
     868,   735,   213,   125,   870,   517,   368,   358,   713,   909,
    1010,   549,   343,   478,  1098,   528,   576,   578,   997,     0,
     887,     0,     0,   410,     0,     0,     0,   676,   888,     0,
       0,   877,   905
};

static const yytype_int16 yycheck[] =
{
     219,     8,    21,   242,   637,   277,   275,   240,   760,   585,
     519,   661,   157,    66,     4,   254,     6,     5,     6,   793,
     251,   793,   586,   793,    55,   793,    16,   793,   793,    55,
       6,     3,    75,    62,   133,    84,    78,   276,     5,     6,
      78,   272,   273,   188,     8,    55,    82,     3,     3,    20,
      20,    22,    22,    76,    77,     6,    63,    28,    28,    49,
      73,   157,   158,   639,     6,     3,   716,   717,    58,    84,
      68,    69,    33,    34,    35,    87,     3,     3,    39,    40,
      37,    79,    80,    81,    88,     3,    87,     5,     6,    79,
      10,   175,   176,     3,   136,     5,     6,    38,     3,     4,
       5,     6,     3,    76,     5,     6,    26,    27,   157,   158,
      34,   101,   102,   103,   104,    66,   215,   160,    34,   132,
       4,     7,   361,   362,    66,   167,    12,   223,    14,    55,
      16,   157,   158,     3,    33,    34,    35,    78,   890,   378,
      39,    40,   157,   158,    62,   157,   158,   157,   158,   139,
     140,   141,    62,   157,   158,   731,   157,   158,   148,   149,
     150,    62,   401,   402,   135,   384,   397,    34,    73,    74,
     223,    34,   411,   412,   413,   414,   415,   416,    83,     3,
      85,   219,    66,    88,     4,     5,     6,   223,   219,   187,
       3,   460,   221,   465,   174,   185,   219,   102,   103,   104,
     439,   440,   129,   108,   131,     4,    78,     8,   157,   158,
     774,     0,   231,   444,   445,   446,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   157,   158,   217,
     218,   207,   137,   138,   139,   140,   197,   142,   228,   144,
    1014,   146,  1014,   148,  1014,   150,  1014,   219,  1014,  1014,
     220,   218,   170,   222,   159,   219,   495,   276,   214,   214,
     170,    70,    71,     3,   136,   157,   158,    66,    72,   170,
     489,   220,     8,     4,     5,     6,   926,   182,     8,   184,
     136,   186,   102,   103,   104,   171,    73,    62,   108,   157,
     158,   530,   531,   223,   513,   167,     8,    72,   197,     8,
     539,   520,   541,     8,   157,   158,   192,   212,   213,   214,
     196,   167,   217,   199,   219,   157,   158,   137,   138,   139,
     225,   157,   158,   554,     8,   157,   158,    63,   220,     3,
       4,     5,     6,    63,    72,   133,   555,     8,     8,  1091,
    1092,     8,   129,     8,   131,   132,     8,   134,   157,   158,
       3,    63,   220,    40,    63,   221,   154,   155,    63,   366,
     599,    33,    49,    73,    33,    34,   999,   220,    78,   218,
      42,   102,   103,   104,   223,   168,   169,   108,   220,    63,
     173,   400,    51,    14,   220,   218,   895,   377,   220,   218,
     223,    65,    63,    63,   223,     3,    63,   217,    63,    73,
      74,    63,     3,   548,   224,   225,   137,   138,   139,    83,
       3,    85,   210,   211,   212,   213,   214,   215,   216,   129,
       3,   131,   132,   662,    20,   664,    22,   219,   102,   103,
     104,     3,    28,   122,   108,   124,   125,  1013,  1190,   128,
    1192,   178,   673,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,     6,  1208,   218,  1210,   221,
     691,   223,   223,   137,   138,   139,   140,   686,   142,    84,
     144,    86,   146,    88,   148,    40,   150,   218,    43,    44,
      33,    34,   223,    48,    49,   159,   217,    73,   757,     3,
     188,   189,   711,   220,   225,   193,   223,   175,   176,   506,
       3,     4,     5,     6,   523,    84,    55,    86,   182,    88,
     184,    34,   186,    36,   182,   754,   184,   162,   186,   756,
     739,   758,   761,     3,     4,     5,     6,   172,   223,   220,
     175,   176,   223,   178,   179,   180,   555,     6,   212,   213,
     214,   220,   197,   217,   223,   219,   220,   133,   197,   194,
     195,   225,   197,   198,    40,   197,   203,    43,    44,   206,
    1193,   794,    48,    49,   223,   224,   152,   153,   154,   155,
      73,    74,   190,   191,   160,    78,  1209,     4,  1211,   195,
      83,    76,    85,    51,    52,    53,    54,   157,   158,     3,
       4,     5,     6,   220,  1227,   867,   223,    45,    46,   102,
     103,   104,   197,   220,    21,   108,   223,    56,    57,    58,
      59,    60,    61,  1246,    63,    64,   220,  1250,   220,   223,
      73,   223,   197,   209,   210,   211,   212,   213,   214,   215,
     216,   161,   162,   163,   137,   138,   139,   140,   220,   142,
     220,   144,    65,   146,    73,   148,    67,   150,    44,    45,
      46,    47,   220,     3,   220,   223,   159,   223,   220,    73,
      74,   223,   220,   220,     3,   223,   223,   686,    62,    83,
     220,    85,   220,   223,   220,   223,    62,   223,   219,   182,
     133,   184,    74,   186,     3,     4,     5,     6,   102,   103,
     104,     3,   220,   220,   108,   223,   223,     3,   705,   152,
     153,   154,   155,   220,     3,   220,   223,   160,   223,   212,
     213,   214,   220,     3,   217,   223,   219,    66,   220,     5,
       6,   223,   225,   137,   138,   139,   140,     4,   142,   220,
     144,     3,   146,     3,   148,   220,   150,     4,   223,   202,
     203,   204,   214,   215,   216,   159,     5,     6,    45,    46,
      43,    44,   910,   911,    73,    74,   209,   210,   211,   212,
     213,   214,   215,   216,    83,   219,    85,   220,   182,     4,
     184,     3,   186,     3,     4,     5,     6,   167,     6,     6,
       4,     3,    55,   102,   103,   104,     4,     4,   197,   108,
       3,     3,    53,    68,    74,   135,     3,    62,   212,   213,
     214,   209,    78,   217,    78,   219,   219,   219,   219,     4,
       4,   225,     4,     4,   219,   219,     6,   219,   137,   138,
     139,   140,   219,   142,   219,   144,   219,   146,   219,   148,
     219,   150,   219,   221,    91,    92,    93,    94,    95,     3,
     159,    98,    99,    73,     6,    47,    47,    78,   105,   106,
     107,    77,     4,    83,    77,    85,     6,  1096,     4,   220,
     220,  1100,     4,   182,    69,   184,    55,   186,   219,     3,
     127,     3,   102,   103,   104,   219,   219,   219,   108,    78,
    1099,   219,     4,   219,   219,   219,   219,     4,     4,   226,
     220,    77,     3,   212,   213,   214,   219,   217,   217,     6,
     219,     6,     6,     5,    43,   219,   225,   137,   138,   139,
     140,   219,   142,     3,   144,   219,   146,   220,   148,     6,
     150,     4,   167,   167,   130,    76,     3,   223,   219,   159,
       3,   135,    76,   220,     0,   223,     4,   157,     3,   160,
     223,     7,   226,     9,    10,    11,    12,    13,    14,    15,
      16,    17,   182,   219,   184,   132,   186,    23,    24,    25,
       6,     6,   226,    29,    30,    31,     3,     3,    33,    34,
     223,    36,     6,     4,     4,    89,    41,    42,     4,   219,
     219,   219,   212,   213,   214,   219,   219,   217,    32,   219,
     219,     4,     6,     3,   219,   225,     3,     7,     6,     9,
      10,    11,    12,    13,    14,   219,    16,     4,   219,   219,
       6,   220,    78,    23,    24,    25,   219,     4,    78,    29,
      30,    31,     7,   220,     9,    10,    11,    12,    13,    14,
      15,    16,    17,   220,     6,   223,   220,   209,    23,    24,
      25,     5,    47,     6,    29,    30,    31,     7,    50,     9,
      10,    11,    12,    13,    14,   223,    16,   181,   132,     6,
     130,   219,   134,    23,    24,    25,   135,   167,    78,    29,
      30,    31,   220,     4,    91,    92,    93,    94,    95,   217,
     217,    98,    99,    73,   219,    75,     6,   223,   105,   106,
     107,   220,   220,    78,     6,    57,    57,     3,   163,   164,
     165,   166,   220,   168,   169,   171,   135,   172,   173,   174,
     127,   223,   177,   178,   179,    73,    93,   223,    78,   184,
     185,     4,   188,    91,    92,    93,    94,    95,   223,   223,
      98,    99,   197,   198,   200,   223,   201,   105,   106,   107,
     205,   223,   223,   133,   223,   223,   135,   223,   223,    93,
      93,    93,   223,   219,    93,    93,   223,     3,   220,   127,
     223,   171,   152,   153,   154,   155,   156,   223,   223,   223,
     160,   223,   223,   223,   223,   133,   223,    66,     6,    52,
     223,   223,   223,   223,   223,   133,   171,     6,   223,     6,
     200,   223,   219,     6,   152,   153,   154,   155,   223,   157,
     223,   223,   160,   188,   152,   153,   154,   155,   223,   219,
       6,   171,   160,     6,   223,   200,     6,   223,     6,   209,
     210,   211,   212,   213,   214,   215,   216,     6,   223,   223,
     223,   223,   223,     6,   219,   223,     6,   223,     4,   223,
     200,   223,   223,   223,   223,   223,     6,   223,   223,   223,
     223,   209,   210,   211,   212,   213,   214,   215,   216,   219,
       6,   209,   210,   211,   212,   213,   214,   215,   216,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   223,   108,   109,   110,
     111,   112,   113,   223,   223,   116,   223,   223,   119,   120,
     121,   122,   123,   124,   125,   126,    91,    92,    93,    94,
      95,     6,   223,    98,    99,   223,   223,     6,   223,   223,
     105,   106,   107,    91,    92,    93,    94,    95,   223,   223,
      98,    99,   223,   223,     6,     6,     6,   105,   106,   107,
       6,     6,   127,     6,   223,     6,    91,    92,    93,    94,
      95,     6,     6,    98,    99,     6,     6,     6,     6,   127,
     105,   106,   107,     6,     6,     6,   223,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   223,     6,
       6,     6,   127,     6,     6,     6,     6,     6,     6,   223,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     223,     6,     4,     4,     4,     4,   220,    53,     6,    62,
      54,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,     6,   220,   220,     6,   220,   220,     6,   220,   220,
       6,   220,     6,   220,   220,     6,   220,   220,   220,   220,
     220,   167,   220,   220,   220,   220,   220,     4,   220,   220,
       4,   220,    49,   220,     6,   220,   220,   220,   220,   220,
       6,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   167,     6,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,     4,    49,   219,   223,
     223,   223,   220,   220,   223,   220,   220,   220,   220,   220,
     220,     4,   220,   220,   223,     6,   219,     6,   220,    48,
     220,   220,    40,    40,     4,   219,    40,   220,    40,    40,
      40,     3,   219,   219,     3,     3,   220,    63,   220,   220,
     728,   641,   128,    21,   730,   379,   226,   215,   599,   780,
     908,   437,   187,   342,  1014,   403,   462,   464,   873,    -1,
     758,    -1,    -1,   261,    -1,    -1,    -1,   540,   758,    -1,
      -1,   746,   777
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    78,   171,   188,
     200,   219,   228,   229,   230,   232,   244,   245,   247,   248,
     251,   252,   253,   254,   255,   280,   285,   286,   287,   288,
     289,   290,   291,    33,    34,    35,    39,    40,   197,    37,
      33,    34,    35,    39,    40,   197,     3,   278,    76,   278,
     168,   169,   173,     3,   231,   232,   244,   245,   248,   251,
     252,   253,   285,   286,   287,   288,   289,     3,    33,    34,
      36,    41,    42,   163,   164,   165,   166,   168,   169,   172,
     173,   174,   177,   178,   179,   184,   185,   197,   198,   201,
     205,    34,    34,    34,    34,   161,   162,   163,     3,     3,
     278,     3,   281,   282,   174,     7,    12,    14,    16,   171,
     192,   196,   199,    33,    34,   254,   255,     0,   222,   340,
      20,    22,    28,   274,     8,   256,   258,    72,   339,   339,
     339,   339,   339,   341,     3,   278,    72,   338,   338,   338,
     338,   338,     3,   221,    14,   278,    76,    77,   219,     3,
       3,     3,   231,   219,     3,   278,     6,   207,   175,   176,
     175,   176,     3,   178,     6,     3,   202,   203,   204,   203,
     206,   278,   278,   278,   278,    62,    55,   223,     6,   197,
     197,   188,   189,   193,   162,   172,   175,   176,   178,   179,
     180,   194,   195,   197,   198,   197,     4,   195,    76,   197,
     197,   220,   220,   230,    21,   219,   257,   258,    65,   265,
      67,   259,    73,     3,   278,   278,   278,     3,    62,    62,
     219,   246,    74,     3,   278,   278,   278,     3,     3,     3,
     249,   250,    66,   271,     4,   337,   337,     3,     4,     5,
       6,    73,    74,    83,    85,   102,   103,   104,   108,   137,
     138,   139,   140,   142,   144,   146,   148,   150,   159,   182,
     184,   186,   212,   213,   214,   217,   219,   225,   292,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   305,   306,
     307,   308,   309,   311,   312,   313,   314,   315,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   329,   330,
     331,   332,   333,   334,     3,     5,     6,    62,   170,     3,
       5,     6,    62,   170,     3,     5,     6,    62,   170,   220,
      40,    43,    44,    48,    49,     3,     3,     4,    10,    26,
      27,   278,   219,   282,   337,     4,   167,     6,     3,     6,
       4,     3,     4,    55,     4,   197,     3,     3,   257,   258,
     292,    53,    68,   263,    74,   135,    55,   219,   246,   278,
      33,    34,    51,     3,   243,    38,   255,    62,   209,   223,
     271,   295,    78,    78,   219,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,    73,    74,   296,
     219,   219,    88,   295,   310,     4,     4,     4,     4,     6,
     334,   219,   219,   219,   219,   219,   219,   219,   122,   124,
     125,   128,   219,   219,   296,   296,     5,     6,   218,   315,
     327,   328,   255,   295,   224,   314,   220,   223,    55,   157,
     158,    73,    75,   133,   152,   153,   154,   155,   156,   160,
     209,   210,   211,   212,   213,   214,   215,   216,   221,   218,
     223,   218,   223,   218,   223,   223,   224,   218,   223,   218,
     223,     3,     6,    47,    47,    77,    78,   342,   256,     4,
      40,    49,     6,    77,   190,   191,     4,   220,   220,    82,
     267,   260,   261,   295,   295,    69,   264,     4,   253,     3,
     129,   131,   233,   235,   236,   242,    55,   219,   346,     3,
       3,   220,   223,   219,   293,   278,   295,   250,   219,   219,
      65,   220,   292,   219,    73,   255,   295,   295,   310,    84,
      86,    88,   295,   295,   295,   295,   295,   295,     4,   219,
     219,   219,   219,     4,     4,   226,   220,   220,    77,   294,
       3,   295,   295,    75,   160,   219,    73,   132,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,     3,   214,   315,     6,   327,     6,   328,   314,
       6,     5,    43,    45,    46,   219,   219,     3,   219,   220,
       6,    33,    42,     4,   167,   167,   292,    76,   268,   223,
      70,    71,   262,   295,   135,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   108,   109,   110,   111,   112,   113,   116,   119,
     120,   121,   122,   123,   124,   125,   126,   238,   130,   219,
     220,   223,   253,     3,   135,     3,   292,   223,    68,    69,
      79,    80,    81,   187,   335,   336,   335,   292,   220,   255,
     220,    55,    87,    84,    86,   295,   295,   220,   220,   220,
     220,   220,   220,    76,   295,     3,   313,   295,     4,   223,
     270,   220,   223,     5,     6,   337,   219,   296,   255,   292,
     132,   157,   226,   226,     6,     6,   243,   234,   236,     3,
     344,   345,     6,     4,     4,   219,   275,   276,   277,   278,
     283,    89,   266,   261,     4,   219,   219,   219,   219,   219,
     219,   219,   219,   219,    73,   129,   131,   132,   239,   240,
     342,   219,   243,    32,   343,   235,   220,     4,   220,   219,
       6,     6,     4,     3,     6,   220,   223,   220,   220,   220,
     238,   295,   295,    84,    87,   296,   223,   223,   223,   223,
       4,    66,   220,     4,    78,   255,   292,   220,   220,   296,
      50,    47,   220,   220,   223,   209,   220,   223,     6,   253,
     223,    56,    58,    59,    60,    61,    63,    64,   284,     3,
      55,   279,   292,   181,   269,     6,   238,   237,   238,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   127,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     127,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   127,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   127,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   127,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   127,   132,   130,   134,   240,   241,
     241,   243,   220,   219,   135,   167,   292,   336,   220,    84,
     295,   220,   217,   329,   315,   217,   316,   319,   324,   329,
       4,   223,   270,   295,   220,   219,   220,   220,     6,     6,
     236,     3,     4,     5,     6,   345,    34,    36,   220,   276,
      57,    57,     3,   297,   298,   299,   300,   301,   302,   303,
     304,   271,   220,   223,   220,   220,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,    93,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,    93,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,    93,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,    93,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
      93,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,    93,   223,   314,   135,   135,   220,   344,     4,     3,
     220,     6,   223,   223,   223,   223,   270,   220,   335,     6,
     279,   277,   277,   219,   223,    52,   273,     6,   238,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   223,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   223,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     223,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   223,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   223,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   223,     6,     4,     4,   220,   342,     4,
       4,     4,     4,   220,   220,     6,    62,   243,   303,    53,
      54,   272,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,     6,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,     6,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,     6,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,     6,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,     6,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,     6,   220,   223,
     223,   223,   270,   270,   167,    40,    43,    44,    48,    49,
     295,   220,   292,   295,   220,   220,   220,   220,   220,   220,
       4,     4,     6,   220,   220,    49,     6,     6,   223,   270,
     223,   270,   342,   167,   219,     6,    45,    46,     6,   220,
       4,   220,    49,     4,    43,    44,     6,   270,   342,   270,
     136,   167,   342,   219,   220,     6,    48,   220,   220,    40,
      40,     4,   136,   167,   342,   136,   167,   219,   220,    40,
      40,    40,    40,     3,   219,   219,   220,     3,     3,   342,
     220,   220,   342
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   227,   228,   229,   229,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   236,
     236,   236,   237,   237,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   239,   239,
     240,   240,   240,   240,   241,   241,   242,   242,   243,   243,
     244,   245,   245,   246,   246,   247,   247,   248,   249,   249,
     250,   251,   251,   251,   251,   251,   252,   252,   252,   253,
     253,   253,   253,   254,   254,   255,   256,   257,   257,   258,
     259,   259,   260,   260,   261,   262,   262,   262,   263,   263,
     264,   264,   265,   265,   266,   266,   267,   267,   268,   268,
     269,   269,   270,   270,   271,   271,   272,   272,   273,   273,
     274,   274,   274,   274,   275,   275,   276,   276,   277,   277,
     278,   278,   279,   279,   279,   279,   280,   280,   281,   281,
     282,   283,   283,   284,   284,   284,   284,   284,   284,   284,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   286,   286,   286,   287,   287,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   289,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     291,   291,   291,   292,   292,   293,   293,   294,   294,   295,
     295,   295,   295,   295,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   297,   297,   297,
     298,   298,   298,   298,   299,   299,   299,   299,   300,   300,
     300,   300,   301,   301,   302,   302,   303,   303,   303,   303,
     303,   303,   304,   304,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   306,   306,   307,   308,   308,   309,   309,
     309,   309,   310,   310,   311,   312,   312,   312,   312,   313,
     313,   313,   313,   314,   314,   314,   314,   314,   314,   314,
     314,   314,   314,   314,   314,   314,   315,   315,   315,   315,
     316,   316,   316,   317,   318,   318,   319,   319,   320,   321,
     321,   322,   323,   323,   324,   325,   325,   326,   326,   327,
     328,   329,   329,   330,   331,   331,   332,   333,   333,   334,
     334,   334,   334,   334,   334,   334,   334,   334,   334,   334,
     334,   335,   335,   336,   336,   336,   336,   336,   336,   337,
     338,   338,   339,   339,   340,   340,   341,   341,   342,   342,
     343,   343,   344,   344,   345,   345,   345,   345,   345,   346,
     346
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
       1,     1,     1,     3,     3,     4,     5,     4,     4,     4,
       4,     4,     4,     4,     3,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       6,     3,     4,     3,     3,     5,     5,     6,     4,     6,
       3,     5,     4,     5,     6,     4,     5,     5,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     1,     1,     2,     2,
       3,     2,     2,     3,     2,     2,     2,     2,     3,     3,
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
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2491 "parser.cpp"
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
#line 2505 "parser.cpp"
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
#line 2519 "parser.cpp"
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
#line 2533 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2542 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 184 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2551 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2562 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2570 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2579 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2588 "parser.cpp"
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
#line 2602 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 285 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2613 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2623 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2643 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2653 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2663 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2673 "parser.cpp"
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
#line 2687 "parser.cpp"
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
#line 2701 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 355 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2711 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2719 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2727 "parser.cpp"
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
#line 2741 "parser.cpp"
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
#line 2755 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2764 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2772 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2780 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2788 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2796 "parser.cpp"
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
#line 2810 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2819 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2837 "parser.cpp"
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
#line 2850 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2859 "parser.cpp"
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
#line 2873 "parser.cpp"
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
#line 2887 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 361 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2897 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2906 "parser.cpp"
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
#line 2920 "parser.cpp"
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
#line 2934 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2942 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2950 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2958 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2966 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2974 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2982 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2990 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2998 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3006 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3014 "parser.cpp"
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
#line 3028 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3036 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3052 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3060 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3068 "parser.cpp"
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
#line 3081 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3089 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3097 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3105 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3113 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3121 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3129 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3137 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3145 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3153 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3161 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3169 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3177 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3185 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3193 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3201 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3209 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 382 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3217 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 386 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3225 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3233 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3241 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3249 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3257 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3265 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3273 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3281 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 218 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3292 "parser.cpp"
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
#line 3306 "parser.cpp"
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
#line 3320 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 194 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3331 "parser.cpp"
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

#line 3439 "parser.cpp"

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
#line 3654 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 521 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3665 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 527 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3676 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 534 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3682 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 535 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3688 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3694 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3700 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 538 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3706 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3712 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3718 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3724 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 542 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3730 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 543 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3736 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 544 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3742 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 545 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3748 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3754 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3760 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3766 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 550 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3772 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 551 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3778 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 552 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3784 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3790 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 554 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3796 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 555 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3802 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3808 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3814 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 558 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3820 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 559 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3826 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 560 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3832 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 561 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3838 "parser.cpp"
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
#line 3860 "parser.cpp"
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
#line 3880 "parser.cpp"
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
#line 3898 "parser.cpp"
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
#line 3931 "parser.cpp"
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
#line 3951 "parser.cpp"
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
#line 3987 "parser.cpp"
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
#line 4009 "parser.cpp"
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
#line 4030 "parser.cpp"
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
#line 4063 "parser.cpp"
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
#line 4098 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 789 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4107 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 793 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4116 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 798 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4125 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 802 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4134 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 808 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4142 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 811 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4150 "parser.cpp"
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
#line 4175 "parser.cpp"
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
#line 4202 "parser.cpp"
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
#line 4231 "parser.cpp"
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
#line 4261 "parser.cpp"
    break;

  case 52: /* column_type_array: column_type  */
#line 910 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4270 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type_array ',' column_type  */
#line 914 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4279 "parser.cpp"
    break;

  case 54: /* column_type: BOOLEAN  */
#line 920 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4285 "parser.cpp"
    break;

  case 55: /* column_type: TINYINT  */
#line 921 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4291 "parser.cpp"
    break;

  case 56: /* column_type: SMALLINT  */
#line 922 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4297 "parser.cpp"
    break;

  case 57: /* column_type: INTEGER  */
#line 923 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4303 "parser.cpp"
    break;

  case 58: /* column_type: INT  */
#line 924 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4309 "parser.cpp"
    break;

  case 59: /* column_type: BIGINT  */
#line 925 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4315 "parser.cpp"
    break;

  case 60: /* column_type: HUGEINT  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4321 "parser.cpp"
    break;

  case 61: /* column_type: FLOAT  */
#line 927 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4327 "parser.cpp"
    break;

  case 62: /* column_type: REAL  */
#line 928 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4333 "parser.cpp"
    break;

  case 63: /* column_type: DOUBLE  */
#line 929 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4339 "parser.cpp"
    break;

  case 64: /* column_type: FLOAT16  */
#line 930 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4345 "parser.cpp"
    break;

  case 65: /* column_type: BFLOAT16  */
#line 931 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4351 "parser.cpp"
    break;

  case 66: /* column_type: DATE  */
#line 932 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4357 "parser.cpp"
    break;

  case 67: /* column_type: TIME  */
#line 933 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4363 "parser.cpp"
    break;

  case 68: /* column_type: DATETIME  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4369 "parser.cpp"
    break;

  case 69: /* column_type: TIMESTAMP  */
#line 935 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4375 "parser.cpp"
    break;

  case 70: /* column_type: UUID  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4381 "parser.cpp"
    break;

  case 71: /* column_type: POINT  */
#line 937 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4387 "parser.cpp"
    break;

  case 72: /* column_type: LINE  */
#line 938 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4393 "parser.cpp"
    break;

  case 73: /* column_type: LSEG  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4399 "parser.cpp"
    break;

  case 74: /* column_type: BOX  */
#line 940 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4405 "parser.cpp"
    break;

  case 75: /* column_type: CIRCLE  */
#line 943 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4411 "parser.cpp"
    break;

  case 76: /* column_type: VARCHAR  */
#line 945 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4417 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 946 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4423 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 947 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4429 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4435 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4441 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4447 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4453 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4459 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4465 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4471 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4477 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4483 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4489 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4495 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4501 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4507 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4513 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4519 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4525 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4531 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4537 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4543 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4549 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4555 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4561 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4567 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4573 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4579 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4585 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4591 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4597 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4603 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4609 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4615 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4621 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4627 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4633 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4639 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4645 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4651 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4657 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4663 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4669 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4675 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4681 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4687 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4693 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4699 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4705 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4711 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4717 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4723 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4729 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4735 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4741 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4747 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4753 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4759 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4765 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4771 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4777 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4783 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4789 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4795 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4801 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4807 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4813 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4819 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4825 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4831 "parser.cpp"
    break;

  case 146: /* column_type: ARRAY '(' column_type ')'  */
#line 1017 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4840 "parser.cpp"
    break;

  case 147: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1021 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4850 "parser.cpp"
    break;

  case 148: /* column_constraints: column_constraint  */
#line 1044 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4859 "parser.cpp"
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
#line 4873 "parser.cpp"
    break;

  case 150: /* column_constraint: PRIMARY KEY  */
#line 1058 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4881 "parser.cpp"
    break;

  case 151: /* column_constraint: UNIQUE  */
#line 1061 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4889 "parser.cpp"
    break;

  case 152: /* column_constraint: NULLABLE  */
#line 1064 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4897 "parser.cpp"
    break;

  case 153: /* column_constraint: NOT NULLABLE  */
#line 1067 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4905 "parser.cpp"
    break;

  case 154: /* default_expr: DEFAULT constant_expr  */
#line 1071 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4913 "parser.cpp"
    break;

  case 155: /* default_expr: %empty  */
#line 1074 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4921 "parser.cpp"
    break;

  case 156: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1079 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4931 "parser.cpp"
    break;

  case 157: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1084 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4941 "parser.cpp"
    break;

  case 158: /* identifier_array: IDENTIFIER  */
#line 1091 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4952 "parser.cpp"
    break;

  case 159: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1097 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4963 "parser.cpp"
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
#line 4980 "parser.cpp"
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
#line 5023 "parser.cpp"
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
#line 5043 "parser.cpp"
    break;

  case 163: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1177 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5051 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: %empty  */
#line 1180 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5059 "parser.cpp"
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
#line 5077 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN explainable_statement  */
#line 1199 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5087 "parser.cpp"
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
#line 5104 "parser.cpp"
    break;

  case 168: /* update_expr_array: update_expr  */
#line 1221 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5113 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1225 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5122 "parser.cpp"
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
#line 5134 "parser.cpp"
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
#line 5150 "parser.cpp"
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
#line 5168 "parser.cpp"
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
#line 5186 "parser.cpp"
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
#line 5204 "parser.cpp"
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
#line 5227 "parser.cpp"
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
#line 5285 "parser.cpp"
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
#line 5345 "parser.cpp"
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
#line 5397 "parser.cpp"
    break;

  case 179: /* select_statement: select_without_paren  */
#line 1483 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5405 "parser.cpp"
    break;

  case 180: /* select_statement: select_with_paren  */
#line 1486 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5413 "parser.cpp"
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
#line 5427 "parser.cpp"
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
#line 5441 "parser.cpp"
    break;

  case 183: /* select_with_paren: '(' select_without_paren ')'  */
#line 1508 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5449 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_with_paren ')'  */
#line 1511 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5457 "parser.cpp"
    break;

  case 185: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1515 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5466 "parser.cpp"
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
#line 5497 "parser.cpp"
    break;

  case 187: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1547 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5505 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1550 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5513 "parser.cpp"
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
#line 5535 "parser.cpp"
    break;

  case 190: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1573 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5543 "parser.cpp"
    break;

  case 191: /* order_by_clause: %empty  */
#line 1576 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5551 "parser.cpp"
    break;

  case 192: /* order_by_expr_list: order_by_expr  */
#line 1580 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5560 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1584 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5569 "parser.cpp"
    break;

  case 194: /* order_by_expr: expr order_by_type  */
#line 1589 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5579 "parser.cpp"
    break;

  case 195: /* order_by_type: ASC  */
#line 1595 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5587 "parser.cpp"
    break;

  case 196: /* order_by_type: DESC  */
#line 1598 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5595 "parser.cpp"
    break;

  case 197: /* order_by_type: %empty  */
#line 1601 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5603 "parser.cpp"
    break;

  case 198: /* limit_expr: LIMIT expr  */
#line 1605 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5611 "parser.cpp"
    break;

  case 199: /* limit_expr: %empty  */
#line 1609 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5617 "parser.cpp"
    break;

  case 200: /* offset_expr: OFFSET expr  */
#line 1611 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5625 "parser.cpp"
    break;

  case 201: /* offset_expr: %empty  */
#line 1615 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5631 "parser.cpp"
    break;

  case 202: /* distinct: DISTINCT  */
#line 1617 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5639 "parser.cpp"
    break;

  case 203: /* distinct: %empty  */
#line 1620 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5647 "parser.cpp"
    break;

  case 204: /* unnest_clause: UNNEST expr_array  */
#line 1624 "parser.y"
                                 {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5655 "parser.cpp"
    break;

  case 205: /* unnest_clause: %empty  */
#line 1627 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5663 "parser.cpp"
    break;

  case 206: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5671 "parser.cpp"
    break;

  case 207: /* highlight_clause: %empty  */
#line 1634 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5679 "parser.cpp"
    break;

  case 208: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5687 "parser.cpp"
    break;

  case 209: /* from_clause: %empty  */
#line 1641 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5695 "parser.cpp"
    break;

  case 210: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5705 "parser.cpp"
    break;

  case 211: /* search_clause: %empty  */
#line 1650 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5713 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5721 "parser.cpp"
    break;

  case 213: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5729 "parser.cpp"
    break;

  case 214: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5737 "parser.cpp"
    break;

  case 215: /* where_clause: %empty  */
#line 1664 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5745 "parser.cpp"
    break;

  case 216: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5753 "parser.cpp"
    break;

  case 217: /* having_clause: %empty  */
#line 1671 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5761 "parser.cpp"
    break;

  case 218: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5769 "parser.cpp"
    break;

  case 219: /* group_by_clause: %empty  */
#line 1678 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5777 "parser.cpp"
    break;

  case 220: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5785 "parser.cpp"
    break;

  case 221: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5793 "parser.cpp"
    break;

  case 222: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5801 "parser.cpp"
    break;

  case 223: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5809 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5817 "parser.cpp"
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
#line 5835 "parser.cpp"
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
#line 5853 "parser.cpp"
    break;

  case 229: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5864 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5874 "parser.cpp"
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
#line 5886 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5896 "parser.cpp"
    break;

  case 233: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5906 "parser.cpp"
    break;

  case 234: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5917 "parser.cpp"
    break;

  case 235: /* table_alias: %empty  */
#line 1772 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5925 "parser.cpp"
    break;

  case 236: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5933 "parser.cpp"
    break;

  case 237: /* with_clause: %empty  */
#line 1782 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5941 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5950 "parser.cpp"
    break;

  case 239: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5959 "parser.cpp"
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
#line 5971 "parser.cpp"
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
#line 5983 "parser.cpp"
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
#line 5996 "parser.cpp"
    break;

  case 243: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 6004 "parser.cpp"
    break;

  case 244: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 6012 "parser.cpp"
    break;

  case 245: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6020 "parser.cpp"
    break;

  case 246: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6028 "parser.cpp"
    break;

  case 247: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6036 "parser.cpp"
    break;

  case 248: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6044 "parser.cpp"
    break;

  case 249: /* join_type: %empty  */
#line 1845 "parser.y"
                {
}
#line 6051 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DATABASES  */
#line 1851 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6060 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TABLES  */
#line 1855 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6069 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW VIEWS  */
#line 1859 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6078 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIGS  */
#line 1863 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6087 "parser.cpp"
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
#line 6099 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW PROFILES  */
#line 1874 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6108 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW BUFFER  */
#line 1878 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6117 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW MEMINDEX  */
#line 1882 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6126 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERIES  */
#line 1886 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6135 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1890 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6145 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTIONS  */
#line 1895 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6154 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1899 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6164 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1904 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6173 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLES  */
#line 1908 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6182 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1912 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6191 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1916 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6202 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1922 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6213 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1928 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6224 "parser.cpp"
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
#line 6240 "parser.cpp"
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
#line 6256 "parser.cpp"
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
#line 6272 "parser.cpp"
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
#line 6289 "parser.cpp"
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
#line 6306 "parser.cpp"
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
#line 6324 "parser.cpp"
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
#line 6343 "parser.cpp"
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
#line 6359 "parser.cpp"
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
#line 6378 "parser.cpp"
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
#line 6399 "parser.cpp"
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
#line 6421 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW LOGS  */
#line 2076 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6430 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW DELTA LOGS  */
#line 2080 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6439 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOGS  */
#line 2084 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6448 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2088 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6457 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2092 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6466 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2096 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6477 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY  */
#line 2102 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6486 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2106 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6495 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2110 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6504 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2114 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6515 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOTS  */
#line 2120 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6524 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2124 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6535 "parser.cpp"
    break;

  case 291: /* flush_statement: FLUSH DATA  */
#line 2134 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6544 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH LOG  */
#line 2138 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6553 "parser.cpp"
    break;

  case 293: /* flush_statement: FLUSH BUFFER  */
#line 2142 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6562 "parser.cpp"
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
#line 6577 "parser.cpp"
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
#line 6601 "parser.cpp"
    break;

  case 296: /* command_statement: USE IDENTIFIER  */
#line 2184 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6612 "parser.cpp"
    break;

  case 297: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2190 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6622 "parser.cpp"
    break;

  case 298: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2195 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6633 "parser.cpp"
    break;

  case 299: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2201 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6644 "parser.cpp"
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
#line 6657 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2215 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6668 "parser.cpp"
    break;

  case 302: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2221 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6679 "parser.cpp"
    break;

  case 303: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2227 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6690 "parser.cpp"
    break;

  case 304: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2233 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6701 "parser.cpp"
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
#line 6714 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2247 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6725 "parser.cpp"
    break;

  case 307: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2253 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6736 "parser.cpp"
    break;

  case 308: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2259 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6747 "parser.cpp"
    break;

  case 309: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2265 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6758 "parser.cpp"
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
#line 6771 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2279 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6782 "parser.cpp"
    break;

  case 312: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2285 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6793 "parser.cpp"
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
#line 6807 "parser.cpp"
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
#line 6821 "parser.cpp"
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
#line 6834 "parser.cpp"
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
#line 6847 "parser.cpp"
    break;

  case 317: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2325 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6858 "parser.cpp"
    break;

  case 318: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2331 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6869 "parser.cpp"
    break;

  case 319: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2337 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6880 "parser.cpp"
    break;

  case 320: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2343 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6891 "parser.cpp"
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
#line 6908 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2363 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6917 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2367 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6927 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2372 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6938 "parser.cpp"
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
#line 6950 "parser.cpp"
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
#line 6963 "parser.cpp"
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
#line 6977 "parser.cpp"
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
#line 6992 "parser.cpp"
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
#line 7007 "parser.cpp"
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
#line 7023 "parser.cpp"
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
#line 7039 "parser.cpp"
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
#line 7056 "parser.cpp"
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
#line 7073 "parser.cpp"
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
#line 7088 "parser.cpp"
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
#line 7104 "parser.cpp"
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
#line 7121 "parser.cpp"
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
#line 7139 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOGS  */
#line 2514 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7148 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2518 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7158 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2523 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7168 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2528 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7179 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2534 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7188 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2538 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7197 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2542 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7208 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2548 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7217 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2552 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7226 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2556 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7237 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2562 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7248 "parser.cpp"
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
#line 7261 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2576 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7272 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODES  */
#line 2582 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7281 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2586 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7292 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW NODE  */
#line 2592 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7301 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2596 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7312 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET ADMIN  */
#line 2602 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7322 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET STANDALONE  */
#line 2607 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7332 "parser.cpp"
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
#line 7344 "parser.cpp"
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
#line 7358 "parser.cpp"
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
#line 7372 "parser.cpp"
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
#line 7386 "parser.cpp"
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
#line 7403 "parser.cpp"
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
#line 7419 "parser.cpp"
    break;

  case 363: /* expr_array: expr_alias  */
#line 2675 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7428 "parser.cpp"
    break;

  case 364: /* expr_array: expr_array ',' expr_alias  */
#line 2679 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7437 "parser.cpp"
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
#line 7452 "parser.cpp"
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
#line 7467 "parser.cpp"
    break;

  case 367: /* expr_alias: expr AS IDENTIFIER  */
#line 2716 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7478 "parser.cpp"
    break;

  case 368: /* expr_alias: expr  */
#line 2722 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7486 "parser.cpp"
    break;

  case 374: /* operand: '(' expr ')'  */
#line 2732 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7494 "parser.cpp"
    break;

  case 375: /* operand: '(' select_without_paren ')'  */
#line 2735 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7505 "parser.cpp"
    break;

  case 376: /* operand: constant_expr  */
#line 2741 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7513 "parser.cpp"
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
#line 7533 "parser.cpp"
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
#line 7554 "parser.cpp"
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
#line 7575 "parser.cpp"
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
#line 7625 "parser.cpp"
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
#line 7668 "parser.cpp"
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
#line 7715 "parser.cpp"
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
#line 7763 "parser.cpp"
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
#line 7791 "parser.cpp"
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
#line 7818 "parser.cpp"
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
#line 7843 "parser.cpp"
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
#line 7868 "parser.cpp"
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
#line 7882 "parser.cpp"
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
#line 7898 "parser.cpp"
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
#line 7914 "parser.cpp"
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
#line 7932 "parser.cpp"
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
#line 7944 "parser.cpp"
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
#line 7958 "parser.cpp"
    break;

  case 404: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3134 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7969 "parser.cpp"
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
#line 7985 "parser.cpp"
    break;

  case 406: /* sub_search: match_vector_expr  */
#line 3152 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7993 "parser.cpp"
    break;

  case 407: /* sub_search: match_text_expr  */
#line 3155 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8001 "parser.cpp"
    break;

  case 408: /* sub_search: match_tensor_expr  */
#line 3158 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8009 "parser.cpp"
    break;

  case 409: /* sub_search: match_sparse_expr  */
#line 3161 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8017 "parser.cpp"
    break;

  case 410: /* sub_search: query_expr  */
#line 3164 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8025 "parser.cpp"
    break;

  case 411: /* sub_search: fusion_expr  */
#line 3167 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8033 "parser.cpp"
    break;

  case 412: /* sub_search_array: sub_search  */
#line 3171 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8042 "parser.cpp"
    break;

  case 413: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3175 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8051 "parser.cpp"
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
#line 8064 "parser.cpp"
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
#line 8077 "parser.cpp"
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
#line 8091 "parser.cpp"
    break;

  case 417: /* function_expr: YEAR '(' expr ')'  */
#line 3205 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8103 "parser.cpp"
    break;

  case 418: /* function_expr: MONTH '(' expr ')'  */
#line 3212 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8115 "parser.cpp"
    break;

  case 419: /* function_expr: DAY '(' expr ')'  */
#line 3219 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8127 "parser.cpp"
    break;

  case 420: /* function_expr: HOUR '(' expr ')'  */
#line 3226 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8139 "parser.cpp"
    break;

  case 421: /* function_expr: MINUTE '(' expr ')'  */
#line 3233 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8151 "parser.cpp"
    break;

  case 422: /* function_expr: SECOND '(' expr ')'  */
#line 3240 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8163 "parser.cpp"
    break;

  case 423: /* function_expr: operand IS NOT NULLABLE  */
#line 3247 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8175 "parser.cpp"
    break;

  case 424: /* function_expr: operand IS NULLABLE  */
#line 3254 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8187 "parser.cpp"
    break;

  case 425: /* function_expr: NOT operand  */
#line 3261 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8199 "parser.cpp"
    break;

  case 426: /* function_expr: '-' operand  */
#line 3268 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8211 "parser.cpp"
    break;

  case 427: /* function_expr: '+' operand  */
#line 3275 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8223 "parser.cpp"
    break;

  case 428: /* function_expr: operand '-' operand  */
#line 3282 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8236 "parser.cpp"
    break;

  case 429: /* function_expr: operand '+' operand  */
#line 3290 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8249 "parser.cpp"
    break;

  case 430: /* function_expr: operand '*' operand  */
#line 3298 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8262 "parser.cpp"
    break;

  case 431: /* function_expr: operand '/' operand  */
#line 3306 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8275 "parser.cpp"
    break;

  case 432: /* function_expr: operand '%' operand  */
#line 3314 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8288 "parser.cpp"
    break;

  case 433: /* function_expr: operand '=' operand  */
#line 3322 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8301 "parser.cpp"
    break;

  case 434: /* function_expr: operand EQUAL operand  */
#line 3330 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8314 "parser.cpp"
    break;

  case 435: /* function_expr: operand NOT_EQ operand  */
#line 3338 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8327 "parser.cpp"
    break;

  case 436: /* function_expr: operand '<' operand  */
#line 3346 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8340 "parser.cpp"
    break;

  case 437: /* function_expr: operand '>' operand  */
#line 3354 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8353 "parser.cpp"
    break;

  case 438: /* function_expr: operand LESS_EQ operand  */
#line 3362 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8366 "parser.cpp"
    break;

  case 439: /* function_expr: operand GREATER_EQ operand  */
#line 3370 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8379 "parser.cpp"
    break;

  case 440: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3378 "parser.y"
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
#line 8414 "parser.cpp"
    break;

  case 441: /* function_expr: operand LIKE operand  */
#line 3408 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8427 "parser.cpp"
    break;

  case 442: /* function_expr: operand NOT LIKE operand  */
#line 3416 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8440 "parser.cpp"
    break;

  case 443: /* conjunction_expr: expr AND expr  */
#line 3425 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8453 "parser.cpp"
    break;

  case 444: /* conjunction_expr: expr OR expr  */
#line 3433 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8466 "parser.cpp"
    break;

  case 445: /* between_expr: operand BETWEEN operand AND operand  */
#line 3442 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8478 "parser.cpp"
    break;

  case 446: /* in_expr: operand IN '(' expr_array ')'  */
#line 3450 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8489 "parser.cpp"
    break;

  case 447: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3456 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8500 "parser.cpp"
    break;

  case 448: /* case_expr: CASE expr case_check_array END  */
#line 3463 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8511 "parser.cpp"
    break;

  case 449: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3469 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8523 "parser.cpp"
    break;

  case 450: /* case_expr: CASE case_check_array END  */
#line 3476 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8533 "parser.cpp"
    break;

  case 451: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3481 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8544 "parser.cpp"
    break;

  case 452: /* case_check_array: WHEN expr THEN expr  */
#line 3488 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8556 "parser.cpp"
    break;

  case 453: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3495 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8568 "parser.cpp"
    break;

  case 454: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3503 "parser.y"
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
#line 8585 "parser.cpp"
    break;

  case 455: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3516 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8596 "parser.cpp"
    break;

  case 456: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3522 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8607 "parser.cpp"
    break;

  case 457: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3528 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8619 "parser.cpp"
    break;

  case 458: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3535 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8631 "parser.cpp"
    break;

  case 459: /* column_expr: IDENTIFIER  */
#line 3543 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8643 "parser.cpp"
    break;

  case 460: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3550 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8655 "parser.cpp"
    break;

  case 461: /* column_expr: '*'  */
#line 3557 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8665 "parser.cpp"
    break;

  case 462: /* column_expr: column_expr '.' '*'  */
#line 3562 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8679 "parser.cpp"
    break;

  case 463: /* constant_expr: STRING  */
#line 3572 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8689 "parser.cpp"
    break;

  case 464: /* constant_expr: TRUE  */
#line 3577 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8699 "parser.cpp"
    break;

  case 465: /* constant_expr: FALSE  */
#line 3582 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8709 "parser.cpp"
    break;

  case 466: /* constant_expr: DOUBLE_VALUE  */
#line 3587 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8719 "parser.cpp"
    break;

  case 467: /* constant_expr: LONG_VALUE  */
#line 3592 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8729 "parser.cpp"
    break;

  case 468: /* constant_expr: DATE STRING  */
#line 3597 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8739 "parser.cpp"
    break;

  case 469: /* constant_expr: TIME STRING  */
#line 3602 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8749 "parser.cpp"
    break;

  case 470: /* constant_expr: DATETIME STRING  */
#line 3607 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8759 "parser.cpp"
    break;

  case 471: /* constant_expr: TIMESTAMP STRING  */
#line 3612 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8769 "parser.cpp"
    break;

  case 472: /* constant_expr: INTERVAL interval_expr  */
#line 3617 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8777 "parser.cpp"
    break;

  case 473: /* constant_expr: interval_expr  */
#line 3620 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8785 "parser.cpp"
    break;

  case 474: /* constant_expr: common_array_expr  */
#line 3623 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8793 "parser.cpp"
    break;

  case 475: /* constant_expr: curly_brackets_expr  */
#line 3626 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8801 "parser.cpp"
    break;

  case 476: /* common_array_expr: array_expr  */
#line 3630 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8809 "parser.cpp"
    break;

  case 477: /* common_array_expr: subarray_array_expr  */
#line 3633 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8817 "parser.cpp"
    break;

  case 478: /* common_array_expr: sparse_array_expr  */
#line 3636 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8825 "parser.cpp"
    break;

  case 479: /* common_array_expr: empty_array_expr  */
#line 3639 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8833 "parser.cpp"
    break;

  case 480: /* common_sparse_array_expr: sparse_array_expr  */
#line 3643 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8841 "parser.cpp"
    break;

  case 481: /* common_sparse_array_expr: array_expr  */
#line 3646 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8849 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: empty_array_expr  */
#line 3649 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8857 "parser.cpp"
    break;

  case 483: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3653 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8865 "parser.cpp"
    break;

  case 484: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3657 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8875 "parser.cpp"
    break;

  case 485: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3662 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8884 "parser.cpp"
    break;

  case 486: /* sparse_array_expr: long_sparse_array_expr  */
#line 3667 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8892 "parser.cpp"
    break;

  case 487: /* sparse_array_expr: double_sparse_array_expr  */
#line 3670 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8900 "parser.cpp"
    break;

  case 488: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3674 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8908 "parser.cpp"
    break;

  case 489: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3678 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8920 "parser.cpp"
    break;

  case 490: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3685 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8931 "parser.cpp"
    break;

  case 491: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3692 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8939 "parser.cpp"
    break;

  case 492: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3696 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8951 "parser.cpp"
    break;

  case 493: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3703 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8962 "parser.cpp"
    break;

  case 494: /* empty_array_expr: '[' ']'  */
#line 3710 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8970 "parser.cpp"
    break;

  case 495: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3714 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8978 "parser.cpp"
    break;

  case 496: /* curly_brackets_expr: '{' '}'  */
#line 3717 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 8986 "parser.cpp"
    break;

  case 497: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3721 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 8995 "parser.cpp"
    break;

  case 498: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3725 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9004 "parser.cpp"
    break;

  case 499: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3730 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9012 "parser.cpp"
    break;

  case 500: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3734 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9020 "parser.cpp"
    break;

  case 501: /* array_expr: long_array_expr  */
#line 3738 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9028 "parser.cpp"
    break;

  case 502: /* array_expr: double_array_expr  */
#line 3741 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9036 "parser.cpp"
    break;

  case 503: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3745 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9044 "parser.cpp"
    break;

  case 504: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3749 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9054 "parser.cpp"
    break;

  case 505: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3754 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9063 "parser.cpp"
    break;

  case 506: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3759 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9071 "parser.cpp"
    break;

  case 507: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3763 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9081 "parser.cpp"
    break;

  case 508: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3768 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9090 "parser.cpp"
    break;

  case 509: /* interval_expr: LONG_VALUE SECONDS  */
#line 3773 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9101 "parser.cpp"
    break;

  case 510: /* interval_expr: LONG_VALUE SECOND  */
#line 3779 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9112 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE MINUTES  */
#line 3785 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9123 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE MINUTE  */
#line 3791 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9134 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE HOURS  */
#line 3797 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9145 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE HOUR  */
#line 3803 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9156 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE DAYS  */
#line 3809 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9167 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE DAY  */
#line 3815 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9178 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE MONTHS  */
#line 3821 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9189 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE MONTH  */
#line 3827 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9200 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE YEARS  */
#line 3833 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9211 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE YEAR  */
#line 3839 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9222 "parser.cpp"
    break;

  case 521: /* copy_option_list: copy_option  */
#line 3850 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9231 "parser.cpp"
    break;

  case 522: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3854 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9240 "parser.cpp"
    break;

  case 523: /* copy_option: FORMAT IDENTIFIER  */
#line 3859 "parser.y"
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
#line 9276 "parser.cpp"
    break;

  case 524: /* copy_option: DELIMITER STRING  */
#line 3890 "parser.y"
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
#line 9291 "parser.cpp"
    break;

  case 525: /* copy_option: HEADER  */
#line 3900 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9301 "parser.cpp"
    break;

  case 526: /* copy_option: OFFSET LONG_VALUE  */
#line 3905 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9311 "parser.cpp"
    break;

  case 527: /* copy_option: LIMIT LONG_VALUE  */
#line 3910 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9321 "parser.cpp"
    break;

  case 528: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3915 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9331 "parser.cpp"
    break;

  case 529: /* file_path: STRING  */
#line 3921 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9339 "parser.cpp"
    break;

  case 530: /* if_exists: IF EXISTS  */
#line 3925 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9345 "parser.cpp"
    break;

  case 531: /* if_exists: %empty  */
#line 3926 "parser.y"
  { (yyval.bool_value) = false; }
#line 9351 "parser.cpp"
    break;

  case 532: /* if_not_exists: IF NOT EXISTS  */
#line 3928 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9357 "parser.cpp"
    break;

  case 533: /* if_not_exists: %empty  */
#line 3929 "parser.y"
  { (yyval.bool_value) = false; }
#line 9363 "parser.cpp"
    break;

  case 536: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3944 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9376 "parser.cpp"
    break;

  case 537: /* if_not_exists_info: %empty  */
#line 3952 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9384 "parser.cpp"
    break;

  case 538: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3956 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9392 "parser.cpp"
    break;

  case 539: /* with_index_param_list: %empty  */
#line 3959 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9400 "parser.cpp"
    break;

  case 540: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3963 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9408 "parser.cpp"
    break;

  case 541: /* optional_table_properties_list: %empty  */
#line 3966 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9416 "parser.cpp"
    break;

  case 542: /* index_param_list: index_param  */
#line 3970 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9425 "parser.cpp"
    break;

  case 543: /* index_param_list: index_param_list ',' index_param  */
#line 3974 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9434 "parser.cpp"
    break;

  case 544: /* index_param: IDENTIFIER  */
#line 3979 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9445 "parser.cpp"
    break;

  case 545: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3985 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9460 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER '=' STRING  */
#line 3995 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9475 "parser.cpp"
    break;

  case 547: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4005 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9488 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4013 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9501 "parser.cpp"
    break;

  case 549: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4024 "parser.y"
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
#line 9537 "parser.cpp"
    break;

  case 550: /* index_info: '(' IDENTIFIER ')'  */
#line 4055 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9548 "parser.cpp"
    break;


#line 9552 "parser.cpp"

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

#line 4062 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

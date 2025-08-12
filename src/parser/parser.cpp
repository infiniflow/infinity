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
  YYSYMBOL_ADD = 24,                       /* ADD  */
  YYSYMBOL_RENAME = 25,                    /* RENAME  */
  YYSYMBOL_DUMP = 26,                      /* DUMP  */
  YYSYMBOL_EXCEPT = 27,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 28,                     /* FLUSH  */
  YYSYMBOL_USE = 29,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 30,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 31,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 32,                  /* DATABASE  */
  YYSYMBOL_TABLE = 33,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 34,                /* COLLECTION  */
  YYSYMBOL_TABLES = 35,                    /* TABLES  */
  YYSYMBOL_INTO = 36,                      /* INTO  */
  YYSYMBOL_VALUES = 37,                    /* VALUES  */
  YYSYMBOL_VIEW = 38,                      /* VIEW  */
  YYSYMBOL_INDEX = 39,                     /* INDEX  */
  YYSYMBOL_TASKS = 40,                     /* TASKS  */
  YYSYMBOL_DATABASES = 41,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 42,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 43,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 44,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 45,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 46,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 47,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 48,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 49,                     /* CHUNK  */
  YYSYMBOL_SYSTEM = 50,                    /* SYSTEM  */
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
  YYSYMBOL_ARRAY = 117,                    /* ARRAY  */
  YYSYMBOL_TUPLE = 118,                    /* TUPLE  */
  YYSYMBOL_EMBEDDING = 119,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 120,                   /* VECTOR  */
  YYSYMBOL_MULTIVECTOR = 121,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 122,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 123,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 124,              /* TENSORARRAY  */
  YYSYMBOL_BIT = 125,                      /* BIT  */
  YYSYMBOL_TEXT = 126,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 127,                  /* PRIMARY  */
  YYSYMBOL_KEY = 128,                      /* KEY  */
  YYSYMBOL_UNIQUE = 129,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 130,                 /* NULLABLE  */
  YYSYMBOL_IS = 131,                       /* IS  */
  YYSYMBOL_DEFAULT = 132,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 133,                  /* COMMENT  */
  YYSYMBOL_IGNORE = 134,                   /* IGNORE  */
  YYSYMBOL_TRUE = 135,                     /* TRUE  */
  YYSYMBOL_FALSE = 136,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 137,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 138,                   /* SECOND  */
  YYSYMBOL_SECONDS = 139,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 140,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 141,                  /* MINUTES  */
  YYSYMBOL_HOUR = 142,                     /* HOUR  */
  YYSYMBOL_HOURS = 143,                    /* HOURS  */
  YYSYMBOL_DAY = 144,                      /* DAY  */
  YYSYMBOL_DAYS = 145,                     /* DAYS  */
  YYSYMBOL_MONTH = 146,                    /* MONTH  */
  YYSYMBOL_MONTHS = 147,                   /* MONTHS  */
  YYSYMBOL_YEAR = 148,                     /* YEAR  */
  YYSYMBOL_YEARS = 149,                    /* YEARS  */
  YYSYMBOL_EQUAL = 150,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 151,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 152,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 153,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 154,                  /* BETWEEN  */
  YYSYMBOL_AND = 155,                      /* AND  */
  YYSYMBOL_OR = 156,                       /* OR  */
  YYSYMBOL_EXTRACT = 157,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 158,                     /* LIKE  */
  YYSYMBOL_DATA = 159,                     /* DATA  */
  YYSYMBOL_LOG = 160,                      /* LOG  */
  YYSYMBOL_BUFFER = 161,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 162,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 163,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 164,                 /* MEMINDEX  */
  YYSYMBOL_USING = 165,                    /* USING  */
  YYSYMBOL_SESSION = 166,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 167,                   /* GLOBAL  */
  YYSYMBOL_OFF = 168,                      /* OFF  */
  YYSYMBOL_EXPORT = 169,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 170,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 171,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 172,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 173,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 174,                 /* VARIABLE  */
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
  YYSYMBOL_LEADER = 186,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 187,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 188,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 189,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 190,               /* STANDALONE  */
  YYSYMBOL_NODES = 191,                    /* NODES  */
  YYSYMBOL_NODE = 192,                     /* NODE  */
  YYSYMBOL_REMOVE = 193,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 194,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 195,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 196,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 197,                  /* RESTORE  */
  YYSYMBOL_CACHES = 198,                   /* CACHES  */
  YYSYMBOL_PERSISTENCE = 199,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 200,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 201,                  /* OBJECTS  */
  YYSYMBOL_FILES = 202,                    /* FILES  */
  YYSYMBOL_MEMORY = 203,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 204,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 205,                  /* HISTORY  */
  YYSYMBOL_CHECK = 206,                    /* CHECK  */
  YYSYMBOL_CLEAN = 207,                    /* CLEAN  */
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
  YYSYMBOL_highlight_clause = 266,         /* highlight_clause  */
  YYSYMBOL_from_clause = 267,              /* from_clause  */
  YYSYMBOL_search_clause = 268,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 269, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 270,             /* where_clause  */
  YYSYMBOL_having_clause = 271,            /* having_clause  */
  YYSYMBOL_group_by_clause = 272,          /* group_by_clause  */
  YYSYMBOL_set_operator = 273,             /* set_operator  */
  YYSYMBOL_table_reference = 274,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 275,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 276,     /* table_reference_name  */
  YYSYMBOL_table_name = 277,               /* table_name  */
  YYSYMBOL_table_alias = 278,              /* table_alias  */
  YYSYMBOL_with_clause = 279,              /* with_clause  */
  YYSYMBOL_with_expr_list = 280,           /* with_expr_list  */
  YYSYMBOL_with_expr = 281,                /* with_expr  */
  YYSYMBOL_join_clause = 282,              /* join_clause  */
  YYSYMBOL_join_type = 283,                /* join_type  */
  YYSYMBOL_show_statement = 284,           /* show_statement  */
  YYSYMBOL_flush_statement = 285,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 286,       /* optimize_statement  */
  YYSYMBOL_command_statement = 287,        /* command_statement  */
  YYSYMBOL_compact_statement = 288,        /* compact_statement  */
  YYSYMBOL_admin_statement = 289,          /* admin_statement  */
  YYSYMBOL_alter_statement = 290,          /* alter_statement  */
  YYSYMBOL_check_statement = 291,          /* check_statement  */
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
#define YYFINAL  132
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1598

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  227
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  551
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1255

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
       0,   519,   519,   523,   529,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   553,   554,   555,   556,   557,   558,   559,   560,   561,
     562,   563,   564,   571,   588,   605,   621,   650,   665,   697,
     715,   733,   761,   792,   796,   801,   805,   811,   814,   821,
     841,   863,   887,   913,   917,   923,   924,   925,   926,   927,
     928,   929,   930,   931,   932,   933,   934,   935,   936,   937,
     938,   939,   940,   941,   942,   943,   946,   948,   949,   950,
     951,   954,   955,   956,   957,   958,   959,   960,   961,   962,
     963,   964,   965,   966,   967,   968,   969,   970,   971,   972,
     973,   974,   975,   976,   977,   978,   979,   980,   981,   982,
     983,   984,   985,   986,   987,   988,   989,   990,   991,   992,
     993,   994,   995,   996,   997,   998,   999,  1000,  1001,  1002,
    1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,  1012,
    1013,  1014,  1015,  1016,  1017,  1018,  1019,  1020,  1024,  1047,
    1051,  1061,  1064,  1067,  1070,  1074,  1077,  1082,  1087,  1094,
    1100,  1110,  1126,  1164,  1180,  1183,  1190,  1207,  1216,  1229,
    1233,  1238,  1251,  1264,  1279,  1294,  1309,  1332,  1385,  1440,
    1491,  1494,  1497,  1506,  1516,  1519,  1523,  1528,  1555,  1558,
    1563,  1580,  1583,  1587,  1591,  1596,  1602,  1605,  1608,  1612,
    1616,  1618,  1622,  1624,  1627,  1631,  1634,  1638,  1641,  1645,
    1650,  1654,  1657,  1661,  1664,  1668,  1671,  1675,  1678,  1682,
    1685,  1688,  1691,  1699,  1702,  1717,  1717,  1719,  1733,  1742,
    1747,  1756,  1761,  1766,  1772,  1779,  1782,  1786,  1789,  1794,
    1806,  1813,  1827,  1830,  1833,  1836,  1839,  1842,  1845,  1851,
    1855,  1859,  1863,  1867,  1874,  1878,  1882,  1886,  1890,  1895,
    1899,  1904,  1908,  1912,  1916,  1922,  1928,  1934,  1945,  1956,
    1967,  1979,  1991,  2004,  2018,  2029,  2043,  2059,  2076,  2080,
    2084,  2090,  2094,  2098,  2104,  2108,  2112,  2116,  2122,  2126,
    2132,  2140,  2144,  2148,  2152,  2160,  2171,  2194,  2200,  2205,
    2211,  2217,  2225,  2231,  2237,  2243,  2249,  2257,  2263,  2269,
    2275,  2281,  2289,  2295,  2301,  2309,  2317,  2323,  2329,  2335,
    2341,  2345,  2359,  2372,  2378,  2385,  2393,  2402,  2412,  2422,
    2433,  2444,  2456,  2468,  2478,  2489,  2501,  2514,  2518,  2523,
    2528,  2534,  2538,  2542,  2548,  2552,  2556,  2562,  2568,  2576,
    2582,  2586,  2592,  2596,  2602,  2607,  2612,  2619,  2628,  2638,
    2647,  2659,  2671,  2675,  2691,  2695,  2700,  2710,  2732,  2738,
    2742,  2743,  2744,  2745,  2746,  2748,  2751,  2757,  2760,  2761,
    2762,  2763,  2764,  2765,  2766,  2767,  2768,  2769,  2773,  2789,
    2806,  2824,  2881,  2931,  2985,  3043,  3067,  3090,  3111,  3132,
    3141,  3152,  3163,  3177,  3184,  3194,  3200,  3212,  3215,  3218,
    3221,  3224,  3227,  3231,  3235,  3240,  3248,  3256,  3265,  3272,
    3279,  3286,  3293,  3300,  3307,  3314,  3321,  3328,  3335,  3342,
    3350,  3358,  3366,  3374,  3382,  3390,  3398,  3406,  3414,  3422,
    3430,  3438,  3468,  3476,  3485,  3493,  3502,  3510,  3516,  3523,
    3529,  3536,  3541,  3548,  3555,  3563,  3576,  3582,  3588,  3595,
    3603,  3610,  3617,  3622,  3632,  3637,  3642,  3647,  3652,  3657,
    3662,  3667,  3672,  3677,  3680,  3683,  3686,  3690,  3693,  3696,
    3699,  3703,  3706,  3709,  3713,  3717,  3722,  3727,  3730,  3734,
    3738,  3745,  3752,  3756,  3763,  3770,  3774,  3777,  3781,  3785,
    3790,  3794,  3798,  3801,  3805,  3809,  3814,  3819,  3823,  3828,
    3833,  3839,  3845,  3851,  3857,  3863,  3869,  3875,  3881,  3887,
    3893,  3899,  3910,  3914,  3919,  3950,  3960,  3965,  3970,  3975,
    3981,  3985,  3986,  3988,  3989,  3991,  3992,  4004,  4012,  4016,
    4019,  4023,  4026,  4030,  4034,  4039,  4045,  4055,  4065,  4073,
    4084,  4115
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
  "PREPARE", "UNION", "ALL", "INTERSECT", "COMPACT", "ADD", "RENAME",
  "DUMP", "EXCEPT", "FLUSH", "USE", "OPTIMIZE", "PROPERTIES", "DATABASE",
  "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "VIEW", "INDEX",
  "TASKS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS", "COLUMN",
  "COLUMNS", "INDEXES", "CHUNK", "SYSTEM", "GROUP", "BY", "HAVING", "AS",
  "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER",
  "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC",
  "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT",
  "HEADER", "HIGHLIGHT", "CAST", "END", "CASE", "ELSE", "THEN", "WHEN",
  "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT",
  "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME",
  "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "ARRAY", "TUPLE", "EMBEDDING", "VECTOR", "MULTIVECTOR",
  "TENSOR", "SPARSE", "TENSORARRAY", "BIT", "TEXT", "PRIMARY", "KEY",
  "UNIQUE", "NULLABLE", "IS", "DEFAULT", "COMMENT", "IGNORE", "TRUE",
  "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR",
  "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL",
  "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT",
  "LIKE", "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION",
  "MEMINDEX", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "CONFIGS",
  "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "LOGS", "CATALOGS",
  "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES", "FUSION",
  "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER", "LEARNER", "CONNECT",
  "STANDALONE", "NODES", "NODE", "REMOVE", "SNAPSHOT", "SNAPSHOTS",
  "RECOVER", "RESTORE", "CACHES", "PERSISTENCE", "OBJECT", "OBJECTS",
  "FILES", "MEMORY", "ALLOCATION", "HISTORY", "CHECK", "CLEAN", "NUMBER",
  "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'",
  "'('", "')'", "'.'", "';'", "','", "'}'", "'{'", "':'", "$accept",
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
  "highlight_clause", "from_clause", "search_clause",
  "optional_search_filter_expr", "where_clause", "having_clause",
  "group_by_clause", "set_operator", "table_reference",
  "table_reference_unit", "table_reference_name", "table_name",
  "table_alias", "with_clause", "with_expr_list", "with_expr",
  "join_clause", "join_type", "show_statement", "flush_statement",
  "optimize_statement", "command_statement", "compact_statement",
  "admin_statement", "alter_statement", "check_statement", "expr_array",
  "insert_row_list", "expr_alias", "expr", "operand", "match_tensor_expr",
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

#define YYPACT_NINF (-760)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-539)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     939,    61,    -5,   274,    55,    21,    55,   338,   815,   882,
      87,   130,   147,   234,   189,   211,   231,    91,   153,   527,
     218,   119,   -35,   286,    81,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,   356,  -760,  -760,   307,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,   264,   264,   264,   264,    -4,
     386,    55,   335,   335,   335,   335,   335,   411,   205,   414,
      55,   -21,   449,   467,   485,   102,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,   356,  -760,  -760,  -760,  -760,  -760,   227,
     487,    55,  -760,  -760,  -760,  -760,  -760,     8,  -760,   347,
     476,  -760,   490,  -760,  -760,   403,   491,  -760,   499,  -760,
    -760,   435,    11,    55,    55,   509,  -760,  -760,  -760,  -760,
    -760,   -44,  -760,   460,   302,  -760,   524,   354,   359,   333,
     844,   363,   537,   373,   492,   382,   388,    55,  -760,  -760,
     349,   366,  -760,    75,  -760,   567,  -760,  -760,    19,   558,
    -760,   544,   557,   639,    55,    55,    55,   658,   612,   615,
     461,   614,   682,    55,    55,    55,   686,  -760,   688,   690,
     633,   696,   696,   595,    73,   136,   270,  -760,   493,  -760,
     453,  -760,  -760,  -760,   702,  -760,   723,  -760,   696,  -760,
    -760,   724,  -760,  -760,  -760,  -760,   459,  -760,   673,    55,
     517,   231,   696,  -760,   734,  -760,   575,  -760,   751,  -760,
    -760,   759,  -760,   760,  -760,   773,   799,  -760,   809,   765,
     819,   636,   832,   834,  -760,  -760,  -760,  -760,  -760,    19,
    -760,  -760,  -760,   595,   795,   775,   776,   721,   -36,  -760,
     461,  -760,    55,   416,   855,    89,  -760,  -760,  -760,  -760,
    -760,   801,  -760,   651,   -33,  -760,   595,  -760,  -760,   791,
     792,   648,  -760,  -760,  1117,   715,   652,   659,   426,   875,
     878,   879,   880,  -760,  -760,   884,   667,   672,   674,   676,
     678,   684,   692,   334,   694,   697,   764,   764,  -760,    18,
     546,    45,   190,  -760,    -7,   916,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,   698,
    -760,  -760,  -760,    72,  -760,  -760,    93,  -760,   167,  -760,
    -760,   392,  -760,  -760,   240,  -760,   249,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,   904,   899,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,   872,   874,   848,    55,   849,   307,  -760,
    -760,  -760,   921,   111,  -760,   924,  -760,  -760,   857,   568,
    -760,   927,  -760,  -760,   716,   717,   -29,   595,   595,   867,
    -760,   934,   -35,    77,   885,   722,   954,   955,  -760,  -760,
     283,   725,  -760,    55,   595,   690,  -760,   617,   741,   742,
     261,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,   764,   744,   951,   865,   595,   595,   124,
     337,  -760,  -760,  -760,  -760,  1117,  -760,   595,   595,   595,
     595,   595,   595,   960,   747,   752,   753,   754,   966,   970,
     618,   618,  -760,   749,  -760,  -760,  -760,  -760,   739,   171,
    -760,  -760,   903,   595,   977,   595,   595,   -14,   763,   -53,
     764,   764,   764,   764,   764,   764,   764,   764,   764,   764,
     764,   764,   764,   764,    23,  -760,   769,  -760,   979,  -760,
     981,   183,  -760,  -760,   990,  -760,   992,   958,   740,   782,
     783,  1000,  -760,   789,  -760,   800,  -760,  1003,  -760,   164,
    1006,   846,   850,  -760,  -760,  -760,   595,   949,   802,  -760,
     132,   617,   595,  -760,  -760,   310,  1093,   898,   810,   322,
    -760,  -760,  -760,   -35,  1025,   897,  -760,  -760,  -760,  1028,
     595,   814,  -760,   617,  -760,   301,   301,   595,  -760,   367,
     865,   883,   812,     2,   -49,   446,  -760,   595,   595,   192,
     195,   197,   199,   201,   217,   965,   595,    25,   595,  1029,
     827,   383,   790,  -760,  -760,   696,  -760,  -760,  -760,   887,
     836,   764,   546,   926,  -760,   964,   964,   324,   324,   941,
     964,   964,   324,   324,   618,   618,  -760,  -760,  -760,  -760,
    -760,  -760,   825,  -760,   835,  -760,  -760,  -760,  -760,  1052,
    1054,  -760,   855,  1059,  -760,  1062,  -760,  -760,  1065,  -760,
    -760,  1069,  1071,   856,    13,   900,   595,  -760,  -760,  -760,
     617,  1079,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,   868,  -760,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,   869,   870,   871,   881,
     886,   888,   891,   892,   417,   893,   855,  1053,    77,   356,
     866,  1094,  -760,   391,   894,  1091,  1100,  1114,  1116,  -760,
    1115,   404,  -760,   421,   442,  -760,   913,  -760,  1093,   595,
    -760,   595,   -20,   168,  -760,  -760,  -760,  -760,  -760,  -760,
     764,  -126,  -760,  -150,   -94,   911,    37,   915,  -760,  1119,
    -760,  -760,  1043,   546,   964,   917,   443,  -760,   764,  1132,
    1134,  1092,  1096,   455,   479,  -760,   931,   484,  -760,  1137,
    -760,  -760,   -35,   925,   691,  -760,   137,  -760,   375,   633,
    -760,  -760,  1138,  1093,  1093,   488,   619,   784,   902,  1129,
    1146,  1017,  1021,  -760,  -760,   332,  -760,  1027,   855,   494,
     950,  1035,  -760,  1005,  -760,  -760,   595,  -760,  -760,  -760,
    -760,  -760,  -760,   301,  -760,  -760,  -760,   952,   617,    54,
    -760,   595,   444,   595,   769,   988,  1167,   975,   595,  -760,
     986,   989,  1004,   522,  -760,  -760,   951,  1203,  1217,  -760,
    -760,  1059,   628,  -760,  1062,    48,    26,    13,  1171,  -760,
    -760,  -760,  -760,  -760,  -760,  1172,  -760,  1226,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  1007,  1180,   541,  1020,
     563,  -760,  1018,  1022,  1023,  1024,  1030,  1044,  1045,  1046,
    1047,  1153,  1049,  1050,  1051,  1055,  1056,  1057,  1058,  1060,
    1061,  1063,  1157,  1064,  1066,  1067,  1068,  1070,  1072,  1073,
    1074,  1075,  1076,  1161,  1077,  1078,  1080,  1081,  1082,  1083,
    1084,  1085,  1086,  1087,  1184,  1088,  1089,  1090,  1095,  1097,
    1098,  1099,  1101,  1102,  1103,  1185,  1104,  1105,  1106,  1107,
    1108,  1109,  1110,  1111,  1112,  1113,  1186,  1118,  -760,  -760,
     183,  -760,  1149,  1152,   569,  -760,  1062,  1284,  1289,   570,
    -760,  -760,  -760,   617,  -760,   141,  1120,    15,  1121,  -760,
    -760,  -760,  1122,  1229,  1126,   617,  -760,   301,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  1296,  -760,
     137,   691,    13,    13,  1123,   375,  1262,  1263,  -760,  1309,
    -760,  -760,  1093,  1311,  1313,  1317,  1331,  1332,  1333,  1334,
    1341,  1342,  1127,  1343,  1345,  1346,  1347,  1348,  1349,  1350,
    1351,  1352,  1353,  1139,  1354,  1355,  1357,  1358,  1359,  1360,
    1361,  1362,  1363,  1364,  1148,  1366,  1367,  1368,  1369,  1370,
    1371,  1372,  1373,  1374,  1375,  1159,  1377,  1378,  1379,  1380,
    1381,  1382,  1383,  1384,  1385,  1386,  1170,  1388,  1389,  1390,
    1391,  1392,  1393,  1394,  1395,  1396,  1397,  1181,  1399,  -760,
    1402,  1403,  -760,   571,  -760,   849,  -760,  1404,  1405,  1406,
     113,  1191,  -760,   580,  1407,  -760,  -760,  1356,   855,  -760,
     595,   595,  -760,  1192,  -760,  1194,  1195,  1196,  1198,  1199,
    1200,  1201,  1202,  1204,  1417,  1205,  1206,  1207,  1208,  1209,
    1210,  1211,  1212,  1213,  1214,  1429,  1216,  1218,  1219,  1220,
    1221,  1222,  1223,  1224,  1225,  1227,  1431,  1228,  1230,  1231,
    1232,  1233,  1234,  1235,  1236,  1237,  1238,  1440,  1239,  1240,
    1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1443,  1249,
    1250,  1251,  1252,  1253,  1254,  1255,  1256,  1257,  1258,  1473,
    1260,  -760,  -760,  -760,  -760,  1259,  1261,  1264,   975,  1316,
    -760,   578,   595,   581,   856,   617,  -760,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  1265,  -760,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  1266,  -760,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  1268,  -760,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  1269,
    -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,
    1270,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,  -760,
    -760,  1271,  -760,  1479,  1488,    59,  1273,  1274,  1447,  1490,
    1491,  -760,  -760,  -760,   617,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  1275,  1276,   975,   849,  1335,  1282,  1496,   761,
      68,  1283,  1500,  1285,  -760,  1458,  1503,   768,  1502,  -760,
     975,   849,   975,   -11,  1290,  1291,  1504,  -760,  1465,  1293,
    -760,  1294,  1476,  1477,  -760,  1513,  -760,  -760,  -760,   176,
     210,  -760,  1299,  1300,  1480,  1482,  -760,  1483,  1484,  1521,
    -760,  -760,  1306,  -760,  1307,  1308,  1524,  1526,   849,  1310,
    1312,  -760,   849,  -760,  -760
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     236,     0,     0,     0,     0,     0,     0,     0,   236,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   236,     0,   536,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   181,   180,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   534,   534,   534,   534,   534,
       0,     0,   532,   532,   532,   532,   532,     0,   229,     0,
       0,     0,     0,     0,     0,   236,   167,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
       0,     0,   250,   251,   249,   255,   259,     0,   256,     0,
       0,   252,     0,   254,   278,   279,     0,   257,     0,   288,
     290,     0,   284,     0,     0,     0,   291,   292,   293,   294,
     297,   229,   295,     0,   235,   237,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   362,   320,
       0,     0,     1,   236,     2,   219,   221,   222,     0,   204,
     186,   192,     0,     0,     0,     0,     0,     0,     0,     0,
     165,     0,     0,     0,     0,     0,     0,   317,     0,     0,
     214,     0,     0,     0,     0,     0,     0,   166,     0,   266,
     267,   260,   261,   262,     0,   263,     0,   253,     0,   258,
     289,     0,   282,   281,   285,   286,     0,   322,     0,     0,
       0,     0,     0,   344,     0,   354,     0,   355,     0,   341,
     342,     0,   337,     0,   350,   352,     0,   345,     0,     0,
       0,     0,     0,     0,   363,   185,   184,     4,   220,     0,
     182,   183,   203,     0,     0,   200,     0,    34,     0,    35,
     165,   537,     0,     0,     0,   236,   531,   172,   174,   173,
     175,     0,   230,     0,   214,   169,     0,   161,   530,     0,
       0,   460,   464,   467,   468,     0,     0,     0,     0,     0,
       0,     0,     0,   465,   466,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   462,     0,
     236,     0,     0,   364,   369,   370,   384,   382,   385,   383,
     386,   387,   379,   374,   373,   372,   380,   381,   371,   378,
     377,   475,   478,     0,   479,   487,     0,   488,     0,   480,
     476,     0,   477,   502,     0,   503,     0,   474,   301,   303,
     302,   299,   300,   306,   308,   307,   304,   305,   311,   313,
     312,   309,   310,   287,     0,     0,   269,   268,   274,   264,
     265,   280,   283,     0,     0,     0,     0,   540,     0,   238,
     298,   347,     0,   338,   343,     0,   351,   346,     0,     0,
     353,     0,   318,   319,     0,     0,   206,     0,     0,   202,
     533,     0,   236,     0,     0,     0,     0,     0,   316,   159,
       0,     0,   163,     0,     0,     0,   168,   213,     0,     0,
       0,   511,   510,   513,   512,   515,   514,   517,   516,   519,
     518,   521,   520,     0,     0,   426,   236,     0,     0,     0,
       0,   469,   470,   471,   472,     0,   473,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     428,   427,   508,   505,   495,   485,   490,   493,     0,     0,
     497,   498,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   484,     0,   489,     0,   492,
       0,     0,   496,   504,     0,   507,     0,   275,   270,     0,
       0,     0,   321,     0,   296,     0,   356,     0,   339,     0,
       0,     0,     0,   349,   189,   188,     0,   208,   191,   193,
     198,   199,     0,   187,    33,    37,     0,     0,     0,     0,
      43,    47,    48,   236,     0,    41,   315,   314,   164,     0,
       0,   162,   176,   171,   170,     0,     0,     0,   415,     0,
     236,     0,     0,     0,     0,     0,   451,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     212,     0,     0,   376,   375,     0,   365,   368,   444,   445,
       0,     0,   236,     0,   425,   435,   436,   439,   440,     0,
     442,   434,   437,   438,   430,   429,   431,   432,   433,   461,
     463,   486,     0,   491,     0,   494,   499,   506,   509,     0,
       0,   271,     0,     0,   359,     0,   239,   340,     0,   323,
     348,     0,     0,   205,     0,   210,     0,   196,   197,   195,
     201,     0,    55,    58,    59,    56,    57,    60,    61,    77,
      62,    64,    63,    80,    67,    68,    69,    65,    66,    70,
      71,    72,    73,    74,    75,    76,     0,     0,     0,     0,
       0,     0,     0,     0,   540,     0,     0,   542,     0,    40,
       0,     0,   160,     0,     0,     0,     0,     0,     0,   526,
       0,     0,   522,     0,     0,   416,     0,   456,     0,     0,
     449,     0,     0,     0,   423,   422,   421,   420,   419,   418,
       0,     0,   460,     0,     0,     0,     0,     0,   405,     0,
     501,   500,     0,   236,   443,     0,     0,   424,     0,     0,
       0,   276,   272,     0,     0,    45,   545,     0,   543,   324,
     357,   358,   236,   207,   223,   225,   234,   226,     0,   214,
     194,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   153,   156,   149,   156,     0,     0,
       0,    36,    44,   551,    42,   366,     0,   528,   527,   525,
     524,   529,   179,     0,   177,   417,   457,     0,   453,     0,
     452,     0,     0,     0,     0,     0,     0,   212,     0,   403,
       0,     0,     0,     0,   458,   447,   446,     0,     0,   361,
     360,     0,     0,   539,     0,     0,     0,     0,     0,   243,
     244,   245,   246,   242,   247,     0,   232,     0,   227,   409,
     407,   410,   408,   411,   412,   413,   209,   218,     0,     0,
       0,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   154,   151,
       0,   150,    50,    49,     0,   158,     0,     0,     0,     0,
     523,   455,   450,   454,   441,     0,     0,     0,     0,   481,
     483,   482,   212,     0,     0,   211,   406,     0,   459,   448,
     277,   273,    46,   546,   547,   549,   548,   544,     0,   325,
     234,   224,     0,     0,   231,     0,     0,   216,    79,     0,
     147,   148,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   155,
       0,     0,   157,     0,    38,   540,   367,     0,     0,     0,
       0,     0,   404,     0,   326,   228,   240,     0,     0,   414,
       0,     0,   190,     0,    54,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    52,    51,   541,   550,     0,     0,   212,   212,   399,
     178,     0,     0,     0,   217,   215,    78,    84,    85,    82,
      83,    86,    87,    88,    89,    90,     0,    81,   128,   129,
     126,   127,   130,   131,   132,   133,   134,     0,   125,    95,
      96,    93,    94,    97,    98,    99,   100,   101,     0,    92,
     106,   107,   104,   105,   108,   109,   110,   111,   112,     0,
     103,   139,   140,   137,   138,   141,   142,   143,   144,   145,
       0,   136,   117,   118,   115,   116,   119,   120,   121,   122,
     123,     0,   114,     0,     0,     0,     0,     0,     0,     0,
       0,   328,   327,   333,   241,   233,    91,   135,   102,   113,
     146,   124,   212,     0,   212,   540,   400,     0,   334,   329,
       0,     0,     0,     0,   398,     0,     0,     0,     0,   330,
     212,   540,   212,   540,     0,     0,     0,   335,   331,     0,
     394,     0,     0,     0,   397,     0,   401,   336,   332,   540,
     388,   396,     0,     0,     0,     0,   393,     0,     0,     0,
     402,   392,     0,   390,     0,     0,     0,     0,   540,     0,
       0,   395,   540,   389,   391
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -760,  -760,  -760,  1398,  1468,    62,  -760,  -760,   889,  -578,
    -760,  -655,  -760,   803,   797,  -760,  -582,   293,   306,  1305,
    -760,   309,  -760,  1151,   321,   351,    -6,  1517,   -19,  1193,
    1321,   -66,  -760,  -760,   936,  -760,  -760,  -760,  -760,  -760,
    -760,  -760,  -759,  -237,  -760,  -760,  -760,  -760,   756,   -83,
     104,   624,  -760,  -760,  1365,  -760,  -760,   362,   369,   374,
     376,   380,  -760,  -760,  -760,  -222,  -760,  1124,  -246,  -233,
    -685,  -683,  -682,  -680,  -678,  -673,   620,  -760,  -760,  -760,
    -760,  -760,  -760,  1140,  -760,  -760,   999,  -276,  -273,  -760,
    -760,  -760,   785,  -760,  -760,  -760,  -760,   786,  -760,  -760,
    1125,  1128,   787,  -760,  -760,  -760,  -760,  1288,  -515,   794,
    -153,   616,   733,  -760,  -760,  -640,  -760,   662,   770,  -760
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   509,   704,   510,   511,
     810,   644,   735,   736,   882,   512,   380,    27,    28,   235,
      29,    30,   244,   245,    31,    32,    33,    34,    35,   140,
     220,   141,   225,   498,   499,   609,   369,   503,   223,   497,
     605,   719,   687,   247,  1022,   927,   138,   713,   714,   715,
     716,   798,    36,   114,   115,   717,   795,    37,    38,    39,
      40,    41,    42,    43,    44,   282,   521,   283,   284,   285,
     286,   287,   288,   289,   290,   291,   805,   806,   292,   293,
     294,   295,   296,   410,   297,   298,   299,   300,   301,   898,
     302,   303,   304,   305,   306,   307,   308,   309,   310,   311,
     436,   437,   312,   313,   314,   315,   316,   317,   661,   662,
     249,   152,   143,   134,   148,   484,   741,   707,   708,   515
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     387,   366,    73,   131,   737,   441,   435,   386,   904,   250,
     703,   663,   409,   757,   171,   705,    58,   189,   372,   563,
     432,   433,   405,   432,   433,   341,   579,   139,   682,   445,
     446,    51,   246,   799,   439,   800,   801,   669,   802,   350,
     803,   767,    16,   430,   431,   804,   135,   444,   136,   252,
     253,   254,   496,   137,   161,   162,   668,  -538,    58,    73,
     560,   445,   446,   760,   739,  1194,   483,   142,   809,   811,
      67,   464,   221,   764,  1210,  -535,   318,   564,   319,   320,
     506,   918,     1,   919,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    45,    46,    47,    60,   763,    11,    48,
      49,    12,   768,    13,    14,    15,   445,   446,    59,     1,
      61,     2,     3,     4,     5,     6,     7,  1098,     9,   112,
     103,   500,   501,  1222,   768,    11,   381,    67,    12,   765,
      13,    14,    15,   768,   321,   445,   446,   892,   523,   323,
     796,   324,   325,  1011,   561,   259,   260,   261,   445,   446,
     487,   262,    16,   365,  1223,   150,   884,   445,   446,   488,
     117,   533,   534,   104,   160,   118,    16,   119,   529,   120,
     405,   539,   540,   541,   542,   543,   544,   158,   768,    16,
     263,   264,   265,   373,    22,   170,   105,   252,   253,   254,
     385,   797,   110,   581,   443,   586,   598,   326,   163,   558,
     559,   607,   608,   912,   507,   599,   508,   186,   187,   445,
     446,   408,   184,   172,   111,   185,   382,   565,   566,   567,
     568,   569,   570,   571,   572,   573,   574,   575,   576,   577,
     578,   214,   712,   434,   113,   279,   434,   580,   219,   278,
     799,   322,   800,   801,    17,   802,   920,   803,   228,   229,
     230,   127,   804,   483,   761,    50,   610,   238,   239,   240,
      18,   438,   279,   116,   251,   252,   253,   254,   128,   440,
     281,    17,    19,   328,   603,   329,   330,  1024,   129,   445,
     446,    20,    21,   259,   260,   261,   132,   445,   446,   262,
     465,   672,   673,   347,    22,   466,   445,   446,   653,    19,
     681,    68,   684,   133,   327,   664,    52,    53,    54,    21,
    1234,   467,    55,    56,    69,   139,   468,    70,   263,   264,
     265,    22,   121,   445,   446,   527,   445,   446,   694,    71,
     135,   331,   136,   255,   256,   142,   375,   137,  1176,  1177,
     696,  1235,   122,   257,  1237,   258,   123,   445,   446,   124,
     445,   446,   445,   446,   445,   446,   445,   446,    68,    72,
     500,   259,   260,   261,  1007,  1094,   505,   262,   655,   656,
      74,    69,   445,   446,    70,  1238,   135,    75,   136,   657,
     658,   659,    76,   137,    77,   469,    71,   532,    78,   149,
     470,   554,  1013,   106,   107,   108,   263,   264,   265,   266,
     279,   267,   692,   268,   731,   269,   151,   270,   281,   271,
     442,   109,   674,   443,   157,   675,    72,   676,   272,   677,
     536,   678,   537,   758,   538,   759,   158,    74,   159,   251,
     252,   253,   254,  1201,    75,  1203,  1103,   679,   332,    76,
     273,    77,   274,   611,   275,    78,   168,   762,   376,   377,
     482,  1219,   164,  1221,   424,   449,   425,   426,   473,   732,
     427,   733,   734,   474,   880,   776,   378,   475,    57,   343,
     165,   773,   476,   276,   277,   278,  -539,  -539,   279,   178,
     280,   528,   807,   344,   345,   660,   281,   522,   166,   731,
     169,   896,   334,   177,   483,   335,   336,   179,   255,   256,
     337,   338,   180,   518,    62,    63,   519,   649,   257,    64,
     258,   666,   188,   408,   190,   893,   531,   895,   195,   196,
     173,   174,   905,   197,   889,   191,   259,   260,   261,   670,
     192,   671,   262,   538,  -539,  -539,   459,   460,   461,   462,
     463,   209,   647,   695,   732,   648,   733,   734,   193,   251,
     252,   253,   254,   194,   273,  1204,   274,   208,   275,   125,
     126,   263,   264,   265,   266,   210,   267,   211,   268,   215,
     269,  1220,   270,  1224,   271,   449,   212,   812,   813,   814,
     815,   816,   213,   272,   817,   818,   216,   665,   218,  1236,
     443,   819,   820,   821,   450,   451,   452,   453,   251,   252,
     253,   254,   455,   688,   999,   273,   689,   274,  1251,   275,
     224,   745,  1254,   822,   443,   471,   472,  1179,   255,   256,
    1180,  1181,   222,    16,   752,  1182,  1183,   753,   257,   226,
     258,   913,   914,   915,   916,   181,   182,   183,   276,   277,
     278,   754,   227,   279,   753,   280,   259,   260,   261,   175,
     176,   281,   262,   456,   457,   458,   459,   460,   461,   462,
     463,   231,   755,   775,   894,   443,   443,   255,   256,   153,
     154,   155,   156,   232,   772,   779,   233,   257,   519,   258,
     234,   263,   264,   265,   266,   237,   267,   236,   268,   241,
     269,   242,   270,   243,   271,   259,   260,   261,   246,   780,
     248,   262,   781,   272,   783,   339,   786,   784,   823,   824,
     825,   826,   827,   333,   885,   828,   829,   519,   251,   252,
     253,   254,   830,   831,   832,   273,   340,   274,   342,   275,
     263,   264,   265,   266,   346,   267,   348,   268,   351,   269,
     352,   270,   909,   271,   833,   443,   788,  -248,   789,   790,
     791,   792,   272,   793,   794,   491,   492,   353,   276,   277,
     278,   928,   354,   279,   929,   280,   355,   251,   252,   253,
     254,   281,   445,   446,   273,  1105,   274,   356,   275,   144,
     145,   146,   147,   931,   590,   591,   932,   403,   404,  1002,
    1006,  1093,   519,   443,   784,   690,   691,   257,  1104,   258,
    1100,  1185,   357,   753,   519,  1208,  1209,   276,   277,   278,
    1216,  1217,   279,   358,   280,   259,   260,   261,    65,   359,
     281,   262,     1,   360,     2,     3,     4,     5,     6,     7,
     361,     9,   461,   462,   463,   362,   403,   363,    11,  1016,
    1017,    12,   368,    13,    14,    15,   257,   367,   258,   370,
     263,   264,   265,   266,   371,   267,  1184,   268,   379,   269,
     384,   270,   383,   271,   259,   260,   261,   390,   388,   389,
     262,   406,   272,   834,   835,   836,   837,   838,   407,   411,
     839,   840,   412,   413,   414,    79,   417,   841,   842,   843,
     415,   418,    16,   419,   273,   420,   274,   421,   275,   263,
     264,   265,   266,   422,   267,   478,   268,   477,   269,   844,
     270,   423,   271,   428,    80,    81,   429,    82,   479,   464,
     480,   272,    83,    84,   481,   486,   483,   276,   277,   278,
     489,   493,   279,   490,   280,   502,   494,   495,   504,   513,
     281,   514,    16,   273,   520,   274,     1,   275,     2,     3,
       4,     5,     6,     7,     8,     9,    10,   516,   517,   553,
     525,   526,    11,   530,   545,    12,   546,    13,    14,    15,
     550,   547,   548,   549,   551,   552,   276,   277,   278,   555,
     557,   279,   562,   280,    17,   582,   279,   584,   447,   281,
     448,   845,   846,   847,   848,   849,   587,   588,   850,   851,
     589,   592,   593,   594,   198,   852,   853,   854,   595,   597,
     600,   601,    19,   531,   199,   602,    16,   200,   201,   202,
     596,   203,    21,   531,   604,   606,   645,   855,   650,   646,
     651,   652,   667,   685,    22,   204,   205,   654,   206,   207,
     680,   561,   445,    85,    86,    87,    88,   449,    89,    90,
     686,   699,    91,    92,    93,   693,   697,    94,   701,    95,
     702,   700,   506,    96,    97,   706,   450,   451,   452,   453,
     454,   709,   449,   710,   455,   711,    98,    99,   718,   443,
     100,   101,   449,   721,   740,   102,   743,   722,   723,   724,
     725,   450,   451,   452,   453,   449,   698,   747,   744,   455,
     726,   450,   451,   452,   453,   727,   748,   728,    17,   455,
     729,   730,   738,   746,  -539,  -539,   452,   453,   749,   750,
     771,   751,  -539,   770,    18,   456,   457,   458,   459,   460,
     461,   462,   463,   756,   766,   769,    19,   774,   691,   690,
     782,   777,   778,   785,   808,    20,    21,   878,   787,   879,
     456,   457,   458,   459,   460,   461,   462,   463,    22,   880,
     456,   457,   458,   459,   460,   461,   462,   463,   887,   886,
     888,   902,   891,  -539,   457,   458,   459,   460,   461,   462,
     463,   612,   613,   614,   615,   616,   617,   618,   619,   620,
     621,   622,   623,   624,   625,   626,   627,   628,   903,   629,
     630,   631,   632,   633,   634,   897,   906,   635,   907,   910,
     636,   637,   638,   639,   640,   641,   642,   643,   856,   857,
     858,   859,   860,   911,   908,   861,   862,   922,   923,   924,
     925,   926,   863,   864,   865,   867,   868,   869,   870,   871,
     930,   933,   872,   873,   942,   934,   935,   936,   953,   874,
     875,   876,   964,   937,   866,   391,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   938,   939,   940,
     941,   877,   943,   944,   945,   975,   986,   997,   946,   947,
     948,   949,  1000,   950,   951,  1001,   952,   954,  1004,   955,
     956,   957,  1005,   958,   768,   959,   960,   961,   962,   963,
     965,   966,  1014,   967,   968,   969,   970,   971,   972,   973,
     974,   976,   977,   978,  1020,  1023,  1021,  1025,   979,  1026,
     980,   981,   982,  1027,   983,   984,   985,   987,   988,   989,
     990,   991,   992,   993,   994,   995,   996,  1028,  1029,  1030,
    1031,   998,  1018,  1008,  1009,  1010,  1012,  1032,  1033,  1035,
    1034,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,
    1046,  1047,  1045,  1048,  1049,  1050,  1051,  1052,  1053,  1054,
    1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,
    1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1089,  1090,  1091,  1092,  1095,  1096,
    1097,  1099,  1106,  1101,  1107,  1108,  1109,  1102,  1110,  1111,
    1112,  1113,  1114,  1116,  1115,  1117,  1118,  1119,  1120,  1121,
    1122,  1123,  1124,  1125,  1126,  1127,  1128,  1138,  1129,  1130,
    1131,  1132,  1133,  1134,  1135,  1136,  1149,  1137,  1139,  1160,
    1140,  1141,  1142,  1143,  1144,  1145,  1146,  1147,  1148,  1150,
    1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1161,
    1162,  1163,  1164,  1165,  1166,  1167,  1168,  1169,  1170,  1171,
    1172,  1178,  1173,  1192,  1174,  1186,  1187,  1175,  1188,  1189,
    1190,  1191,  1193,  1195,  1196,  1197,  1198,  1199,  1200,  1202,
    1205,  1206,  1207,  1211,  1212,  1213,  1214,  1215,  1218,  1225,
    1227,  1226,  1228,  1229,  1230,  1231,  1232,  1233,  1239,  1241,
    1240,  1242,  1243,  1244,  1245,  1246,  1247,  1249,  1248,  1250,
    1252,   217,  1253,   167,   883,   374,   524,   742,   881,   130,
     364,   485,   720,   921,  1015,  1019,   683,   890,  1003,   535,
     899,   900,   901,   416,   917,     0,   349,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   556,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   583,     0,     0,     0,     0,   585
};

static const yytype_int16 yycheck[] =
{
     246,   223,     8,    22,   644,   281,   279,   244,   767,   162,
     592,   526,   258,   668,     6,   593,     3,    61,    54,    72,
       5,     6,   255,     5,     6,   178,     3,     8,     3,   155,
     156,    36,    65,   718,   280,   718,   718,    86,   718,   192,
     718,     4,    77,   276,   277,   718,    20,    54,    22,     4,
       5,     6,    81,    27,    75,    76,    54,    61,     3,    65,
      74,   155,   156,    83,   646,     6,    77,    71,   723,   724,
       8,   221,   138,   223,     6,     0,     3,   130,     5,     6,
       3,    33,     7,    35,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    32,    33,    34,    75,   223,    23,    38,
      39,    26,    65,    28,    29,    30,   155,   156,     4,     7,
       6,     9,    10,    11,    12,    13,    14,     4,    16,    15,
      33,   367,   368,   134,    65,    23,    37,    65,    26,   223,
      28,    29,    30,    65,    61,   155,   156,    83,   384,     3,
       3,     5,     6,   902,   158,   100,   101,   102,   155,   156,
      39,   106,    77,   219,   165,    51,   738,   155,   156,    48,
       7,   407,   408,    33,    60,    12,    77,    14,   390,    16,
     403,   417,   418,   419,   420,   421,   422,   221,    65,    77,
     135,   136,   137,   219,   219,    81,    39,     4,     5,     6,
     223,    54,     3,   466,   223,   471,    32,    61,   219,   445,
     446,    69,    70,   781,   127,    41,   129,   103,   104,   155,
     156,    87,   201,   205,     3,   204,   235,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     463,   127,   219,   218,     3,   217,   218,   214,   219,   214,
     925,   168,   925,   925,   169,   925,   220,   925,   144,   145,
     146,    33,   925,    77,    86,   194,   502,   153,   154,   155,
     185,   280,   217,   172,     3,     4,     5,     6,    50,   224,
     225,   169,   197,     3,   496,     5,     6,   932,   159,   155,
     156,   206,   207,   100,   101,   102,     0,   155,   156,   106,
     218,   537,   538,   189,   219,   223,   155,   156,   520,   197,
     546,     8,   548,   222,   168,   527,    32,    33,    34,   207,
     134,   218,    38,    39,     8,     8,   223,     8,   135,   136,
     137,   219,   169,   155,   156,    64,   155,   156,   561,     8,
      20,    61,    22,    72,    73,    71,   232,    27,  1097,  1098,
     562,   165,   189,    82,   134,    84,   193,   155,   156,   196,
     155,   156,   155,   156,   155,   156,   155,   156,    65,     8,
     606,   100,   101,   102,   223,  1005,   372,   106,    67,    68,
       8,    65,   155,   156,    65,   165,    20,     8,    22,    78,
      79,    80,     8,    27,     8,   218,    65,   406,     8,     3,
     223,   220,   907,   159,   160,   161,   135,   136,   137,   138,
     217,   140,   555,   142,    72,   144,    71,   146,   225,   148,
     220,   177,   220,   223,     3,   220,    65,   220,   157,   220,
      83,   220,    85,   669,    87,   671,   221,    65,    14,     3,
       4,     5,     6,  1192,    65,  1194,  1018,   220,   168,    65,
     179,    65,   181,   133,   183,    65,   219,   680,    32,    33,
     346,  1210,     3,  1212,   120,   131,   122,   123,   218,   127,
     126,   129,   130,   223,   132,   698,    50,   218,   194,    10,
       3,   693,   223,   212,   213,   214,   152,   153,   217,    76,
     219,   220,   719,    24,    25,   184,   225,   383,     3,    72,
       3,   764,    39,     3,    77,    42,    43,     6,    72,    73,
      47,    48,     3,   220,   166,   167,   223,   513,    82,   171,
      84,   530,     3,    87,    54,   761,    72,   763,   185,   186,
     173,   174,   768,   190,   746,   223,   100,   101,   102,    83,
       6,    85,   106,    87,   210,   211,   212,   213,   214,   215,
     216,     4,   220,   562,   127,   223,   129,   130,   194,     3,
       4,     5,     6,   194,   179,  1195,   181,   194,   183,    32,
      33,   135,   136,   137,   138,   192,   140,    75,   142,   220,
     144,  1211,   146,  1213,   148,   131,   194,    89,    90,    91,
      92,    93,   194,   157,    96,    97,   220,   220,    21,  1229,
     223,   103,   104,   105,   150,   151,   152,   153,     3,     4,
       5,     6,   158,   220,   880,   179,   223,   181,  1248,   183,
      66,   220,  1252,   125,   223,   223,   224,    39,    72,    73,
      42,    43,    64,    77,   220,    47,    48,   223,    82,    72,
      84,     3,     4,     5,     6,   200,   201,   202,   212,   213,
     214,   220,     3,   217,   223,   219,   100,   101,   102,   173,
     174,   225,   106,   209,   210,   211,   212,   213,   214,   215,
     216,     3,   220,   220,   220,   223,   223,    72,    73,    53,
      54,    55,    56,    61,   693,   220,    61,    82,   223,    84,
     219,   135,   136,   137,   138,     3,   140,    73,   142,     3,
     144,     3,   146,     3,   148,   100,   101,   102,    65,   220,
       4,   106,   223,   157,   220,     3,   712,   223,    89,    90,
      91,    92,    93,   220,   220,    96,    97,   223,     3,     4,
       5,     6,   103,   104,   105,   179,     3,   181,     4,   183,
     135,   136,   137,   138,    61,   140,   219,   142,     4,   144,
     165,   146,   220,   148,   125,   223,    55,    56,    57,    58,
      59,    60,   157,    62,    63,   187,   188,     6,   212,   213,
     214,   220,     3,   217,   223,   219,     6,     3,     4,     5,
       6,   225,   155,   156,   179,  1021,   181,     4,   183,    46,
      47,    48,    49,   220,    44,    45,   223,    72,    73,   220,
     220,   220,   223,   223,   223,     5,     6,    82,  1020,    84,
     220,   220,     3,   223,   223,    44,    45,   212,   213,   214,
      42,    43,   217,     4,   219,   100,   101,   102,     3,    54,
     225,   106,     7,     4,     9,    10,    11,    12,    13,    14,
     194,    16,   214,   215,   216,     3,    72,     3,    23,   922,
     923,    26,    67,    28,    29,    30,    82,    52,    84,    73,
     135,   136,   137,   138,   133,   140,  1102,   142,     3,   144,
     209,   146,    61,   148,   100,   101,   102,   219,    77,    77,
     106,   219,   157,    89,    90,    91,    92,    93,   219,     4,
      96,    97,     4,     4,     4,     3,   219,   103,   104,   105,
       6,   219,    77,   219,   179,   219,   181,   219,   183,   135,
     136,   137,   138,   219,   140,     6,   142,     3,   144,   125,
     146,   219,   148,   219,    32,    33,   219,    35,    46,   221,
      46,   157,    40,    41,    76,     4,    77,   212,   213,   214,
       6,     4,   217,    76,   219,    68,   220,   220,     4,    54,
     225,   219,    77,   179,   219,   181,     7,   183,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     3,     3,   220,
     219,   219,    23,   219,     4,    26,   219,    28,    29,    30,
       4,   219,   219,   219,     4,   226,   212,   213,   214,    76,
       3,   217,   219,   219,   169,     6,   217,     6,    72,   225,
      74,    89,    90,    91,    92,    93,     6,     5,    96,    97,
      42,   219,   219,     3,   160,   103,   104,   105,   219,     6,
       4,   165,   197,    72,   170,   165,    77,   173,   174,   175,
     220,   177,   207,    72,    75,   223,   128,   125,     3,   219,
     133,     3,   220,     4,   219,   191,   192,   223,   194,   195,
      75,   158,   155,   161,   162,   163,   164,   131,   166,   167,
     223,   226,   170,   171,   172,   219,   130,   175,     6,   177,
       6,   226,     3,   181,   182,     3,   150,   151,   152,   153,
     154,     6,   131,     4,   158,     4,   194,   195,   178,   223,
     198,   199,   131,     4,    31,   203,   220,   219,   219,   219,
     219,   150,   151,   152,   153,   131,   155,     6,     4,   158,
     219,   150,   151,   152,   153,   219,     6,   219,   169,   158,
     219,   219,   219,   219,   150,   151,   152,   153,     4,     3,
      77,     6,   158,     4,   185,   209,   210,   211,   212,   213,
     214,   215,   216,   220,   223,   220,   197,   220,     6,     5,
     209,    49,    46,     6,     6,   206,   207,   130,   223,   128,
     209,   210,   211,   212,   213,   214,   215,   216,   219,   132,
     209,   210,   211,   212,   213,   214,   215,   216,   133,   219,
     165,     4,   220,   209,   210,   211,   212,   213,   214,   215,
     216,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   223,   106,
     107,   108,   109,   110,   111,   217,   220,   114,   219,     6,
     117,   118,   119,   120,   121,   122,   123,   124,    89,    90,
      91,    92,    93,     6,   220,    96,    97,    56,    56,     3,
     223,    51,   103,   104,   105,    89,    90,    91,    92,    93,
     220,   223,    96,    97,    91,   223,   223,   223,    91,   103,
     104,   105,    91,   223,   125,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   223,   223,   223,
     223,   125,   223,   223,   223,    91,    91,    91,   223,   223,
     223,   223,   133,   223,   223,   133,   223,   223,     4,   223,
     223,   223,     3,   223,    65,   223,   223,   223,   223,   223,
     223,   223,     6,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,    52,     6,    53,     6,   223,     6,
     223,   223,   223,     6,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,     6,     6,     6,
       6,   223,   219,   223,   223,   223,   220,     6,     6,     6,
     223,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   223,     6,     6,     6,     6,     6,     6,     6,
       6,   223,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   223,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   223,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   223,     6,     4,     4,     4,     4,
       4,   220,   220,     6,   220,   220,   220,    61,   220,   220,
     220,   220,   220,     6,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,     6,   220,     6,   220,   220,
     220,   220,   220,   220,   220,   220,     6,   220,   220,     6,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,     6,
     220,   165,   223,     4,   223,   220,   220,   223,   220,   220,
     220,   220,     4,   220,   220,    48,     6,     6,   223,   223,
     165,   219,     6,   220,     4,   220,    48,     4,     6,   219,
       6,   220,    47,   220,   220,    39,    39,     4,   219,    39,
     220,    39,    39,    39,     3,   219,   219,     3,   220,     3,
     220,   133,   220,    65,   737,   230,   385,   648,   735,    22,
     219,   348,   606,   787,   920,   925,   547,   753,   886,   409,
     765,   765,   765,   265,   784,    -1,   191,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   443,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   468,    -1,    -1,    -1,    -1,   470
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    77,   169,   185,   197,
     206,   207,   219,   228,   229,   230,   232,   244,   245,   247,
     248,   251,   252,   253,   254,   255,   279,   284,   285,   286,
     287,   288,   289,   290,   291,    32,    33,    34,    38,    39,
     194,    36,    32,    33,    34,    38,    39,   194,     3,   277,
      75,   277,   166,   167,   171,     3,   231,   232,   244,   245,
     248,   251,   252,   253,   284,   285,   286,   287,   288,     3,
      32,    33,    35,    40,    41,   161,   162,   163,   164,   166,
     167,   170,   171,   172,   175,   177,   181,   182,   194,   195,
     198,   199,   203,    33,    33,    39,   159,   160,   161,   177,
       3,     3,   277,     3,   280,   281,   172,     7,    12,    14,
      16,   169,   189,   193,   196,    32,    33,    33,    50,   159,
     254,   255,     0,   222,   340,    20,    22,    27,   273,     8,
     256,   258,    71,   339,   339,   339,   339,   339,   341,     3,
     277,    71,   338,   338,   338,   338,   338,     3,   221,    14,
     277,    75,    76,   219,     3,     3,     3,   231,   219,     3,
     277,     6,   205,   173,   174,   173,   174,     3,    76,     6,
       3,   200,   201,   202,   201,   204,   277,   277,     3,    61,
      54,   223,     6,   194,   194,   185,   186,   190,   160,   170,
     173,   174,   175,   177,   191,   192,   194,   195,   194,     4,
     192,    75,   194,   194,   277,   220,   220,   230,    21,   219,
     257,   258,    64,   265,    66,   259,    72,     3,   277,   277,
     277,     3,    61,    61,   219,   246,    73,     3,   277,   277,
     277,     3,     3,     3,   249,   250,    65,   270,     4,   337,
     337,     3,     4,     5,     6,    72,    73,    82,    84,   100,
     101,   102,   106,   135,   136,   137,   138,   140,   142,   144,
     146,   148,   157,   179,   181,   183,   212,   213,   214,   217,
     219,   225,   292,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   305,   306,   307,   308,   309,   311,   312,   313,
     314,   315,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   329,   330,   331,   332,   333,   334,     3,     5,
       6,    61,   168,     3,     5,     6,    61,   168,     3,     5,
       6,    61,   168,   220,    39,    42,    43,    47,    48,     3,
       3,   337,     4,    10,    24,    25,    61,   277,   219,   281,
     337,     4,   165,     6,     3,     6,     4,     3,     4,    54,
       4,   194,     3,     3,   257,   258,   292,    52,    67,   263,
      73,   133,    54,   219,   246,   277,    32,    33,    50,     3,
     243,    37,   255,    61,   209,   223,   270,   295,    77,    77,
     219,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,    72,    73,   296,   219,   219,    87,   295,
     310,     4,     4,     4,     4,     6,   334,   219,   219,   219,
     219,   219,   219,   219,   120,   122,   123,   126,   219,   219,
     296,   296,     5,     6,   218,   315,   327,   328,   255,   295,
     224,   314,   220,   223,    54,   155,   156,    72,    74,   131,
     150,   151,   152,   153,   154,   158,   209,   210,   211,   212,
     213,   214,   215,   216,   221,   218,   223,   218,   223,   218,
     223,   223,   224,   218,   223,   218,   223,     3,     6,    46,
      46,    76,   277,    77,   342,   256,     4,    39,    48,     6,
      76,   187,   188,     4,   220,   220,    81,   266,   260,   261,
     295,   295,    68,   264,     4,   253,     3,   127,   129,   233,
     235,   236,   242,    54,   219,   346,     3,     3,   220,   223,
     219,   293,   277,   295,   250,   219,   219,    64,   220,   292,
     219,    72,   255,   295,   295,   310,    83,    85,    87,   295,
     295,   295,   295,   295,   295,     4,   219,   219,   219,   219,
       4,     4,   226,   220,   220,    76,   294,     3,   295,   295,
      74,   158,   219,    72,   130,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,     3,
     214,   315,     6,   327,     6,   328,   314,     6,     5,    42,
      44,    45,   219,   219,     3,   219,   220,     6,    32,    41,
       4,   165,   165,   292,    75,   267,   223,    69,    70,   262,
     295,   133,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   106,
     107,   108,   109,   110,   111,   114,   117,   118,   119,   120,
     121,   122,   123,   124,   238,   128,   219,   220,   223,   253,
       3,   133,     3,   292,   223,    67,    68,    78,    79,    80,
     184,   335,   336,   335,   292,   220,   255,   220,    54,    86,
      83,    85,   295,   295,   220,   220,   220,   220,   220,   220,
      75,   295,     3,   313,   295,     4,   223,   269,   220,   223,
       5,     6,   337,   219,   296,   255,   292,   130,   155,   226,
     226,     6,     6,   243,   234,   236,     3,   344,   345,     6,
       4,     4,   219,   274,   275,   276,   277,   282,   178,   268,
     261,     4,   219,   219,   219,   219,   219,   219,   219,   219,
     219,    72,   127,   129,   130,   239,   240,   342,   219,   243,
      31,   343,   235,   220,     4,   220,   219,     6,     6,     4,
       3,     6,   220,   223,   220,   220,   220,   238,   295,   295,
      83,    86,   296,   223,   223,   223,   223,     4,    65,   220,
       4,    77,   255,   292,   220,   220,   296,    49,    46,   220,
     220,   223,   209,   220,   223,     6,   253,   223,    55,    57,
      58,    59,    60,    62,    63,   283,     3,    54,   278,   297,
     298,   299,   300,   301,   302,   303,   304,   270,     6,   238,
     237,   238,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   125,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   125,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   125,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   125,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   125,    89,    90,    91,
      92,    93,    96,    97,   103,   104,   105,   125,   130,   128,
     132,   240,   241,   241,   243,   220,   219,   133,   165,   292,
     336,   220,    83,   295,   220,   295,   315,   217,   316,   319,
     324,   329,     4,   223,   269,   295,   220,   219,   220,   220,
       6,     6,   236,     3,     4,     5,     6,   345,    33,    35,
     220,   275,    56,    56,     3,   223,    51,   272,   220,   223,
     220,   220,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,    91,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,    91,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,    91,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,    91,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,    91,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,    91,   223,   314,
     133,   133,   220,   344,     4,     3,   220,   223,   223,   223,
     223,   269,   220,   335,     6,   278,   276,   276,   219,   303,
      52,    53,   271,     6,   238,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   223,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   223,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   223,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   223,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   223,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   223,
       6,     4,     4,   220,   342,     4,     4,     4,     4,   220,
     220,     6,    61,   243,   292,   295,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,     6,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,     6,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,     6,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,     6,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
       6,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,     6,   220,   223,   223,   223,   269,   269,   165,    39,
      42,    43,    47,    48,   295,   220,   220,   220,   220,   220,
     220,   220,     4,     4,     6,   220,   220,    48,     6,     6,
     223,   269,   223,   269,   342,   165,   219,     6,    44,    45,
       6,   220,     4,   220,    48,     4,    42,    43,     6,   269,
     342,   269,   134,   165,   342,   219,   220,     6,    47,   220,
     220,    39,    39,     4,   134,   165,   342,   134,   165,   219,
     220,    39,    39,    39,    39,     3,   219,   219,   220,     3,
       3,   342,   220,   220,   342
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   227,   228,   229,   229,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   233,   233,   234,   234,   235,   235,   236,
     236,   236,   236,   237,   237,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   238,
     238,   238,   238,   238,   238,   238,   238,   238,   238,   239,
     239,   240,   240,   240,   240,   241,   241,   242,   242,   243,
     243,   244,   245,   245,   246,   246,   247,   247,   248,   249,
     249,   250,   251,   251,   251,   251,   251,   252,   252,   252,
     253,   253,   253,   253,   254,   254,   255,   256,   257,   257,
     258,   259,   259,   260,   260,   261,   262,   262,   262,   263,
     263,   264,   264,   265,   265,   266,   266,   267,   267,   268,
     268,   269,   269,   270,   270,   271,   271,   272,   272,   273,
     273,   273,   273,   274,   274,   275,   275,   276,   276,   277,
     277,   278,   278,   278,   278,   279,   279,   280,   280,   281,
     282,   282,   283,   283,   283,   283,   283,   283,   283,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   285,   285,   285,   285,   286,   286,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
     287,   287,   288,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   290,
     290,   290,   291,   291,   292,   292,   293,   293,   294,   294,
     295,   295,   295,   295,   295,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   297,   297,
     297,   298,   298,   298,   298,   299,   299,   299,   299,   300,
     300,   300,   300,   301,   301,   302,   302,   303,   303,   303,
     303,   303,   303,   304,   304,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   305,   305,   305,   306,   306,   307,   308,   308,   309,
     309,   309,   309,   310,   310,   311,   312,   312,   312,   312,
     313,   313,   313,   313,   314,   314,   314,   314,   314,   314,
     314,   314,   314,   314,   314,   314,   314,   315,   315,   315,
     315,   316,   316,   316,   317,   318,   318,   319,   319,   320,
     321,   321,   322,   323,   323,   324,   325,   325,   326,   326,
     327,   328,   329,   329,   330,   331,   331,   332,   333,   333,
     334,   334,   334,   334,   334,   334,   334,   334,   334,   334,
     334,   334,   335,   335,   336,   336,   336,   336,   336,   336,
     337,   338,   338,   339,   339,   340,   340,   341,   341,   342,
     342,   343,   343,   344,   344,   345,   345,   345,   345,   345,
     346,   346
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     4,     4,     8,     6,    10,     8,
       7,     6,     8,     1,     3,     1,     3,     1,     1,     4,
       4,     6,     6,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     4,
       1,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     4,     4,     1,
       2,     2,     1,     1,     2,     2,     0,     5,     4,     1,
       3,     4,     6,     5,     3,     0,     3,     2,     5,     1,
       3,     3,     4,     4,     4,     4,     6,     8,    11,     8,
       1,     1,     3,     3,     3,     3,     2,     4,     3,     3,
       9,     3,     0,     1,     3,     2,     1,     1,     0,     2,
       0,     2,     0,     1,     0,     2,     0,     2,     0,     2,
       0,     3,     0,     2,     0,     2,     0,     3,     0,     1,
       2,     1,     1,     1,     3,     1,     1,     2,     4,     1,
       3,     2,     1,     5,     0,     2,     0,     1,     3,     5,
       4,     6,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     2,     2,     3,     2,     2,     2,     2,     3,     2,
       3,     3,     3,     3,     4,     4,     3,     3,     4,     4,
       5,     6,     7,     9,     4,     5,     7,     9,     2,     2,
       4,     3,     3,     4,     2,     3,     3,     4,     2,     3,
       2,     2,     2,     2,     2,     2,     5,     2,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     6,     6,     5,     3,     4,     4,
       2,     5,     3,     6,     7,     9,    10,    12,    12,    13,
      14,    15,    16,    12,    13,    15,    16,     3,     4,     5,
       6,     3,     3,     4,     3,     3,     4,     4,     6,     5,
       3,     4,     3,     4,     3,     3,     5,     7,     7,     6,
       8,     8,     2,     3,     1,     3,     3,     5,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,    14,    19,
      16,    20,    16,    15,    13,    18,    14,    13,    11,     8,
      10,    13,    15,     5,     7,     4,     6,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     4,     5,     4,     4,
       4,     4,     4,     4,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     2,     2,     2,     3,
       3,     3,     1,     1,     2,     2,     3,     2,     2,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     3,     2,     2,     1,     2,     2,     2,
       1,     2,     0,     3,     0,     1,     0,     2,     0,     4,
       0,     4,     0,     1,     3,     1,     3,     3,     3,     3,
       6,     3
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
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2489 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2497 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 246 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2511 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 226 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 190 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2548 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 185 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2557 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 322 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2568 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2576 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 333 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2585 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 333 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2594 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 293 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2619 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2639 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2649 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2659 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2669 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2679 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 276 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2693 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 276 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 356 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2717 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2725 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2733 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2747 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2756 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2764 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2772 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2780 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2788 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2802 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2811 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2829 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2842 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 346 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2851 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2865 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 303 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 362 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2889 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2898 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2912 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2926 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2934 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2942 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2950 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2958 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2966 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2974 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2982 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2990 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2998 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3006 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 3020 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3028 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3036 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3052 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3060 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 374 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3073 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3081 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3089 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3097 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3105 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3113 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3121 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3129 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3137 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3145 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3153 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3161 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3169 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3177 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3185 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3193 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3201 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 383 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3209 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 387 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3217 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3225 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3233 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3241 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3249 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3257 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3265 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3273 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 219 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3284 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3298 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3312 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 195 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3323 "parser.cpp"
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

#line 3431 "parser.cpp"

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
#line 519 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3646 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 523 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3657 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 529 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3668 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 536 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3674 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3680 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3686 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3692 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3698 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3704 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3710 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3716 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 544 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3722 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 545 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3728 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 546 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3734 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3740 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 548 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3746 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 549 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3752 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 550 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3758 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 551 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3764 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 553 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3770 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 554 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3776 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 555 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3782 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 556 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3788 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3794 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 558 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3800 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3806 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 560 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3812 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 561 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3818 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 562 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3824 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 563 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3830 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 564 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3836 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 571 "parser.y"
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
#line 3858 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 588 "parser.y"
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
#line 3878 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 605 "parser.y"
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
#line 3896 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 621 "parser.y"
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
#line 3929 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 650 "parser.y"
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
#line 3949 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 665 "parser.y"
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
#line 3985 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 697 "parser.y"
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
#line 4007 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 715 "parser.y"
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
#line 4028 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 733 "parser.y"
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
#line 4061 "parser.cpp"
    break;

  case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 761 "parser.y"
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
#line 4096 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 792 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4105 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 796 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4114 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 801 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4123 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 805 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4132 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 811 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4140 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 814 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4148 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 821 "parser.y"
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
#line 4173 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 841 "parser.y"
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
#line 4200 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 863 "parser.y"
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
#line 4229 "parser.cpp"
    break;

  case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 887 "parser.y"
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
#line 4259 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 913 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4268 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 917 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4277 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 923 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4283 "parser.cpp"
    break;

  case 56: /* column_type: TINYINT  */
#line 924 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4289 "parser.cpp"
    break;

  case 57: /* column_type: SMALLINT  */
#line 925 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4295 "parser.cpp"
    break;

  case 58: /* column_type: INTEGER  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4301 "parser.cpp"
    break;

  case 59: /* column_type: INT  */
#line 927 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4307 "parser.cpp"
    break;

  case 60: /* column_type: BIGINT  */
#line 928 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4313 "parser.cpp"
    break;

  case 61: /* column_type: HUGEINT  */
#line 929 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4319 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT  */
#line 930 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4325 "parser.cpp"
    break;

  case 63: /* column_type: REAL  */
#line 931 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4331 "parser.cpp"
    break;

  case 64: /* column_type: DOUBLE  */
#line 932 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4337 "parser.cpp"
    break;

  case 65: /* column_type: FLOAT16  */
#line 933 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4343 "parser.cpp"
    break;

  case 66: /* column_type: BFLOAT16  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4349 "parser.cpp"
    break;

  case 67: /* column_type: DATE  */
#line 935 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4355 "parser.cpp"
    break;

  case 68: /* column_type: TIME  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4361 "parser.cpp"
    break;

  case 69: /* column_type: DATETIME  */
#line 937 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4367 "parser.cpp"
    break;

  case 70: /* column_type: TIMESTAMP  */
#line 938 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4373 "parser.cpp"
    break;

  case 71: /* column_type: UUID  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4379 "parser.cpp"
    break;

  case 72: /* column_type: POINT  */
#line 940 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4385 "parser.cpp"
    break;

  case 73: /* column_type: LINE  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4391 "parser.cpp"
    break;

  case 74: /* column_type: LSEG  */
#line 942 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4397 "parser.cpp"
    break;

  case 75: /* column_type: BOX  */
#line 943 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4403 "parser.cpp"
    break;

  case 76: /* column_type: CIRCLE  */
#line 946 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4409 "parser.cpp"
    break;

  case 77: /* column_type: VARCHAR  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4415 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 949 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4421 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 950 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4427 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL  */
#line 951 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4433 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4439 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4445 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4451 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4457 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4463 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4469 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4475 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4481 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4487 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4493 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4499 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4505 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4511 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4517 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4523 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4529 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4535 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4541 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4547 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4553 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4559 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4565 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4571 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4577 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4583 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4589 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4595 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4601 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4607 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4613 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4619 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4625 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4631 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4637 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4643 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4649 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4655 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4661 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4667 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4673 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4679 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4685 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4691 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4697 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4703 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4709 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4715 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4721 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4727 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4733 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4739 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4745 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4751 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4757 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4763 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4769 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4775 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4781 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4787 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4793 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4799 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4805 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4811 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4817 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4823 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4829 "parser.cpp"
    break;

  case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1020 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4838 "parser.cpp"
    break;

  case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1024 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4848 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraint  */
#line 1047 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4857 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraints column_constraint  */
#line 1051 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4871 "parser.cpp"
    break;

  case 151: /* column_constraint: PRIMARY KEY  */
#line 1061 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4879 "parser.cpp"
    break;

  case 152: /* column_constraint: UNIQUE  */
#line 1064 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4887 "parser.cpp"
    break;

  case 153: /* column_constraint: NULLABLE  */
#line 1067 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4895 "parser.cpp"
    break;

  case 154: /* column_constraint: NOT NULLABLE  */
#line 1070 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4903 "parser.cpp"
    break;

  case 155: /* default_expr: DEFAULT constant_expr  */
#line 1074 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4911 "parser.cpp"
    break;

  case 156: /* default_expr: %empty  */
#line 1077 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4919 "parser.cpp"
    break;

  case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1082 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4929 "parser.cpp"
    break;

  case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1087 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4939 "parser.cpp"
    break;

  case 159: /* identifier_array: IDENTIFIER  */
#line 1094 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4950 "parser.cpp"
    break;

  case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1100 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4961 "parser.cpp"
    break;

  case 161: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1110 "parser.y"
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
#line 4978 "parser.cpp"
    break;

  case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1126 "parser.y"
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
#line 5021 "parser.cpp"
    break;

  case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1164 "parser.y"
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
#line 5041 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1180 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5049 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: %empty  */
#line 1183 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5057 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1190 "parser.y"
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
    else {
        free (yyvsp[-1].str_value);
        yyerror(&yyloc, scanner, result, "invalid explain option");
        YYERROR;
    }
    free((yyvsp[-1].str_value));
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5080 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1207 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5090 "parser.cpp"
    break;

  case 168: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1216 "parser.y"
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

  case 169: /* update_expr_array: update_expr  */
#line 1229 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5116 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1233 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5125 "parser.cpp"
    break;

  case 171: /* update_expr: IDENTIFIER '=' expr  */
#line 1238 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5137 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1251 "parser.y"
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

  case 173: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1264 "parser.y"
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

  case 174: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1279 "parser.y"
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

  case 175: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1294 "parser.y"
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

  case 176: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1309 "parser.y"
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

  case 177: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1332 "parser.y"
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

  case 178: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1385 "parser.y"
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

  case 179: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1440 "parser.y"
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

  case 180: /* select_statement: select_without_paren  */
#line 1491 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5408 "parser.cpp"
    break;

  case 181: /* select_statement: select_with_paren  */
#line 1494 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5416 "parser.cpp"
    break;

  case 182: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1497 "parser.y"
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

  case 183: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1506 "parser.y"
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

  case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1516 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5452 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1519 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5460 "parser.cpp"
    break;

  case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1523 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5469 "parser.cpp"
    break;

  case 187: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1528 "parser.y"
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

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1555 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5508 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1558 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5516 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1563 "parser.y"
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
#line 5537 "parser.cpp"
    break;

  case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1580 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5545 "parser.cpp"
    break;

  case 192: /* order_by_clause: %empty  */
#line 1583 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5553 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr  */
#line 1587 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5562 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1591 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5571 "parser.cpp"
    break;

  case 195: /* order_by_expr: expr order_by_type  */
#line 1596 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5581 "parser.cpp"
    break;

  case 196: /* order_by_type: ASC  */
#line 1602 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5589 "parser.cpp"
    break;

  case 197: /* order_by_type: DESC  */
#line 1605 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5597 "parser.cpp"
    break;

  case 198: /* order_by_type: %empty  */
#line 1608 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5605 "parser.cpp"
    break;

  case 199: /* limit_expr: LIMIT expr  */
#line 1612 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5613 "parser.cpp"
    break;

  case 200: /* limit_expr: %empty  */
#line 1616 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5619 "parser.cpp"
    break;

  case 201: /* offset_expr: OFFSET expr  */
#line 1618 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5627 "parser.cpp"
    break;

  case 202: /* offset_expr: %empty  */
#line 1622 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5633 "parser.cpp"
    break;

  case 203: /* distinct: DISTINCT  */
#line 1624 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5641 "parser.cpp"
    break;

  case 204: /* distinct: %empty  */
#line 1627 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5649 "parser.cpp"
    break;

  case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5657 "parser.cpp"
    break;

  case 206: /* highlight_clause: %empty  */
#line 1634 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5665 "parser.cpp"
    break;

  case 207: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5673 "parser.cpp"
    break;

  case 208: /* from_clause: %empty  */
#line 1641 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5681 "parser.cpp"
    break;

  case 209: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5691 "parser.cpp"
    break;

  case 210: /* search_clause: %empty  */
#line 1650 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5699 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5707 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5715 "parser.cpp"
    break;

  case 213: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5723 "parser.cpp"
    break;

  case 214: /* where_clause: %empty  */
#line 1664 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5731 "parser.cpp"
    break;

  case 215: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5739 "parser.cpp"
    break;

  case 216: /* having_clause: %empty  */
#line 1671 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5747 "parser.cpp"
    break;

  case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5755 "parser.cpp"
    break;

  case 218: /* group_by_clause: %empty  */
#line 1678 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5763 "parser.cpp"
    break;

  case 219: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5771 "parser.cpp"
    break;

  case 220: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5779 "parser.cpp"
    break;

  case 221: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5787 "parser.cpp"
    break;

  case 222: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5795 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5803 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference ',' table_reference_unit  */
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
#line 5821 "parser.cpp"
    break;

  case 227: /* table_reference_name: table_name table_alias  */
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
#line 5839 "parser.cpp"
    break;

  case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5850 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5860 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1747 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5872 "parser.cpp"
    break;

  case 231: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5882 "parser.cpp"
    break;

  case 232: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5892 "parser.cpp"
    break;

  case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5903 "parser.cpp"
    break;

  case 234: /* table_alias: %empty  */
#line 1772 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5911 "parser.cpp"
    break;

  case 235: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5919 "parser.cpp"
    break;

  case 236: /* with_clause: %empty  */
#line 1782 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5927 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5936 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5945 "parser.cpp"
    break;

  case 239: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1794 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5957 "parser.cpp"
    break;

  case 240: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1806 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5969 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1813 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5982 "parser.cpp"
    break;

  case 242: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5990 "parser.cpp"
    break;

  case 243: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5998 "parser.cpp"
    break;

  case 244: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6006 "parser.cpp"
    break;

  case 245: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6014 "parser.cpp"
    break;

  case 246: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6022 "parser.cpp"
    break;

  case 247: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6030 "parser.cpp"
    break;

  case 248: /* join_type: %empty  */
#line 1845 "parser.y"
                {
}
#line 6037 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW DATABASES  */
#line 1851 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6046 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLES  */
#line 1855 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6055 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TASKS  */
#line 1859 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6064 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIGS  */
#line 1863 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6073 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1867 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6085 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW PROFILES  */
#line 1874 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6094 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW BUFFER  */
#line 1878 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6103 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW MEMINDEX  */
#line 1882 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6112 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERIES  */
#line 1886 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6121 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1890 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6131 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1895 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6140 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1899 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6150 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1904 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6159 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1908 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6168 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1912 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6177 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1916 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6188 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1922 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1928 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6210 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name  */
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
#line 6226 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name COLUMNS  */
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
#line 6242 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name SEGMENTS  */
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
#line 6258 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
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
#line 6275 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
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
#line 6292 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
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
#line 6310 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
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
#line 6329 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name INDEXES  */
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
#line 6345 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
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
#line 6364 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
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
#line 6385 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
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
#line 6407 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW LOGS  */
#line 2076 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6416 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOG  */
#line 2080 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6425 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG TO file_path  */
#line 2084 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6436 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2090 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6445 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2094 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6454 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2098 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6465 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW MEMORY  */
#line 2104 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6474 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2108 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6483 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2112 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6492 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2116 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6503 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW SNAPSHOTS  */
#line 2122 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6512 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2126 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6523 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW CACHES  */
#line 2132 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6532 "parser.cpp"
    break;

  case 291: /* flush_statement: FLUSH DATA  */
#line 2140 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6541 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH LOG  */
#line 2144 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6550 "parser.cpp"
    break;

  case 293: /* flush_statement: FLUSH BUFFER  */
#line 2148 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6559 "parser.cpp"
    break;

  case 294: /* flush_statement: FLUSH CATALOG  */
#line 2152 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6568 "parser.cpp"
    break;

  case 295: /* optimize_statement: OPTIMIZE table_name  */
#line 2160 "parser.y"
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
#line 6583 "parser.cpp"
    break;

  case 296: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2171 "parser.y"
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
#line 6607 "parser.cpp"
    break;

  case 297: /* command_statement: USE IDENTIFIER  */
#line 2194 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6618 "parser.cpp"
    break;

  case 298: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2200 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6628 "parser.cpp"
    break;

  case 299: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2205 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6639 "parser.cpp"
    break;

  case 300: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2211 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6650 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2217 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6663 "parser.cpp"
    break;

  case 302: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2225 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6674 "parser.cpp"
    break;

  case 303: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2231 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6685 "parser.cpp"
    break;

  case 304: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2237 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6696 "parser.cpp"
    break;

  case 305: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2243 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6707 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2249 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6720 "parser.cpp"
    break;

  case 307: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2257 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6731 "parser.cpp"
    break;

  case 308: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2263 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6742 "parser.cpp"
    break;

  case 309: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2269 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6753 "parser.cpp"
    break;

  case 310: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2275 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6764 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2281 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6777 "parser.cpp"
    break;

  case 312: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2289 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6788 "parser.cpp"
    break;

  case 313: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2295 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6799 "parser.cpp"
    break;

  case 314: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2301 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6812 "parser.cpp"
    break;

  case 315: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2309 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6825 "parser.cpp"
    break;

  case 316: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2317 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6836 "parser.cpp"
    break;

  case 317: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2323 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6847 "parser.cpp"
    break;

  case 318: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2329 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6858 "parser.cpp"
    break;

  case 319: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2335 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6869 "parser.cpp"
    break;

  case 320: /* command_statement: CLEAN DATA  */
#line 2341 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 6878 "parser.cpp"
    break;

  case 321: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2345 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::DumpIndexCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_, (yyvsp[-2].str_value));
    free((yyvsp[-2].str_value));
    if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6896 "parser.cpp"
    break;

  case 322: /* compact_statement: COMPACT TABLE table_name  */
#line 2359 "parser.y"
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
#line 6913 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2372 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6924 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2378 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6936 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2385 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6949 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
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
#line 6963 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
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
#line 6978 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
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
#line 6993 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
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
#line 7009 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
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
#line 7025 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
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
#line 7042 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
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
#line 7059 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
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
#line 7074 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
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
#line 7090 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
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
#line 7107 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
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
#line 7125 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW LOGS  */
#line 2514 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7134 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2518 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7144 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2523 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7154 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2528 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7165 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2534 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7174 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2538 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7183 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2542 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7194 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2548 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7203 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2552 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7212 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2556 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7223 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2562 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7234 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2568 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7247 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2576 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7258 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW NODES  */
#line 2582 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7267 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2586 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7278 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE  */
#line 2592 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7287 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2596 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7298 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SET ADMIN  */
#line 2602 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7308 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET STANDALONE  */
#line 2607 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7318 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2612 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7330 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
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
#line 7344 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
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
#line 7358 "parser.cpp"
    break;

  case 359: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
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
#line 7372 "parser.cpp"
    break;

  case 360: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
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
#line 7389 "parser.cpp"
    break;

  case 361: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
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
#line 7405 "parser.cpp"
    break;

  case 362: /* check_statement: CHECK SYSTEM  */
#line 2671 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7414 "parser.cpp"
    break;

  case 363: /* check_statement: CHECK TABLE table_name  */
#line 2675 "parser.y"
                         {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kTable;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.check_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.check_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 7430 "parser.cpp"
    break;

  case 364: /* expr_array: expr_alias  */
#line 2691 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7439 "parser.cpp"
    break;

  case 365: /* expr_array: expr_array ',' expr_alias  */
#line 2695 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7448 "parser.cpp"
    break;

  case 366: /* insert_row_list: '(' expr_array ')'  */
#line 2700 "parser.y"
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
#line 7463 "parser.cpp"
    break;

  case 367: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2710 "parser.y"
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
#line 7478 "parser.cpp"
    break;

  case 368: /* expr_alias: expr AS IDENTIFIER  */
#line 2732 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7489 "parser.cpp"
    break;

  case 369: /* expr_alias: expr  */
#line 2738 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7497 "parser.cpp"
    break;

  case 375: /* operand: '(' expr ')'  */
#line 2748 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7505 "parser.cpp"
    break;

  case 376: /* operand: '(' select_without_paren ')'  */
#line 2751 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7516 "parser.cpp"
    break;

  case 377: /* operand: constant_expr  */
#line 2757 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7524 "parser.cpp"
    break;

  case 388: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2773 "parser.y"
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
#line 7544 "parser.cpp"
    break;

  case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2789 "parser.y"
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
#line 7565 "parser.cpp"
    break;

  case 390: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2806 "parser.y"
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
#line 7586 "parser.cpp"
    break;

  case 391: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2824 "parser.y"
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

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-12].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-14].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-12].str_value), const_expr);
        if (!check) {
            goto Error1;
        }
        delete (yyvsp[-14].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-14].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-12].str_value);
    }
    free((yyvsp[-12].str_value));
    free((yyvsp[-10].str_value));

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
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-14].expr_t))) {
        delete (yyvsp[-14].expr_t);
    } else {
        delete (yyvsp[-14].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 7647 "parser.cpp"
    break;

  case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2881 "parser.y"
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

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-8].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-10].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-8].str_value), const_expr);
        if (!check) {
            goto Error2;
        }
        delete (yyvsp[-10].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-10].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-8].str_value);
    }
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));

    match_vector_expr->topn_ = (yyvsp[-4].long_value);
    match_vector_expr->filter_expr_.reset((yyvsp[-3].expr_t));
    match_vector_expr->ignore_index_ = true;
    goto Return2;
Error2:
    free((yyvsp[-8].str_value));
    free((yyvsp[-6].str_value));
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-10].expr_t))) {
        delete (yyvsp[-10].expr_t);
    } else {
        delete (yyvsp[-10].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 7701 "parser.cpp"
    break;

  case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2931 "parser.y"
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

    // vector data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-7].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-9].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-7].str_value), const_expr);
        if (!check) {
            goto Error3;
        }
        delete (yyvsp[-9].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-9].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-7].str_value);
    }
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));

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
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-9].expr_t))) {
        delete (yyvsp[-9].expr_t);
    } else {
        delete (yyvsp[-9].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return3:
    ;
}
#line 7759 "parser.cpp"
    break;

  case 394: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2985 "parser.y"
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

    // vector search data type and query embedding expression
    ParserHelper::ToLower((yyvsp[-5].str_value));
    // For backward compatibility, try to handle as array_expr first, then as general expr
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-7].expr_t))) {
        check = match_vector_expr->InitEmbedding((yyvsp[-5].str_value), const_expr);
        if (!check) {
            goto Error4;
        }
        delete (yyvsp[-7].expr_t);
    } else {
        // Store the expression for later evaluation during execution
        match_vector_expr->query_embedding_expr_.reset((yyvsp[-7].expr_t));
        match_vector_expr->embedding_data_type_str_ = (yyvsp[-5].str_value);
    }
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));

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
    if (auto* const_expr = dynamic_cast<infinity::ConstantExpr*>((yyvsp[-7].expr_t))) {
        delete (yyvsp[-7].expr_t);
    } else {
        delete (yyvsp[-7].expr_t);
    }
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return4:
    ;
}
#line 7818 "parser.cpp"
    break;

  case 395: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3043 "parser.y"
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
#line 7846 "parser.cpp"
    break;

  case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3067 "parser.y"
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
#line 7873 "parser.cpp"
    break;

  case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3090 "parser.y"
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
#line 7898 "parser.cpp"
    break;

  case 398: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3111 "parser.y"
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
#line 7923 "parser.cpp"
    break;

  case 399: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3132 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7937 "parser.cpp"
    break;

  case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3141 "parser.y"
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
#line 7953 "parser.cpp"
    break;

  case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3152 "parser.y"
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
#line 7969 "parser.cpp"
    break;

  case 402: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3163 "parser.y"
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
#line 7987 "parser.cpp"
    break;

  case 403: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3177 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7999 "parser.cpp"
    break;

  case 404: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3184 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8013 "parser.cpp"
    break;

  case 405: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3194 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8024 "parser.cpp"
    break;

  case 406: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3200 "parser.y"
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
#line 8040 "parser.cpp"
    break;

  case 407: /* sub_search: match_vector_expr  */
#line 3212 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8048 "parser.cpp"
    break;

  case 408: /* sub_search: match_text_expr  */
#line 3215 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8056 "parser.cpp"
    break;

  case 409: /* sub_search: match_tensor_expr  */
#line 3218 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8064 "parser.cpp"
    break;

  case 410: /* sub_search: match_sparse_expr  */
#line 3221 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8072 "parser.cpp"
    break;

  case 411: /* sub_search: query_expr  */
#line 3224 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8080 "parser.cpp"
    break;

  case 412: /* sub_search: fusion_expr  */
#line 3227 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8088 "parser.cpp"
    break;

  case 413: /* sub_search_array: sub_search  */
#line 3231 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8097 "parser.cpp"
    break;

  case 414: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3235 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8106 "parser.cpp"
    break;

  case 415: /* function_expr: IDENTIFIER '(' ')'  */
#line 3240 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8119 "parser.cpp"
    break;

  case 416: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3248 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8132 "parser.cpp"
    break;

  case 417: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3256 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8146 "parser.cpp"
    break;

  case 418: /* function_expr: YEAR '(' expr ')'  */
#line 3265 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8158 "parser.cpp"
    break;

  case 419: /* function_expr: MONTH '(' expr ')'  */
#line 3272 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8170 "parser.cpp"
    break;

  case 420: /* function_expr: DAY '(' expr ')'  */
#line 3279 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8182 "parser.cpp"
    break;

  case 421: /* function_expr: HOUR '(' expr ')'  */
#line 3286 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8194 "parser.cpp"
    break;

  case 422: /* function_expr: MINUTE '(' expr ')'  */
#line 3293 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8206 "parser.cpp"
    break;

  case 423: /* function_expr: SECOND '(' expr ')'  */
#line 3300 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8218 "parser.cpp"
    break;

  case 424: /* function_expr: operand IS NOT NULLABLE  */
#line 3307 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8230 "parser.cpp"
    break;

  case 425: /* function_expr: operand IS NULLABLE  */
#line 3314 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8242 "parser.cpp"
    break;

  case 426: /* function_expr: NOT operand  */
#line 3321 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8254 "parser.cpp"
    break;

  case 427: /* function_expr: '-' operand  */
#line 3328 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8266 "parser.cpp"
    break;

  case 428: /* function_expr: '+' operand  */
#line 3335 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8278 "parser.cpp"
    break;

  case 429: /* function_expr: operand '-' operand  */
#line 3342 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8291 "parser.cpp"
    break;

  case 430: /* function_expr: operand '+' operand  */
#line 3350 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8304 "parser.cpp"
    break;

  case 431: /* function_expr: operand '*' operand  */
#line 3358 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8317 "parser.cpp"
    break;

  case 432: /* function_expr: operand '/' operand  */
#line 3366 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8330 "parser.cpp"
    break;

  case 433: /* function_expr: operand '%' operand  */
#line 3374 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8343 "parser.cpp"
    break;

  case 434: /* function_expr: operand '=' operand  */
#line 3382 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8356 "parser.cpp"
    break;

  case 435: /* function_expr: operand EQUAL operand  */
#line 3390 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8369 "parser.cpp"
    break;

  case 436: /* function_expr: operand NOT_EQ operand  */
#line 3398 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8382 "parser.cpp"
    break;

  case 437: /* function_expr: operand '<' operand  */
#line 3406 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8395 "parser.cpp"
    break;

  case 438: /* function_expr: operand '>' operand  */
#line 3414 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8408 "parser.cpp"
    break;

  case 439: /* function_expr: operand LESS_EQ operand  */
#line 3422 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8421 "parser.cpp"
    break;

  case 440: /* function_expr: operand GREATER_EQ operand  */
#line 3430 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8434 "parser.cpp"
    break;

  case 441: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3438 "parser.y"
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
#line 8469 "parser.cpp"
    break;

  case 442: /* function_expr: operand LIKE operand  */
#line 3468 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8482 "parser.cpp"
    break;

  case 443: /* function_expr: operand NOT LIKE operand  */
#line 3476 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8495 "parser.cpp"
    break;

  case 444: /* conjunction_expr: expr AND expr  */
#line 3485 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8508 "parser.cpp"
    break;

  case 445: /* conjunction_expr: expr OR expr  */
#line 3493 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8521 "parser.cpp"
    break;

  case 446: /* between_expr: operand BETWEEN operand AND operand  */
#line 3502 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8533 "parser.cpp"
    break;

  case 447: /* in_expr: operand IN '(' expr_array ')'  */
#line 3510 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8544 "parser.cpp"
    break;

  case 448: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3516 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8555 "parser.cpp"
    break;

  case 449: /* case_expr: CASE expr case_check_array END  */
#line 3523 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8566 "parser.cpp"
    break;

  case 450: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3529 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8578 "parser.cpp"
    break;

  case 451: /* case_expr: CASE case_check_array END  */
#line 3536 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8588 "parser.cpp"
    break;

  case 452: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3541 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8599 "parser.cpp"
    break;

  case 453: /* case_check_array: WHEN expr THEN expr  */
#line 3548 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8611 "parser.cpp"
    break;

  case 454: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3555 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8623 "parser.cpp"
    break;

  case 455: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3563 "parser.y"
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
#line 8640 "parser.cpp"
    break;

  case 456: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3576 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8651 "parser.cpp"
    break;

  case 457: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3582 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8662 "parser.cpp"
    break;

  case 458: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3588 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8674 "parser.cpp"
    break;

  case 459: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3595 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8686 "parser.cpp"
    break;

  case 460: /* column_expr: IDENTIFIER  */
#line 3603 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8698 "parser.cpp"
    break;

  case 461: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3610 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8710 "parser.cpp"
    break;

  case 462: /* column_expr: '*'  */
#line 3617 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8720 "parser.cpp"
    break;

  case 463: /* column_expr: column_expr '.' '*'  */
#line 3622 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8734 "parser.cpp"
    break;

  case 464: /* constant_expr: STRING  */
#line 3632 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8744 "parser.cpp"
    break;

  case 465: /* constant_expr: TRUE  */
#line 3637 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8754 "parser.cpp"
    break;

  case 466: /* constant_expr: FALSE  */
#line 3642 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8764 "parser.cpp"
    break;

  case 467: /* constant_expr: DOUBLE_VALUE  */
#line 3647 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8774 "parser.cpp"
    break;

  case 468: /* constant_expr: LONG_VALUE  */
#line 3652 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8784 "parser.cpp"
    break;

  case 469: /* constant_expr: DATE STRING  */
#line 3657 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8794 "parser.cpp"
    break;

  case 470: /* constant_expr: TIME STRING  */
#line 3662 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8804 "parser.cpp"
    break;

  case 471: /* constant_expr: DATETIME STRING  */
#line 3667 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8814 "parser.cpp"
    break;

  case 472: /* constant_expr: TIMESTAMP STRING  */
#line 3672 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8824 "parser.cpp"
    break;

  case 473: /* constant_expr: INTERVAL interval_expr  */
#line 3677 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8832 "parser.cpp"
    break;

  case 474: /* constant_expr: interval_expr  */
#line 3680 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8840 "parser.cpp"
    break;

  case 475: /* constant_expr: common_array_expr  */
#line 3683 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8848 "parser.cpp"
    break;

  case 476: /* constant_expr: curly_brackets_expr  */
#line 3686 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8856 "parser.cpp"
    break;

  case 477: /* common_array_expr: array_expr  */
#line 3690 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8864 "parser.cpp"
    break;

  case 478: /* common_array_expr: subarray_array_expr  */
#line 3693 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8872 "parser.cpp"
    break;

  case 479: /* common_array_expr: sparse_array_expr  */
#line 3696 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8880 "parser.cpp"
    break;

  case 480: /* common_array_expr: empty_array_expr  */
#line 3699 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8888 "parser.cpp"
    break;

  case 481: /* common_sparse_array_expr: sparse_array_expr  */
#line 3703 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8896 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: array_expr  */
#line 3706 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8904 "parser.cpp"
    break;

  case 483: /* common_sparse_array_expr: empty_array_expr  */
#line 3709 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8912 "parser.cpp"
    break;

  case 484: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3713 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8920 "parser.cpp"
    break;

  case 485: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3717 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8930 "parser.cpp"
    break;

  case 486: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3722 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8939 "parser.cpp"
    break;

  case 487: /* sparse_array_expr: long_sparse_array_expr  */
#line 3727 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8947 "parser.cpp"
    break;

  case 488: /* sparse_array_expr: double_sparse_array_expr  */
#line 3730 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8955 "parser.cpp"
    break;

  case 489: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3734 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8963 "parser.cpp"
    break;

  case 490: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3738 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8975 "parser.cpp"
    break;

  case 491: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3745 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8986 "parser.cpp"
    break;

  case 492: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3752 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8994 "parser.cpp"
    break;

  case 493: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3756 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9006 "parser.cpp"
    break;

  case 494: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3763 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9017 "parser.cpp"
    break;

  case 495: /* empty_array_expr: '[' ']'  */
#line 3770 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9025 "parser.cpp"
    break;

  case 496: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3774 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9033 "parser.cpp"
    break;

  case 497: /* curly_brackets_expr: '{' '}'  */
#line 3777 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9041 "parser.cpp"
    break;

  case 498: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3781 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9050 "parser.cpp"
    break;

  case 499: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3785 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9059 "parser.cpp"
    break;

  case 500: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3790 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9067 "parser.cpp"
    break;

  case 501: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3794 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9075 "parser.cpp"
    break;

  case 502: /* array_expr: long_array_expr  */
#line 3798 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9083 "parser.cpp"
    break;

  case 503: /* array_expr: double_array_expr  */
#line 3801 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9091 "parser.cpp"
    break;

  case 504: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3805 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9099 "parser.cpp"
    break;

  case 505: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3809 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9109 "parser.cpp"
    break;

  case 506: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3814 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9118 "parser.cpp"
    break;

  case 507: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3819 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9126 "parser.cpp"
    break;

  case 508: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3823 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9136 "parser.cpp"
    break;

  case 509: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3828 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9145 "parser.cpp"
    break;

  case 510: /* interval_expr: LONG_VALUE SECONDS  */
#line 3833 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9156 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE SECOND  */
#line 3839 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9167 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE MINUTES  */
#line 3845 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9178 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE MINUTE  */
#line 3851 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9189 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE HOURS  */
#line 3857 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9200 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE HOUR  */
#line 3863 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9211 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE DAYS  */
#line 3869 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9222 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE DAY  */
#line 3875 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9233 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE MONTHS  */
#line 3881 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9244 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE MONTH  */
#line 3887 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9255 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE YEARS  */
#line 3893 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9266 "parser.cpp"
    break;

  case 521: /* interval_expr: LONG_VALUE YEAR  */
#line 3899 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9277 "parser.cpp"
    break;

  case 522: /* copy_option_list: copy_option  */
#line 3910 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9286 "parser.cpp"
    break;

  case 523: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3914 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9295 "parser.cpp"
    break;

  case 524: /* copy_option: FORMAT IDENTIFIER  */
#line 3919 "parser.y"
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
#line 9331 "parser.cpp"
    break;

  case 525: /* copy_option: DELIMITER STRING  */
#line 3950 "parser.y"
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
#line 9346 "parser.cpp"
    break;

  case 526: /* copy_option: HEADER  */
#line 3960 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9356 "parser.cpp"
    break;

  case 527: /* copy_option: OFFSET LONG_VALUE  */
#line 3965 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9366 "parser.cpp"
    break;

  case 528: /* copy_option: LIMIT LONG_VALUE  */
#line 3970 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9376 "parser.cpp"
    break;

  case 529: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3975 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9386 "parser.cpp"
    break;

  case 530: /* file_path: STRING  */
#line 3981 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9394 "parser.cpp"
    break;

  case 531: /* if_exists: IF EXISTS  */
#line 3985 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9400 "parser.cpp"
    break;

  case 532: /* if_exists: %empty  */
#line 3986 "parser.y"
  { (yyval.bool_value) = false; }
#line 9406 "parser.cpp"
    break;

  case 533: /* if_not_exists: IF NOT EXISTS  */
#line 3988 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9412 "parser.cpp"
    break;

  case 534: /* if_not_exists: %empty  */
#line 3989 "parser.y"
  { (yyval.bool_value) = false; }
#line 9418 "parser.cpp"
    break;

  case 537: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4004 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9431 "parser.cpp"
    break;

  case 538: /* if_not_exists_info: %empty  */
#line 4012 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9439 "parser.cpp"
    break;

  case 539: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4016 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9447 "parser.cpp"
    break;

  case 540: /* with_index_param_list: %empty  */
#line 4019 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9455 "parser.cpp"
    break;

  case 541: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4023 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9463 "parser.cpp"
    break;

  case 542: /* optional_table_properties_list: %empty  */
#line 4026 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9471 "parser.cpp"
    break;

  case 543: /* index_param_list: index_param  */
#line 4030 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9480 "parser.cpp"
    break;

  case 544: /* index_param_list: index_param_list ',' index_param  */
#line 4034 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9489 "parser.cpp"
    break;

  case 545: /* index_param: IDENTIFIER  */
#line 4039 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9500 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4045 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9515 "parser.cpp"
    break;

  case 547: /* index_param: IDENTIFIER '=' STRING  */
#line 4055 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9530 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4065 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9543 "parser.cpp"
    break;

  case 549: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4073 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9556 "parser.cpp"
    break;

  case 550: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4084 "parser.y"
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
#line 9592 "parser.cpp"
    break;

  case 551: /* index_info: '(' IDENTIFIER ')'  */
#line 4115 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9603 "parser.cpp"
    break;


#line 9607 "parser.cpp"

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

#line 4122 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

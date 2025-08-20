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
  YYSYMBOL_CACHE = 199,                    /* CACHE  */
  YYSYMBOL_PERSISTENCE = 200,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 201,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 202,                  /* OBJECTS  */
  YYSYMBOL_FILES = 203,                    /* FILES  */
  YYSYMBOL_MEMORY = 204,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 205,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 206,                  /* HISTORY  */
  YYSYMBOL_CHECK = 207,                    /* CHECK  */
  YYSYMBOL_CLEAN = 208,                    /* CLEAN  */
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
  YYSYMBOL_check_statement = 292,          /* check_statement  */
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
#define YYFINAL  133
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1568

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
    1899,  1904,  1908,  1912,  1916,  1922,  1928,  1935,  1946,  1957,
    1968,  1980,  1992,  2005,  2019,  2030,  2045,  2062,  2080,  2084,
    2088,  2094,  2098,  2102,  2108,  2112,  2116,  2120,  2127,  2131,
    2138,  2142,  2150,  2154,  2158,  2162,  2170,  2181,  2205,  2211,
    2216,  2222,  2228,  2236,  2242,  2248,  2254,  2260,  2268,  2274,
    2280,  2286,  2292,  2300,  2306,  2312,  2320,  2328,  2334,  2340,
    2346,  2352,  2356,  2368,  2381,  2387,  2394,  2402,  2411,  2421,
    2431,  2442,  2453,  2465,  2477,  2487,  2498,  2510,  2523,  2527,
    2532,  2537,  2543,  2547,  2551,  2558,  2562,  2566,  2573,  2579,
    2587,  2593,  2597,  2603,  2607,  2613,  2618,  2623,  2630,  2639,
    2649,  2659,  2671,  2683,  2687,  2703,  2707,  2712,  2722,  2744,
    2750,  2754,  2755,  2756,  2757,  2758,  2760,  2763,  2769,  2772,
    2773,  2774,  2775,  2776,  2777,  2778,  2779,  2780,  2781,  2785,
    2801,  2819,  2837,  2895,  2945,  2999,  3057,  3082,  3105,  3126,
    3147,  3156,  3167,  3178,  3192,  3199,  3209,  3215,  3227,  3230,
    3233,  3236,  3239,  3242,  3246,  3250,  3255,  3263,  3271,  3280,
    3287,  3294,  3301,  3308,  3315,  3322,  3329,  3336,  3343,  3350,
    3357,  3365,  3373,  3381,  3389,  3397,  3405,  3413,  3421,  3429,
    3437,  3445,  3453,  3483,  3491,  3500,  3508,  3517,  3525,  3531,
    3538,  3544,  3551,  3556,  3563,  3570,  3578,  3591,  3597,  3603,
    3610,  3618,  3625,  3632,  3637,  3647,  3652,  3657,  3662,  3667,
    3672,  3677,  3682,  3687,  3692,  3695,  3698,  3701,  3705,  3708,
    3711,  3714,  3718,  3721,  3724,  3728,  3732,  3737,  3742,  3745,
    3749,  3753,  3760,  3767,  3771,  3778,  3785,  3789,  3792,  3796,
    3800,  3805,  3809,  3813,  3816,  3820,  3824,  3829,  3834,  3838,
    3843,  3848,  3854,  3860,  3866,  3872,  3878,  3884,  3890,  3896,
    3902,  3908,  3914,  3925,  3929,  3934,  3966,  3976,  3981,  3986,
    3991,  3997,  4001,  4002,  4004,  4005,  4007,  4008,  4020,  4028,
    4032,  4035,  4039,  4042,  4046,  4050,  4055,  4061,  4071,  4081,
    4089,  4100,  4132
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
  "RECOVER", "RESTORE", "CACHES", "CACHE", "PERSISTENCE", "OBJECT",
  "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "HISTORY", "CHECK", "CLEAN",
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

#define YYPACT_NINF (-761)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-540)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     879,   -15,     5,    81,    58,     1,    58,   309,    70,   948,
      62,   134,    77,   279,   128,   177,   187,   125,   118,   169,
     161,    50,   -39,   255,    87,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,   428,  -761,  -761,   315,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,   277,   277,   277,   277,    94,
     355,    58,   297,   297,   297,   297,   297,   372,   171,   389,
      58,   -28,   427,   446,   502,   920,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,   428,  -761,  -761,  -761,  -761,  -761,   296,
     508,    58,  -761,  -761,  -761,  -761,  -761,    48,  -761,   168,
     269,  -761,   517,  -761,  -761,   464,   491,  -761,   539,  -761,
    -761,  -761,   287,   174,    58,    58,   551,  -761,  -761,  -761,
    -761,  -761,   -31,  -761,   520,   348,  -761,   573,   398,   417,
      86,  1132,   419,   586,   426,   553,   432,   436,    58,  -761,
    -761,   441,   443,  -761,   810,  -761,   632,  -761,  -761,    27,
     602,  -761,   606,   596,   671,    58,    58,    58,   683,   642,
     653,   489,   658,   729,    58,    58,    58,   737,  -761,   741,
     743,   689,   756,   756,   651,    98,   102,   121,  -761,   540,
    -761,   471,  -761,  -761,  -761,   759,  -761,   764,  -761,   756,
    -761,  -761,   765,  -761,  -761,  -761,  -761,   421,  -761,   710,
      58,   557,   187,   756,  -761,   769,  -761,   613,  -761,   773,
    -761,  -761,   777,  -761,   784,  -761,   788,   791,  -761,   792,
     746,   797,   608,   800,   801,  -761,  -761,  -761,  -761,  -761,
      27,  -761,  -761,  -761,   651,   753,   739,   740,   679,   -34,
    -761,   489,  -761,    58,   293,   811,    25,  -761,  -761,  -761,
    -761,  -761,   754,  -761,   618,   -37,  -761,   651,  -761,  -761,
     752,   758,   598,  -761,  -761,   824,   778,   611,   617,   523,
     812,   840,   841,   843,  -761,  -761,   846,   629,   633,   635,
     637,   638,   639,   641,   344,   643,   647,   280,   280,  -761,
      26,   628,    83,   145,  -761,   -11,  1003,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
     646,  -761,  -761,  -761,   233,  -761,  -761,   260,  -761,   262,
    -761,  -761,   253,  -761,  -761,   268,  -761,   285,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,   867,   866,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,   827,   828,   799,    58,   804,   315,
    -761,  -761,  -761,   872,   165,  -761,   876,  -761,  -761,   807,
     281,  -761,   893,  -761,  -761,   664,   677,   -26,   651,   651,
     831,  -761,   896,   -39,    37,   847,   684,   900,   903,  -761,
    -761,   170,   690,  -761,    58,   651,   743,  -761,   404,   691,
     692,   401,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,   280,   697,  1028,   842,   651,   651,
      41,   326,  -761,  -761,  -761,  -761,   824,  -761,   651,   651,
     651,   651,   651,   651,   917,   703,   705,   708,   717,   934,
     935,   510,   510,  -761,   713,  -761,  -761,  -761,  -761,   720,
     113,  -761,  -761,   868,   651,   939,   651,   651,   -21,   725,
     116,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,    33,  -761,   734,  -761,   941,
    -761,   949,   106,  -761,  -761,   952,  -761,   955,   912,   490,
     755,   757,   971,  -761,   762,  -761,   763,  -761,   970,  -761,
     247,   974,   820,   821,  -761,  -761,  -761,   651,   915,   770,
    -761,   194,   404,   651,  -761,  -761,    45,  1158,   859,   779,
     275,  -761,  -761,  -761,   -39,   997,   869,  -761,  -761,  -761,
     998,   651,   781,  -761,   404,  -761,    74,    74,   651,  -761,
     329,   842,   845,   785,    -5,   -20,   375,  -761,   651,   651,
     126,   138,   140,   157,   162,   164,   933,   651,    34,   651,
    1005,   786,   346,   597,  -761,  -761,   756,  -761,  -761,  -761,
     856,   793,   280,   628,   884,  -761,   435,   435,   184,   184,
    1013,   435,   435,   184,   184,   510,   510,  -761,  -761,  -761,
    -761,  -761,  -761,   789,  -761,   813,  -761,  -761,  -761,  -761,
    1009,  1014,  -761,   811,  1016,  -761,  1023,  -761,  -761,  1021,
    -761,  -761,  1034,  1037,   818,    18,   865,   651,  -761,  -761,
    -761,   404,  1040,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,   825,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,   826,   829,   830,
     838,   849,   850,   851,   852,   244,   853,   811,  1020,    37,
     428,   844,  1043,  -761,   357,   854,  1055,  1056,  1059,  1075,
    -761,  1073,   370,  -761,   385,   396,  -761,   860,  -761,  1158,
     651,  -761,   651,     2,    60,  -761,  -761,  -761,  -761,  -761,
    -761,   280,   -85,  -761,   188,   -64,   858,    29,   862,  -761,
    1076,  -761,  -761,  1007,   628,   435,   880,   420,  -761,   280,
    1084,  1098,  1057,  1058,   449,   466,  -761,   895,   470,  -761,
    1101,  -761,  -761,   -39,   889,   580,  -761,    75,  -761,   342,
     689,  -761,  -761,  1102,  1158,  1158,   472,   592,   932,   963,
    1106,  1194,   986,   993,  -761,  -761,   201,  -761,   990,   811,
     487,   904,   994,  -761,   961,  -761,  -761,   651,  -761,  -761,
    -761,  -761,  -761,  -761,    74,  -761,  -761,  -761,   910,   404,
      14,  -761,   651,   881,   651,   734,   914,  1129,   911,   651,
    -761,   916,   918,   924,   492,  -761,  -761,  1028,  1130,  1133,
    -761,  -761,  1016,   527,  -761,  1023,   386,    36,    18,  1085,
    -761,  -761,  -761,  -761,  -761,  -761,  1093,  -761,  1147,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,   927,  1107,   494,
     946,   498,  -761,   936,   938,   945,   950,   951,   953,   958,
     959,   960,  1079,   964,   965,   966,   967,   968,   969,   976,
     977,   980,   981,  1081,   982,   983,   984,   988,  1008,  1010,
    1011,  1012,  1039,  1046,  1082,  1047,  1049,  1050,  1064,  1065,
    1069,  1070,  1071,  1072,  1077,  1094,  1080,  1086,  1087,  1088,
    1089,  1090,  1091,  1092,  1096,  1097,  1103,  1104,  1105,  1108,
    1109,  1110,  1111,  1112,  1113,  1114,  1115,  1131,  1116,  -761,
    -761,   106,  -761,  1054,  1100,   518,  -761,  1023,  1172,  1218,
     524,  -761,  -761,  -761,   404,  -761,   105,  1117,    21,  1118,
    -761,  -761,  -761,  1119,  1235,  1123,   404,  -761,    74,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  1231,
    -761,    75,   580,    18,    18,  1083,   342,  1256,  1264,  -761,
    1312,  -761,  -761,  1158,  1316,  1319,  1324,  1325,  1339,  1340,
    1341,  1342,  1343,  1126,  1345,  1346,  1347,  1348,  1349,  1350,
    1351,  1352,  1353,  1354,  1137,  1356,  1357,  1358,  1359,  1360,
    1361,  1362,  1363,  1364,  1365,  1148,  1367,  1368,  1369,  1370,
    1371,  1372,  1373,  1374,  1375,  1376,  1159,  1378,  1379,  1380,
    1381,  1382,  1383,  1384,  1385,  1386,  1387,  1170,  1389,  1390,
    1391,  1392,  1393,  1394,  1395,  1396,  1397,  1398,  1181,  1400,
    -761,  1403,  1404,  -761,   526,  -761,   804,  -761,  1405,  1406,
    1407,    53,  1191,  -761,   534,  1408,  -761,  -761,  1355,   811,
    -761,   651,   651,  -761,  1192,  -761,  1196,  1197,  1198,  1199,
    1200,  1201,  1202,  1203,  1204,  1409,  1205,  1206,  1207,  1208,
    1209,  1210,  1211,  1212,  1213,  1214,  1430,  1216,  1217,  1219,
    1220,  1221,  1222,  1223,  1224,  1225,  1226,  1433,  1227,  1228,
    1229,  1230,  1232,  1233,  1234,  1236,  1237,  1238,  1446,  1239,
    1240,  1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1450,
    1249,  1250,  1251,  1252,  1253,  1254,  1255,  1257,  1258,  1259,
    1471,  1260,  -761,  -761,  -761,  -761,  1261,  1262,  1263,   911,
    1317,  -761,   509,   651,   535,   818,   404,  -761,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  1267,  -761,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  1268,  -761,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  1269,
    -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    1270,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    -761,  1271,  -761,  -761,  -761,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  1272,  -761,  1479,  1480,    68,  1273,  1274,  1448,
    1491,  1492,  -761,  -761,  -761,   404,  -761,  -761,  -761,  -761,
    -761,  -761,  -761,  1275,  1276,   911,   804,  1336,  1282,  1497,
     648,    84,  1283,  1501,  1285,  -761,  1459,  1504,   656,  1503,
    -761,   911,   804,   911,   -25,  1290,  1291,  1505,  -761,  1466,
    1293,  -761,  1294,  1477,  1478,  -761,  1514,  -761,  -761,  -761,
     -17,   -59,  -761,  1299,  1300,  1481,  1483,  -761,  1484,  1485,
    1522,  -761,  -761,  1306,  -761,  1307,  1308,  1525,  1527,   804,
    1310,  1311,  -761,   804,  -761,  -761
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     236,     0,     0,     0,     0,     0,     0,     0,   236,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   236,     0,   537,     3,     5,    10,    12,    13,
      11,     6,     7,     9,   181,   180,     0,     8,    14,    15,
      16,    17,    18,    19,    20,   535,   535,   535,   535,   535,
       0,     0,   533,   533,   533,   533,   533,     0,   229,     0,
       0,     0,     0,     0,     0,   236,   167,    21,    26,    28,
      27,    22,    23,    25,    24,    29,    30,    31,    32,     0,
       0,     0,   250,   251,   249,   255,   259,     0,   256,     0,
       0,   252,     0,   254,   278,   279,     0,   257,     0,   288,
     290,   291,     0,   284,     0,     0,     0,   292,   293,   294,
     295,   298,   229,   296,     0,   235,   237,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   363,
     321,     0,     0,     1,   236,     2,   219,   221,   222,     0,
     204,   186,   192,     0,     0,     0,     0,     0,     0,     0,
       0,   165,     0,     0,     0,     0,     0,     0,   318,     0,
       0,   214,     0,     0,     0,     0,     0,     0,   166,     0,
     266,   267,   260,   261,   262,     0,   263,     0,   253,     0,
     258,   289,     0,   282,   281,   285,   286,     0,   323,     0,
       0,     0,     0,     0,   345,     0,   355,     0,   356,     0,
     342,   343,     0,   338,     0,   351,   353,     0,   346,     0,
       0,     0,     0,     0,     0,   364,   185,   184,     4,   220,
       0,   182,   183,   203,     0,     0,   200,     0,    34,     0,
      35,   165,   538,     0,     0,     0,   236,   532,   172,   174,
     173,   175,     0,   230,     0,   214,   169,     0,   161,   531,
       0,     0,   461,   465,   468,   469,     0,     0,     0,     0,
       0,     0,     0,     0,   466,   467,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   463,
       0,   236,     0,     0,   365,   370,   371,   385,   383,   386,
     384,   387,   388,   380,   375,   374,   373,   381,   382,   372,
     379,   378,   476,   479,     0,   480,   488,     0,   489,     0,
     481,   477,     0,   478,   503,     0,   504,     0,   475,   302,
     304,   303,   300,   301,   307,   309,   308,   305,   306,   312,
     314,   313,   310,   311,   287,     0,     0,   269,   268,   274,
     264,   265,   280,   283,     0,     0,     0,     0,   541,     0,
     238,   299,   348,     0,   339,   344,     0,   352,   347,     0,
       0,   354,     0,   319,   320,     0,     0,   206,     0,     0,
     202,   534,     0,   236,     0,     0,     0,     0,     0,   317,
     159,     0,     0,   163,     0,     0,     0,   168,   213,     0,
       0,     0,   512,   511,   514,   513,   516,   515,   518,   517,
     520,   519,   522,   521,     0,     0,   427,   236,     0,     0,
       0,     0,   470,   471,   472,   473,     0,   474,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   429,   428,   509,   506,   496,   486,   491,   494,     0,
       0,   498,   499,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   485,     0,   490,     0,
     493,     0,     0,   497,   505,     0,   508,     0,   275,   270,
       0,     0,     0,   322,     0,   297,     0,   357,     0,   340,
       0,     0,     0,     0,   350,   189,   188,     0,   208,   191,
     193,   198,   199,     0,   187,    33,    37,     0,     0,     0,
       0,    43,    47,    48,   236,     0,    41,   316,   315,   164,
       0,     0,   162,   176,   171,   170,     0,     0,     0,   416,
       0,   236,     0,     0,     0,     0,     0,   452,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   212,     0,     0,   377,   376,     0,   366,   369,   445,
     446,     0,     0,   236,     0,   426,   436,   437,   440,   441,
       0,   443,   435,   438,   439,   431,   430,   432,   433,   434,
     462,   464,   487,     0,   492,     0,   495,   500,   507,   510,
       0,     0,   271,     0,     0,   360,     0,   239,   341,     0,
     324,   349,     0,     0,   205,     0,   210,     0,   196,   197,
     195,   201,     0,    55,    58,    59,    56,    57,    60,    61,
      77,    62,    64,    63,    80,    67,    68,    69,    65,    66,
      70,    71,    72,    73,    74,    75,    76,     0,     0,     0,
       0,     0,     0,     0,     0,   541,     0,     0,   543,     0,
      40,     0,     0,   160,     0,     0,     0,     0,     0,     0,
     527,     0,     0,   523,     0,     0,   417,     0,   457,     0,
       0,   450,     0,     0,     0,   424,   423,   422,   421,   420,
     419,     0,     0,   461,     0,     0,     0,     0,     0,   406,
       0,   502,   501,     0,   236,   444,     0,     0,   425,     0,
       0,     0,   276,   272,     0,     0,    45,   546,     0,   544,
     325,   358,   359,   236,   207,   223,   225,   234,   226,     0,
     214,   194,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   152,   153,   156,   149,   156,     0,
       0,     0,    36,    44,   552,    42,   367,     0,   529,   528,
     526,   525,   530,   179,     0,   177,   418,   458,     0,   454,
       0,   453,     0,     0,     0,     0,     0,     0,   212,     0,
     404,     0,     0,     0,     0,   459,   448,   447,     0,     0,
     362,   361,     0,     0,   540,     0,     0,     0,     0,     0,
     243,   244,   245,   246,   242,   247,     0,   232,     0,   227,
     410,   408,   411,   409,   412,   413,   414,   209,   218,     0,
       0,     0,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   154,
     151,     0,   150,    50,    49,     0,   158,     0,     0,     0,
       0,   524,   456,   451,   455,   442,     0,     0,     0,     0,
     482,   484,   483,   212,     0,     0,   211,   407,     0,   460,
     449,   277,   273,    46,   547,   548,   550,   549,   545,     0,
     326,   234,   224,     0,     0,   231,     0,     0,   216,    79,
       0,   147,   148,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     155,     0,     0,   157,     0,    38,   541,   368,     0,     0,
       0,     0,     0,   405,     0,   327,   228,   240,     0,     0,
     415,     0,     0,   190,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    51,   542,   551,     0,     0,   212,   212,
     400,   178,     0,     0,     0,   217,   215,    78,    84,    85,
      82,    83,    86,    87,    88,    89,    90,     0,    81,   128,
     129,   126,   127,   130,   131,   132,   133,   134,     0,   125,
      95,    96,    93,    94,    97,    98,    99,   100,   101,     0,
      92,   106,   107,   104,   105,   108,   109,   110,   111,   112,
       0,   103,   139,   140,   137,   138,   141,   142,   143,   144,
     145,     0,   136,   117,   118,   115,   116,   119,   120,   121,
     122,   123,     0,   114,     0,     0,     0,     0,     0,     0,
       0,     0,   329,   328,   334,   241,   233,    91,   135,   102,
     113,   146,   124,   212,     0,   212,   541,   401,     0,   335,
     330,     0,     0,     0,     0,   399,     0,     0,     0,     0,
     331,   212,   541,   212,   541,     0,     0,     0,   336,   332,
       0,   395,     0,     0,     0,   398,     0,   402,   337,   333,
     541,   389,   397,     0,     0,     0,     0,   394,     0,     0,
       0,   403,   393,     0,   391,     0,     0,     0,     0,   541,
       0,     0,   396,   541,   390,   392
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -761,  -761,  -761,  1399,  1469,   257,  -761,  -761,   886,  -572,
    -761,  -656,  -761,   802,   798,  -761,  -583,   291,   298,  1309,
    -761,   343,  -761,  1151,   349,   362,    -6,  1517,   -19,  1193,
    1321,   -97,  -761,  -761,   937,  -761,  -761,  -761,  -761,  -761,
    -761,  -761,  -760,  -238,  -761,  -761,  -761,  -761,   760,  -203,
     199,   622,  -761,  -761,  1366,  -761,  -761,   364,   369,   376,
     379,   382,  -761,  -761,  -761,  -223,  -761,  1120,  -247,  -227,
    -705,  -704,  -703,  -675,  -674,  -673,   619,  -761,  -761,  -761,
    -761,  -761,  -761,  1136,  -761,  -761,   999,  -277,  -274,  -761,
    -761,  -761,   783,  -761,  -761,  -761,  -761,   787,  -761,  -761,
    1099,  1095,   790,  -761,  -761,  -761,  -761,  1284,  -516,   803,
    -154,   545,   630,  -761,  -761,  -641,  -761,   665,   766,  -761
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   510,   705,   511,   512,
     811,   645,   736,   737,   883,   513,   381,    27,    28,   236,
      29,    30,   245,   246,    31,    32,    33,    34,    35,   141,
     221,   142,   226,   499,   500,   610,   370,   504,   224,   498,
     606,   720,   688,   248,  1023,   928,   139,   714,   715,   716,
     717,   799,    36,   115,   116,   718,   796,    37,    38,    39,
      40,    41,    42,    43,    44,   283,   522,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   806,   807,   293,   294,
     295,   296,   297,   411,   298,   299,   300,   301,   302,   899,
     303,   304,   305,   306,   307,   308,   309,   310,   311,   312,
     437,   438,   313,   314,   315,   316,   317,   318,   662,   663,
     250,   153,   144,   135,   149,   485,   742,   708,   709,   516
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     388,   367,    73,   132,   738,   442,   436,   387,   905,   251,
     704,   664,   410,   758,   800,   801,   802,    45,    46,    47,
     373,    58,   706,    48,    49,   342,   433,   434,   247,   406,
     190,   433,   434,   768,   440,   140,   580,   683,    16,   351,
     507,    51,   222,   445,   803,   804,   805,   162,   163,   669,
     431,   432,   484,   561,   172,   497,   136,  1099,   137,    73,
     484,    58,   382,   138,   740,   136,   670,   137,   810,   812,
     446,   447,   138,    65,  1195,  1238,    60,     1,   797,     2,
       3,     4,     5,     6,     7,   761,     9,   253,   254,   255,
    1211,   446,   447,    11,   769,   104,    12,   893,    13,    14,
      15,   319,    16,   320,   321,   324,  1239,   325,   326,  1223,
     253,   254,   255,    52,    53,    54,   106,  1235,   769,    55,
      56,   501,   502,   366,   329,   118,   330,   331,   409,   798,
     119,   111,   120,   769,   121,   446,   447,   562,   524,   764,
    1224,   656,   657,  1012,   446,   447,   762,    16,  1236,   769,
     446,   447,   658,   659,   660,  -539,   885,   446,   447,   322,
     766,   534,   535,   327,   508,   143,   509,   105,   530,   446,
     447,   540,   541,   542,   543,   544,   545,   406,   612,    50,
     112,    22,   332,   260,   261,   262,   374,   386,   564,   263,
     114,   159,   164,   582,   128,   587,   446,   447,   444,   559,
     560,   126,   127,    59,   488,    61,   260,   261,   262,   130,
     913,   129,   263,   489,   113,   446,   447,   383,   264,   265,
     266,   800,   801,   802,   566,   567,   568,   569,   570,   571,
     572,   573,   574,   575,   576,   577,   578,   579,   713,    17,
     435,   264,   265,   266,   280,   435,   565,   220,   581,   279,
     151,   803,   804,   805,   173,   133,   611,   921,   661,   161,
     446,   447,   439,   608,   609,    67,   323,    19,   446,   447,
     328,   196,   197,   732,   604,    57,   198,  1025,    21,   599,
     171,   446,   447,   252,   253,   254,   255,   122,   600,   333,
      22,   673,   674,   446,   447,   446,   447,   117,   654,    68,
     682,   280,   685,   187,   188,   665,    69,   123,   441,   282,
     134,   124,   446,   447,   125,   450,   732,   446,   447,   446,
     447,   484,    67,   140,   280,   377,   378,   215,   733,  1008,
     734,   735,   282,   881,   555,   695,  -540,  -540,  1177,  1178,
     697,   174,   175,   379,   229,   230,   231,   675,   143,   446,
     447,    70,   404,   239,   240,   241,    68,    71,   150,   676,
     501,   677,   258,    69,   259,  1095,   443,   506,   152,   444,
      72,   733,    74,   734,   735,   158,   185,    75,   678,   186,
     260,   261,   262,   679,    76,   680,   263,    77,   533,   348,
      78,   519,  1014,   159,   520,  -540,  -540,   460,   461,   462,
     463,   464,   693,   160,   252,   253,   254,   255,    70,   537,
     465,   538,   765,   539,    71,   264,   265,   266,   267,   919,
     268,   920,   269,   759,   270,   760,   271,    72,   272,    74,
     165,   344,   376,  1202,    75,  1204,  1104,   273,   107,   108,
     109,    76,   176,   177,    77,   345,   346,    78,   136,   166,
     137,  1220,   466,  1222,   763,   138,   110,   467,   671,   274,
     672,   275,   539,   276,   425,   528,   426,   427,   492,   493,
     428,   774,   777,   256,   257,    62,    63,   472,   473,   468,
      64,   470,   808,   258,   469,   259,   471,   474,   182,   183,
     184,   897,   475,   277,   278,   279,   648,   180,   280,   649,
     281,   260,   261,   262,   476,   167,   282,   263,   650,   477,
     335,   170,   667,   336,   337,   894,   169,   896,   338,   339,
     178,   274,   906,   275,   890,   276,   252,   253,   254,   255,
     914,   915,   916,   917,   591,   592,   264,   265,   266,   267,
     179,   268,   181,   269,   696,   270,   483,   271,  1180,   272,
     666,  1181,  1182,   444,   189,  1205,  1183,  1184,   273,   446,
     447,   813,   814,   815,   816,   817,   450,   689,   818,   819,
     690,  1221,   192,  1225,   191,   820,   821,   822,   746,   193,
     274,   444,   275,   523,   276,  -540,  -540,   453,   454,  1237,
     210,   753,   194,  -540,   754,   256,   257,   823,   154,   155,
     156,   157,   691,   692,  1000,   258,   755,   259,  1252,   754,
     409,   195,  1255,   209,   277,   278,   279,   756,   211,   280,
     444,   281,   529,   260,   261,   262,   213,   282,   212,   263,
     214,   252,   253,   254,   255,   789,  -248,   790,   791,   792,
     793,   776,   794,   795,   444,  -540,   458,   459,   460,   461,
     462,   463,   464,   219,   252,   253,   254,   255,   264,   265,
     266,   267,   216,   268,   217,   269,   223,   270,   227,   271,
     780,   272,   225,   520,   228,   773,   145,   146,   147,   148,
     273,   824,   825,   826,   827,   828,   232,   781,   829,   830,
     782,   784,  1209,  1210,   785,   831,   832,   833,  1217,  1218,
     256,   257,   274,   233,   275,    16,   276,   787,   886,   235,
     258,   520,   259,   910,   234,   929,   444,   834,   930,   932,
    1017,  1018,   933,   256,   257,   462,   463,   464,   260,   261,
     262,   237,   238,   258,   263,   259,   277,   278,   279,  1003,
     242,   280,   520,   281,   243,  1007,   244,  1094,   444,   282,
     785,   260,   261,   262,   247,  1101,  1186,   263,   754,   520,
     249,   334,   340,   264,   265,   266,   267,   341,   268,   343,
     269,   347,   270,   352,   271,  1106,   272,   349,   353,   354,
     355,   252,   253,   254,   255,   273,   264,   265,   266,   267,
     356,   268,   357,   269,   358,   270,   359,   271,  1105,   272,
     360,   361,   362,   363,   364,   368,   369,   274,   273,   275,
    -536,   276,   372,   371,   380,   384,   412,     1,   391,     2,
       3,     4,     5,     6,     7,     8,     9,    10,   385,   389,
     274,   407,   275,    11,   276,   390,    12,   408,    13,    14,
      15,   277,   278,   279,   413,   414,   280,   415,   281,   418,
     404,   405,   416,   419,   282,   420,  1185,   421,   422,   423,
     258,   424,   259,   429,   277,   278,   279,   430,   465,   280,
     478,   281,   479,   480,   481,   482,   487,   282,   260,   261,
     262,   484,   490,   491,   263,   495,     1,    16,     2,     3,
       4,     5,     6,     7,     8,     9,    10,   494,   496,   503,
     505,   514,    11,   517,   515,    12,   518,    13,    14,    15,
     521,   526,   527,   264,   265,   266,   267,   531,   268,    16,
     269,   546,   270,   547,   271,   548,   272,     1,   549,     2,
       3,     4,     5,     6,     7,   273,     9,   550,   551,   552,
     553,   554,   558,    11,   556,   563,    12,   583,    13,    14,
      15,    79,   280,   532,   590,   585,    16,   274,   588,   275,
     589,   276,   392,   393,   394,   395,   396,   397,   398,   399,
     400,   401,   402,   403,   595,   593,   598,   594,   601,    17,
      80,    81,   596,    82,   597,   602,   603,   646,    83,    84,
     605,   277,   278,   279,   607,    18,   280,    16,   281,   647,
     651,   653,   652,   562,   282,   655,   668,    19,   681,   686,
     687,   446,   450,   694,   698,   702,   700,    20,    21,   507,
     703,   835,   836,   837,   838,   839,   707,   710,   840,   841,
      22,   451,   452,   453,   454,   842,   843,   844,   711,   456,
     701,   712,   444,   719,   722,   723,   724,   745,    17,   725,
     726,   741,   846,   847,   848,   849,   850,   845,   727,   851,
     852,   748,   749,   750,    18,   744,   853,   854,   855,   728,
     729,   730,   731,   739,   747,   448,    19,   449,   751,   752,
     771,   757,   767,   770,   772,   532,    20,    21,   856,    17,
     692,   457,   458,   459,   460,   461,   462,   463,   464,    22,
     532,   775,   895,   691,   779,   783,   778,   786,   809,    85,
      86,    87,    88,   788,    89,    90,   879,    19,    91,    92,
      93,   880,   881,    94,   887,    95,   889,   888,    21,    96,
      97,   892,   898,   903,   450,   904,   911,   907,   908,   912,
      22,   923,    98,    99,   450,   909,   100,   101,   102,   924,
     925,   926,   103,   451,   452,   453,   454,   455,   927,   450,
     934,   456,   935,   451,   452,   453,   454,   931,   699,   936,
     943,   456,   954,   965,   937,   938,  1005,   939,   451,   452,
     453,   454,   940,   941,   942,   976,   456,  1001,   944,   945,
     946,   947,   948,   949,   987,   857,   858,   859,   860,   861,
     950,   951,   862,   863,   952,   953,   955,   956,   957,   864,
     865,   866,   958,   457,   458,   459,   460,   461,   462,   463,
     464,  1006,   998,   457,   458,   459,   460,   461,   462,   463,
     464,   867,   959,  1002,   960,   961,   962,  1015,   457,   458,
     459,   460,   461,   462,   463,   464,   613,   614,   615,   616,
     617,   618,   619,   620,   621,   622,   623,   624,   625,   626,
     627,   628,   629,   963,   630,   631,   632,   633,   634,   635,
     964,   966,   636,   967,   968,   637,   638,   639,   640,   641,
     642,   643,   644,   868,   869,   870,   871,   872,   969,   970,
     873,   874,   199,   971,   972,   973,   974,   875,   876,   877,
     769,   975,   200,  1019,   977,   201,   202,   203,  1021,   204,
     978,   979,   980,   981,   982,   983,   984,  1022,  1024,   878,
     985,   986,  1026,   205,   206,  1027,   207,   208,   988,   989,
    1028,  1029,   990,   991,   992,   993,   994,   995,   996,   997,
     999,  1009,  1010,  1011,  1013,  1030,  1031,  1032,  1033,  1034,
    1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,
    1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,
    1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,
    1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1096,
    1097,  1098,  1100,  1107,  1102,  1117,  1103,  1108,  1109,  1110,
    1111,  1112,  1113,  1114,  1115,  1116,  1118,  1119,  1120,  1121,
    1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1130,  1139,
    1131,  1132,  1133,  1134,  1135,  1136,  1137,  1138,  1140,  1141,
    1142,  1143,  1150,  1144,  1145,  1146,  1161,  1147,  1148,  1149,
    1151,  1152,  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,
    1162,  1163,  1164,  1165,  1166,  1167,  1168,  1172,  1169,  1170,
    1171,  1173,  1179,  1193,  1194,  1174,  1175,  1176,  1187,  1188,
    1189,  1190,  1191,  1192,  1196,  1197,  1198,  1199,  1200,  1201,
    1203,  1206,  1207,  1208,  1212,  1213,  1214,  1215,  1216,  1219,
    1226,  1228,  1227,  1229,  1230,  1231,  1232,  1233,  1234,  1240,
    1242,  1241,  1243,  1244,  1245,  1246,  1247,  1248,  1250,  1249,
    1251,  1253,  1254,   218,   168,   743,   884,   525,   882,   131,
     375,   365,   486,  1016,   721,  1020,   536,   684,   922,   900,
     417,   918,  1004,   901,     0,     0,   902,   891,   350,     0,
       0,     0,     0,     0,   557,     0,   586,     0,   584
};

static const yytype_int16 yycheck[] =
{
     247,   224,     8,    22,   645,   282,   280,   245,   768,   163,
     593,   527,   259,   669,   719,   719,   719,    32,    33,    34,
      54,     3,   594,    38,    39,   179,     5,     6,    65,   256,
      61,     5,     6,     4,   281,     8,     3,     3,    77,   193,
       3,    36,   139,    54,   719,   719,   719,    75,    76,    54,
     277,   278,    77,    74,     6,    81,    20,     4,    22,    65,
      77,     3,    37,    27,   647,    20,    86,    22,   724,   725,
     155,   156,    27,     3,     6,   134,    75,     7,     3,     9,
      10,    11,    12,    13,    14,    83,    16,     4,     5,     6,
       6,   155,   156,    23,    65,    33,    26,    83,    28,    29,
      30,     3,    77,     5,     6,     3,   165,     5,     6,   134,
       4,     5,     6,    32,    33,    34,    39,   134,    65,    38,
      39,   368,   369,   220,     3,     7,     5,     6,    87,    54,
      12,     3,    14,    65,    16,   155,   156,   158,   385,   224,
     165,    67,    68,   903,   155,   156,    86,    77,   165,    65,
     155,   156,    78,    79,    80,    61,   739,   155,   156,    61,
     224,   408,   409,    61,   127,    71,   129,    33,   391,   155,
     156,   418,   419,   420,   421,   422,   423,   404,   133,   194,
       3,   220,    61,   100,   101,   102,   220,   224,    72,   106,
       3,   222,   220,   467,    33,   472,   155,   156,   224,   446,
     447,    32,    33,     4,    39,     6,   100,   101,   102,   159,
     782,    50,   106,    48,    15,   155,   156,   236,   135,   136,
     137,   926,   926,   926,   451,   452,   453,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   220,   169,
     219,   135,   136,   137,   218,   219,   130,   220,   215,   215,
      51,   926,   926,   926,   206,     0,   503,   221,   184,    60,
     155,   156,   281,    69,    70,     8,   168,   197,   155,   156,
     168,   185,   186,    72,   497,   194,   190,   933,   208,    32,
      81,   155,   156,     3,     4,     5,     6,   169,    41,   168,
     220,   538,   539,   155,   156,   155,   156,   172,   521,     8,
     547,   218,   549,   104,   105,   528,     8,   189,   225,   226,
     223,   193,   155,   156,   196,   131,    72,   155,   156,   155,
     156,    77,    65,     8,   218,    32,    33,   128,   127,   224,
     129,   130,   226,   132,   221,   562,   152,   153,  1098,  1099,
     563,   173,   174,    50,   145,   146,   147,   221,    71,   155,
     156,     8,    72,   154,   155,   156,    65,     8,     3,   221,
     607,   221,    82,    65,    84,  1006,   221,   373,    71,   224,
       8,   127,     8,   129,   130,     3,   202,     8,   221,   205,
     100,   101,   102,   221,     8,   221,   106,     8,   407,   190,
       8,   221,   908,   222,   224,   211,   212,   213,   214,   215,
     216,   217,   556,    14,     3,     4,     5,     6,    65,    83,
     222,    85,   224,    87,    65,   135,   136,   137,   138,    33,
     140,    35,   142,   670,   144,   672,   146,    65,   148,    65,
       3,    10,   233,  1193,    65,  1195,  1019,   157,   159,   160,
     161,    65,   173,   174,    65,    24,    25,    65,    20,     3,
      22,  1211,   219,  1213,   681,    27,   177,   224,    83,   179,
      85,   181,    87,   183,   120,    64,   122,   123,   187,   188,
     126,   694,   699,    72,    73,   166,   167,   224,   225,   219,
     171,   219,   720,    82,   224,    84,   224,   219,   201,   202,
     203,   765,   224,   213,   214,   215,   221,     6,   218,   224,
     220,   100,   101,   102,   219,     3,   226,   106,   514,   224,
      39,     3,   531,    42,    43,   762,   220,   764,    47,    48,
       3,   179,   769,   181,   747,   183,     3,     4,     5,     6,
       3,     4,     5,     6,    44,    45,   135,   136,   137,   138,
      76,   140,     3,   142,   563,   144,   347,   146,    39,   148,
     221,    42,    43,   224,     3,  1196,    47,    48,   157,   155,
     156,    89,    90,    91,    92,    93,   131,   221,    96,    97,
     224,  1212,   224,  1214,    54,   103,   104,   105,   221,     6,
     179,   224,   181,   384,   183,   150,   151,   152,   153,  1230,
       4,   221,   194,   158,   224,    72,    73,   125,    53,    54,
      55,    56,     5,     6,   881,    82,   221,    84,  1249,   224,
      87,   194,  1253,   194,   213,   214,   215,   221,   192,   218,
     224,   220,   221,   100,   101,   102,   194,   226,    75,   106,
     194,     3,     4,     5,     6,    55,    56,    57,    58,    59,
      60,   221,    62,    63,   224,   210,   211,   212,   213,   214,
     215,   216,   217,    21,     3,     4,     5,     6,   135,   136,
     137,   138,   221,   140,   221,   142,    64,   144,    72,   146,
     221,   148,    66,   224,     3,   694,    46,    47,    48,    49,
     157,    89,    90,    91,    92,    93,     3,   221,    96,    97,
     224,   221,    44,    45,   224,   103,   104,   105,    42,    43,
      72,    73,   179,    61,   181,    77,   183,   713,   221,   220,
      82,   224,    84,   221,    61,   221,   224,   125,   224,   221,
     923,   924,   224,    72,    73,   215,   216,   217,   100,   101,
     102,    73,     3,    82,   106,    84,   213,   214,   215,   221,
       3,   218,   224,   220,     3,   221,     3,   221,   224,   226,
     224,   100,   101,   102,    65,   221,   221,   106,   224,   224,
       4,   221,     3,   135,   136,   137,   138,     3,   140,     4,
     142,    61,   144,     4,   146,  1022,   148,   220,   165,     6,
       3,     3,     4,     5,     6,   157,   135,   136,   137,   138,
       6,   140,     4,   142,     3,   144,     4,   146,  1021,   148,
      54,     4,   194,     3,     3,    52,    67,   179,   157,   181,
       0,   183,   133,    73,     3,    61,     4,     7,   220,     9,
      10,    11,    12,    13,    14,    15,    16,    17,   210,    77,
     179,   220,   181,    23,   183,    77,    26,   220,    28,    29,
      30,   213,   214,   215,     4,     4,   218,     4,   220,   220,
      72,    73,     6,   220,   226,   220,  1103,   220,   220,   220,
      82,   220,    84,   220,   213,   214,   215,   220,   222,   218,
       3,   220,     6,    46,    46,    76,     4,   226,   100,   101,
     102,    77,     6,    76,   106,   221,     7,    77,     9,    10,
      11,    12,    13,    14,    15,    16,    17,     4,   221,    68,
       4,    54,    23,     3,   220,    26,     3,    28,    29,    30,
     220,   220,   220,   135,   136,   137,   138,   220,   140,    77,
     142,     4,   144,   220,   146,   220,   148,     7,   220,     9,
      10,    11,    12,    13,    14,   157,    16,   220,     4,     4,
     227,   221,     3,    23,    76,   220,    26,     6,    28,    29,
      30,     3,   218,    72,    42,     6,    77,   179,     6,   181,
       5,   183,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,     3,   220,     6,   220,     4,   169,
      32,    33,   220,    35,   221,   165,   165,   128,    40,    41,
      75,   213,   214,   215,   224,   185,   218,    77,   220,   220,
       3,     3,   133,   158,   226,   224,   221,   197,    75,     4,
     224,   155,   131,   220,   130,     6,   227,   207,   208,     3,
       6,    89,    90,    91,    92,    93,     3,     6,    96,    97,
     220,   150,   151,   152,   153,   103,   104,   105,     4,   158,
     227,     4,   224,   178,     4,   220,   220,     4,   169,   220,
     220,    31,    89,    90,    91,    92,    93,   125,   220,    96,
      97,     6,     6,     4,   185,   221,   103,   104,   105,   220,
     220,   220,   220,   220,   220,    72,   197,    74,     3,     6,
       4,   221,   224,   221,    77,    72,   207,   208,   125,   169,
       6,   210,   211,   212,   213,   214,   215,   216,   217,   220,
      72,   221,   221,     5,    46,   210,    49,     6,     6,   161,
     162,   163,   164,   224,   166,   167,   130,   197,   170,   171,
     172,   128,   132,   175,   220,   177,   165,   133,   208,   181,
     182,   221,   218,     4,   131,   224,     6,   221,   220,     6,
     220,    56,   194,   195,   131,   221,   198,   199,   200,    56,
       3,   224,   204,   150,   151,   152,   153,   154,    51,   131,
     224,   158,   224,   150,   151,   152,   153,   221,   155,   224,
      91,   158,    91,    91,   224,   224,     4,   224,   150,   151,
     152,   153,   224,   224,   224,    91,   158,   133,   224,   224,
     224,   224,   224,   224,    91,    89,    90,    91,    92,    93,
     224,   224,    96,    97,   224,   224,   224,   224,   224,   103,
     104,   105,   224,   210,   211,   212,   213,   214,   215,   216,
     217,     3,    91,   210,   211,   212,   213,   214,   215,   216,
     217,   125,   224,   133,   224,   224,   224,     6,   210,   211,
     212,   213,   214,   215,   216,   217,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   224,   106,   107,   108,   109,   110,   111,
     224,   224,   114,   224,   224,   117,   118,   119,   120,   121,
     122,   123,   124,    89,    90,    91,    92,    93,   224,   224,
      96,    97,   160,   224,   224,   224,   224,   103,   104,   105,
      65,   224,   170,   220,   224,   173,   174,   175,    52,   177,
     224,   224,   224,   224,   224,   224,   224,    53,     6,   125,
     224,   224,     6,   191,   192,     6,   194,   195,   224,   224,
       6,     6,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   221,     6,     6,     6,     6,     6,
     224,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   224,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   224,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   224,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   224,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   224,     6,     4,     4,     4,
       4,     4,   221,   221,     6,     6,    61,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,     6,   221,   221,     6,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,     6,   221,   221,   221,     6,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,     6,   221,   221,
     221,   221,   165,     4,     4,   224,   224,   224,   221,   221,
     221,   221,   221,   221,   221,   221,    48,     6,     6,   224,
     224,   165,   220,     6,   221,     4,   221,    48,     4,     6,
     220,     6,   221,    47,   221,   221,    39,    39,     4,   220,
      39,   221,    39,    39,    39,     3,   220,   220,     3,   221,
       3,   221,   221,   134,    65,   649,   738,   386,   736,    22,
     231,   220,   349,   921,   607,   926,   410,   548,   788,   766,
     266,   785,   887,   766,    -1,    -1,   766,   754,   192,    -1,
      -1,    -1,    -1,    -1,   444,    -1,   471,    -1,   469
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    77,   169,   185,   197,
     207,   208,   220,   229,   230,   231,   233,   245,   246,   248,
     249,   252,   253,   254,   255,   256,   280,   285,   286,   287,
     288,   289,   290,   291,   292,    32,    33,    34,    38,    39,
     194,    36,    32,    33,    34,    38,    39,   194,     3,   278,
      75,   278,   166,   167,   171,     3,   232,   233,   245,   246,
     249,   252,   253,   254,   285,   286,   287,   288,   289,     3,
      32,    33,    35,    40,    41,   161,   162,   163,   164,   166,
     167,   170,   171,   172,   175,   177,   181,   182,   194,   195,
     198,   199,   200,   204,    33,    33,    39,   159,   160,   161,
     177,     3,     3,   278,     3,   281,   282,   172,     7,    12,
      14,    16,   169,   189,   193,   196,    32,    33,    33,    50,
     159,   255,   256,     0,   223,   341,    20,    22,    27,   274,
       8,   257,   259,    71,   340,   340,   340,   340,   340,   342,
       3,   278,    71,   339,   339,   339,   339,   339,     3,   222,
      14,   278,    75,    76,   220,     3,     3,     3,   232,   220,
       3,   278,     6,   206,   173,   174,   173,   174,     3,    76,
       6,     3,   201,   202,   203,   202,   205,   278,   278,     3,
      61,    54,   224,     6,   194,   194,   185,   186,   190,   160,
     170,   173,   174,   175,   177,   191,   192,   194,   195,   194,
       4,   192,    75,   194,   194,   278,   221,   221,   231,    21,
     220,   258,   259,    64,   266,    66,   260,    72,     3,   278,
     278,   278,     3,    61,    61,   220,   247,    73,     3,   278,
     278,   278,     3,     3,     3,   250,   251,    65,   271,     4,
     338,   338,     3,     4,     5,     6,    72,    73,    82,    84,
     100,   101,   102,   106,   135,   136,   137,   138,   140,   142,
     144,   146,   148,   157,   179,   181,   183,   213,   214,   215,
     218,   220,   226,   293,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   306,   307,   308,   309,   310,   312,   313,
     314,   315,   316,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   330,   331,   332,   333,   334,   335,     3,
       5,     6,    61,   168,     3,     5,     6,    61,   168,     3,
       5,     6,    61,   168,   221,    39,    42,    43,    47,    48,
       3,     3,   338,     4,    10,    24,    25,    61,   278,   220,
     282,   338,     4,   165,     6,     3,     6,     4,     3,     4,
      54,     4,   194,     3,     3,   258,   259,   293,    52,    67,
     264,    73,   133,    54,   220,   247,   278,    32,    33,    50,
       3,   244,    37,   256,    61,   210,   224,   271,   296,    77,
      77,   220,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,    72,    73,   297,   220,   220,    87,
     296,   311,     4,     4,     4,     4,     6,   335,   220,   220,
     220,   220,   220,   220,   220,   120,   122,   123,   126,   220,
     220,   297,   297,     5,     6,   219,   316,   328,   329,   256,
     296,   225,   315,   221,   224,    54,   155,   156,    72,    74,
     131,   150,   151,   152,   153,   154,   158,   210,   211,   212,
     213,   214,   215,   216,   217,   222,   219,   224,   219,   224,
     219,   224,   224,   225,   219,   224,   219,   224,     3,     6,
      46,    46,    76,   278,    77,   343,   257,     4,    39,    48,
       6,    76,   187,   188,     4,   221,   221,    81,   267,   261,
     262,   296,   296,    68,   265,     4,   254,     3,   127,   129,
     234,   236,   237,   243,    54,   220,   347,     3,     3,   221,
     224,   220,   294,   278,   296,   251,   220,   220,    64,   221,
     293,   220,    72,   256,   296,   296,   311,    83,    85,    87,
     296,   296,   296,   296,   296,   296,     4,   220,   220,   220,
     220,     4,     4,   227,   221,   221,    76,   295,     3,   296,
     296,    74,   158,   220,    72,   130,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   297,
       3,   215,   316,     6,   328,     6,   329,   315,     6,     5,
      42,    44,    45,   220,   220,     3,   220,   221,     6,    32,
      41,     4,   165,   165,   293,    75,   268,   224,    69,    70,
     263,   296,   133,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     106,   107,   108,   109,   110,   111,   114,   117,   118,   119,
     120,   121,   122,   123,   124,   239,   128,   220,   221,   224,
     254,     3,   133,     3,   293,   224,    67,    68,    78,    79,
      80,   184,   336,   337,   336,   293,   221,   256,   221,    54,
      86,    83,    85,   296,   296,   221,   221,   221,   221,   221,
     221,    75,   296,     3,   314,   296,     4,   224,   270,   221,
     224,     5,     6,   338,   220,   297,   256,   293,   130,   155,
     227,   227,     6,     6,   244,   235,   237,     3,   345,   346,
       6,     4,     4,   220,   275,   276,   277,   278,   283,   178,
     269,   262,     4,   220,   220,   220,   220,   220,   220,   220,
     220,   220,    72,   127,   129,   130,   240,   241,   343,   220,
     244,    31,   344,   236,   221,     4,   221,   220,     6,     6,
       4,     3,     6,   221,   224,   221,   221,   221,   239,   296,
     296,    83,    86,   297,   224,   224,   224,   224,     4,    65,
     221,     4,    77,   256,   293,   221,   221,   297,    49,    46,
     221,   221,   224,   210,   221,   224,     6,   254,   224,    55,
      57,    58,    59,    60,    62,    63,   284,     3,    54,   279,
     298,   299,   300,   301,   302,   303,   304,   305,   271,     6,
     239,   238,   239,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   125,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   125,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   125,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   125,    89,    90,    91,
      92,    93,    96,    97,   103,   104,   105,   125,    89,    90,
      91,    92,    93,    96,    97,   103,   104,   105,   125,   130,
     128,   132,   241,   242,   242,   244,   221,   220,   133,   165,
     293,   337,   221,    83,   296,   221,   296,   316,   218,   317,
     320,   325,   330,     4,   224,   270,   296,   221,   220,   221,
     221,     6,     6,   237,     3,     4,     5,     6,   346,    33,
      35,   221,   276,    56,    56,     3,   224,    51,   273,   221,
     224,   221,   221,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,    91,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,    91,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,    91,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,    91,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,    91,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,    91,   224,
     315,   133,   133,   221,   345,     4,     3,   221,   224,   224,
     224,   224,   270,   221,   336,     6,   279,   277,   277,   220,
     304,    52,    53,   272,     6,   239,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   224,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   224,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   224,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   224,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   224,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     224,     6,     4,     4,   221,   343,     4,     4,     4,     4,
     221,   221,     6,    61,   244,   293,   296,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,     6,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,     6,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,     6,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
       6,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,     6,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   221,     6,   221,   224,   224,   224,   270,   270,   165,
      39,    42,    43,    47,    48,   296,   221,   221,   221,   221,
     221,   221,   221,     4,     4,     6,   221,   221,    48,     6,
       6,   224,   270,   224,   270,   343,   165,   220,     6,    44,
      45,     6,   221,     4,   221,    48,     4,    42,    43,     6,
     270,   343,   270,   134,   165,   343,   220,   221,     6,    47,
     221,   221,    39,    39,     4,   134,   165,   343,   134,   165,
     220,   221,    39,    39,    39,    39,     3,   220,   220,   221,
       3,     3,   343,   221,   221,   343
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   228,   229,   230,   230,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   234,   234,   235,   235,   236,   236,   237,
     237,   237,   237,   238,   238,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   239,
     239,   239,   239,   239,   239,   239,   239,   239,   239,   240,
     240,   241,   241,   241,   241,   242,   242,   243,   243,   244,
     244,   245,   246,   246,   247,   247,   248,   248,   249,   250,
     250,   251,   252,   252,   252,   252,   252,   253,   253,   253,
     254,   254,   254,   254,   255,   255,   256,   257,   258,   258,
     259,   260,   260,   261,   261,   262,   263,   263,   263,   264,
     264,   265,   265,   266,   266,   267,   267,   268,   268,   269,
     269,   270,   270,   271,   271,   272,   272,   273,   273,   274,
     274,   274,   274,   275,   275,   276,   276,   277,   277,   278,
     278,   279,   279,   279,   279,   280,   280,   281,   281,   282,
     283,   283,   284,   284,   284,   284,   284,   284,   284,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   286,   286,   286,   286,   287,   287,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   289,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     291,   291,   291,   292,   292,   293,   293,   294,   294,   295,
     295,   296,   296,   296,   296,   296,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   298,
     298,   298,   299,   299,   299,   299,   300,   300,   300,   300,
     301,   301,   301,   301,   302,   302,   303,   303,   304,   304,
     304,   304,   304,   304,   305,   305,   306,   306,   306,   306,
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
       2,     2,     2,     2,     2,     2,     2,     5,     2,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     6,     6,     5,     3,     4,
       4,     2,     5,     3,     6,     7,     9,    10,    12,    12,
      13,    14,    15,    16,    12,    13,    15,    16,     3,     4,
       5,     6,     3,     3,     4,     3,     3,     4,     4,     6,
       5,     3,     4,     3,     4,     3,     3,     5,     7,     7,
       6,     8,     8,     2,     3,     1,     3,     3,     5,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,    14,
      19,    16,    20,    16,    15,    13,    18,    14,    13,    11,
       8,    10,    13,    15,     5,     7,     4,     6,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     4,     5,     4,
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
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2484 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2492 "parser.cpp"
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
#line 2506 "parser.cpp"
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
#line 2520 "parser.cpp"
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
#line 2534 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 190 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2543 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 185 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2552 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 322 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2563 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2571 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 333 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2580 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 333 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2589 "parser.cpp"
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
#line 2603 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2624 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2634 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2644 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2654 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2664 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 368 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2674 "parser.cpp"
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
#line 2688 "parser.cpp"
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
#line 2702 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 356 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2712 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2720 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2728 "parser.cpp"
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
#line 2742 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2751 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2759 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2767 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2783 "parser.cpp"
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
#line 2797 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2815 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2824 "parser.cpp"
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
#line 2837 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 346 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2846 "parser.cpp"
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
#line 2860 "parser.cpp"
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
#line 2874 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 362 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 351 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2893 "parser.cpp"
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
#line 2907 "parser.cpp"
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
#line 2921 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2929 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3001 "parser.cpp"
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
#line 3015 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3023 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3055 "parser.cpp"
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
#line 3068 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3076 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3084 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 338 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3100 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3108 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3116 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3124 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3132 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3140 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3148 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3156 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3164 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3172 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3180 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3188 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3196 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 383 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3204 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 387 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3212 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3220 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3228 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3236 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3244 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3252 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 342 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3260 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 329 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3268 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 219 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3279 "parser.cpp"
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
#line 3293 "parser.cpp"
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
#line 3307 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 195 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3318 "parser.cpp"
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

#line 3426 "parser.cpp"

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
#line 3641 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 523 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3652 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 529 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3663 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 536 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3669 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3675 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3681 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3687 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3693 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3699 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3705 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3711 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 544 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3717 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 545 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3723 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 546 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3729 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3735 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 548 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3741 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 549 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3747 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 550 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3753 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 551 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3759 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 553 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3765 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 554 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3771 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 555 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3777 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 556 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3783 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3789 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 558 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3795 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3801 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 560 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3807 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 561 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3813 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 562 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3819 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 563 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3825 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 564 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3831 "parser.cpp"
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
#line 3853 "parser.cpp"
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
#line 3873 "parser.cpp"
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
#line 3891 "parser.cpp"
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
#line 3924 "parser.cpp"
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
#line 3944 "parser.cpp"
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
#line 3980 "parser.cpp"
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
#line 4002 "parser.cpp"
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
#line 4023 "parser.cpp"
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
#line 4056 "parser.cpp"
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
#line 4091 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 792 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4100 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 796 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4109 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 801 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4118 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 805 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4127 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 811 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4135 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 814 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4143 "parser.cpp"
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
#line 4168 "parser.cpp"
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
#line 4195 "parser.cpp"
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
#line 4224 "parser.cpp"
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
#line 4254 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 913 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4263 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 917 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4272 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 923 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4278 "parser.cpp"
    break;

  case 56: /* column_type: TINYINT  */
#line 924 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4284 "parser.cpp"
    break;

  case 57: /* column_type: SMALLINT  */
#line 925 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4290 "parser.cpp"
    break;

  case 58: /* column_type: INTEGER  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4296 "parser.cpp"
    break;

  case 59: /* column_type: INT  */
#line 927 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4302 "parser.cpp"
    break;

  case 60: /* column_type: BIGINT  */
#line 928 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4308 "parser.cpp"
    break;

  case 61: /* column_type: HUGEINT  */
#line 929 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4314 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT  */
#line 930 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4320 "parser.cpp"
    break;

  case 63: /* column_type: REAL  */
#line 931 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4326 "parser.cpp"
    break;

  case 64: /* column_type: DOUBLE  */
#line 932 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4332 "parser.cpp"
    break;

  case 65: /* column_type: FLOAT16  */
#line 933 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4338 "parser.cpp"
    break;

  case 66: /* column_type: BFLOAT16  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4344 "parser.cpp"
    break;

  case 67: /* column_type: DATE  */
#line 935 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4350 "parser.cpp"
    break;

  case 68: /* column_type: TIME  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4356 "parser.cpp"
    break;

  case 69: /* column_type: DATETIME  */
#line 937 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4362 "parser.cpp"
    break;

  case 70: /* column_type: TIMESTAMP  */
#line 938 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4368 "parser.cpp"
    break;

  case 71: /* column_type: UUID  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4374 "parser.cpp"
    break;

  case 72: /* column_type: POINT  */
#line 940 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4380 "parser.cpp"
    break;

  case 73: /* column_type: LINE  */
#line 941 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4386 "parser.cpp"
    break;

  case 74: /* column_type: LSEG  */
#line 942 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4392 "parser.cpp"
    break;

  case 75: /* column_type: BOX  */
#line 943 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4398 "parser.cpp"
    break;

  case 76: /* column_type: CIRCLE  */
#line 946 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4404 "parser.cpp"
    break;

  case 77: /* column_type: VARCHAR  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4410 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 949 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4416 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 950 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4422 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL  */
#line 951 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4428 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4434 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4440 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4446 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4452 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4458 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4464 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4470 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4476 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4482 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4488 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4494 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4500 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4506 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4512 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4518 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4524 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4530 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4536 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4542 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4548 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4554 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4560 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4566 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4572 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4578 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4584 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4590 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4596 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4602 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4608 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4614 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4620 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4626 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4632 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4638 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4644 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4650 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4656 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4662 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4668 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4674 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4680 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4686 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4692 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4698 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4704 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4710 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4716 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4722 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4728 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4734 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4740 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4746 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4752 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4758 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4764 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4770 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4776 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4782 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4788 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4794 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4800 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4806 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4812 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4818 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4824 "parser.cpp"
    break;

  case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1020 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4833 "parser.cpp"
    break;

  case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1024 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4843 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraint  */
#line 1047 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4852 "parser.cpp"
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
#line 4866 "parser.cpp"
    break;

  case 151: /* column_constraint: PRIMARY KEY  */
#line 1061 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4874 "parser.cpp"
    break;

  case 152: /* column_constraint: UNIQUE  */
#line 1064 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4882 "parser.cpp"
    break;

  case 153: /* column_constraint: NULLABLE  */
#line 1067 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4890 "parser.cpp"
    break;

  case 154: /* column_constraint: NOT NULLABLE  */
#line 1070 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4898 "parser.cpp"
    break;

  case 155: /* default_expr: DEFAULT constant_expr  */
#line 1074 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4906 "parser.cpp"
    break;

  case 156: /* default_expr: %empty  */
#line 1077 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4914 "parser.cpp"
    break;

  case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1082 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4924 "parser.cpp"
    break;

  case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1087 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4934 "parser.cpp"
    break;

  case 159: /* identifier_array: IDENTIFIER  */
#line 1094 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4945 "parser.cpp"
    break;

  case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1100 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4956 "parser.cpp"
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
#line 4973 "parser.cpp"
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
#line 5016 "parser.cpp"
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
#line 5036 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1180 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5044 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: %empty  */
#line 1183 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5052 "parser.cpp"
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
#line 5075 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1207 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5085 "parser.cpp"
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
#line 5102 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr  */
#line 1229 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5111 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1233 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5120 "parser.cpp"
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
#line 5132 "parser.cpp"
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
#line 5148 "parser.cpp"
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
#line 5166 "parser.cpp"
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
#line 5184 "parser.cpp"
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
#line 5202 "parser.cpp"
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
#line 5225 "parser.cpp"
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
#line 5283 "parser.cpp"
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
#line 5343 "parser.cpp"
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
#line 5395 "parser.cpp"
    break;

  case 180: /* select_statement: select_without_paren  */
#line 1491 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5403 "parser.cpp"
    break;

  case 181: /* select_statement: select_with_paren  */
#line 1494 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5411 "parser.cpp"
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
#line 5425 "parser.cpp"
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
#line 5439 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1516 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5447 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1519 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5455 "parser.cpp"
    break;

  case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1523 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5464 "parser.cpp"
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
#line 5495 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1555 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5503 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1558 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5511 "parser.cpp"
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
#line 5532 "parser.cpp"
    break;

  case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1580 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5540 "parser.cpp"
    break;

  case 192: /* order_by_clause: %empty  */
#line 1583 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5548 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr  */
#line 1587 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5557 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1591 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5566 "parser.cpp"
    break;

  case 195: /* order_by_expr: expr order_by_type  */
#line 1596 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5576 "parser.cpp"
    break;

  case 196: /* order_by_type: ASC  */
#line 1602 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5584 "parser.cpp"
    break;

  case 197: /* order_by_type: DESC  */
#line 1605 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5592 "parser.cpp"
    break;

  case 198: /* order_by_type: %empty  */
#line 1608 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5600 "parser.cpp"
    break;

  case 199: /* limit_expr: LIMIT expr  */
#line 1612 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5608 "parser.cpp"
    break;

  case 200: /* limit_expr: %empty  */
#line 1616 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5614 "parser.cpp"
    break;

  case 201: /* offset_expr: OFFSET expr  */
#line 1618 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5622 "parser.cpp"
    break;

  case 202: /* offset_expr: %empty  */
#line 1622 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5628 "parser.cpp"
    break;

  case 203: /* distinct: DISTINCT  */
#line 1624 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5636 "parser.cpp"
    break;

  case 204: /* distinct: %empty  */
#line 1627 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5644 "parser.cpp"
    break;

  case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1631 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5652 "parser.cpp"
    break;

  case 206: /* highlight_clause: %empty  */
#line 1634 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5660 "parser.cpp"
    break;

  case 207: /* from_clause: FROM table_reference  */
#line 1638 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5668 "parser.cpp"
    break;

  case 208: /* from_clause: %empty  */
#line 1641 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5676 "parser.cpp"
    break;

  case 209: /* search_clause: SEARCH sub_search_array  */
#line 1645 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5686 "parser.cpp"
    break;

  case 210: /* search_clause: %empty  */
#line 1650 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5694 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1654 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5702 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: %empty  */
#line 1657 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5710 "parser.cpp"
    break;

  case 213: /* where_clause: WHERE expr  */
#line 1661 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5718 "parser.cpp"
    break;

  case 214: /* where_clause: %empty  */
#line 1664 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5726 "parser.cpp"
    break;

  case 215: /* having_clause: HAVING expr  */
#line 1668 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5734 "parser.cpp"
    break;

  case 216: /* having_clause: %empty  */
#line 1671 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5742 "parser.cpp"
    break;

  case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1675 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5750 "parser.cpp"
    break;

  case 218: /* group_by_clause: %empty  */
#line 1678 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5758 "parser.cpp"
    break;

  case 219: /* set_operator: UNION  */
#line 1682 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5766 "parser.cpp"
    break;

  case 220: /* set_operator: UNION ALL  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5774 "parser.cpp"
    break;

  case 221: /* set_operator: INTERSECT  */
#line 1688 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5782 "parser.cpp"
    break;

  case 222: /* set_operator: EXCEPT  */
#line 1691 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5790 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference_unit  */
#line 1699 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5798 "parser.cpp"
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
#line 5816 "parser.cpp"
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
#line 5834 "parser.cpp"
    break;

  case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1733 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5845 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER  */
#line 1742 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5855 "parser.cpp"
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
#line 5867 "parser.cpp"
    break;

  case 231: /* table_alias: AS IDENTIFIER  */
#line 1756 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5877 "parser.cpp"
    break;

  case 232: /* table_alias: IDENTIFIER  */
#line 1761 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5887 "parser.cpp"
    break;

  case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1766 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5898 "parser.cpp"
    break;

  case 234: /* table_alias: %empty  */
#line 1772 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5906 "parser.cpp"
    break;

  case 235: /* with_clause: WITH with_expr_list  */
#line 1779 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5914 "parser.cpp"
    break;

  case 236: /* with_clause: %empty  */
#line 1782 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5922 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr  */
#line 1786 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5931 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1789 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5940 "parser.cpp"
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
#line 5952 "parser.cpp"
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
#line 5964 "parser.cpp"
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
#line 5977 "parser.cpp"
    break;

  case 242: /* join_type: INNER  */
#line 1827 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5985 "parser.cpp"
    break;

  case 243: /* join_type: LEFT  */
#line 1830 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5993 "parser.cpp"
    break;

  case 244: /* join_type: RIGHT  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6001 "parser.cpp"
    break;

  case 245: /* join_type: OUTER  */
#line 1836 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6009 "parser.cpp"
    break;

  case 246: /* join_type: FULL  */
#line 1839 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6017 "parser.cpp"
    break;

  case 247: /* join_type: CROSS  */
#line 1842 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6025 "parser.cpp"
    break;

  case 248: /* join_type: %empty  */
#line 1845 "parser.y"
                {
}
#line 6032 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW DATABASES  */
#line 1851 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6041 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLES  */
#line 1855 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6050 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TASKS  */
#line 1859 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6059 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIGS  */
#line 1863 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6068 "parser.cpp"
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
#line 6080 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW PROFILES  */
#line 1874 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6089 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW BUFFER  */
#line 1878 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6098 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW MEMINDEX  */
#line 1882 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6107 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERIES  */
#line 1886 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6116 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1890 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6126 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1895 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6135 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1899 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6145 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1904 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6154 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1908 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6163 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1912 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6172 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1916 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6183 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1922 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6194 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1928 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6206 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name  */
#line 1935 "parser.y"
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
#line 6222 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1946 "parser.y"
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
#line 6238 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1957 "parser.y"
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
#line 6254 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1968 "parser.y"
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
#line 6271 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1980 "parser.y"
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
#line 6288 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1992 "parser.y"
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
#line 6306 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2005 "parser.y"
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
#line 6325 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2019 "parser.y"
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
#line 6341 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2030 "parser.y"
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

    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->index_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6361 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2045 "parser.y"
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

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.show_stmt)->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
}
#line 6383 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2062 "parser.y"
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

      ParserHelper::ToLower((yyvsp[-4].str_value));
      (yyval.show_stmt)->index_name_ = (yyvsp[-4].str_value);
      free((yyvsp[-4].str_value));

      (yyval.show_stmt)->segment_id_ = (yyvsp[-2].long_value);
      (yyval.show_stmt)->chunk_id_ = (yyvsp[0].long_value);
}
#line 6406 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW LOGS  */
#line 2080 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6415 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOG  */
#line 2084 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6424 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG TO file_path  */
#line 2088 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6435 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2094 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6444 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2098 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6453 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2102 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6464 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW MEMORY  */
#line 2108 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6473 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2112 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6482 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2116 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6491 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2120 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      ParserHelper::ToLower((yyvsp[-2].str_value));
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6503 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW SNAPSHOTS  */
#line 2127 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6512 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2131 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6524 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW CACHES  */
#line 2138 "parser.y"
              {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListCaches;
}
#line 6533 "parser.cpp"
    break;

  case 291: /* show_statement: SHOW CACHE  */
#line 2142 "parser.y"
             {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowCache;
}
#line 6542 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH DATA  */
#line 2150 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6551 "parser.cpp"
    break;

  case 293: /* flush_statement: FLUSH LOG  */
#line 2154 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6560 "parser.cpp"
    break;

  case 294: /* flush_statement: FLUSH BUFFER  */
#line 2158 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6569 "parser.cpp"
    break;

  case 295: /* flush_statement: FLUSH CATALOG  */
#line 2162 "parser.y"
                {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kCatalog;
}
#line 6578 "parser.cpp"
    break;

  case 296: /* optimize_statement: OPTIMIZE table_name  */
#line 2170 "parser.y"
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
#line 6593 "parser.cpp"
    break;

  case 297: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2181 "parser.y"
                                                         {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    if((yyvsp[-1].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[-1].table_name_t)->schema_name_ptr_;
        free((yyvsp[-1].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[-1].table_name_t)->table_name_ptr_;
    free((yyvsp[-1].table_name_t)->table_name_ptr_);
    delete (yyvsp[-1].table_name_t);

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.optimize_stmt)->index_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));

    for (auto *&index_param : *(yyvsp[0].with_index_param_list_t)) {
        (yyval.optimize_stmt)->opt_params_.emplace_back(std::unique_ptr<infinity::InitParameter>(index_param));
        index_param = nullptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
}
#line 6618 "parser.cpp"
    break;

  case 298: /* command_statement: USE IDENTIFIER  */
#line 2205 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6629 "parser.cpp"
    break;

  case 299: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2211 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6639 "parser.cpp"
    break;

  case 300: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2216 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6650 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2222 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6661 "parser.cpp"
    break;

  case 302: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2228 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6674 "parser.cpp"
    break;

  case 303: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2236 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6685 "parser.cpp"
    break;

  case 304: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2242 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6696 "parser.cpp"
    break;

  case 305: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2248 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6707 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2254 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6718 "parser.cpp"
    break;

  case 307: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2260 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6731 "parser.cpp"
    break;

  case 308: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2268 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6742 "parser.cpp"
    break;

  case 309: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2274 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6753 "parser.cpp"
    break;

  case 310: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2280 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6764 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2286 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6775 "parser.cpp"
    break;

  case 312: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2292 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6788 "parser.cpp"
    break;

  case 313: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2300 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6799 "parser.cpp"
    break;

  case 314: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2306 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6810 "parser.cpp"
    break;

  case 315: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2312 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6823 "parser.cpp"
    break;

  case 316: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2320 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6836 "parser.cpp"
    break;

  case 317: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2328 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6847 "parser.cpp"
    break;

  case 318: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2334 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6858 "parser.cpp"
    break;

  case 319: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2340 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6869 "parser.cpp"
    break;

  case 320: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2346 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6880 "parser.cpp"
    break;

  case 321: /* command_statement: CLEAN DATA  */
#line 2352 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 6889 "parser.cpp"
    break;

  case 322: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2356 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::DumpIndexCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_, (yyvsp[-2].str_value));
    free((yyvsp[-2].str_value));
    if ((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6905 "parser.cpp"
    break;

  case 323: /* compact_statement: COMPACT TABLE table_name  */
#line 2368 "parser.y"
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
#line 6922 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2381 "parser.y"
                                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6933 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2387 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6945 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2394 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6958 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2402 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6972 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2411 "parser.y"
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
#line 6987 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2421 "parser.y"
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
#line 7002 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2431 "parser.y"
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
#line 7018 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2442 "parser.y"
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
#line 7034 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2453 "parser.y"
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
#line 7051 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2465 "parser.y"
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
#line 7068 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2477 "parser.y"
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
#line 7083 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2487 "parser.y"
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
#line 7099 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2498 "parser.y"
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
#line 7116 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2510 "parser.y"
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
#line 7134 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOGS  */
#line 2523 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7143 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2527 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7153 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2532 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7163 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2537 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7174 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2543 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7183 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2547 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7192 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2551 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7204 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2558 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7213 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2562 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7222 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2566 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     ParserHelper::ToLower((yyvsp[0].str_value));
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7234 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2573 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7245 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2579 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7258 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2587 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7269 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODES  */
#line 2593 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7278 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2597 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7289 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW NODE  */
#line 2603 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7298 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2607 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7309 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET ADMIN  */
#line 2613 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7319 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET STANDALONE  */
#line 2618 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7329 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2623 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7341 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2630 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7355 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2639 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7369 "parser.cpp"
    break;

  case 360: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2649 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ParserHelper::ToLower((yyvsp[0].str_value));
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7384 "parser.cpp"
    break;

  case 361: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2659 "parser.y"
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
#line 7401 "parser.cpp"
    break;

  case 362: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2671 "parser.y"
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
#line 7417 "parser.cpp"
    break;

  case 363: /* check_statement: CHECK SYSTEM  */
#line 2683 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7426 "parser.cpp"
    break;

  case 364: /* check_statement: CHECK TABLE table_name  */
#line 2687 "parser.y"
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
#line 7442 "parser.cpp"
    break;

  case 365: /* expr_array: expr_alias  */
#line 2703 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7451 "parser.cpp"
    break;

  case 366: /* expr_array: expr_array ',' expr_alias  */
#line 2707 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7460 "parser.cpp"
    break;

  case 367: /* insert_row_list: '(' expr_array ')'  */
#line 2712 "parser.y"
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
#line 7475 "parser.cpp"
    break;

  case 368: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2722 "parser.y"
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
#line 7490 "parser.cpp"
    break;

  case 369: /* expr_alias: expr AS IDENTIFIER  */
#line 2744 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7501 "parser.cpp"
    break;

  case 370: /* expr_alias: expr  */
#line 2750 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7509 "parser.cpp"
    break;

  case 376: /* operand: '(' expr ')'  */
#line 2760 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7517 "parser.cpp"
    break;

  case 377: /* operand: '(' select_without_paren ')'  */
#line 2763 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7528 "parser.cpp"
    break;

  case 378: /* operand: constant_expr  */
#line 2769 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7536 "parser.cpp"
    break;

  case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2785 "parser.y"
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
#line 7556 "parser.cpp"
    break;

  case 390: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2801 "parser.y"
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
    ParserHelper::ToLower((yyvsp[-1].str_value));
    match_tensor_expr->index_name_ = (yyvsp[-1].str_value);
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 7578 "parser.cpp"
    break;

  case 391: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2819 "parser.y"
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
#line 7599 "parser.cpp"
    break;

  case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2837 "parser.y"
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

    ParserHelper::ToLower((yyvsp[-2].str_value));
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
#line 7661 "parser.cpp"
    break;

  case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2895 "parser.y"
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
#line 7715 "parser.cpp"
    break;

  case 394: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2945 "parser.y"
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
#line 7773 "parser.cpp"
    break;

  case 395: /* match_vector_expr: MATCH VECTOR '(' expr ',' expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2999 "parser.y"
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
#line 7832 "parser.cpp"
    break;

  case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3057 "parser.y"
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

    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
}
#line 7861 "parser.cpp"
    break;

  case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3082 "parser.y"
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
#line 7888 "parser.cpp"
    break;

  case 398: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3105 "parser.y"
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
#line 7913 "parser.cpp"
    break;

  case 399: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3126 "parser.y"
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
#line 7938 "parser.cpp"
    break;

  case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3147 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7952 "parser.cpp"
    break;

  case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3156 "parser.y"
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
#line 7968 "parser.cpp"
    break;

  case 402: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3167 "parser.y"
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
#line 7984 "parser.cpp"
    break;

  case 403: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3178 "parser.y"
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
#line 8002 "parser.cpp"
    break;

  case 404: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3192 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8014 "parser.cpp"
    break;

  case 405: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3199 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 8028 "parser.cpp"
    break;

  case 406: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3209 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 8039 "parser.cpp"
    break;

  case 407: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3215 "parser.y"
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
#line 8055 "parser.cpp"
    break;

  case 408: /* sub_search: match_vector_expr  */
#line 3227 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8063 "parser.cpp"
    break;

  case 409: /* sub_search: match_text_expr  */
#line 3230 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8071 "parser.cpp"
    break;

  case 410: /* sub_search: match_tensor_expr  */
#line 3233 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8079 "parser.cpp"
    break;

  case 411: /* sub_search: match_sparse_expr  */
#line 3236 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8087 "parser.cpp"
    break;

  case 412: /* sub_search: query_expr  */
#line 3239 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8095 "parser.cpp"
    break;

  case 413: /* sub_search: fusion_expr  */
#line 3242 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8103 "parser.cpp"
    break;

  case 414: /* sub_search_array: sub_search  */
#line 3246 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8112 "parser.cpp"
    break;

  case 415: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3250 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8121 "parser.cpp"
    break;

  case 416: /* function_expr: IDENTIFIER '(' ')'  */
#line 3255 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8134 "parser.cpp"
    break;

  case 417: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3263 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8147 "parser.cpp"
    break;

  case 418: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3271 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8161 "parser.cpp"
    break;

  case 419: /* function_expr: YEAR '(' expr ')'  */
#line 3280 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8173 "parser.cpp"
    break;

  case 420: /* function_expr: MONTH '(' expr ')'  */
#line 3287 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8185 "parser.cpp"
    break;

  case 421: /* function_expr: DAY '(' expr ')'  */
#line 3294 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8197 "parser.cpp"
    break;

  case 422: /* function_expr: HOUR '(' expr ')'  */
#line 3301 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8209 "parser.cpp"
    break;

  case 423: /* function_expr: MINUTE '(' expr ')'  */
#line 3308 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8221 "parser.cpp"
    break;

  case 424: /* function_expr: SECOND '(' expr ')'  */
#line 3315 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8233 "parser.cpp"
    break;

  case 425: /* function_expr: operand IS NOT NULLABLE  */
#line 3322 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8245 "parser.cpp"
    break;

  case 426: /* function_expr: operand IS NULLABLE  */
#line 3329 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8257 "parser.cpp"
    break;

  case 427: /* function_expr: NOT operand  */
#line 3336 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8269 "parser.cpp"
    break;

  case 428: /* function_expr: '-' operand  */
#line 3343 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8281 "parser.cpp"
    break;

  case 429: /* function_expr: '+' operand  */
#line 3350 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8293 "parser.cpp"
    break;

  case 430: /* function_expr: operand '-' operand  */
#line 3357 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8306 "parser.cpp"
    break;

  case 431: /* function_expr: operand '+' operand  */
#line 3365 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8319 "parser.cpp"
    break;

  case 432: /* function_expr: operand '*' operand  */
#line 3373 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8332 "parser.cpp"
    break;

  case 433: /* function_expr: operand '/' operand  */
#line 3381 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8345 "parser.cpp"
    break;

  case 434: /* function_expr: operand '%' operand  */
#line 3389 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8358 "parser.cpp"
    break;

  case 435: /* function_expr: operand '=' operand  */
#line 3397 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8371 "parser.cpp"
    break;

  case 436: /* function_expr: operand EQUAL operand  */
#line 3405 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8384 "parser.cpp"
    break;

  case 437: /* function_expr: operand NOT_EQ operand  */
#line 3413 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8397 "parser.cpp"
    break;

  case 438: /* function_expr: operand '<' operand  */
#line 3421 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8410 "parser.cpp"
    break;

  case 439: /* function_expr: operand '>' operand  */
#line 3429 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8423 "parser.cpp"
    break;

  case 440: /* function_expr: operand LESS_EQ operand  */
#line 3437 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8436 "parser.cpp"
    break;

  case 441: /* function_expr: operand GREATER_EQ operand  */
#line 3445 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8449 "parser.cpp"
    break;

  case 442: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3453 "parser.y"
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
#line 8484 "parser.cpp"
    break;

  case 443: /* function_expr: operand LIKE operand  */
#line 3483 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8497 "parser.cpp"
    break;

  case 444: /* function_expr: operand NOT LIKE operand  */
#line 3491 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8510 "parser.cpp"
    break;

  case 445: /* conjunction_expr: expr AND expr  */
#line 3500 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8523 "parser.cpp"
    break;

  case 446: /* conjunction_expr: expr OR expr  */
#line 3508 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8536 "parser.cpp"
    break;

  case 447: /* between_expr: operand BETWEEN operand AND operand  */
#line 3517 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8548 "parser.cpp"
    break;

  case 448: /* in_expr: operand IN '(' expr_array ')'  */
#line 3525 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8559 "parser.cpp"
    break;

  case 449: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3531 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8570 "parser.cpp"
    break;

  case 450: /* case_expr: CASE expr case_check_array END  */
#line 3538 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8581 "parser.cpp"
    break;

  case 451: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3544 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8593 "parser.cpp"
    break;

  case 452: /* case_expr: CASE case_check_array END  */
#line 3551 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8603 "parser.cpp"
    break;

  case 453: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3556 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8614 "parser.cpp"
    break;

  case 454: /* case_check_array: WHEN expr THEN expr  */
#line 3563 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8626 "parser.cpp"
    break;

  case 455: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3570 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8638 "parser.cpp"
    break;

  case 456: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3578 "parser.y"
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
#line 8655 "parser.cpp"
    break;

  case 457: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3591 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8666 "parser.cpp"
    break;

  case 458: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3597 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8677 "parser.cpp"
    break;

  case 459: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3603 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8689 "parser.cpp"
    break;

  case 460: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3610 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8701 "parser.cpp"
    break;

  case 461: /* column_expr: IDENTIFIER  */
#line 3618 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8713 "parser.cpp"
    break;

  case 462: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3625 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8725 "parser.cpp"
    break;

  case 463: /* column_expr: '*'  */
#line 3632 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8735 "parser.cpp"
    break;

  case 464: /* column_expr: column_expr '.' '*'  */
#line 3637 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8749 "parser.cpp"
    break;

  case 465: /* constant_expr: STRING  */
#line 3647 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8759 "parser.cpp"
    break;

  case 466: /* constant_expr: TRUE  */
#line 3652 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8769 "parser.cpp"
    break;

  case 467: /* constant_expr: FALSE  */
#line 3657 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8779 "parser.cpp"
    break;

  case 468: /* constant_expr: DOUBLE_VALUE  */
#line 3662 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8789 "parser.cpp"
    break;

  case 469: /* constant_expr: LONG_VALUE  */
#line 3667 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8799 "parser.cpp"
    break;

  case 470: /* constant_expr: DATE STRING  */
#line 3672 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8809 "parser.cpp"
    break;

  case 471: /* constant_expr: TIME STRING  */
#line 3677 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8819 "parser.cpp"
    break;

  case 472: /* constant_expr: DATETIME STRING  */
#line 3682 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8829 "parser.cpp"
    break;

  case 473: /* constant_expr: TIMESTAMP STRING  */
#line 3687 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8839 "parser.cpp"
    break;

  case 474: /* constant_expr: INTERVAL interval_expr  */
#line 3692 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8847 "parser.cpp"
    break;

  case 475: /* constant_expr: interval_expr  */
#line 3695 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8855 "parser.cpp"
    break;

  case 476: /* constant_expr: common_array_expr  */
#line 3698 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8863 "parser.cpp"
    break;

  case 477: /* constant_expr: curly_brackets_expr  */
#line 3701 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8871 "parser.cpp"
    break;

  case 478: /* common_array_expr: array_expr  */
#line 3705 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8879 "parser.cpp"
    break;

  case 479: /* common_array_expr: subarray_array_expr  */
#line 3708 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8887 "parser.cpp"
    break;

  case 480: /* common_array_expr: sparse_array_expr  */
#line 3711 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8895 "parser.cpp"
    break;

  case 481: /* common_array_expr: empty_array_expr  */
#line 3714 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8903 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: sparse_array_expr  */
#line 3718 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8911 "parser.cpp"
    break;

  case 483: /* common_sparse_array_expr: array_expr  */
#line 3721 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8919 "parser.cpp"
    break;

  case 484: /* common_sparse_array_expr: empty_array_expr  */
#line 3724 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8927 "parser.cpp"
    break;

  case 485: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3728 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8935 "parser.cpp"
    break;

  case 486: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3732 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8945 "parser.cpp"
    break;

  case 487: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3737 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8954 "parser.cpp"
    break;

  case 488: /* sparse_array_expr: long_sparse_array_expr  */
#line 3742 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8962 "parser.cpp"
    break;

  case 489: /* sparse_array_expr: double_sparse_array_expr  */
#line 3745 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8970 "parser.cpp"
    break;

  case 490: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3749 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8978 "parser.cpp"
    break;

  case 491: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3753 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8990 "parser.cpp"
    break;

  case 492: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3760 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9001 "parser.cpp"
    break;

  case 493: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3767 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9009 "parser.cpp"
    break;

  case 494: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3771 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 9021 "parser.cpp"
    break;

  case 495: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3778 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9032 "parser.cpp"
    break;

  case 496: /* empty_array_expr: '[' ']'  */
#line 3785 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 9040 "parser.cpp"
    break;

  case 497: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3789 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9048 "parser.cpp"
    break;

  case 498: /* curly_brackets_expr: '{' '}'  */
#line 3792 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 9056 "parser.cpp"
    break;

  case 499: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3796 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9065 "parser.cpp"
    break;

  case 500: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3800 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9074 "parser.cpp"
    break;

  case 501: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3805 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9082 "parser.cpp"
    break;

  case 502: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3809 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9090 "parser.cpp"
    break;

  case 503: /* array_expr: long_array_expr  */
#line 3813 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9098 "parser.cpp"
    break;

  case 504: /* array_expr: double_array_expr  */
#line 3816 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9106 "parser.cpp"
    break;

  case 505: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3820 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9114 "parser.cpp"
    break;

  case 506: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3824 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9124 "parser.cpp"
    break;

  case 507: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3829 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9133 "parser.cpp"
    break;

  case 508: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3834 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9141 "parser.cpp"
    break;

  case 509: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3838 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9151 "parser.cpp"
    break;

  case 510: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3843 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9160 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE SECONDS  */
#line 3848 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9171 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE SECOND  */
#line 3854 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9182 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE MINUTES  */
#line 3860 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9193 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE MINUTE  */
#line 3866 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9204 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE HOURS  */
#line 3872 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9215 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE HOUR  */
#line 3878 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9226 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE DAYS  */
#line 3884 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9237 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE DAY  */
#line 3890 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9248 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE MONTHS  */
#line 3896 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9259 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE MONTH  */
#line 3902 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9270 "parser.cpp"
    break;

  case 521: /* interval_expr: LONG_VALUE YEARS  */
#line 3908 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9281 "parser.cpp"
    break;

  case 522: /* interval_expr: LONG_VALUE YEAR  */
#line 3914 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9292 "parser.cpp"
    break;

  case 523: /* copy_option_list: copy_option  */
#line 3925 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9301 "parser.cpp"
    break;

  case 524: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3929 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9310 "parser.cpp"
    break;

  case 525: /* copy_option: FORMAT IDENTIFIER  */
#line 3934 "parser.y"
                                {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    ParserHelper::ToLower((yyvsp[0].str_value));
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
#line 9347 "parser.cpp"
    break;

  case 526: /* copy_option: DELIMITER STRING  */
#line 3966 "parser.y"
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
#line 9362 "parser.cpp"
    break;

  case 527: /* copy_option: HEADER  */
#line 3976 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9372 "parser.cpp"
    break;

  case 528: /* copy_option: OFFSET LONG_VALUE  */
#line 3981 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9382 "parser.cpp"
    break;

  case 529: /* copy_option: LIMIT LONG_VALUE  */
#line 3986 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9392 "parser.cpp"
    break;

  case 530: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3991 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9402 "parser.cpp"
    break;

  case 531: /* file_path: STRING  */
#line 3997 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9410 "parser.cpp"
    break;

  case 532: /* if_exists: IF EXISTS  */
#line 4001 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9416 "parser.cpp"
    break;

  case 533: /* if_exists: %empty  */
#line 4002 "parser.y"
  { (yyval.bool_value) = false; }
#line 9422 "parser.cpp"
    break;

  case 534: /* if_not_exists: IF NOT EXISTS  */
#line 4004 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9428 "parser.cpp"
    break;

  case 535: /* if_not_exists: %empty  */
#line 4005 "parser.y"
  { (yyval.bool_value) = false; }
#line 9434 "parser.cpp"
    break;

  case 538: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 4020 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9447 "parser.cpp"
    break;

  case 539: /* if_not_exists_info: %empty  */
#line 4028 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9455 "parser.cpp"
    break;

  case 540: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 4032 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9463 "parser.cpp"
    break;

  case 541: /* with_index_param_list: %empty  */
#line 4035 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9471 "parser.cpp"
    break;

  case 542: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 4039 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9479 "parser.cpp"
    break;

  case 543: /* optional_table_properties_list: %empty  */
#line 4042 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9487 "parser.cpp"
    break;

  case 544: /* index_param_list: index_param  */
#line 4046 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9496 "parser.cpp"
    break;

  case 545: /* index_param_list: index_param_list ',' index_param  */
#line 4050 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9505 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER  */
#line 4055 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9516 "parser.cpp"
    break;

  case 547: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4061 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9531 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' STRING  */
#line 4071 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9546 "parser.cpp"
    break;

  case 549: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4081 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9559 "parser.cpp"
    break;

  case 550: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4089 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9572 "parser.cpp"
    break;

  case 551: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4100 "parser.y"
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
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-4].str_value);
    (yyval.index_info_t)->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-4].str_value));
}
#line 9609 "parser.cpp"
    break;

  case 552: /* index_info: '(' IDENTIFIER ')'  */
#line 4132 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9621 "parser.cpp"
    break;


#line 9625 "parser.cpp"

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

#line 4140 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

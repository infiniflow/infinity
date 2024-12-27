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
  YYSYMBOL_EMBEDDING = 119,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 120,                   /* VECTOR  */
  YYSYMBOL_BIT = 121,                      /* BIT  */
  YYSYMBOL_TEXT = 122,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 123,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 124,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 125,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 126,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 127,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 128,                  /* PRIMARY  */
  YYSYMBOL_KEY = 129,                      /* KEY  */
  YYSYMBOL_UNIQUE = 130,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 131,                 /* NULLABLE  */
  YYSYMBOL_IS = 132,                       /* IS  */
  YYSYMBOL_DEFAULT = 133,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 134,                  /* COMMENT  */
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
  YYSYMBOL_DELTA = 175,                    /* DELTA  */
  YYSYMBOL_LOGS = 176,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 177,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 178,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 179,                   /* SEARCH  */
  YYSYMBOL_MATCH = 180,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 181,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 182,                    /* QUERY  */
  YYSYMBOL_QUERIES = 183,                  /* QUERIES  */
  YYSYMBOL_FUSION = 184,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 185,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 186,                    /* ADMIN  */
  YYSYMBOL_LEADER = 187,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 188,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 189,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 190,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 191,               /* STANDALONE  */
  YYSYMBOL_NODES = 192,                    /* NODES  */
  YYSYMBOL_NODE = 193,                     /* NODE  */
  YYSYMBOL_REMOVE = 194,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 195,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 196,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 197,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 198,                  /* RESTORE  */
  YYSYMBOL_PERSISTENCE = 199,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 200,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 201,                  /* OBJECTS  */
  YYSYMBOL_FILES = 202,                    /* FILES  */
  YYSYMBOL_MEMORY = 203,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 204,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 205,                   /* NUMBER  */
  YYSYMBOL_206_ = 206,                     /* '='  */
  YYSYMBOL_207_ = 207,                     /* '<'  */
  YYSYMBOL_208_ = 208,                     /* '>'  */
  YYSYMBOL_209_ = 209,                     /* '+'  */
  YYSYMBOL_210_ = 210,                     /* '-'  */
  YYSYMBOL_211_ = 211,                     /* '*'  */
  YYSYMBOL_212_ = 212,                     /* '/'  */
  YYSYMBOL_213_ = 213,                     /* '%'  */
  YYSYMBOL_214_ = 214,                     /* '['  */
  YYSYMBOL_215_ = 215,                     /* ']'  */
  YYSYMBOL_216_ = 216,                     /* '('  */
  YYSYMBOL_217_ = 217,                     /* ')'  */
  YYSYMBOL_218_ = 218,                     /* '.'  */
  YYSYMBOL_219_ = 219,                     /* ';'  */
  YYSYMBOL_220_ = 220,                     /* ','  */
  YYSYMBOL_221_ = 221,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 222,                 /* $accept  */
  YYSYMBOL_input_pattern = 223,            /* input_pattern  */
  YYSYMBOL_statement_list = 224,           /* statement_list  */
  YYSYMBOL_statement = 225,                /* statement  */
  YYSYMBOL_explainable_statement = 226,    /* explainable_statement  */
  YYSYMBOL_create_statement = 227,         /* create_statement  */
  YYSYMBOL_table_element_array = 228,      /* table_element_array  */
  YYSYMBOL_column_def_array = 229,         /* column_def_array  */
  YYSYMBOL_table_element = 230,            /* table_element  */
  YYSYMBOL_table_column = 231,             /* table_column  */
  YYSYMBOL_column_type = 232,              /* column_type  */
  YYSYMBOL_column_constraints = 233,       /* column_constraints  */
  YYSYMBOL_column_constraint = 234,        /* column_constraint  */
  YYSYMBOL_default_expr = 235,             /* default_expr  */
  YYSYMBOL_table_constraint = 236,         /* table_constraint  */
  YYSYMBOL_identifier_array = 237,         /* identifier_array  */
  YYSYMBOL_delete_statement = 238,         /* delete_statement  */
  YYSYMBOL_insert_statement = 239,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 240, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 241,        /* explain_statement  */
  YYSYMBOL_update_statement = 242,         /* update_statement  */
  YYSYMBOL_update_expr_array = 243,        /* update_expr_array  */
  YYSYMBOL_update_expr = 244,              /* update_expr  */
  YYSYMBOL_drop_statement = 245,           /* drop_statement  */
  YYSYMBOL_copy_statement = 246,           /* copy_statement  */
  YYSYMBOL_select_statement = 247,         /* select_statement  */
  YYSYMBOL_select_with_paren = 248,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 249,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 250, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 251, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 252, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 253,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 254,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 255,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 256,            /* order_by_type  */
  YYSYMBOL_limit_expr = 257,               /* limit_expr  */
  YYSYMBOL_offset_expr = 258,              /* offset_expr  */
  YYSYMBOL_distinct = 259,                 /* distinct  */
  YYSYMBOL_unnest_clause = 260,            /* unnest_clause  */
  YYSYMBOL_highlight_clause = 261,         /* highlight_clause  */
  YYSYMBOL_from_clause = 262,              /* from_clause  */
  YYSYMBOL_search_clause = 263,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 264, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 265,             /* where_clause  */
  YYSYMBOL_having_clause = 266,            /* having_clause  */
  YYSYMBOL_group_by_clause = 267,          /* group_by_clause  */
  YYSYMBOL_set_operator = 268,             /* set_operator  */
  YYSYMBOL_table_reference = 269,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 270,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 271,     /* table_reference_name  */
  YYSYMBOL_table_name = 272,               /* table_name  */
  YYSYMBOL_table_alias = 273,              /* table_alias  */
  YYSYMBOL_with_clause = 274,              /* with_clause  */
  YYSYMBOL_with_expr_list = 275,           /* with_expr_list  */
  YYSYMBOL_with_expr = 276,                /* with_expr  */
  YYSYMBOL_join_clause = 277,              /* join_clause  */
  YYSYMBOL_join_type = 278,                /* join_type  */
  YYSYMBOL_show_statement = 279,           /* show_statement  */
  YYSYMBOL_flush_statement = 280,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 281,       /* optimize_statement  */
  YYSYMBOL_command_statement = 282,        /* command_statement  */
  YYSYMBOL_compact_statement = 283,        /* compact_statement  */
  YYSYMBOL_admin_statement = 284,          /* admin_statement  */
  YYSYMBOL_alter_statement = 285,          /* alter_statement  */
  YYSYMBOL_expr_array = 286,               /* expr_array  */
  YYSYMBOL_insert_row_list = 287,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 288,               /* expr_alias  */
  YYSYMBOL_expr = 289,                     /* expr  */
  YYSYMBOL_operand = 290,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 291,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 292,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 293,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 294,          /* match_text_expr  */
  YYSYMBOL_query_expr = 295,               /* query_expr  */
  YYSYMBOL_fusion_expr = 296,              /* fusion_expr  */
  YYSYMBOL_sub_search = 297,               /* sub_search  */
  YYSYMBOL_sub_search_array = 298,         /* sub_search_array  */
  YYSYMBOL_function_expr = 299,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 300,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 301,             /* between_expr  */
  YYSYMBOL_in_expr = 302,                  /* in_expr  */
  YYSYMBOL_case_expr = 303,                /* case_expr  */
  YYSYMBOL_case_check_array = 304,         /* case_check_array  */
  YYSYMBOL_cast_expr = 305,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 306,            /* subquery_expr  */
  YYSYMBOL_column_expr = 307,              /* column_expr  */
  YYSYMBOL_constant_expr = 308,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 309,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 310, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 311,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 312, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 313,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 314,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 315, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 316, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 317, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 318,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 319,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 320,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 321,               /* array_expr  */
  YYSYMBOL_long_array_expr = 322,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 323, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 324,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 325, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 326,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 327,         /* copy_option_list  */
  YYSYMBOL_copy_option = 328,              /* copy_option  */
  YYSYMBOL_file_path = 329,                /* file_path  */
  YYSYMBOL_if_exists = 330,                /* if_exists  */
  YYSYMBOL_if_not_exists = 331,            /* if_not_exists  */
  YYSYMBOL_semicolon = 332,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 333,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 334,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 335, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 336,         /* index_param_list  */
  YYSYMBOL_index_param = 337,              /* index_param  */
  YYSYMBOL_index_info = 338                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 467 "parser.cpp"

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
#define YYLAST   1475

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  222
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  117
/* YYNRULES -- Number of rules.  */
#define YYNRULES  534
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1208

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   460


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
       2,     2,     2,     2,     2,     2,     2,   213,     2,     2,
     216,   217,   211,   209,   220,   210,   218,   212,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   221,   219,
     207,   206,   208,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   214,     2,   215,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   503,   503,   507,   513,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   554,   571,   588,   604,   633,   648,   680,   698,
     716,   744,   775,   779,   784,   788,   794,   797,   804,   855,
     892,   944,   984,   985,   986,   987,   988,   989,   990,   991,
     992,   993,   994,   995,   996,   997,   998,   999,  1000,  1001,
    1002,  1003,  1004,  1007,  1009,  1010,  1011,  1012,  1015,  1016,
    1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,  1029,  1030,  1031,  1032,  1033,  1034,  1035,  1036,
    1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,
    1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,  1055,  1056,
    1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,
    1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,  1075,  1076,
    1077,  1078,  1079,  1080,  1099,  1103,  1113,  1116,  1119,  1122,
    1126,  1129,  1134,  1139,  1146,  1152,  1162,  1178,  1216,  1232,
    1235,  1242,  1254,  1263,  1276,  1280,  1285,  1298,  1311,  1326,
    1341,  1356,  1379,  1432,  1487,  1538,  1541,  1544,  1553,  1563,
    1566,  1570,  1575,  1602,  1605,  1610,  1628,  1631,  1635,  1639,
    1644,  1650,  1653,  1656,  1660,  1664,  1666,  1670,  1672,  1675,
    1679,  1682,  1686,  1689,  1693,  1696,  1700,  1705,  1709,  1712,
    1716,  1719,  1723,  1726,  1730,  1733,  1737,  1740,  1743,  1746,
    1754,  1757,  1772,  1772,  1774,  1788,  1797,  1802,  1811,  1816,
    1821,  1827,  1834,  1837,  1841,  1844,  1849,  1861,  1868,  1882,
    1885,  1888,  1891,  1894,  1897,  1900,  1906,  1910,  1914,  1918,
    1922,  1929,  1933,  1937,  1941,  1945,  1950,  1954,  1959,  1963,
    1967,  1973,  1979,  1985,  1996,  2007,  2018,  2030,  2042,  2055,
    2069,  2080,  2094,  2110,  2127,  2131,  2135,  2139,  2143,  2147,
    2153,  2157,  2161,  2165,  2171,  2175,  2185,  2189,  2193,  2201,
    2212,  2235,  2241,  2246,  2252,  2258,  2266,  2272,  2278,  2284,
    2290,  2298,  2304,  2310,  2316,  2322,  2330,  2336,  2342,  2351,
    2360,  2366,  2372,  2378,  2384,  2390,  2397,  2410,  2414,  2419,
    2425,  2432,  2440,  2449,  2459,  2469,  2480,  2491,  2503,  2515,
    2525,  2536,  2548,  2561,  2565,  2570,  2575,  2581,  2585,  2589,
    2595,  2599,  2603,  2609,  2615,  2623,  2629,  2633,  2639,  2643,
    2649,  2654,  2659,  2666,  2675,  2685,  2694,  2706,  2722,  2726,
    2731,  2741,  2763,  2769,  2773,  2774,  2775,  2776,  2777,  2779,
    2782,  2788,  2791,  2792,  2793,  2794,  2795,  2796,  2797,  2798,
    2799,  2800,  2804,  2820,  2837,  2855,  2901,  2940,  2983,  3030,
    3054,  3077,  3098,  3119,  3128,  3139,  3150,  3164,  3171,  3181,
    3187,  3199,  3202,  3205,  3208,  3211,  3214,  3218,  3222,  3227,
    3235,  3243,  3252,  3259,  3266,  3273,  3280,  3287,  3295,  3303,
    3311,  3319,  3327,  3335,  3343,  3351,  3359,  3367,  3375,  3383,
    3413,  3421,  3430,  3438,  3447,  3455,  3461,  3468,  3474,  3481,
    3486,  3493,  3500,  3508,  3535,  3541,  3547,  3554,  3562,  3569,
    3576,  3581,  3591,  3596,  3601,  3606,  3611,  3616,  3621,  3626,
    3631,  3636,  3639,  3642,  3646,  3649,  3652,  3655,  3659,  3662,
    3665,  3669,  3673,  3678,  3683,  3686,  3690,  3694,  3701,  3708,
    3712,  3719,  3726,  3730,  3734,  3738,  3741,  3745,  3749,  3754,
    3759,  3763,  3768,  3773,  3779,  3785,  3791,  3797,  3803,  3809,
    3815,  3821,  3827,  3833,  3839,  3850,  3854,  3859,  3890,  3900,
    3905,  3910,  3915,  3921,  3925,  3926,  3928,  3929,  3931,  3932,
    3944,  3952,  3956,  3959,  3963,  3966,  3970,  3974,  3979,  3985,
    3995,  4005,  4013,  4024,  4055
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
  "CIRCLE", "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT",
  "MULTIVECTOR", "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY",
  "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "COMMENT", "TRUE", "FALSE",
  "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS",
  "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ",
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE",
  "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION", "MEMINDEX",
  "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "CONFIGS", "CONFIG",
  "PROFILES", "VARIABLES", "VARIABLE", "DELTA", "LOGS", "CATALOGS",
  "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES", "FUSION",
  "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER", "LEARNER", "CONNECT",
  "STANDALONE", "NODES", "NODE", "REMOVE", "SNAPSHOT", "SNAPSHOTS",
  "RECOVER", "RESTORE", "PERSISTENCE", "OBJECT", "OBJECTS", "FILES",
  "MEMORY", "ALLOCATION", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','",
  "':'", "$accept", "input_pattern", "statement_list", "statement",
  "explainable_statement", "create_statement", "table_element_array",
  "column_def_array", "table_element", "table_column", "column_type",
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

#define YYPACT_NINF (-744)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-522)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     871,   394,    43,    88,    84,    29,    84,   220,    72,   805,
      59,    86,    96,   103,   279,   180,   246,   284,   176,    92,
     418,   -36,   297,   137,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,   348,  -744,  -744,   367,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,   -32,   -31,   292,   292,   213,   190,    84,
     331,   331,   331,   331,   331,   390,   199,   410,    84,   -41,
     488,   493,   516,   289,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,   348,  -744,  -744,  -744,  -744,  -744,   316,   543,    84,
    -744,  -744,  -744,  -744,  -744,   547,  -744,   354,   389,  -744,
     569,  -744,   434,  -744,  -744,   571,  -744,   611,  -744,   419,
      47,    84,    84,    84,    84,  -744,  -744,  -744,  -744,    -6,
    -744,   575,   415,  -744,   631,   444,   460,   235,   750,   474,
     649,   481,   606,   496,   499,   479,   495,  -744,    37,  -744,
     698,  -744,  -744,     9,   660,  -744,   661,   656,   727,   728,
     729,    84,    84,    84,   730,   673,   731,   521,   665,   737,
      84,    84,    84,   738,  -744,   739,   740,   679,   742,   742,
     612,    71,   136,   141,  -744,   527,  -744,   310,  -744,  -744,
     747,  -744,   748,  -744,  -744,  -744,  -744,   749,  -744,  -744,
    -744,  -744,   347,  -744,  -744,  -744,    84,   538,   284,   742,
    -744,   754,  -744,   587,  -744,   753,  -744,  -744,   757,  -744,
    -744,   758,  -744,   759,   762,  -744,   763,   707,   766,   576,
     769,   770,  -744,  -744,  -744,  -744,     9,  -744,  -744,  -744,
     612,   713,   706,   701,  -744,   642,  -744,   -17,  -744,   521,
    -744,    84,  -744,   774,   133,  -744,  -744,  -744,  -744,  -744,
     717,  -744,   577,   -38,  -744,   612,  -744,  -744,   703,   708,
     566,  -744,  -744,   503,   653,   568,   579,   456,   781,   787,
     789,   796,  -744,  -744,   795,   586,   401,   588,   589,   695,
     695,  -744,    28,   497,     4,  -744,    58,   257,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,   591,  -744,  -744,  -744,    49,  -744,  -744,   217,  -744,
     226,  -744,  -744,  -744,   258,  -744,   265,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,   784,   800,  -744,  -744,  -744,  -744,
    -744,  -744,   771,   777,   752,   764,   367,  -744,  -744,  -744,
     821,   277,  -744,   830,  -744,  -744,   767,   473,  -744,   823,
    -744,  -744,   623,   626,   -39,   612,   612,   776,  -744,   844,
     -36,    42,   794,   634,  -744,   162,   635,  -744,    84,   612,
     740,  -744,   537,   637,   638,   340,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,   695,   639,
     416,   779,   612,   612,   138,   400,  -744,  -744,  -744,  -744,
     503,  -744,   854,   643,   644,   645,   650,   861,   864,   477,
     477,  -744,   651,  -744,  -744,  -744,  -744,   654,    89,   793,
     612,   870,   612,   612,    -4,   658,   129,   695,   695,   695,
     695,   695,   695,   695,   695,   695,   695,   695,   695,   695,
     695,    17,  -744,   662,  -744,   883,  -744,   884,  -744,   885,
    -744,   887,   850,   657,   681,   682,   896,   687,  -744,   690,
    -744,   902,  -744,   318,   908,   751,   760,  -744,  -744,  -744,
     612,   837,   694,  -744,    61,   537,   612,  -744,  -744,    70,
     980,   788,   702,   230,  -744,  -744,  -744,   -36,   916,   797,
    -744,   918,   612,   709,  -744,   537,  -744,   139,   139,   612,
    -744,   254,   779,   772,   705,   118,    13,   420,  -744,   612,
     612,   856,   612,   933,    27,   612,   720,   275,   699,  -744,
    -744,   742,  -744,  -744,  -744,   792,   732,   695,   497,   813,
    -744,   807,   807,   304,   304,   783,   807,   807,   304,   304,
     477,   477,  -744,  -744,  -744,  -744,  -744,  -744,   733,  -744,
     734,  -744,  -744,  -744,   944,   945,  -744,   774,   949,  -744,
     950,  -744,  -744,   955,  -744,  -744,   952,   958,   743,    20,
     875,   612,  -744,  -744,  -744,   537,   966,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,   768,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,   782,   786,   790,   791,   810,   811,   250,   814,   774,
     941,    42,   348,   761,   970,  -744,   290,   815,   973,   977,
     981,   983,  -744,   991,   317,  -744,   323,   325,  -744,   816,
    -744,   980,   612,  -744,   612,   121,   160,   695,   -46,   785,
    -744,   -94,   -20,    68,   817,  -744,   995,  -744,  -744,   925,
     497,   807,   822,   335,  -744,   695,  1003,  1005,   961,   965,
     356,   361,  -744,   826,   386,  -744,  1032,  -744,  -744,   -36,
     824,   756,  -744,   322,  -744,   612,   862,  -744,  -744,  1036,
     930,  1016,  1033,  1050,  1067,  1084,   912,   917,  -744,  -744,
     205,  -744,   914,   774,   387,   829,   915,  -744,   888,  -744,
    -744,   612,  -744,  -744,  -744,  -744,  -744,  -744,   139,  -744,
    -744,  -744,   831,   537,   125,  -744,   612,   189,   836,  1048,
     662,   840,   835,   612,  -744,   839,   842,   843,   391,  -744,
    -744,   416,  1053,  1055,  -744,  -744,   949,   580,  -744,   950,
     502,    35,    20,  1006,  -744,  -744,  -744,  -744,  -744,  -744,
    1007,  -744,  1059,  -744,   743,   351,   679,   392,   845,   846,
     847,   848,   874,   877,   878,   881,   882,  1002,   892,   893,
     897,   898,   899,   900,   909,   910,   913,   926,  1023,   927,
     931,   932,   943,   947,   948,   960,   964,   967,   972,  1041,
     974,   975,   976,   978,   979,   982,   984,   986,   987,   988,
    1042,   989,   990,   992,   993,   994,   996,   997,   998,   999,
    1000,  1043,  1001,  1004,  1008,  1009,  1010,  1011,  1012,  1013,
    1014,  1015,  1057,  1017,  -744,  -744,   155,  -744,  1019,  1030,
     443,  -744,   950,  1165,  1167,   451,  -744,  -744,  -744,   537,
    -744,   704,  1018,  1020,  1021,    19,  1022,  -744,  -744,  -744,
    1115,   968,   537,  -744,   139,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  1180,  -744,   322,   756,    20,
      20,   985,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  1024,
    1141,  -744,  1191,  1194,  1197,  1205,  1209,  1216,  1217,  1219,
    1220,  1221,  1025,  1230,  1233,  1237,  1240,  1241,  1242,  1243,
    1244,  1245,  1246,  1034,  1247,  1249,  1250,  1251,  1252,  1253,
    1254,  1255,  1256,  1257,  1044,  1259,  1260,  1261,  1262,  1263,
    1264,  1265,  1266,  1267,  1268,  1056,  1269,  1271,  1272,  1273,
    1274,  1275,  1276,  1277,  1278,  1279,  1066,  1281,  1282,  1283,
    1284,  1285,  1286,  1287,  1288,  1289,  1290,  1077,  1292,  -744,
    1295,  1296,  -744,   458,  -744,   764,  -744,  -744,  1297,    74,
    1085,  1299,  1300,  -744,   463,  1301,  -744,  -744,  1248,   774,
     351,  1258,  1270,  1088,  1089,  1091,  1092,  1095,  1096,  1097,
    1098,  1099,  1100,  1312,  1102,  1103,  1104,  1105,  1106,  1108,
    1109,  1110,  1111,  1112,  1324,  1114,  1116,  1117,  1118,  1119,
    1120,  1121,  1122,  1123,  1124,  1326,  1125,  1126,  1127,  1128,
    1129,  1130,  1131,  1132,  1133,  1134,  1346,  1136,  1137,  1138,
    1139,  1140,  1142,  1143,  1144,  1145,  1146,  1352,  1147,  1148,
    1149,  1150,  1151,  1152,  1153,  1154,  1155,  1156,  1368,  1158,
    -744,  -744,  -744,  -744,  1157,   835,  1211,  1159,  1160,  -744,
     450,   612,   467,  -744,   612,   612,  -744,  -744,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  1161,  -744,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  1164,  -744,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  1166,
    -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    1168,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  1169,  -744,  -744,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  1170,  -744,  1378,  1171,  1335,  1385,    85,  1173,
    1386,  1387,  -744,  -744,  -744,   537,  -744,   743,   537,  -744,
    -744,  -744,  -744,  -744,  -744,  1174,  1226,  1179,  1176,   835,
     764,  1391,   672,   156,  1181,  1350,  1396,  1397,  1185,  -744,
     678,  1398,  -744,   835,   764,  1187,  1188,   835,   -13,  1400,
    -744,  1359,  1192,  -744,  1404,  -744,  1193,  1371,  1372,  -744,
    -744,  -744,    11,  1196,   -58,  -744,  1198,  1375,  1376,  -744,
    -744,  1377,  1379,  1415,  -744,  1204,  -744,  1206,  1207,  1418,
    1420,   764,  1208,  1210,  -744,   764,  -744,  -744
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     233,     0,     0,     0,     0,     0,     0,     0,   233,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   233,     0,   519,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   176,   175,     0,     8,    14,    15,    16,
      17,    18,    19,   517,   517,   517,   517,   517,     0,     0,
     515,   515,   515,   515,   515,     0,   226,     0,     0,     0,
       0,     0,     0,   233,   162,    20,    25,    27,    26,    21,
      22,    24,    23,    28,    29,    30,    31,     0,     0,     0,
     247,   248,   246,   252,   256,     0,   253,     0,     0,   249,
       0,   251,     0,   274,   276,     0,   254,     0,   284,     0,
     280,     0,     0,     0,     0,   286,   287,   288,   291,   226,
     289,     0,   232,   234,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     1,   233,     2,
     216,   218,   219,     0,   199,   181,   187,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   160,     0,     0,
       0,     0,     0,     0,   313,     0,     0,   211,     0,     0,
       0,     0,     0,     0,   161,     0,   262,   263,   257,   258,
       0,   259,     0,   250,   275,   255,   285,     0,   278,   277,
     281,   282,     0,   316,   308,   309,     0,     0,     0,     0,
     340,     0,   350,     0,   351,     0,   337,   338,     0,   333,
     317,     0,   346,   348,     0,   341,     0,     0,     0,     0,
       0,     0,   180,   179,     4,   217,     0,   177,   178,   198,
       0,     0,   195,     0,   311,    33,   310,     0,    34,   160,
     520,     0,   312,     0,   233,   514,   167,   169,   168,   170,
       0,   227,     0,   211,   164,     0,   156,   513,     0,     0,
     448,   452,   455,   456,     0,     0,     0,     0,     0,     0,
       0,     0,   453,   454,     0,     0,     0,     0,     0,     0,
       0,   450,     0,   233,     0,   358,   363,   364,   378,   376,
     379,   377,   380,   381,   373,   368,   367,   366,   374,   375,
     365,   372,   371,   463,   465,     0,   466,   474,     0,   475,
       0,   467,   464,   485,     0,   486,     0,   462,   295,   297,
     296,   293,   294,   300,   302,   301,   298,   299,   305,   307,
     306,   303,   304,   283,     0,     0,   265,   264,   270,   260,
     261,   279,     0,     0,     0,   523,     0,   235,   292,   343,
       0,   334,   339,   318,   347,   342,     0,     0,   349,     0,
     314,   315,     0,     0,   203,     0,     0,   197,   516,     0,
     233,     0,     0,     0,   154,     0,     0,   158,     0,     0,
       0,   163,   210,     0,     0,     0,   494,   493,   496,   495,
     498,   497,   500,   499,   502,   501,   504,   503,     0,     0,
     414,   233,     0,     0,     0,     0,   457,   458,   459,   460,
       0,   461,     0,     0,     0,     0,     0,     0,     0,   416,
     415,   491,   488,   482,   472,   477,   480,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   471,     0,   476,     0,   479,     0,   487,     0,
     490,     0,   271,   266,     0,     0,     0,     0,   290,     0,
     352,     0,   335,     0,     0,     0,     0,   345,   184,   183,
       0,   205,   186,   188,   193,   194,     0,   182,    32,    36,
       0,     0,     0,     0,    42,    46,    47,   233,     0,    40,
     159,     0,     0,   157,   171,   166,   165,     0,     0,     0,
     409,     0,   233,     0,     0,     0,     0,     0,   439,     0,
       0,     0,     0,     0,     0,     0,   209,     0,     0,   370,
     369,     0,   359,   362,   432,   433,     0,     0,   233,     0,
     413,   423,   424,   427,   428,     0,   430,   422,   425,   426,
     418,   417,   419,   420,   421,   449,   451,   473,     0,   478,
       0,   481,   489,   492,     0,     0,   267,     0,     0,   355,
       0,   236,   336,     0,   319,   344,     0,     0,   202,     0,
     201,     0,   191,   192,   190,   196,     0,    52,    55,    56,
      53,    54,    57,    58,    74,    59,    61,    60,    77,    64,
      65,    66,    62,    63,    67,    68,    69,    70,    71,    72,
      73,     0,     0,     0,     0,     0,     0,   523,     0,     0,
     525,     0,    39,     0,     0,   155,     0,     0,     0,     0,
       0,     0,   509,     0,     0,   505,     0,     0,   410,     0,
     444,     0,     0,   437,     0,     0,     0,     0,     0,     0,
     448,     0,     0,     0,     0,   399,     0,   484,   483,     0,
     233,   431,     0,     0,   412,     0,     0,     0,   272,   268,
       0,     0,    44,   528,     0,   526,   320,   353,   354,   233,
     204,   220,   222,   231,   223,     0,   207,   189,    38,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   147,   148,
     151,   144,   151,     0,     0,     0,    35,    43,   534,    41,
     360,     0,   511,   510,   508,   507,   512,   174,     0,   172,
     411,   445,     0,   441,     0,   440,     0,     0,     0,     0,
       0,     0,   209,     0,   397,     0,     0,     0,     0,   446,
     435,   434,     0,     0,   357,   356,     0,     0,   522,     0,
       0,     0,     0,     0,   240,   241,   242,   243,   239,   244,
       0,   229,     0,   224,   200,     0,   211,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   149,   146,     0,   145,    49,    48,
       0,   153,     0,     0,     0,     0,   506,   443,   438,   442,
     429,     0,     0,   209,     0,     0,     0,   468,   470,   469,
       0,     0,   208,   400,     0,   447,   436,   273,   269,    45,
     529,   530,   532,   531,   527,     0,   321,   231,   221,     0,
       0,   228,   403,   401,   404,   402,   405,   406,   407,   206,
     215,    76,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   150,
       0,     0,   152,     0,    37,   523,   361,   488,     0,     0,
       0,     0,     0,   398,     0,   322,   225,   237,     0,     0,
       0,     0,   213,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      51,    50,   524,   533,     0,   209,   393,     0,   209,   173,
       0,     0,     0,   408,     0,     0,   185,    75,    81,    82,
      79,    80,    83,    84,    85,    86,    87,     0,    78,   125,
     126,   123,   124,   127,   128,   129,   130,   131,     0,   122,
      92,    93,    90,    91,    94,    95,    96,    97,    98,     0,
      89,   103,   104,   101,   102,   105,   106,   107,   108,   109,
       0,   100,   136,   137,   134,   135,   138,   139,   140,   141,
     142,     0,   133,   114,   115,   112,   113,   116,   117,   118,
     119,   120,     0,   111,     0,     0,     0,     0,     0,     0,
       0,     0,   324,   323,   329,   238,   230,   214,   212,    88,
     132,    99,   110,   143,   121,   209,   394,     0,     0,   209,
     523,   330,   325,     0,     0,     0,     0,     0,     0,   392,
       0,     0,   326,   209,   523,     0,     0,   209,   523,     0,
     331,   327,     0,   388,     0,   395,     0,     0,     0,   391,
     332,   328,   523,     0,   382,   390,     0,     0,     0,   387,
     396,     0,     0,     0,   386,     0,   384,     0,     0,     0,
       0,   523,     0,     0,   389,   523,   383,   385
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -744,  -744,  -744,  1298,  1365,   232,  -744,  -744,   818,  -526,
     799,  -744,   741,   744,  -744,  -550,   247,   248,  1203,  -744,
     259,  -744,  1063,   299,   321,    -5,  1413,   -19,  1101,  1222,
     -63,  -744,  -744,   868,  -744,  -744,  -744,  -744,  -744,  -744,
    -744,  -744,  -714,  -238,  -744,  -744,  -744,  -744,   693,  -146,
      15,   573,  -744,  -744,  1280,  -744,  -744,   329,   341,   353,
     355,   357,  -744,  -744,  -220,  -744,  1026,  -244,  -243,  -743,
    -741,  -740,  -739,  -737,  -733,   471,  -744,  -744,  -744,  -744,
    -744,  -744,  1049,  -744,  -744,   928,   618,  -266,  -744,  -744,
    -744,   724,  -744,  -744,  -744,  -744,   726,  1027,  1028,   -45,
    -744,  -744,  -744,  -744,  1184,  -488,   745,  -150,   536,   550,
    -744,  -744,  -603,  -744,   617,   711,  -744
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    22,    23,    24,    64,    25,   483,   661,   484,   485,
     607,   690,   691,   828,   486,   365,    26,    27,   234,    28,
      29,   243,   244,    30,    31,    32,    33,    34,   135,   217,
     136,   222,   472,   473,   574,   357,   477,   220,   676,   471,
     570,   756,   644,   246,  1056,   972,   133,   670,   671,   672,
     673,   753,    35,   112,   113,   674,   750,    36,    37,    38,
      39,    40,    41,    42,   274,   493,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   878,   879,   284,   285,   286,
     287,   288,   395,   289,   290,   291,   292,   293,   846,   294,
     295,   296,   297,   298,   299,   300,   301,   415,   416,   302,
     303,   304,   305,   306,   307,   624,   625,   248,   149,   139,
     129,   145,   458,   696,   664,   665,   489
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     354,   372,   126,    71,   692,   371,   414,   660,   851,   249,
     626,   390,   872,   394,   873,   874,   875,   134,   876,    57,
     545,    59,   877,    56,   411,   412,   409,   410,   245,   418,
     640,   110,   662,   411,   412,   158,   159,  -518,   360,   338,
     137,   137,    17,   470,     1,   480,     2,     3,     4,     5,
       6,     7,     8,     9,    10,   130,   186,   131,    71,   694,
      11,    12,    13,   132,   147,   457,    14,    15,    16,  1191,
     218,   526,   722,   157,   308,    63,   309,   310,  1045,     1,
      49,     2,     3,     4,     5,     6,     7,    56,     9,   457,
     130,  1149,   131,   101,   167,    11,    12,    13,   132,   115,
     632,    14,    15,    16,   116,    58,   117,  1192,   118,   422,
     423,   474,   475,   421,  1177,    17,   182,   183,   184,   185,
     102,    50,    51,    52,   441,   495,   720,    53,    54,   960,
     103,   572,   573,   311,   723,   422,   423,   104,  1187,   313,
     723,   314,   315,   830,   318,   390,   319,   320,   505,   506,
      17,   723,  1178,   353,   527,   501,   227,   228,   229,   251,
     252,   253,  1163,   138,   140,   237,   238,   239,   422,   423,
     481,   366,   482,   631,   718,   160,  1188,   547,   524,   525,
      21,   420,   370,   108,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   316,   361,
     721,   335,   529,   321,   576,   715,    18,   618,   619,   838,
     859,    17,   155,   422,   423,   367,   422,   423,   620,   621,
     622,   419,   723,    19,   420,   216,   393,   872,   546,   873,
     874,   875,   575,   876,   413,    20,   669,   877,   271,   312,
      65,    18,   272,   413,   422,   423,   363,   716,   180,   109,
     568,   181,   867,    21,   417,    66,    67,   258,   259,   260,
     530,   119,   503,   261,   442,   635,   636,    68,   638,   443,
      20,   642,   616,   422,   423,  -521,   422,   423,   686,   627,
     422,   423,   120,    55,   651,   137,   121,   111,    21,   122,
     262,   263,   264,   422,   423,    65,     1,   127,     2,     3,
       4,     5,     6,     7,   317,     9,   520,    69,   653,   322,
      66,    67,    11,    12,    13,   422,   423,   461,    14,    15,
      16,   426,    68,   686,   623,   751,   462,   474,   457,    70,
     424,  1125,   425,   687,  1129,   688,   689,    72,   826,   427,
     428,   429,   430,   250,   251,   252,   253,   432,   114,    73,
     324,   563,  1043,   325,   326,   479,   128,   332,   327,   328,
     564,    74,    69,    75,   137,    76,   964,    17,   130,   272,
     131,   649,   504,   333,   334,   134,   132,   752,   687,   490,
     688,   689,   491,   494,    70,   146,    60,    61,   713,   426,
     714,    62,    72,   154,   717,   433,   434,   435,   436,   437,
     438,   439,   440,   148,    73,   499,   840,   427,   428,   429,
     430,   431,   731,   254,   255,   432,    74,   155,    75,  1052,
      76,   192,   193,   256,   156,   257,   194,    43,    44,    45,
     728,  1154,   444,    46,    47,  1158,   426,   445,   105,   106,
     107,   446,   258,   259,   260,    48,   447,   610,   261,  1172,
     611,   123,   124,  1176,   844,   754,  -522,  -522,    18,   250,
     251,   252,   253,   433,   434,   435,   436,   437,   438,   439,
     440,   628,   839,   448,   420,   262,   263,   264,   449,   852,
     450,   835,   612,   629,   508,   451,   509,    20,   510,   503,
    1130,   161,   645,  1131,  1132,   646,   162,   265,  1133,  1134,
     250,   251,   252,   253,   633,    21,   634,   700,   510,   652,
     420,  -522,  -522,   436,   437,   438,   439,   440,   880,   163,
     266,   403,   267,   404,   268,   405,   406,   169,   170,   254,
     255,   266,   165,   267,   707,   268,   865,   708,   866,   256,
     709,   257,   710,   708,   393,   420,   166,  1159,   426,   269,
     270,   271,   730,   168,   272,   420,   273,   500,   258,   259,
     260,  1173,   171,   172,   261,  1179,   427,   428,   429,   430,
     254,   255,   173,   734,   432,    17,   491,   175,   735,  1189,
     256,   736,   257,   860,   861,   862,   863,   150,   151,   152,
     153,   262,   263,   264,   141,   142,   143,   144,  1204,   258,
     259,   260,  1207,   738,   831,   261,   739,   491,   856,   881,
     174,   420,   882,   265,   176,   250,   251,   252,   253,   177,
     178,   179,   433,   434,   435,   436,   437,   438,   439,   440,
     187,   727,   262,   263,   264,   188,   266,   189,   267,   190,
     268,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   207,   265,   191,   250,   251,   252,   253,
     952,   465,   466,   491,   741,   269,   270,   271,   956,   206,
     272,   420,   273,   842,   208,  1042,   849,   266,   739,   267,
    1049,   268,   209,   708,  1136,   254,   255,   491,   438,   439,
     440,   210,   422,   423,   211,   256,   212,   257,   250,   251,
     252,   253,   555,   556,   647,   648,   269,   270,   271,   411,
     957,   272,   213,   273,   258,   259,   260,  1161,  1162,   215,
     261,  1169,  1170,   967,   968,   219,   388,   389,   221,   223,
     224,   225,   226,   230,   232,   231,   256,   233,   257,   235,
     236,   240,   241,   242,   323,   245,   247,   262,   263,   264,
     329,   330,   340,   331,   336,   258,   259,   260,   339,   341,
     342,   261,   347,   344,   343,   345,   355,   346,   388,   265,
     348,   349,   350,   351,   356,   358,   359,   364,   256,   368,
     257,   373,   375,   369,   391,   396,   374,   452,   262,   263,
     264,   397,   266,   398,   267,   392,   268,   258,   259,   260,
     399,   400,   402,   261,   407,   408,   453,  1135,    77,   441,
     265,  1138,   743,  -245,   744,   745,   746,   747,   454,   748,
     749,   269,   270,   271,   455,   460,   272,   467,   273,   456,
     262,   263,   264,   266,  1137,   267,   463,   268,    78,    79,
     468,    80,   457,   469,   464,   476,    81,    82,   478,   487,
     488,   492,   265,   497,   498,   502,   503,    17,   511,   512,
     513,   514,   269,   270,   271,   516,   515,   272,   517,   273,
     521,   519,   518,   523,   528,   266,   272,   267,     1,   268,
       2,     3,     4,     5,     6,     7,     8,     9,    10,   548,
     550,   552,   553,   554,    11,    12,    13,   557,   558,   559,
      14,    15,    16,   560,   269,   270,   271,   561,   562,   272,
     195,   273,   565,   569,   571,   426,   566,   608,   609,   613,
     196,   615,   630,   197,   198,   567,   199,   200,   201,   617,
     527,   614,   637,   427,   428,   429,   430,   639,   655,   426,
     643,   432,   202,   203,   654,   204,   205,   422,   650,    17,
     658,   659,   480,   663,   656,   657,   667,  -522,  -522,   429,
     430,   666,   668,   420,   675,  -522,    83,    84,    85,    86,
     678,    87,    88,   695,   699,    89,    90,    91,   698,   702,
      92,    93,    94,   703,   679,   704,   705,    95,    96,   433,
     434,   435,   436,   437,   438,   439,   440,   706,   680,   725,
      97,    98,   681,   726,    99,   719,   682,   683,   100,   648,
     647,   732,   733,  -522,   434,   435,   436,   437,   438,   439,
     440,   758,   759,   760,   761,   762,   684,   685,   763,   764,
     693,   701,   737,   711,   724,   765,   766,   767,   740,   729,
      18,   755,   757,   824,   742,   832,   825,   826,   837,   833,
     841,   768,   843,   834,   845,   850,   853,    19,   854,   857,
     855,   858,   871,   869,   870,   883,   884,   885,   886,    20,
     577,   578,   579,   580,   581,   582,   583,   584,   585,   586,
     587,   588,   589,   590,   591,   592,   593,    21,   594,   595,
     596,   597,   598,   599,   887,   892,   600,   888,   889,   601,
     602,   890,   891,   603,   604,   605,   606,   769,   770,   771,
     772,   773,   893,   894,   774,   775,   903,   895,   896,   897,
     898,   776,   777,   778,   780,   781,   782,   783,   784,   899,
     900,   785,   786,   901,   914,   925,   936,   779,   787,   788,
     789,   791,   792,   793,   794,   795,   902,   904,   796,   797,
     947,   905,   906,   950,   790,   798,   799,   800,   802,   803,
     804,   805,   806,   907,   951,   807,   808,   908,   909,   954,
     955,   801,   809,   810,   811,   813,   814,   815,   816,   817,
     910,   723,   818,   819,   911,   963,   965,   912,   812,   820,
     821,   822,   913,   971,   915,   916,   917,   973,   918,   919,
     974,   969,   920,   975,   921,   823,   922,   923,   924,   926,
     927,   976,   928,   929,   930,   977,   931,   932,   933,   934,
     935,   937,   978,   979,   938,   980,   981,   982,   939,   940,
     941,   942,   943,   944,   945,   946,   984,   948,   958,   985,
     959,   961,   962,   986,   970,   983,   987,   988,   989,   990,
     991,   992,   993,   995,   994,   996,   997,   998,   999,  1000,
    1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,
    1011,  1012,  1013,  1014,  1015,  1017,  1016,  1018,  1019,  1020,
    1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,  1029,  1030,
    1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1040,
    1041,  1044,  1046,  1047,  1048,  1057,  1058,  1050,  1059,  1060,
    1051,  1054,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,
    1069,  1070,  1071,  1072,  1055,  1073,  1074,  1075,  1076,  1077,
    1078,  1079,  1089,  1080,  1081,  1082,  1083,  1084,  1085,  1086,
    1087,  1088,  1090,  1091,  1092,  1093,  1094,  1095,  1096,  1097,
    1098,  1099,  1100,  1101,  1102,  1103,  1104,  1105,  1111,  1106,
    1107,  1108,  1109,  1110,  1112,  1113,  1114,  1115,  1116,  1117,
    1118,  1119,  1120,  1121,  1122,  1123,  1126,  1124,  1139,  1127,
    1128,  1140,  1145,  1141,  1147,  1142,  1143,  1144,  1146,  1148,
    1150,  1155,  1151,  1152,  1153,  1156,  1157,  1160,  1164,  1165,
    1166,  1167,  1168,  1174,  1171,  1175,  1180,  1181,  1183,  1182,
    1184,  1185,  1186,  1190,  1193,  1194,  1195,  1196,  1198,  1197,
    1199,  1202,  1200,  1203,  1201,  1205,   214,  1206,   164,   697,
     712,   827,   362,   496,   125,   868,   829,   459,   352,   677,
     966,  1053,   641,   507,   949,   847,   522,   848,   401,   953,
     864,     0,     0,   836,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   337,     0,
       0,     0,   549,     0,     0,   551
};

static const yytype_int16 yycheck[] =
{
     220,   245,    21,     8,   607,   243,   272,   557,   722,   159,
     498,   254,   755,   257,   755,   755,   755,     8,   755,     4,
       3,     6,   755,     3,     5,     6,   269,   270,    66,   273,
       3,    16,   558,     5,     6,    76,    77,     0,    55,   189,
      72,    72,    78,    82,     7,     3,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    20,    62,    22,    63,   609,
      23,    24,    25,    28,    49,    78,    29,    30,    31,   127,
     133,    75,     4,    58,     3,     3,     5,     6,     4,     7,
      37,     9,    10,    11,    12,    13,    14,     3,    16,    78,
      20,     6,    22,    34,    79,    23,    24,    25,    28,     7,
      87,    29,    30,    31,    12,    76,    14,   165,    16,   155,
     156,   355,   356,    55,   127,    78,   101,   102,   103,   104,
      34,    33,    34,    35,   218,   369,   220,    39,    40,   843,
      34,    70,    71,    62,    66,   155,   156,    34,   127,     3,
      66,     5,     6,   693,     3,   388,     5,     6,   392,   393,
      78,    66,   165,   216,   158,   375,   141,   142,   143,     4,
       5,     6,     6,   195,   195,   150,   151,   152,   155,   156,
     128,    38,   130,    55,   220,   216,   165,   443,   422,   423,
     216,   220,   220,     3,   427,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,    62,   216,
     220,   186,    73,    62,   134,    84,   169,    68,    69,    84,
     736,    78,   218,   155,   156,   234,   155,   156,    79,    80,
      81,   217,    66,   186,   220,   216,    88,   970,   211,   970,
     970,   970,   476,   970,   215,   198,   216,   970,   211,   168,
       8,   169,   214,   215,   155,   156,   231,    87,   201,     3,
     470,   204,   217,   216,   273,     8,     8,   102,   103,   104,
     131,   169,    73,   108,   215,   509,   510,     8,   512,   220,
     198,   515,   492,   155,   156,    62,   155,   156,    73,   499,
     155,   156,   190,   195,   527,    72,   194,     3,   216,   197,
     135,   136,   137,   155,   156,    63,     7,     0,     9,    10,
      11,    12,    13,    14,   168,    16,   217,     8,   528,   168,
      63,    63,    23,    24,    25,   155,   156,    40,    29,    30,
      31,   132,    63,    73,   185,     3,    49,   571,    78,     8,
      73,  1045,    75,   128,  1048,   130,   131,     8,   133,   150,
     151,   152,   153,     3,     4,     5,     6,   158,   172,     8,
      40,    33,   955,    43,    44,   360,   219,    10,    48,    49,
      42,     8,    63,     8,    72,     8,   854,    78,    20,   214,
      22,   521,   391,    26,    27,     8,    28,    55,   128,   217,
     130,   131,   220,   368,    63,   195,   166,   167,   632,   132,
     634,   171,    63,     3,   637,   206,   207,   208,   209,   210,
     211,   212,   213,    72,    63,    65,   217,   150,   151,   152,
     153,   154,   655,    73,    74,   158,    63,   218,    63,   969,
      63,   186,   187,    83,    14,    85,   191,    33,    34,    35,
     650,  1145,   215,    39,    40,  1149,   132,   220,   159,   160,
     161,   215,   102,   103,   104,    51,   220,   217,   108,  1163,
     220,    33,    34,  1167,   720,   675,   152,   153,   169,     3,
       4,     5,     6,   206,   207,   208,   209,   210,   211,   212,
     213,   217,   716,   215,   220,   135,   136,   137,   220,   723,
     215,   701,   487,   502,    84,   220,    86,   198,    88,    73,
      40,     3,   217,    43,    44,   220,     3,   157,    48,    49,
       3,     4,     5,     6,    84,   216,    86,   217,    88,   528,
     220,   207,   208,   209,   210,   211,   212,   213,   756,     3,
     180,   120,   182,   122,   184,   124,   125,   173,   174,    73,
      74,   180,   216,   182,   217,   184,    34,   220,    36,    83,
     217,    85,   217,   220,    88,   220,     3,  1150,   132,   209,
     210,   211,   217,     6,   214,   220,   216,   217,   102,   103,
     104,  1164,   173,   174,   108,  1168,   150,   151,   152,   153,
      73,    74,     3,   217,   158,    78,   220,     6,   217,  1182,
      83,   220,    85,     3,     4,     5,     6,    51,    52,    53,
      54,   135,   136,   137,    44,    45,    46,    47,  1201,   102,
     103,   104,  1205,   217,   217,   108,   220,   220,   217,   217,
     176,   220,   220,   157,     3,     3,     4,     5,     6,   200,
     201,   202,   206,   207,   208,   209,   210,   211,   212,   213,
      55,   650,   135,   136,   137,   220,   180,     6,   182,   195,
     184,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,     4,   157,   195,     3,     4,     5,     6,
     217,   188,   189,   220,   669,   209,   210,   211,   217,   195,
     214,   220,   216,   718,   193,   217,   721,   180,   220,   182,
     217,   184,    76,   220,   217,    73,    74,   220,   211,   212,
     213,   195,   155,   156,   195,    83,   217,    85,     3,     4,
       5,     6,    45,    46,     5,     6,   209,   210,   211,     5,
       6,   214,   217,   216,   102,   103,   104,    45,    46,    21,
     108,    43,    44,   869,   870,    65,    73,    74,    67,    73,
       3,     3,     3,     3,     3,    62,    83,   216,    85,    74,
       3,     3,     3,     3,   217,    66,     4,   135,   136,   137,
       3,     3,   165,     4,   216,   102,   103,   104,     4,     6,
       3,   108,    55,     4,     6,     3,    53,     4,    73,   157,
       4,   195,     3,     3,    68,    74,   134,     3,    83,    62,
      85,    78,   216,   206,   216,     4,    78,     3,   135,   136,
     137,     4,   180,     4,   182,   216,   184,   102,   103,   104,
       4,     6,   216,   108,   216,   216,     6,  1051,     3,   218,
     157,  1055,    56,    57,    58,    59,    60,    61,    47,    63,
      64,   209,   210,   211,    47,     4,   214,     4,   216,    77,
     135,   136,   137,   180,  1054,   182,     6,   184,    33,    34,
     217,    36,    78,   217,    77,    69,    41,    42,     4,    55,
     216,   216,   157,   216,   216,   216,    73,    78,     4,   216,
     216,   216,   209,   210,   211,     4,   216,   214,     4,   216,
      77,   217,   221,     3,   216,   180,   214,   182,     7,   184,
       9,    10,    11,    12,    13,    14,    15,    16,    17,     6,
       6,     6,     5,    43,    23,    24,    25,   216,   216,     3,
      29,    30,    31,   216,   209,   210,   211,   217,     6,   214,
     160,   216,     4,    76,   220,   132,   165,   129,   216,     3,
     170,     3,   217,   173,   174,   165,   176,   177,   178,   220,
     158,   134,    76,   150,   151,   152,   153,     4,   155,   132,
     220,   158,   192,   193,   131,   195,   196,   155,   216,    78,
       6,     6,     3,     3,   221,   221,     4,   150,   151,   152,
     153,     6,     4,   220,    89,   158,   161,   162,   163,   164,
       4,   166,   167,    32,     4,   170,   171,   172,   217,     6,
     175,   176,   177,     6,   216,     4,     3,   182,   183,   206,
     207,   208,   209,   210,   211,   212,   213,     6,   216,     4,
     195,   196,   216,    78,   199,   220,   216,   216,   203,     6,
       5,    50,    47,   206,   207,   208,   209,   210,   211,   212,
     213,    91,    92,    93,    94,    95,   216,   216,    98,    99,
     216,   216,   206,   217,   217,   105,   106,   107,     6,   217,
     169,   179,     6,   131,   220,   216,   129,   133,   217,   134,
     214,   121,     4,   165,   214,   220,   217,   186,   216,     6,
     217,     6,     3,    57,    57,   220,   220,   220,   220,   198,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   216,   108,   109,
     110,   111,   112,   113,   220,    93,   116,   220,   220,   119,
     120,   220,   220,   123,   124,   125,   126,    91,    92,    93,
      94,    95,   220,   220,    98,    99,    93,   220,   220,   220,
     220,   105,   106,   107,    91,    92,    93,    94,    95,   220,
     220,    98,    99,   220,    93,    93,    93,   121,   105,   106,
     107,    91,    92,    93,    94,    95,   220,   220,    98,    99,
      93,   220,   220,   134,   121,   105,   106,   107,    91,    92,
      93,    94,    95,   220,   134,    98,    99,   220,   220,     4,
       3,   121,   105,   106,   107,    91,    92,    93,    94,    95,
     220,    66,    98,    99,   220,   217,     6,   220,   121,   105,
     106,   107,   220,    52,   220,   220,   220,     6,   220,   220,
       6,   216,   220,     6,   220,   121,   220,   220,   220,   220,
     220,     6,   220,   220,   220,     6,   220,   220,   220,   220,
     220,   220,     6,     6,   220,     6,     6,     6,   220,   220,
     220,   220,   220,   220,   220,   220,     6,   220,   220,     6,
     220,   220,   220,     6,   220,   220,     6,     6,     6,     6,
       6,     6,     6,     6,   220,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   220,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   220,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   220,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   220,     6,     4,
       4,     4,   217,     4,     4,   217,   217,     6,   217,   217,
      62,    53,   217,   217,   217,   217,   217,   217,     6,   217,
     217,   217,   217,   217,    54,   217,   217,   217,   217,   217,
       6,   217,     6,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,     6,   217,   217,   217,   217,   217,     6,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,     6,   217,   165,   220,   217,   220,
     220,   217,     4,   217,    49,   217,   217,   217,   217,     4,
     217,   165,     6,     6,   220,   216,   220,     6,   217,    49,
       4,     4,   217,   216,     6,   217,     6,    48,     4,   217,
     217,    40,    40,   217,   216,    40,    40,    40,     3,    40,
     216,     3,   216,     3,   217,   217,   128,   217,    63,   611,
     631,   690,   229,   370,    21,   742,   692,   336,   216,   571,
     867,   970,   514,   394,   826,   721,   420,   721,   264,   832,
     739,    -1,    -1,   708,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   188,    -1,
      -1,    -1,   445,    -1,    -1,   447
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    78,   169,   186,
     198,   216,   223,   224,   225,   227,   238,   239,   241,   242,
     245,   246,   247,   248,   249,   274,   279,   280,   281,   282,
     283,   284,   285,    33,    34,    35,    39,    40,    51,    37,
      33,    34,    35,    39,    40,   195,     3,   272,    76,   272,
     166,   167,   171,     3,   226,   227,   238,   239,   242,   245,
     246,   247,   279,   280,   281,   282,   283,     3,    33,    34,
      36,    41,    42,   161,   162,   163,   164,   166,   167,   170,
     171,   172,   175,   176,   177,   182,   183,   195,   196,   199,
     203,    34,    34,    34,    34,   159,   160,   161,     3,     3,
     272,     3,   275,   276,   172,     7,    12,    14,    16,   169,
     190,   194,   197,    33,    34,   248,   249,     0,   219,   332,
      20,    22,    28,   268,     8,   250,   252,    72,   195,   331,
     195,   331,   331,   331,   331,   333,   195,   272,    72,   330,
     330,   330,   330,   330,     3,   218,    14,   272,    76,    77,
     216,     3,     3,     3,   226,   216,     3,   272,     6,   173,
     174,   173,   174,     3,   176,     6,     3,   200,   201,   202,
     201,   204,   272,   272,   272,   272,    62,    55,   220,     6,
     195,   195,   186,   187,   191,   160,   170,   173,   174,   176,
     177,   178,   192,   193,   195,   196,   195,     4,   193,    76,
     195,   195,   217,   217,   225,    21,   216,   251,   252,    65,
     259,    67,   253,    73,     3,     3,     3,   272,   272,   272,
       3,    62,     3,   216,   240,    74,     3,   272,   272,   272,
       3,     3,     3,   243,   244,    66,   265,     4,   329,   329,
       3,     4,     5,     6,    73,    74,    83,    85,   102,   103,
     104,   108,   135,   136,   137,   157,   180,   182,   184,   209,
     210,   211,   214,   216,   286,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   299,   300,   301,   302,   303,   305,
     306,   307,   308,   309,   311,   312,   313,   314,   315,   316,
     317,   318,   321,   322,   323,   324,   325,   326,     3,     5,
       6,    62,   168,     3,     5,     6,    62,   168,     3,     5,
       6,    62,   168,   217,    40,    43,    44,    48,    49,     3,
       3,     4,    10,    26,    27,   272,   216,   276,   329,     4,
     165,     6,     3,     6,     4,     3,     4,    55,     4,   195,
       3,     3,   251,   252,   286,    53,    68,   257,    74,   134,
      55,   216,   240,   272,     3,   237,    38,   249,    62,   206,
     220,   265,   289,    78,    78,   216,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,    73,    74,
     290,   216,   216,    88,   289,   304,     4,     4,     4,     4,
       6,   326,   216,   120,   122,   124,   125,   216,   216,   290,
     290,     5,     6,   215,   309,   319,   320,   249,   289,   217,
     220,    55,   155,   156,    73,    75,   132,   150,   151,   152,
     153,   154,   158,   206,   207,   208,   209,   210,   211,   212,
     213,   218,   215,   220,   215,   220,   215,   220,   215,   220,
     215,   220,     3,     6,    47,    47,    77,    78,   334,   250,
       4,    40,    49,     6,    77,   188,   189,     4,   217,   217,
      82,   261,   254,   255,   289,   289,    69,   258,     4,   247,
       3,   128,   130,   228,   230,   231,   236,    55,   216,   338,
     217,   220,   216,   287,   272,   289,   244,   216,   216,    65,
     217,   286,   216,    73,   249,   289,   289,   304,    84,    86,
      88,     4,   216,   216,   216,   216,     4,     4,   221,   217,
     217,    77,   288,     3,   289,   289,    75,   158,   216,    73,
     131,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   290,   290,   290,     3,   211,   309,     6,   319,
       6,   320,     6,     5,    43,    45,    46,   216,   216,     3,
     216,   217,     6,    33,    42,     4,   165,   165,   286,    76,
     262,   220,    70,    71,   256,   289,   134,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   108,   109,   110,   111,   112,   113,
     116,   119,   120,   123,   124,   125,   126,   232,   129,   216,
     217,   220,   247,     3,   134,     3,   286,   220,    68,    69,
      79,    80,    81,   185,   327,   328,   327,   286,   217,   249,
     217,    55,    87,    84,    86,   289,   289,    76,   289,     4,
       3,   307,   289,   220,   264,   217,   220,     5,     6,   329,
     216,   290,   249,   286,   131,   155,   221,   221,     6,     6,
     237,   229,   231,     3,   336,   337,     6,     4,     4,   216,
     269,   270,   271,   272,   277,    89,   260,   255,     4,   216,
     216,   216,   216,   216,   216,   216,    73,   128,   130,   131,
     233,   234,   334,   216,   237,    32,   335,   230,   217,     4,
     217,   216,     6,     6,     4,     3,     6,   217,   220,   217,
     217,   217,   232,   289,   289,    84,    87,   290,   220,   220,
     220,   220,     4,    66,   217,     4,    78,   249,   286,   217,
     217,   290,    50,    47,   217,   217,   220,   206,   217,   220,
       6,   247,   220,    56,    58,    59,    60,    61,    63,    64,
     278,     3,    55,   273,   286,   179,   263,     6,    91,    92,
      93,    94,    95,    98,    99,   105,   106,   107,   121,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   121,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     121,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   121,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   121,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   121,   131,   129,   133,   234,   235,   235,
     237,   217,   216,   134,   165,   286,   328,   217,    84,   289,
     217,   214,   321,     4,   309,   214,   310,   313,   318,   321,
     220,   264,   289,   217,   216,   217,   217,     6,     6,   231,
       3,     4,     5,     6,   337,    34,    36,   217,   270,    57,
      57,     3,   291,   292,   293,   294,   295,   296,   297,   298,
     265,   217,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,    93,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,    93,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,    93,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,    93,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,    93,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,    93,   220,   308,
     134,   134,   217,   336,     4,     3,   217,     6,   220,   220,
     264,   220,   220,   217,   327,     6,   273,   271,   271,   216,
     220,    52,   267,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   220,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   220,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   220,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   220,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   220,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   220,     6,
       4,     4,   217,   334,     4,     4,   217,     4,     4,   217,
       6,    62,   237,   297,    53,    54,   266,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,     6,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,     6,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,     6,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
       6,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,     6,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,     6,   217,   220,   264,   165,   220,   220,   264,
      40,    43,    44,    48,    49,   289,   217,   286,   289,   217,
     217,   217,   217,   217,   217,     4,   217,    49,     4,     6,
     217,     6,     6,   220,   264,   165,   216,   220,   264,   334,
       6,    45,    46,     6,   217,    49,     4,     4,   217,    43,
      44,     6,   264,   334,   216,   217,   264,   127,   165,   334,
       6,    48,   217,     4,   217,    40,    40,   127,   165,   334,
     217,   127,   165,   216,    40,    40,    40,    40,     3,   216,
     216,   217,     3,     3,   334,   217,   217,   334
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   222,   223,   224,   224,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     226,   226,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   226,   227,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   228,   228,   229,   229,   230,   230,   231,   231,
     231,   231,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   233,   233,   234,   234,   234,   234,
     235,   235,   236,   236,   237,   237,   238,   239,   239,   240,
     240,   241,   241,   242,   243,   243,   244,   245,   245,   245,
     245,   245,   246,   246,   246,   247,   247,   247,   247,   248,
     248,   249,   250,   251,   251,   252,   253,   253,   254,   254,
     255,   256,   256,   256,   257,   257,   258,   258,   259,   259,
     260,   260,   261,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   266,   266,   267,   267,   268,   268,   268,   268,
     269,   269,   270,   270,   271,   271,   272,   272,   273,   273,
     273,   273,   274,   274,   275,   275,   276,   277,   277,   278,
     278,   278,   278,   278,   278,   278,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   279,   279,   279,   279,
     279,   279,   279,   279,   279,   279,   280,   280,   280,   281,
     281,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   283,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   285,   285,   285,   286,   286,
     287,   287,   288,   288,   289,   289,   289,   289,   289,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,   291,   291,   291,   292,   292,   292,   292,   293,
     293,   293,   293,   294,   294,   294,   294,   295,   295,   296,
     296,   297,   297,   297,   297,   297,   297,   298,   298,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   300,   300,   301,   302,   302,   303,   303,   303,
     303,   304,   304,   305,   306,   306,   306,   306,   307,   307,
     307,   307,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   308,   308,   309,   309,   309,   309,   310,   310,
     310,   311,   312,   312,   313,   313,   314,   315,   315,   316,
     317,   317,   318,   319,   320,   321,   321,   322,   323,   323,
     324,   325,   325,   326,   326,   326,   326,   326,   326,   326,
     326,   326,   326,   326,   326,   327,   327,   328,   328,   328,
     328,   328,   328,   329,   330,   330,   331,   331,   332,   332,
     333,   333,   334,   334,   335,   335,   336,   336,   337,   337,
     337,   337,   337,   338,   338
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     4,     4,     8,     6,    10,     8,     7,
       6,     8,     1,     3,     1,     3,     1,     1,     4,     4,
       6,     6,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     4,     1,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     1,     2,     2,     1,     1,     2,
       2,     0,     5,     4,     1,     3,     4,     6,     5,     3,
       0,     3,     2,     5,     1,     3,     3,     4,     4,     4,
       4,     6,     8,    11,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,    10,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       2,     0,     2,     0,     3,     0,     1,     2,     1,     1,
       1,     3,     1,     1,     2,     4,     1,     3,     2,     1,
       5,     0,     2,     0,     1,     3,     5,     4,     6,     1,
       1,     1,     1,     1,     1,     0,     2,     2,     2,     2,
       3,     2,     2,     2,     2,     3,     2,     3,     3,     3,
       4,     4,     3,     3,     4,     4,     5,     6,     7,     9,
       4,     5,     7,     9,     2,     3,     2,     3,     3,     4,
       2,     3,     3,     4,     2,     3,     2,     2,     2,     2,
       5,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     3,     3,
       4,     4,     4,     3,     4,     4,     3,     3,     4,     6,
       7,     9,    10,    12,    12,    13,    14,    15,    16,    12,
      13,    15,    16,     3,     4,     5,     6,     3,     3,     4,
       3,     3,     4,     4,     6,     5,     3,     4,     3,     4,
       3,     3,     5,     7,     7,     6,     8,     8,     1,     3,
       3,     5,     3,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,    14,    19,    16,    20,    16,    15,    13,    18,
      14,    13,    11,     8,    10,    13,    15,     5,     7,     4,
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
       3,     3,     3,     6,     3
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
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2434 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2442 "parser.cpp"
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
#line 2456 "parser.cpp"
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
#line 2470 "parser.cpp"
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
#line 2484 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 310 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2495 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2512 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2521 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 281 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 274 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2546 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2556 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2566 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2576 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2586 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2596 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2606 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2620 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2634 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 344 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2644 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2660 "parser.cpp"
        break;

    case YYSYMBOL_unnest_clause: /* unnest_clause  */
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
#line 2674 "parser.cpp"
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
#line 2688 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2697 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2705 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2713 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2721 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2729 "parser.cpp"
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
#line 2743 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2752 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2761 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 301 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2783 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2830 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2839 "parser.cpp"
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
#line 2853 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 254 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2867 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2875 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2883 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2891 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2899 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2907 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2915 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2923 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2931 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2939 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2947 "parser.cpp"
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
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 362 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3014 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3022 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3030 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3038 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3046 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3054 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3062 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3070 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3078 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3086 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3094 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3102 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3110 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3118 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3126 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 371 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3134 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 375 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3142 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3150 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3158 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3166 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3174 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3182 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3190 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3198 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3209 "parser.cpp"
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
#line 3223 "parser.cpp"
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
#line 3237 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3248 "parser.cpp"
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

#line 3356 "parser.cpp"

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
#line 503 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3571 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 507 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3582 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 513 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3593 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 520 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3599 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 521 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3605 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 522 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3611 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 523 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3617 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 524 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3623 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 525 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3629 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 526 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3635 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3641 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 528 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3647 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 529 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3653 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 530 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3659 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3665 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3671 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3677 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3683 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 536 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3689 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3695 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3701 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3707 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3713 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3719 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3725 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3731 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3737 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3743 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3749 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3755 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 554 "parser.y"
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
#line 3777 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 571 "parser.y"
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
#line 3797 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 588 "parser.y"
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
#line 3815 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 604 "parser.y"
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
#line 3848 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 633 "parser.y"
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
#line 3868 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 648 "parser.y"
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
#line 3904 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 680 "parser.y"
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
#line 3926 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 698 "parser.y"
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
#line 3947 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 716 "parser.y"
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
#line 3980 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 744 "parser.y"
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
#line 4015 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 775 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4024 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 779 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4033 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 784 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4042 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 788 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4051 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 794 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4059 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 797 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4067 "parser.cpp"
    break;

  case 48: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 804 "parser.y"
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
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, "", std::move(default_expr));

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4123 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 855 "parser.y"
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
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, "", default_expr);

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
#line 4165 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 892 "parser.y"
                                                                           {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    std::vector<std::unique_ptr<infinity::InitParameter>> index_param_list = infinity::InitParameter::MakeInitParameterList((yyvsp[-3].with_index_param_list_t));
    switch((yyvsp[-4].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-4].column_type_t).precision, (yyvsp[-4].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                free((yyvsp[-5].str_value));
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
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-4].column_type_t).embedding_type_, (yyvsp[-4].column_type_t).width);
            break;
        }
        case infinity::LogicalType::kSparse: {
            auto store_type = infinity::SparseInfo::ParseStoreType(index_param_list);
            type_info_ptr = infinity::SparseInfo::Make((yyvsp[-4].column_type_t).embedding_type_, (yyvsp[-4].column_type_t).width, store_type);
            if (type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create sparse info.");
                free((yyvsp[-5].str_value));
                YYERROR;
            }
            break;
        }
        default: {
            break;
        }
    }

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-4].column_type_t).logical_type_, type_info_ptr, (yyvsp[0].str_value), std::move(default_expr));
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
#line 4222 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 944 "parser.y"
                                                                        {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-4].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-4].column_type_t).precision, (yyvsp[-4].column_type_t).scale);
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
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-4].column_type_t).embedding_type_, (yyvsp[-4].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[-2].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-4].column_type_t).logical_type_, type_info_ptr, (yyvsp[0].str_value), default_expr);
    free((yyvsp[0].str_value));

    ParserHelper::ToLower((yyvsp[-5].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-5].str_value);
    (yyval.table_column_t)->constraints_ = *(yyvsp[-3].column_constraints_t);
    delete (yyvsp[-3].column_constraints_t);
    free((yyvsp[-5].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 4265 "parser.cpp"
    break;

  case 52: /* column_type: BOOLEAN  */
#line 984 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4271 "parser.cpp"
    break;

  case 53: /* column_type: TINYINT  */
#line 985 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4277 "parser.cpp"
    break;

  case 54: /* column_type: SMALLINT  */
#line 986 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4283 "parser.cpp"
    break;

  case 55: /* column_type: INTEGER  */
#line 987 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4289 "parser.cpp"
    break;

  case 56: /* column_type: INT  */
#line 988 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4295 "parser.cpp"
    break;

  case 57: /* column_type: BIGINT  */
#line 989 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4301 "parser.cpp"
    break;

  case 58: /* column_type: HUGEINT  */
#line 990 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4307 "parser.cpp"
    break;

  case 59: /* column_type: FLOAT  */
#line 991 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4313 "parser.cpp"
    break;

  case 60: /* column_type: REAL  */
#line 992 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4319 "parser.cpp"
    break;

  case 61: /* column_type: DOUBLE  */
#line 993 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4325 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT16  */
#line 994 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4331 "parser.cpp"
    break;

  case 63: /* column_type: BFLOAT16  */
#line 995 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4337 "parser.cpp"
    break;

  case 64: /* column_type: DATE  */
#line 996 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4343 "parser.cpp"
    break;

  case 65: /* column_type: TIME  */
#line 997 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4349 "parser.cpp"
    break;

  case 66: /* column_type: DATETIME  */
#line 998 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4355 "parser.cpp"
    break;

  case 67: /* column_type: TIMESTAMP  */
#line 999 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4361 "parser.cpp"
    break;

  case 68: /* column_type: UUID  */
#line 1000 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4367 "parser.cpp"
    break;

  case 69: /* column_type: POINT  */
#line 1001 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4373 "parser.cpp"
    break;

  case 70: /* column_type: LINE  */
#line 1002 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4379 "parser.cpp"
    break;

  case 71: /* column_type: LSEG  */
#line 1003 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4385 "parser.cpp"
    break;

  case 72: /* column_type: BOX  */
#line 1004 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4391 "parser.cpp"
    break;

  case 73: /* column_type: CIRCLE  */
#line 1007 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4397 "parser.cpp"
    break;

  case 74: /* column_type: VARCHAR  */
#line 1009 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4403 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4409 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 1011 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4415 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL  */
#line 1012 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4421 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4427 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4433 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4439 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4445 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4451 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4457 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4463 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4469 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4475 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4481 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4487 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4493 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4499 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4505 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4511 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4517 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4523 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4529 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4535 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4541 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4547 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4553 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4559 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4565 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4571 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4577 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4583 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4589 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4595 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4601 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4607 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4613 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4619 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4625 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4631 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4637 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4643 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4649 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4655 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1054 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4661 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1055 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4667 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1056 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4673 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1057 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4679 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1058 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4685 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1059 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4691 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1060 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4697 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1061 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4703 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1062 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4709 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1063 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4715 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1064 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4721 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1065 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4727 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1066 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4733 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1067 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4739 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1068 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4745 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1069 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4751 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1070 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4757 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1071 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4763 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1072 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4769 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1073 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4775 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1074 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4781 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1075 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4787 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1076 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4793 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1077 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4799 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1078 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4805 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1079 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4811 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1080 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4817 "parser.cpp"
    break;

  case 144: /* column_constraints: column_constraint  */
#line 1099 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4826 "parser.cpp"
    break;

  case 145: /* column_constraints: column_constraints column_constraint  */
#line 1103 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4840 "parser.cpp"
    break;

  case 146: /* column_constraint: PRIMARY KEY  */
#line 1113 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4848 "parser.cpp"
    break;

  case 147: /* column_constraint: UNIQUE  */
#line 1116 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4856 "parser.cpp"
    break;

  case 148: /* column_constraint: NULLABLE  */
#line 1119 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4864 "parser.cpp"
    break;

  case 149: /* column_constraint: NOT NULLABLE  */
#line 1122 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4872 "parser.cpp"
    break;

  case 150: /* default_expr: DEFAULT constant_expr  */
#line 1126 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4880 "parser.cpp"
    break;

  case 151: /* default_expr: %empty  */
#line 1129 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4888 "parser.cpp"
    break;

  case 152: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1134 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4898 "parser.cpp"
    break;

  case 153: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1139 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4908 "parser.cpp"
    break;

  case 154: /* identifier_array: IDENTIFIER  */
#line 1146 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4919 "parser.cpp"
    break;

  case 155: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1152 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4930 "parser.cpp"
    break;

  case 156: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1162 "parser.y"
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
#line 4947 "parser.cpp"
    break;

  case 157: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1178 "parser.y"
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
#line 4990 "parser.cpp"
    break;

  case 158: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1216 "parser.y"
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
#line 5010 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1232 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5018 "parser.cpp"
    break;

  case 160: /* optional_identifier_array: %empty  */
#line 1235 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5026 "parser.cpp"
    break;

  case 161: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1242 "parser.y"
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
#line 5044 "parser.cpp"
    break;

  case 162: /* explain_statement: EXPLAIN explainable_statement  */
#line 1254 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5054 "parser.cpp"
    break;

  case 163: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1263 "parser.y"
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
#line 5071 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr  */
#line 1276 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5080 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1280 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5089 "parser.cpp"
    break;

  case 166: /* update_expr: IDENTIFIER '=' expr  */
#line 1285 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5101 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1298 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5117 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1311 "parser.y"
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
#line 5135 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1326 "parser.y"
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
#line 5153 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1341 "parser.y"
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
#line 5171 "parser.cpp"
    break;

  case 171: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1356 "parser.y"
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
#line 5194 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1379 "parser.y"
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
#line 5252 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1432 "parser.y"
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
#line 5312 "parser.cpp"
    break;

  case 174: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1487 "parser.y"
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
#line 5364 "parser.cpp"
    break;

  case 175: /* select_statement: select_without_paren  */
#line 1538 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5372 "parser.cpp"
    break;

  case 176: /* select_statement: select_with_paren  */
#line 1541 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5380 "parser.cpp"
    break;

  case 177: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1544 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5394 "parser.cpp"
    break;

  case 178: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1553 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5408 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_without_paren ')'  */
#line 1563 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5416 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_with_paren ')'  */
#line 1566 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5424 "parser.cpp"
    break;

  case 181: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1570 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5433 "parser.cpp"
    break;

  case 182: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1575 "parser.y"
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
#line 5464 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1602 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5472 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1605 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5480 "parser.cpp"
    break;

  case 185: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause unnest_clause search_clause where_clause group_by_clause having_clause  */
#line 1610 "parser.y"
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
#line 5502 "parser.cpp"
    break;

  case 186: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1628 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5510 "parser.cpp"
    break;

  case 187: /* order_by_clause: %empty  */
#line 1631 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5518 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr  */
#line 1635 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5527 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1639 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5536 "parser.cpp"
    break;

  case 190: /* order_by_expr: expr order_by_type  */
#line 1644 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5546 "parser.cpp"
    break;

  case 191: /* order_by_type: ASC  */
#line 1650 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5554 "parser.cpp"
    break;

  case 192: /* order_by_type: DESC  */
#line 1653 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5562 "parser.cpp"
    break;

  case 193: /* order_by_type: %empty  */
#line 1656 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5570 "parser.cpp"
    break;

  case 194: /* limit_expr: LIMIT expr  */
#line 1660 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5578 "parser.cpp"
    break;

  case 195: /* limit_expr: %empty  */
#line 1664 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5584 "parser.cpp"
    break;

  case 196: /* offset_expr: OFFSET expr  */
#line 1666 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5592 "parser.cpp"
    break;

  case 197: /* offset_expr: %empty  */
#line 1670 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5598 "parser.cpp"
    break;

  case 198: /* distinct: DISTINCT  */
#line 1672 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5606 "parser.cpp"
    break;

  case 199: /* distinct: %empty  */
#line 1675 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5614 "parser.cpp"
    break;

  case 200: /* unnest_clause: UNNEST expr_array  */
#line 1679 "parser.y"
                                 {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5622 "parser.cpp"
    break;

  case 201: /* unnest_clause: %empty  */
#line 1682 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5630 "parser.cpp"
    break;

  case 202: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1686 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5638 "parser.cpp"
    break;

  case 203: /* highlight_clause: %empty  */
#line 1689 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5646 "parser.cpp"
    break;

  case 204: /* from_clause: FROM table_reference  */
#line 1693 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5654 "parser.cpp"
    break;

  case 205: /* from_clause: %empty  */
#line 1696 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5662 "parser.cpp"
    break;

  case 206: /* search_clause: SEARCH sub_search_array  */
#line 1700 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5672 "parser.cpp"
    break;

  case 207: /* search_clause: %empty  */
#line 1705 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5680 "parser.cpp"
    break;

  case 208: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1709 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5688 "parser.cpp"
    break;

  case 209: /* optional_search_filter_expr: %empty  */
#line 1712 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5696 "parser.cpp"
    break;

  case 210: /* where_clause: WHERE expr  */
#line 1716 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5704 "parser.cpp"
    break;

  case 211: /* where_clause: %empty  */
#line 1719 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5712 "parser.cpp"
    break;

  case 212: /* having_clause: HAVING expr  */
#line 1723 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5720 "parser.cpp"
    break;

  case 213: /* having_clause: %empty  */
#line 1726 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5728 "parser.cpp"
    break;

  case 214: /* group_by_clause: GROUP BY expr_array  */
#line 1730 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5736 "parser.cpp"
    break;

  case 215: /* group_by_clause: %empty  */
#line 1733 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5744 "parser.cpp"
    break;

  case 216: /* set_operator: UNION  */
#line 1737 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5752 "parser.cpp"
    break;

  case 217: /* set_operator: UNION ALL  */
#line 1740 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5760 "parser.cpp"
    break;

  case 218: /* set_operator: INTERSECT  */
#line 1743 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5768 "parser.cpp"
    break;

  case 219: /* set_operator: EXCEPT  */
#line 1746 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5776 "parser.cpp"
    break;

  case 220: /* table_reference: table_reference_unit  */
#line 1754 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5784 "parser.cpp"
    break;

  case 221: /* table_reference: table_reference ',' table_reference_unit  */
#line 1757 "parser.y"
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
#line 5802 "parser.cpp"
    break;

  case 224: /* table_reference_name: table_name table_alias  */
#line 1774 "parser.y"
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
#line 5820 "parser.cpp"
    break;

  case 225: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1788 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5831 "parser.cpp"
    break;

  case 226: /* table_name: IDENTIFIER  */
#line 1797 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5841 "parser.cpp"
    break;

  case 227: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1802 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5853 "parser.cpp"
    break;

  case 228: /* table_alias: AS IDENTIFIER  */
#line 1811 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5863 "parser.cpp"
    break;

  case 229: /* table_alias: IDENTIFIER  */
#line 1816 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5873 "parser.cpp"
    break;

  case 230: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1821 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5884 "parser.cpp"
    break;

  case 231: /* table_alias: %empty  */
#line 1827 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5892 "parser.cpp"
    break;

  case 232: /* with_clause: WITH with_expr_list  */
#line 1834 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5900 "parser.cpp"
    break;

  case 233: /* with_clause: %empty  */
#line 1837 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5908 "parser.cpp"
    break;

  case 234: /* with_expr_list: with_expr  */
#line 1841 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5917 "parser.cpp"
    break;

  case 235: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1844 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5926 "parser.cpp"
    break;

  case 236: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1849 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5938 "parser.cpp"
    break;

  case 237: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1861 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5950 "parser.cpp"
    break;

  case 238: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1868 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5963 "parser.cpp"
    break;

  case 239: /* join_type: INNER  */
#line 1882 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5971 "parser.cpp"
    break;

  case 240: /* join_type: LEFT  */
#line 1885 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5979 "parser.cpp"
    break;

  case 241: /* join_type: RIGHT  */
#line 1888 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5987 "parser.cpp"
    break;

  case 242: /* join_type: OUTER  */
#line 1891 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5995 "parser.cpp"
    break;

  case 243: /* join_type: FULL  */
#line 1894 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6003 "parser.cpp"
    break;

  case 244: /* join_type: CROSS  */
#line 1897 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6011 "parser.cpp"
    break;

  case 245: /* join_type: %empty  */
#line 1900 "parser.y"
                {
}
#line 6018 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW DATABASES  */
#line 1906 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6027 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLES  */
#line 1910 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6036 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW VIEWS  */
#line 1914 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6045 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW CONFIGS  */
#line 1918 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6054 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1922 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6066 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW PROFILES  */
#line 1929 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6075 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW BUFFER  */
#line 1933 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6084 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW MEMINDEX  */
#line 1937 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6093 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW QUERIES  */
#line 1941 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6102 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1945 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6112 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TRANSACTIONS  */
#line 1950 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6121 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1954 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6131 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW SESSION VARIABLES  */
#line 1959 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6140 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1963 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6149 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1967 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6160 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1973 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6171 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1979 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6182 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name  */
#line 1985 "parser.y"
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
#line 6198 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1996 "parser.y"
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
#line 6214 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 2007 "parser.y"
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
#line 6230 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 2018 "parser.y"
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
#line 6247 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 2030 "parser.y"
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
#line 6264 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2042 "parser.y"
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
#line 6282 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2055 "parser.y"
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
#line 6301 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2069 "parser.y"
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
#line 6317 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2080 "parser.y"
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
#line 6336 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2094 "parser.y"
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
#line 6357 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2110 "parser.y"
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
#line 6379 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW LOGS  */
#line 2127 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6388 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW DELTA LOGS  */
#line 2131 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6397 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW CATALOGS  */
#line 2135 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6406 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2139 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6415 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2143 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6424 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2147 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6435 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW MEMORY  */
#line 2153 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6444 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2157 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6453 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2161 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6462 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2165 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6473 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW SNAPSHOTS  */
#line 2171 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6482 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2175 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6493 "parser.cpp"
    break;

  case 286: /* flush_statement: FLUSH DATA  */
#line 2185 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6502 "parser.cpp"
    break;

  case 287: /* flush_statement: FLUSH LOG  */
#line 2189 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6511 "parser.cpp"
    break;

  case 288: /* flush_statement: FLUSH BUFFER  */
#line 2193 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6520 "parser.cpp"
    break;

  case 289: /* optimize_statement: OPTIMIZE table_name  */
#line 2201 "parser.y"
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
#line 6535 "parser.cpp"
    break;

  case 290: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2212 "parser.y"
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
#line 6559 "parser.cpp"
    break;

  case 291: /* command_statement: USE IDENTIFIER  */
#line 2235 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6570 "parser.cpp"
    break;

  case 292: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2241 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6580 "parser.cpp"
    break;

  case 293: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2246 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6591 "parser.cpp"
    break;

  case 294: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2252 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6602 "parser.cpp"
    break;

  case 295: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2258 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6615 "parser.cpp"
    break;

  case 296: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2266 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6626 "parser.cpp"
    break;

  case 297: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2272 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6637 "parser.cpp"
    break;

  case 298: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2278 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6648 "parser.cpp"
    break;

  case 299: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2284 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6659 "parser.cpp"
    break;

  case 300: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2290 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6672 "parser.cpp"
    break;

  case 301: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2298 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6683 "parser.cpp"
    break;

  case 302: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2304 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6694 "parser.cpp"
    break;

  case 303: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2310 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6705 "parser.cpp"
    break;

  case 304: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2316 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6716 "parser.cpp"
    break;

  case 305: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2322 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6729 "parser.cpp"
    break;

  case 306: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2330 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6740 "parser.cpp"
    break;

  case 307: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2336 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6751 "parser.cpp"
    break;

  case 308: /* command_statement: LOCK TABLE table_name  */
#line 2342 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6765 "parser.cpp"
    break;

  case 309: /* command_statement: UNLOCK TABLE table_name  */
#line 2351 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6779 "parser.cpp"
    break;

  case 310: /* command_statement: CREATE TABLE SNAPSHOT IDENTIFIER  */
#line 2360 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6790 "parser.cpp"
    break;

  case 311: /* command_statement: CREATE DATABASE SNAPSHOT IDENTIFIER  */
#line 2366 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6801 "parser.cpp"
    break;

  case 312: /* command_statement: CREATE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2372 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[0].str_value));
}
#line 6812 "parser.cpp"
    break;

  case 313: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2378 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6823 "parser.cpp"
    break;

  case 314: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2384 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6834 "parser.cpp"
    break;

  case 315: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2390 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6845 "parser.cpp"
    break;

  case 316: /* compact_statement: COMPACT TABLE table_name  */
#line 2397 "parser.y"
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
#line 6862 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2410 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6871 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2414 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6881 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2419 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6892 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2425 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6904 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2432 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6917 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2440 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6931 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2449 "parser.y"
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
#line 6946 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2459 "parser.y"
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
#line 6961 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2469 "parser.y"
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
#line 6977 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2480 "parser.y"
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
#line 6993 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2491 "parser.y"
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
#line 7010 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2503 "parser.y"
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
#line 7027 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2515 "parser.y"
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
#line 7042 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2525 "parser.y"
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
#line 7058 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2536 "parser.y"
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
#line 7075 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2548 "parser.y"
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
#line 7093 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW LOGS  */
#line 2561 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7102 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2565 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7112 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2570 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7122 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2575 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7133 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2581 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7142 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2585 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7151 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2589 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7162 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2595 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7171 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2599 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7180 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2603 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7191 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2609 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7202 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2615 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7215 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2623 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7226 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW NODES  */
#line 2629 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7235 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2633 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7246 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SHOW NODE  */
#line 2639 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7255 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2643 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7266 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SET ADMIN  */
#line 2649 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7276 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SET STANDALONE  */
#line 2654 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7286 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2659 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7298 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2666 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7312 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2675 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7326 "parser.cpp"
    break;

  case 355: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2685 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7340 "parser.cpp"
    break;

  case 356: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2694 "parser.y"
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
#line 7357 "parser.cpp"
    break;

  case 357: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2706 "parser.y"
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
#line 7373 "parser.cpp"
    break;

  case 358: /* expr_array: expr_alias  */
#line 2722 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7382 "parser.cpp"
    break;

  case 359: /* expr_array: expr_array ',' expr_alias  */
#line 2726 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7391 "parser.cpp"
    break;

  case 360: /* insert_row_list: '(' expr_array ')'  */
#line 2731 "parser.y"
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
#line 7406 "parser.cpp"
    break;

  case 361: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2741 "parser.y"
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
#line 7421 "parser.cpp"
    break;

  case 362: /* expr_alias: expr AS IDENTIFIER  */
#line 2763 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7432 "parser.cpp"
    break;

  case 363: /* expr_alias: expr  */
#line 2769 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7440 "parser.cpp"
    break;

  case 369: /* operand: '(' expr ')'  */
#line 2779 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7448 "parser.cpp"
    break;

  case 370: /* operand: '(' select_without_paren ')'  */
#line 2782 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7459 "parser.cpp"
    break;

  case 371: /* operand: constant_expr  */
#line 2788 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7467 "parser.cpp"
    break;

  case 382: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2804 "parser.y"
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
#line 7487 "parser.cpp"
    break;

  case 383: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2820 "parser.y"
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
#line 7508 "parser.cpp"
    break;

  case 384: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2837 "parser.y"
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
#line 7529 "parser.cpp"
    break;

  case 385: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2855 "parser.y"
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
#line 7579 "parser.cpp"
    break;

  case 386: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2901 "parser.y"
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
#line 7622 "parser.cpp"
    break;

  case 387: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2940 "parser.y"
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
#line 7669 "parser.cpp"
    break;

  case 388: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2983 "parser.y"
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
#line 7717 "parser.cpp"
    break;

  case 389: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3030 "parser.y"
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
#line 7745 "parser.cpp"
    break;

  case 390: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3054 "parser.y"
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
#line 7772 "parser.cpp"
    break;

  case 391: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3077 "parser.y"
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
#line 7797 "parser.cpp"
    break;

  case 392: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3098 "parser.y"
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
#line 7822 "parser.cpp"
    break;

  case 393: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3119 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7836 "parser.cpp"
    break;

  case 394: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3128 "parser.y"
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
#line 7852 "parser.cpp"
    break;

  case 395: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3139 "parser.y"
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
#line 7868 "parser.cpp"
    break;

  case 396: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3150 "parser.y"
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
#line 7886 "parser.cpp"
    break;

  case 397: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3164 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7898 "parser.cpp"
    break;

  case 398: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3171 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7912 "parser.cpp"
    break;

  case 399: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3181 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7923 "parser.cpp"
    break;

  case 400: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3187 "parser.y"
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
#line 7939 "parser.cpp"
    break;

  case 401: /* sub_search: match_vector_expr  */
#line 3199 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7947 "parser.cpp"
    break;

  case 402: /* sub_search: match_text_expr  */
#line 3202 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7955 "parser.cpp"
    break;

  case 403: /* sub_search: match_tensor_expr  */
#line 3205 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7963 "parser.cpp"
    break;

  case 404: /* sub_search: match_sparse_expr  */
#line 3208 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7971 "parser.cpp"
    break;

  case 405: /* sub_search: query_expr  */
#line 3211 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7979 "parser.cpp"
    break;

  case 406: /* sub_search: fusion_expr  */
#line 3214 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7987 "parser.cpp"
    break;

  case 407: /* sub_search_array: sub_search  */
#line 3218 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7996 "parser.cpp"
    break;

  case 408: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3222 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8005 "parser.cpp"
    break;

  case 409: /* function_expr: IDENTIFIER '(' ')'  */
#line 3227 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8018 "parser.cpp"
    break;

  case 410: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3235 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8031 "parser.cpp"
    break;

  case 411: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3243 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8045 "parser.cpp"
    break;

  case 412: /* function_expr: operand IS NOT NULLABLE  */
#line 3252 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8057 "parser.cpp"
    break;

  case 413: /* function_expr: operand IS NULLABLE  */
#line 3259 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8069 "parser.cpp"
    break;

  case 414: /* function_expr: NOT operand  */
#line 3266 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8081 "parser.cpp"
    break;

  case 415: /* function_expr: '-' operand  */
#line 3273 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8093 "parser.cpp"
    break;

  case 416: /* function_expr: '+' operand  */
#line 3280 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8105 "parser.cpp"
    break;

  case 417: /* function_expr: operand '-' operand  */
#line 3287 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8118 "parser.cpp"
    break;

  case 418: /* function_expr: operand '+' operand  */
#line 3295 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8131 "parser.cpp"
    break;

  case 419: /* function_expr: operand '*' operand  */
#line 3303 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8144 "parser.cpp"
    break;

  case 420: /* function_expr: operand '/' operand  */
#line 3311 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8157 "parser.cpp"
    break;

  case 421: /* function_expr: operand '%' operand  */
#line 3319 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8170 "parser.cpp"
    break;

  case 422: /* function_expr: operand '=' operand  */
#line 3327 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8183 "parser.cpp"
    break;

  case 423: /* function_expr: operand EQUAL operand  */
#line 3335 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8196 "parser.cpp"
    break;

  case 424: /* function_expr: operand NOT_EQ operand  */
#line 3343 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8209 "parser.cpp"
    break;

  case 425: /* function_expr: operand '<' operand  */
#line 3351 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8222 "parser.cpp"
    break;

  case 426: /* function_expr: operand '>' operand  */
#line 3359 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8235 "parser.cpp"
    break;

  case 427: /* function_expr: operand LESS_EQ operand  */
#line 3367 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8248 "parser.cpp"
    break;

  case 428: /* function_expr: operand GREATER_EQ operand  */
#line 3375 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8261 "parser.cpp"
    break;

  case 429: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3383 "parser.y"
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
#line 8296 "parser.cpp"
    break;

  case 430: /* function_expr: operand LIKE operand  */
#line 3413 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8309 "parser.cpp"
    break;

  case 431: /* function_expr: operand NOT LIKE operand  */
#line 3421 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8322 "parser.cpp"
    break;

  case 432: /* conjunction_expr: expr AND expr  */
#line 3430 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8335 "parser.cpp"
    break;

  case 433: /* conjunction_expr: expr OR expr  */
#line 3438 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8348 "parser.cpp"
    break;

  case 434: /* between_expr: operand BETWEEN operand AND operand  */
#line 3447 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8360 "parser.cpp"
    break;

  case 435: /* in_expr: operand IN '(' expr_array ')'  */
#line 3455 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8371 "parser.cpp"
    break;

  case 436: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3461 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8382 "parser.cpp"
    break;

  case 437: /* case_expr: CASE expr case_check_array END  */
#line 3468 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8393 "parser.cpp"
    break;

  case 438: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3474 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8405 "parser.cpp"
    break;

  case 439: /* case_expr: CASE case_check_array END  */
#line 3481 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8415 "parser.cpp"
    break;

  case 440: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3486 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8426 "parser.cpp"
    break;

  case 441: /* case_check_array: WHEN expr THEN expr  */
#line 3493 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8438 "parser.cpp"
    break;

  case 442: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3500 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8450 "parser.cpp"
    break;

  case 443: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3508 "parser.y"
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
#line 8481 "parser.cpp"
    break;

  case 444: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3535 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8492 "parser.cpp"
    break;

  case 445: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3541 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8503 "parser.cpp"
    break;

  case 446: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3547 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8515 "parser.cpp"
    break;

  case 447: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3554 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8527 "parser.cpp"
    break;

  case 448: /* column_expr: IDENTIFIER  */
#line 3562 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8539 "parser.cpp"
    break;

  case 449: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3569 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8551 "parser.cpp"
    break;

  case 450: /* column_expr: '*'  */
#line 3576 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8561 "parser.cpp"
    break;

  case 451: /* column_expr: column_expr '.' '*'  */
#line 3581 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8575 "parser.cpp"
    break;

  case 452: /* constant_expr: STRING  */
#line 3591 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8585 "parser.cpp"
    break;

  case 453: /* constant_expr: TRUE  */
#line 3596 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8595 "parser.cpp"
    break;

  case 454: /* constant_expr: FALSE  */
#line 3601 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8605 "parser.cpp"
    break;

  case 455: /* constant_expr: DOUBLE_VALUE  */
#line 3606 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8615 "parser.cpp"
    break;

  case 456: /* constant_expr: LONG_VALUE  */
#line 3611 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8625 "parser.cpp"
    break;

  case 457: /* constant_expr: DATE STRING  */
#line 3616 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8635 "parser.cpp"
    break;

  case 458: /* constant_expr: TIME STRING  */
#line 3621 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8645 "parser.cpp"
    break;

  case 459: /* constant_expr: DATETIME STRING  */
#line 3626 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8655 "parser.cpp"
    break;

  case 460: /* constant_expr: TIMESTAMP STRING  */
#line 3631 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8665 "parser.cpp"
    break;

  case 461: /* constant_expr: INTERVAL interval_expr  */
#line 3636 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8673 "parser.cpp"
    break;

  case 462: /* constant_expr: interval_expr  */
#line 3639 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8681 "parser.cpp"
    break;

  case 463: /* constant_expr: common_array_expr  */
#line 3642 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8689 "parser.cpp"
    break;

  case 464: /* common_array_expr: array_expr  */
#line 3646 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8697 "parser.cpp"
    break;

  case 465: /* common_array_expr: subarray_array_expr  */
#line 3649 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8705 "parser.cpp"
    break;

  case 466: /* common_array_expr: sparse_array_expr  */
#line 3652 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8713 "parser.cpp"
    break;

  case 467: /* common_array_expr: empty_array_expr  */
#line 3655 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8721 "parser.cpp"
    break;

  case 468: /* common_sparse_array_expr: sparse_array_expr  */
#line 3659 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8729 "parser.cpp"
    break;

  case 469: /* common_sparse_array_expr: array_expr  */
#line 3662 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8737 "parser.cpp"
    break;

  case 470: /* common_sparse_array_expr: empty_array_expr  */
#line 3665 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8745 "parser.cpp"
    break;

  case 471: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3669 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8753 "parser.cpp"
    break;

  case 472: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3673 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8763 "parser.cpp"
    break;

  case 473: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3678 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8772 "parser.cpp"
    break;

  case 474: /* sparse_array_expr: long_sparse_array_expr  */
#line 3683 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8780 "parser.cpp"
    break;

  case 475: /* sparse_array_expr: double_sparse_array_expr  */
#line 3686 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8788 "parser.cpp"
    break;

  case 476: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3690 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8796 "parser.cpp"
    break;

  case 477: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3694 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8808 "parser.cpp"
    break;

  case 478: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3701 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8819 "parser.cpp"
    break;

  case 479: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3708 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8827 "parser.cpp"
    break;

  case 480: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3712 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8839 "parser.cpp"
    break;

  case 481: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3719 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8850 "parser.cpp"
    break;

  case 482: /* empty_array_expr: '[' ']'  */
#line 3726 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8858 "parser.cpp"
    break;

  case 483: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3730 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8866 "parser.cpp"
    break;

  case 484: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3734 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8874 "parser.cpp"
    break;

  case 485: /* array_expr: long_array_expr  */
#line 3738 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8882 "parser.cpp"
    break;

  case 486: /* array_expr: double_array_expr  */
#line 3741 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8890 "parser.cpp"
    break;

  case 487: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3745 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8898 "parser.cpp"
    break;

  case 488: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3749 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8908 "parser.cpp"
    break;

  case 489: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3754 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8917 "parser.cpp"
    break;

  case 490: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3759 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8925 "parser.cpp"
    break;

  case 491: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3763 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8935 "parser.cpp"
    break;

  case 492: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3768 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8944 "parser.cpp"
    break;

  case 493: /* interval_expr: LONG_VALUE SECONDS  */
#line 3773 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8955 "parser.cpp"
    break;

  case 494: /* interval_expr: LONG_VALUE SECOND  */
#line 3779 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8966 "parser.cpp"
    break;

  case 495: /* interval_expr: LONG_VALUE MINUTES  */
#line 3785 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8977 "parser.cpp"
    break;

  case 496: /* interval_expr: LONG_VALUE MINUTE  */
#line 3791 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8988 "parser.cpp"
    break;

  case 497: /* interval_expr: LONG_VALUE HOURS  */
#line 3797 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8999 "parser.cpp"
    break;

  case 498: /* interval_expr: LONG_VALUE HOUR  */
#line 3803 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9010 "parser.cpp"
    break;

  case 499: /* interval_expr: LONG_VALUE DAYS  */
#line 3809 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9021 "parser.cpp"
    break;

  case 500: /* interval_expr: LONG_VALUE DAY  */
#line 3815 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9032 "parser.cpp"
    break;

  case 501: /* interval_expr: LONG_VALUE MONTHS  */
#line 3821 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9043 "parser.cpp"
    break;

  case 502: /* interval_expr: LONG_VALUE MONTH  */
#line 3827 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9054 "parser.cpp"
    break;

  case 503: /* interval_expr: LONG_VALUE YEARS  */
#line 3833 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9065 "parser.cpp"
    break;

  case 504: /* interval_expr: LONG_VALUE YEAR  */
#line 3839 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9076 "parser.cpp"
    break;

  case 505: /* copy_option_list: copy_option  */
#line 3850 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9085 "parser.cpp"
    break;

  case 506: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3854 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9094 "parser.cpp"
    break;

  case 507: /* copy_option: FORMAT IDENTIFIER  */
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
#line 9130 "parser.cpp"
    break;

  case 508: /* copy_option: DELIMITER STRING  */
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
#line 9145 "parser.cpp"
    break;

  case 509: /* copy_option: HEADER  */
#line 3900 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9155 "parser.cpp"
    break;

  case 510: /* copy_option: OFFSET LONG_VALUE  */
#line 3905 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9165 "parser.cpp"
    break;

  case 511: /* copy_option: LIMIT LONG_VALUE  */
#line 3910 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9175 "parser.cpp"
    break;

  case 512: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3915 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9185 "parser.cpp"
    break;

  case 513: /* file_path: STRING  */
#line 3921 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9193 "parser.cpp"
    break;

  case 514: /* if_exists: IF EXISTS  */
#line 3925 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9199 "parser.cpp"
    break;

  case 515: /* if_exists: %empty  */
#line 3926 "parser.y"
  { (yyval.bool_value) = false; }
#line 9205 "parser.cpp"
    break;

  case 516: /* if_not_exists: IF NOT EXISTS  */
#line 3928 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9211 "parser.cpp"
    break;

  case 517: /* if_not_exists: %empty  */
#line 3929 "parser.y"
  { (yyval.bool_value) = false; }
#line 9217 "parser.cpp"
    break;

  case 520: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3944 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9230 "parser.cpp"
    break;

  case 521: /* if_not_exists_info: %empty  */
#line 3952 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9238 "parser.cpp"
    break;

  case 522: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3956 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9246 "parser.cpp"
    break;

  case 523: /* with_index_param_list: %empty  */
#line 3959 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9254 "parser.cpp"
    break;

  case 524: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3963 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9262 "parser.cpp"
    break;

  case 525: /* optional_table_properties_list: %empty  */
#line 3966 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9270 "parser.cpp"
    break;

  case 526: /* index_param_list: index_param  */
#line 3970 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9279 "parser.cpp"
    break;

  case 527: /* index_param_list: index_param_list ',' index_param  */
#line 3974 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9288 "parser.cpp"
    break;

  case 528: /* index_param: IDENTIFIER  */
#line 3979 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9299 "parser.cpp"
    break;

  case 529: /* index_param: IDENTIFIER '=' IDENTIFIER  */
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
#line 9314 "parser.cpp"
    break;

  case 530: /* index_param: IDENTIFIER '=' STRING  */
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
#line 9329 "parser.cpp"
    break;

  case 531: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4005 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9342 "parser.cpp"
    break;

  case 532: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4013 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9355 "parser.cpp"
    break;

  case 533: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
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
#line 9391 "parser.cpp"
    break;

  case 534: /* index_info: '(' IDENTIFIER ')'  */
#line 4055 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9402 "parser.cpp"
    break;


#line 9406 "parser.cpp"

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

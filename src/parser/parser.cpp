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
#define YYFINAL  125
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1542

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  222
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  117
/* YYNRULES -- Number of rules.  */
#define YYNRULES  532
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1205

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
    2153,  2157,  2161,  2165,  2175,  2179,  2183,  2191,  2202,  2225,
    2231,  2236,  2242,  2248,  2256,  2262,  2268,  2274,  2280,  2288,
    2294,  2300,  2306,  2312,  2320,  2326,  2332,  2341,  2350,  2356,
    2362,  2368,  2374,  2380,  2387,  2400,  2404,  2409,  2415,  2422,
    2430,  2439,  2449,  2459,  2470,  2481,  2493,  2505,  2515,  2526,
    2538,  2551,  2555,  2560,  2565,  2571,  2575,  2579,  2585,  2589,
    2593,  2599,  2605,  2613,  2619,  2623,  2629,  2633,  2639,  2644,
    2649,  2656,  2665,  2675,  2684,  2696,  2712,  2716,  2721,  2731,
    2753,  2759,  2763,  2764,  2765,  2766,  2767,  2769,  2772,  2778,
    2781,  2782,  2783,  2784,  2785,  2786,  2787,  2788,  2789,  2790,
    2794,  2810,  2827,  2845,  2891,  2930,  2973,  3020,  3044,  3067,
    3088,  3109,  3118,  3129,  3140,  3154,  3161,  3171,  3177,  3189,
    3192,  3195,  3198,  3201,  3204,  3208,  3212,  3217,  3225,  3233,
    3242,  3249,  3256,  3263,  3270,  3277,  3285,  3293,  3301,  3309,
    3317,  3325,  3333,  3341,  3349,  3357,  3365,  3373,  3403,  3411,
    3420,  3428,  3437,  3445,  3451,  3458,  3464,  3471,  3476,  3483,
    3490,  3498,  3525,  3531,  3537,  3544,  3552,  3559,  3566,  3571,
    3581,  3586,  3591,  3596,  3601,  3606,  3611,  3616,  3621,  3626,
    3629,  3632,  3636,  3639,  3642,  3645,  3649,  3652,  3655,  3659,
    3663,  3668,  3673,  3676,  3680,  3684,  3691,  3698,  3702,  3709,
    3716,  3720,  3724,  3728,  3731,  3735,  3739,  3744,  3749,  3753,
    3758,  3763,  3769,  3775,  3781,  3787,  3793,  3799,  3805,  3811,
    3817,  3823,  3829,  3840,  3844,  3849,  3880,  3890,  3895,  3900,
    3905,  3911,  3915,  3916,  3918,  3919,  3921,  3922,  3934,  3942,
    3946,  3949,  3953,  3956,  3960,  3964,  3969,  3975,  3985,  3995,
    4003,  4014,  4045
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

#define YYPACT_NINF (-741)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-520)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     785,   603,    30,   182,    51,    11,    51,   218,   688,   735,
     110,   127,   139,   171,   326,   220,   266,   295,   131,   119,
      90,   -53,   328,    98,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,   310,  -741,  -741,   348,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,   -41,   -33,   292,   292,   148,   180,    51,
     330,   330,   330,   330,   330,   414,   193,   407,    51,   -35,
     437,   448,   457,   812,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,   310,  -741,  -741,  -741,  -741,  -741,   264,   489,    51,
    -741,  -741,  -741,  -741,  -741,   496,  -741,   213,   353,  -741,
     493,  -741,   333,  -741,  -741,   509,  -741,   349,   172,    51,
      51,    51,    51,  -741,  -741,  -741,  -741,   -38,  -741,   462,
     313,  -741,   529,   342,   378,   247,   800,   380,   582,   400,
     515,   405,   409,   385,   393,  -741,    69,  -741,   595,  -741,
    -741,     9,   562,  -741,   564,   556,   632,   652,   656,    51,
      51,    51,   666,   571,   668,   466,   614,   691,    51,    51,
      51,   700,  -741,   702,   704,   671,   728,   728,    67,    41,
      45,    58,  -741,   534,  -741,   601,  -741,  -741,   751,  -741,
     753,  -741,  -741,  -741,   760,  -741,  -741,  -741,  -741,   373,
    -741,  -741,  -741,    51,   551,   295,   728,  -741,   766,  -741,
     615,  -741,   775,  -741,  -741,   782,  -741,  -741,   780,  -741,
     787,   790,  -741,   799,   749,   801,   611,   809,   810,  -741,
    -741,  -741,  -741,     9,  -741,  -741,  -741,    67,   764,   750,
     746,  -741,   693,  -741,     3,  -741,   466,  -741,    51,  -741,
     826,    94,  -741,  -741,  -741,  -741,  -741,   768,  -741,   625,
     -21,  -741,    67,  -741,  -741,   754,   755,   622,  -741,  -741,
     996,   549,   623,   624,   354,   840,   841,   842,   844,  -741,
    -741,   843,   634,   298,   635,   636,   573,   573,  -741,    29,
     410,   178,  -741,   105,   715,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,   637,  -741,
    -741,  -741,  -125,  -741,  -741,  -118,  -741,   100,  -741,  -741,
    -741,   181,  -741,   221,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,   850,   848,  -741,  -741,  -741,  -741,  -741,  -741,   811,
     813,   779,   781,   348,  -741,  -741,  -741,   857,   293,  -741,
     856,  -741,  -741,   793,   355,  -741,   860,  -741,  -741,   654,
     655,   -52,    67,    67,   806,  -741,   870,   -53,    46,   821,
     661,  -741,   212,   663,  -741,    51,    67,   704,  -741,   484,
     664,   665,   289,  -741,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,   573,   667,   805,   804,    67,
      67,   149,   359,  -741,  -741,  -741,  -741,   996,  -741,   880,
     669,   672,   673,   675,   883,   888,   503,   503,  -741,   674,
    -741,  -741,  -741,  -741,   676,   151,   817,    67,   897,    67,
      67,   -43,   687,    33,   573,   573,   573,   573,   573,   573,
     573,   573,   573,   573,   573,   573,   573,   573,    25,  -741,
     694,  -741,   903,  -741,   907,  -741,   908,  -741,   910,   873,
     602,   703,   713,   917,   714,  -741,   716,  -741,   925,  -741,
     303,   928,   770,   771,  -741,  -741,  -741,    67,   863,   721,
    -741,   188,   484,    67,  -741,  -741,   108,   939,   815,   724,
     250,  -741,  -741,  -741,   -53,   940,   808,  -741,   942,    67,
     729,  -741,   484,  -741,   194,   194,    67,  -741,   277,   804,
     789,   733,   146,   -34,   364,  -741,    67,    67,   872,    67,
     947,    37,    67,   732,   284,   720,  -741,  -741,   728,  -741,
    -741,  -741,   798,   743,   573,   410,   830,  -741,   814,   814,
     312,   312,   197,   814,   814,   312,   312,   503,   503,  -741,
    -741,  -741,  -741,  -741,  -741,   741,  -741,   747,  -741,  -741,
    -741,   963,   969,  -741,   826,   976,  -741,   977,  -741,  -741,
     978,  -741,  -741,   981,   982,   762,    19,   898,    67,  -741,
    -741,  -741,   484,   984,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,   773,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,   774,   778,
     783,   784,   786,   788,   209,   791,   826,   959,    46,   310,
     792,   993,  -741,   290,   803,   997,   999,   994,  1003,  -741,
    1002,   339,  -741,   340,   341,  -741,   829,  -741,   939,    67,
    -741,    67,   134,    27,   573,   -67,   833,  -741,    34,   141,
      39,   837,  -741,  1052,  -741,  -741,   979,   410,   814,   854,
     352,  -741,   573,  1054,  1056,  1022,  1028,   381,   392,  -741,
     871,   397,  -741,  1070,  -741,  -741,   -53,   858,   683,  -741,
     156,  -741,    67,   900,  -741,  -741,  1082,   490,   629,   975,
     992,  1009,  1026,   958,   964,  -741,  -741,   191,  -741,   961,
     826,   398,   876,   971,  -741,   930,  -741,  -741,    67,  -741,
    -741,  -741,  -741,  -741,  -741,   194,  -741,  -741,  -741,   889,
     484,   144,  -741,    67,   455,   895,  1106,   694,   909,   891,
      67,  -741,   905,   896,   911,   408,  -741,  -741,   805,  1120,
    1121,  -741,  -741,   976,   471,  -741,   977,   432,    40,    19,
    1072,  -741,  -741,  -741,  -741,  -741,  -741,  1089,  -741,  1145,
    -741,   762,   279,   671,   463,   929,   931,   932,   933,   934,
     935,   936,   937,   938,  1057,   941,   943,   944,   945,   946,
     948,   949,   950,   951,   952,  1066,   953,   954,   955,   956,
     957,   960,   962,   965,   966,   967,  1067,   968,   970,   972,
     973,   974,   980,   983,   985,   986,   987,  1069,   988,   989,
     990,   991,   995,   998,  1000,  1001,  1004,  1005,  1074,  1006,
    1007,  1008,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1085,
    1017,  -741,  -741,   104,  -741,  1045,  1047,   470,  -741,   977,
    1179,  1181,   472,  -741,  -741,  -741,   484,  -741,   756,  1018,
    1019,  1020,    31,  1021,  -741,  -741,  -741,  1123,  1025,   484,
    -741,   194,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  1185,  -741,   156,   683,    19,    19,  1027,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  1024,  1143,  -741,  1190,
    1191,  1192,  1193,  1195,  1196,  1198,  1206,  1207,  1208,  1029,
    1210,  1211,  1213,  1216,  1217,  1223,  1239,  1240,  1241,  1242,
    1030,  1245,  1246,  1247,  1248,  1249,  1250,  1251,  1252,  1253,
    1254,  1041,  1256,  1257,  1258,  1259,  1260,  1261,  1262,  1263,
    1264,  1265,  1053,  1266,  1268,  1269,  1270,  1271,  1272,  1273,
    1274,  1275,  1276,  1063,  1278,  1279,  1280,  1281,  1282,  1283,
    1284,  1285,  1286,  1287,  1075,  1288,  -741,  1292,  1293,  -741,
     476,  -741,   781,  -741,  -741,  1294,    61,  1083,  1295,  1297,
    -741,   528,  1296,  -741,  -741,  1243,   826,   279,  1255,  1267,
    1086,  1087,  1090,  1092,  1093,  1094,  1095,  1096,  1097,  1098,
    1300,  1099,  1100,  1101,  1102,  1103,  1105,  1107,  1108,  1109,
    1110,  1317,  1111,  1112,  1113,  1114,  1115,  1116,  1117,  1118,
    1119,  1122,  1331,  1124,  1125,  1126,  1127,  1128,  1129,  1130,
    1131,  1132,  1133,  1332,  1134,  1135,  1136,  1137,  1138,  1139,
    1140,  1141,  1142,  1144,  1334,  1146,  1147,  1148,  1149,  1150,
    1151,  1152,  1153,  1154,  1155,  1354,  1156,  -741,  -741,  -741,
    -741,  1157,   891,  1197,  1158,  1159,  -741,   630,    67,   532,
    -741,    67,    67,  -741,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  1163,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  1164,  -741,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  1165,  -741,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  1166,  -741,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  1167,  -741,
    -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  -741,  1168,
    -741,  1370,  1169,  1326,  1372,    68,  1170,  1382,  1383,  -741,
    -741,  -741,   484,  -741,   762,   484,  -741,  -741,  -741,  -741,
    -741,  -741,  1171,  1225,  1176,  1173,   891,   781,  1388,   727,
      71,  1178,  1347,  1393,  1394,  1182,  -741,   731,  1395,  -741,
     891,   781,  1184,  1186,   891,   -26,  1396,  -741,  1356,  1188,
    -741,  1402,  -741,  1194,  1367,  1368,  -741,  -741,  -741,   -23,
    1199,   -14,  -741,  1201,  1369,  1373,  -741,  -741,  1374,  1375,
    1407,  -741,  1202,  -741,  1203,  1204,  1409,  1417,   781,  1205,
    1209,  -741,   781,  -741,  -741
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     233,     0,     0,     0,     0,     0,     0,     0,   233,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   233,     0,   517,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   176,   175,     0,     8,    14,    15,    16,
      17,    18,    19,   515,   515,   515,   515,   515,     0,     0,
     513,   513,   513,   513,   513,     0,   226,     0,     0,     0,
       0,     0,     0,   233,   162,    20,    25,    27,    26,    21,
      22,    24,    23,    28,    29,    30,    31,     0,     0,     0,
     247,   248,   246,   252,   256,     0,   253,     0,     0,   249,
       0,   251,     0,   274,   276,     0,   254,     0,   280,     0,
       0,     0,     0,   284,   285,   286,   289,   226,   287,     0,
     232,   234,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,   233,     2,   216,   218,
     219,     0,   199,   181,   187,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   160,     0,     0,     0,     0,
       0,     0,   311,     0,     0,   211,     0,     0,     0,     0,
       0,     0,   161,     0,   262,   263,   257,   258,     0,   259,
       0,   250,   275,   255,     0,   278,   277,   281,   282,     0,
     314,   306,   307,     0,     0,     0,     0,   338,     0,   348,
       0,   349,     0,   335,   336,     0,   331,   315,     0,   344,
     346,     0,   339,     0,     0,     0,     0,     0,     0,   180,
     179,     4,   217,     0,   177,   178,   198,     0,     0,   195,
       0,   309,    33,   308,     0,    34,   160,   518,     0,   310,
       0,   233,   512,   167,   169,   168,   170,     0,   227,     0,
     211,   164,     0,   156,   511,     0,     0,   446,   450,   453,
     454,     0,     0,     0,     0,     0,     0,     0,     0,   451,
     452,     0,     0,     0,     0,     0,     0,     0,   448,     0,
     233,     0,   356,   361,   362,   376,   374,   377,   375,   378,
     379,   371,   366,   365,   364,   372,   373,   363,   370,   369,
     461,   463,     0,   464,   472,     0,   473,     0,   465,   462,
     483,     0,   484,     0,   460,   293,   295,   294,   291,   292,
     298,   300,   299,   296,   297,   303,   305,   304,   301,   302,
     283,     0,     0,   265,   264,   270,   260,   261,   279,     0,
       0,     0,   521,     0,   235,   290,   341,     0,   332,   337,
     316,   345,   340,     0,     0,   347,     0,   312,   313,     0,
       0,   203,     0,     0,   197,   514,     0,   233,     0,     0,
       0,   154,     0,     0,   158,     0,     0,     0,   163,   210,
       0,     0,     0,   492,   491,   494,   493,   496,   495,   498,
     497,   500,   499,   502,   501,     0,     0,   412,   233,     0,
       0,     0,     0,   455,   456,   457,   458,     0,   459,     0,
       0,     0,     0,     0,     0,     0,   414,   413,   489,   486,
     480,   470,   475,   478,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   469,
       0,   474,     0,   477,     0,   485,     0,   488,     0,   271,
     266,     0,     0,     0,     0,   288,     0,   350,     0,   333,
       0,     0,     0,     0,   343,   184,   183,     0,   205,   186,
     188,   193,   194,     0,   182,    32,    36,     0,     0,     0,
       0,    42,    46,    47,   233,     0,    40,   159,     0,     0,
     157,   171,   166,   165,     0,     0,     0,   407,     0,   233,
       0,     0,     0,     0,     0,   437,     0,     0,     0,     0,
       0,     0,     0,   209,     0,     0,   368,   367,     0,   357,
     360,   430,   431,     0,     0,   233,     0,   411,   421,   422,
     425,   426,     0,   428,   420,   423,   424,   416,   415,   417,
     418,   419,   447,   449,   471,     0,   476,     0,   479,   487,
     490,     0,     0,   267,     0,     0,   353,     0,   236,   334,
       0,   317,   342,     0,     0,   202,     0,   201,     0,   191,
     192,   190,   196,     0,    52,    55,    56,    53,    54,    57,
      58,    74,    59,    61,    60,    77,    64,    65,    66,    62,
      63,    67,    68,    69,    70,    71,    72,    73,     0,     0,
       0,     0,     0,     0,   521,     0,     0,   523,     0,    39,
       0,     0,   155,     0,     0,     0,     0,     0,     0,   507,
       0,     0,   503,     0,     0,   408,     0,   442,     0,     0,
     435,     0,     0,     0,     0,     0,     0,   446,     0,     0,
       0,     0,   397,     0,   482,   481,     0,   233,   429,     0,
       0,   410,     0,     0,     0,   272,   268,     0,     0,    44,
     526,     0,   524,   318,   351,   352,   233,   204,   220,   222,
     231,   223,     0,   207,   189,    38,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   147,   148,   151,   144,   151,
       0,     0,     0,    35,    43,   532,    41,   358,     0,   509,
     508,   506,   505,   510,   174,     0,   172,   409,   443,     0,
     439,     0,   438,     0,     0,     0,     0,     0,     0,   209,
       0,   395,     0,     0,     0,     0,   444,   433,   432,     0,
       0,   355,   354,     0,     0,   520,     0,     0,     0,     0,
       0,   240,   241,   242,   243,   239,   244,     0,   229,     0,
     224,   200,     0,   211,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   149,   146,     0,   145,    49,    48,     0,   153,     0,
       0,     0,     0,   504,   441,   436,   440,   427,     0,     0,
     209,     0,     0,     0,   466,   468,   467,     0,     0,   208,
     398,     0,   445,   434,   273,   269,    45,   527,   528,   530,
     529,   525,     0,   319,   231,   221,     0,     0,   228,   401,
     399,   402,   400,   403,   404,   405,   206,   215,    76,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   150,     0,     0,   152,
       0,    37,   521,   359,   486,     0,     0,     0,     0,     0,
     396,     0,   320,   225,   237,     0,     0,     0,     0,   213,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    51,    50,   522,
     531,     0,   209,   391,     0,   209,   173,     0,     0,     0,
     406,     0,     0,   185,    75,    81,    82,    79,    80,    83,
      84,    85,    86,    87,     0,    78,   125,   126,   123,   124,
     127,   128,   129,   130,   131,     0,   122,    92,    93,    90,
      91,    94,    95,    96,    97,    98,     0,    89,   103,   104,
     101,   102,   105,   106,   107,   108,   109,     0,   100,   136,
     137,   134,   135,   138,   139,   140,   141,   142,     0,   133,
     114,   115,   112,   113,   116,   117,   118,   119,   120,     0,
     111,     0,     0,     0,     0,     0,     0,     0,     0,   322,
     321,   327,   238,   230,   214,   212,    88,   132,    99,   110,
     143,   121,   209,   392,     0,     0,   209,   521,   328,   323,
       0,     0,     0,     0,     0,     0,   390,     0,     0,   324,
     209,   521,     0,     0,   209,   521,     0,   329,   325,     0,
     386,     0,   393,     0,     0,     0,   389,   330,   326,   521,
       0,   380,   388,     0,     0,     0,   385,   394,     0,     0,
       0,   384,     0,   382,     0,     0,     0,     0,   521,     0,
       0,   387,   521,   381,   383
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -741,  -741,  -741,  1298,  1360,    83,  -741,  -741,   819,  -535,
     797,  -741,   742,   739,  -741,  -547,   248,   272,  1212,  -741,
     278,  -741,  1064,   283,   302,    -6,  1411,   -17,  1104,  1220,
     -75,  -741,  -741,   862,  -741,  -741,  -741,  -741,  -741,  -741,
    -741,  -741,  -711,  -237,  -741,  -741,  -741,  -741,   695,   -88,
      17,   572,  -741,  -741,  1277,  -741,  -741,   304,   306,   315,
     317,   318,  -741,  -741,  -217,  -741,  1023,  -241,  -240,  -740,
    -738,  -737,  -736,  -734,  -733,   468,  -741,  -741,  -741,  -741,
    -741,  -741,  1048,  -741,  -741,  1031,   618,  -263,  -741,  -741,
    -741,   725,  -741,  -741,  -741,  -741,   726,  1032,  1033,  -144,
    -741,  -741,  -741,  -741,  1187,  -485,   737,  -148,   478,   495,
    -741,  -741,  -599,  -741,   616,   710,  -741
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    22,    23,    24,    64,    25,   480,   658,   481,   482,
     604,   687,   688,   825,   483,   362,    26,    27,   231,    28,
      29,   240,   241,    30,    31,    32,    33,    34,   133,   214,
     134,   219,   469,   470,   571,   354,   474,   217,   673,   468,
     567,   753,   641,   243,  1053,   969,   131,   667,   668,   669,
     670,   750,    35,   110,   111,   671,   747,    36,    37,    38,
      39,    40,    41,    42,   271,   490,   272,   273,   274,   275,
     276,   277,   278,   279,   280,   875,   876,   281,   282,   283,
     284,   285,   392,   286,   287,   288,   289,   290,   843,   291,
     292,   293,   294,   295,   296,   297,   298,   412,   413,   299,
     300,   301,   302,   303,   304,   621,   622,   245,   147,   137,
     127,   143,   455,   693,   661,   662,   486
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     351,   369,    71,   368,   124,   689,   411,   657,   848,   246,
     623,   387,   869,   391,   870,   871,   872,   132,   873,   874,
     659,    57,    56,    59,   183,    17,   406,   407,   542,   415,
     467,   135,   523,   108,   408,   409,   408,   409,   335,   135,
     637,   156,   157,   719,   305,   242,   306,   307,   310,   477,
     311,   312,   454,   629,    56,   454,   215,    71,   357,   691,
     128,   315,   129,   316,   317,  1042,   145,    49,   130,  -516,
     247,   248,   249,   250,  1146,   155,     1,  1160,     2,     3,
       4,     5,     6,     7,     8,     9,    10,    58,   419,   420,
     439,    65,    11,    12,    13,   440,   165,   441,    14,    15,
      16,  1174,   442,   308,  1184,   720,   526,   313,   248,   249,
     250,   471,   472,  1188,   713,   524,   179,   180,   181,   182,
     318,   419,   420,   121,   122,   492,   113,   720,   128,   957,
     129,   114,   363,   115,   720,   116,   130,   720,   350,  1175,
     251,   252,  1185,   827,    99,   387,    65,    17,   502,   503,
     253,  1189,   254,   715,   136,   498,   224,   225,   226,   748,
     418,   100,   138,    21,   527,   234,   235,   236,   417,   255,
     256,   257,    17,   101,   478,   258,   479,   544,   521,   522,
     153,   158,   419,   420,   528,   529,   530,   531,   532,   533,
     534,   535,   536,   537,   538,   539,   540,   541,   856,   367,
     332,   628,   259,   260,   261,   102,   255,   256,   257,   309,
    -519,   749,   258,   314,   364,    50,    51,    52,   712,   358,
     135,    53,    54,   106,   262,   213,   319,   869,   835,   870,
     871,   872,   572,   873,   874,   666,   543,   390,    18,   259,
     260,   261,   573,   269,   410,   360,   410,   263,   268,   264,
     565,   265,   438,   414,   717,    19,    66,   864,   569,   570,
     419,   420,   615,   616,   683,   632,   633,    20,   635,   107,
     500,   639,   613,   617,   618,   619,   266,   267,   268,   624,
      67,   269,   683,   270,   648,    21,    68,   454,   117,   419,
     420,    69,   247,   248,   249,   250,   419,   420,   109,   419,
     420,   419,   420,   112,   419,   420,   419,   420,   650,   118,
      70,    66,    72,   119,    73,   443,   120,   126,   269,   684,
     444,   685,   686,    74,   823,    75,    76,   471,   125,   423,
     128,  1122,   129,   458,  1126,    67,   560,   684,   130,   685,
     686,    68,   459,   419,   420,   561,    69,   424,   425,   426,
     427,   476,   652,  1040,   496,   429,   132,   247,   248,   249,
     250,   718,   251,   252,   135,    70,   961,    72,   517,    73,
     646,   501,   253,   177,   254,   144,   178,    55,    74,   620,
      75,    76,   491,   329,    60,    61,   167,   168,   710,    62,
     711,   255,   256,   257,   714,   416,   445,   258,   417,   330,
     331,   446,   146,   430,   431,   432,   433,   434,   435,   436,
     437,   153,   728,   247,   248,   249,   250,   152,   400,  1049,
     401,   154,   402,   403,   259,   260,   261,   251,   252,   487,
     725,  1151,   488,   189,   190,  1155,   447,   253,   191,   254,
     159,   448,   390,   505,   423,   506,   262,   507,   630,  1169,
     631,   160,   507,  1173,   841,   751,   255,   256,   257,   263,
     161,   264,   258,   265,  -520,  -520,   862,   607,   863,   263,
     608,   264,   836,   265,   857,   858,   859,   860,   609,   849,
     163,   832,   626,   251,   252,   103,   104,   105,    17,   259,
     260,   261,   164,   253,   625,   254,   171,   417,   266,   267,
     268,   642,   166,   269,   643,   270,   497,   697,   649,   172,
     417,   262,   255,   256,   257,   173,   877,   184,   258,  -520,
    -520,   433,   434,   435,   436,   437,   169,   170,   500,   148,
     149,   150,   151,   185,   263,   186,   264,   187,   265,   139,
     140,   141,   142,   462,   463,   259,   260,   261,  1156,   174,
     175,   176,   247,   248,   249,   250,   704,   706,   707,   705,
     705,   417,  1170,   266,   267,   268,  1176,   262,   269,   727,
     270,   839,   417,   188,   846,   203,   247,   248,   249,   250,
    1186,   755,   756,   757,   758,   759,   204,   423,   760,   761,
     263,   206,   264,   205,   265,   762,   763,   764,   731,  1201,
     207,   488,   209,  1204,   208,   424,   425,   426,   427,   732,
     210,   765,   733,   429,   735,   828,   212,   736,   488,   266,
     267,   268,   385,   386,   269,   853,   270,   216,   417,   220,
     724,   218,   253,   228,   254,   221,    43,    44,    45,   419,
     420,   321,    46,    47,   322,   323,   385,   552,   553,   324,
     325,   255,   256,   257,    48,   222,   253,   258,   254,   223,
     738,   430,   431,   432,   433,   434,   435,   436,   437,   227,
    1127,   229,   837,  1128,  1129,   255,   256,   257,  1130,  1131,
     878,   258,   230,   879,   259,   260,   261,   949,   232,   953,
     488,    63,   417,  1039,   233,     1,   736,     2,     3,     4,
       5,     6,     7,   237,     9,   238,   262,   239,   259,   260,
     261,    11,    12,    13,   435,   436,   437,    14,    15,    16,
     766,   767,   768,   769,   770,   644,   645,   771,   772,   263,
     262,   264,   244,   265,   773,   774,   775,   242,    77,   740,
    -245,   741,   742,   743,   744,  1046,   745,   746,   705,  1133,
     776,   320,   488,   263,   326,   264,   327,   265,   266,   267,
     268,   408,   954,   269,   328,   270,    17,   333,    78,    79,
     336,    80,  1158,  1159,  1166,  1167,    81,    82,   964,   965,
     337,   338,   266,   267,   268,   339,   340,   269,   421,   270,
     422,   341,     1,   342,     2,     3,     4,     5,     6,     7,
       8,     9,    10,   343,   344,   345,   346,  1132,    11,    12,
      13,  1135,   347,   348,    14,    15,    16,   352,   353,     1,
     355,     2,     3,     4,     5,     6,     7,   356,     9,   361,
     365,   366,   370,   371,  1134,    11,    12,    13,   372,   388,
     389,    14,    15,    16,   393,   394,   395,   423,   396,   397,
     399,   404,   405,   449,   450,   438,   453,    18,   451,   454,
     452,   457,   460,    17,   464,   424,   425,   426,   427,   428,
     461,   465,   466,   429,   475,   473,   484,   485,   500,   489,
     494,   495,    17,   499,   508,   509,    20,   513,   510,   511,
      17,   512,   514,   516,   518,   515,    83,    84,    85,    86,
     520,    87,    88,   525,    21,    89,    90,    91,   269,   545,
      92,    93,    94,   547,   549,   550,   551,    95,    96,   554,
     556,   430,   431,   432,   433,   434,   435,   436,   437,   555,
     557,   559,   562,   558,    97,   563,   564,   423,    98,   566,
     606,   568,   611,   610,   605,   612,   423,   524,   634,   614,
     627,   636,   640,   419,    18,   424,   425,   426,   427,   647,
     192,   651,   653,   429,  -520,  -520,   426,   427,   654,   655,
     193,    19,  -520,   194,   195,   656,   196,   197,   198,   477,
     660,    18,   417,    20,   663,   664,   665,   672,   675,   676,
     677,   692,   199,   200,   678,   201,   202,   696,   701,   679,
     680,    21,   681,   699,   682,   700,   702,   690,   703,   695,
      20,   430,   431,   432,   433,   434,   435,   436,   437,   698,
    -520,   431,   432,   433,   434,   435,   436,   437,    21,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   587,   588,   589,   590,   708,   591,   592,   593,
     594,   595,   596,   716,   721,   597,   722,   723,   598,   599,
     645,   644,   600,   601,   602,   603,   777,   778,   779,   780,
     781,   726,   729,   782,   783,   730,   737,   734,   739,   752,
     784,   785,   786,   788,   789,   790,   791,   792,   754,   821,
     793,   794,   829,   822,   823,   831,   787,   795,   796,   797,
     799,   800,   801,   802,   803,   830,   834,   804,   805,   838,
     840,   847,   851,   798,   806,   807,   808,   810,   811,   812,
     813,   814,   850,   842,   815,   816,   854,   855,   852,   866,
     809,   817,   818,   819,   373,   374,   375,   376,   377,   378,
     379,   380,   381,   382,   383,   384,   867,   820,   868,   880,
     889,   881,   882,   883,   884,   885,   886,   887,   888,   900,
     911,   890,   922,   891,   892,   893,   894,   933,   895,   896,
     897,   898,   899,   901,   902,   903,   904,   905,   944,   947,
     906,   948,   907,   951,   952,   908,   909,   910,   912,   720,
     913,   962,   914,   915,   916,   968,   970,   971,   972,   973,
     917,   974,   975,   918,   976,   919,   920,   921,   923,   924,
     925,   926,   977,   978,   979,   927,   981,   982,   928,   983,
     929,   930,   984,   985,   931,   932,   934,   935,   936,   986,
     937,   938,   939,   940,   941,   942,   943,   945,   955,   956,
     958,   959,   960,   966,   967,   987,   988,   989,   990,   980,
     991,   992,   993,   994,   995,   996,   997,   998,   999,  1000,
    1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,
    1011,  1012,  1014,  1013,  1015,  1016,  1017,  1018,  1019,  1020,
    1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,  1029,  1030,
    1031,  1032,  1033,  1034,  1036,  1035,  1037,  1038,  1041,  1044,
    1043,  1045,  1047,  1054,  1055,  1048,  1064,  1056,  1051,  1057,
    1058,  1059,  1060,  1061,  1062,  1063,  1065,  1066,  1067,  1068,
    1069,  1052,  1070,  1075,  1071,  1072,  1073,  1074,  1076,  1077,
    1078,  1079,  1080,  1081,  1082,  1083,  1084,  1086,  1097,  1085,
    1108,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1094,  1095,
    1096,  1098,  1099,  1100,  1101,  1102,  1103,  1104,  1105,  1106,
    1119,  1107,  1123,  1109,  1110,  1111,  1112,  1113,  1114,  1115,
    1116,  1117,  1118,  1120,  1142,  1144,  1145,  1121,  1124,  1125,
    1136,  1137,  1138,  1139,  1140,  1141,  1143,  1147,  1148,  1149,
    1152,  1150,  1153,  1154,  1157,  1161,  1162,  1163,  1164,  1165,
    1171,  1168,  1177,  1172,  1178,  1179,  1180,  1182,  1183,  1191,
    1195,  1181,  1199,  1192,  1193,  1194,  1187,  1190,  1196,  1197,
    1200,  1198,  1202,   162,   211,   709,  1203,   694,   826,   824,
     674,   493,   123,   349,   865,  1050,   963,   456,   359,   504,
     519,   946,   833,   844,   845,   950,   861,     0,   398,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   334,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   546,     0,     0,   548,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   638
};

static const yytype_int16 yycheck[] =
{
     217,   242,     8,   240,    21,   604,   269,   554,   719,   157,
     495,   251,   752,   254,   752,   752,   752,     8,   752,   752,
     555,     4,     3,     6,    62,    78,   266,   267,     3,   270,
      82,    72,    75,    16,     5,     6,     5,     6,   186,    72,
       3,    76,    77,     4,     3,    66,     5,     6,     3,     3,
       5,     6,    78,    87,     3,    78,   131,    63,    55,   606,
      20,     3,    22,     5,     6,     4,    49,    37,    28,     0,
       3,     4,     5,     6,     6,    58,     7,     6,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    76,   155,   156,
     215,     8,    23,    24,    25,   220,    79,   215,    29,    30,
      31,   127,   220,    62,   127,    66,    73,    62,     4,     5,
       6,   352,   353,   127,    87,   158,    99,   100,   101,   102,
      62,   155,   156,    33,    34,   366,     7,    66,    20,   840,
      22,    12,    38,    14,    66,    16,    28,    66,   213,   165,
      73,    74,   165,   690,    34,   385,    63,    78,   389,   390,
      83,   165,    85,   220,   195,   372,   139,   140,   141,     3,
      55,    34,   195,   216,   131,   148,   149,   150,   220,   102,
     103,   104,    78,    34,   128,   108,   130,   440,   419,   420,
     218,   216,   155,   156,   424,   425,   426,   427,   428,   429,
     430,   431,   432,   433,   434,   435,   436,   437,   733,   220,
     183,    55,   135,   136,   137,    34,   102,   103,   104,   168,
      62,    55,   108,   168,   231,    33,    34,    35,    84,   216,
      72,    39,    40,     3,   157,   216,   168,   967,    84,   967,
     967,   967,   473,   967,   967,   216,   211,    88,   169,   135,
     136,   137,   134,   214,   215,   228,   215,   180,   211,   182,
     467,   184,   218,   270,   220,   186,     8,   217,    70,    71,
     155,   156,    68,    69,    73,   506,   507,   198,   509,     3,
      73,   512,   489,    79,    80,    81,   209,   210,   211,   496,
       8,   214,    73,   216,   524,   216,     8,    78,   169,   155,
     156,     8,     3,     4,     5,     6,   155,   156,     3,   155,
     156,   155,   156,   172,   155,   156,   155,   156,   525,   190,
       8,    63,     8,   194,     8,   215,   197,   219,   214,   128,
     220,   130,   131,     8,   133,     8,     8,   568,     0,   132,
      20,  1042,    22,    40,  1045,    63,    33,   128,    28,   130,
     131,    63,    49,   155,   156,    42,    63,   150,   151,   152,
     153,   357,   155,   952,    65,   158,     8,     3,     4,     5,
       6,   220,    73,    74,    72,    63,   851,    63,   217,    63,
     518,   388,    83,   201,    85,   195,   204,   195,    63,   185,
      63,    63,   365,    10,   166,   167,   173,   174,   629,   171,
     631,   102,   103,   104,   634,   217,   215,   108,   220,    26,
      27,   220,    72,   206,   207,   208,   209,   210,   211,   212,
     213,   218,   652,     3,     4,     5,     6,     3,   120,   966,
     122,    14,   124,   125,   135,   136,   137,    73,    74,   217,
     647,  1142,   220,   186,   187,  1146,   215,    83,   191,    85,
       3,   220,    88,    84,   132,    86,   157,    88,    84,  1160,
      86,     3,    88,  1164,   717,   672,   102,   103,   104,   180,
       3,   182,   108,   184,   152,   153,    34,   217,    36,   180,
     220,   182,   713,   184,     3,     4,     5,     6,   484,   720,
     216,   698,   499,    73,    74,   159,   160,   161,    78,   135,
     136,   137,     3,    83,   217,    85,     3,   220,   209,   210,
     211,   217,     6,   214,   220,   216,   217,   217,   525,   176,
     220,   157,   102,   103,   104,     6,   753,    55,   108,   207,
     208,   209,   210,   211,   212,   213,   173,   174,    73,    51,
      52,    53,    54,   220,   180,     6,   182,   195,   184,    44,
      45,    46,    47,   188,   189,   135,   136,   137,  1147,   200,
     201,   202,     3,     4,     5,     6,   217,   217,   217,   220,
     220,   220,  1161,   209,   210,   211,  1165,   157,   214,   217,
     216,   715,   220,   195,   718,   195,     3,     4,     5,     6,
    1179,    91,    92,    93,    94,    95,     4,   132,    98,    99,
     180,    76,   182,   193,   184,   105,   106,   107,   217,  1198,
     195,   220,   217,  1202,   195,   150,   151,   152,   153,   217,
     217,   121,   220,   158,   217,   217,    21,   220,   220,   209,
     210,   211,    73,    74,   214,   217,   216,    65,   220,    73,
     647,    67,    83,    62,    85,     3,    33,    34,    35,   155,
     156,    40,    39,    40,    43,    44,    73,    45,    46,    48,
      49,   102,   103,   104,    51,     3,    83,   108,    85,     3,
     666,   206,   207,   208,   209,   210,   211,   212,   213,     3,
      40,     3,   217,    43,    44,   102,   103,   104,    48,    49,
     217,   108,   216,   220,   135,   136,   137,   217,    74,   217,
     220,     3,   220,   217,     3,     7,   220,     9,    10,    11,
      12,    13,    14,     3,    16,     3,   157,     3,   135,   136,
     137,    23,    24,    25,   211,   212,   213,    29,    30,    31,
      91,    92,    93,    94,    95,     5,     6,    98,    99,   180,
     157,   182,     4,   184,   105,   106,   107,    66,     3,    56,
      57,    58,    59,    60,    61,   217,    63,    64,   220,   217,
     121,   217,   220,   180,     3,   182,     3,   184,   209,   210,
     211,     5,     6,   214,     4,   216,    78,   216,    33,    34,
       4,    36,    45,    46,    43,    44,    41,    42,   866,   867,
     165,     6,   209,   210,   211,     3,     6,   214,    73,   216,
      75,     4,     7,     3,     9,    10,    11,    12,    13,    14,
      15,    16,    17,     4,    55,     4,   195,  1048,    23,    24,
      25,  1052,     3,     3,    29,    30,    31,    53,    68,     7,
      74,     9,    10,    11,    12,    13,    14,   134,    16,     3,
      62,   206,    78,    78,  1051,    23,    24,    25,   216,   216,
     216,    29,    30,    31,     4,     4,     4,   132,     4,     6,
     216,   216,   216,     3,     6,   218,    77,   169,    47,    78,
      47,     4,     6,    78,     4,   150,   151,   152,   153,   154,
      77,   217,   217,   158,     4,    69,    55,   216,    73,   216,
     216,   216,    78,   216,     4,   216,   198,     4,   216,   216,
      78,   216,     4,   217,    77,   221,   161,   162,   163,   164,
       3,   166,   167,   216,   216,   170,   171,   172,   214,     6,
     175,   176,   177,     6,     6,     5,    43,   182,   183,   216,
       3,   206,   207,   208,   209,   210,   211,   212,   213,   216,
     216,     6,     4,   217,   199,   165,   165,   132,   203,    76,
     216,   220,   134,     3,   129,     3,   132,   158,    76,   220,
     217,     4,   220,   155,   169,   150,   151,   152,   153,   216,
     160,   131,   221,   158,   150,   151,   152,   153,   221,     6,
     170,   186,   158,   173,   174,     6,   176,   177,   178,     3,
       3,   169,   220,   198,     6,     4,     4,    89,     4,   216,
     216,    32,   192,   193,   216,   195,   196,     4,     4,   216,
     216,   216,   216,     6,   216,     6,     3,   216,     6,   217,
     198,   206,   207,   208,   209,   210,   211,   212,   213,   216,
     206,   207,   208,   209,   210,   211,   212,   213,   216,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   217,   108,   109,   110,
     111,   112,   113,   220,   217,   116,     4,    78,   119,   120,
       6,     5,   123,   124,   125,   126,    91,    92,    93,    94,
      95,   217,    50,    98,    99,    47,     6,   206,   220,   179,
     105,   106,   107,    91,    92,    93,    94,    95,     6,   131,
      98,    99,   216,   129,   133,   165,   121,   105,   106,   107,
      91,    92,    93,    94,    95,   134,   217,    98,    99,   214,
       4,   220,   216,   121,   105,   106,   107,    91,    92,    93,
      94,    95,   217,   214,    98,    99,     6,     6,   217,    57,
     121,   105,   106,   107,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,    57,   121,     3,   220,
      93,   220,   220,   220,   220,   220,   220,   220,   220,    93,
      93,   220,    93,   220,   220,   220,   220,    93,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,    93,   134,
     220,   134,   220,     4,     3,   220,   220,   220,   220,    66,
     220,     6,   220,   220,   220,    52,     6,     6,     6,     6,
     220,     6,     6,   220,     6,   220,   220,   220,   220,   220,
     220,   220,     6,     6,     6,   220,     6,     6,   220,     6,
     220,   220,     6,     6,   220,   220,   220,   220,   220,     6,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   217,   216,   220,     6,     6,     6,     6,   220,
     220,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   220,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   220,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   220,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   220,     4,     4,     4,     4,
     217,     4,     6,   217,   217,    62,     6,   217,    53,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,    54,   217,     6,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,     6,     6,   217,
       6,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
       6,   217,   165,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,     4,    49,     4,   220,   220,   220,
     217,   217,   217,   217,   217,   217,   217,   217,     6,     6,
     165,   220,   216,   220,     6,   217,    49,     4,     4,   217,
     216,     6,     6,   217,    48,   217,     4,    40,    40,    40,
       3,   217,     3,    40,    40,    40,   217,   216,   216,   216,
       3,   217,   217,    63,   126,   628,   217,   608,   689,   687,
     568,   367,    21,   213,   739,   967,   864,   333,   226,   391,
     417,   823,   705,   718,   718,   829,   736,    -1,   261,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   185,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   442,    -1,    -1,   444,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   511
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
     171,   172,   175,   176,   177,   182,   183,   199,   203,    34,
      34,    34,    34,   159,   160,   161,     3,     3,   272,     3,
     275,   276,   172,     7,    12,    14,    16,   169,   190,   194,
     197,    33,    34,   248,   249,     0,   219,   332,    20,    22,
      28,   268,     8,   250,   252,    72,   195,   331,   195,   331,
     331,   331,   331,   333,   195,   272,    72,   330,   330,   330,
     330,   330,     3,   218,    14,   272,    76,    77,   216,     3,
       3,     3,   226,   216,     3,   272,     6,   173,   174,   173,
     174,     3,   176,     6,   200,   201,   202,   201,   204,   272,
     272,   272,   272,    62,    55,   220,     6,   195,   195,   186,
     187,   191,   160,   170,   173,   174,   176,   177,   178,   192,
     193,   195,   196,   195,     4,   193,    76,   195,   195,   217,
     217,   225,    21,   216,   251,   252,    65,   259,    67,   253,
      73,     3,     3,     3,   272,   272,   272,     3,    62,     3,
     216,   240,    74,     3,   272,   272,   272,     3,     3,     3,
     243,   244,    66,   265,     4,   329,   329,     3,     4,     5,
       6,    73,    74,    83,    85,   102,   103,   104,   108,   135,
     136,   137,   157,   180,   182,   184,   209,   210,   211,   214,
     216,   286,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   299,   300,   301,   302,   303,   305,   306,   307,   308,
     309,   311,   312,   313,   314,   315,   316,   317,   318,   321,
     322,   323,   324,   325,   326,     3,     5,     6,    62,   168,
       3,     5,     6,    62,   168,     3,     5,     6,    62,   168,
     217,    40,    43,    44,    48,    49,     3,     3,     4,    10,
      26,    27,   272,   216,   276,   329,     4,   165,     6,     3,
       6,     4,     3,     4,    55,     4,   195,     3,     3,   251,
     252,   286,    53,    68,   257,    74,   134,    55,   216,   240,
     272,     3,   237,    38,   249,    62,   206,   220,   265,   289,
      78,    78,   216,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,    73,    74,   290,   216,   216,
      88,   289,   304,     4,     4,     4,     4,     6,   326,   216,
     120,   122,   124,   125,   216,   216,   290,   290,     5,     6,
     215,   309,   319,   320,   249,   289,   217,   220,    55,   155,
     156,    73,    75,   132,   150,   151,   152,   153,   154,   158,
     206,   207,   208,   209,   210,   211,   212,   213,   218,   215,
     220,   215,   220,   215,   220,   215,   220,   215,   220,     3,
       6,    47,    47,    77,    78,   334,   250,     4,    40,    49,
       6,    77,   188,   189,     4,   217,   217,    82,   261,   254,
     255,   289,   289,    69,   258,     4,   247,     3,   128,   130,
     228,   230,   231,   236,    55,   216,   338,   217,   220,   216,
     287,   272,   289,   244,   216,   216,    65,   217,   286,   216,
      73,   249,   289,   289,   304,    84,    86,    88,     4,   216,
     216,   216,   216,     4,     4,   221,   217,   217,    77,   288,
       3,   289,   289,    75,   158,   216,    73,   131,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     290,   290,     3,   211,   309,     6,   319,     6,   320,     6,
       5,    43,    45,    46,   216,   216,     3,   216,   217,     6,
      33,    42,     4,   165,   165,   286,    76,   262,   220,    70,
      71,   256,   289,   134,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   108,   109,   110,   111,   112,   113,   116,   119,   120,
     123,   124,   125,   126,   232,   129,   216,   217,   220,   247,
       3,   134,     3,   286,   220,    68,    69,    79,    80,    81,
     185,   327,   328,   327,   286,   217,   249,   217,    55,    87,
      84,    86,   289,   289,    76,   289,     4,     3,   307,   289,
     220,   264,   217,   220,     5,     6,   329,   216,   290,   249,
     286,   131,   155,   221,   221,     6,     6,   237,   229,   231,
       3,   336,   337,     6,     4,     4,   216,   269,   270,   271,
     272,   277,    89,   260,   255,     4,   216,   216,   216,   216,
     216,   216,   216,    73,   128,   130,   131,   233,   234,   334,
     216,   237,    32,   335,   230,   217,     4,   217,   216,     6,
       6,     4,     3,     6,   217,   220,   217,   217,   217,   232,
     289,   289,    84,    87,   290,   220,   220,   220,   220,     4,
      66,   217,     4,    78,   249,   286,   217,   217,   290,    50,
      47,   217,   217,   220,   206,   217,   220,     6,   247,   220,
      56,    58,    59,    60,    61,    63,    64,   278,     3,    55,
     273,   286,   179,   263,     6,    91,    92,    93,    94,    95,
      98,    99,   105,   106,   107,   121,    91,    92,    93,    94,
      95,    98,    99,   105,   106,   107,   121,    91,    92,    93,
      94,    95,    98,    99,   105,   106,   107,   121,    91,    92,
      93,    94,    95,    98,    99,   105,   106,   107,   121,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   121,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     121,   131,   129,   133,   234,   235,   235,   237,   217,   216,
     134,   165,   286,   328,   217,    84,   289,   217,   214,   321,
       4,   309,   214,   310,   313,   318,   321,   220,   264,   289,
     217,   216,   217,   217,     6,     6,   231,     3,     4,     5,
       6,   337,    34,    36,   217,   270,    57,    57,     3,   291,
     292,   293,   294,   295,   296,   297,   298,   265,   217,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,    93,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
      93,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     220,    93,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,    93,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,    93,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,    93,   220,   308,   134,   134,   217,
     336,     4,     3,   217,     6,   220,   220,   264,   220,   220,
     217,   327,     6,   273,   271,   271,   216,   220,    52,   267,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     220,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   220,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   220,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   220,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   220,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   220,     6,     4,     4,   217,
     334,     4,     4,   217,     4,     4,   217,     6,    62,   237,
     297,    53,    54,   266,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,     6,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,     6,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,     6,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,     6,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,     6,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,     6,
     217,   220,   264,   165,   220,   220,   264,    40,    43,    44,
      48,    49,   289,   217,   286,   289,   217,   217,   217,   217,
     217,   217,     4,   217,    49,     4,     6,   217,     6,     6,
     220,   264,   165,   216,   220,   264,   334,     6,    45,    46,
       6,   217,    49,     4,     4,   217,    43,    44,     6,   264,
     334,   216,   217,   264,   127,   165,   334,     6,    48,   217,
       4,   217,    40,    40,   127,   165,   334,   217,   127,   165,
     216,    40,    40,    40,    40,     3,   216,   216,   217,     3,
       3,   334,   217,   217,   334
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
     279,   279,   279,   279,   280,   280,   280,   281,   281,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   283,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   285,   285,   285,   286,   286,   287,   287,
     288,   288,   289,   289,   289,   289,   289,   290,   290,   290,
     290,   290,   290,   290,   290,   290,   290,   290,   290,   290,
     291,   291,   291,   292,   292,   292,   292,   293,   293,   293,
     293,   294,   294,   294,   294,   295,   295,   296,   296,   297,
     297,   297,   297,   297,   297,   298,   298,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     299,   299,   299,   299,   299,   299,   299,   299,   299,   299,
     300,   300,   301,   302,   302,   303,   303,   303,   303,   304,
     304,   305,   306,   306,   306,   306,   307,   307,   307,   307,
     308,   308,   308,   308,   308,   308,   308,   308,   308,   308,
     308,   308,   309,   309,   309,   309,   310,   310,   310,   311,
     312,   312,   313,   313,   314,   315,   315,   316,   317,   317,
     318,   319,   320,   321,   321,   322,   323,   323,   324,   325,
     325,   326,   326,   326,   326,   326,   326,   326,   326,   326,
     326,   326,   326,   327,   327,   328,   328,   328,   328,   328,
     328,   329,   330,   330,   331,   331,   332,   332,   333,   333,
     334,   334,   335,   335,   336,   336,   337,   337,   337,   337,
     337,   338,   338
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
       2,     3,     3,     4,     2,     2,     2,     2,     5,     2,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     3,     3,     4,     4,
       4,     3,     4,     4,     3,     3,     4,     6,     7,     9,
      10,    12,    12,    13,    14,    15,    16,    12,    13,    15,
      16,     3,     4,     5,     6,     3,     3,     4,     3,     3,
       4,     4,     6,     5,     3,     4,     3,     4,     3,     3,
       5,     7,     7,     6,     8,     8,     1,     3,     3,     5,
       3,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
      14,    19,    16,    20,    16,    15,    13,    18,    14,    13,
      11,     8,    10,    13,    15,     5,     7,     4,     6,     1,
       1,     1,     1,     1,     1,     1,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     3,     1,     1,     2,     2,     3,     2,     2,     3,
       2,     3,     3,     1,     1,     2,     2,     3,     2,     2,
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
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2448 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2456 "parser.cpp"
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
#line 2470 "parser.cpp"
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
#line 2484 "parser.cpp"
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
#line 2498 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 310 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2509 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2517 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2526 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2535 "parser.cpp"
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
#line 2549 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 274 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2570 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2580 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2590 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2610 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2620 "parser.cpp"
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
#line 2634 "parser.cpp"
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
#line 2648 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 344 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2658 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2666 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2674 "parser.cpp"
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
#line 2688 "parser.cpp"
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
#line 2702 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2711 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2719 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2727 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2735 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2743 "parser.cpp"
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
#line 2757 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2766 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2775 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2784 "parser.cpp"
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
#line 2797 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2806 "parser.cpp"
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
#line 2820 "parser.cpp"
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
#line 2834 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2853 "parser.cpp"
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
#line 2867 "parser.cpp"
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
#line 2881 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2889 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2897 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2905 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2913 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2921 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2929 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2961 "parser.cpp"
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
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3007 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3015 "parser.cpp"
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
#line 3028 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3036 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3052 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3060 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3068 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3076 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3084 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3100 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3108 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3116 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3124 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3132 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3140 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 371 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3148 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 375 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3156 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3164 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3172 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3180 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3188 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3196 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3204 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3212 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3223 "parser.cpp"
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
#line 3237 "parser.cpp"
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
#line 3251 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3262 "parser.cpp"
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

#line 3370 "parser.cpp"

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
#line 3585 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 507 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3596 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 513 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3607 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 520 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3613 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 521 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3619 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 522 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3625 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 523 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3631 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 524 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3637 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 525 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3643 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 526 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3649 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3655 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 528 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3661 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 529 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3667 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 530 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3673 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3679 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3685 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3691 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3697 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 536 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3703 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3709 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3715 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3721 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3727 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3733 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3739 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3745 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3751 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3757 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3763 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3769 "parser.cpp"
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
#line 3791 "parser.cpp"
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
#line 3811 "parser.cpp"
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
#line 3829 "parser.cpp"
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
#line 3862 "parser.cpp"
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
#line 3882 "parser.cpp"
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
#line 3918 "parser.cpp"
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
#line 3940 "parser.cpp"
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
#line 3961 "parser.cpp"
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
#line 3994 "parser.cpp"
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
#line 4029 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 775 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4038 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 779 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4047 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 784 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4056 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 788 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4065 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 794 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4073 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 797 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4081 "parser.cpp"
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
#line 4137 "parser.cpp"
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
#line 4179 "parser.cpp"
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
#line 4236 "parser.cpp"
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
#line 4279 "parser.cpp"
    break;

  case 52: /* column_type: BOOLEAN  */
#line 984 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4285 "parser.cpp"
    break;

  case 53: /* column_type: TINYINT  */
#line 985 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4291 "parser.cpp"
    break;

  case 54: /* column_type: SMALLINT  */
#line 986 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4297 "parser.cpp"
    break;

  case 55: /* column_type: INTEGER  */
#line 987 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4303 "parser.cpp"
    break;

  case 56: /* column_type: INT  */
#line 988 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4309 "parser.cpp"
    break;

  case 57: /* column_type: BIGINT  */
#line 989 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4315 "parser.cpp"
    break;

  case 58: /* column_type: HUGEINT  */
#line 990 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4321 "parser.cpp"
    break;

  case 59: /* column_type: FLOAT  */
#line 991 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4327 "parser.cpp"
    break;

  case 60: /* column_type: REAL  */
#line 992 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4333 "parser.cpp"
    break;

  case 61: /* column_type: DOUBLE  */
#line 993 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4339 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT16  */
#line 994 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4345 "parser.cpp"
    break;

  case 63: /* column_type: BFLOAT16  */
#line 995 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4351 "parser.cpp"
    break;

  case 64: /* column_type: DATE  */
#line 996 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4357 "parser.cpp"
    break;

  case 65: /* column_type: TIME  */
#line 997 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4363 "parser.cpp"
    break;

  case 66: /* column_type: DATETIME  */
#line 998 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4369 "parser.cpp"
    break;

  case 67: /* column_type: TIMESTAMP  */
#line 999 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4375 "parser.cpp"
    break;

  case 68: /* column_type: UUID  */
#line 1000 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4381 "parser.cpp"
    break;

  case 69: /* column_type: POINT  */
#line 1001 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4387 "parser.cpp"
    break;

  case 70: /* column_type: LINE  */
#line 1002 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4393 "parser.cpp"
    break;

  case 71: /* column_type: LSEG  */
#line 1003 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4399 "parser.cpp"
    break;

  case 72: /* column_type: BOX  */
#line 1004 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4405 "parser.cpp"
    break;

  case 73: /* column_type: CIRCLE  */
#line 1007 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4411 "parser.cpp"
    break;

  case 74: /* column_type: VARCHAR  */
#line 1009 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4417 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4423 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 1011 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4429 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL  */
#line 1012 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4435 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4441 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4447 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4453 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4459 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4465 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4471 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4477 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4483 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4489 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4495 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4501 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4507 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4513 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4519 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4525 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4531 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4537 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4543 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4549 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4555 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4561 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4567 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4573 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4579 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4585 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4591 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4597 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4603 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4609 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4615 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4621 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4627 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4633 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4639 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4645 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4651 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4657 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4663 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4669 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1054 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4675 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1055 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4681 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1056 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4687 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1057 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4693 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1058 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4699 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1059 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4705 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1060 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4711 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1061 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4717 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1062 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4723 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1063 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4729 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1064 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4735 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1065 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4741 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1066 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4747 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1067 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4753 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1068 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4759 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1069 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4765 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1070 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4771 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1071 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4777 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1072 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4783 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1073 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4789 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1074 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4795 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1075 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4801 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1076 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4807 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1077 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4813 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1078 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4819 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1079 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4825 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1080 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4831 "parser.cpp"
    break;

  case 144: /* column_constraints: column_constraint  */
#line 1099 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4840 "parser.cpp"
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
#line 4854 "parser.cpp"
    break;

  case 146: /* column_constraint: PRIMARY KEY  */
#line 1113 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4862 "parser.cpp"
    break;

  case 147: /* column_constraint: UNIQUE  */
#line 1116 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4870 "parser.cpp"
    break;

  case 148: /* column_constraint: NULLABLE  */
#line 1119 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4878 "parser.cpp"
    break;

  case 149: /* column_constraint: NOT NULLABLE  */
#line 1122 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4886 "parser.cpp"
    break;

  case 150: /* default_expr: DEFAULT constant_expr  */
#line 1126 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4894 "parser.cpp"
    break;

  case 151: /* default_expr: %empty  */
#line 1129 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4902 "parser.cpp"
    break;

  case 152: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1134 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4912 "parser.cpp"
    break;

  case 153: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1139 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4922 "parser.cpp"
    break;

  case 154: /* identifier_array: IDENTIFIER  */
#line 1146 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4933 "parser.cpp"
    break;

  case 155: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1152 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4944 "parser.cpp"
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
#line 4961 "parser.cpp"
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
#line 5004 "parser.cpp"
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
#line 5024 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1232 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5032 "parser.cpp"
    break;

  case 160: /* optional_identifier_array: %empty  */
#line 1235 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5040 "parser.cpp"
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
#line 5058 "parser.cpp"
    break;

  case 162: /* explain_statement: EXPLAIN explainable_statement  */
#line 1254 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5068 "parser.cpp"
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
#line 5085 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr  */
#line 1276 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5094 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1280 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5103 "parser.cpp"
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
#line 5115 "parser.cpp"
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
#line 5131 "parser.cpp"
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
#line 5149 "parser.cpp"
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
#line 5167 "parser.cpp"
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
#line 5185 "parser.cpp"
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
#line 5208 "parser.cpp"
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
#line 5266 "parser.cpp"
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
#line 5326 "parser.cpp"
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
#line 5378 "parser.cpp"
    break;

  case 175: /* select_statement: select_without_paren  */
#line 1538 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5386 "parser.cpp"
    break;

  case 176: /* select_statement: select_with_paren  */
#line 1541 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5394 "parser.cpp"
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
#line 5408 "parser.cpp"
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
#line 5422 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_without_paren ')'  */
#line 1563 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5430 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_with_paren ')'  */
#line 1566 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5438 "parser.cpp"
    break;

  case 181: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1570 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5447 "parser.cpp"
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
#line 5478 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1602 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5486 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1605 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5494 "parser.cpp"
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
#line 5516 "parser.cpp"
    break;

  case 186: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1628 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5524 "parser.cpp"
    break;

  case 187: /* order_by_clause: %empty  */
#line 1631 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5532 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr  */
#line 1635 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5541 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1639 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5550 "parser.cpp"
    break;

  case 190: /* order_by_expr: expr order_by_type  */
#line 1644 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5560 "parser.cpp"
    break;

  case 191: /* order_by_type: ASC  */
#line 1650 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5568 "parser.cpp"
    break;

  case 192: /* order_by_type: DESC  */
#line 1653 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5576 "parser.cpp"
    break;

  case 193: /* order_by_type: %empty  */
#line 1656 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5584 "parser.cpp"
    break;

  case 194: /* limit_expr: LIMIT expr  */
#line 1660 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5592 "parser.cpp"
    break;

  case 195: /* limit_expr: %empty  */
#line 1664 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5598 "parser.cpp"
    break;

  case 196: /* offset_expr: OFFSET expr  */
#line 1666 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5606 "parser.cpp"
    break;

  case 197: /* offset_expr: %empty  */
#line 1670 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5612 "parser.cpp"
    break;

  case 198: /* distinct: DISTINCT  */
#line 1672 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5620 "parser.cpp"
    break;

  case 199: /* distinct: %empty  */
#line 1675 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5628 "parser.cpp"
    break;

  case 200: /* unnest_clause: UNNEST expr_array  */
#line 1679 "parser.y"
                                 {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5636 "parser.cpp"
    break;

  case 201: /* unnest_clause: %empty  */
#line 1682 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5644 "parser.cpp"
    break;

  case 202: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1686 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5652 "parser.cpp"
    break;

  case 203: /* highlight_clause: %empty  */
#line 1689 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5660 "parser.cpp"
    break;

  case 204: /* from_clause: FROM table_reference  */
#line 1693 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5668 "parser.cpp"
    break;

  case 205: /* from_clause: %empty  */
#line 1696 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5676 "parser.cpp"
    break;

  case 206: /* search_clause: SEARCH sub_search_array  */
#line 1700 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5686 "parser.cpp"
    break;

  case 207: /* search_clause: %empty  */
#line 1705 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5694 "parser.cpp"
    break;

  case 208: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1709 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5702 "parser.cpp"
    break;

  case 209: /* optional_search_filter_expr: %empty  */
#line 1712 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5710 "parser.cpp"
    break;

  case 210: /* where_clause: WHERE expr  */
#line 1716 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5718 "parser.cpp"
    break;

  case 211: /* where_clause: %empty  */
#line 1719 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5726 "parser.cpp"
    break;

  case 212: /* having_clause: HAVING expr  */
#line 1723 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5734 "parser.cpp"
    break;

  case 213: /* having_clause: %empty  */
#line 1726 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5742 "parser.cpp"
    break;

  case 214: /* group_by_clause: GROUP BY expr_array  */
#line 1730 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5750 "parser.cpp"
    break;

  case 215: /* group_by_clause: %empty  */
#line 1733 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5758 "parser.cpp"
    break;

  case 216: /* set_operator: UNION  */
#line 1737 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5766 "parser.cpp"
    break;

  case 217: /* set_operator: UNION ALL  */
#line 1740 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5774 "parser.cpp"
    break;

  case 218: /* set_operator: INTERSECT  */
#line 1743 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5782 "parser.cpp"
    break;

  case 219: /* set_operator: EXCEPT  */
#line 1746 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5790 "parser.cpp"
    break;

  case 220: /* table_reference: table_reference_unit  */
#line 1754 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5798 "parser.cpp"
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
#line 5816 "parser.cpp"
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
#line 5834 "parser.cpp"
    break;

  case 225: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1788 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5845 "parser.cpp"
    break;

  case 226: /* table_name: IDENTIFIER  */
#line 1797 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5855 "parser.cpp"
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
#line 5867 "parser.cpp"
    break;

  case 228: /* table_alias: AS IDENTIFIER  */
#line 1811 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5877 "parser.cpp"
    break;

  case 229: /* table_alias: IDENTIFIER  */
#line 1816 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5887 "parser.cpp"
    break;

  case 230: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1821 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5898 "parser.cpp"
    break;

  case 231: /* table_alias: %empty  */
#line 1827 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5906 "parser.cpp"
    break;

  case 232: /* with_clause: WITH with_expr_list  */
#line 1834 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5914 "parser.cpp"
    break;

  case 233: /* with_clause: %empty  */
#line 1837 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5922 "parser.cpp"
    break;

  case 234: /* with_expr_list: with_expr  */
#line 1841 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5931 "parser.cpp"
    break;

  case 235: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1844 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5940 "parser.cpp"
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
#line 5952 "parser.cpp"
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
#line 5964 "parser.cpp"
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
#line 5977 "parser.cpp"
    break;

  case 239: /* join_type: INNER  */
#line 1882 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5985 "parser.cpp"
    break;

  case 240: /* join_type: LEFT  */
#line 1885 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5993 "parser.cpp"
    break;

  case 241: /* join_type: RIGHT  */
#line 1888 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 6001 "parser.cpp"
    break;

  case 242: /* join_type: OUTER  */
#line 1891 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6009 "parser.cpp"
    break;

  case 243: /* join_type: FULL  */
#line 1894 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6017 "parser.cpp"
    break;

  case 244: /* join_type: CROSS  */
#line 1897 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6025 "parser.cpp"
    break;

  case 245: /* join_type: %empty  */
#line 1900 "parser.y"
                {
}
#line 6032 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW DATABASES  */
#line 1906 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6041 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLES  */
#line 1910 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6050 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW VIEWS  */
#line 1914 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6059 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW CONFIGS  */
#line 1918 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6068 "parser.cpp"
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
#line 6080 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW PROFILES  */
#line 1929 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6089 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW BUFFER  */
#line 1933 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6098 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW MEMINDEX  */
#line 1937 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6107 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW QUERIES  */
#line 1941 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6116 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1945 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6126 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TRANSACTIONS  */
#line 1950 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6135 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1954 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6145 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW SESSION VARIABLES  */
#line 1959 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6154 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1963 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6163 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1967 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6174 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1973 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6185 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1979 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6196 "parser.cpp"
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
#line 6212 "parser.cpp"
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
#line 6228 "parser.cpp"
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
#line 6244 "parser.cpp"
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
#line 6261 "parser.cpp"
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
#line 6278 "parser.cpp"
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
#line 6296 "parser.cpp"
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
#line 6315 "parser.cpp"
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
#line 6331 "parser.cpp"
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
#line 6350 "parser.cpp"
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
#line 6371 "parser.cpp"
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
#line 6393 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW LOGS  */
#line 2127 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6402 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW DELTA LOGS  */
#line 2131 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6411 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW CATALOGS  */
#line 2135 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6420 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2139 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6429 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2143 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6438 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2147 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6449 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW MEMORY  */
#line 2153 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6458 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2157 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6467 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2161 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6476 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2165 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6487 "parser.cpp"
    break;

  case 284: /* flush_statement: FLUSH DATA  */
#line 2175 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6496 "parser.cpp"
    break;

  case 285: /* flush_statement: FLUSH LOG  */
#line 2179 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6505 "parser.cpp"
    break;

  case 286: /* flush_statement: FLUSH BUFFER  */
#line 2183 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6514 "parser.cpp"
    break;

  case 287: /* optimize_statement: OPTIMIZE table_name  */
#line 2191 "parser.y"
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
#line 6529 "parser.cpp"
    break;

  case 288: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2202 "parser.y"
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
#line 6553 "parser.cpp"
    break;

  case 289: /* command_statement: USE IDENTIFIER  */
#line 2225 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6564 "parser.cpp"
    break;

  case 290: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2231 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6574 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2236 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6585 "parser.cpp"
    break;

  case 292: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2242 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6596 "parser.cpp"
    break;

  case 293: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2248 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6609 "parser.cpp"
    break;

  case 294: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2256 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6620 "parser.cpp"
    break;

  case 295: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2262 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6631 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2268 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6642 "parser.cpp"
    break;

  case 297: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2274 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6653 "parser.cpp"
    break;

  case 298: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2280 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6666 "parser.cpp"
    break;

  case 299: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2288 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6677 "parser.cpp"
    break;

  case 300: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2294 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6688 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2300 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6699 "parser.cpp"
    break;

  case 302: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2306 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6710 "parser.cpp"
    break;

  case 303: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2312 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6723 "parser.cpp"
    break;

  case 304: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2320 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6734 "parser.cpp"
    break;

  case 305: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2326 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6745 "parser.cpp"
    break;

  case 306: /* command_statement: LOCK TABLE table_name  */
#line 2332 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6759 "parser.cpp"
    break;

  case 307: /* command_statement: UNLOCK TABLE table_name  */
#line 2341 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6773 "parser.cpp"
    break;

  case 308: /* command_statement: CREATE TABLE SNAPSHOT IDENTIFIER  */
#line 2350 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6784 "parser.cpp"
    break;

  case 309: /* command_statement: CREATE DATABASE SNAPSHOT IDENTIFIER  */
#line 2356 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6795 "parser.cpp"
    break;

  case 310: /* command_statement: CREATE SYSTEM SNAPSHOT IDENTIFIER  */
#line 2362 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[0].str_value));
}
#line 6806 "parser.cpp"
    break;

  case 311: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2368 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6817 "parser.cpp"
    break;

  case 312: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2374 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6828 "parser.cpp"
    break;

  case 313: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2380 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6839 "parser.cpp"
    break;

  case 314: /* compact_statement: COMPACT TABLE table_name  */
#line 2387 "parser.y"
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
#line 6856 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2400 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6865 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2404 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6875 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2409 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6886 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2415 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6898 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2422 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6911 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2430 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6925 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2439 "parser.y"
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
#line 6940 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2449 "parser.y"
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
#line 6955 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2459 "parser.y"
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
#line 6971 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2470 "parser.y"
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
#line 6987 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2481 "parser.y"
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
#line 7004 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2493 "parser.y"
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
#line 7021 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2505 "parser.y"
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
#line 7036 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2515 "parser.y"
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
#line 7052 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2526 "parser.y"
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
#line 7069 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2538 "parser.y"
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
#line 7087 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW LOGS  */
#line 2551 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7096 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2555 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7106 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2560 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7116 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2565 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7127 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2571 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7136 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2575 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7145 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2579 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7156 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2585 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7165 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2589 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7174 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2593 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7185 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2599 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7196 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2605 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7209 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2613 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7220 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW NODES  */
#line 2619 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7229 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2623 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7240 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW NODE  */
#line 2629 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7249 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2633 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7260 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN SET ADMIN  */
#line 2639 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7270 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN SET STANDALONE  */
#line 2644 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7280 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2649 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7292 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2656 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7306 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2665 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7320 "parser.cpp"
    break;

  case 353: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2675 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7334 "parser.cpp"
    break;

  case 354: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2684 "parser.y"
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
#line 7351 "parser.cpp"
    break;

  case 355: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2696 "parser.y"
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
#line 7367 "parser.cpp"
    break;

  case 356: /* expr_array: expr_alias  */
#line 2712 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7376 "parser.cpp"
    break;

  case 357: /* expr_array: expr_array ',' expr_alias  */
#line 2716 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7385 "parser.cpp"
    break;

  case 358: /* insert_row_list: '(' expr_array ')'  */
#line 2721 "parser.y"
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
#line 7400 "parser.cpp"
    break;

  case 359: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2731 "parser.y"
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
#line 7415 "parser.cpp"
    break;

  case 360: /* expr_alias: expr AS IDENTIFIER  */
#line 2753 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7426 "parser.cpp"
    break;

  case 361: /* expr_alias: expr  */
#line 2759 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7434 "parser.cpp"
    break;

  case 367: /* operand: '(' expr ')'  */
#line 2769 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7442 "parser.cpp"
    break;

  case 368: /* operand: '(' select_without_paren ')'  */
#line 2772 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7453 "parser.cpp"
    break;

  case 369: /* operand: constant_expr  */
#line 2778 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7461 "parser.cpp"
    break;

  case 380: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2794 "parser.y"
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
#line 7481 "parser.cpp"
    break;

  case 381: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2810 "parser.y"
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
#line 7502 "parser.cpp"
    break;

  case 382: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2827 "parser.y"
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
#line 7523 "parser.cpp"
    break;

  case 383: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2845 "parser.y"
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
#line 7573 "parser.cpp"
    break;

  case 384: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2891 "parser.y"
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
#line 7616 "parser.cpp"
    break;

  case 385: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2930 "parser.y"
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
#line 7663 "parser.cpp"
    break;

  case 386: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2973 "parser.y"
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
#line 7711 "parser.cpp"
    break;

  case 387: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3020 "parser.y"
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
#line 7739 "parser.cpp"
    break;

  case 388: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3044 "parser.y"
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
#line 7766 "parser.cpp"
    break;

  case 389: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3067 "parser.y"
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
#line 7791 "parser.cpp"
    break;

  case 390: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3088 "parser.y"
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
#line 7816 "parser.cpp"
    break;

  case 391: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3109 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7830 "parser.cpp"
    break;

  case 392: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3118 "parser.y"
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
#line 7846 "parser.cpp"
    break;

  case 393: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3129 "parser.y"
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
#line 7862 "parser.cpp"
    break;

  case 394: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3140 "parser.y"
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
#line 7880 "parser.cpp"
    break;

  case 395: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3154 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7892 "parser.cpp"
    break;

  case 396: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3161 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7906 "parser.cpp"
    break;

  case 397: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3171 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7917 "parser.cpp"
    break;

  case 398: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3177 "parser.y"
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
#line 7933 "parser.cpp"
    break;

  case 399: /* sub_search: match_vector_expr  */
#line 3189 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7941 "parser.cpp"
    break;

  case 400: /* sub_search: match_text_expr  */
#line 3192 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7949 "parser.cpp"
    break;

  case 401: /* sub_search: match_tensor_expr  */
#line 3195 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7957 "parser.cpp"
    break;

  case 402: /* sub_search: match_sparse_expr  */
#line 3198 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7965 "parser.cpp"
    break;

  case 403: /* sub_search: query_expr  */
#line 3201 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7973 "parser.cpp"
    break;

  case 404: /* sub_search: fusion_expr  */
#line 3204 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7981 "parser.cpp"
    break;

  case 405: /* sub_search_array: sub_search  */
#line 3208 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7990 "parser.cpp"
    break;

  case 406: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3212 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7999 "parser.cpp"
    break;

  case 407: /* function_expr: IDENTIFIER '(' ')'  */
#line 3217 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8012 "parser.cpp"
    break;

  case 408: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3225 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8025 "parser.cpp"
    break;

  case 409: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3233 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8039 "parser.cpp"
    break;

  case 410: /* function_expr: operand IS NOT NULLABLE  */
#line 3242 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8051 "parser.cpp"
    break;

  case 411: /* function_expr: operand IS NULLABLE  */
#line 3249 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8063 "parser.cpp"
    break;

  case 412: /* function_expr: NOT operand  */
#line 3256 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8075 "parser.cpp"
    break;

  case 413: /* function_expr: '-' operand  */
#line 3263 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8087 "parser.cpp"
    break;

  case 414: /* function_expr: '+' operand  */
#line 3270 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8099 "parser.cpp"
    break;

  case 415: /* function_expr: operand '-' operand  */
#line 3277 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8112 "parser.cpp"
    break;

  case 416: /* function_expr: operand '+' operand  */
#line 3285 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8125 "parser.cpp"
    break;

  case 417: /* function_expr: operand '*' operand  */
#line 3293 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8138 "parser.cpp"
    break;

  case 418: /* function_expr: operand '/' operand  */
#line 3301 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8151 "parser.cpp"
    break;

  case 419: /* function_expr: operand '%' operand  */
#line 3309 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8164 "parser.cpp"
    break;

  case 420: /* function_expr: operand '=' operand  */
#line 3317 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8177 "parser.cpp"
    break;

  case 421: /* function_expr: operand EQUAL operand  */
#line 3325 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8190 "parser.cpp"
    break;

  case 422: /* function_expr: operand NOT_EQ operand  */
#line 3333 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8203 "parser.cpp"
    break;

  case 423: /* function_expr: operand '<' operand  */
#line 3341 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8216 "parser.cpp"
    break;

  case 424: /* function_expr: operand '>' operand  */
#line 3349 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8229 "parser.cpp"
    break;

  case 425: /* function_expr: operand LESS_EQ operand  */
#line 3357 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8242 "parser.cpp"
    break;

  case 426: /* function_expr: operand GREATER_EQ operand  */
#line 3365 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8255 "parser.cpp"
    break;

  case 427: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3373 "parser.y"
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
#line 8290 "parser.cpp"
    break;

  case 428: /* function_expr: operand LIKE operand  */
#line 3403 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8303 "parser.cpp"
    break;

  case 429: /* function_expr: operand NOT LIKE operand  */
#line 3411 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8316 "parser.cpp"
    break;

  case 430: /* conjunction_expr: expr AND expr  */
#line 3420 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8329 "parser.cpp"
    break;

  case 431: /* conjunction_expr: expr OR expr  */
#line 3428 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8342 "parser.cpp"
    break;

  case 432: /* between_expr: operand BETWEEN operand AND operand  */
#line 3437 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8354 "parser.cpp"
    break;

  case 433: /* in_expr: operand IN '(' expr_array ')'  */
#line 3445 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8365 "parser.cpp"
    break;

  case 434: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3451 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8376 "parser.cpp"
    break;

  case 435: /* case_expr: CASE expr case_check_array END  */
#line 3458 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8387 "parser.cpp"
    break;

  case 436: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3464 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8399 "parser.cpp"
    break;

  case 437: /* case_expr: CASE case_check_array END  */
#line 3471 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8409 "parser.cpp"
    break;

  case 438: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3476 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8420 "parser.cpp"
    break;

  case 439: /* case_check_array: WHEN expr THEN expr  */
#line 3483 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8432 "parser.cpp"
    break;

  case 440: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3490 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8444 "parser.cpp"
    break;

  case 441: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3498 "parser.y"
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
#line 8475 "parser.cpp"
    break;

  case 442: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3525 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8486 "parser.cpp"
    break;

  case 443: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3531 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8497 "parser.cpp"
    break;

  case 444: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3537 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8509 "parser.cpp"
    break;

  case 445: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3544 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8521 "parser.cpp"
    break;

  case 446: /* column_expr: IDENTIFIER  */
#line 3552 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8533 "parser.cpp"
    break;

  case 447: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3559 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8545 "parser.cpp"
    break;

  case 448: /* column_expr: '*'  */
#line 3566 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8555 "parser.cpp"
    break;

  case 449: /* column_expr: column_expr '.' '*'  */
#line 3571 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8569 "parser.cpp"
    break;

  case 450: /* constant_expr: STRING  */
#line 3581 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8579 "parser.cpp"
    break;

  case 451: /* constant_expr: TRUE  */
#line 3586 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8589 "parser.cpp"
    break;

  case 452: /* constant_expr: FALSE  */
#line 3591 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8599 "parser.cpp"
    break;

  case 453: /* constant_expr: DOUBLE_VALUE  */
#line 3596 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8609 "parser.cpp"
    break;

  case 454: /* constant_expr: LONG_VALUE  */
#line 3601 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8619 "parser.cpp"
    break;

  case 455: /* constant_expr: DATE STRING  */
#line 3606 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8629 "parser.cpp"
    break;

  case 456: /* constant_expr: TIME STRING  */
#line 3611 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8639 "parser.cpp"
    break;

  case 457: /* constant_expr: DATETIME STRING  */
#line 3616 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8649 "parser.cpp"
    break;

  case 458: /* constant_expr: TIMESTAMP STRING  */
#line 3621 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8659 "parser.cpp"
    break;

  case 459: /* constant_expr: INTERVAL interval_expr  */
#line 3626 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8667 "parser.cpp"
    break;

  case 460: /* constant_expr: interval_expr  */
#line 3629 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8675 "parser.cpp"
    break;

  case 461: /* constant_expr: common_array_expr  */
#line 3632 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8683 "parser.cpp"
    break;

  case 462: /* common_array_expr: array_expr  */
#line 3636 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8691 "parser.cpp"
    break;

  case 463: /* common_array_expr: subarray_array_expr  */
#line 3639 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8699 "parser.cpp"
    break;

  case 464: /* common_array_expr: sparse_array_expr  */
#line 3642 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8707 "parser.cpp"
    break;

  case 465: /* common_array_expr: empty_array_expr  */
#line 3645 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8715 "parser.cpp"
    break;

  case 466: /* common_sparse_array_expr: sparse_array_expr  */
#line 3649 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8723 "parser.cpp"
    break;

  case 467: /* common_sparse_array_expr: array_expr  */
#line 3652 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8731 "parser.cpp"
    break;

  case 468: /* common_sparse_array_expr: empty_array_expr  */
#line 3655 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8739 "parser.cpp"
    break;

  case 469: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3659 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8747 "parser.cpp"
    break;

  case 470: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3663 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8757 "parser.cpp"
    break;

  case 471: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3668 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8766 "parser.cpp"
    break;

  case 472: /* sparse_array_expr: long_sparse_array_expr  */
#line 3673 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8774 "parser.cpp"
    break;

  case 473: /* sparse_array_expr: double_sparse_array_expr  */
#line 3676 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8782 "parser.cpp"
    break;

  case 474: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3680 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8790 "parser.cpp"
    break;

  case 475: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3684 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8802 "parser.cpp"
    break;

  case 476: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3691 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8813 "parser.cpp"
    break;

  case 477: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3698 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8821 "parser.cpp"
    break;

  case 478: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3702 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8833 "parser.cpp"
    break;

  case 479: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3709 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8844 "parser.cpp"
    break;

  case 480: /* empty_array_expr: '[' ']'  */
#line 3716 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8852 "parser.cpp"
    break;

  case 481: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3720 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8860 "parser.cpp"
    break;

  case 482: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3724 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8868 "parser.cpp"
    break;

  case 483: /* array_expr: long_array_expr  */
#line 3728 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8876 "parser.cpp"
    break;

  case 484: /* array_expr: double_array_expr  */
#line 3731 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8884 "parser.cpp"
    break;

  case 485: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3735 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8892 "parser.cpp"
    break;

  case 486: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3739 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8902 "parser.cpp"
    break;

  case 487: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3744 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8911 "parser.cpp"
    break;

  case 488: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3749 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8919 "parser.cpp"
    break;

  case 489: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3753 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8929 "parser.cpp"
    break;

  case 490: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3758 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8938 "parser.cpp"
    break;

  case 491: /* interval_expr: LONG_VALUE SECONDS  */
#line 3763 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8949 "parser.cpp"
    break;

  case 492: /* interval_expr: LONG_VALUE SECOND  */
#line 3769 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8960 "parser.cpp"
    break;

  case 493: /* interval_expr: LONG_VALUE MINUTES  */
#line 3775 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8971 "parser.cpp"
    break;

  case 494: /* interval_expr: LONG_VALUE MINUTE  */
#line 3781 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8982 "parser.cpp"
    break;

  case 495: /* interval_expr: LONG_VALUE HOURS  */
#line 3787 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8993 "parser.cpp"
    break;

  case 496: /* interval_expr: LONG_VALUE HOUR  */
#line 3793 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9004 "parser.cpp"
    break;

  case 497: /* interval_expr: LONG_VALUE DAYS  */
#line 3799 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9015 "parser.cpp"
    break;

  case 498: /* interval_expr: LONG_VALUE DAY  */
#line 3805 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9026 "parser.cpp"
    break;

  case 499: /* interval_expr: LONG_VALUE MONTHS  */
#line 3811 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9037 "parser.cpp"
    break;

  case 500: /* interval_expr: LONG_VALUE MONTH  */
#line 3817 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9048 "parser.cpp"
    break;

  case 501: /* interval_expr: LONG_VALUE YEARS  */
#line 3823 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9059 "parser.cpp"
    break;

  case 502: /* interval_expr: LONG_VALUE YEAR  */
#line 3829 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9070 "parser.cpp"
    break;

  case 503: /* copy_option_list: copy_option  */
#line 3840 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9079 "parser.cpp"
    break;

  case 504: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3844 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9088 "parser.cpp"
    break;

  case 505: /* copy_option: FORMAT IDENTIFIER  */
#line 3849 "parser.y"
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
#line 9124 "parser.cpp"
    break;

  case 506: /* copy_option: DELIMITER STRING  */
#line 3880 "parser.y"
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
#line 9139 "parser.cpp"
    break;

  case 507: /* copy_option: HEADER  */
#line 3890 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9149 "parser.cpp"
    break;

  case 508: /* copy_option: OFFSET LONG_VALUE  */
#line 3895 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9159 "parser.cpp"
    break;

  case 509: /* copy_option: LIMIT LONG_VALUE  */
#line 3900 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9169 "parser.cpp"
    break;

  case 510: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3905 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9179 "parser.cpp"
    break;

  case 511: /* file_path: STRING  */
#line 3911 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9187 "parser.cpp"
    break;

  case 512: /* if_exists: IF EXISTS  */
#line 3915 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9193 "parser.cpp"
    break;

  case 513: /* if_exists: %empty  */
#line 3916 "parser.y"
  { (yyval.bool_value) = false; }
#line 9199 "parser.cpp"
    break;

  case 514: /* if_not_exists: IF NOT EXISTS  */
#line 3918 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9205 "parser.cpp"
    break;

  case 515: /* if_not_exists: %empty  */
#line 3919 "parser.y"
  { (yyval.bool_value) = false; }
#line 9211 "parser.cpp"
    break;

  case 518: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3934 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9224 "parser.cpp"
    break;

  case 519: /* if_not_exists_info: %empty  */
#line 3942 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9232 "parser.cpp"
    break;

  case 520: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3946 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9240 "parser.cpp"
    break;

  case 521: /* with_index_param_list: %empty  */
#line 3949 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9248 "parser.cpp"
    break;

  case 522: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3953 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9256 "parser.cpp"
    break;

  case 523: /* optional_table_properties_list: %empty  */
#line 3956 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9264 "parser.cpp"
    break;

  case 524: /* index_param_list: index_param  */
#line 3960 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9273 "parser.cpp"
    break;

  case 525: /* index_param_list: index_param_list ',' index_param  */
#line 3964 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9282 "parser.cpp"
    break;

  case 526: /* index_param: IDENTIFIER  */
#line 3969 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9293 "parser.cpp"
    break;

  case 527: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3975 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9308 "parser.cpp"
    break;

  case 528: /* index_param: IDENTIFIER '=' STRING  */
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
#line 9323 "parser.cpp"
    break;

  case 529: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3995 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9336 "parser.cpp"
    break;

  case 530: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4003 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9349 "parser.cpp"
    break;

  case 531: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4014 "parser.y"
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
#line 9385 "parser.cpp"
    break;

  case 532: /* index_info: '(' IDENTIFIER ')'  */
#line 4045 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9396 "parser.cpp"
    break;


#line 9400 "parser.cpp"

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

#line 4052 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

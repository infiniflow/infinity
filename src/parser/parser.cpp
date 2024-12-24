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
  YYSYMBOL_UNNEST = 88,                    /* UNNEST  */
  YYSYMBOL_BOOLEAN = 89,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 90,                   /* INTEGER  */
  YYSYMBOL_INT = 91,                       /* INT  */
  YYSYMBOL_TINYINT = 92,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 93,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 94,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 95,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 96,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 97,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 98,                    /* DOUBLE  */
  YYSYMBOL_REAL = 99,                      /* REAL  */
  YYSYMBOL_DECIMAL = 100,                  /* DECIMAL  */
  YYSYMBOL_DATE = 101,                     /* DATE  */
  YYSYMBOL_TIME = 102,                     /* TIME  */
  YYSYMBOL_DATETIME = 103,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 104,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 105,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 106,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 107,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 108,                     /* UUID  */
  YYSYMBOL_POINT = 109,                    /* POINT  */
  YYSYMBOL_LINE = 110,                     /* LINE  */
  YYSYMBOL_LSEG = 111,                     /* LSEG  */
  YYSYMBOL_BOX = 112,                      /* BOX  */
  YYSYMBOL_PATH = 113,                     /* PATH  */
  YYSYMBOL_POLYGON = 114,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 115,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 116,                     /* BLOB  */
  YYSYMBOL_BITMAP = 117,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 118,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 119,                   /* VECTOR  */
  YYSYMBOL_BIT = 120,                      /* BIT  */
  YYSYMBOL_TEXT = 121,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 122,              /* MULTIVECTOR  */
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
  YYSYMBOL_COMMENT = 133,                  /* COMMENT  */
  YYSYMBOL_TRUE = 134,                     /* TRUE  */
  YYSYMBOL_FALSE = 135,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 136,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 137,                   /* SECOND  */
  YYSYMBOL_SECONDS = 138,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 139,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 140,                  /* MINUTES  */
  YYSYMBOL_HOUR = 141,                     /* HOUR  */
  YYSYMBOL_HOURS = 142,                    /* HOURS  */
  YYSYMBOL_DAY = 143,                      /* DAY  */
  YYSYMBOL_DAYS = 144,                     /* DAYS  */
  YYSYMBOL_MONTH = 145,                    /* MONTH  */
  YYSYMBOL_MONTHS = 146,                   /* MONTHS  */
  YYSYMBOL_YEAR = 147,                     /* YEAR  */
  YYSYMBOL_YEARS = 148,                    /* YEARS  */
  YYSYMBOL_EQUAL = 149,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 150,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 151,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 152,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 153,                  /* BETWEEN  */
  YYSYMBOL_AND = 154,                      /* AND  */
  YYSYMBOL_OR = 155,                       /* OR  */
  YYSYMBOL_EXTRACT = 156,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 157,                     /* LIKE  */
  YYSYMBOL_DATA = 158,                     /* DATA  */
  YYSYMBOL_LOG = 159,                      /* LOG  */
  YYSYMBOL_BUFFER = 160,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 161,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 162,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 163,                 /* MEMINDEX  */
  YYSYMBOL_USING = 164,                    /* USING  */
  YYSYMBOL_SESSION = 165,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 166,                   /* GLOBAL  */
  YYSYMBOL_OFF = 167,                      /* OFF  */
  YYSYMBOL_EXPORT = 168,                   /* EXPORT  */
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
  YYSYMBOL_PERSISTENCE = 197,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 198,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 199,                  /* OBJECTS  */
  YYSYMBOL_FILES = 200,                    /* FILES  */
  YYSYMBOL_MEMORY = 201,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 202,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 203,                   /* NUMBER  */
  YYSYMBOL_204_ = 204,                     /* '='  */
  YYSYMBOL_205_ = 205,                     /* '<'  */
  YYSYMBOL_206_ = 206,                     /* '>'  */
  YYSYMBOL_207_ = 207,                     /* '+'  */
  YYSYMBOL_208_ = 208,                     /* '-'  */
  YYSYMBOL_209_ = 209,                     /* '*'  */
  YYSYMBOL_210_ = 210,                     /* '/'  */
  YYSYMBOL_211_ = 211,                     /* '%'  */
  YYSYMBOL_212_ = 212,                     /* '['  */
  YYSYMBOL_213_ = 213,                     /* ']'  */
  YYSYMBOL_214_ = 214,                     /* '('  */
  YYSYMBOL_215_ = 215,                     /* ')'  */
  YYSYMBOL_216_ = 216,                     /* '.'  */
  YYSYMBOL_217_ = 217,                     /* ';'  */
  YYSYMBOL_218_ = 218,                     /* ','  */
  YYSYMBOL_219_ = 219,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 220,                 /* $accept  */
  YYSYMBOL_input_pattern = 221,            /* input_pattern  */
  YYSYMBOL_statement_list = 222,           /* statement_list  */
  YYSYMBOL_statement = 223,                /* statement  */
  YYSYMBOL_explainable_statement = 224,    /* explainable_statement  */
  YYSYMBOL_create_statement = 225,         /* create_statement  */
  YYSYMBOL_table_element_array = 226,      /* table_element_array  */
  YYSYMBOL_column_def_array = 227,         /* column_def_array  */
  YYSYMBOL_table_element = 228,            /* table_element  */
  YYSYMBOL_table_column = 229,             /* table_column  */
  YYSYMBOL_column_type = 230,              /* column_type  */
  YYSYMBOL_column_constraints = 231,       /* column_constraints  */
  YYSYMBOL_column_constraint = 232,        /* column_constraint  */
  YYSYMBOL_default_expr = 233,             /* default_expr  */
  YYSYMBOL_table_constraint = 234,         /* table_constraint  */
  YYSYMBOL_identifier_array = 235,         /* identifier_array  */
  YYSYMBOL_delete_statement = 236,         /* delete_statement  */
  YYSYMBOL_insert_statement = 237,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 238, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 239,        /* explain_statement  */
  YYSYMBOL_update_statement = 240,         /* update_statement  */
  YYSYMBOL_update_expr_array = 241,        /* update_expr_array  */
  YYSYMBOL_update_expr = 242,              /* update_expr  */
  YYSYMBOL_drop_statement = 243,           /* drop_statement  */
  YYSYMBOL_copy_statement = 244,           /* copy_statement  */
  YYSYMBOL_select_statement = 245,         /* select_statement  */
  YYSYMBOL_select_with_paren = 246,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 247,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 248, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 249, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 250, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 251,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 252,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 253,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 254,            /* order_by_type  */
  YYSYMBOL_limit_expr = 255,               /* limit_expr  */
  YYSYMBOL_offset_expr = 256,              /* offset_expr  */
  YYSYMBOL_distinct = 257,                 /* distinct  */
  YYSYMBOL_unnest_clause = 258,            /* unnest_clause  */
  YYSYMBOL_highlight_clause = 259,         /* highlight_clause  */
  YYSYMBOL_from_clause = 260,              /* from_clause  */
  YYSYMBOL_search_clause = 261,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 262, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 263,             /* where_clause  */
  YYSYMBOL_having_clause = 264,            /* having_clause  */
  YYSYMBOL_group_by_clause = 265,          /* group_by_clause  */
  YYSYMBOL_set_operator = 266,             /* set_operator  */
  YYSYMBOL_table_reference = 267,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 268,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 269,     /* table_reference_name  */
  YYSYMBOL_table_name = 270,               /* table_name  */
  YYSYMBOL_table_alias = 271,              /* table_alias  */
  YYSYMBOL_with_clause = 272,              /* with_clause  */
  YYSYMBOL_with_expr_list = 273,           /* with_expr_list  */
  YYSYMBOL_with_expr = 274,                /* with_expr  */
  YYSYMBOL_join_clause = 275,              /* join_clause  */
  YYSYMBOL_join_type = 276,                /* join_type  */
  YYSYMBOL_show_statement = 277,           /* show_statement  */
  YYSYMBOL_flush_statement = 278,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 279,       /* optimize_statement  */
  YYSYMBOL_command_statement = 280,        /* command_statement  */
  YYSYMBOL_compact_statement = 281,        /* compact_statement  */
  YYSYMBOL_admin_statement = 282,          /* admin_statement  */
  YYSYMBOL_alter_statement = 283,          /* alter_statement  */
  YYSYMBOL_expr_array = 284,               /* expr_array  */
  YYSYMBOL_insert_row_list = 285,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 286,               /* expr_alias  */
  YYSYMBOL_expr = 287,                     /* expr  */
  YYSYMBOL_operand = 288,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 289,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 290,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 291,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 292,          /* match_text_expr  */
  YYSYMBOL_query_expr = 293,               /* query_expr  */
  YYSYMBOL_fusion_expr = 294,              /* fusion_expr  */
  YYSYMBOL_sub_search = 295,               /* sub_search  */
  YYSYMBOL_sub_search_array = 296,         /* sub_search_array  */
  YYSYMBOL_function_expr = 297,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 298,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 299,             /* between_expr  */
  YYSYMBOL_in_expr = 300,                  /* in_expr  */
  YYSYMBOL_case_expr = 301,                /* case_expr  */
  YYSYMBOL_case_check_array = 302,         /* case_check_array  */
  YYSYMBOL_cast_expr = 303,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 304,            /* subquery_expr  */
  YYSYMBOL_column_expr = 305,              /* column_expr  */
  YYSYMBOL_constant_expr = 306,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 307,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 308, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 309,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 310, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 311,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 312,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 313, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 314, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 315, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 316,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 317,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 318,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 319,               /* array_expr  */
  YYSYMBOL_long_array_expr = 320,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 321, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 322,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 323, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 324,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 325,         /* copy_option_list  */
  YYSYMBOL_copy_option = 326,              /* copy_option  */
  YYSYMBOL_file_path = 327,                /* file_path  */
  YYSYMBOL_if_exists = 328,                /* if_exists  */
  YYSYMBOL_if_not_exists = 329,            /* if_not_exists  */
  YYSYMBOL_semicolon = 330,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 331,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 332,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 333, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 334,         /* index_param_list  */
  YYSYMBOL_index_param = 335,              /* index_param  */
  YYSYMBOL_index_info = 336                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 465 "parser.cpp"

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
#define YYFINAL  120
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1432

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  220
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  117
/* YYNRULES -- Number of rules.  */
#define YYNRULES  526
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1189

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   458


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
       2,     2,     2,     2,     2,     2,     2,   211,     2,     2,
     214,   215,   209,   207,   218,   208,   216,   210,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   219,   217,
     205,   204,   206,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   212,     2,   213,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197,   198,   199,   200,   201,   202,   203
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
    2294,  2300,  2306,  2312,  2320,  2326,  2332,  2341,  2351,  2364,
    2368,  2373,  2379,  2386,  2394,  2403,  2413,  2423,  2434,  2445,
    2457,  2469,  2479,  2490,  2502,  2515,  2519,  2524,  2529,  2535,
    2539,  2543,  2549,  2553,  2557,  2563,  2569,  2577,  2583,  2587,
    2593,  2597,  2603,  2608,  2613,  2620,  2629,  2639,  2648,  2660,
    2676,  2680,  2685,  2695,  2717,  2723,  2727,  2728,  2729,  2730,
    2731,  2733,  2736,  2742,  2745,  2746,  2747,  2748,  2749,  2750,
    2751,  2752,  2753,  2754,  2758,  2774,  2791,  2809,  2855,  2894,
    2937,  2984,  3008,  3031,  3052,  3073,  3082,  3093,  3104,  3118,
    3125,  3135,  3141,  3153,  3156,  3159,  3162,  3165,  3168,  3172,
    3176,  3181,  3189,  3197,  3206,  3213,  3220,  3227,  3234,  3241,
    3249,  3257,  3265,  3273,  3281,  3289,  3297,  3305,  3313,  3321,
    3329,  3337,  3367,  3375,  3384,  3392,  3401,  3409,  3415,  3422,
    3428,  3435,  3440,  3447,  3454,  3462,  3489,  3495,  3501,  3508,
    3516,  3523,  3530,  3535,  3545,  3550,  3555,  3560,  3565,  3570,
    3575,  3580,  3585,  3590,  3593,  3596,  3600,  3603,  3606,  3609,
    3613,  3616,  3619,  3623,  3627,  3632,  3637,  3640,  3644,  3648,
    3655,  3662,  3666,  3673,  3680,  3684,  3688,  3692,  3695,  3699,
    3703,  3708,  3713,  3717,  3722,  3727,  3733,  3739,  3745,  3751,
    3757,  3763,  3769,  3775,  3781,  3787,  3793,  3804,  3808,  3813,
    3844,  3854,  3859,  3864,  3869,  3875,  3879,  3880,  3882,  3883,
    3885,  3886,  3898,  3906,  3910,  3913,  3917,  3920,  3924,  3928,
    3933,  3939,  3949,  3959,  3967,  3978,  4009
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
  "UNNEST", "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT",
  "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE",
  "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP",
  "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE",
  "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT", "MULTIVECTOR",
  "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "COMMENT", "TRUE", "FALSE", "INTERVAL",
  "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS",
  "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ",
  "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG",
  "BUFFER", "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION",
  "GLOBAL", "OFF", "EXPORT", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER",
  "FOLLOWER", "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE",
  "REMOVE", "SNAPSHOT", "SNAPSHOTS", "RECOVER", "PERSISTENCE", "OBJECT",
  "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "NUMBER", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'",
  "'.'", "';'", "','", "':'", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "column_def_array", "table_element",
  "table_column", "column_type", "column_constraints", "column_constraint",
  "default_expr", "table_constraint", "identifier_array",
  "delete_statement", "insert_statement", "optional_identifier_array",
  "explain_statement", "update_statement", "update_expr_array",
  "update_expr", "drop_statement", "copy_statement", "select_statement",
  "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "unnest_clause", "highlight_clause",
  "from_clause", "search_clause", "optional_search_filter_expr",
  "where_clause", "having_clause", "group_by_clause", "set_operator",
  "table_reference", "table_reference_unit", "table_reference_name",
  "table_name", "table_alias", "with_clause", "with_expr_list",
  "with_expr", "join_clause", "join_type", "show_statement",
  "flush_statement", "optimize_statement", "command_statement",
  "compact_statement", "admin_statement", "alter_statement", "expr_array",
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

#define YYPACT_NINF (-725)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-514)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     652,   445,    35,   464,    99,   149,    99,   -81,    83,   785,
     124,   210,   253,   266,   278,   330,   340,   371,   235,   123,
     -55,   422,   241,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,   389,  -725,  -725,   456,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,   402,   402,   402,   402,   185,    99,   417,   417,
     417,   417,   417,   259,   494,    99,   -45,   479,   493,   516,
     694,  -725,  -725,  -725,  -725,  -725,  -725,  -725,   389,  -725,
    -725,  -725,  -725,  -725,   348,   539,    99,  -725,  -725,  -725,
    -725,  -725,   538,  -725,    85,   257,  -725,   561,  -725,   408,
    -725,  -725,   571,  -725,   248,    87,    99,    99,    99,    99,
    -725,  -725,  -725,  -725,    -8,  -725,   543,   387,  -725,   604,
     424,   428,   271,   663,   430,   612,   450,   574,   442,   443,
    -725,    51,  -725,   632,  -725,  -725,    24,   610,  -725,   613,
     606,   677,    99,    99,    99,   681,   624,   472,   625,   699,
      99,    99,    99,   708,   713,   717,   656,   718,   718,   553,
      40,   192,   200,  -725,   511,  -725,   384,  -725,  -725,   724,
    -725,   727,  -725,  -725,  -725,   729,  -725,  -725,  -725,  -725,
     373,  -725,  -725,  -725,    99,   521,   371,   718,  -725,   733,
    -725,   577,  -725,   732,  -725,  -725,   739,  -725,  -725,   737,
    -725,   740,   741,  -725,   742,   693,   744,   556,  -725,  -725,
    -725,  -725,    24,  -725,  -725,  -725,   553,   700,   686,   682,
     621,   -15,  -725,   472,  -725,    99,   753,    45,  -725,  -725,
    -725,  -725,  -725,   696,  -725,   554,     5,  -725,   553,  -725,
    -725,   687,   689,   549,  -725,  -725,   759,   667,   558,   559,
     399,   755,   771,   772,   773,  -725,  -725,   774,   564,   249,
     565,   568,   709,   709,  -725,    28,   438,   111,  -725,    -2,
     821,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,  -725,  -725,   567,  -725,  -725,  -725,    42,  -725,
    -725,    55,  -725,   132,  -725,  -725,  -725,   221,  -725,   227,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,   781,   779,  -725,
    -725,  -725,  -725,  -725,  -725,   743,   747,   710,   712,   456,
    -725,  -725,  -725,   783,   252,  -725,   789,  -725,  -725,   721,
     127,  -725,   792,   583,   584,   -24,   553,   553,   736,  -725,
     796,   -55,    41,   752,   593,  -725,   112,   594,  -725,    99,
     553,   717,  -725,   358,   599,   600,   206,  -725,  -725,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,   709,
     601,   187,   748,   553,   553,   128,   380,  -725,  -725,  -725,
    -725,   759,  -725,   813,   614,   616,   617,   619,   820,   830,
     327,   327,  -725,   622,  -725,  -725,  -725,  -725,   627,    95,
     776,   553,   844,   553,   553,   -26,   635,   163,   709,   709,
     709,   709,   709,   709,   709,   709,   709,   709,   709,   709,
     709,   709,    38,  -725,   641,  -725,   850,  -725,   853,  -725,
     854,  -725,   856,   824,   525,   649,   650,   865,   659,  -725,
     665,  -725,   872,  -725,   374,   878,   719,   722,  -725,  -725,
    -725,   553,   809,   669,  -725,   144,   358,   553,  -725,  -725,
     129,   961,   757,   675,   208,  -725,  -725,  -725,   -55,   888,
     761,  -725,   906,   553,   695,  -725,   358,  -725,    37,    37,
     553,  -725,   275,   748,   754,   697,    17,   -30,   404,  -725,
     553,   553,   839,   553,   911,    39,   553,   701,   299,   463,
    -725,  -725,   718,  -725,  -725,  -725,   766,   723,   709,   438,
     802,  -725,   171,   171,   343,   343,   838,   171,   171,   343,
     343,   327,   327,  -725,  -725,  -725,  -725,  -725,  -725,   703,
    -725,   714,  -725,  -725,  -725,   928,   930,  -725,   753,   935,
    -725,   936,  -725,  -725,   934,  -725,  -725,   937,   938,   725,
      12,   861,   553,  -725,  -725,  -725,   358,   940,  -725,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,   749,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,   750,   751,   762,   763,   765,   767,   234,   769,
     753,   921,    41,   389,   760,   953,  -725,   303,   770,   952,
     956,   964,   977,  -725,   979,   314,  -725,   328,   345,  -725,
     778,  -725,   961,   553,  -725,   553,   -10,    62,   709,   -76,
     780,  -725,   142,   130,    46,   782,  -725,   987,  -725,  -725,
     917,   438,   171,   784,   366,  -725,   709,   990,   995,   951,
     955,   394,   405,  -725,   804,   421,  -725,  1003,  -725,  -725,
     -55,   794,   468,  -725,   217,  -725,   553,   835,  -725,  -725,
    1008,   495,   913,   997,  1014,  1031,  1048,   885,   892,  -725,
    -725,   224,  -725,   884,   753,   423,   807,   889,  -725,   859,
    -725,  -725,   553,  -725,  -725,  -725,  -725,  -725,  -725,    37,
    -725,  -725,  -725,   819,   358,   121,  -725,   553,   720,   812,
    1032,   641,   823,   822,   553,  -725,   826,   825,   852,   425,
    -725,  -725,   187,  1068,  1069,  -725,  -725,   935,   563,  -725,
     936,   376,    27,    12,   981,  -725,  -725,  -725,  -725,  -725,
    -725,   982,  -725,  1074,  -725,   725,   326,   656,   433,   860,
     863,   864,   874,   875,   879,   880,   881,   882,  1004,   891,
     895,   896,   897,   898,   908,   909,   912,   914,   915,  1018,
     925,   926,   929,   931,   932,   939,   941,   942,   943,   944,
    1039,   945,   946,   947,   948,   949,   954,   957,   958,   959,
     960,  1056,   962,   963,   965,   966,   967,   968,   969,   970,
     971,   972,  1063,   973,   974,   975,   976,   978,   980,   983,
     984,   985,   986,  1064,   988,  -725,  -725,    31,  -725,  1025,
    1036,   475,  -725,   936,  1166,  1168,   476,  -725,  -725,  -725,
     358,  -725,   570,   989,   991,   992,    15,   993,  -725,  -725,
    -725,  1108,   998,   358,  -725,    37,  -725,  -725,  -725,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  1173,  -725,   217,   468,
      12,    12,   994,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
     996,  1123,  -725,  1176,  1189,  1191,  1193,  1194,  1199,  1206,
    1209,  1210,  1211,  1000,  1213,  1214,  1215,  1216,  1217,  1218,
    1219,  1220,  1221,  1222,  1011,  1224,  1225,  1226,  1227,  1228,
    1229,  1230,  1231,  1232,  1233,  1022,  1235,  1236,  1237,  1238,
    1239,  1240,  1241,  1242,  1243,  1244,  1033,  1246,  1247,  1248,
    1249,  1250,  1251,  1252,  1253,  1254,  1255,  1044,  1257,  1258,
    1259,  1260,  1261,  1262,  1263,  1264,  1265,  1266,  1055,  1268,
    -725,  1271,  1272,  -725,   477,  -725,   712,  -725,  -725,  1273,
      96,  1065,  1274,  1275,  -725,   481,  1276,  -725,  -725,  1223,
     753,   326,  1234,  1245,  1066,  1070,  1072,  1073,  1075,  1076,
    1077,  1078,  1079,  1080,  1277,  1081,  1082,  1084,  1085,  1086,
    1087,  1088,  1089,  1090,  1091,  1283,  1092,  1093,  1094,  1095,
    1096,  1097,  1098,  1099,  1100,  1101,  1311,  1103,  1104,  1105,
    1106,  1107,  1109,  1110,  1111,  1112,  1113,  1317,  1114,  1115,
    1116,  1117,  1118,  1119,  1120,  1121,  1122,  1124,  1332,  1125,
    1126,  1127,  1128,  1129,  1130,  1131,  1132,  1133,  1134,  1344,
    1136,  -725,  -725,  -725,  -725,  1135,   822,  1188,  1137,  1138,
    -725,   411,   553,   482,  -725,   553,   553,  -725,  -725,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  1139,  -725,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  1142,
    -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    1143,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  1144,  -725,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,  1145,  -725,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,  -725,  1146,  -725,  1358,  1148,  1315,  1361,   212,
    1151,  1362,  1363,  -725,  -725,  -725,   358,  -725,   725,   358,
    -725,  -725,  -725,  -725,  -725,  -725,  1149,  1207,  1156,  1154,
     822,   712,  1367,   545,   216,  1159,  1326,  1372,  1373,  1163,
    -725,   552,  1374,  -725,   822,   712,  1165,  1167,   822,    10,
    1375,  -725,  1335,  1169,  -725,  1381,  -725,  1171,  1347,  1348,
    -725,  -725,  -725,    14,  1174,   -38,  -725,  1177,  1350,  1352,
    -725,  -725,  1353,  1354,  1392,  -725,  1182,  -725,  1183,  1184,
    1395,  1397,   712,  1186,  1187,  -725,   712,  -725,  -725
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     233,     0,     0,     0,     0,     0,     0,     0,   233,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     233,     0,   511,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   176,   175,     0,     8,    14,    15,    16,    17,
      18,    19,   509,   509,   509,   509,   509,     0,   507,   507,
     507,   507,   507,   226,     0,     0,     0,     0,     0,     0,
     233,   162,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   247,   248,   246,
     252,   256,     0,   253,     0,     0,   249,     0,   251,     0,
     274,   276,     0,   254,     0,   280,     0,     0,     0,     0,
     284,   285,   286,   289,   226,   287,     0,   232,   234,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,   233,     2,   216,   218,   219,     0,   199,   181,   187,
       0,     0,     0,     0,     0,     0,     0,   160,     0,     0,
       0,     0,     0,     0,     0,     0,   211,     0,     0,     0,
       0,     0,     0,   161,     0,   262,   263,   257,   258,     0,
     259,     0,   250,   275,   255,     0,   278,   277,   281,   282,
       0,   308,   306,   307,     0,     0,     0,     0,   332,     0,
     342,     0,   343,     0,   329,   330,     0,   325,   309,     0,
     338,   340,     0,   333,     0,     0,     0,     0,   180,   179,
       4,   217,     0,   177,   178,   198,     0,     0,   195,     0,
      33,     0,    34,   160,   512,     0,     0,   233,   506,   167,
     169,   168,   170,     0,   227,     0,   211,   164,     0,   156,
     505,     0,     0,   440,   444,   447,   448,     0,     0,     0,
       0,     0,     0,     0,     0,   445,   446,     0,     0,     0,
       0,     0,     0,     0,   442,     0,   233,     0,   350,   355,
     356,   370,   368,   371,   369,   372,   373,   365,   360,   359,
     358,   366,   367,   357,   364,   363,   455,   457,     0,   458,
     466,     0,   467,     0,   459,   456,   477,     0,   478,     0,
     454,   293,   295,   294,   291,   292,   298,   300,   299,   296,
     297,   303,   305,   304,   301,   302,   283,     0,     0,   265,
     264,   270,   260,   261,   279,     0,     0,     0,   515,     0,
     235,   290,   335,     0,   326,   331,   310,   339,   334,     0,
       0,   341,     0,     0,     0,   203,     0,     0,   197,   508,
       0,   233,     0,     0,     0,   154,     0,     0,   158,     0,
       0,     0,   163,   210,     0,     0,     0,   486,   485,   488,
     487,   490,   489,   492,   491,   494,   493,   496,   495,     0,
       0,   406,   233,     0,     0,     0,     0,   449,   450,   451,
     452,     0,   453,     0,     0,     0,     0,     0,     0,     0,
     408,   407,   483,   480,   474,   464,   469,   472,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   463,     0,   468,     0,   471,     0,   479,
       0,   482,     0,   271,   266,     0,     0,     0,     0,   288,
       0,   344,     0,   327,     0,     0,     0,     0,   337,   184,
     183,     0,   205,   186,   188,   193,   194,     0,   182,    32,
      36,     0,     0,     0,     0,    42,    46,    47,   233,     0,
      40,   159,     0,     0,   157,   171,   166,   165,     0,     0,
       0,   401,     0,   233,     0,     0,     0,     0,     0,   431,
       0,     0,     0,     0,     0,     0,     0,   209,     0,     0,
     362,   361,     0,   351,   354,   424,   425,     0,     0,   233,
       0,   405,   415,   416,   419,   420,     0,   422,   414,   417,
     418,   410,   409,   411,   412,   413,   441,   443,   465,     0,
     470,     0,   473,   481,   484,     0,     0,   267,     0,     0,
     347,     0,   236,   328,     0,   311,   336,     0,     0,   202,
       0,   201,     0,   191,   192,   190,   196,     0,    52,    55,
      56,    53,    54,    57,    58,    74,    59,    61,    60,    77,
      64,    65,    66,    62,    63,    67,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,     0,   515,     0,
       0,   517,     0,    39,     0,     0,   155,     0,     0,     0,
       0,     0,     0,   501,     0,     0,   497,     0,     0,   402,
       0,   436,     0,     0,   429,     0,     0,     0,     0,     0,
       0,   440,     0,     0,     0,     0,   391,     0,   476,   475,
       0,   233,   423,     0,     0,   404,     0,     0,     0,   272,
     268,     0,     0,    44,   520,     0,   518,   312,   345,   346,
     233,   204,   220,   222,   231,   223,     0,   207,   189,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   147,
     148,   151,   144,   151,     0,     0,     0,    35,    43,   526,
      41,   352,     0,   503,   502,   500,   499,   504,   174,     0,
     172,   403,   437,     0,   433,     0,   432,     0,     0,     0,
       0,     0,     0,   209,     0,   389,     0,     0,     0,     0,
     438,   427,   426,     0,     0,   349,   348,     0,     0,   514,
       0,     0,     0,     0,     0,   240,   241,   242,   243,   239,
     244,     0,   229,     0,   224,   200,     0,   211,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   149,   146,     0,   145,    49,
      48,     0,   153,     0,     0,     0,     0,   498,   435,   430,
     434,   421,     0,     0,   209,     0,     0,     0,   460,   462,
     461,     0,     0,   208,   392,     0,   439,   428,   273,   269,
      45,   521,   522,   524,   523,   519,     0,   313,   231,   221,
       0,     0,   228,   395,   393,   396,   394,   397,   398,   399,
     206,   215,    76,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     150,     0,     0,   152,     0,    37,   515,   353,   480,     0,
       0,     0,     0,     0,   390,     0,   314,   225,   237,     0,
       0,     0,     0,   213,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    51,    50,   516,   525,     0,   209,   385,     0,   209,
     173,     0,     0,     0,   400,     0,     0,   185,    75,    81,
      82,    79,    80,    83,    84,    85,    86,    87,     0,    78,
     125,   126,   123,   124,   127,   128,   129,   130,   131,     0,
     122,    92,    93,    90,    91,    94,    95,    96,    97,    98,
       0,    89,   103,   104,   101,   102,   105,   106,   107,   108,
     109,     0,   100,   136,   137,   134,   135,   138,   139,   140,
     141,   142,     0,   133,   114,   115,   112,   113,   116,   117,
     118,   119,   120,     0,   111,     0,     0,     0,     0,     0,
       0,     0,     0,   316,   315,   321,   238,   230,   214,   212,
      88,   132,    99,   110,   143,   121,   209,   386,     0,     0,
     209,   515,   322,   317,     0,     0,     0,     0,     0,     0,
     384,     0,     0,   318,   209,   515,     0,     0,   209,   515,
       0,   323,   319,     0,   380,     0,   387,     0,     0,     0,
     383,   324,   320,   515,     0,   374,   382,     0,     0,     0,
     379,   388,     0,     0,     0,   378,     0,   376,     0,     0,
       0,     0,   515,     0,     0,   381,   515,   375,   377
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -725,  -725,  -725,  1282,  1345,   119,  -725,  -725,   814,  -516,
     795,  -725,   738,   731,  -725,  -531,   165,   194,  1195,  -725,
     244,  -725,  1059,   264,   286,    -6,  1391,   -17,  1102,  1212,
     -99,  -725,  -725,   866,  -725,  -725,  -725,  -725,  -725,  -725,
    -725,  -725,  -695,  -221,  -725,  -725,  -725,  -725,   690,  -247,
      22,   569,  -725,  -725,  1256,  -725,  -725,   287,   297,   309,
     324,   341,  -725,  -725,  -206,  -725,  1015,  -227,  -228,  -724,
    -722,  -720,  -719,  -718,  -717,   461,  -725,  -725,  -725,  -725,
    -725,  -725,  1040,  -725,  -725,   924,   615,  -249,  -725,  -725,
    -725,   726,  -725,  -725,  -725,  -725,   728,   999,  1001,    29,
    -725,  -725,  -725,  -725,  1179,  -469,   734,  -137,   578,   588,
    -725,  -725,  -584,  -725,   607,   704,  -725
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   464,   642,   465,   466,
     588,   671,   672,   809,   467,   346,    25,    26,   217,    27,
      28,   226,   227,    29,    30,    31,    32,    33,   128,   203,
     129,   208,   453,   454,   555,   338,   458,   206,   657,   452,
     551,   737,   625,   229,  1037,   953,   126,   651,   652,   653,
     654,   734,    34,   107,   108,   655,   731,    35,    36,    37,
      38,    39,    40,    41,   257,   474,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   859,   860,   267,   268,   269,
     270,   271,   376,   272,   273,   274,   275,   276,   827,   277,
     278,   279,   280,   281,   282,   283,   284,   396,   397,   285,
     286,   287,   288,   289,   290,   605,   606,   231,   139,   131,
     122,   136,   439,   677,   645,   646,   470
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     335,   353,    68,   119,   673,   352,   395,   641,   832,   371,
     607,   232,   853,   375,   854,    53,   855,   856,   857,   858,
     392,   393,    17,   643,   390,   391,    54,   204,    56,   399,
     147,   148,   127,   392,   393,   234,   235,   236,   105,   341,
     321,   526,   621,   291,   461,   292,   293,   123,   507,   124,
     703,  -510,   402,   174,    68,   125,   613,   451,     1,   675,
       2,     3,     4,     5,     6,     7,     8,     9,    10,   137,
     228,   612,    47,   696,    11,    12,    13,   146,   403,   404,
      14,    15,    16,   347,    57,    58,    60,   438,  1172,    59,
       1,   438,     2,     3,     4,     5,     6,     7,   156,     9,
    1026,   294,    53,   334,   599,   600,    11,    12,    13,   455,
     456,   704,    14,    15,    16,   601,   602,   603,   170,   171,
     172,   173,    17,   476,   403,   404,  1173,    62,    17,   941,
     110,   508,   241,   242,   243,   111,  1158,   112,   244,   113,
    1168,   371,   699,   811,   403,   404,   486,   487,   697,   123,
     482,   124,   403,   404,   211,   212,   213,   125,    96,    20,
      17,   704,   220,   221,   222,   245,   246,   247,   462,   149,
     463,   403,   404,    63,  1159,   528,   505,   506,  1169,    62,
     512,   513,   514,   515,   516,   517,   518,   519,   520,   521,
     522,   523,   524,   525,   401,   296,   318,   297,   298,   342,
     348,   840,    64,   301,   819,   302,   303,   295,   144,   233,
     234,   235,   236,   553,   554,   374,   403,   404,  1130,    18,
     732,   604,  1144,   351,    55,    63,   650,   853,   394,   854,
     556,   855,   856,   857,   858,   510,    19,   344,   202,   398,
     255,   394,   848,   255,    97,   549,  -513,   527,   254,   403,
     404,    18,    65,   299,    64,   423,   130,   158,   159,   484,
     424,   304,   557,   616,   617,    20,   619,   597,   425,   623,
     480,   733,    66,   426,   608,   403,   404,   704,   237,   238,
     632,   704,   403,   404,   403,   404,   168,    98,   239,   169,
     240,   114,   442,   511,    67,    69,   667,    20,   403,   404,
      99,   443,   407,   634,    65,    70,   667,   241,   242,   243,
     501,   438,   115,   244,   446,   447,   116,    71,   407,   117,
    -514,  -514,   410,   411,    66,   455,   400,   471,  -514,   401,
     472,  1106,    72,   103,  1110,   460,   408,   409,   410,   411,
     245,   246,   247,   104,   413,   427,    67,    69,   702,    73,
     428,   668,  1024,   669,   670,   485,   807,    70,   422,   300,
     701,   668,   248,   669,   670,   630,   945,   305,   384,    71,
     385,   475,   386,   387,   106,  -514,   415,   416,   417,   418,
     419,   420,   421,   315,    72,   249,   694,   250,   695,   251,
     698,   414,   415,   416,   417,   418,   419,   420,   421,   316,
     317,    73,   233,   234,   235,   236,   109,   544,   712,   123,
     846,   124,   847,   252,   253,   254,   545,   125,   255,  1033,
     256,   481,   120,   591,   307,   709,   592,   308,   309,   160,
     161,  1135,   310,   311,   429,  1139,   100,   101,   102,   430,
     431,   233,   234,   235,   236,   432,   165,   166,   167,  1153,
     735,  1111,   825,  1157,  1112,  1113,   180,   181,   121,  1114,
    1115,   182,   593,   489,   127,   490,   610,   491,   628,   629,
     820,   237,   238,   130,   407,   144,   816,   833,    42,    43,
      44,   239,   150,   240,    45,    46,   374,   614,   138,   615,
     609,   491,   633,   401,  -514,  -514,   151,    48,    49,    50,
     241,   242,   243,    51,    52,   249,   244,   250,   145,   251,
     237,   238,   403,   404,   626,    17,   861,   627,   681,   152,
     239,   401,   240,   724,  -245,   725,   726,   727,   728,   688,
     729,   730,   689,   245,   246,   247,   419,   420,   421,   241,
     242,   243,   155,   690,   157,   244,   689,  1140,  -514,  -514,
     417,   418,   419,   420,   421,   248,   233,   234,   235,   236,
     691,  1154,   154,   401,   162,  1160,   841,   842,   843,   844,
     536,   537,   245,   246,   247,   392,   938,   164,   249,  1170,
     250,   711,   251,   163,   401,   739,   740,   741,   742,   743,
    1142,  1143,   744,   745,   248,  1150,  1151,   175,  1185,   746,
     747,   748,  1188,   948,   949,   176,   252,   253,   254,   715,
     177,   255,   472,   256,   708,   749,   195,   249,   178,   250,
     716,   251,   179,   717,   194,   237,   238,   140,   141,   142,
     143,   132,   133,   134,   135,   239,   719,   240,   812,   720,
     837,   472,   196,   401,   722,   252,   253,   254,   862,   197,
     255,   863,   256,   201,   241,   242,   243,   198,   199,     1,
     244,     2,     3,     4,     5,     6,     7,     8,     9,    10,
     233,   234,   235,   236,   205,    11,    12,    13,   209,   207,
     210,    14,    15,    16,   214,   215,   216,   245,   246,   247,
     933,   937,  1023,   472,   401,   720,  1030,  1117,   218,   689,
     472,     1,   219,     2,     3,     4,     5,     6,     7,   248,
       9,   223,   233,   234,   235,   236,   224,    11,    12,    13,
     225,   228,   230,    14,    15,    16,   306,   312,   823,    17,
     313,   830,   249,   314,   250,   319,   251,   322,   324,   369,
     370,   323,   325,   326,   327,   328,   329,   330,   331,   239,
     332,   240,   336,   337,   340,   339,   345,   349,   350,   377,
     252,   253,   254,   356,   354,   255,   355,   256,   241,   242,
     243,    17,   372,   373,   244,   378,   379,   380,   383,   388,
     381,   369,   389,   422,   433,   434,   437,   441,    74,   438,
     435,   239,   484,   240,   436,   444,   448,   445,   449,   450,
     459,   245,   246,   247,   457,  1116,   468,   469,   473,  1119,
     241,   242,   243,   478,   479,   483,   244,   492,    75,    76,
      18,    77,   183,   248,   497,    17,    78,    79,   493,  1118,
     494,   495,   184,   496,   498,   185,   186,    19,   187,   188,
     189,   499,   500,   245,   246,   247,   249,   504,   250,   509,
     251,   407,   502,   255,   190,   191,   529,   192,   193,   531,
     533,   534,    18,   538,   539,   248,    20,   535,   540,   408,
     409,   410,   411,   541,   252,   253,   254,   413,   543,   255,
     542,   256,   546,   547,   550,   589,   548,   552,   249,   590,
     250,   594,   251,   405,   595,   406,   357,   358,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,    20,   596,
     484,   508,   611,   598,   618,   620,   252,   253,   254,   624,
     403,   255,   637,   256,   414,   415,   416,   417,   418,   419,
     420,   421,   635,   638,   639,   821,   640,   631,   461,   644,
     647,   648,   649,   401,   659,    80,    81,    82,    83,   656,
      84,    85,   407,   676,    86,    87,    88,   680,   683,    89,
      90,    91,   684,   660,   661,   662,    92,    93,   685,   407,
     408,   409,   410,   411,   412,   679,   663,   664,   413,   665,
     686,   666,    94,   674,   682,   687,    95,   408,   409,   410,
     411,   706,   636,   692,   707,   413,   629,   705,   700,   710,
     628,   713,   714,   750,   751,   752,   753,   754,   718,   721,
     755,   756,   723,   736,   738,   805,   807,   757,   758,   759,
     806,   813,   814,   815,   822,   414,   415,   416,   417,   418,
     419,   420,   421,   760,   818,   826,   824,   850,   851,   835,
     831,   834,   414,   415,   416,   417,   418,   419,   420,   421,
     558,   559,   560,   561,   562,   563,   564,   565,   566,   567,
     568,   569,   570,   571,   572,   573,   574,   836,   575,   576,
     577,   578,   579,   580,   838,   839,   581,   852,   864,   582,
     583,   865,   866,   584,   585,   586,   587,   761,   762,   763,
     764,   765,   867,   868,   766,   767,   873,   869,   870,   871,
     872,   768,   769,   770,   772,   773,   774,   775,   776,   874,
     884,   777,   778,   875,   876,   877,   878,   771,   779,   780,
     781,   783,   784,   785,   786,   787,   879,   880,   788,   789,
     881,   895,   882,   883,   782,   790,   791,   792,   794,   795,
     796,   797,   798,   885,   886,   799,   800,   887,   906,   888,
     889,   793,   801,   802,   803,   917,   928,   890,   931,   891,
     892,   893,   894,   896,   897,   898,   899,   900,   804,   932,
     935,   936,   901,   704,   952,   902,   903,   904,   905,   946,
     907,   908,   954,   909,   910,   911,   912,   913,   914,   915,
     916,   918,   919,   920,   921,   955,   922,   956,   923,   957,
     958,   924,   925,   926,   927,   959,   929,   939,   950,   940,
     942,   943,   960,   944,   951,   961,   962,   963,   964,   965,
     966,   967,   968,   969,   970,   971,   972,   973,   974,   975,
     976,   977,   978,   979,   980,   981,   982,   983,   984,   985,
     986,   987,   988,   989,   990,   991,   992,   993,   994,   995,
     996,   997,   998,   999,  1000,  1001,  1002,  1003,  1004,  1005,
    1006,  1007,  1008,  1009,  1010,  1011,  1012,  1013,  1014,  1015,
    1016,  1017,  1018,  1019,  1020,  1021,  1022,  1025,  1028,  1029,
    1027,  1038,  1031,  1048,  1032,  1039,  1035,  1040,  1041,  1059,
    1042,  1043,  1044,  1045,  1046,  1047,  1049,  1050,  1036,  1051,
    1052,  1053,  1054,  1055,  1056,  1057,  1058,  1060,  1061,  1062,
    1063,  1064,  1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,
    1073,  1074,  1075,  1081,  1076,  1077,  1078,  1079,  1080,  1082,
    1083,  1084,  1085,  1086,  1087,  1088,  1089,  1090,  1092,  1091,
    1093,  1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1102,
    1103,  1104,  1107,  1105,  1120,  1108,  1109,  1121,  1122,  1123,
    1124,  1125,  1126,  1127,  1128,  1129,  1131,  1134,  1132,  1133,
    1137,  1136,  1138,  1141,  1145,  1146,  1147,  1148,  1149,  1155,
    1152,  1161,  1156,  1162,  1163,  1164,  1165,  1166,  1167,  1171,
    1175,  1174,  1176,  1177,  1178,  1179,  1180,  1181,  1183,  1182,
    1184,  1186,  1187,   200,   810,   153,   678,   693,   343,   808,
     477,   118,  1034,   849,   333,   488,   503,   947,   658,   622,
     934,   440,   930,   817,   845,   530,   382,     0,   828,   532,
     829,     0,   320
};

static const yytype_int16 yycheck[] =
{
     206,   228,     8,    20,   588,   226,   255,   538,   703,   237,
     479,   148,   736,   240,   736,     3,   736,   736,   736,   736,
       5,     6,    77,   539,   252,   253,     4,   126,     6,   256,
      75,    76,     8,     5,     6,     4,     5,     6,    16,    54,
     177,     3,     3,     3,     3,     5,     6,    20,    74,    22,
       4,     0,    54,    61,    60,    28,    86,    81,     7,   590,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    47,
      65,    54,    37,    83,    23,    24,    25,    55,   154,   155,
      29,    30,    31,    38,   165,   166,     3,    77,   126,   170,
       7,    77,     9,    10,    11,    12,    13,    14,    76,    16,
       4,    61,     3,   202,    67,    68,    23,    24,    25,   336,
     337,    65,    29,    30,    31,    78,    79,    80,    96,    97,
      98,    99,    77,   350,   154,   155,   164,     8,    77,   824,
       7,   157,   101,   102,   103,    12,   126,    14,   107,    16,
     126,   369,   218,   674,   154,   155,   373,   374,    86,    20,
     356,    22,   154,   155,   132,   133,   134,    28,    34,   214,
      77,    65,   140,   141,   142,   134,   135,   136,   127,   214,
     129,   154,   155,     8,   164,   424,   403,   404,   164,    60,
     408,   409,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   218,     3,   174,     5,     6,   214,
     217,   717,     8,     3,    83,     5,     6,   167,   216,     3,
       4,     5,     6,    69,    70,    87,   154,   155,     6,   168,
       3,   184,     6,   218,    75,    60,   214,   951,   213,   951,
     457,   951,   951,   951,   951,    72,   185,   215,   214,   256,
     212,   213,   215,   212,    34,   451,    61,   209,   209,   154,
     155,   168,     8,    61,    60,   213,    71,   172,   173,    72,
     218,    61,   133,   490,   491,   214,   493,   473,   213,   496,
      64,    54,     8,   218,   480,   154,   155,    65,    72,    73,
     508,    65,   154,   155,   154,   155,   199,    34,    82,   202,
      84,   168,    40,   130,     8,     8,    72,   214,   154,   155,
      34,    49,   131,   509,    60,     8,    72,   101,   102,   103,
     215,    77,   189,   107,   187,   188,   193,     8,   131,   196,
     149,   150,   151,   152,    60,   552,   215,   215,   157,   218,
     218,  1026,     8,     3,  1029,   341,   149,   150,   151,   152,
     134,   135,   136,     3,   157,   213,    60,    60,   218,     8,
     218,   127,   936,   129,   130,   372,   132,    60,   216,   167,
     218,   127,   156,   129,   130,   502,   835,   167,   119,    60,
     121,   349,   123,   124,     3,   204,   205,   206,   207,   208,
     209,   210,   211,    10,    60,   179,   613,   181,   615,   183,
     618,   204,   205,   206,   207,   208,   209,   210,   211,    26,
      27,    60,     3,     4,     5,     6,   171,    33,   636,    20,
      34,    22,    36,   207,   208,   209,    42,    28,   212,   950,
     214,   215,     0,   215,    40,   631,   218,    43,    44,   172,
     173,  1126,    48,    49,   213,  1130,   158,   159,   160,   218,
     213,     3,     4,     5,     6,   218,   198,   199,   200,  1144,
     656,    40,   701,  1148,    43,    44,   185,   186,   217,    48,
      49,   190,   468,    83,     8,    85,   483,    87,     5,     6,
     697,    72,    73,    71,   131,   216,   682,   704,    33,    34,
      35,    82,     3,    84,    39,    40,    87,    83,    71,    85,
     215,    87,   509,   218,   151,   152,     3,    33,    34,    35,
     101,   102,   103,    39,    40,   179,   107,   181,    14,   183,
      72,    73,   154,   155,   215,    77,   737,   218,   215,     3,
      82,   218,    84,    55,    56,    57,    58,    59,    60,   215,
      62,    63,   218,   134,   135,   136,   209,   210,   211,   101,
     102,   103,     3,   215,     6,   107,   218,  1131,   205,   206,
     207,   208,   209,   210,   211,   156,     3,     4,     5,     6,
     215,  1145,   214,   218,     3,  1149,     3,     4,     5,     6,
      45,    46,   134,   135,   136,     5,     6,     6,   179,  1163,
     181,   215,   183,   175,   218,    90,    91,    92,    93,    94,
      45,    46,    97,    98,   156,    43,    44,    54,  1182,   104,
     105,   106,  1186,   850,   851,   218,   207,   208,   209,   215,
       6,   212,   218,   214,   631,   120,     4,   179,   194,   181,
     215,   183,   194,   218,   194,    72,    73,    49,    50,    51,
      52,    43,    44,    45,    46,    82,   215,    84,   215,   218,
     215,   218,   192,   218,   650,   207,   208,   209,   215,    75,
     212,   218,   214,    21,   101,   102,   103,   215,   215,     7,
     107,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       3,     4,     5,     6,    64,    23,    24,    25,    72,    66,
       3,    29,    30,    31,     3,    61,   214,   134,   135,   136,
     215,   215,   215,   218,   218,   218,   215,   215,    73,   218,
     218,     7,     3,     9,    10,    11,    12,    13,    14,   156,
      16,     3,     3,     4,     5,     6,     3,    23,    24,    25,
       3,    65,     4,    29,    30,    31,   215,     3,   699,    77,
       3,   702,   179,     4,   181,   214,   183,     4,     6,    72,
      73,   164,     3,     6,     4,     4,     4,    54,     4,    82,
     194,    84,    52,    67,   133,    73,     3,    61,   204,     4,
     207,   208,   209,   214,    77,   212,    77,   214,   101,   102,
     103,    77,   214,   214,   107,     4,     4,     4,   214,   214,
       6,    72,   214,   216,     3,     6,    76,     4,     3,    77,
      47,    82,    72,    84,    47,     6,     4,    76,   215,   215,
       4,   134,   135,   136,    68,  1032,    54,   214,   214,  1036,
     101,   102,   103,   214,   214,   214,   107,     4,    33,    34,
     168,    36,   159,   156,     4,    77,    41,    42,   214,  1035,
     214,   214,   169,   214,     4,   172,   173,   185,   175,   176,
     177,   219,   215,   134,   135,   136,   179,     3,   181,   214,
     183,   131,    76,   212,   191,   192,     6,   194,   195,     6,
       6,     5,   168,   214,   214,   156,   214,    43,     3,   149,
     150,   151,   152,   214,   207,   208,   209,   157,     6,   212,
     215,   214,     4,   164,    75,   128,   164,   218,   179,   214,
     181,     3,   183,    72,   133,    74,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   214,     3,
      72,   157,   215,   218,    75,     4,   207,   208,   209,   218,
     154,   212,   219,   214,   204,   205,   206,   207,   208,   209,
     210,   211,   130,   219,     6,   215,     6,   214,     3,     3,
       6,     4,     4,   218,     4,   160,   161,   162,   163,    88,
     165,   166,   131,    32,   169,   170,   171,     4,     6,   174,
     175,   176,     6,   214,   214,   214,   181,   182,     4,   131,
     149,   150,   151,   152,   153,   215,   214,   214,   157,   214,
       3,   214,   197,   214,   214,     6,   201,   149,   150,   151,
     152,     4,   154,   215,    77,   157,     6,   215,   218,   215,
       5,    50,    47,    90,    91,    92,    93,    94,   204,     6,
      97,    98,   218,   178,     6,   130,   132,   104,   105,   106,
     128,   214,   133,   164,   212,   204,   205,   206,   207,   208,
     209,   210,   211,   120,   215,   212,     4,    56,    56,   214,
     218,   215,   204,   205,   206,   207,   208,   209,   210,   211,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   215,   107,   108,
     109,   110,   111,   112,     6,     6,   115,     3,   218,   118,
     119,   218,   218,   122,   123,   124,   125,    90,    91,    92,
      93,    94,   218,   218,    97,    98,    92,   218,   218,   218,
     218,   104,   105,   106,    90,    91,    92,    93,    94,   218,
      92,    97,    98,   218,   218,   218,   218,   120,   104,   105,
     106,    90,    91,    92,    93,    94,   218,   218,    97,    98,
     218,    92,   218,   218,   120,   104,   105,   106,    90,    91,
      92,    93,    94,   218,   218,    97,    98,   218,    92,   218,
     218,   120,   104,   105,   106,    92,    92,   218,   133,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   120,   133,
       4,     3,   218,    65,    51,   218,   218,   218,   218,     6,
     218,   218,     6,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,     6,   218,     6,   218,     6,
       6,   218,   218,   218,   218,     6,   218,   218,   214,   218,
     218,   218,     6,   215,   218,     6,     6,     6,   218,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   218,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     218,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   218,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   218,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   218,     6,     4,     4,     4,     4,     4,
     215,   215,     6,     6,    61,   215,    52,   215,   215,     6,
     215,   215,   215,   215,   215,   215,   215,   215,    53,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,     6,   215,   215,
     215,   215,   215,     6,   215,   215,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,     6,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
       6,   215,   164,   218,   215,   218,   218,   215,   215,   215,
     215,   215,     4,   215,    49,     4,   215,   218,     6,     6,
     214,   164,   218,     6,   215,    49,     4,     4,   215,   214,
       6,     6,   215,    48,   215,     4,   215,    40,    40,   215,
      40,   214,    40,    40,    40,     3,   214,   214,     3,   215,
       3,   215,   215,   121,   673,    60,   592,   612,   213,   671,
     351,    20,   951,   723,   202,   375,   401,   848,   552,   495,
     813,   319,   807,   689,   720,   426,   247,    -1,   702,   428,
     702,    -1,   176
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    77,   168,   185,
     214,   221,   222,   223,   225,   236,   237,   239,   240,   243,
     244,   245,   246,   247,   272,   277,   278,   279,   280,   281,
     282,   283,    33,    34,    35,    39,    40,    37,    33,    34,
      35,    39,    40,     3,   270,    75,   270,   165,   166,   170,
       3,   224,   225,   236,   237,   240,   243,   244,   245,   277,
     278,   279,   280,   281,     3,    33,    34,    36,    41,    42,
     160,   161,   162,   163,   165,   166,   169,   170,   171,   174,
     175,   176,   181,   182,   197,   201,    34,    34,    34,    34,
     158,   159,   160,     3,     3,   270,     3,   273,   274,   171,
       7,    12,    14,    16,   168,   189,   193,   196,   246,   247,
       0,   217,   330,    20,    22,    28,   266,     8,   248,   250,
      71,   329,   329,   329,   329,   329,   331,   270,    71,   328,
     328,   328,   328,   328,   216,    14,   270,    75,    76,   214,
       3,     3,     3,   224,   214,     3,   270,     6,   172,   173,
     172,   173,     3,   175,     6,   198,   199,   200,   199,   202,
     270,   270,   270,   270,    61,    54,   218,     6,   194,   194,
     185,   186,   190,   159,   169,   172,   173,   175,   176,   177,
     191,   192,   194,   195,   194,     4,   192,    75,   215,   215,
     223,    21,   214,   249,   250,    64,   257,    66,   251,    72,
       3,   270,   270,   270,     3,    61,   214,   238,    73,     3,
     270,   270,   270,     3,     3,     3,   241,   242,    65,   263,
       4,   327,   327,     3,     4,     5,     6,    72,    73,    82,
      84,   101,   102,   103,   107,   134,   135,   136,   156,   179,
     181,   183,   207,   208,   209,   212,   214,   284,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   297,   298,   299,
     300,   301,   303,   304,   305,   306,   307,   309,   310,   311,
     312,   313,   314,   315,   316,   319,   320,   321,   322,   323,
     324,     3,     5,     6,    61,   167,     3,     5,     6,    61,
     167,     3,     5,     6,    61,   167,   215,    40,    43,    44,
      48,    49,     3,     3,     4,    10,    26,    27,   270,   214,
     274,   327,     4,   164,     6,     3,     6,     4,     4,     4,
      54,     4,   194,   249,   250,   284,    52,    67,   255,    73,
     133,    54,   214,   238,   270,     3,   235,    38,   247,    61,
     204,   218,   263,   287,    77,    77,   214,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,    72,
      73,   288,   214,   214,    87,   287,   302,     4,     4,     4,
       4,     6,   324,   214,   119,   121,   123,   124,   214,   214,
     288,   288,     5,     6,   213,   307,   317,   318,   247,   287,
     215,   218,    54,   154,   155,    72,    74,   131,   149,   150,
     151,   152,   153,   157,   204,   205,   206,   207,   208,   209,
     210,   211,   216,   213,   218,   213,   218,   213,   218,   213,
     218,   213,   218,     3,     6,    47,    47,    76,    77,   332,
     248,     4,    40,    49,     6,    76,   187,   188,     4,   215,
     215,    81,   259,   252,   253,   287,   287,    68,   256,     4,
     245,     3,   127,   129,   226,   228,   229,   234,    54,   214,
     336,   215,   218,   214,   285,   270,   287,   242,   214,   214,
      64,   215,   284,   214,    72,   247,   287,   287,   302,    83,
      85,    87,     4,   214,   214,   214,   214,     4,     4,   219,
     215,   215,    76,   286,     3,   287,   287,    74,   157,   214,
      72,   130,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,     3,   209,   307,     6,
     317,     6,   318,     6,     5,    43,    45,    46,   214,   214,
       3,   214,   215,     6,    33,    42,     4,   164,   164,   284,
      75,   260,   218,    69,    70,   254,   287,   133,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   107,   108,   109,   110,   111,
     112,   115,   118,   119,   122,   123,   124,   125,   230,   128,
     214,   215,   218,   245,     3,   133,     3,   284,   218,    67,
      68,    78,    79,    80,   184,   325,   326,   325,   284,   215,
     247,   215,    54,    86,    83,    85,   287,   287,    75,   287,
       4,     3,   305,   287,   218,   262,   215,   218,     5,     6,
     327,   214,   288,   247,   284,   130,   154,   219,   219,     6,
       6,   235,   227,   229,     3,   334,   335,     6,     4,     4,
     214,   267,   268,   269,   270,   275,    88,   258,   253,     4,
     214,   214,   214,   214,   214,   214,   214,    72,   127,   129,
     130,   231,   232,   332,   214,   235,    32,   333,   228,   215,
       4,   215,   214,     6,     6,     4,     3,     6,   215,   218,
     215,   215,   215,   230,   287,   287,    83,    86,   288,   218,
     218,   218,   218,     4,    65,   215,     4,    77,   247,   284,
     215,   215,   288,    50,    47,   215,   215,   218,   204,   215,
     218,     6,   245,   218,    55,    57,    58,    59,    60,    62,
      63,   276,     3,    54,   271,   284,   178,   261,     6,    90,
      91,    92,    93,    94,    97,    98,   104,   105,   106,   120,
      90,    91,    92,    93,    94,    97,    98,   104,   105,   106,
     120,    90,    91,    92,    93,    94,    97,    98,   104,   105,
     106,   120,    90,    91,    92,    93,    94,    97,    98,   104,
     105,   106,   120,    90,    91,    92,    93,    94,    97,    98,
     104,   105,   106,   120,    90,    91,    92,    93,    94,    97,
      98,   104,   105,   106,   120,   130,   128,   132,   232,   233,
     233,   235,   215,   214,   133,   164,   284,   326,   215,    83,
     287,   215,   212,   319,     4,   307,   212,   308,   311,   316,
     319,   218,   262,   287,   215,   214,   215,   215,     6,     6,
     229,     3,     4,     5,     6,   335,    34,    36,   215,   268,
      56,    56,     3,   289,   290,   291,   292,   293,   294,   295,
     296,   263,   215,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   218,    92,   218,   218,   218,   218,   218,   218,
     218,   218,   218,   218,    92,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,    92,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,    92,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,    92,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,    92,   218,
     306,   133,   133,   215,   334,     4,     3,   215,     6,   218,
     218,   262,   218,   218,   215,   325,     6,   271,   269,   269,
     214,   218,    51,   265,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   218,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   218,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   218,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   218,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   218,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   218,
       6,     4,     4,   215,   332,     4,     4,   215,     4,     4,
     215,     6,    61,   235,   295,    52,    53,   264,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,     6,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,     6,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
       6,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,     6,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,     6,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   215,     6,   215,   218,   262,   164,   218,   218,
     262,    40,    43,    44,    48,    49,   287,   215,   284,   287,
     215,   215,   215,   215,   215,   215,     4,   215,    49,     4,
       6,   215,     6,     6,   218,   262,   164,   214,   218,   262,
     332,     6,    45,    46,     6,   215,    49,     4,     4,   215,
      43,    44,     6,   262,   332,   214,   215,   262,   126,   164,
     332,     6,    48,   215,     4,   215,    40,    40,   126,   164,
     332,   215,   126,   164,   214,    40,    40,    40,    40,     3,
     214,   214,   215,     3,     3,   332,   215,   215,   332
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   220,   221,   222,   222,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   226,   226,   227,   227,   228,   228,   229,   229,
     229,   229,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   231,   231,   232,   232,   232,   232,
     233,   233,   234,   234,   235,   235,   236,   237,   237,   238,
     238,   239,   239,   240,   241,   241,   242,   243,   243,   243,
     243,   243,   244,   244,   244,   245,   245,   245,   245,   246,
     246,   247,   248,   249,   249,   250,   251,   251,   252,   252,
     253,   254,   254,   254,   255,   255,   256,   256,   257,   257,
     258,   258,   259,   259,   260,   260,   261,   261,   262,   262,
     263,   263,   264,   264,   265,   265,   266,   266,   266,   266,
     267,   267,   268,   268,   269,   269,   270,   270,   271,   271,
     271,   271,   272,   272,   273,   273,   274,   275,   275,   276,
     276,   276,   276,   276,   276,   276,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   278,   278,   278,   279,   279,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   281,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   283,   283,   283,
     284,   284,   285,   285,   286,   286,   287,   287,   287,   287,
     287,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   289,   289,   289,   290,   290,   290,
     290,   291,   291,   291,   291,   292,   292,   292,   292,   293,
     293,   294,   294,   295,   295,   295,   295,   295,   295,   296,
     296,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   298,   298,   299,   300,   300,   301,
     301,   301,   301,   302,   302,   303,   304,   304,   304,   304,
     305,   305,   305,   305,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   306,   306,   306,   307,   307,   307,   307,
     308,   308,   308,   309,   310,   310,   311,   311,   312,   313,
     313,   314,   315,   315,   316,   317,   318,   319,   319,   320,
     321,   321,   322,   323,   323,   324,   324,   324,   324,   324,
     324,   324,   324,   324,   324,   324,   324,   325,   325,   326,
     326,   326,   326,   326,   326,   327,   328,   328,   329,   329,
     330,   330,   331,   331,   332,   332,   333,   333,   334,   334,
     335,   335,   335,   335,   335,   336,   336
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
       4,     4,     4,     4,     4,     4,     3,     3,     3,     3,
       4,     6,     7,     9,    10,    12,    12,    13,    14,    15,
      16,    12,    13,    15,    16,     3,     4,     5,     6,     3,
       3,     4,     3,     3,     4,     4,     6,     5,     3,     4,
       3,     4,     3,     3,     5,     7,     7,     6,     8,     8,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,    14,    19,    16,    20,    16,    15,
      13,    18,    14,    13,    11,     8,    10,    13,    15,     5,
       7,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     4,     5,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     3,     1,     1,     2,     2,
       3,     2,     2,     3,     2,     3,     3,     1,     1,     2,
       2,     3,     2,     2,     3,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     3,     2,
       2,     1,     2,     2,     2,     1,     2,     0,     3,     0,
       1,     0,     2,     0,     4,     0,     4,     0,     1,     3,
       1,     3,     3,     3,     3,     6,     3
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
#line 2413 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2421 "parser.cpp"
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
#line 2435 "parser.cpp"
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
#line 2449 "parser.cpp"
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
#line 2463 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 310 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2482 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2500 "parser.cpp"
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
#line 2514 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 274 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2545 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2565 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2575 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2585 "parser.cpp"
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
#line 2599 "parser.cpp"
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
#line 2613 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 344 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2623 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2639 "parser.cpp"
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
#line 2653 "parser.cpp"
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
#line 2667 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2684 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2692 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2700 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2708 "parser.cpp"
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
#line 2722 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2731 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2749 "parser.cpp"
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
#line 2762 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2771 "parser.cpp"
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
#line 2785 "parser.cpp"
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
#line 2799 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2809 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2818 "parser.cpp"
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
#line 2832 "parser.cpp"
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
#line 2846 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2854 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2862 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2870 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2878 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2886 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2894 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2902 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2910 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2918 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2926 "parser.cpp"
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
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2980 "parser.cpp"
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
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3009 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3017 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3025 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3033 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3041 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3049 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3057 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3065 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3073 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3081 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3089 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3097 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3105 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 371 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3113 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 375 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3121 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3129 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3137 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3145 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3153 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3161 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3169 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3177 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3188 "parser.cpp"
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
#line 3202 "parser.cpp"
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
#line 3216 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3227 "parser.cpp"
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

#line 3335 "parser.cpp"

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
#line 3550 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 507 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3561 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 513 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3572 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 520 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3578 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 521 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3584 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 522 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3590 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 523 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3596 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 524 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3602 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 525 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3608 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 526 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3614 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3620 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 528 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3626 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 529 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3632 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 530 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3638 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3644 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3650 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3656 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3662 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 536 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3668 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3674 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3680 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3686 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3692 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3698 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3704 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3710 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3716 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3722 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3728 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3734 "parser.cpp"
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
#line 3756 "parser.cpp"
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
#line 3776 "parser.cpp"
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
#line 3794 "parser.cpp"
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
#line 3827 "parser.cpp"
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
#line 3847 "parser.cpp"
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
#line 3883 "parser.cpp"
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
#line 3905 "parser.cpp"
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
#line 3926 "parser.cpp"
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
#line 3959 "parser.cpp"
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
#line 3994 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 775 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4003 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 779 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4012 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 784 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4021 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 788 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4030 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 794 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4038 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 797 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4046 "parser.cpp"
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
#line 4102 "parser.cpp"
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
#line 4144 "parser.cpp"
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
#line 4201 "parser.cpp"
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
#line 4244 "parser.cpp"
    break;

  case 52: /* column_type: BOOLEAN  */
#line 984 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4250 "parser.cpp"
    break;

  case 53: /* column_type: TINYINT  */
#line 985 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4256 "parser.cpp"
    break;

  case 54: /* column_type: SMALLINT  */
#line 986 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4262 "parser.cpp"
    break;

  case 55: /* column_type: INTEGER  */
#line 987 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4268 "parser.cpp"
    break;

  case 56: /* column_type: INT  */
#line 988 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4274 "parser.cpp"
    break;

  case 57: /* column_type: BIGINT  */
#line 989 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4280 "parser.cpp"
    break;

  case 58: /* column_type: HUGEINT  */
#line 990 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4286 "parser.cpp"
    break;

  case 59: /* column_type: FLOAT  */
#line 991 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4292 "parser.cpp"
    break;

  case 60: /* column_type: REAL  */
#line 992 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4298 "parser.cpp"
    break;

  case 61: /* column_type: DOUBLE  */
#line 993 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4304 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT16  */
#line 994 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4310 "parser.cpp"
    break;

  case 63: /* column_type: BFLOAT16  */
#line 995 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4316 "parser.cpp"
    break;

  case 64: /* column_type: DATE  */
#line 996 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4322 "parser.cpp"
    break;

  case 65: /* column_type: TIME  */
#line 997 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4328 "parser.cpp"
    break;

  case 66: /* column_type: DATETIME  */
#line 998 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4334 "parser.cpp"
    break;

  case 67: /* column_type: TIMESTAMP  */
#line 999 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4340 "parser.cpp"
    break;

  case 68: /* column_type: UUID  */
#line 1000 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4346 "parser.cpp"
    break;

  case 69: /* column_type: POINT  */
#line 1001 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4352 "parser.cpp"
    break;

  case 70: /* column_type: LINE  */
#line 1002 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4358 "parser.cpp"
    break;

  case 71: /* column_type: LSEG  */
#line 1003 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4364 "parser.cpp"
    break;

  case 72: /* column_type: BOX  */
#line 1004 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4370 "parser.cpp"
    break;

  case 73: /* column_type: CIRCLE  */
#line 1007 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4376 "parser.cpp"
    break;

  case 74: /* column_type: VARCHAR  */
#line 1009 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4382 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4388 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 1011 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4394 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL  */
#line 1012 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4400 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4406 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4412 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4418 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4424 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4430 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4436 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4442 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4448 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4454 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4460 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4466 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4472 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4478 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4484 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4490 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4496 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4502 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4508 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4514 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4520 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4526 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4532 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4538 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4544 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4550 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4556 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4562 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4568 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4574 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4580 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4586 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4592 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4598 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4604 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4610 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4616 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4622 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4628 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4634 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1054 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4640 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1055 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4646 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1056 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4652 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1057 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4658 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1058 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4664 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1059 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4670 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1060 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4676 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1061 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4682 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1062 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4688 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1063 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4694 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1064 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4700 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1065 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4706 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1066 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4712 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1067 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4718 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1068 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4724 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1069 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4730 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1070 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4736 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1071 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4742 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1072 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4748 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1073 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4754 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1074 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4760 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1075 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4766 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1076 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4772 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1077 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4778 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1078 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4784 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1079 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4790 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1080 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4796 "parser.cpp"
    break;

  case 144: /* column_constraints: column_constraint  */
#line 1099 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4805 "parser.cpp"
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
#line 4819 "parser.cpp"
    break;

  case 146: /* column_constraint: PRIMARY KEY  */
#line 1113 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4827 "parser.cpp"
    break;

  case 147: /* column_constraint: UNIQUE  */
#line 1116 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4835 "parser.cpp"
    break;

  case 148: /* column_constraint: NULLABLE  */
#line 1119 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4843 "parser.cpp"
    break;

  case 149: /* column_constraint: NOT NULLABLE  */
#line 1122 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4851 "parser.cpp"
    break;

  case 150: /* default_expr: DEFAULT constant_expr  */
#line 1126 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4859 "parser.cpp"
    break;

  case 151: /* default_expr: %empty  */
#line 1129 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4867 "parser.cpp"
    break;

  case 152: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1134 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4877 "parser.cpp"
    break;

  case 153: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1139 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4887 "parser.cpp"
    break;

  case 154: /* identifier_array: IDENTIFIER  */
#line 1146 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4898 "parser.cpp"
    break;

  case 155: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1152 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4909 "parser.cpp"
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
#line 4926 "parser.cpp"
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
#line 4969 "parser.cpp"
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
#line 4989 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1232 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4997 "parser.cpp"
    break;

  case 160: /* optional_identifier_array: %empty  */
#line 1235 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5005 "parser.cpp"
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
#line 5023 "parser.cpp"
    break;

  case 162: /* explain_statement: EXPLAIN explainable_statement  */
#line 1254 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5033 "parser.cpp"
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
#line 5050 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr  */
#line 1276 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5059 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1280 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5068 "parser.cpp"
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
#line 5080 "parser.cpp"
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
#line 5096 "parser.cpp"
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
#line 5114 "parser.cpp"
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
#line 5132 "parser.cpp"
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
#line 5150 "parser.cpp"
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
#line 5173 "parser.cpp"
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
#line 5231 "parser.cpp"
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
#line 5291 "parser.cpp"
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
#line 5343 "parser.cpp"
    break;

  case 175: /* select_statement: select_without_paren  */
#line 1538 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5351 "parser.cpp"
    break;

  case 176: /* select_statement: select_with_paren  */
#line 1541 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5359 "parser.cpp"
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
#line 5373 "parser.cpp"
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
#line 5387 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_without_paren ')'  */
#line 1563 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5395 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_with_paren ')'  */
#line 1566 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5403 "parser.cpp"
    break;

  case 181: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1570 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5412 "parser.cpp"
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
#line 5443 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1602 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5451 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1605 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5459 "parser.cpp"
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
#line 5481 "parser.cpp"
    break;

  case 186: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1628 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5489 "parser.cpp"
    break;

  case 187: /* order_by_clause: %empty  */
#line 1631 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5497 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr  */
#line 1635 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5506 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1639 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5515 "parser.cpp"
    break;

  case 190: /* order_by_expr: expr order_by_type  */
#line 1644 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5525 "parser.cpp"
    break;

  case 191: /* order_by_type: ASC  */
#line 1650 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5533 "parser.cpp"
    break;

  case 192: /* order_by_type: DESC  */
#line 1653 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5541 "parser.cpp"
    break;

  case 193: /* order_by_type: %empty  */
#line 1656 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5549 "parser.cpp"
    break;

  case 194: /* limit_expr: LIMIT expr  */
#line 1660 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5557 "parser.cpp"
    break;

  case 195: /* limit_expr: %empty  */
#line 1664 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5563 "parser.cpp"
    break;

  case 196: /* offset_expr: OFFSET expr  */
#line 1666 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5571 "parser.cpp"
    break;

  case 197: /* offset_expr: %empty  */
#line 1670 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5577 "parser.cpp"
    break;

  case 198: /* distinct: DISTINCT  */
#line 1672 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5585 "parser.cpp"
    break;

  case 199: /* distinct: %empty  */
#line 1675 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5593 "parser.cpp"
    break;

  case 200: /* unnest_clause: UNNEST expr_array  */
#line 1679 "parser.y"
                                 {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5601 "parser.cpp"
    break;

  case 201: /* unnest_clause: %empty  */
#line 1682 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5609 "parser.cpp"
    break;

  case 202: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1686 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5617 "parser.cpp"
    break;

  case 203: /* highlight_clause: %empty  */
#line 1689 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5625 "parser.cpp"
    break;

  case 204: /* from_clause: FROM table_reference  */
#line 1693 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5633 "parser.cpp"
    break;

  case 205: /* from_clause: %empty  */
#line 1696 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5641 "parser.cpp"
    break;

  case 206: /* search_clause: SEARCH sub_search_array  */
#line 1700 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5651 "parser.cpp"
    break;

  case 207: /* search_clause: %empty  */
#line 1705 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5659 "parser.cpp"
    break;

  case 208: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1709 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5667 "parser.cpp"
    break;

  case 209: /* optional_search_filter_expr: %empty  */
#line 1712 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5675 "parser.cpp"
    break;

  case 210: /* where_clause: WHERE expr  */
#line 1716 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5683 "parser.cpp"
    break;

  case 211: /* where_clause: %empty  */
#line 1719 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5691 "parser.cpp"
    break;

  case 212: /* having_clause: HAVING expr  */
#line 1723 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5699 "parser.cpp"
    break;

  case 213: /* having_clause: %empty  */
#line 1726 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5707 "parser.cpp"
    break;

  case 214: /* group_by_clause: GROUP BY expr_array  */
#line 1730 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5715 "parser.cpp"
    break;

  case 215: /* group_by_clause: %empty  */
#line 1733 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5723 "parser.cpp"
    break;

  case 216: /* set_operator: UNION  */
#line 1737 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5731 "parser.cpp"
    break;

  case 217: /* set_operator: UNION ALL  */
#line 1740 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5739 "parser.cpp"
    break;

  case 218: /* set_operator: INTERSECT  */
#line 1743 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5747 "parser.cpp"
    break;

  case 219: /* set_operator: EXCEPT  */
#line 1746 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5755 "parser.cpp"
    break;

  case 220: /* table_reference: table_reference_unit  */
#line 1754 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5763 "parser.cpp"
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
#line 5781 "parser.cpp"
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
#line 5799 "parser.cpp"
    break;

  case 225: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1788 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5810 "parser.cpp"
    break;

  case 226: /* table_name: IDENTIFIER  */
#line 1797 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5820 "parser.cpp"
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
#line 5832 "parser.cpp"
    break;

  case 228: /* table_alias: AS IDENTIFIER  */
#line 1811 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5842 "parser.cpp"
    break;

  case 229: /* table_alias: IDENTIFIER  */
#line 1816 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5852 "parser.cpp"
    break;

  case 230: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1821 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5863 "parser.cpp"
    break;

  case 231: /* table_alias: %empty  */
#line 1827 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5871 "parser.cpp"
    break;

  case 232: /* with_clause: WITH with_expr_list  */
#line 1834 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5879 "parser.cpp"
    break;

  case 233: /* with_clause: %empty  */
#line 1837 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5887 "parser.cpp"
    break;

  case 234: /* with_expr_list: with_expr  */
#line 1841 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5896 "parser.cpp"
    break;

  case 235: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1844 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5905 "parser.cpp"
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
#line 5917 "parser.cpp"
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
#line 5929 "parser.cpp"
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
#line 5942 "parser.cpp"
    break;

  case 239: /* join_type: INNER  */
#line 1882 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5950 "parser.cpp"
    break;

  case 240: /* join_type: LEFT  */
#line 1885 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5958 "parser.cpp"
    break;

  case 241: /* join_type: RIGHT  */
#line 1888 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5966 "parser.cpp"
    break;

  case 242: /* join_type: OUTER  */
#line 1891 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5974 "parser.cpp"
    break;

  case 243: /* join_type: FULL  */
#line 1894 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5982 "parser.cpp"
    break;

  case 244: /* join_type: CROSS  */
#line 1897 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5990 "parser.cpp"
    break;

  case 245: /* join_type: %empty  */
#line 1900 "parser.y"
                {
}
#line 5997 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW DATABASES  */
#line 1906 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6006 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLES  */
#line 1910 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6015 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW VIEWS  */
#line 1914 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6024 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW CONFIGS  */
#line 1918 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6033 "parser.cpp"
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
#line 6045 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW PROFILES  */
#line 1929 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6054 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW BUFFER  */
#line 1933 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6063 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW MEMINDEX  */
#line 1937 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6072 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW QUERIES  */
#line 1941 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6081 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1945 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6091 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TRANSACTIONS  */
#line 1950 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6100 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1954 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6110 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW SESSION VARIABLES  */
#line 1959 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6119 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1963 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6128 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1967 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6139 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1973 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6150 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1979 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6161 "parser.cpp"
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
#line 6177 "parser.cpp"
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
#line 6193 "parser.cpp"
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
#line 6209 "parser.cpp"
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
#line 6226 "parser.cpp"
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
#line 6243 "parser.cpp"
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
#line 6261 "parser.cpp"
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
#line 6280 "parser.cpp"
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
#line 6296 "parser.cpp"
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
#line 6315 "parser.cpp"
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
#line 6336 "parser.cpp"
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
#line 6358 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW LOGS  */
#line 2127 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6367 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW DELTA LOGS  */
#line 2131 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6376 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW CATALOGS  */
#line 2135 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6385 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2139 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6394 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2143 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6403 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2147 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6414 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW MEMORY  */
#line 2153 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6423 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2157 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6432 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2161 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6441 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2165 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6452 "parser.cpp"
    break;

  case 284: /* flush_statement: FLUSH DATA  */
#line 2175 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6461 "parser.cpp"
    break;

  case 285: /* flush_statement: FLUSH LOG  */
#line 2179 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6470 "parser.cpp"
    break;

  case 286: /* flush_statement: FLUSH BUFFER  */
#line 2183 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6479 "parser.cpp"
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
#line 6494 "parser.cpp"
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
#line 6518 "parser.cpp"
    break;

  case 289: /* command_statement: USE IDENTIFIER  */
#line 2225 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6529 "parser.cpp"
    break;

  case 290: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2231 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6539 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2236 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6550 "parser.cpp"
    break;

  case 292: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2242 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6561 "parser.cpp"
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
#line 6574 "parser.cpp"
    break;

  case 294: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2256 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6585 "parser.cpp"
    break;

  case 295: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2262 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6596 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2268 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6607 "parser.cpp"
    break;

  case 297: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2274 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6618 "parser.cpp"
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
#line 6631 "parser.cpp"
    break;

  case 299: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2288 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6642 "parser.cpp"
    break;

  case 300: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2294 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6653 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2300 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6664 "parser.cpp"
    break;

  case 302: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2306 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6675 "parser.cpp"
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
#line 6688 "parser.cpp"
    break;

  case 304: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2320 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6699 "parser.cpp"
    break;

  case 305: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2326 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6710 "parser.cpp"
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
#line 6724 "parser.cpp"
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
#line 6738 "parser.cpp"
    break;

  case 308: /* compact_statement: COMPACT TABLE table_name  */
#line 2351 "parser.y"
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
#line 6755 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2364 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6764 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2368 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6774 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2373 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6785 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2379 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6797 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2386 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6810 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2394 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6824 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2403 "parser.y"
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
#line 6839 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2413 "parser.y"
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
#line 6854 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2423 "parser.y"
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
#line 6870 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2434 "parser.y"
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
#line 6886 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2445 "parser.y"
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
#line 6903 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2457 "parser.y"
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
#line 6920 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2469 "parser.y"
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
#line 6935 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2479 "parser.y"
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
#line 6951 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2490 "parser.y"
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
#line 6968 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2502 "parser.y"
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
#line 6986 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW LOGS  */
#line 2515 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6995 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2519 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7005 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2524 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7015 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2529 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7026 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2535 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7035 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2539 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7044 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2543 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7055 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2549 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7064 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2553 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7073 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW SNAPSHOT STRING  */
#line 2557 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7084 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2563 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7095 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2569 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7108 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2577 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7119 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW NODES  */
#line 2583 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7128 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2587 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7139 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW NODE  */
#line 2593 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7148 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2597 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7159 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SET ADMIN  */
#line 2603 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7169 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SET STANDALONE  */
#line 2608 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7179 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2613 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7191 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2620 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7205 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2629 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7219 "parser.cpp"
    break;

  case 347: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2639 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7233 "parser.cpp"
    break;

  case 348: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2648 "parser.y"
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
#line 7250 "parser.cpp"
    break;

  case 349: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2660 "parser.y"
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
#line 7266 "parser.cpp"
    break;

  case 350: /* expr_array: expr_alias  */
#line 2676 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7275 "parser.cpp"
    break;

  case 351: /* expr_array: expr_array ',' expr_alias  */
#line 2680 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7284 "parser.cpp"
    break;

  case 352: /* insert_row_list: '(' expr_array ')'  */
#line 2685 "parser.y"
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
#line 7299 "parser.cpp"
    break;

  case 353: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2695 "parser.y"
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
#line 7314 "parser.cpp"
    break;

  case 354: /* expr_alias: expr AS IDENTIFIER  */
#line 2717 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7325 "parser.cpp"
    break;

  case 355: /* expr_alias: expr  */
#line 2723 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7333 "parser.cpp"
    break;

  case 361: /* operand: '(' expr ')'  */
#line 2733 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7341 "parser.cpp"
    break;

  case 362: /* operand: '(' select_without_paren ')'  */
#line 2736 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7352 "parser.cpp"
    break;

  case 363: /* operand: constant_expr  */
#line 2742 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7360 "parser.cpp"
    break;

  case 374: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2758 "parser.y"
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
#line 7380 "parser.cpp"
    break;

  case 375: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2774 "parser.y"
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
#line 7401 "parser.cpp"
    break;

  case 376: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2791 "parser.y"
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
#line 7422 "parser.cpp"
    break;

  case 377: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2809 "parser.y"
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
#line 7472 "parser.cpp"
    break;

  case 378: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2855 "parser.y"
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
#line 7515 "parser.cpp"
    break;

  case 379: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2894 "parser.y"
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
#line 7562 "parser.cpp"
    break;

  case 380: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2937 "parser.y"
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
#line 7610 "parser.cpp"
    break;

  case 381: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2984 "parser.y"
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
#line 7638 "parser.cpp"
    break;

  case 382: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3008 "parser.y"
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
#line 7665 "parser.cpp"
    break;

  case 383: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3031 "parser.y"
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
#line 7690 "parser.cpp"
    break;

  case 384: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3052 "parser.y"
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
#line 7715 "parser.cpp"
    break;

  case 385: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3073 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7729 "parser.cpp"
    break;

  case 386: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3082 "parser.y"
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
#line 7745 "parser.cpp"
    break;

  case 387: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3093 "parser.y"
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
#line 7761 "parser.cpp"
    break;

  case 388: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3104 "parser.y"
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
#line 7779 "parser.cpp"
    break;

  case 389: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3118 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7791 "parser.cpp"
    break;

  case 390: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3125 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7805 "parser.cpp"
    break;

  case 391: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3135 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7816 "parser.cpp"
    break;

  case 392: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3141 "parser.y"
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
#line 7832 "parser.cpp"
    break;

  case 393: /* sub_search: match_vector_expr  */
#line 3153 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7840 "parser.cpp"
    break;

  case 394: /* sub_search: match_text_expr  */
#line 3156 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7848 "parser.cpp"
    break;

  case 395: /* sub_search: match_tensor_expr  */
#line 3159 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7856 "parser.cpp"
    break;

  case 396: /* sub_search: match_sparse_expr  */
#line 3162 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7864 "parser.cpp"
    break;

  case 397: /* sub_search: query_expr  */
#line 3165 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7872 "parser.cpp"
    break;

  case 398: /* sub_search: fusion_expr  */
#line 3168 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7880 "parser.cpp"
    break;

  case 399: /* sub_search_array: sub_search  */
#line 3172 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7889 "parser.cpp"
    break;

  case 400: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3176 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7898 "parser.cpp"
    break;

  case 401: /* function_expr: IDENTIFIER '(' ')'  */
#line 3181 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7911 "parser.cpp"
    break;

  case 402: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3189 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7924 "parser.cpp"
    break;

  case 403: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3197 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7938 "parser.cpp"
    break;

  case 404: /* function_expr: operand IS NOT NULLABLE  */
#line 3206 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7950 "parser.cpp"
    break;

  case 405: /* function_expr: operand IS NULLABLE  */
#line 3213 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7962 "parser.cpp"
    break;

  case 406: /* function_expr: NOT operand  */
#line 3220 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7974 "parser.cpp"
    break;

  case 407: /* function_expr: '-' operand  */
#line 3227 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7986 "parser.cpp"
    break;

  case 408: /* function_expr: '+' operand  */
#line 3234 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7998 "parser.cpp"
    break;

  case 409: /* function_expr: operand '-' operand  */
#line 3241 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8011 "parser.cpp"
    break;

  case 410: /* function_expr: operand '+' operand  */
#line 3249 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8024 "parser.cpp"
    break;

  case 411: /* function_expr: operand '*' operand  */
#line 3257 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8037 "parser.cpp"
    break;

  case 412: /* function_expr: operand '/' operand  */
#line 3265 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8050 "parser.cpp"
    break;

  case 413: /* function_expr: operand '%' operand  */
#line 3273 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8063 "parser.cpp"
    break;

  case 414: /* function_expr: operand '=' operand  */
#line 3281 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8076 "parser.cpp"
    break;

  case 415: /* function_expr: operand EQUAL operand  */
#line 3289 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8089 "parser.cpp"
    break;

  case 416: /* function_expr: operand NOT_EQ operand  */
#line 3297 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8102 "parser.cpp"
    break;

  case 417: /* function_expr: operand '<' operand  */
#line 3305 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8115 "parser.cpp"
    break;

  case 418: /* function_expr: operand '>' operand  */
#line 3313 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8128 "parser.cpp"
    break;

  case 419: /* function_expr: operand LESS_EQ operand  */
#line 3321 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8141 "parser.cpp"
    break;

  case 420: /* function_expr: operand GREATER_EQ operand  */
#line 3329 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8154 "parser.cpp"
    break;

  case 421: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3337 "parser.y"
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
#line 8189 "parser.cpp"
    break;

  case 422: /* function_expr: operand LIKE operand  */
#line 3367 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8202 "parser.cpp"
    break;

  case 423: /* function_expr: operand NOT LIKE operand  */
#line 3375 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8215 "parser.cpp"
    break;

  case 424: /* conjunction_expr: expr AND expr  */
#line 3384 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8228 "parser.cpp"
    break;

  case 425: /* conjunction_expr: expr OR expr  */
#line 3392 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8241 "parser.cpp"
    break;

  case 426: /* between_expr: operand BETWEEN operand AND operand  */
#line 3401 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8253 "parser.cpp"
    break;

  case 427: /* in_expr: operand IN '(' expr_array ')'  */
#line 3409 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8264 "parser.cpp"
    break;

  case 428: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3415 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8275 "parser.cpp"
    break;

  case 429: /* case_expr: CASE expr case_check_array END  */
#line 3422 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8286 "parser.cpp"
    break;

  case 430: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3428 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8298 "parser.cpp"
    break;

  case 431: /* case_expr: CASE case_check_array END  */
#line 3435 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8308 "parser.cpp"
    break;

  case 432: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3440 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8319 "parser.cpp"
    break;

  case 433: /* case_check_array: WHEN expr THEN expr  */
#line 3447 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8331 "parser.cpp"
    break;

  case 434: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3454 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8343 "parser.cpp"
    break;

  case 435: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3462 "parser.y"
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
#line 8374 "parser.cpp"
    break;

  case 436: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3489 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8385 "parser.cpp"
    break;

  case 437: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3495 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8396 "parser.cpp"
    break;

  case 438: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3501 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8408 "parser.cpp"
    break;

  case 439: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3508 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8420 "parser.cpp"
    break;

  case 440: /* column_expr: IDENTIFIER  */
#line 3516 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8432 "parser.cpp"
    break;

  case 441: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3523 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8444 "parser.cpp"
    break;

  case 442: /* column_expr: '*'  */
#line 3530 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8454 "parser.cpp"
    break;

  case 443: /* column_expr: column_expr '.' '*'  */
#line 3535 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8468 "parser.cpp"
    break;

  case 444: /* constant_expr: STRING  */
#line 3545 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8478 "parser.cpp"
    break;

  case 445: /* constant_expr: TRUE  */
#line 3550 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8488 "parser.cpp"
    break;

  case 446: /* constant_expr: FALSE  */
#line 3555 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8498 "parser.cpp"
    break;

  case 447: /* constant_expr: DOUBLE_VALUE  */
#line 3560 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8508 "parser.cpp"
    break;

  case 448: /* constant_expr: LONG_VALUE  */
#line 3565 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8518 "parser.cpp"
    break;

  case 449: /* constant_expr: DATE STRING  */
#line 3570 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8528 "parser.cpp"
    break;

  case 450: /* constant_expr: TIME STRING  */
#line 3575 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8538 "parser.cpp"
    break;

  case 451: /* constant_expr: DATETIME STRING  */
#line 3580 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8548 "parser.cpp"
    break;

  case 452: /* constant_expr: TIMESTAMP STRING  */
#line 3585 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8558 "parser.cpp"
    break;

  case 453: /* constant_expr: INTERVAL interval_expr  */
#line 3590 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8566 "parser.cpp"
    break;

  case 454: /* constant_expr: interval_expr  */
#line 3593 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8574 "parser.cpp"
    break;

  case 455: /* constant_expr: common_array_expr  */
#line 3596 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8582 "parser.cpp"
    break;

  case 456: /* common_array_expr: array_expr  */
#line 3600 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8590 "parser.cpp"
    break;

  case 457: /* common_array_expr: subarray_array_expr  */
#line 3603 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8598 "parser.cpp"
    break;

  case 458: /* common_array_expr: sparse_array_expr  */
#line 3606 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8606 "parser.cpp"
    break;

  case 459: /* common_array_expr: empty_array_expr  */
#line 3609 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8614 "parser.cpp"
    break;

  case 460: /* common_sparse_array_expr: sparse_array_expr  */
#line 3613 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8622 "parser.cpp"
    break;

  case 461: /* common_sparse_array_expr: array_expr  */
#line 3616 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8630 "parser.cpp"
    break;

  case 462: /* common_sparse_array_expr: empty_array_expr  */
#line 3619 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8638 "parser.cpp"
    break;

  case 463: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3623 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8646 "parser.cpp"
    break;

  case 464: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3627 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8656 "parser.cpp"
    break;

  case 465: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3632 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8665 "parser.cpp"
    break;

  case 466: /* sparse_array_expr: long_sparse_array_expr  */
#line 3637 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8673 "parser.cpp"
    break;

  case 467: /* sparse_array_expr: double_sparse_array_expr  */
#line 3640 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8681 "parser.cpp"
    break;

  case 468: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3644 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8689 "parser.cpp"
    break;

  case 469: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3648 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8701 "parser.cpp"
    break;

  case 470: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3655 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8712 "parser.cpp"
    break;

  case 471: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3662 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8720 "parser.cpp"
    break;

  case 472: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3666 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8732 "parser.cpp"
    break;

  case 473: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3673 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8743 "parser.cpp"
    break;

  case 474: /* empty_array_expr: '[' ']'  */
#line 3680 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8751 "parser.cpp"
    break;

  case 475: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3684 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8759 "parser.cpp"
    break;

  case 476: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3688 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8767 "parser.cpp"
    break;

  case 477: /* array_expr: long_array_expr  */
#line 3692 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8775 "parser.cpp"
    break;

  case 478: /* array_expr: double_array_expr  */
#line 3695 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8783 "parser.cpp"
    break;

  case 479: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3699 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8791 "parser.cpp"
    break;

  case 480: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3703 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8801 "parser.cpp"
    break;

  case 481: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3708 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8810 "parser.cpp"
    break;

  case 482: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3713 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8818 "parser.cpp"
    break;

  case 483: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3717 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8828 "parser.cpp"
    break;

  case 484: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3722 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8837 "parser.cpp"
    break;

  case 485: /* interval_expr: LONG_VALUE SECONDS  */
#line 3727 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8848 "parser.cpp"
    break;

  case 486: /* interval_expr: LONG_VALUE SECOND  */
#line 3733 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8859 "parser.cpp"
    break;

  case 487: /* interval_expr: LONG_VALUE MINUTES  */
#line 3739 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8870 "parser.cpp"
    break;

  case 488: /* interval_expr: LONG_VALUE MINUTE  */
#line 3745 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8881 "parser.cpp"
    break;

  case 489: /* interval_expr: LONG_VALUE HOURS  */
#line 3751 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8892 "parser.cpp"
    break;

  case 490: /* interval_expr: LONG_VALUE HOUR  */
#line 3757 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8903 "parser.cpp"
    break;

  case 491: /* interval_expr: LONG_VALUE DAYS  */
#line 3763 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8914 "parser.cpp"
    break;

  case 492: /* interval_expr: LONG_VALUE DAY  */
#line 3769 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8925 "parser.cpp"
    break;

  case 493: /* interval_expr: LONG_VALUE MONTHS  */
#line 3775 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8936 "parser.cpp"
    break;

  case 494: /* interval_expr: LONG_VALUE MONTH  */
#line 3781 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8947 "parser.cpp"
    break;

  case 495: /* interval_expr: LONG_VALUE YEARS  */
#line 3787 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8958 "parser.cpp"
    break;

  case 496: /* interval_expr: LONG_VALUE YEAR  */
#line 3793 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8969 "parser.cpp"
    break;

  case 497: /* copy_option_list: copy_option  */
#line 3804 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8978 "parser.cpp"
    break;

  case 498: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3808 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8987 "parser.cpp"
    break;

  case 499: /* copy_option: FORMAT IDENTIFIER  */
#line 3813 "parser.y"
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
#line 9023 "parser.cpp"
    break;

  case 500: /* copy_option: DELIMITER STRING  */
#line 3844 "parser.y"
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
#line 9038 "parser.cpp"
    break;

  case 501: /* copy_option: HEADER  */
#line 3854 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9048 "parser.cpp"
    break;

  case 502: /* copy_option: OFFSET LONG_VALUE  */
#line 3859 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9058 "parser.cpp"
    break;

  case 503: /* copy_option: LIMIT LONG_VALUE  */
#line 3864 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9068 "parser.cpp"
    break;

  case 504: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3869 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9078 "parser.cpp"
    break;

  case 505: /* file_path: STRING  */
#line 3875 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9086 "parser.cpp"
    break;

  case 506: /* if_exists: IF EXISTS  */
#line 3879 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9092 "parser.cpp"
    break;

  case 507: /* if_exists: %empty  */
#line 3880 "parser.y"
  { (yyval.bool_value) = false; }
#line 9098 "parser.cpp"
    break;

  case 508: /* if_not_exists: IF NOT EXISTS  */
#line 3882 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9104 "parser.cpp"
    break;

  case 509: /* if_not_exists: %empty  */
#line 3883 "parser.y"
  { (yyval.bool_value) = false; }
#line 9110 "parser.cpp"
    break;

  case 512: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3898 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9123 "parser.cpp"
    break;

  case 513: /* if_not_exists_info: %empty  */
#line 3906 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9131 "parser.cpp"
    break;

  case 514: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3910 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9139 "parser.cpp"
    break;

  case 515: /* with_index_param_list: %empty  */
#line 3913 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9147 "parser.cpp"
    break;

  case 516: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3917 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9155 "parser.cpp"
    break;

  case 517: /* optional_table_properties_list: %empty  */
#line 3920 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9163 "parser.cpp"
    break;

  case 518: /* index_param_list: index_param  */
#line 3924 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9172 "parser.cpp"
    break;

  case 519: /* index_param_list: index_param_list ',' index_param  */
#line 3928 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9181 "parser.cpp"
    break;

  case 520: /* index_param: IDENTIFIER  */
#line 3933 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9192 "parser.cpp"
    break;

  case 521: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3939 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9207 "parser.cpp"
    break;

  case 522: /* index_param: IDENTIFIER '=' STRING  */
#line 3949 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9222 "parser.cpp"
    break;

  case 523: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3959 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9235 "parser.cpp"
    break;

  case 524: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3967 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9248 "parser.cpp"
    break;

  case 525: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3978 "parser.y"
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
#line 9284 "parser.cpp"
    break;

  case 526: /* index_info: '(' IDENTIFIER ')'  */
#line 4009 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9295 "parser.cpp"
    break;


#line 9299 "parser.cpp"

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

#line 4016 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

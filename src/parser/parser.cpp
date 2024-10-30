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
  YYSYMBOL_COMMENT = 132,                  /* COMMENT  */
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
  YYSYMBOL_CONFIGS = 168,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 169,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 170,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 171,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 172,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 173,                    /* DELTA  */
  YYSYMBOL_LOGS = 174,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 175,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 176,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 177,                   /* SEARCH  */
  YYSYMBOL_MATCH = 178,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 179,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 180,                    /* QUERY  */
  YYSYMBOL_QUERIES = 181,                  /* QUERIES  */
  YYSYMBOL_FUSION = 182,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 183,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 184,                    /* ADMIN  */
  YYSYMBOL_LEADER = 185,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 186,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 187,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 188,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 189,               /* STANDALONE  */
  YYSYMBOL_NODES = 190,                    /* NODES  */
  YYSYMBOL_NODE = 191,                     /* NODE  */
  YYSYMBOL_REMOVE = 192,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 193,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 194,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 195,                  /* RECOVER  */
  YYSYMBOL_PERSISTENCE = 196,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 197,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 198,                  /* OBJECTS  */
  YYSYMBOL_FILES = 199,                    /* FILES  */
  YYSYMBOL_MEMORY = 200,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 201,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 202,                   /* NUMBER  */
  YYSYMBOL_203_ = 203,                     /* '='  */
  YYSYMBOL_204_ = 204,                     /* '<'  */
  YYSYMBOL_205_ = 205,                     /* '>'  */
  YYSYMBOL_206_ = 206,                     /* '+'  */
  YYSYMBOL_207_ = 207,                     /* '-'  */
  YYSYMBOL_208_ = 208,                     /* '*'  */
  YYSYMBOL_209_ = 209,                     /* '/'  */
  YYSYMBOL_210_ = 210,                     /* '%'  */
  YYSYMBOL_211_ = 211,                     /* '['  */
  YYSYMBOL_212_ = 212,                     /* ']'  */
  YYSYMBOL_213_ = 213,                     /* '('  */
  YYSYMBOL_214_ = 214,                     /* ')'  */
  YYSYMBOL_215_ = 215,                     /* '.'  */
  YYSYMBOL_216_ = 216,                     /* ';'  */
  YYSYMBOL_217_ = 217,                     /* ','  */
  YYSYMBOL_218_ = 218,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 219,                 /* $accept  */
  YYSYMBOL_input_pattern = 220,            /* input_pattern  */
  YYSYMBOL_statement_list = 221,           /* statement_list  */
  YYSYMBOL_statement = 222,                /* statement  */
  YYSYMBOL_explainable_statement = 223,    /* explainable_statement  */
  YYSYMBOL_create_statement = 224,         /* create_statement  */
  YYSYMBOL_table_element_array = 225,      /* table_element_array  */
  YYSYMBOL_column_def_array = 226,         /* column_def_array  */
  YYSYMBOL_table_element = 227,            /* table_element  */
  YYSYMBOL_table_column = 228,             /* table_column  */
  YYSYMBOL_column_type = 229,              /* column_type  */
  YYSYMBOL_column_constraints = 230,       /* column_constraints  */
  YYSYMBOL_column_constraint = 231,        /* column_constraint  */
  YYSYMBOL_default_expr = 232,             /* default_expr  */
  YYSYMBOL_table_constraint = 233,         /* table_constraint  */
  YYSYMBOL_identifier_array = 234,         /* identifier_array  */
  YYSYMBOL_delete_statement = 235,         /* delete_statement  */
  YYSYMBOL_insert_statement = 236,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 237, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 238,        /* explain_statement  */
  YYSYMBOL_update_statement = 239,         /* update_statement  */
  YYSYMBOL_update_expr_array = 240,        /* update_expr_array  */
  YYSYMBOL_update_expr = 241,              /* update_expr  */
  YYSYMBOL_drop_statement = 242,           /* drop_statement  */
  YYSYMBOL_copy_statement = 243,           /* copy_statement  */
  YYSYMBOL_select_statement = 244,         /* select_statement  */
  YYSYMBOL_select_with_paren = 245,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 246,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 247, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 248, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 249, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 250,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 251,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 252,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 253,            /* order_by_type  */
  YYSYMBOL_limit_expr = 254,               /* limit_expr  */
  YYSYMBOL_offset_expr = 255,              /* offset_expr  */
  YYSYMBOL_distinct = 256,                 /* distinct  */
  YYSYMBOL_highlight_clause = 257,         /* highlight_clause  */
  YYSYMBOL_from_clause = 258,              /* from_clause  */
  YYSYMBOL_search_clause = 259,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 260, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 261,             /* where_clause  */
  YYSYMBOL_having_clause = 262,            /* having_clause  */
  YYSYMBOL_group_by_clause = 263,          /* group_by_clause  */
  YYSYMBOL_set_operator = 264,             /* set_operator  */
  YYSYMBOL_table_reference = 265,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 266,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 267,     /* table_reference_name  */
  YYSYMBOL_table_name = 268,               /* table_name  */
  YYSYMBOL_table_alias = 269,              /* table_alias  */
  YYSYMBOL_with_clause = 270,              /* with_clause  */
  YYSYMBOL_with_expr_list = 271,           /* with_expr_list  */
  YYSYMBOL_with_expr = 272,                /* with_expr  */
  YYSYMBOL_join_clause = 273,              /* join_clause  */
  YYSYMBOL_join_type = 274,                /* join_type  */
  YYSYMBOL_show_statement = 275,           /* show_statement  */
  YYSYMBOL_flush_statement = 276,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 277,       /* optimize_statement  */
  YYSYMBOL_command_statement = 278,        /* command_statement  */
  YYSYMBOL_compact_statement = 279,        /* compact_statement  */
  YYSYMBOL_admin_statement = 280,          /* admin_statement  */
  YYSYMBOL_alter_statement = 281,          /* alter_statement  */
  YYSYMBOL_expr_array = 282,               /* expr_array  */
  YYSYMBOL_insert_row_list = 283,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 284,               /* expr_alias  */
  YYSYMBOL_expr = 285,                     /* expr  */
  YYSYMBOL_operand = 286,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 287,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 288,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 289,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 290,          /* match_text_expr  */
  YYSYMBOL_query_expr = 291,               /* query_expr  */
  YYSYMBOL_fusion_expr = 292,              /* fusion_expr  */
  YYSYMBOL_sub_search = 293,               /* sub_search  */
  YYSYMBOL_sub_search_array = 294,         /* sub_search_array  */
  YYSYMBOL_function_expr = 295,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 296,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 297,             /* between_expr  */
  YYSYMBOL_in_expr = 298,                  /* in_expr  */
  YYSYMBOL_case_expr = 299,                /* case_expr  */
  YYSYMBOL_case_check_array = 300,         /* case_check_array  */
  YYSYMBOL_cast_expr = 301,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 302,            /* subquery_expr  */
  YYSYMBOL_column_expr = 303,              /* column_expr  */
  YYSYMBOL_constant_expr = 304,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 305,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 306, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 307,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 308, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 309,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 310,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 311, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 312, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 313, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 314,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 315,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 316,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 317,               /* array_expr  */
  YYSYMBOL_long_array_expr = 318,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 319, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 320,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 321, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 322,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 323,         /* copy_option_list  */
  YYSYMBOL_copy_option = 324,              /* copy_option  */
  YYSYMBOL_file_path = 325,                /* file_path  */
  YYSYMBOL_if_exists = 326,                /* if_exists  */
  YYSYMBOL_if_not_exists = 327,            /* if_not_exists  */
  YYSYMBOL_semicolon = 328,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 329,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 330,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 331, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 332,         /* index_param_list  */
  YYSYMBOL_index_param = 333,              /* index_param  */
  YYSYMBOL_index_info = 334                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 463 "parser.cpp"

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
#define YYLAST   1433

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  219
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  520
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1169

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   457


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
       2,     2,     2,     2,     2,     2,     2,   210,     2,     2,
     213,   214,   208,   206,   217,   207,   215,   209,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   218,   216,
     204,   203,   205,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   211,     2,   212,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197,   198,   199,   200,   201,   202
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   503,   503,   507,   513,   520,   521,   522,   523,   524,
     525,   526,   527,   528,   529,   530,   531,   532,   533,   534,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   554,   571,   588,   604,   633,   648,   680,   698,
     716,   744,   774,   778,   783,   787,   793,   796,   803,   854,
     891,   943,   983,   984,   985,   986,   987,   988,   989,   990,
     991,   992,   993,   994,   995,   996,   997,   998,   999,  1000,
    1001,  1002,  1003,  1006,  1008,  1009,  1010,  1011,  1014,  1015,
    1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,
    1026,  1027,  1028,  1029,  1030,  1031,  1032,  1033,  1034,  1035,
    1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,
    1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,  1055,
    1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,
    1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,  1075,
    1076,  1077,  1078,  1079,  1098,  1102,  1112,  1115,  1118,  1121,
    1125,  1128,  1133,  1138,  1145,  1151,  1161,  1177,  1215,  1231,
    1234,  1241,  1253,  1262,  1275,  1279,  1284,  1297,  1310,  1325,
    1340,  1355,  1378,  1431,  1486,  1537,  1540,  1543,  1552,  1562,
    1565,  1569,  1574,  1601,  1604,  1609,  1626,  1629,  1633,  1637,
    1642,  1648,  1651,  1654,  1658,  1662,  1664,  1668,  1670,  1673,
    1677,  1680,  1684,  1687,  1691,  1696,  1700,  1703,  1707,  1710,
    1714,  1717,  1721,  1724,  1728,  1731,  1734,  1737,  1745,  1748,
    1763,  1763,  1765,  1779,  1788,  1793,  1802,  1807,  1812,  1818,
    1825,  1828,  1832,  1835,  1840,  1852,  1859,  1873,  1876,  1879,
    1882,  1885,  1888,  1891,  1897,  1901,  1905,  1909,  1913,  1920,
    1924,  1928,  1932,  1936,  1941,  1945,  1950,  1954,  1958,  1964,
    1970,  1976,  1987,  1998,  2009,  2021,  2033,  2046,  2060,  2071,
    2085,  2101,  2118,  2122,  2126,  2130,  2134,  2138,  2144,  2148,
    2152,  2156,  2166,  2170,  2174,  2182,  2193,  2216,  2222,  2227,
    2233,  2239,  2247,  2253,  2259,  2265,  2271,  2279,  2285,  2291,
    2297,  2303,  2311,  2317,  2323,  2332,  2342,  2355,  2359,  2364,
    2370,  2377,  2385,  2394,  2404,  2414,  2425,  2436,  2448,  2460,
    2470,  2481,  2493,  2506,  2510,  2515,  2520,  2526,  2530,  2534,
    2540,  2544,  2548,  2554,  2560,  2568,  2574,  2578,  2584,  2588,
    2594,  2599,  2604,  2611,  2620,  2630,  2639,  2651,  2667,  2671,
    2676,  2686,  2708,  2714,  2718,  2719,  2720,  2721,  2722,  2724,
    2727,  2733,  2736,  2737,  2738,  2739,  2740,  2741,  2742,  2743,
    2744,  2745,  2749,  2767,  2813,  2852,  2895,  2942,  2966,  2989,
    3010,  3031,  3040,  3052,  3059,  3069,  3075,  3087,  3090,  3093,
    3096,  3099,  3102,  3106,  3110,  3115,  3123,  3131,  3140,  3147,
    3154,  3161,  3168,  3175,  3183,  3191,  3199,  3207,  3215,  3223,
    3231,  3239,  3247,  3255,  3263,  3271,  3301,  3309,  3318,  3326,
    3335,  3343,  3349,  3356,  3362,  3369,  3374,  3381,  3388,  3396,
    3423,  3429,  3435,  3442,  3450,  3457,  3464,  3469,  3479,  3484,
    3489,  3494,  3499,  3504,  3509,  3514,  3519,  3524,  3527,  3530,
    3534,  3537,  3540,  3543,  3547,  3550,  3553,  3557,  3561,  3566,
    3571,  3574,  3578,  3582,  3589,  3596,  3600,  3607,  3614,  3618,
    3622,  3626,  3629,  3633,  3637,  3642,  3647,  3651,  3656,  3661,
    3667,  3673,  3679,  3685,  3691,  3697,  3703,  3709,  3715,  3721,
    3727,  3738,  3742,  3747,  3778,  3788,  3793,  3798,  3803,  3809,
    3813,  3814,  3816,  3817,  3819,  3820,  3832,  3840,  3844,  3847,
    3851,  3854,  3858,  3862,  3867,  3873,  3883,  3893,  3901,  3912,
    3943
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
  "offset_expr", "distinct", "highlight_clause", "from_clause",
  "search_clause", "optional_search_filter_expr", "where_clause",
  "having_clause", "group_by_clause", "set_operator", "table_reference",
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

#define YYPACT_NINF (-698)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-508)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     704,   410,    80,   425,   140,    90,   140,     8,   125,   759,
     146,   162,   232,   277,   289,   248,   288,   316,   109,   201,
     -42,   336,   147,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,   344,  -698,  -698,   351,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,   296,   296,   296,   296,   243,   140,   315,   315,
     315,   315,   315,   165,   370,   140,   -34,   397,   438,   450,
      81,  -698,  -698,  -698,  -698,  -698,  -698,  -698,   344,  -698,
    -698,  -698,  -698,  -698,   200,   464,   140,  -698,  -698,  -698,
    -698,  -698,   467,  -698,   127,   156,  -698,   485,  -698,   260,
    -698,  -698,   489,  -698,   318,  -149,   140,   140,   140,   140,
    -698,  -698,  -698,  -698,   -45,  -698,   448,   292,  -698,   519,
     339,   366,   223,   666,   376,   572,   390,   512,   375,   381,
    -698,    54,  -698,   596,  -698,  -698,     6,   558,  -698,   560,
     556,   627,   140,   140,   140,   633,   585,   435,   577,   653,
     140,   140,   140,   660,   667,   668,   607,   670,   670,   567,
      41,    52,   192,  -698,   461,  -698,   334,  -698,  -698,   675,
    -698,   676,  -698,  -698,  -698,   677,  -698,  -698,  -698,  -698,
      46,  -698,  -698,  -698,   140,   469,   316,   670,  -698,   679,
    -698,   517,  -698,   678,  -698,  -698,   682,  -698,  -698,   681,
    -698,   685,   686,  -698,   691,   642,   693,   505,  -698,  -698,
    -698,  -698,     6,  -698,  -698,  -698,   567,   651,   632,   634,
     576,    -4,  -698,   435,  -698,   140,   706,   128,  -698,  -698,
    -698,  -698,  -698,   662,  -698,   509,   -14,  -698,   567,  -698,
    -698,   649,   654,   511,  -698,  -698,   736,   604,   523,   527,
     412,   726,   728,   737,   738,  -698,  -698,   740,   530,   217,
     531,   537,   688,   688,  -698,    13,   451,   143,  -698,   103,
     795,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,   536,  -698,  -698,  -698,   100,  -698,
    -698,   178,  -698,   202,  -698,  -698,  -698,   209,  -698,   212,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,   749,   747,  -698,
    -698,  -698,  -698,  -698,  -698,   707,   708,   680,   684,   351,
    -698,  -698,  -698,   753,    40,  -698,   752,  -698,  -698,   687,
     236,  -698,   760,   551,   552,    19,   567,   567,   700,  -698,
     763,   -42,    78,   715,   563,  -698,   187,   564,  -698,   140,
     567,   668,  -698,   449,   566,   570,   297,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,   688,
     574,   826,   714,   567,   567,     9,   350,  -698,  -698,  -698,
    -698,   736,  -698,   781,   583,   584,   586,   589,   794,   799,
     340,   340,  -698,   588,  -698,  -698,  -698,  -698,   590,   -67,
     731,   567,   805,   567,   567,   -36,   600,    35,   688,   688,
     688,   688,   688,   688,   688,   688,   688,   688,   688,   688,
     688,   688,    12,  -698,   598,  -698,   808,  -698,   810,  -698,
     813,  -698,   815,   775,   417,   612,   613,   824,   615,  -698,
     616,  -698,   825,  -698,   273,   828,   672,   673,  -698,  -698,
    -698,   567,   758,   622,  -698,   190,   449,   567,  -698,  -698,
     230,   959,   717,   638,   222,  -698,  -698,  -698,   -42,   842,
     720,  -698,   843,   567,   636,  -698,   449,  -698,   205,   205,
     567,  -698,   257,   714,   698,   644,   124,   142,   385,  -698,
     567,   567,   786,   567,   858,    14,   567,   646,   276,   610,
    -698,  -698,   670,  -698,  -698,  -698,   711,   652,   688,   451,
     756,  -698,   836,   836,   356,   356,   812,   836,   836,   356,
     356,   340,   340,  -698,  -698,  -698,  -698,  -698,  -698,   669,
    -698,   671,  -698,  -698,  -698,   880,   884,  -698,   706,   888,
    -698,   889,  -698,  -698,   887,  -698,  -698,   893,   896,   694,
      10,   733,   567,  -698,  -698,  -698,   449,   908,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,   701,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,   702,   703,   709,   713,   718,   722,   203,   723,
     706,   898,    78,   344,   724,   933,  -698,   283,   735,   935,
     943,   946,   949,  -698,   947,   284,  -698,   312,   313,  -698,
     743,  -698,   959,   567,  -698,   567,   -35,   155,   688,   101,
     741,  -698,   158,   133,   242,   750,  -698,   950,  -698,  -698,
     890,   451,   836,   755,   317,  -698,   688,   964,   966,   922,
     926,   341,   360,  -698,   776,   361,  -698,   972,  -698,  -698,
     -42,   764,   481,  -698,   271,  -698,   309,   607,  -698,  -698,
     974,   508,   995,  1012,  1029,  1046,  1063,   854,   861,  -698,
    -698,   225,  -698,   859,   706,   365,   778,   857,  -698,   830,
    -698,  -698,   567,  -698,  -698,  -698,  -698,  -698,  -698,   205,
    -698,  -698,  -698,   780,   449,   -24,  -698,   567,   699,   784,
     992,   598,   796,   789,   567,  -698,   783,   797,   798,   374,
    -698,  -698,   826,  1002,  1003,  -698,  -698,   888,   343,  -698,
     889,   358,    17,    10,   955,  -698,  -698,  -698,  -698,  -698,
    -698,   957,  -698,  1011,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,   806,   973,   379,   809,   811,   820,   821,   847,
     855,   862,   872,   873,   934,   876,   877,   878,   879,   894,
     895,   906,   907,   910,   911,   936,   912,   913,   923,   924,
     927,   928,   929,   930,   940,   941,   980,   944,   945,   952,
     953,   954,   956,   958,   960,   961,   962,   983,   963,   967,
     968,   969,   970,   971,   975,   976,   977,   978,   984,   979,
     981,   982,   985,   986,   987,   988,   989,   990,   991,  1006,
     993,  -698,  -698,   110,  -698,  1031,  1032,   407,  -698,   889,
    1074,  1103,   418,  -698,  -698,  -698,   449,  -698,   647,   994,
     996,   997,    15,   998,  -698,  -698,  -698,  1042,   899,   449,
    -698,   205,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  1104,  -698,   271,   481,    10,    10,   999,   309,
    1120,  1121,  -698,  1170,  1175,  1177,  1183,  1184,  1185,  1191,
    1194,  1195,  1203,  1000,  1210,  1212,  1213,  1214,  1215,  1216,
    1217,  1218,  1219,  1220,  1010,  1222,  1223,  1224,  1225,  1226,
    1227,  1228,  1229,  1230,  1231,  1021,  1233,  1234,  1235,  1236,
    1237,  1238,  1239,  1240,  1241,  1242,  1033,  1243,  1245,  1246,
    1247,  1248,  1249,  1250,  1251,  1252,  1253,  1043,  1255,  1256,
    1257,  1258,  1259,  1260,  1261,  1262,  1263,  1264,  1054,  1266,
    -698,  1269,  1270,  -698,   420,  -698,   684,  -698,  -698,  1271,
     245,  1062,  1273,  1274,  -698,   424,  1275,  -698,  -698,  1221,
     706,  -698,   567,   567,  -698,  1065,  1066,  1069,  1070,  1071,
    1072,  1073,  1075,  1076,  1077,  1282,  1078,  1079,  1080,  1081,
    1082,  1083,  1084,  1085,  1086,  1087,  1296,  1089,  1090,  1091,
    1092,  1093,  1094,  1095,  1096,  1097,  1098,  1307,  1100,  1101,
    1102,  1105,  1106,  1107,  1108,  1109,  1110,  1111,  1311,  1112,
    1113,  1114,  1115,  1116,  1117,  1118,  1119,  1122,  1123,  1312,
    1124,  1125,  1126,  1127,  1128,  1129,  1130,  1131,  1132,  1133,
    1328,  1134,  -698,  -698,  -698,  -698,  1135,   789,  -698,  1136,
    1137,  -698,   362,   567,   428,   694,   449,  -698,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  1141,  -698,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  1142,  -698,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  1143,
    -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    1144,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  1145,  -698,  -698,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  1146,  -698,  1331,  1147,  1345,    56,  1148,  1344,
    1357,  -698,  -698,  -698,   449,  -698,  -698,  -698,  -698,  -698,
    -698,  -698,  1149,  -698,  1150,   789,   684,  1358,   609,   134,
    1151,  1347,  1154,  -698,   617,  1363,  -698,   789,   684,   789,
     157,  1364,  -698,  1323,  1158,  -698,  1159,  1334,  1335,  -698,
    -698,  -698,   163,  -698,  -698,  1163,  1337,  1338,  -698,  1376,
    -698,  1167,  1168,  1378,   684,  1169,  -698,   684,  -698
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     231,     0,     0,     0,     0,     0,     0,     0,   231,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     231,     0,   505,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   176,   175,     0,     8,    14,    15,    16,    17,
      18,    19,   503,   503,   503,   503,   503,     0,   501,   501,
     501,   501,   501,   224,     0,     0,     0,     0,     0,     0,
     231,   162,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   245,   246,   244,
     250,   254,     0,   251,     0,     0,   247,     0,   249,     0,
     272,   274,     0,   252,     0,   278,     0,     0,     0,     0,
     282,   283,   284,   287,   224,   285,     0,   230,   232,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,   231,     2,   214,   216,   217,     0,   199,   181,   187,
       0,     0,     0,     0,     0,     0,     0,   160,     0,     0,
       0,     0,     0,     0,     0,     0,   209,     0,     0,     0,
       0,     0,     0,   161,     0,   260,   261,   255,   256,     0,
     257,     0,   248,   273,   253,     0,   276,   275,   279,   280,
       0,   306,   304,   305,     0,     0,     0,     0,   330,     0,
     340,     0,   341,     0,   327,   328,     0,   323,   307,     0,
     336,   338,     0,   331,     0,     0,     0,     0,   180,   179,
       4,   215,     0,   177,   178,   198,     0,     0,   195,     0,
      33,     0,    34,   160,   506,     0,     0,   231,   500,   167,
     169,   168,   170,     0,   225,     0,   209,   164,     0,   156,
     499,     0,     0,   434,   438,   441,   442,     0,     0,     0,
       0,     0,     0,     0,     0,   439,   440,     0,     0,     0,
       0,     0,     0,     0,   436,     0,   231,     0,   348,   353,
     354,   368,   366,   369,   367,   370,   371,   363,   358,   357,
     356,   364,   365,   355,   362,   361,   449,   451,     0,   452,
     460,     0,   461,     0,   453,   450,   471,     0,   472,     0,
     448,   291,   293,   292,   289,   290,   296,   298,   297,   294,
     295,   301,   303,   302,   299,   300,   281,     0,     0,   263,
     262,   268,   258,   259,   277,     0,     0,     0,   509,     0,
     233,   288,   333,     0,   324,   329,   308,   337,   332,     0,
       0,   339,     0,     0,     0,   201,     0,     0,   197,   502,
       0,   231,     0,     0,     0,   154,     0,     0,   158,     0,
       0,     0,   163,   208,     0,     0,     0,   480,   479,   482,
     481,   484,   483,   486,   485,   488,   487,   490,   489,     0,
       0,   400,   231,     0,     0,     0,     0,   443,   444,   445,
     446,     0,   447,     0,     0,     0,     0,     0,     0,     0,
     402,   401,   477,   474,   468,   458,   463,   466,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   457,     0,   462,     0,   465,     0,   473,
       0,   476,     0,   269,   264,     0,     0,     0,     0,   286,
       0,   342,     0,   325,     0,     0,     0,     0,   335,   184,
     183,     0,   203,   186,   188,   193,   194,     0,   182,    32,
      36,     0,     0,     0,     0,    42,    46,    47,   231,     0,
      40,   159,     0,     0,   157,   171,   166,   165,     0,     0,
       0,   395,     0,   231,     0,     0,     0,     0,     0,   425,
       0,     0,     0,     0,     0,     0,     0,   207,     0,     0,
     360,   359,     0,   349,   352,   418,   419,     0,     0,   231,
       0,   399,   409,   410,   413,   414,     0,   416,   408,   411,
     412,   404,   403,   405,   406,   407,   435,   437,   459,     0,
     464,     0,   467,   475,   478,     0,     0,   265,     0,     0,
     345,     0,   234,   326,     0,   309,   334,     0,     0,   200,
       0,   205,     0,   191,   192,   190,   196,     0,    52,    55,
      56,    53,    54,    57,    58,    74,    59,    61,    60,    77,
      64,    65,    66,    62,    63,    67,    68,    69,    70,    71,
      72,    73,     0,     0,     0,     0,     0,     0,   509,     0,
       0,   511,     0,    39,     0,     0,   155,     0,     0,     0,
       0,     0,     0,   495,     0,     0,   491,     0,     0,   396,
       0,   430,     0,     0,   423,     0,     0,     0,     0,     0,
       0,   434,     0,     0,     0,     0,   385,     0,   470,   469,
       0,   231,   417,     0,     0,   398,     0,     0,     0,   270,
     266,     0,     0,    44,   514,     0,   512,   310,   343,   344,
     231,   202,   218,   220,   229,   221,     0,   209,   189,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   147,
     148,   151,   144,   151,     0,     0,     0,    35,    43,   520,
      41,   350,     0,   497,   496,   494,   493,   498,   174,     0,
     172,   397,   431,     0,   427,     0,   426,     0,     0,     0,
       0,     0,     0,   207,     0,   383,     0,     0,     0,     0,
     432,   421,   420,     0,     0,   347,   346,     0,     0,   508,
       0,     0,     0,     0,     0,   238,   239,   240,   241,   237,
     242,     0,   227,     0,   222,   389,   387,   390,   388,   391,
     392,   393,   204,   213,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   149,   146,     0,   145,    49,    48,     0,   153,     0,
       0,     0,     0,   492,   429,   424,   428,   415,     0,     0,
     207,     0,     0,     0,   454,   456,   455,     0,     0,   206,
     386,     0,   433,   422,   271,   267,    45,   515,   516,   518,
     517,   513,     0,   311,   229,   219,     0,     0,   226,     0,
       0,   211,    76,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     150,     0,     0,   152,     0,    37,   509,   351,   474,     0,
       0,     0,     0,     0,   384,     0,   312,   223,   235,     0,
       0,   394,     0,     0,   185,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    51,    50,   510,   519,     0,   207,   381,     0,
     207,   173,     0,     0,     0,   212,   210,    75,    81,    82,
      79,    80,    83,    84,    85,    86,    87,     0,    78,   125,
     126,   123,   124,   127,   128,   129,   130,   131,     0,   122,
      92,    93,    90,    91,    94,    95,    96,    97,    98,     0,
      89,   103,   104,   101,   102,   105,   106,   107,   108,   109,
       0,   100,   136,   137,   134,   135,   138,   139,   140,   141,
     142,     0,   133,   114,   115,   112,   113,   116,   117,   118,
     119,   120,     0,   111,     0,     0,     0,     0,     0,     0,
       0,   314,   313,   319,   236,   228,    88,   132,    99,   110,
     143,   121,   207,   382,     0,   207,   509,   320,   315,     0,
       0,     0,     0,   380,     0,     0,   316,   207,   509,   207,
     509,     0,   321,   317,     0,   376,     0,     0,     0,   379,
     322,   318,   509,   372,   378,     0,     0,     0,   375,     0,
     374,     0,     0,     0,   509,     0,   377,   509,   373
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -698,  -698,  -698,  1265,  1324,    16,  -698,  -698,   793,  -503,
     777,  -698,   716,   719,  -698,  -530,    67,    91,  1178,  -698,
      93,  -698,  1037,   253,   256,    -7,  1370,   -17,  1088,  1192,
    -104,  -698,  -698,   841,  -698,  -698,  -698,  -698,  -698,  -698,
    -698,  -697,  -219,  -698,  -698,  -698,  -698,   674,  -191,    27,
     541,  -698,  -698,  1232,  -698,  -698,   262,   263,   281,   282,
     285,  -698,  -698,  -204,  -698,  1001,  -228,  -227,  -633,  -629,
    -627,  -626,  -624,  -622,   539,  -698,  -698,  -698,  -698,  -698,
    -698,  1024,  -698,  -698,   901,   587,  -250,  -698,  -698,  -698,
     710,  -698,  -698,  -698,  -698,   712,  1004,  1005,   -55,  -698,
    -698,  -698,  -698,  1156,  -470,   721,  -137,   431,   476,  -698,
    -698,  -584,  -698,   582,   689,  -698
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   464,   642,   465,   466,
     588,   671,   672,   815,   467,   346,    25,    26,   217,    27,
      28,   226,   227,    29,    30,    31,    32,    33,   128,   203,
     129,   208,   453,   454,   555,   338,   458,   206,   452,   551,
     657,   625,   229,   954,   861,   126,   651,   652,   653,   654,
     734,    34,   107,   108,   655,   731,    35,    36,    37,    38,
      39,    40,    41,   257,   474,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   741,   742,   267,   268,   269,   270,
     271,   376,   272,   273,   274,   275,   276,   833,   277,   278,
     279,   280,   281,   282,   283,   284,   396,   397,   285,   286,
     287,   288,   289,   290,   605,   606,   231,   139,   131,   122,
     136,   439,   677,   645,   646,   470
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     353,    68,   335,   119,   673,   395,   838,   352,   641,   607,
     371,   232,   375,    53,   127,   526,   174,   621,   392,   393,
     392,   393,   204,   735,    62,   390,   391,   736,   399,   737,
     738,    54,   739,    56,   740,    17,   643,   123,   507,   124,
     321,   147,   148,   105,   291,   125,   292,   293,   696,   168,
     341,   228,   169,    68,  -504,   296,   315,   297,   298,   825,
     675,     1,  1125,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   316,   317,   137,    63,    62,    11,    12,    13,
     442,   461,   146,    14,    15,    16,   403,   404,     1,   443,
       2,     3,     4,     5,     6,     7,   374,     9,   334,    64,
     451,    65,   294,   156,    11,    12,    13,   510,   455,   456,
      14,    15,    16,   299,   234,   235,   236,    47,   403,   404,
     508,   704,   476,   170,   171,   172,   173,    63,    60,   403,
     404,    17,     1,   941,     2,     3,     4,     5,     6,     7,
    1137,     9,   371,    53,   817,   486,   487,   501,    11,    12,
      13,    64,   482,    65,    14,    15,    16,   402,    17,   211,
     212,   213,   403,   404,   511,    55,   347,   220,   221,   222,
     144,    20,    57,    58,   528,   505,   506,    59,   612,   149,
      96,   512,   513,   514,   515,   516,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   301,    97,   302,   303,   704,
     348,   318,    17,   351,   462,    17,   463,   295,   110,   342,
     241,   242,   243,   111,   846,   112,   244,   113,   300,   202,
     527,    18,   254,   650,   255,   394,   735,   394,   613,   556,
     736,   854,   737,   738,   438,   739,   401,   740,    19,   398,
     438,   697,   344,   245,   246,   247,   703,   549,    18,  1027,
     123,   103,   124,   304,   403,   404,   403,   404,   125,   553,
     554,    66,   616,   617,    67,   619,    98,    20,   623,   597,
      69,    70,   599,   600,   732,   667,   608,   403,   404,   109,
     438,   632,  1147,   601,   602,   603,   403,   404,  1156,    71,
      72,   104,    18,    73,    20,   403,   404,   667,   158,   159,
     233,   234,   235,   236,  -507,   634,   544,   704,   403,   404,
     704,    99,   423,    66,   130,   545,    67,   424,   699,   106,
    1148,   255,    69,    70,   455,   733,  1157,   160,   161,   668,
    1105,   669,   670,  1108,   460,   384,   120,   385,    20,   386,
     387,    71,    72,   403,   404,    73,   847,   848,   849,   850,
     702,   668,  1025,   669,   670,   485,   813,   400,   305,   127,
     401,   480,   557,   121,   123,   630,   124,   130,   114,   237,
     238,   945,   125,   422,   307,   701,   475,   308,   309,   239,
     144,   240,   310,   311,   145,   694,   138,   695,   604,   115,
     425,   698,   852,   116,   853,   426,   117,   241,   242,   243,
     150,   471,  1109,   244,   472,  1110,  1111,   180,   181,   712,
    1112,  1113,   182,   154,   427,   233,   234,   235,   236,   428,
    1034,   429,   446,   447,   431,  1130,   430,   709,  1132,   432,
     245,   246,   247,   489,   163,   490,   591,   491,   743,   592,
    1144,   151,  1146,    42,    43,    44,   100,   101,   102,    45,
      46,   831,   248,   152,   233,   234,   235,   236,    48,    49,
      50,   593,   536,   537,    51,    52,   610,   155,   614,   826,
     615,   609,   491,   157,   401,   249,   839,   250,   822,   251,
     140,   141,   142,   143,   237,   238,   407,   249,   162,   250,
     626,   251,   633,   627,   239,   164,   240,   681,   688,   374,
     401,   689,   175,   252,   253,   254,  -508,  -508,   255,   176,
     256,   481,   241,   242,   243,   165,   166,   167,   244,   132,
     133,   134,   135,   237,   238,   177,   690,   691,    17,   689,
     401,   711,   178,   239,   401,   240,   724,  -243,   725,   726,
     727,   728,  1133,   729,   730,   245,   246,   247,   419,   420,
     421,   241,   242,   243,  1145,   715,  1149,   244,   472,   179,
    -508,  -508,   417,   418,   419,   420,   421,   248,  1158,   194,
     233,   234,   235,   236,   716,   719,   195,   717,   720,   818,
    1166,   196,   472,  1168,   245,   246,   247,   197,   843,   198,
     249,   401,   250,   862,   251,   199,   863,   745,   746,   747,
     748,   749,   403,   404,   750,   751,   248,   233,   234,   235,
     236,   752,   753,   754,   708,   628,   629,   201,   252,   253,
     254,   933,   205,   255,   472,   256,   207,   755,   209,   249,
     210,   250,   937,   251,  1024,   401,   214,   720,  1031,   237,
     238,   689,  1115,   722,   829,   472,   215,   836,   216,   239,
     218,   240,   392,   938,  1135,  1136,   219,   252,   253,   254,
    1141,  1142,   255,   223,   256,   948,   949,   241,   242,   243,
     224,   225,   228,   244,   230,   306,   369,   370,   312,   313,
     323,   314,   319,   322,   324,   325,   239,   326,   240,   327,
     328,   233,   234,   235,   236,   329,   330,   331,   332,   337,
     245,   246,   247,   336,   241,   242,   243,   339,   340,   345,
     244,     1,   350,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   248,   349,   356,  1036,   354,    11,    12,    13,
     377,   355,   378,    14,    15,    16,   372,   245,   246,   247,
     373,   379,   380,   383,   388,   249,   381,   250,  1035,   251,
     389,   422,   433,   434,   435,   436,   437,   441,   444,   248,
     369,   438,    74,   445,   448,   449,   450,   459,   457,   468,
     239,   484,   240,   252,   253,   254,   469,   473,   255,   478,
     256,    17,   249,   479,   250,   492,   251,   483,   241,   242,
     243,    17,    75,    76,   244,    77,   493,   494,   497,   495,
      78,    79,   496,   498,   500,  1114,   499,   502,   504,   255,
     252,   253,   254,   509,   529,   255,   531,   256,   535,   533,
     534,   245,   246,   247,   183,   538,   539,   540,   541,   407,
     542,   543,   546,   550,   184,   547,   548,   185,   186,   552,
     187,   188,   189,   248,   589,   594,   596,   408,   409,   410,
     411,   590,   595,   598,   508,   413,   190,   191,   611,   192,
     193,   618,   620,   624,   403,   631,   249,   405,   250,   406,
     251,    18,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   484,   635,   639,   637,    19,   638,
     640,   461,   644,   647,   252,   253,   254,   648,   484,   255,
     649,   256,   414,   415,   416,   417,   418,   419,   420,   421,
     656,   401,   659,   827,   660,   661,   662,    20,    80,    81,
      82,    83,   663,    84,    85,   407,   664,    86,    87,    88,
     676,   665,    89,    90,    91,   666,   674,   680,   679,    92,
      93,   683,   407,   408,   409,   410,   411,   412,   682,   684,
     685,   413,   686,   687,   706,    94,   407,   692,   700,    95,
     408,   409,   410,   411,   705,   636,   407,   707,   413,   710,
     629,   628,   713,   714,   408,   409,   410,   411,   721,   718,
     744,   723,   413,   811,  -508,  -508,   410,   411,   812,   820,
     813,   819,  -508,   821,   824,   828,   830,   840,   414,   415,
     416,   417,   418,   419,   420,   421,   837,   832,   844,   845,
     841,   856,   842,   857,   858,   414,   415,   416,   417,   418,
     419,   420,   421,   859,   860,   873,   864,   884,   865,   414,
     415,   416,   417,   418,   419,   420,   421,   866,   867,  -508,
     415,   416,   417,   418,   419,   420,   421,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
     571,   572,   573,   574,   868,   575,   576,   577,   578,   579,
     580,   895,   869,   581,   906,   917,   582,   583,   935,   870,
     584,   585,   586,   587,   756,   757,   758,   759,   760,   871,
     872,   761,   762,   874,   875,   876,   877,   928,   763,   764,
     765,   767,   768,   769,   770,   771,   936,   704,   772,   773,
     946,   878,   879,   944,   766,   774,   775,   776,   778,   779,
     780,   781,   782,   880,   881,   783,   784,   882,   883,   885,
     886,   777,   785,   786,   787,   789,   790,   791,   792,   793,
     887,   888,   794,   795,   889,   890,   891,   892,   788,   796,
     797,   798,   800,   801,   802,   803,   804,   893,   894,   805,
     806,   896,   897,   931,   932,   799,   807,   808,   809,   898,
     899,   900,   952,   901,   953,   902,   955,   903,   904,   905,
     907,   956,   810,   957,   908,   909,   910,   911,   912,   958,
     959,   960,   913,   914,   915,   916,   918,   961,   919,   920,
     962,   963,   921,   922,   923,   924,   925,   926,   927,   964,
     929,   939,   950,   940,   942,   943,   966,   965,   967,   968,
     969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
     979,   980,   981,   982,   983,   984,   985,   986,   987,   988,
     989,   990,   991,   992,   993,   994,   995,   996,   997,   999,
     998,  1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1026,  1028,  1029,  1030,  1037,
    1038,  1032,  1033,  1039,  1040,  1041,  1042,  1043,  1047,  1044,
    1045,  1046,  1048,  1049,  1050,  1051,  1052,  1053,  1054,  1055,
    1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,
    1066,  1067,  1068,  1069,  1070,  1071,  1072,  1080,  1091,  1073,
    1074,  1075,  1076,  1077,  1078,  1079,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1102,  1122,  1089,  1090,  1092,  1093,
    1094,  1095,  1096,  1097,  1098,  1099,  1100,  1101,  1103,  1124,
    1127,  1139,  1104,  1106,  1107,  1116,  1117,  1118,  1119,  1120,
    1121,  1123,  1126,  1128,  1134,  1138,  1129,  1131,  1140,  1143,
    1150,  1151,  1152,  1153,  1154,  1155,  1159,  1160,  1161,  1162,
    1163,  1165,  1164,  1167,   153,   678,   200,   814,   477,   693,
     118,   343,   816,   658,   333,   947,   622,   855,   951,   488,
     930,   934,   503,   382,     0,     0,     0,   440,   320,   851,
     823,     0,   834,     0,   835,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     530,     0,     0,   532
};

static const yytype_int16 yycheck[] =
{
     228,     8,   206,    20,   588,   255,   703,   226,   538,   479,
     237,   148,   240,     3,     8,     3,    61,     3,     5,     6,
       5,     6,   126,   656,     8,   252,   253,   656,   256,   656,
     656,     4,   656,     6,   656,    77,   539,    20,    74,    22,
     177,    75,    76,    16,     3,    28,     5,     6,    83,   198,
      54,    65,   201,    60,     0,     3,    10,     5,     6,    83,
     590,     7,     6,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    26,    27,    47,     8,    60,    23,    24,    25,
      40,     3,    55,    29,    30,    31,   153,   154,     7,    49,
       9,    10,    11,    12,    13,    14,    87,    16,   202,     8,
      81,     8,    61,    76,    23,    24,    25,    72,   336,   337,
      29,    30,    31,    61,     4,     5,     6,    37,   153,   154,
     156,    65,   350,    96,    97,    98,    99,    60,     3,   153,
     154,    77,     7,   830,     9,    10,    11,    12,    13,    14,
       6,    16,   369,     3,   674,   373,   374,   214,    23,    24,
      25,    60,   356,    60,    29,    30,    31,    54,    77,   132,
     133,   134,   153,   154,   129,    75,    38,   140,   141,   142,
     215,   213,   164,   165,   424,   403,   404,   169,    54,   213,
      34,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,     3,    34,     5,     6,    65,
     217,   174,    77,   217,   126,    77,   128,   166,     7,   213,
     100,   101,   102,    12,   717,    14,   106,    16,   166,   213,
     208,   167,   208,   213,   211,   212,   859,   212,    86,   457,
     859,   214,   859,   859,    77,   859,   217,   859,   184,   256,
      77,    86,   215,   133,   134,   135,     4,   451,   167,     4,
      20,     3,    22,    61,   153,   154,   153,   154,    28,    69,
      70,     8,   490,   491,     8,   493,    34,   213,   496,   473,
       8,     8,    67,    68,     3,    72,   480,   153,   154,   170,
      77,   508,   125,    78,    79,    80,   153,   154,   125,     8,
       8,     3,   167,     8,   213,   153,   154,    72,   171,   172,
       3,     4,     5,     6,    61,   509,    33,    65,   153,   154,
      65,    34,   212,    60,    71,    42,    60,   217,   217,     3,
     163,   211,    60,    60,   552,    54,   163,   171,   172,   126,
    1027,   128,   129,  1030,   341,   118,     0,   120,   213,   122,
     123,    60,    60,   153,   154,    60,     3,     4,     5,     6,
     217,   126,   936,   128,   129,   372,   131,   214,   166,     8,
     217,    64,   132,   216,    20,   502,    22,    71,   167,    72,
      73,   841,    28,   215,    40,   217,   349,    43,    44,    82,
     215,    84,    48,    49,    14,   613,    71,   615,   183,   188,
     212,   618,    34,   192,    36,   217,   195,   100,   101,   102,
       3,   214,    40,   106,   217,    43,    44,   184,   185,   636,
      48,    49,   189,   213,   212,     3,     4,     5,     6,   217,
     950,   212,   186,   187,   212,  1122,   217,   631,  1125,   217,
     133,   134,   135,    83,   174,    85,   214,    87,   657,   217,
    1137,     3,  1139,    33,    34,    35,   157,   158,   159,    39,
      40,   701,   155,     3,     3,     4,     5,     6,    33,    34,
      35,   468,    45,    46,    39,    40,   483,     3,    83,   697,
      85,   214,    87,     6,   217,   178,   704,   180,   682,   182,
      49,    50,    51,    52,    72,    73,   130,   178,     3,   180,
     214,   182,   509,   217,    82,     6,    84,   214,   214,    87,
     217,   217,    54,   206,   207,   208,   150,   151,   211,   217,
     213,   214,   100,   101,   102,   197,   198,   199,   106,    43,
      44,    45,    46,    72,    73,     6,   214,   214,    77,   217,
     217,   214,   193,    82,   217,    84,    55,    56,    57,    58,
      59,    60,  1126,    62,    63,   133,   134,   135,   208,   209,
     210,   100,   101,   102,  1138,   214,  1140,   106,   217,   193,
     204,   205,   206,   207,   208,   209,   210,   155,  1152,   193,
       3,     4,     5,     6,   214,   214,     4,   217,   217,   214,
    1164,   191,   217,  1167,   133,   134,   135,    75,   214,   214,
     178,   217,   180,   214,   182,   214,   217,    89,    90,    91,
      92,    93,   153,   154,    96,    97,   155,     3,     4,     5,
       6,   103,   104,   105,   631,     5,     6,    21,   206,   207,
     208,   214,    64,   211,   217,   213,    66,   119,    72,   178,
       3,   180,   214,   182,   214,   217,     3,   217,   214,    72,
      73,   217,   214,   650,   699,   217,    61,   702,   213,    82,
      73,    84,     5,     6,    45,    46,     3,   206,   207,   208,
      43,    44,   211,     3,   213,   856,   857,   100,   101,   102,
       3,     3,    65,   106,     4,   214,    72,    73,     3,     3,
     163,     4,   213,     4,     6,     3,    82,     6,    84,     4,
       4,     3,     4,     5,     6,     4,    54,     4,   193,    67,
     133,   134,   135,    52,   100,   101,   102,    73,   132,     3,
     106,     7,   203,     9,    10,    11,    12,    13,    14,    15,
      16,    17,   155,    61,   213,   953,    77,    23,    24,    25,
       4,    77,     4,    29,    30,    31,   213,   133,   134,   135,
     213,     4,     4,   213,   213,   178,     6,   180,   952,   182,
     213,   215,     3,     6,    47,    47,    76,     4,     6,   155,
      72,    77,     3,    76,     4,   214,   214,     4,    68,    54,
      82,    72,    84,   206,   207,   208,   213,   213,   211,   213,
     213,    77,   178,   213,   180,     4,   182,   213,   100,   101,
     102,    77,    33,    34,   106,    36,   213,   213,     4,   213,
      41,    42,   213,     4,   214,  1033,   218,    76,     3,   211,
     206,   207,   208,   213,     6,   211,     6,   213,    43,     6,
       5,   133,   134,   135,   158,   213,   213,     3,   213,   130,
     214,     6,     4,    75,   168,   163,   163,   171,   172,   217,
     174,   175,   176,   155,   127,     3,     3,   148,   149,   150,
     151,   213,   132,   217,   156,   156,   190,   191,   214,   193,
     194,    75,     4,   217,   153,   213,   178,    72,   180,    74,
     182,   167,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,    72,   129,     6,   218,   184,   218,
       6,     3,     3,     6,   206,   207,   208,     4,    72,   211,
       4,   213,   203,   204,   205,   206,   207,   208,   209,   210,
     177,   217,     4,   214,   213,   213,   213,   213,   159,   160,
     161,   162,   213,   164,   165,   130,   213,   168,   169,   170,
      32,   213,   173,   174,   175,   213,   213,     4,   214,   180,
     181,     6,   130,   148,   149,   150,   151,   152,   213,     6,
       4,   156,     3,     6,     4,   196,   130,   214,   217,   200,
     148,   149,   150,   151,   214,   153,   130,    77,   156,   214,
       6,     5,    50,    47,   148,   149,   150,   151,     6,   203,
       6,   217,   156,   129,   148,   149,   150,   151,   127,   132,
     131,   213,   156,   163,   214,   211,     4,   214,   203,   204,
     205,   206,   207,   208,   209,   210,   217,   211,     6,     6,
     213,    56,   214,    56,     3,   203,   204,   205,   206,   207,
     208,   209,   210,   217,    51,    91,   217,    91,   217,   203,
     204,   205,   206,   207,   208,   209,   210,   217,   217,   203,
     204,   205,   206,   207,   208,   209,   210,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   217,   106,   107,   108,   109,   110,
     111,    91,   217,   114,    91,    91,   117,   118,     4,   217,
     121,   122,   123,   124,    89,    90,    91,    92,    93,   217,
     217,    96,    97,   217,   217,   217,   217,    91,   103,   104,
     105,    89,    90,    91,    92,    93,     3,    65,    96,    97,
       6,   217,   217,   214,   119,   103,   104,   105,    89,    90,
      91,    92,    93,   217,   217,    96,    97,   217,   217,   217,
     217,   119,   103,   104,   105,    89,    90,    91,    92,    93,
     217,   217,    96,    97,   217,   217,   217,   217,   119,   103,
     104,   105,    89,    90,    91,    92,    93,   217,   217,    96,
      97,   217,   217,   132,   132,   119,   103,   104,   105,   217,
     217,   217,    52,   217,    53,   217,     6,   217,   217,   217,
     217,     6,   119,     6,   217,   217,   217,   217,   217,     6,
       6,     6,   217,   217,   217,   217,   217,     6,   217,   217,
       6,     6,   217,   217,   217,   217,   217,   217,   217,     6,
     217,   217,   213,   217,   217,   217,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   217,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     217,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     217,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   217,     6,     4,     4,     4,   214,     4,     4,   214,
     214,     6,    61,   214,   214,   214,   214,   214,     6,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,     6,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,     6,   214,   214,   214,     6,     6,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,     6,     4,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,     4,
       6,     4,   217,   217,   217,   214,   214,   214,   214,   214,
     214,   214,   214,     6,     6,   214,   217,   217,   214,     6,
       6,    48,   214,   214,    40,    40,   213,    40,    40,     3,
     213,     3,   214,   214,    60,   592,   121,   671,   351,   612,
      20,   213,   673,   552,   202,   854,   495,   723,   859,   375,
     813,   819,   401,   247,    -1,    -1,    -1,   319,   176,   720,
     689,    -1,   702,    -1,   702,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     426,    -1,    -1,   428
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    77,   167,   184,
     213,   220,   221,   222,   224,   235,   236,   238,   239,   242,
     243,   244,   245,   246,   270,   275,   276,   277,   278,   279,
     280,   281,    33,    34,    35,    39,    40,    37,    33,    34,
      35,    39,    40,     3,   268,    75,   268,   164,   165,   169,
       3,   223,   224,   235,   236,   239,   242,   243,   244,   275,
     276,   277,   278,   279,     3,    33,    34,    36,    41,    42,
     159,   160,   161,   162,   164,   165,   168,   169,   170,   173,
     174,   175,   180,   181,   196,   200,    34,    34,    34,    34,
     157,   158,   159,     3,     3,   268,     3,   271,   272,   170,
       7,    12,    14,    16,   167,   188,   192,   195,   245,   246,
       0,   216,   328,    20,    22,    28,   264,     8,   247,   249,
      71,   327,   327,   327,   327,   327,   329,   268,    71,   326,
     326,   326,   326,   326,   215,    14,   268,    75,    76,   213,
       3,     3,     3,   223,   213,     3,   268,     6,   171,   172,
     171,   172,     3,   174,     6,   197,   198,   199,   198,   201,
     268,   268,   268,   268,    61,    54,   217,     6,   193,   193,
     184,   185,   189,   158,   168,   171,   172,   174,   175,   176,
     190,   191,   193,   194,   193,     4,   191,    75,   214,   214,
     222,    21,   213,   248,   249,    64,   256,    66,   250,    72,
       3,   268,   268,   268,     3,    61,   213,   237,    73,     3,
     268,   268,   268,     3,     3,     3,   240,   241,    65,   261,
       4,   325,   325,     3,     4,     5,     6,    72,    73,    82,
      84,   100,   101,   102,   106,   133,   134,   135,   155,   178,
     180,   182,   206,   207,   208,   211,   213,   282,   284,   285,
     286,   287,   288,   289,   290,   291,   292,   295,   296,   297,
     298,   299,   301,   302,   303,   304,   305,   307,   308,   309,
     310,   311,   312,   313,   314,   317,   318,   319,   320,   321,
     322,     3,     5,     6,    61,   166,     3,     5,     6,    61,
     166,     3,     5,     6,    61,   166,   214,    40,    43,    44,
      48,    49,     3,     3,     4,    10,    26,    27,   268,   213,
     272,   325,     4,   163,     6,     3,     6,     4,     4,     4,
      54,     4,   193,   248,   249,   282,    52,    67,   254,    73,
     132,    54,   213,   237,   268,     3,   234,    38,   246,    61,
     203,   217,   261,   285,    77,    77,   213,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,    72,
      73,   286,   213,   213,    87,   285,   300,     4,     4,     4,
       4,     6,   322,   213,   118,   120,   122,   123,   213,   213,
     286,   286,     5,     6,   212,   305,   315,   316,   246,   285,
     214,   217,    54,   153,   154,    72,    74,   130,   148,   149,
     150,   151,   152,   156,   203,   204,   205,   206,   207,   208,
     209,   210,   215,   212,   217,   212,   217,   212,   217,   212,
     217,   212,   217,     3,     6,    47,    47,    76,    77,   330,
     247,     4,    40,    49,     6,    76,   186,   187,     4,   214,
     214,    81,   257,   251,   252,   285,   285,    68,   255,     4,
     244,     3,   126,   128,   225,   227,   228,   233,    54,   213,
     334,   214,   217,   213,   283,   268,   285,   241,   213,   213,
      64,   214,   282,   213,    72,   246,   285,   285,   300,    83,
      85,    87,     4,   213,   213,   213,   213,     4,     4,   218,
     214,   214,    76,   284,     3,   285,   285,    74,   156,   213,
      72,   129,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,     3,   208,   305,     6,
     315,     6,   316,     6,     5,    43,    45,    46,   213,   213,
       3,   213,   214,     6,    33,    42,     4,   163,   163,   282,
      75,   258,   217,    69,    70,   253,   285,   132,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   106,   107,   108,   109,   110,
     111,   114,   117,   118,   121,   122,   123,   124,   229,   127,
     213,   214,   217,   244,     3,   132,     3,   282,   217,    67,
      68,    78,    79,    80,   183,   323,   324,   323,   282,   214,
     246,   214,    54,    86,    83,    85,   285,   285,    75,   285,
       4,     3,   303,   285,   217,   260,   214,   217,     5,     6,
     325,   213,   286,   246,   282,   129,   153,   218,   218,     6,
       6,   234,   226,   228,     3,   332,   333,     6,     4,     4,
     213,   265,   266,   267,   268,   273,   177,   259,   252,     4,
     213,   213,   213,   213,   213,   213,   213,    72,   126,   128,
     129,   230,   231,   330,   213,   234,    32,   331,   227,   214,
       4,   214,   213,     6,     6,     4,     3,     6,   214,   217,
     214,   214,   214,   229,   285,   285,    83,    86,   286,   217,
     217,   217,   217,     4,    65,   214,     4,    77,   246,   282,
     214,   214,   286,    50,    47,   214,   214,   217,   203,   214,
     217,     6,   244,   217,    55,    57,    58,    59,    60,    62,
      63,   274,     3,    54,   269,   287,   288,   289,   290,   291,
     292,   293,   294,   261,     6,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   119,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   119,    89,    90,    91,
      92,    93,    96,    97,   103,   104,   105,   119,    89,    90,
      91,    92,    93,    96,    97,   103,   104,   105,   119,    89,
      90,    91,    92,    93,    96,    97,   103,   104,   105,   119,
      89,    90,    91,    92,    93,    96,    97,   103,   104,   105,
     119,   129,   127,   131,   231,   232,   232,   234,   214,   213,
     132,   163,   282,   324,   214,    83,   285,   214,   211,   317,
       4,   305,   211,   306,   309,   314,   317,   217,   260,   285,
     214,   213,   214,   214,     6,     6,   228,     3,     4,     5,
       6,   333,    34,    36,   214,   266,    56,    56,     3,   217,
      51,   263,   214,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,    91,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,    91,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,    91,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,    91,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,    91,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,    91,   217,
     304,   132,   132,   214,   332,     4,     3,   214,     6,   217,
     217,   260,   217,   217,   214,   323,     6,   269,   267,   267,
     213,   293,    52,    53,   262,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   217,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   217,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   217,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   217,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     217,     6,     4,     4,   214,   330,     4,     4,   214,     4,
       4,   214,     6,    61,   234,   282,   285,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,     6,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,     6,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,     6,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
       6,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,     6,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,     6,   214,   217,   260,   217,   217,   260,    40,
      43,    44,    48,    49,   285,   214,   214,   214,   214,   214,
     214,   214,     4,   214,     4,     6,   214,     6,     6,   217,
     260,   217,   260,   330,     6,    45,    46,     6,   214,     4,
     214,    43,    44,     6,   260,   330,   260,   125,   163,   330,
       6,    48,   214,   214,    40,    40,   125,   163,   330,   213,
      40,    40,     3,   213,   214,     3,   330,   214,   330
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   219,   220,   221,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   225,   225,   226,   226,   227,   227,   228,   228,
     228,   228,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   230,   230,   231,   231,   231,   231,
     232,   232,   233,   233,   234,   234,   235,   236,   236,   237,
     237,   238,   238,   239,   240,   240,   241,   242,   242,   242,
     242,   242,   243,   243,   243,   244,   244,   244,   244,   245,
     245,   246,   247,   248,   248,   249,   250,   250,   251,   251,
     252,   253,   253,   253,   254,   254,   255,   255,   256,   256,
     257,   257,   258,   258,   259,   259,   260,   260,   261,   261,
     262,   262,   263,   263,   264,   264,   264,   264,   265,   265,
     266,   266,   267,   267,   268,   268,   269,   269,   269,   269,
     270,   270,   271,   271,   272,   273,   273,   274,   274,   274,
     274,   274,   274,   274,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   276,   276,   276,   277,   277,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   279,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   281,   281,   281,   282,   282,
     283,   283,   284,   284,   285,   285,   285,   285,   285,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   287,   288,   288,   288,   288,   289,   289,   289,
     289,   290,   290,   291,   291,   292,   292,   293,   293,   293,
     293,   293,   293,   294,   294,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   296,   296,
     297,   298,   298,   299,   299,   299,   299,   300,   300,   301,
     302,   302,   302,   302,   303,   303,   303,   303,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     305,   305,   305,   305,   306,   306,   306,   307,   308,   308,
     309,   309,   310,   311,   311,   312,   313,   313,   314,   315,
     316,   317,   317,   318,   319,   319,   320,   321,   321,   322,
     322,   322,   322,   322,   322,   322,   322,   322,   322,   322,
     322,   323,   323,   324,   324,   324,   324,   324,   324,   325,
     326,   326,   327,   327,   328,   328,   329,   329,   330,   330,
     331,   331,   332,   332,   333,   333,   333,   333,   333,   334,
     334
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
       3,     2,     4,     3,     3,     9,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     3,     0,     2,     0,
       2,     0,     3,     0,     1,     2,     1,     1,     1,     3,
       1,     1,     2,     4,     1,     3,     2,     1,     5,     0,
       2,     0,     1,     3,     5,     4,     6,     1,     1,     1,
       1,     1,     1,     0,     2,     2,     2,     2,     3,     2,
       2,     2,     2,     3,     2,     3,     3,     3,     4,     4,
       3,     3,     4,     4,     5,     6,     7,     9,     4,     5,
       7,     9,     2,     3,     2,     3,     3,     4,     2,     3,
       3,     4,     2,     2,     2,     2,     5,     2,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     3,     3,     3,     3,     4,     6,
       7,     9,    10,    12,    12,    13,    14,    15,    16,    12,
      13,    15,    16,     3,     4,     5,     6,     3,     3,     4,
       3,     3,     4,     4,     6,     5,     3,     4,     3,     4,
       3,     3,     5,     7,     7,     6,     8,     8,     1,     3,
       3,     5,     3,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,    14,    20,    16,    15,    13,    18,    14,    13,
      11,     8,    10,     5,     7,     4,     6,     1,     1,     1,
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
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2405 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2413 "parser.cpp"
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
#line 2427 "parser.cpp"
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
#line 2441 "parser.cpp"
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
#line 2455 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 310 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2466 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2492 "parser.cpp"
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
#line 2506 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 274 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2517 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2527 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2537 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2557 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2567 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 356 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2577 "parser.cpp"
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
#line 2591 "parser.cpp"
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
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 344 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2615 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2623 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2631 "parser.cpp"
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
#line 2645 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2654 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2662 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2670 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2678 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2686 "parser.cpp"
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
#line 2700 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2709 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2727 "parser.cpp"
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
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2749 "parser.cpp"
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
#line 2763 "parser.cpp"
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
#line 2777 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2787 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2796 "parser.cpp"
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
#line 2810 "parser.cpp"
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
#line 2824 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2840 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2856 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2864 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2872 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2880 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2888 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2896 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2904 "parser.cpp"
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
#line 2918 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2926 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2934 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2942 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2950 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2958 "parser.cpp"
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
#line 2971 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2979 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2987 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 326 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2995 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3003 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3011 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3019 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3027 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3035 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3043 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3051 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3059 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3067 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3075 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3083 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 371 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3091 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 375 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3099 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3107 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3115 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3123 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3131 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3139 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 330 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3147 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 317 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3155 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3166 "parser.cpp"
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
#line 3180 "parser.cpp"
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
#line 3194 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3205 "parser.cpp"
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

#line 3313 "parser.cpp"

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
#line 3528 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 507 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3539 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 513 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3550 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 520 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3556 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 521 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3562 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 522 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3568 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 523 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3574 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 524 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3580 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 525 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3586 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 526 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3592 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3598 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 528 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3604 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 529 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3610 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 530 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3616 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3622 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3628 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3634 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3640 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 536 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3646 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3652 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3658 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3664 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3670 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3676 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3682 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3688 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 544 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3694 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 545 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3700 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3706 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 547 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3712 "parser.cpp"
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
#line 3734 "parser.cpp"
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
#line 3754 "parser.cpp"
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
#line 3772 "parser.cpp"
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
#line 3805 "parser.cpp"
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
#line 3825 "parser.cpp"
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
#line 3861 "parser.cpp"
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
#line 3883 "parser.cpp"
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
#line 3904 "parser.cpp"
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
#line 3937 "parser.cpp"
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

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3971 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 774 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3980 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 778 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3989 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 783 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3998 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 787 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4007 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 793 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4015 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 796 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4023 "parser.cpp"
    break;

  case 48: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 803 "parser.y"
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
#line 4079 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 854 "parser.y"
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
#line 4121 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 891 "parser.y"
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
#line 4178 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 943 "parser.y"
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
#line 4221 "parser.cpp"
    break;

  case 52: /* column_type: BOOLEAN  */
#line 983 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4227 "parser.cpp"
    break;

  case 53: /* column_type: TINYINT  */
#line 984 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4233 "parser.cpp"
    break;

  case 54: /* column_type: SMALLINT  */
#line 985 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4239 "parser.cpp"
    break;

  case 55: /* column_type: INTEGER  */
#line 986 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4245 "parser.cpp"
    break;

  case 56: /* column_type: INT  */
#line 987 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4251 "parser.cpp"
    break;

  case 57: /* column_type: BIGINT  */
#line 988 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4257 "parser.cpp"
    break;

  case 58: /* column_type: HUGEINT  */
#line 989 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4263 "parser.cpp"
    break;

  case 59: /* column_type: FLOAT  */
#line 990 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4269 "parser.cpp"
    break;

  case 60: /* column_type: REAL  */
#line 991 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4275 "parser.cpp"
    break;

  case 61: /* column_type: DOUBLE  */
#line 992 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4281 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT16  */
#line 993 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4287 "parser.cpp"
    break;

  case 63: /* column_type: BFLOAT16  */
#line 994 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4293 "parser.cpp"
    break;

  case 64: /* column_type: DATE  */
#line 995 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4299 "parser.cpp"
    break;

  case 65: /* column_type: TIME  */
#line 996 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4305 "parser.cpp"
    break;

  case 66: /* column_type: DATETIME  */
#line 997 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4311 "parser.cpp"
    break;

  case 67: /* column_type: TIMESTAMP  */
#line 998 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4317 "parser.cpp"
    break;

  case 68: /* column_type: UUID  */
#line 999 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4323 "parser.cpp"
    break;

  case 69: /* column_type: POINT  */
#line 1000 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4329 "parser.cpp"
    break;

  case 70: /* column_type: LINE  */
#line 1001 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4335 "parser.cpp"
    break;

  case 71: /* column_type: LSEG  */
#line 1002 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4341 "parser.cpp"
    break;

  case 72: /* column_type: BOX  */
#line 1003 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4347 "parser.cpp"
    break;

  case 73: /* column_type: CIRCLE  */
#line 1006 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4353 "parser.cpp"
    break;

  case 74: /* column_type: VARCHAR  */
#line 1008 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4359 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4365 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 1010 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4371 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL  */
#line 1011 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4377 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4383 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4389 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4395 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4401 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4407 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4413 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4419 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4425 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4431 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4437 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4443 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4449 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4455 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4461 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4467 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4473 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4479 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4485 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4491 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4497 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4503 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4509 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4515 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4521 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4527 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4533 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4539 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4545 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4551 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4557 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4563 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4569 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4575 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4581 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4587 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4593 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4599 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4605 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4611 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4617 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1054 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4623 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1055 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4629 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1056 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4635 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1057 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4641 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1058 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4647 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1059 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4653 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1060 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4659 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1061 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4665 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1062 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4671 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1063 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4677 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1064 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4683 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1065 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4689 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1066 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4695 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1067 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4701 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1068 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4707 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1069 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4713 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1070 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4719 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1071 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4725 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1072 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4731 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1073 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4737 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1074 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4743 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1075 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4749 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1076 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4755 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1077 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4761 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1078 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4767 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1079 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4773 "parser.cpp"
    break;

  case 144: /* column_constraints: column_constraint  */
#line 1098 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4782 "parser.cpp"
    break;

  case 145: /* column_constraints: column_constraints column_constraint  */
#line 1102 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4796 "parser.cpp"
    break;

  case 146: /* column_constraint: PRIMARY KEY  */
#line 1112 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4804 "parser.cpp"
    break;

  case 147: /* column_constraint: UNIQUE  */
#line 1115 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4812 "parser.cpp"
    break;

  case 148: /* column_constraint: NULLABLE  */
#line 1118 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4820 "parser.cpp"
    break;

  case 149: /* column_constraint: NOT NULLABLE  */
#line 1121 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4828 "parser.cpp"
    break;

  case 150: /* default_expr: DEFAULT constant_expr  */
#line 1125 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4836 "parser.cpp"
    break;

  case 151: /* default_expr: %empty  */
#line 1128 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4844 "parser.cpp"
    break;

  case 152: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1133 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4854 "parser.cpp"
    break;

  case 153: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1138 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4864 "parser.cpp"
    break;

  case 154: /* identifier_array: IDENTIFIER  */
#line 1145 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4875 "parser.cpp"
    break;

  case 155: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1151 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4886 "parser.cpp"
    break;

  case 156: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1161 "parser.y"
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
#line 4903 "parser.cpp"
    break;

  case 157: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1177 "parser.y"
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
#line 4946 "parser.cpp"
    break;

  case 158: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1215 "parser.y"
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
#line 4966 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1231 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4974 "parser.cpp"
    break;

  case 160: /* optional_identifier_array: %empty  */
#line 1234 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4982 "parser.cpp"
    break;

  case 161: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1241 "parser.y"
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
#line 5000 "parser.cpp"
    break;

  case 162: /* explain_statement: EXPLAIN explainable_statement  */
#line 1253 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5010 "parser.cpp"
    break;

  case 163: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1262 "parser.y"
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
#line 5027 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr  */
#line 1275 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5036 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1279 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5045 "parser.cpp"
    break;

  case 166: /* update_expr: IDENTIFIER '=' expr  */
#line 1284 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5057 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1297 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5073 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1310 "parser.y"
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
#line 5091 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1325 "parser.y"
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
#line 5109 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1340 "parser.y"
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
#line 5127 "parser.cpp"
    break;

  case 171: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1355 "parser.y"
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
#line 5150 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1378 "parser.y"
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
#line 5208 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1431 "parser.y"
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
#line 5268 "parser.cpp"
    break;

  case 174: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1486 "parser.y"
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
#line 5320 "parser.cpp"
    break;

  case 175: /* select_statement: select_without_paren  */
#line 1537 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5328 "parser.cpp"
    break;

  case 176: /* select_statement: select_with_paren  */
#line 1540 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5336 "parser.cpp"
    break;

  case 177: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1543 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5350 "parser.cpp"
    break;

  case 178: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1552 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5364 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_without_paren ')'  */
#line 1562 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5372 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_with_paren ')'  */
#line 1565 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5380 "parser.cpp"
    break;

  case 181: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1569 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5389 "parser.cpp"
    break;

  case 182: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1574 "parser.y"
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
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 5420 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1601 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5428 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1604 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5436 "parser.cpp"
    break;

  case 185: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1609 "parser.y"
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
#line 5457 "parser.cpp"
    break;

  case 186: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1626 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5465 "parser.cpp"
    break;

  case 187: /* order_by_clause: %empty  */
#line 1629 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5473 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr  */
#line 1633 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5482 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1637 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5491 "parser.cpp"
    break;

  case 190: /* order_by_expr: expr order_by_type  */
#line 1642 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5501 "parser.cpp"
    break;

  case 191: /* order_by_type: ASC  */
#line 1648 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5509 "parser.cpp"
    break;

  case 192: /* order_by_type: DESC  */
#line 1651 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5517 "parser.cpp"
    break;

  case 193: /* order_by_type: %empty  */
#line 1654 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5525 "parser.cpp"
    break;

  case 194: /* limit_expr: LIMIT expr  */
#line 1658 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5533 "parser.cpp"
    break;

  case 195: /* limit_expr: %empty  */
#line 1662 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5539 "parser.cpp"
    break;

  case 196: /* offset_expr: OFFSET expr  */
#line 1664 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5547 "parser.cpp"
    break;

  case 197: /* offset_expr: %empty  */
#line 1668 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5553 "parser.cpp"
    break;

  case 198: /* distinct: DISTINCT  */
#line 1670 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5561 "parser.cpp"
    break;

  case 199: /* distinct: %empty  */
#line 1673 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5569 "parser.cpp"
    break;

  case 200: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1677 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5577 "parser.cpp"
    break;

  case 201: /* highlight_clause: %empty  */
#line 1680 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5585 "parser.cpp"
    break;

  case 202: /* from_clause: FROM table_reference  */
#line 1684 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5593 "parser.cpp"
    break;

  case 203: /* from_clause: %empty  */
#line 1687 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5601 "parser.cpp"
    break;

  case 204: /* search_clause: SEARCH sub_search_array  */
#line 1691 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5611 "parser.cpp"
    break;

  case 205: /* search_clause: %empty  */
#line 1696 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5619 "parser.cpp"
    break;

  case 206: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1700 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5627 "parser.cpp"
    break;

  case 207: /* optional_search_filter_expr: %empty  */
#line 1703 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5635 "parser.cpp"
    break;

  case 208: /* where_clause: WHERE expr  */
#line 1707 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5643 "parser.cpp"
    break;

  case 209: /* where_clause: %empty  */
#line 1710 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5651 "parser.cpp"
    break;

  case 210: /* having_clause: HAVING expr  */
#line 1714 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5659 "parser.cpp"
    break;

  case 211: /* having_clause: %empty  */
#line 1717 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5667 "parser.cpp"
    break;

  case 212: /* group_by_clause: GROUP BY expr_array  */
#line 1721 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5675 "parser.cpp"
    break;

  case 213: /* group_by_clause: %empty  */
#line 1724 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5683 "parser.cpp"
    break;

  case 214: /* set_operator: UNION  */
#line 1728 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5691 "parser.cpp"
    break;

  case 215: /* set_operator: UNION ALL  */
#line 1731 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5699 "parser.cpp"
    break;

  case 216: /* set_operator: INTERSECT  */
#line 1734 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5707 "parser.cpp"
    break;

  case 217: /* set_operator: EXCEPT  */
#line 1737 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5715 "parser.cpp"
    break;

  case 218: /* table_reference: table_reference_unit  */
#line 1745 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5723 "parser.cpp"
    break;

  case 219: /* table_reference: table_reference ',' table_reference_unit  */
#line 1748 "parser.y"
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
#line 5741 "parser.cpp"
    break;

  case 222: /* table_reference_name: table_name table_alias  */
#line 1765 "parser.y"
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
#line 5759 "parser.cpp"
    break;

  case 223: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1779 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5770 "parser.cpp"
    break;

  case 224: /* table_name: IDENTIFIER  */
#line 1788 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5780 "parser.cpp"
    break;

  case 225: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1793 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5792 "parser.cpp"
    break;

  case 226: /* table_alias: AS IDENTIFIER  */
#line 1802 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5802 "parser.cpp"
    break;

  case 227: /* table_alias: IDENTIFIER  */
#line 1807 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5812 "parser.cpp"
    break;

  case 228: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1812 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5823 "parser.cpp"
    break;

  case 229: /* table_alias: %empty  */
#line 1818 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5831 "parser.cpp"
    break;

  case 230: /* with_clause: WITH with_expr_list  */
#line 1825 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5839 "parser.cpp"
    break;

  case 231: /* with_clause: %empty  */
#line 1828 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5847 "parser.cpp"
    break;

  case 232: /* with_expr_list: with_expr  */
#line 1832 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5856 "parser.cpp"
    break;

  case 233: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1835 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5865 "parser.cpp"
    break;

  case 234: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1840 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5877 "parser.cpp"
    break;

  case 235: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1852 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5889 "parser.cpp"
    break;

  case 236: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1859 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5902 "parser.cpp"
    break;

  case 237: /* join_type: INNER  */
#line 1873 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5910 "parser.cpp"
    break;

  case 238: /* join_type: LEFT  */
#line 1876 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5918 "parser.cpp"
    break;

  case 239: /* join_type: RIGHT  */
#line 1879 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5926 "parser.cpp"
    break;

  case 240: /* join_type: OUTER  */
#line 1882 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5934 "parser.cpp"
    break;

  case 241: /* join_type: FULL  */
#line 1885 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5942 "parser.cpp"
    break;

  case 242: /* join_type: CROSS  */
#line 1888 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5950 "parser.cpp"
    break;

  case 243: /* join_type: %empty  */
#line 1891 "parser.y"
                {
}
#line 5957 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW DATABASES  */
#line 1897 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5966 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW TABLES  */
#line 1901 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5975 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW VIEWS  */
#line 1905 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5984 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW CONFIGS  */
#line 1909 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5993 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1913 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6005 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW PROFILES  */
#line 1920 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6014 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW BUFFER  */
#line 1924 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6023 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW MEMINDEX  */
#line 1928 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6032 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW QUERIES  */
#line 1932 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6041 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1936 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6051 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TRANSACTIONS  */
#line 1941 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6060 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1945 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6070 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW SESSION VARIABLES  */
#line 1950 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6079 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1954 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6088 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1958 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6099 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1964 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6110 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1970 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6121 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name  */
#line 1976 "parser.y"
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
#line 6137 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1987 "parser.y"
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
#line 6153 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1998 "parser.y"
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
#line 6169 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 2009 "parser.y"
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
#line 6186 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 2021 "parser.y"
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
#line 6203 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2033 "parser.y"
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
#line 6221 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2046 "parser.y"
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
#line 6240 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2060 "parser.y"
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
#line 6256 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2071 "parser.y"
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
#line 6275 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2085 "parser.y"
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
#line 6296 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2101 "parser.y"
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
#line 6318 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW LOGS  */
#line 2118 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6327 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW DELTA LOGS  */
#line 2122 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6336 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW CATALOGS  */
#line 2126 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6345 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2130 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6354 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2134 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6363 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2138 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6374 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW MEMORY  */
#line 2144 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6383 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2148 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6392 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2152 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6401 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2156 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6412 "parser.cpp"
    break;

  case 282: /* flush_statement: FLUSH DATA  */
#line 2166 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6421 "parser.cpp"
    break;

  case 283: /* flush_statement: FLUSH LOG  */
#line 2170 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6430 "parser.cpp"
    break;

  case 284: /* flush_statement: FLUSH BUFFER  */
#line 2174 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6439 "parser.cpp"
    break;

  case 285: /* optimize_statement: OPTIMIZE table_name  */
#line 2182 "parser.y"
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
#line 6454 "parser.cpp"
    break;

  case 286: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2193 "parser.y"
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
#line 6478 "parser.cpp"
    break;

  case 287: /* command_statement: USE IDENTIFIER  */
#line 2216 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6489 "parser.cpp"
    break;

  case 288: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2222 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6499 "parser.cpp"
    break;

  case 289: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2227 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6510 "parser.cpp"
    break;

  case 290: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2233 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6521 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2239 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6534 "parser.cpp"
    break;

  case 292: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2247 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6545 "parser.cpp"
    break;

  case 293: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2253 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6556 "parser.cpp"
    break;

  case 294: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2259 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6567 "parser.cpp"
    break;

  case 295: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2265 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6578 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2271 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6591 "parser.cpp"
    break;

  case 297: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2279 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6602 "parser.cpp"
    break;

  case 298: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2285 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6613 "parser.cpp"
    break;

  case 299: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2291 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6624 "parser.cpp"
    break;

  case 300: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2297 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6635 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2303 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6648 "parser.cpp"
    break;

  case 302: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2311 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6659 "parser.cpp"
    break;

  case 303: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2317 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6670 "parser.cpp"
    break;

  case 304: /* command_statement: LOCK TABLE table_name  */
#line 2323 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6684 "parser.cpp"
    break;

  case 305: /* command_statement: UNLOCK TABLE table_name  */
#line 2332 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6698 "parser.cpp"
    break;

  case 306: /* compact_statement: COMPACT TABLE table_name  */
#line 2342 "parser.y"
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
#line 6715 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2355 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6724 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2359 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6734 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2364 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6745 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2370 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6757 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2377 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6770 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2385 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6784 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2394 "parser.y"
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
#line 6799 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2404 "parser.y"
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
#line 6814 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2414 "parser.y"
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
#line 6830 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2425 "parser.y"
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
#line 6846 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2436 "parser.y"
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
#line 6863 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2448 "parser.y"
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
#line 6880 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2460 "parser.y"
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
#line 6895 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2470 "parser.y"
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
#line 6911 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2481 "parser.y"
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
#line 6928 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2493 "parser.y"
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
#line 6946 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW LOGS  */
#line 2506 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6955 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2510 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6965 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2515 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6975 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2520 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6986 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2526 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6995 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2530 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7004 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2534 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7015 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2540 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7024 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2544 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7033 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW SNAPSHOT STRING  */
#line 2548 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7044 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2554 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7055 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2560 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7068 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2568 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7079 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW NODES  */
#line 2574 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7088 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2578 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7099 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW NODE  */
#line 2584 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7108 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2588 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7119 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SET ADMIN  */
#line 2594 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7129 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SET STANDALONE  */
#line 2599 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7139 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2604 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7151 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2611 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7165 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2620 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7179 "parser.cpp"
    break;

  case 345: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2630 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7193 "parser.cpp"
    break;

  case 346: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2639 "parser.y"
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
#line 7210 "parser.cpp"
    break;

  case 347: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2651 "parser.y"
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
#line 7226 "parser.cpp"
    break;

  case 348: /* expr_array: expr_alias  */
#line 2667 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7235 "parser.cpp"
    break;

  case 349: /* expr_array: expr_array ',' expr_alias  */
#line 2671 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7244 "parser.cpp"
    break;

  case 350: /* insert_row_list: '(' expr_array ')'  */
#line 2676 "parser.y"
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
#line 7259 "parser.cpp"
    break;

  case 351: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2686 "parser.y"
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
#line 7274 "parser.cpp"
    break;

  case 352: /* expr_alias: expr AS IDENTIFIER  */
#line 2708 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7285 "parser.cpp"
    break;

  case 353: /* expr_alias: expr  */
#line 2714 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7293 "parser.cpp"
    break;

  case 359: /* operand: '(' expr ')'  */
#line 2724 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7301 "parser.cpp"
    break;

  case 360: /* operand: '(' select_without_paren ')'  */
#line 2727 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7312 "parser.cpp"
    break;

  case 361: /* operand: constant_expr  */
#line 2733 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7320 "parser.cpp"
    break;

  case 372: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2749 "parser.y"
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
#line 7340 "parser.cpp"
    break;

  case 373: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2767 "parser.y"
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
#line 7390 "parser.cpp"
    break;

  case 374: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2813 "parser.y"
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
#line 7433 "parser.cpp"
    break;

  case 375: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2852 "parser.y"
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
#line 7480 "parser.cpp"
    break;

  case 376: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2895 "parser.y"
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
#line 7528 "parser.cpp"
    break;

  case 377: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2942 "parser.y"
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
#line 7556 "parser.cpp"
    break;

  case 378: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2966 "parser.y"
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
#line 7583 "parser.cpp"
    break;

  case 379: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2989 "parser.y"
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
#line 7608 "parser.cpp"
    break;

  case 380: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3010 "parser.y"
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
#line 7633 "parser.cpp"
    break;

  case 381: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3031 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7647 "parser.cpp"
    break;

  case 382: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3040 "parser.y"
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
#line 7663 "parser.cpp"
    break;

  case 383: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3052 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7675 "parser.cpp"
    break;

  case 384: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3059 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7689 "parser.cpp"
    break;

  case 385: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3069 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7700 "parser.cpp"
    break;

  case 386: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3075 "parser.y"
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
#line 7716 "parser.cpp"
    break;

  case 387: /* sub_search: match_vector_expr  */
#line 3087 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7724 "parser.cpp"
    break;

  case 388: /* sub_search: match_text_expr  */
#line 3090 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7732 "parser.cpp"
    break;

  case 389: /* sub_search: match_tensor_expr  */
#line 3093 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7740 "parser.cpp"
    break;

  case 390: /* sub_search: match_sparse_expr  */
#line 3096 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7748 "parser.cpp"
    break;

  case 391: /* sub_search: query_expr  */
#line 3099 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7756 "parser.cpp"
    break;

  case 392: /* sub_search: fusion_expr  */
#line 3102 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7764 "parser.cpp"
    break;

  case 393: /* sub_search_array: sub_search  */
#line 3106 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7773 "parser.cpp"
    break;

  case 394: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3110 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7782 "parser.cpp"
    break;

  case 395: /* function_expr: IDENTIFIER '(' ')'  */
#line 3115 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7795 "parser.cpp"
    break;

  case 396: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3123 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7808 "parser.cpp"
    break;

  case 397: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3131 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7822 "parser.cpp"
    break;

  case 398: /* function_expr: operand IS NOT NULLABLE  */
#line 3140 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7834 "parser.cpp"
    break;

  case 399: /* function_expr: operand IS NULLABLE  */
#line 3147 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7846 "parser.cpp"
    break;

  case 400: /* function_expr: NOT operand  */
#line 3154 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7858 "parser.cpp"
    break;

  case 401: /* function_expr: '-' operand  */
#line 3161 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7870 "parser.cpp"
    break;

  case 402: /* function_expr: '+' operand  */
#line 3168 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7882 "parser.cpp"
    break;

  case 403: /* function_expr: operand '-' operand  */
#line 3175 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7895 "parser.cpp"
    break;

  case 404: /* function_expr: operand '+' operand  */
#line 3183 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7908 "parser.cpp"
    break;

  case 405: /* function_expr: operand '*' operand  */
#line 3191 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7921 "parser.cpp"
    break;

  case 406: /* function_expr: operand '/' operand  */
#line 3199 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7934 "parser.cpp"
    break;

  case 407: /* function_expr: operand '%' operand  */
#line 3207 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7947 "parser.cpp"
    break;

  case 408: /* function_expr: operand '=' operand  */
#line 3215 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7960 "parser.cpp"
    break;

  case 409: /* function_expr: operand EQUAL operand  */
#line 3223 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7973 "parser.cpp"
    break;

  case 410: /* function_expr: operand NOT_EQ operand  */
#line 3231 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7986 "parser.cpp"
    break;

  case 411: /* function_expr: operand '<' operand  */
#line 3239 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7999 "parser.cpp"
    break;

  case 412: /* function_expr: operand '>' operand  */
#line 3247 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8012 "parser.cpp"
    break;

  case 413: /* function_expr: operand LESS_EQ operand  */
#line 3255 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8025 "parser.cpp"
    break;

  case 414: /* function_expr: operand GREATER_EQ operand  */
#line 3263 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8038 "parser.cpp"
    break;

  case 415: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3271 "parser.y"
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
#line 8073 "parser.cpp"
    break;

  case 416: /* function_expr: operand LIKE operand  */
#line 3301 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8086 "parser.cpp"
    break;

  case 417: /* function_expr: operand NOT LIKE operand  */
#line 3309 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8099 "parser.cpp"
    break;

  case 418: /* conjunction_expr: expr AND expr  */
#line 3318 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8112 "parser.cpp"
    break;

  case 419: /* conjunction_expr: expr OR expr  */
#line 3326 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8125 "parser.cpp"
    break;

  case 420: /* between_expr: operand BETWEEN operand AND operand  */
#line 3335 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8137 "parser.cpp"
    break;

  case 421: /* in_expr: operand IN '(' expr_array ')'  */
#line 3343 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8148 "parser.cpp"
    break;

  case 422: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3349 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8159 "parser.cpp"
    break;

  case 423: /* case_expr: CASE expr case_check_array END  */
#line 3356 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8170 "parser.cpp"
    break;

  case 424: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3362 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8182 "parser.cpp"
    break;

  case 425: /* case_expr: CASE case_check_array END  */
#line 3369 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8192 "parser.cpp"
    break;

  case 426: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3374 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8203 "parser.cpp"
    break;

  case 427: /* case_check_array: WHEN expr THEN expr  */
#line 3381 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8215 "parser.cpp"
    break;

  case 428: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3388 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8227 "parser.cpp"
    break;

  case 429: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3396 "parser.y"
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
#line 8258 "parser.cpp"
    break;

  case 430: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3423 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8269 "parser.cpp"
    break;

  case 431: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3429 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8280 "parser.cpp"
    break;

  case 432: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3435 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8292 "parser.cpp"
    break;

  case 433: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3442 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8304 "parser.cpp"
    break;

  case 434: /* column_expr: IDENTIFIER  */
#line 3450 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8316 "parser.cpp"
    break;

  case 435: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3457 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8328 "parser.cpp"
    break;

  case 436: /* column_expr: '*'  */
#line 3464 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8338 "parser.cpp"
    break;

  case 437: /* column_expr: column_expr '.' '*'  */
#line 3469 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8352 "parser.cpp"
    break;

  case 438: /* constant_expr: STRING  */
#line 3479 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8362 "parser.cpp"
    break;

  case 439: /* constant_expr: TRUE  */
#line 3484 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8372 "parser.cpp"
    break;

  case 440: /* constant_expr: FALSE  */
#line 3489 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8382 "parser.cpp"
    break;

  case 441: /* constant_expr: DOUBLE_VALUE  */
#line 3494 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8392 "parser.cpp"
    break;

  case 442: /* constant_expr: LONG_VALUE  */
#line 3499 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8402 "parser.cpp"
    break;

  case 443: /* constant_expr: DATE STRING  */
#line 3504 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8412 "parser.cpp"
    break;

  case 444: /* constant_expr: TIME STRING  */
#line 3509 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8422 "parser.cpp"
    break;

  case 445: /* constant_expr: DATETIME STRING  */
#line 3514 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8432 "parser.cpp"
    break;

  case 446: /* constant_expr: TIMESTAMP STRING  */
#line 3519 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8442 "parser.cpp"
    break;

  case 447: /* constant_expr: INTERVAL interval_expr  */
#line 3524 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8450 "parser.cpp"
    break;

  case 448: /* constant_expr: interval_expr  */
#line 3527 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8458 "parser.cpp"
    break;

  case 449: /* constant_expr: common_array_expr  */
#line 3530 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8466 "parser.cpp"
    break;

  case 450: /* common_array_expr: array_expr  */
#line 3534 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8474 "parser.cpp"
    break;

  case 451: /* common_array_expr: subarray_array_expr  */
#line 3537 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8482 "parser.cpp"
    break;

  case 452: /* common_array_expr: sparse_array_expr  */
#line 3540 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8490 "parser.cpp"
    break;

  case 453: /* common_array_expr: empty_array_expr  */
#line 3543 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8498 "parser.cpp"
    break;

  case 454: /* common_sparse_array_expr: sparse_array_expr  */
#line 3547 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8506 "parser.cpp"
    break;

  case 455: /* common_sparse_array_expr: array_expr  */
#line 3550 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8514 "parser.cpp"
    break;

  case 456: /* common_sparse_array_expr: empty_array_expr  */
#line 3553 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8522 "parser.cpp"
    break;

  case 457: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3557 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8530 "parser.cpp"
    break;

  case 458: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3561 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8540 "parser.cpp"
    break;

  case 459: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3566 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8549 "parser.cpp"
    break;

  case 460: /* sparse_array_expr: long_sparse_array_expr  */
#line 3571 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8557 "parser.cpp"
    break;

  case 461: /* sparse_array_expr: double_sparse_array_expr  */
#line 3574 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8565 "parser.cpp"
    break;

  case 462: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3578 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8573 "parser.cpp"
    break;

  case 463: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3582 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8585 "parser.cpp"
    break;

  case 464: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3589 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8596 "parser.cpp"
    break;

  case 465: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3596 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8604 "parser.cpp"
    break;

  case 466: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3600 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8616 "parser.cpp"
    break;

  case 467: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3607 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8627 "parser.cpp"
    break;

  case 468: /* empty_array_expr: '[' ']'  */
#line 3614 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8635 "parser.cpp"
    break;

  case 469: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3618 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8643 "parser.cpp"
    break;

  case 470: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3622 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8651 "parser.cpp"
    break;

  case 471: /* array_expr: long_array_expr  */
#line 3626 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8659 "parser.cpp"
    break;

  case 472: /* array_expr: double_array_expr  */
#line 3629 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8667 "parser.cpp"
    break;

  case 473: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3633 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8675 "parser.cpp"
    break;

  case 474: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3637 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8685 "parser.cpp"
    break;

  case 475: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3642 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8694 "parser.cpp"
    break;

  case 476: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3647 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8702 "parser.cpp"
    break;

  case 477: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3651 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8712 "parser.cpp"
    break;

  case 478: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3656 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8721 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE SECONDS  */
#line 3661 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8732 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE SECOND  */
#line 3667 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8743 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE MINUTES  */
#line 3673 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8754 "parser.cpp"
    break;

  case 482: /* interval_expr: LONG_VALUE MINUTE  */
#line 3679 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8765 "parser.cpp"
    break;

  case 483: /* interval_expr: LONG_VALUE HOURS  */
#line 3685 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8776 "parser.cpp"
    break;

  case 484: /* interval_expr: LONG_VALUE HOUR  */
#line 3691 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8787 "parser.cpp"
    break;

  case 485: /* interval_expr: LONG_VALUE DAYS  */
#line 3697 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8798 "parser.cpp"
    break;

  case 486: /* interval_expr: LONG_VALUE DAY  */
#line 3703 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8809 "parser.cpp"
    break;

  case 487: /* interval_expr: LONG_VALUE MONTHS  */
#line 3709 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8820 "parser.cpp"
    break;

  case 488: /* interval_expr: LONG_VALUE MONTH  */
#line 3715 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8831 "parser.cpp"
    break;

  case 489: /* interval_expr: LONG_VALUE YEARS  */
#line 3721 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8842 "parser.cpp"
    break;

  case 490: /* interval_expr: LONG_VALUE YEAR  */
#line 3727 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8853 "parser.cpp"
    break;

  case 491: /* copy_option_list: copy_option  */
#line 3738 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8862 "parser.cpp"
    break;

  case 492: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3742 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8871 "parser.cpp"
    break;

  case 493: /* copy_option: FORMAT IDENTIFIER  */
#line 3747 "parser.y"
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
#line 8907 "parser.cpp"
    break;

  case 494: /* copy_option: DELIMITER STRING  */
#line 3778 "parser.y"
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
#line 8922 "parser.cpp"
    break;

  case 495: /* copy_option: HEADER  */
#line 3788 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8932 "parser.cpp"
    break;

  case 496: /* copy_option: OFFSET LONG_VALUE  */
#line 3793 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8942 "parser.cpp"
    break;

  case 497: /* copy_option: LIMIT LONG_VALUE  */
#line 3798 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8952 "parser.cpp"
    break;

  case 498: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3803 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8962 "parser.cpp"
    break;

  case 499: /* file_path: STRING  */
#line 3809 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8970 "parser.cpp"
    break;

  case 500: /* if_exists: IF EXISTS  */
#line 3813 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8976 "parser.cpp"
    break;

  case 501: /* if_exists: %empty  */
#line 3814 "parser.y"
  { (yyval.bool_value) = false; }
#line 8982 "parser.cpp"
    break;

  case 502: /* if_not_exists: IF NOT EXISTS  */
#line 3816 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8988 "parser.cpp"
    break;

  case 503: /* if_not_exists: %empty  */
#line 3817 "parser.y"
  { (yyval.bool_value) = false; }
#line 8994 "parser.cpp"
    break;

  case 506: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3832 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9007 "parser.cpp"
    break;

  case 507: /* if_not_exists_info: %empty  */
#line 3840 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9015 "parser.cpp"
    break;

  case 508: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3844 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9023 "parser.cpp"
    break;

  case 509: /* with_index_param_list: %empty  */
#line 3847 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9031 "parser.cpp"
    break;

  case 510: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3851 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9039 "parser.cpp"
    break;

  case 511: /* optional_table_properties_list: %empty  */
#line 3854 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9047 "parser.cpp"
    break;

  case 512: /* index_param_list: index_param  */
#line 3858 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9056 "parser.cpp"
    break;

  case 513: /* index_param_list: index_param_list ',' index_param  */
#line 3862 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9065 "parser.cpp"
    break;

  case 514: /* index_param: IDENTIFIER  */
#line 3867 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9076 "parser.cpp"
    break;

  case 515: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3873 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9091 "parser.cpp"
    break;

  case 516: /* index_param: IDENTIFIER '=' STRING  */
#line 3883 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9106 "parser.cpp"
    break;

  case 517: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3893 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9119 "parser.cpp"
    break;

  case 518: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3901 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9132 "parser.cpp"
    break;

  case 519: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3912 "parser.y"
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
#line 9168 "parser.cpp"
    break;

  case 520: /* index_info: '(' IDENTIFIER ')'  */
#line 3943 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9179 "parser.cpp"
    break;


#line 9183 "parser.cpp"

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

#line 3950 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

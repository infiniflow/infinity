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
  YYSYMBOL_expr_array_list = 283,          /* expr_array_list  */
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
#define YYLAST   1417

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  219
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  519
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1167

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
       0,   506,   506,   510,   516,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   557,   574,   591,   607,   636,   651,   683,   701,
     719,   748,   752,   757,   761,   767,   770,   777,   828,   865,
     917,   957,   958,   959,   960,   961,   962,   963,   964,   965,
     966,   967,   968,   969,   970,   971,   972,   973,   974,   975,
     976,   977,   980,   982,   983,   984,   985,   988,   989,   990,
     991,   992,   993,   994,   995,   996,   997,   998,   999,  1000,
    1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,
    1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,  1019,  1020,
    1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,  1029,  1030,
    1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1040,
    1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,  1050,
    1051,  1052,  1053,  1072,  1076,  1086,  1089,  1092,  1095,  1099,
    1102,  1107,  1112,  1119,  1125,  1135,  1151,  1185,  1198,  1201,
    1208,  1220,  1229,  1242,  1246,  1251,  1264,  1277,  1292,  1307,
    1322,  1345,  1398,  1453,  1504,  1507,  1510,  1519,  1529,  1532,
    1536,  1541,  1568,  1571,  1576,  1593,  1596,  1600,  1604,  1609,
    1615,  1618,  1621,  1625,  1629,  1631,  1635,  1637,  1640,  1644,
    1647,  1651,  1654,  1658,  1663,  1667,  1670,  1674,  1677,  1681,
    1684,  1688,  1691,  1695,  1698,  1701,  1704,  1712,  1715,  1730,
    1730,  1732,  1746,  1755,  1760,  1769,  1774,  1779,  1785,  1792,
    1795,  1799,  1802,  1807,  1819,  1826,  1840,  1843,  1846,  1849,
    1852,  1855,  1858,  1864,  1868,  1872,  1876,  1880,  1887,  1891,
    1895,  1899,  1903,  1908,  1912,  1917,  1921,  1925,  1931,  1937,
    1943,  1954,  1965,  1976,  1988,  2000,  2013,  2027,  2038,  2052,
    2068,  2085,  2089,  2093,  2097,  2101,  2105,  2111,  2115,  2119,
    2123,  2133,  2137,  2141,  2149,  2160,  2183,  2189,  2194,  2200,
    2206,  2214,  2220,  2226,  2232,  2238,  2246,  2252,  2258,  2264,
    2270,  2278,  2284,  2290,  2299,  2309,  2322,  2326,  2331,  2337,
    2344,  2352,  2361,  2371,  2381,  2392,  2403,  2415,  2427,  2437,
    2448,  2460,  2473,  2477,  2482,  2487,  2493,  2497,  2501,  2507,
    2511,  2515,  2521,  2527,  2535,  2541,  2545,  2551,  2555,  2561,
    2566,  2571,  2578,  2587,  2597,  2606,  2618,  2634,  2638,  2643,
    2647,  2680,  2686,  2690,  2691,  2692,  2693,  2694,  2696,  2699,
    2705,  2708,  2709,  2710,  2711,  2712,  2713,  2714,  2715,  2716,
    2717,  2721,  2739,  2785,  2824,  2867,  2914,  2938,  2961,  2982,
    3003,  3012,  3024,  3031,  3041,  3047,  3059,  3062,  3065,  3068,
    3071,  3074,  3078,  3082,  3087,  3095,  3103,  3112,  3119,  3126,
    3133,  3140,  3147,  3155,  3163,  3171,  3179,  3187,  3195,  3203,
    3211,  3219,  3227,  3235,  3243,  3273,  3281,  3290,  3298,  3307,
    3315,  3321,  3328,  3334,  3341,  3346,  3353,  3360,  3368,  3395,
    3401,  3407,  3414,  3422,  3429,  3436,  3441,  3451,  3456,  3461,
    3466,  3471,  3476,  3481,  3486,  3491,  3496,  3499,  3502,  3506,
    3509,  3512,  3515,  3519,  3522,  3525,  3529,  3533,  3538,  3543,
    3546,  3550,  3554,  3561,  3568,  3572,  3579,  3586,  3590,  3594,
    3598,  3601,  3605,  3609,  3614,  3619,  3623,  3628,  3633,  3639,
    3645,  3651,  3657,  3663,  3669,  3675,  3681,  3687,  3693,  3699,
    3710,  3714,  3719,  3750,  3760,  3765,  3770,  3775,  3781,  3785,
    3786,  3788,  3789,  3791,  3792,  3804,  3812,  3816,  3819,  3823,
    3826,  3830,  3834,  3839,  3845,  3855,  3865,  3873,  3884,  3915
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

#define YYPACT_NINF (-696)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-507)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     328,   348,     0,   423,    47,    78,    47,   -59,    88,   786,
      53,   124,   190,   237,   168,   282,   295,   368,   -89,   197,
     -41,   166,    63,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    -696,   326,  -696,  -696,   383,  -696,  -696,  -696,  -696,  -696,
    -696,  -696,   345,   345,   345,   345,   248,    47,   353,   353,
     353,   353,   353,   232,   437,    47,    -3,   475,   477,   487,
     283,  -696,  -696,  -696,  -696,  -696,  -696,  -696,   326,  -696,
    -696,  -696,  -696,  -696,   281,   510,    47,  -696,  -696,  -696,
    -696,  -696,   511,  -696,   288,   293,  -696,   525,  -696,   356,
    -696,  -696,   533,  -696,   427,   -71,    47,    47,    47,    47,
    -696,  -696,  -696,  -696,   -44,  -696,   465,   350,  -696,   547,
     387,   393,   102,   520,   417,   565,   421,   500,   407,   414,
    -696,    54,  -696,   609,  -696,  -696,     6,   577,  -696,   584,
     571,   649,    47,    47,    47,   650,   593,   445,   586,   658,
      47,    47,    47,   667,   668,   669,   612,   670,   670,   603,
      46,    83,   134,  -696,   466,  -696,   330,  -696,  -696,   676,
    -696,   678,  -696,  -696,  -696,   679,  -696,  -696,  -696,  -696,
     153,  -696,  -696,  -696,    47,   469,   368,   670,  -696,   680,
    -696,   523,  -696,   684,  -696,  -696,   686,  -696,  -696,   687,
    -696,   693,   694,  -696,   695,   652,   703,   515,  -696,  -696,
    -696,  -696,     6,  -696,  -696,  -696,   603,   663,   645,   643,
     585,   -10,  -696,   445,  -696,    47,   715,   139,  -696,  -696,
    -696,  -696,  -696,   659,  -696,   518,   -20,  -696,   603,  -696,
    -696,   642,   647,   512,  -696,  -696,   257,   660,   522,   526,
     449,   722,   727,   730,   736,  -696,  -696,   735,   530,   299,
     535,   537,   724,   724,  -696,    16,   567,   163,  -696,     1,
     769,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,   536,  -696,  -696,  -696,   155,  -696,
    -696,   194,  -696,   215,  -696,  -696,  -696,   219,  -696,   229,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,   749,   747,  -696,
    -696,  -696,  -696,  -696,  -696,   707,   709,   681,   682,   383,
    -696,  -696,  -696,   759,   201,  -696,   758,  -696,  -696,   689,
     247,  -696,   763,   554,   555,   -39,   603,   603,   702,  -696,
     767,   -41,    89,   718,   563,  -696,   193,   564,  -696,    47,
     603,   669,  -696,   354,   566,   569,   409,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,   724,
     573,   793,   710,   603,   603,   135,   385,  -696,  -696,  -696,
    -696,   257,  -696,   780,   575,   578,   579,   587,   794,   795,
     424,   424,  -696,   572,  -696,  -696,  -696,  -696,   583,   -78,
     725,   603,   799,   603,   603,   -18,   591,    21,   724,   724,
     724,   724,   724,   724,   724,   724,   724,   724,   724,   724,
     724,   724,    12,  -696,   594,  -696,   801,  -696,   806,  -696,
     811,  -696,   813,   778,   516,   610,   616,   832,   623,  -696,
     631,  -696,   831,  -696,   333,   840,   688,   692,  -696,  -696,
    -696,   603,   777,   639,  -696,   129,   354,   603,  -696,  -696,
     142,   984,   737,   656,   231,  -696,  -696,  -696,   -41,   860,
    -696,  -696,   867,   603,   655,  -696,   354,  -696,   178,   178,
     603,  -696,   289,   710,   719,   664,     3,   119,   392,  -696,
     603,   603,   802,   603,   870,    24,   603,   665,   306,   613,
    -696,  -696,   670,  -696,  -696,  -696,   728,   671,   724,   567,
     751,  -696,   861,   861,   279,   279,   839,   861,   861,   279,
     279,   424,   424,  -696,  -696,  -696,  -696,  -696,  -696,   677,
    -696,   685,  -696,  -696,  -696,   877,   879,  -696,   715,   891,
    -696,   893,  -696,  -696,   892,  -696,  -696,   896,   897,   690,
      10,   731,   603,  -696,  -696,  -696,   354,   901,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,   696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    -696,  -696,   697,   699,   700,   701,   711,   713,   203,   714,
     715,   883,    89,   326,   708,  -696,   315,   716,   910,   922,
     929,   933,  -696,   928,   320,  -696,   321,   346,  -696,   726,
    -696,   984,   603,  -696,   603,   -25,   149,   724,    98,   721,
    -696,   132,   100,    76,   738,  -696,   935,  -696,  -696,   876,
     567,   861,   743,   351,  -696,   724,   952,   957,   913,   917,
     360,   362,  -696,   762,   371,  -696,   962,  -696,  -696,   -41,
     753,   442,  -696,   274,  -696,   336,   612,  -696,  -696,   965,
     757,   931,  1020,  1037,  1054,  1071,   851,   854,  -696,  -696,
     192,  -696,   852,   715,   376,   771,   853,  -696,   830,  -696,
     603,  -696,  -696,  -696,  -696,  -696,  -696,   178,  -696,  -696,
    -696,   790,   354,    90,  -696,   603,   683,   783,  1001,   594,
     796,   789,   603,  -696,   800,   803,   804,   378,  -696,  -696,
     793,  1002,  1007,  -696,  -696,   891,   521,  -696,   893,   435,
      19,    10,   959,  -696,  -696,  -696,  -696,  -696,  -696,   963,
    -696,  1022,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
     809,   978,   380,   814,   815,   816,   820,   821,   822,   823,
     824,   834,   939,   835,   836,   837,   838,   841,   842,   843,
     844,   845,   846,   966,   872,   880,   882,   886,   887,   898,
     902,   903,   904,   905,  1005,   914,   915,   918,   919,   920,
     921,   932,   936,   937,   938,  1009,   948,   949,   953,   954,
     955,   960,   961,   964,   967,   968,  1023,   969,   970,   971,
     972,   974,   975,   976,   977,   979,   980,  1027,   981,  -696,
    -696,    14,  -696,   924,  1016,   382,  -696,   893,  1148,  1166,
     384,  -696,  -696,  -696,   354,  -696,   630,   982,   983,   985,
      18,   986,  -696,  -696,  -696,  1114,   987,   354,  -696,   178,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    1174,  -696,   274,   442,    10,    10,   991,   336,  1130,  1142,
    -696,  1177,  1199,  1200,  1201,  1202,  1203,  1204,  1205,  1206,
    1207,   997,  1209,  1210,  1211,  1212,  1213,  1214,  1215,  1216,
    1217,  1218,  1008,  1220,  1221,  1222,  1223,  1224,  1225,  1226,
    1227,  1228,  1229,  1019,  1231,  1232,  1233,  1234,  1235,  1236,
    1237,  1238,  1239,  1240,  1030,  1242,  1243,  1244,  1245,  1246,
    1247,  1248,  1249,  1250,  1251,  1041,  1253,  1254,  1255,  1256,
    1257,  1258,  1259,  1260,  1261,  1262,  1052,  1264,  -696,  1267,
    1268,  -696,   386,  -696,   682,  -696,  -696,  1269,    86,  1060,
    1271,  1272,  -696,   388,  1273,  -696,  -696,  1219,   715,  -696,
     603,   603,  -696,  1063,  1064,  1067,  1068,  1069,  1070,  1072,
    1073,  1074,  1075,  1279,  1076,  1077,  1078,  1079,  1080,  1081,
    1082,  1083,  1084,  1085,  1294,  1087,  1088,  1089,  1090,  1091,
    1092,  1093,  1094,  1095,  1096,  1305,  1098,  1099,  1100,  1101,
    1102,  1103,  1104,  1105,  1106,  1107,  1316,  1109,  1110,  1111,
    1112,  1113,  1115,  1116,  1117,  1118,  1119,  1322,  1120,  1121,
    1122,  1123,  1124,  1125,  1126,  1127,  1128,  1129,  1338,  1131,
    -696,  -696,  -696,  -696,  1132,   789,  -696,  1133,  1134,  -696,
     395,   603,   397,   690,   354,  -696,  -696,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  1138,  -696,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  1139,  -696,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  1140,  -696,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  1141,  -696,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  1143,
    -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    1144,  -696,  1342,  1145,  1343,    56,  1146,  1350,  1355,  -696,
    -696,  -696,   354,  -696,  -696,  -696,  -696,  -696,  -696,  -696,
    1147,  -696,  1149,   789,   682,  1356,   592,   196,  1151,  1344,
    1153,  -696,   602,  1357,  -696,   789,   682,   789,   -31,  1362,
    -696,  1321,  1156,  -696,  1157,  1332,  1333,  -696,  -696,  -696,
     -29,  -696,  -696,  1161,  1335,  1336,  -696,  1374,  -696,  1165,
    1167,  1376,   682,  1168,  -696,   682,  -696
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     230,     0,     0,     0,     0,     0,     0,     0,   230,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     230,     0,   504,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   175,   174,     0,     8,    14,    15,    16,    17,
      18,    19,   502,   502,   502,   502,   502,     0,   500,   500,
     500,   500,   500,   223,     0,     0,     0,     0,     0,     0,
     230,   161,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   244,   245,   243,
     249,   253,     0,   250,     0,     0,   246,     0,   248,     0,
     271,   273,     0,   251,     0,   277,     0,     0,     0,     0,
     281,   282,   283,   286,   223,   284,     0,   229,   231,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,   230,     2,   213,   215,   216,     0,   198,   180,   186,
       0,     0,     0,     0,     0,     0,     0,   159,     0,     0,
       0,     0,     0,     0,     0,     0,   208,     0,     0,     0,
       0,     0,     0,   160,     0,   259,   260,   254,   255,     0,
     256,     0,   247,   272,   252,     0,   275,   274,   278,   279,
       0,   305,   303,   304,     0,     0,     0,     0,   329,     0,
     339,     0,   340,     0,   326,   327,     0,   322,   306,     0,
     335,   337,     0,   330,     0,     0,     0,     0,   179,   178,
       4,   214,     0,   176,   177,   197,     0,     0,   194,     0,
      33,     0,    34,   159,   505,     0,     0,   230,   499,   166,
     168,   167,   169,     0,   224,     0,   208,   163,     0,   155,
     498,     0,     0,   433,   437,   440,   441,     0,     0,     0,
       0,     0,     0,     0,     0,   438,   439,     0,     0,     0,
       0,     0,     0,     0,   435,     0,   230,     0,   347,   352,
     353,   367,   365,   368,   366,   369,   370,   362,   357,   356,
     355,   363,   364,   354,   361,   360,   448,   450,     0,   451,
     459,     0,   460,     0,   452,   449,   470,     0,   471,     0,
     447,   290,   292,   291,   288,   289,   295,   297,   296,   293,
     294,   300,   302,   301,   298,   299,   280,     0,     0,   262,
     261,   267,   257,   258,   276,     0,     0,     0,   508,     0,
     232,   287,   332,     0,   323,   328,   307,   336,   331,     0,
       0,   338,     0,     0,     0,   200,     0,     0,   196,   501,
       0,   230,     0,     0,     0,   153,     0,     0,   157,     0,
       0,     0,   162,   207,     0,     0,     0,   479,   478,   481,
     480,   483,   482,   485,   484,   487,   486,   489,   488,     0,
       0,   399,   230,     0,     0,     0,     0,   442,   443,   444,
     445,     0,   446,     0,     0,     0,     0,     0,     0,     0,
     401,   400,   476,   473,   467,   457,   462,   465,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   456,     0,   461,     0,   464,     0,   472,
       0,   475,     0,   268,   263,     0,     0,     0,     0,   285,
       0,   341,     0,   324,     0,     0,     0,     0,   334,   183,
     182,     0,   202,   185,   187,   192,   193,     0,   181,    32,
      36,     0,     0,     0,     0,    41,    45,    46,   230,     0,
      40,   158,     0,     0,   156,   170,   165,   164,     0,     0,
       0,   394,     0,   230,     0,     0,     0,     0,     0,   424,
       0,     0,     0,     0,     0,     0,     0,   206,     0,     0,
     359,   358,     0,   348,   351,   417,   418,     0,     0,   230,
       0,   398,   408,   409,   412,   413,     0,   415,   407,   410,
     411,   403,   402,   404,   405,   406,   434,   436,   458,     0,
     463,     0,   466,   474,   477,     0,     0,   264,     0,     0,
     344,     0,   233,   325,     0,   308,   333,     0,     0,   199,
       0,   204,     0,   190,   191,   189,   195,     0,    51,    54,
      55,    52,    53,    56,    57,    73,    58,    60,    59,    76,
      63,    64,    65,    61,    62,    66,    67,    68,    69,    70,
      71,    72,     0,     0,     0,     0,     0,     0,   508,     0,
       0,   510,     0,    39,     0,   154,     0,     0,     0,     0,
       0,     0,   494,     0,     0,   490,     0,     0,   395,     0,
     429,     0,     0,   422,     0,     0,     0,     0,     0,     0,
     433,     0,     0,     0,     0,   384,     0,   469,   468,     0,
     230,   416,     0,     0,   397,     0,     0,     0,   269,   265,
       0,     0,    43,   513,     0,   511,   309,   342,   343,   230,
     201,   217,   219,   228,   220,     0,   208,   188,    38,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,   147,
     150,   143,   150,     0,     0,     0,    35,    42,   519,   349,
       0,   496,   495,   493,   492,   497,   173,     0,   171,   396,
     430,     0,   426,     0,   425,     0,     0,     0,     0,     0,
       0,   206,     0,   382,     0,     0,     0,     0,   431,   420,
     419,     0,     0,   346,   345,     0,     0,   507,     0,     0,
       0,     0,     0,   237,   238,   239,   240,   236,   241,     0,
     226,     0,   221,   388,   386,   389,   387,   390,   391,   392,
     203,   212,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   148,
     145,     0,   144,    48,    47,     0,   152,     0,     0,     0,
       0,   491,   428,   423,   427,   414,     0,     0,   206,     0,
       0,     0,   453,   455,   454,     0,     0,   205,   385,     0,
     432,   421,   270,   266,    44,   514,   515,   517,   516,   512,
       0,   310,   228,   218,     0,     0,   225,     0,     0,   210,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   149,     0,
       0,   151,     0,    37,   508,   350,   473,     0,     0,     0,
       0,     0,   383,     0,   311,   222,   234,     0,     0,   393,
       0,     0,   184,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    49,   509,   518,     0,   206,   380,     0,   206,   172,
       0,     0,     0,   211,   209,    74,    80,    81,    78,    79,
      82,    83,    84,    85,    86,     0,    77,   124,   125,   122,
     123,   126,   127,   128,   129,   130,     0,   121,    91,    92,
      89,    90,    93,    94,    95,    96,    97,     0,    88,   102,
     103,   100,   101,   104,   105,   106,   107,   108,     0,    99,
     135,   136,   133,   134,   137,   138,   139,   140,   141,     0,
     132,   113,   114,   111,   112,   115,   116,   117,   118,   119,
       0,   110,     0,     0,     0,     0,     0,     0,     0,   313,
     312,   318,   235,   227,    87,   131,    98,   109,   142,   120,
     206,   381,     0,   206,   508,   319,   314,     0,     0,     0,
       0,   379,     0,     0,   315,   206,   508,   206,   508,     0,
     320,   316,     0,   375,     0,     0,     0,   378,   321,   317,
     508,   371,   377,     0,     0,     0,   374,     0,   373,     0,
       0,     0,   508,     0,   376,   508,   372
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -696,  -696,  -696,  1263,  1320,   188,  -696,  -696,   791,  -509,
     774,  -696,   717,   720,  -696,  -530,   199,   200,  1173,  -696,
     206,  -696,  1038,   218,   262,    -7,  1368,   -17,  1086,  1188,
     -67,  -696,  -696,   847,  -696,  -696,  -696,  -696,  -696,  -696,
    -696,  -695,  -219,  -696,  -696,  -696,  -696,   672,  -207,    27,
     539,  -696,  -696,  1230,  -696,  -696,   276,   296,   302,   303,
     308,  -696,  -696,  -204,  -696,   993,  -228,  -227,  -639,  -626,
    -623,  -621,  -620,  -617,   538,  -696,  -696,  -696,  -696,  -696,
    -696,  1021,  -696,  -696,   906,   589,  -250,  -696,  -696,  -696,
     698,  -696,  -696,  -696,  -696,   704,   988,   989,  -277,  -696,
    -696,  -696,  -696,  1150,  -470,   723,  -137,   496,   513,  -696,
    -696,  -584,  -696,   590,   691,  -696
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   464,   641,   465,   466,
     588,   670,   671,   813,   467,   346,    25,    26,   217,    27,
      28,   226,   227,    29,    30,    31,    32,    33,   128,   203,
     129,   208,   453,   454,   555,   338,   458,   206,   452,   551,
     656,   624,   229,   952,   859,   126,   650,   651,   652,   653,
     732,    34,   107,   108,   654,   729,    35,    36,    37,    38,
      39,    40,    41,   257,   474,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   739,   740,   267,   268,   269,   270,
     271,   376,   272,   273,   274,   275,   276,   831,   277,   278,
     279,   280,   281,   282,   283,   284,   396,   397,   285,   286,
     287,   288,   289,   290,   604,   605,   231,   139,   131,   122,
     136,   439,   676,   644,   645,   470
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     353,    68,   335,   119,   672,   395,   836,   352,   640,   606,
     371,   232,   375,    53,   127,   526,   733,   174,   234,   235,
     236,   392,   393,   392,   393,   390,   391,   620,   399,   734,
     642,    54,   735,    56,   736,   737,    17,    47,   738,   123,
     321,   124,   451,   105,   341,   228,   438,   125,   438,   291,
      53,   292,   293,    68,  -503,   402,   507,   611,   694,   204,
     674,     1,  1123,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   147,   148,   137,   403,   404,    11,    12,    13,
     701,   109,   146,    14,    15,    16,   296,    96,   297,   298,
    1025,    60,   461,   510,  1145,     1,  1154,     2,     3,     4,
       5,     6,     7,   156,     9,    57,    58,   294,   455,   456,
      59,    11,    12,    13,   241,   242,   243,    14,    15,    16,
     244,   702,   476,   170,   171,   172,   173,   168,   403,   404,
     169,    17,  1146,   939,  1155,   334,   501,   301,   508,   302,
     303,   702,   371,   815,   299,   486,   487,   245,   246,   247,
     511,   702,   482,    55,   403,   404,   403,   404,    97,   211,
     212,   213,   123,   315,   124,    17,   120,   220,   221,   222,
     125,   144,    20,   823,   528,   505,   506,   347,   401,   316,
     317,   512,   513,   514,   515,   516,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   304,    62,   351,   553,   554,
     348,   318,  1135,   342,   110,   612,   844,    63,    64,   111,
     149,   112,   295,   113,    65,   462,    17,   463,   733,   202,
     527,    18,   374,   649,    98,   255,    66,   255,   394,   556,
     394,   734,   254,   852,   735,   695,   736,   737,    19,   398,
     738,   442,   344,   403,   404,   598,   599,   549,    62,   300,
     443,   403,   404,   403,   404,    18,   600,   601,   602,    63,
      64,   702,   615,   616,   666,   618,    65,    20,   622,   596,
      67,    99,   403,   404,   557,   666,   607,   730,    66,   121,
     438,   631,   403,   404,    69,   103,   180,   181,   403,   404,
       1,   182,     2,     3,     4,     5,     6,     7,   104,     9,
     305,    20,   403,   404,    70,   633,    11,    12,    13,  -506,
      71,    72,    14,    15,    16,   697,    73,   700,   667,   130,
     668,   669,    67,   811,   455,   100,   101,   102,   731,   667,
    1103,   668,   669,  1106,   460,     1,    69,     2,     3,     4,
       5,     6,     7,     8,     9,    10,   123,   422,   124,   699,
    1023,    11,    12,    13,   125,   485,    70,    14,    15,    16,
      17,   603,    71,    72,   114,   629,   544,   423,    73,   943,
     307,   106,   424,   308,   309,   545,   475,   400,   310,   311,
     401,    42,    43,    44,   692,   115,   693,    45,    46,   116,
     696,   127,   117,   357,   358,   359,   360,   361,   362,   363,
     364,   365,   366,   367,   368,    17,   425,   471,   710,   407,
     472,   426,   233,   234,   235,   236,   130,   384,  1032,   385,
     827,   386,   387,   834,   138,  1128,   707,   427,  1130,  -507,
    -507,   429,   428,   446,   447,  1107,   430,   741,  1108,  1109,
    1142,   431,  1144,  1110,  1111,   591,   432,   144,   592,   829,
      18,   145,   233,   234,   235,   236,    48,    49,    50,   158,
     159,   593,    51,    52,   160,   161,   609,   824,   489,   850,
     490,   851,   491,   480,   837,   613,   820,   614,   150,   491,
     151,   237,   238,  -507,  -507,   417,   418,   419,   420,   421,
     152,   239,   632,   240,   154,    18,    20,   722,  -242,   723,
     724,   725,   726,   608,   727,   728,   401,   403,   404,   241,
     242,   243,    19,   155,   249,   244,   250,   157,   251,   175,
     625,   237,   238,   626,   845,   846,   847,   848,   162,   679,
     163,   239,   401,   240,   686,   688,   374,   687,   687,   164,
    1131,    20,   245,   246,   247,   140,   141,   142,   143,   241,
     242,   243,  1143,   177,  1147,   244,   132,   133,   134,   135,
     689,   536,   537,   401,   248,   709,  1156,   176,   401,   195,
     233,   234,   235,   236,   713,   197,   714,   472,  1164,   715,
     178,  1166,   245,   246,   247,   717,   179,   249,   718,   250,
     816,   251,   841,   472,   860,   401,   931,   861,   935,   472,
    1022,   401,  1029,   718,   248,   687,   233,   234,   235,   236,
     194,  1113,   196,   706,   472,   252,   253,   254,   627,   628,
     255,   198,   256,   481,   165,   166,   167,   249,   199,   250,
     201,   251,   419,   420,   421,   392,   936,  1133,  1134,   237,
     238,   205,   720,   209,    17,  1139,  1140,   946,   947,   239,
     207,   240,   210,   214,   215,   252,   253,   254,   216,   218,
     255,   219,   256,   233,   234,   235,   236,   241,   242,   243,
     223,   224,   225,   244,   230,   237,   238,   228,   183,   312,
     306,   313,   319,   314,   322,   239,   323,   240,   184,   325,
     324,   185,   186,   326,   187,   188,   189,   327,   328,   329,
     245,   246,   247,   241,   242,   243,   330,   331,   332,   244,
     190,   191,   337,   192,   193,   336,   339,   340,   345,   354,
     349,   350,   248,  1034,   355,   356,   377,   233,   234,   235,
     236,   378,   369,   370,   379,   372,   245,   246,   247,   373,
     380,   381,   239,   383,   240,   249,  1033,   250,   388,   251,
     389,   422,   433,   434,   435,   484,   436,   437,   248,   438,
     241,   242,   243,   441,   444,   445,   244,   448,   449,   450,
     457,   459,   468,   252,   253,   254,   469,   473,   255,   478,
     256,   249,   479,   250,   492,   251,   483,    17,   493,    74,
     499,   494,   495,   245,   246,   247,   369,   500,   497,   498,
     496,   502,   504,  1112,   509,   255,   239,   529,   240,   252,
     253,   254,   531,   407,   255,   248,   256,   533,   534,    75,
      76,   535,    77,   538,   241,   242,   243,    78,    79,   539,
     244,   408,   409,   410,   411,   540,   541,   543,   249,   413,
     250,   405,   251,   406,   546,   542,   743,   744,   745,   746,
     747,   547,   550,   748,   749,   548,   552,   245,   246,   247,
     750,   751,   752,   594,   589,   484,   252,   253,   254,   590,
     595,   255,   597,   256,   619,   508,   753,   617,   610,   248,
     634,   403,   623,   638,   630,   639,   414,   415,   416,   417,
     418,   419,   420,   421,   461,   636,   643,   825,   646,   407,
     647,   648,   249,   637,   250,   658,   251,   401,   655,   659,
     660,   484,   661,   662,   663,   675,   681,   408,   409,   410,
     411,   412,   678,   407,   664,   413,   665,   673,   682,   680,
     252,   253,   254,   683,   685,   255,   684,   256,   698,   704,
     690,   408,   409,   410,   411,    80,    81,    82,    83,   413,
      84,    85,   703,   705,    86,    87,    88,   708,   628,    89,
      90,    91,   627,   711,   712,   716,    92,    93,   719,   407,
     721,   742,   414,   415,   416,   417,   418,   419,   420,   421,
     809,   810,    94,   811,   817,   818,    95,   408,   409,   410,
     411,   407,   635,   819,   826,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   822,   828,   835,   830,   842,  -507,
    -507,   410,   411,   843,   838,   854,   839,  -507,   840,   855,
     754,   755,   756,   757,   758,   856,   857,   759,   760,   858,
     871,   862,   863,   864,   761,   762,   763,   865,   866,   867,
     868,   869,   414,   415,   416,   417,   418,   419,   420,   421,
     764,   870,   872,   873,   874,   875,   929,   882,   876,   877,
     878,   879,   880,   881,  -507,   415,   416,   417,   418,   419,
     420,   421,   558,   559,   560,   561,   562,   563,   564,   565,
     566,   567,   568,   569,   570,   571,   572,   573,   574,   883,
     575,   576,   577,   578,   579,   580,   893,   884,   581,   885,
     904,   582,   583,   886,   887,   584,   585,   586,   587,   765,
     766,   767,   768,   769,   915,   888,   770,   771,   926,   889,
     890,   891,   892,   772,   773,   774,   776,   777,   778,   779,
     780,   894,   895,   781,   782,   896,   897,   898,   899,   775,
     783,   784,   785,   787,   788,   789,   790,   791,   930,   900,
     792,   793,   933,   901,   902,   903,   786,   794,   795,   796,
     798,   799,   800,   801,   802,   905,   906,   803,   804,   934,
     907,   908,   909,   797,   805,   806,   807,   910,   911,   702,
     944,   912,   950,   953,   913,   914,   916,   917,   918,   919,
     808,   920,   921,   922,   923,   951,   924,   925,   927,   937,
     938,   942,   940,   941,   948,   954,   955,   956,   957,   958,
     959,   960,   961,   962,   963,   964,   965,   966,   967,   968,
     969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
     979,   980,   981,   982,   983,   984,   985,   986,   987,   988,
     989,   990,   991,   992,   993,   994,   995,   996,   997,   998,
     999,  1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,
    1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1024,  1026,  1027,  1028,  1035,  1036,  1030,
    1031,  1037,  1038,  1039,  1040,  1045,  1041,  1042,  1043,  1044,
    1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,  1055,
    1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,  1065,
    1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,  1075,
    1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1089,  1084,
    1085,  1086,  1087,  1088,  1090,  1091,  1092,  1093,  1094,  1095,
    1096,  1097,  1098,  1099,  1100,  1101,  1120,  1122,  1137,  1102,
    1104,  1105,  1114,  1115,  1116,  1117,  1125,  1118,  1119,  1121,
    1124,  1126,  1132,  1141,  1127,  1136,  1129,  1138,  1148,  1149,
    1150,  1151,  1152,  1153,  1157,  1158,  1159,  1160,  1161,  1163,
     153,  1162,  1165,   677,   200,   691,   343,   812,   118,   477,
     333,   945,   814,   853,   503,   949,   488,   382,   832,   657,
     928,   621,     0,     0,   833,   440,   320,   932,     0,   849,
     821,     0,     0,     0,   530,     0,     0,   532
};

static const yytype_int16 yycheck[] =
{
     228,     8,   206,    20,   588,   255,   701,   226,   538,   479,
     237,   148,   240,     3,     8,     3,   655,    61,     4,     5,
       6,     5,     6,     5,     6,   252,   253,     3,   256,   655,
     539,     4,   655,     6,   655,   655,    77,    37,   655,    20,
     177,    22,    81,    16,    54,    65,    77,    28,    77,     3,
       3,     5,     6,    60,     0,    54,    74,    54,    83,   126,
     590,     7,     6,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    75,    76,    47,   153,   154,    23,    24,    25,
       4,   170,    55,    29,    30,    31,     3,    34,     5,     6,
       4,     3,     3,    72,   125,     7,   125,     9,    10,    11,
      12,    13,    14,    76,    16,   164,   165,    61,   336,   337,
     169,    23,    24,    25,   100,   101,   102,    29,    30,    31,
     106,    65,   350,    96,    97,    98,    99,   198,   153,   154,
     201,    77,   163,   828,   163,   202,   214,     3,   156,     5,
       6,    65,   369,   673,    61,   373,   374,   133,   134,   135,
     129,    65,   356,    75,   153,   154,   153,   154,    34,   132,
     133,   134,    20,    10,    22,    77,     0,   140,   141,   142,
      28,   215,   213,    83,   424,   403,   404,    38,   217,    26,
      27,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   421,    61,     8,   217,    69,    70,
     217,   174,     6,   213,     7,    86,   715,     8,     8,    12,
     213,    14,   166,    16,     8,   126,    77,   128,   857,   213,
     208,   167,    87,   213,    34,   211,     8,   211,   212,   457,
     212,   857,   208,   214,   857,    86,   857,   857,   184,   256,
     857,    40,   215,   153,   154,    67,    68,   451,    60,   166,
      49,   153,   154,   153,   154,   167,    78,    79,    80,    60,
      60,    65,   490,   491,    72,   493,    60,   213,   496,   473,
       8,    34,   153,   154,   132,    72,   480,     3,    60,   216,
      77,   508,   153,   154,     8,     3,   184,   185,   153,   154,
       7,   189,     9,    10,    11,    12,    13,    14,     3,    16,
     166,   213,   153,   154,     8,   509,    23,    24,    25,    61,
       8,     8,    29,    30,    31,   217,     8,   217,   126,    71,
     128,   129,    60,   131,   552,   157,   158,   159,    54,   126,
    1025,   128,   129,  1028,   341,     7,    60,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    20,   215,    22,   217,
     934,    23,    24,    25,    28,   372,    60,    29,    30,    31,
      77,   183,    60,    60,   167,   502,    33,   212,    60,   839,
      40,     3,   217,    43,    44,    42,   349,   214,    48,    49,
     217,    33,    34,    35,   612,   188,   614,    39,    40,   192,
     617,     8,   195,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,    77,   212,   214,   635,   130,
     217,   217,     3,     4,     5,     6,    71,   118,   948,   120,
     697,   122,   123,   700,    71,  1120,   630,   212,  1123,   150,
     151,   212,   217,   186,   187,    40,   217,   656,    43,    44,
    1135,   212,  1137,    48,    49,   214,   217,   215,   217,   699,
     167,    14,     3,     4,     5,     6,    33,    34,    35,   171,
     172,   468,    39,    40,   171,   172,   483,   695,    83,    34,
      85,    36,    87,    64,   702,    83,   680,    85,     3,    87,
       3,    72,    73,   204,   205,   206,   207,   208,   209,   210,
       3,    82,   509,    84,   213,   167,   213,    55,    56,    57,
      58,    59,    60,   214,    62,    63,   217,   153,   154,   100,
     101,   102,   184,     3,   178,   106,   180,     6,   182,    54,
     214,    72,    73,   217,     3,     4,     5,     6,     3,   214,
     174,    82,   217,    84,   214,   214,    87,   217,   217,     6,
    1124,   213,   133,   134,   135,    49,    50,    51,    52,   100,
     101,   102,  1136,     6,  1138,   106,    43,    44,    45,    46,
     214,    45,    46,   217,   155,   214,  1150,   217,   217,     4,
       3,     4,     5,     6,   214,    75,   214,   217,  1162,   217,
     193,  1165,   133,   134,   135,   214,   193,   178,   217,   180,
     214,   182,   214,   217,   214,   217,   214,   217,   214,   217,
     214,   217,   214,   217,   155,   217,     3,     4,     5,     6,
     193,   214,   191,   630,   217,   206,   207,   208,     5,     6,
     211,   214,   213,   214,   197,   198,   199,   178,   214,   180,
      21,   182,   208,   209,   210,     5,     6,    45,    46,    72,
      73,    64,   649,    72,    77,    43,    44,   854,   855,    82,
      66,    84,     3,     3,    61,   206,   207,   208,   213,    73,
     211,     3,   213,     3,     4,     5,     6,   100,   101,   102,
       3,     3,     3,   106,     4,    72,    73,    65,   158,     3,
     214,     3,   213,     4,     4,    82,   163,    84,   168,     3,
       6,   171,   172,     6,   174,   175,   176,     4,     4,     4,
     133,   134,   135,   100,   101,   102,    54,     4,   193,   106,
     190,   191,    67,   193,   194,    52,    73,   132,     3,    77,
      61,   203,   155,   951,    77,   213,     4,     3,     4,     5,
       6,     4,    72,    73,     4,   213,   133,   134,   135,   213,
       4,     6,    82,   213,    84,   178,   950,   180,   213,   182,
     213,   215,     3,     6,    47,    72,    47,    76,   155,    77,
     100,   101,   102,     4,     6,    76,   106,     4,   214,   214,
      68,     4,    54,   206,   207,   208,   213,   213,   211,   213,
     213,   178,   213,   180,     4,   182,   213,    77,   213,     3,
     218,   213,   213,   133,   134,   135,    72,   214,     4,     4,
     213,    76,     3,  1031,   213,   211,    82,     6,    84,   206,
     207,   208,     6,   130,   211,   155,   213,     6,     5,    33,
      34,    43,    36,   213,   100,   101,   102,    41,    42,   213,
     106,   148,   149,   150,   151,     3,   213,     6,   178,   156,
     180,    72,   182,    74,     4,   214,    89,    90,    91,    92,
      93,   163,    75,    96,    97,   163,   217,   133,   134,   135,
     103,   104,   105,     3,   127,    72,   206,   207,   208,   213,
       3,   211,   217,   213,     4,   156,   119,    75,   214,   155,
     129,   153,   217,     6,   213,     6,   203,   204,   205,   206,
     207,   208,   209,   210,     3,   218,     3,   214,     6,   130,
       4,     4,   178,   218,   180,     4,   182,   217,   177,   213,
     213,    72,   213,   213,   213,    32,     6,   148,   149,   150,
     151,   152,   214,   130,   213,   156,   213,   213,     6,   213,
     206,   207,   208,     4,     6,   211,     3,   213,   217,     4,
     214,   148,   149,   150,   151,   159,   160,   161,   162,   156,
     164,   165,   214,    77,   168,   169,   170,   214,     6,   173,
     174,   175,     5,    50,    47,   203,   180,   181,     6,   130,
     217,     6,   203,   204,   205,   206,   207,   208,   209,   210,
     129,   127,   196,   131,   213,   132,   200,   148,   149,   150,
     151,   130,   153,   163,   211,   156,   203,   204,   205,   206,
     207,   208,   209,   210,   214,     4,   217,   211,     6,   148,
     149,   150,   151,     6,   214,    56,   213,   156,   214,    56,
      89,    90,    91,    92,    93,     3,   217,    96,    97,    51,
      91,   217,   217,   217,   103,   104,   105,   217,   217,   217,
     217,   217,   203,   204,   205,   206,   207,   208,   209,   210,
     119,   217,   217,   217,   217,   217,   132,    91,   217,   217,
     217,   217,   217,   217,   203,   204,   205,   206,   207,   208,
     209,   210,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   217,
     106,   107,   108,   109,   110,   111,    91,   217,   114,   217,
      91,   117,   118,   217,   217,   121,   122,   123,   124,    89,
      90,    91,    92,    93,    91,   217,    96,    97,    91,   217,
     217,   217,   217,   103,   104,   105,    89,    90,    91,    92,
      93,   217,   217,    96,    97,   217,   217,   217,   217,   119,
     103,   104,   105,    89,    90,    91,    92,    93,   132,   217,
      96,    97,     4,   217,   217,   217,   119,   103,   104,   105,
      89,    90,    91,    92,    93,   217,   217,    96,    97,     3,
     217,   217,   217,   119,   103,   104,   105,   217,   217,    65,
       6,   217,    52,     6,   217,   217,   217,   217,   217,   217,
     119,   217,   217,   217,   217,    53,   217,   217,   217,   217,
     217,   214,   217,   217,   213,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   217,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   217,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   217,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   217,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   217,
       6,     4,     4,     4,   214,     4,     4,   214,   214,     6,
      61,   214,   214,   214,   214,     6,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
       6,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,     6,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,     6,   214,   214,   214,   214,   214,     6,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,     6,   214,     4,     4,     4,   217,
     217,   217,   214,   214,   214,   214,     6,   214,   214,   214,
     214,     6,     6,     6,   217,   214,   217,   214,     6,    48,
     214,   214,    40,    40,   213,    40,    40,     3,   213,     3,
      60,   214,   214,   592,   121,   611,   213,   670,    20,   351,
     202,   852,   672,   721,   401,   857,   375,   247,   700,   552,
     811,   495,    -1,    -1,   700,   319,   176,   817,    -1,   718,
     687,    -1,    -1,    -1,   426,    -1,    -1,   428
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
     213,   214,   217,   244,     3,     3,   282,   217,    67,    68,
      78,    79,    80,   183,   323,   324,   323,   282,   214,   246,
     214,    54,    86,    83,    85,   285,   285,    75,   285,     4,
       3,   303,   285,   217,   260,   214,   217,     5,     6,   325,
     213,   286,   246,   282,   129,   153,   218,   218,     6,     6,
     234,   226,   228,     3,   332,   333,     6,     4,     4,   213,
     265,   266,   267,   268,   273,   177,   259,   252,     4,   213,
     213,   213,   213,   213,   213,   213,    72,   126,   128,   129,
     230,   231,   330,   213,   234,    32,   331,   227,   214,   214,
     213,     6,     6,     4,     3,     6,   214,   217,   214,   214,
     214,   229,   285,   285,    83,    86,   286,   217,   217,   217,
     217,     4,    65,   214,     4,    77,   246,   282,   214,   214,
     286,    50,    47,   214,   214,   217,   203,   214,   217,     6,
     244,   217,    55,    57,    58,    59,    60,    62,    63,   274,
       3,    54,   269,   287,   288,   289,   290,   291,   292,   293,
     294,   261,     6,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   119,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   119,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   119,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   119,    89,    90,    91,
      92,    93,    96,    97,   103,   104,   105,   119,    89,    90,
      91,    92,    93,    96,    97,   103,   104,   105,   119,   129,
     127,   131,   231,   232,   232,   234,   214,   213,   132,   163,
     282,   324,   214,    83,   285,   214,   211,   317,     4,   305,
     211,   306,   309,   314,   317,   217,   260,   285,   214,   213,
     214,   214,     6,     6,   228,     3,     4,     5,     6,   333,
      34,    36,   214,   266,    56,    56,     3,   217,    51,   263,
     214,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,    91,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,    91,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,    91,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,    91,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,    91,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,    91,   217,   304,   132,
     132,   214,   332,     4,     3,   214,     6,   217,   217,   260,
     217,   217,   214,   323,     6,   269,   267,   267,   213,   293,
      52,    53,   262,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   217,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   217,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   217,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   217,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   217,     6,
       4,     4,   214,   330,     4,     4,   214,     4,     4,   214,
       6,    61,   234,   282,   285,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,     6,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,     6,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,     6,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,     6,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,     6,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
       6,   214,   217,   260,   217,   217,   260,    40,    43,    44,
      48,    49,   285,   214,   214,   214,   214,   214,   214,   214,
       4,   214,     4,     6,   214,     6,     6,   217,   260,   217,
     260,   330,     6,    45,    46,     6,   214,     4,   214,    43,
      44,     6,   260,   330,   260,   125,   163,   330,     6,    48,
     214,   214,    40,    40,   125,   163,   330,   213,    40,    40,
       3,   213,   214,     3,   330,   214,   330
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   219,   220,   221,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   225,   225,   226,   226,   227,   227,   228,   228,   228,
     228,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   230,   230,   231,   231,   231,   231,   232,
     232,   233,   233,   234,   234,   235,   236,   236,   237,   237,
     238,   238,   239,   240,   240,   241,   242,   242,   242,   242,
     242,   243,   243,   243,   244,   244,   244,   244,   245,   245,
     246,   247,   248,   248,   249,   250,   250,   251,   251,   252,
     253,   253,   253,   254,   254,   255,   255,   256,   256,   257,
     257,   258,   258,   259,   259,   260,   260,   261,   261,   262,
     262,   263,   263,   264,   264,   264,   264,   265,   265,   266,
     266,   267,   267,   268,   268,   269,   269,   269,   269,   270,
     270,   271,   271,   272,   273,   273,   274,   274,   274,   274,
     274,   274,   274,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   276,   276,   276,   277,   277,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   279,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   281,   281,   281,   282,   282,   283,
     283,   284,   284,   285,   285,   285,   285,   285,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   287,   288,   288,   288,   288,   289,   289,   289,   289,
     290,   290,   291,   291,   292,   292,   293,   293,   293,   293,
     293,   293,   294,   294,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   296,   296,   297,
     298,   298,   299,   299,   299,   299,   300,   300,   301,   302,
     302,   302,   302,   303,   303,   303,   303,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   305,
     305,   305,   305,   306,   306,   306,   307,   308,   308,   309,
     309,   310,   311,   311,   312,   313,   313,   314,   315,   316,
     317,   317,   318,   319,   319,   320,   321,   321,   322,   322,
     322,   322,   322,   322,   322,   322,   322,   322,   322,   322,
     323,   323,   324,   324,   324,   324,   324,   324,   325,   326,
     326,   327,   327,   328,   328,   329,   329,   330,   330,   331,
     331,   332,   332,   333,   333,   333,   333,   333,   334,   334
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     4,     4,     8,     6,    10,     8,     7,
       6,     1,     3,     1,     3,     1,     1,     4,     4,     6,
       6,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     4,     1,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     1,     2,     2,     1,     1,     2,     2,
       0,     5,     4,     1,     3,     4,     6,     5,     3,     0,
       3,     2,     5,     1,     3,     3,     4,     4,     4,     4,
       6,     8,    11,     8,     1,     1,     3,     3,     3,     3,
       2,     4,     3,     3,     9,     3,     0,     1,     3,     2,
       1,     1,     0,     2,     0,     2,     0,     1,     0,     2,
       0,     2,     0,     2,     0,     3,     0,     2,     0,     2,
       0,     3,     0,     1,     2,     1,     1,     1,     3,     1,
       1,     2,     4,     1,     3,     2,     1,     5,     0,     2,
       0,     1,     3,     5,     4,     6,     1,     1,     1,     1,
       1,     1,     0,     2,     2,     2,     2,     3,     2,     2,
       2,     2,     3,     2,     3,     3,     3,     4,     4,     3,
       3,     4,     4,     5,     6,     7,     9,     4,     5,     7,
       9,     2,     3,     2,     3,     3,     4,     2,     3,     3,
       4,     2,     2,     2,     2,     5,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     3,     3,     3,     3,     4,     6,     7,
       9,    10,    12,    12,    13,    14,    15,    16,    12,    13,
      15,    16,     3,     4,     5,     6,     3,     3,     4,     3,
       3,     4,     4,     6,     5,     3,     4,     3,     4,     3,
       3,     5,     7,     7,     6,     8,     8,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,    14,    20,    16,    15,    13,    18,    14,    13,    11,
       8,    10,     5,     7,     4,     6,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     4,     5,     4,     3,     2,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     6,     3,     4,     3,     3,     5,
       5,     6,     4,     6,     3,     5,     4,     5,     6,     4,
       5,     5,     6,     1,     3,     1,     3,     1,     1,     1,
       1,     1,     2,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     3,     1,
       1,     2,     2,     3,     2,     2,     3,     2,     3,     3,
       1,     1,     2,     2,     3,     2,     2,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     3,     2,     2,     1,     2,     2,     2,     1,     2,
       0,     3,     0,     1,     0,     2,     0,     4,     0,     4,
       0,     1,     3,     1,     3,     3,     3,     3,     6,     3
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
#line 2398 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2406 "parser.cpp"
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
#line 2420 "parser.cpp"
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
#line 2434 "parser.cpp"
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
#line 2448 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2459 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2467 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2476 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2485 "parser.cpp"
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
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2510 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2520 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2530 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2540 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2550 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2570 "parser.cpp"
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
#line 2584 "parser.cpp"
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
#line 2598 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2616 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2624 "parser.cpp"
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
#line 2638 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2647 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2655 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2663 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2671 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2679 "parser.cpp"
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
#line 2693 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2702 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2711 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2720 "parser.cpp"
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
#line 2733 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2742 "parser.cpp"
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
#line 2756 "parser.cpp"
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
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2780 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2789 "parser.cpp"
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
#line 2803 "parser.cpp"
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
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2892 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2900 "parser.cpp"
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
#line 2914 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2922 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2930 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2938 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2946 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2954 "parser.cpp"
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
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3007 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3015 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3023 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3055 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3063 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3071 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3079 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3151 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3162 "parser.cpp"
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
#line 3176 "parser.cpp"
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
#line 3190 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3201 "parser.cpp"
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

#line 3309 "parser.cpp"

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
#line 3524 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 510 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3535 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 516 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3546 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 523 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3552 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3558 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3564 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3570 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3576 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3582 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3588 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3594 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3600 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 532 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3606 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 533 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3612 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3618 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3624 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3630 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3636 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 539 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3642 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3648 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3654 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3660 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3666 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3672 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3678 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3684 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3690 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3696 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3702 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3708 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 557 "parser.y"
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
#line 3730 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 574 "parser.y"
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
#line 3750 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 591 "parser.y"
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
#line 3768 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 607 "parser.y"
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
#line 3801 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 636 "parser.y"
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
#line 3821 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 651 "parser.y"
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
#line 3857 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 683 "parser.y"
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
#line 3879 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 701 "parser.y"
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
#line 3900 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 719 "parser.y"
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
#line 3933 "parser.cpp"
    break;

  case 41: /* table_element_array: table_element  */
#line 748 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3942 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element_array ',' table_element  */
#line 752 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3951 "parser.cpp"
    break;

  case 43: /* column_def_array: table_column  */
#line 757 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3960 "parser.cpp"
    break;

  case 44: /* column_def_array: column_def_array ',' table_column  */
#line 761 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3969 "parser.cpp"
    break;

  case 45: /* table_element: table_column  */
#line 767 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3977 "parser.cpp"
    break;

  case 46: /* table_element: table_constraint  */
#line 770 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3985 "parser.cpp"
    break;

  case 47: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 777 "parser.y"
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
#line 4041 "parser.cpp"
    break;

  case 48: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 828 "parser.y"
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
#line 4083 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 865 "parser.y"
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
#line 4140 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 917 "parser.y"
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
#line 4183 "parser.cpp"
    break;

  case 51: /* column_type: BOOLEAN  */
#line 957 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4189 "parser.cpp"
    break;

  case 52: /* column_type: TINYINT  */
#line 958 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4195 "parser.cpp"
    break;

  case 53: /* column_type: SMALLINT  */
#line 959 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4201 "parser.cpp"
    break;

  case 54: /* column_type: INTEGER  */
#line 960 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4207 "parser.cpp"
    break;

  case 55: /* column_type: INT  */
#line 961 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4213 "parser.cpp"
    break;

  case 56: /* column_type: BIGINT  */
#line 962 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4219 "parser.cpp"
    break;

  case 57: /* column_type: HUGEINT  */
#line 963 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4225 "parser.cpp"
    break;

  case 58: /* column_type: FLOAT  */
#line 964 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4231 "parser.cpp"
    break;

  case 59: /* column_type: REAL  */
#line 965 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4237 "parser.cpp"
    break;

  case 60: /* column_type: DOUBLE  */
#line 966 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4243 "parser.cpp"
    break;

  case 61: /* column_type: FLOAT16  */
#line 967 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4249 "parser.cpp"
    break;

  case 62: /* column_type: BFLOAT16  */
#line 968 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4255 "parser.cpp"
    break;

  case 63: /* column_type: DATE  */
#line 969 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4261 "parser.cpp"
    break;

  case 64: /* column_type: TIME  */
#line 970 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4267 "parser.cpp"
    break;

  case 65: /* column_type: DATETIME  */
#line 971 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4273 "parser.cpp"
    break;

  case 66: /* column_type: TIMESTAMP  */
#line 972 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4279 "parser.cpp"
    break;

  case 67: /* column_type: UUID  */
#line 973 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4285 "parser.cpp"
    break;

  case 68: /* column_type: POINT  */
#line 974 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4291 "parser.cpp"
    break;

  case 69: /* column_type: LINE  */
#line 975 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4297 "parser.cpp"
    break;

  case 70: /* column_type: LSEG  */
#line 976 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4303 "parser.cpp"
    break;

  case 71: /* column_type: BOX  */
#line 977 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4309 "parser.cpp"
    break;

  case 72: /* column_type: CIRCLE  */
#line 980 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4315 "parser.cpp"
    break;

  case 73: /* column_type: VARCHAR  */
#line 982 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4321 "parser.cpp"
    break;

  case 74: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4327 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 984 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4333 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL  */
#line 985 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4339 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4345 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4351 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4357 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4363 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4369 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4375 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4381 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4387 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4393 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4399 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4405 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4411 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4417 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4423 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4429 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4435 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4441 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4447 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4453 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4459 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4465 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4471 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4477 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4483 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4489 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4495 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4501 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4507 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4513 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4519 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4525 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4531 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4537 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4543 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4549 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4555 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4561 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4567 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4573 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4579 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4585 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4591 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4597 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4603 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4609 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4615 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4621 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4627 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4633 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4639 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4645 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4651 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4657 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4663 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4669 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4675 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4681 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4687 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4693 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4699 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4705 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4711 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4717 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4723 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4729 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4735 "parser.cpp"
    break;

  case 143: /* column_constraints: column_constraint  */
#line 1072 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4744 "parser.cpp"
    break;

  case 144: /* column_constraints: column_constraints column_constraint  */
#line 1076 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4758 "parser.cpp"
    break;

  case 145: /* column_constraint: PRIMARY KEY  */
#line 1086 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4766 "parser.cpp"
    break;

  case 146: /* column_constraint: UNIQUE  */
#line 1089 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4774 "parser.cpp"
    break;

  case 147: /* column_constraint: NULLABLE  */
#line 1092 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4782 "parser.cpp"
    break;

  case 148: /* column_constraint: NOT NULLABLE  */
#line 1095 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4790 "parser.cpp"
    break;

  case 149: /* default_expr: DEFAULT constant_expr  */
#line 1099 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4798 "parser.cpp"
    break;

  case 150: /* default_expr: %empty  */
#line 1102 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4806 "parser.cpp"
    break;

  case 151: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1107 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4816 "parser.cpp"
    break;

  case 152: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1112 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4826 "parser.cpp"
    break;

  case 153: /* identifier_array: IDENTIFIER  */
#line 1119 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4837 "parser.cpp"
    break;

  case 154: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1125 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4848 "parser.cpp"
    break;

  case 155: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1135 "parser.y"
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
#line 4865 "parser.cpp"
    break;

  case 156: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 1151 "parser.y"
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
#line 4904 "parser.cpp"
    break;

  case 157: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1185 "parser.y"
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
#line 4921 "parser.cpp"
    break;

  case 158: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1198 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4929 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: %empty  */
#line 1201 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4937 "parser.cpp"
    break;

  case 160: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1208 "parser.y"
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
#line 4955 "parser.cpp"
    break;

  case 161: /* explain_statement: EXPLAIN explainable_statement  */
#line 1220 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4965 "parser.cpp"
    break;

  case 162: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1229 "parser.y"
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
#line 4982 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr  */
#line 1242 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4991 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1246 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5000 "parser.cpp"
    break;

  case 165: /* update_expr: IDENTIFIER '=' expr  */
#line 1251 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5012 "parser.cpp"
    break;

  case 166: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1264 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5028 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1277 "parser.y"
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
#line 5046 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1292 "parser.y"
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
#line 5064 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1307 "parser.y"
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
#line 5082 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1322 "parser.y"
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
#line 5105 "parser.cpp"
    break;

  case 171: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1345 "parser.y"
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
#line 5163 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1398 "parser.y"
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
#line 5223 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1453 "parser.y"
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
#line 5275 "parser.cpp"
    break;

  case 174: /* select_statement: select_without_paren  */
#line 1504 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5283 "parser.cpp"
    break;

  case 175: /* select_statement: select_with_paren  */
#line 1507 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5291 "parser.cpp"
    break;

  case 176: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1510 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5305 "parser.cpp"
    break;

  case 177: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1519 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5319 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_without_paren ')'  */
#line 1529 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5327 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_with_paren ')'  */
#line 1532 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5335 "parser.cpp"
    break;

  case 180: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1536 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5344 "parser.cpp"
    break;

  case 181: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1541 "parser.y"
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
#line 5375 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1568 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5383 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1571 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5391 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1576 "parser.y"
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
#line 5412 "parser.cpp"
    break;

  case 185: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1593 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5420 "parser.cpp"
    break;

  case 186: /* order_by_clause: %empty  */
#line 1596 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5428 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr  */
#line 1600 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5437 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1604 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5446 "parser.cpp"
    break;

  case 189: /* order_by_expr: expr order_by_type  */
#line 1609 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5456 "parser.cpp"
    break;

  case 190: /* order_by_type: ASC  */
#line 1615 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5464 "parser.cpp"
    break;

  case 191: /* order_by_type: DESC  */
#line 1618 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5472 "parser.cpp"
    break;

  case 192: /* order_by_type: %empty  */
#line 1621 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5480 "parser.cpp"
    break;

  case 193: /* limit_expr: LIMIT expr  */
#line 1625 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5488 "parser.cpp"
    break;

  case 194: /* limit_expr: %empty  */
#line 1629 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5494 "parser.cpp"
    break;

  case 195: /* offset_expr: OFFSET expr  */
#line 1631 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5502 "parser.cpp"
    break;

  case 196: /* offset_expr: %empty  */
#line 1635 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5508 "parser.cpp"
    break;

  case 197: /* distinct: DISTINCT  */
#line 1637 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5516 "parser.cpp"
    break;

  case 198: /* distinct: %empty  */
#line 1640 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5524 "parser.cpp"
    break;

  case 199: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1644 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5532 "parser.cpp"
    break;

  case 200: /* highlight_clause: %empty  */
#line 1647 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5540 "parser.cpp"
    break;

  case 201: /* from_clause: FROM table_reference  */
#line 1651 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5548 "parser.cpp"
    break;

  case 202: /* from_clause: %empty  */
#line 1654 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5556 "parser.cpp"
    break;

  case 203: /* search_clause: SEARCH sub_search_array  */
#line 1658 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5566 "parser.cpp"
    break;

  case 204: /* search_clause: %empty  */
#line 1663 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5574 "parser.cpp"
    break;

  case 205: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1667 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5582 "parser.cpp"
    break;

  case 206: /* optional_search_filter_expr: %empty  */
#line 1670 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5590 "parser.cpp"
    break;

  case 207: /* where_clause: WHERE expr  */
#line 1674 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5598 "parser.cpp"
    break;

  case 208: /* where_clause: %empty  */
#line 1677 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5606 "parser.cpp"
    break;

  case 209: /* having_clause: HAVING expr  */
#line 1681 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5614 "parser.cpp"
    break;

  case 210: /* having_clause: %empty  */
#line 1684 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5622 "parser.cpp"
    break;

  case 211: /* group_by_clause: GROUP BY expr_array  */
#line 1688 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5630 "parser.cpp"
    break;

  case 212: /* group_by_clause: %empty  */
#line 1691 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5638 "parser.cpp"
    break;

  case 213: /* set_operator: UNION  */
#line 1695 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5646 "parser.cpp"
    break;

  case 214: /* set_operator: UNION ALL  */
#line 1698 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5654 "parser.cpp"
    break;

  case 215: /* set_operator: INTERSECT  */
#line 1701 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5662 "parser.cpp"
    break;

  case 216: /* set_operator: EXCEPT  */
#line 1704 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5670 "parser.cpp"
    break;

  case 217: /* table_reference: table_reference_unit  */
#line 1712 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5678 "parser.cpp"
    break;

  case 218: /* table_reference: table_reference ',' table_reference_unit  */
#line 1715 "parser.y"
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
#line 5696 "parser.cpp"
    break;

  case 221: /* table_reference_name: table_name table_alias  */
#line 1732 "parser.y"
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
#line 5714 "parser.cpp"
    break;

  case 222: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1746 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5725 "parser.cpp"
    break;

  case 223: /* table_name: IDENTIFIER  */
#line 1755 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5735 "parser.cpp"
    break;

  case 224: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1760 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5747 "parser.cpp"
    break;

  case 225: /* table_alias: AS IDENTIFIER  */
#line 1769 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5757 "parser.cpp"
    break;

  case 226: /* table_alias: IDENTIFIER  */
#line 1774 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5767 "parser.cpp"
    break;

  case 227: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1779 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5778 "parser.cpp"
    break;

  case 228: /* table_alias: %empty  */
#line 1785 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5786 "parser.cpp"
    break;

  case 229: /* with_clause: WITH with_expr_list  */
#line 1792 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5794 "parser.cpp"
    break;

  case 230: /* with_clause: %empty  */
#line 1795 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5802 "parser.cpp"
    break;

  case 231: /* with_expr_list: with_expr  */
#line 1799 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5811 "parser.cpp"
    break;

  case 232: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1802 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5820 "parser.cpp"
    break;

  case 233: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1807 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5832 "parser.cpp"
    break;

  case 234: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1819 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5844 "parser.cpp"
    break;

  case 235: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1826 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5857 "parser.cpp"
    break;

  case 236: /* join_type: INNER  */
#line 1840 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5865 "parser.cpp"
    break;

  case 237: /* join_type: LEFT  */
#line 1843 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5873 "parser.cpp"
    break;

  case 238: /* join_type: RIGHT  */
#line 1846 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5881 "parser.cpp"
    break;

  case 239: /* join_type: OUTER  */
#line 1849 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5889 "parser.cpp"
    break;

  case 240: /* join_type: FULL  */
#line 1852 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5897 "parser.cpp"
    break;

  case 241: /* join_type: CROSS  */
#line 1855 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5905 "parser.cpp"
    break;

  case 242: /* join_type: %empty  */
#line 1858 "parser.y"
                {
}
#line 5912 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASES  */
#line 1864 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5921 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLES  */
#line 1868 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5930 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW VIEWS  */
#line 1872 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5939 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW CONFIGS  */
#line 1876 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5948 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1880 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5960 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW PROFILES  */
#line 1887 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5969 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW BUFFER  */
#line 1891 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5978 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW MEMINDEX  */
#line 1895 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5987 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW QUERIES  */
#line 1899 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5996 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1903 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6006 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TRANSACTIONS  */
#line 1908 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6015 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1912 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6025 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW SESSION VARIABLES  */
#line 1917 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6034 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1921 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6043 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1925 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6054 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1931 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6065 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1937 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6076 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name  */
#line 1943 "parser.y"
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
#line 6092 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1954 "parser.y"
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
#line 6108 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1965 "parser.y"
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
#line 6124 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1976 "parser.y"
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
#line 6141 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1988 "parser.y"
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
#line 6158 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2000 "parser.y"
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
#line 6176 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2013 "parser.y"
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
#line 6195 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2027 "parser.y"
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
#line 6211 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2038 "parser.y"
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
#line 6230 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2052 "parser.y"
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
#line 6251 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2068 "parser.y"
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
#line 6273 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW LOGS  */
#line 2085 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6282 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW DELTA LOGS  */
#line 2089 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6291 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW CATALOGS  */
#line 2093 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6300 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2097 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6309 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2101 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6318 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2105 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6329 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW MEMORY  */
#line 2111 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6338 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2115 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6347 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2119 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6356 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2123 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6367 "parser.cpp"
    break;

  case 281: /* flush_statement: FLUSH DATA  */
#line 2133 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6376 "parser.cpp"
    break;

  case 282: /* flush_statement: FLUSH LOG  */
#line 2137 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6385 "parser.cpp"
    break;

  case 283: /* flush_statement: FLUSH BUFFER  */
#line 2141 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6394 "parser.cpp"
    break;

  case 284: /* optimize_statement: OPTIMIZE table_name  */
#line 2149 "parser.y"
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
#line 6409 "parser.cpp"
    break;

  case 285: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2160 "parser.y"
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
#line 6433 "parser.cpp"
    break;

  case 286: /* command_statement: USE IDENTIFIER  */
#line 2183 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6444 "parser.cpp"
    break;

  case 287: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2189 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6454 "parser.cpp"
    break;

  case 288: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2194 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6465 "parser.cpp"
    break;

  case 289: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2200 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6476 "parser.cpp"
    break;

  case 290: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2206 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6489 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2214 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6500 "parser.cpp"
    break;

  case 292: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2220 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6511 "parser.cpp"
    break;

  case 293: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2226 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6522 "parser.cpp"
    break;

  case 294: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2232 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6533 "parser.cpp"
    break;

  case 295: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2238 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6546 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2246 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6557 "parser.cpp"
    break;

  case 297: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2252 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6568 "parser.cpp"
    break;

  case 298: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2258 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6579 "parser.cpp"
    break;

  case 299: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2264 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6590 "parser.cpp"
    break;

  case 300: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2270 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6603 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2278 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6614 "parser.cpp"
    break;

  case 302: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2284 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6625 "parser.cpp"
    break;

  case 303: /* command_statement: LOCK TABLE table_name  */
#line 2290 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6639 "parser.cpp"
    break;

  case 304: /* command_statement: UNLOCK TABLE table_name  */
#line 2299 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6653 "parser.cpp"
    break;

  case 305: /* compact_statement: COMPACT TABLE table_name  */
#line 2309 "parser.y"
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
#line 6670 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2322 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6679 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2326 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6689 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2331 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6700 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2337 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6712 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2344 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6725 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2352 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6739 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2361 "parser.y"
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
#line 6754 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2371 "parser.y"
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
#line 6769 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2381 "parser.y"
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
#line 6785 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2392 "parser.y"
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
#line 6801 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2403 "parser.y"
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
#line 6818 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2415 "parser.y"
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
#line 6835 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2427 "parser.y"
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
#line 6850 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2437 "parser.y"
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
#line 6866 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2448 "parser.y"
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
#line 6883 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2460 "parser.y"
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
#line 6901 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW LOGS  */
#line 2473 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6910 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2477 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6920 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2482 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6930 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2487 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6941 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2493 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6950 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2497 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6959 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2501 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6970 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2507 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 6979 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2511 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 6988 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW SNAPSHOT STRING  */
#line 2515 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6999 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2521 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7010 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2527 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7023 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2535 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7034 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW NODES  */
#line 2541 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7043 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2545 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7054 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW NODE  */
#line 2551 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7063 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2555 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7074 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SET ADMIN  */
#line 2561 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7084 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SET STANDALONE  */
#line 2566 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7094 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2571 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7106 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2578 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7120 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2587 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7134 "parser.cpp"
    break;

  case 344: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2597 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7148 "parser.cpp"
    break;

  case 345: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2606 "parser.y"
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
#line 7165 "parser.cpp"
    break;

  case 346: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2618 "parser.y"
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
#line 7181 "parser.cpp"
    break;

  case 347: /* expr_array: expr_alias  */
#line 2634 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7190 "parser.cpp"
    break;

  case 348: /* expr_array: expr_array ',' expr_alias  */
#line 2638 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7199 "parser.cpp"
    break;

  case 349: /* expr_array_list: '(' expr_array ')'  */
#line 2643 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 7208 "parser.cpp"
    break;

  case 350: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2647 "parser.y"
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
#line 7234 "parser.cpp"
    break;

  case 351: /* expr_alias: expr AS IDENTIFIER  */
#line 2680 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7245 "parser.cpp"
    break;

  case 352: /* expr_alias: expr  */
#line 2686 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7253 "parser.cpp"
    break;

  case 358: /* operand: '(' expr ')'  */
#line 2696 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7261 "parser.cpp"
    break;

  case 359: /* operand: '(' select_without_paren ')'  */
#line 2699 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7272 "parser.cpp"
    break;

  case 360: /* operand: constant_expr  */
#line 2705 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7280 "parser.cpp"
    break;

  case 371: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2721 "parser.y"
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
#line 7300 "parser.cpp"
    break;

  case 372: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2739 "parser.y"
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
#line 7350 "parser.cpp"
    break;

  case 373: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2785 "parser.y"
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
#line 7393 "parser.cpp"
    break;

  case 374: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2824 "parser.y"
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
#line 7440 "parser.cpp"
    break;

  case 375: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2867 "parser.y"
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
#line 7488 "parser.cpp"
    break;

  case 376: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2914 "parser.y"
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
#line 7516 "parser.cpp"
    break;

  case 377: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2938 "parser.y"
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
#line 7543 "parser.cpp"
    break;

  case 378: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2961 "parser.y"
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
#line 7568 "parser.cpp"
    break;

  case 379: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2982 "parser.y"
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
#line 7593 "parser.cpp"
    break;

  case 380: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3003 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7607 "parser.cpp"
    break;

  case 381: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3012 "parser.y"
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
#line 7623 "parser.cpp"
    break;

  case 382: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3024 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7635 "parser.cpp"
    break;

  case 383: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3031 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7649 "parser.cpp"
    break;

  case 384: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3041 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7660 "parser.cpp"
    break;

  case 385: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3047 "parser.y"
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
#line 7676 "parser.cpp"
    break;

  case 386: /* sub_search: match_vector_expr  */
#line 3059 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7684 "parser.cpp"
    break;

  case 387: /* sub_search: match_text_expr  */
#line 3062 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7692 "parser.cpp"
    break;

  case 388: /* sub_search: match_tensor_expr  */
#line 3065 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7700 "parser.cpp"
    break;

  case 389: /* sub_search: match_sparse_expr  */
#line 3068 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7708 "parser.cpp"
    break;

  case 390: /* sub_search: query_expr  */
#line 3071 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7716 "parser.cpp"
    break;

  case 391: /* sub_search: fusion_expr  */
#line 3074 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7724 "parser.cpp"
    break;

  case 392: /* sub_search_array: sub_search  */
#line 3078 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7733 "parser.cpp"
    break;

  case 393: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3082 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7742 "parser.cpp"
    break;

  case 394: /* function_expr: IDENTIFIER '(' ')'  */
#line 3087 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7755 "parser.cpp"
    break;

  case 395: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3095 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7768 "parser.cpp"
    break;

  case 396: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3103 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7782 "parser.cpp"
    break;

  case 397: /* function_expr: operand IS NOT NULLABLE  */
#line 3112 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7794 "parser.cpp"
    break;

  case 398: /* function_expr: operand IS NULLABLE  */
#line 3119 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7806 "parser.cpp"
    break;

  case 399: /* function_expr: NOT operand  */
#line 3126 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7818 "parser.cpp"
    break;

  case 400: /* function_expr: '-' operand  */
#line 3133 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7830 "parser.cpp"
    break;

  case 401: /* function_expr: '+' operand  */
#line 3140 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7842 "parser.cpp"
    break;

  case 402: /* function_expr: operand '-' operand  */
#line 3147 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7855 "parser.cpp"
    break;

  case 403: /* function_expr: operand '+' operand  */
#line 3155 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7868 "parser.cpp"
    break;

  case 404: /* function_expr: operand '*' operand  */
#line 3163 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7881 "parser.cpp"
    break;

  case 405: /* function_expr: operand '/' operand  */
#line 3171 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7894 "parser.cpp"
    break;

  case 406: /* function_expr: operand '%' operand  */
#line 3179 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7907 "parser.cpp"
    break;

  case 407: /* function_expr: operand '=' operand  */
#line 3187 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7920 "parser.cpp"
    break;

  case 408: /* function_expr: operand EQUAL operand  */
#line 3195 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7933 "parser.cpp"
    break;

  case 409: /* function_expr: operand NOT_EQ operand  */
#line 3203 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7946 "parser.cpp"
    break;

  case 410: /* function_expr: operand '<' operand  */
#line 3211 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7959 "parser.cpp"
    break;

  case 411: /* function_expr: operand '>' operand  */
#line 3219 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7972 "parser.cpp"
    break;

  case 412: /* function_expr: operand LESS_EQ operand  */
#line 3227 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7985 "parser.cpp"
    break;

  case 413: /* function_expr: operand GREATER_EQ operand  */
#line 3235 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7998 "parser.cpp"
    break;

  case 414: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3243 "parser.y"
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
#line 8033 "parser.cpp"
    break;

  case 415: /* function_expr: operand LIKE operand  */
#line 3273 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8046 "parser.cpp"
    break;

  case 416: /* function_expr: operand NOT LIKE operand  */
#line 3281 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8059 "parser.cpp"
    break;

  case 417: /* conjunction_expr: expr AND expr  */
#line 3290 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8072 "parser.cpp"
    break;

  case 418: /* conjunction_expr: expr OR expr  */
#line 3298 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8085 "parser.cpp"
    break;

  case 419: /* between_expr: operand BETWEEN operand AND operand  */
#line 3307 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8097 "parser.cpp"
    break;

  case 420: /* in_expr: operand IN '(' expr_array ')'  */
#line 3315 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8108 "parser.cpp"
    break;

  case 421: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3321 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8119 "parser.cpp"
    break;

  case 422: /* case_expr: CASE expr case_check_array END  */
#line 3328 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8130 "parser.cpp"
    break;

  case 423: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3334 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8142 "parser.cpp"
    break;

  case 424: /* case_expr: CASE case_check_array END  */
#line 3341 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8152 "parser.cpp"
    break;

  case 425: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3346 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8163 "parser.cpp"
    break;

  case 426: /* case_check_array: WHEN expr THEN expr  */
#line 3353 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8175 "parser.cpp"
    break;

  case 427: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3360 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8187 "parser.cpp"
    break;

  case 428: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3368 "parser.y"
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
#line 8218 "parser.cpp"
    break;

  case 429: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3395 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8229 "parser.cpp"
    break;

  case 430: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3401 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8240 "parser.cpp"
    break;

  case 431: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3407 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8252 "parser.cpp"
    break;

  case 432: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3414 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8264 "parser.cpp"
    break;

  case 433: /* column_expr: IDENTIFIER  */
#line 3422 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8276 "parser.cpp"
    break;

  case 434: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3429 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8288 "parser.cpp"
    break;

  case 435: /* column_expr: '*'  */
#line 3436 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8298 "parser.cpp"
    break;

  case 436: /* column_expr: column_expr '.' '*'  */
#line 3441 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8312 "parser.cpp"
    break;

  case 437: /* constant_expr: STRING  */
#line 3451 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8322 "parser.cpp"
    break;

  case 438: /* constant_expr: TRUE  */
#line 3456 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8332 "parser.cpp"
    break;

  case 439: /* constant_expr: FALSE  */
#line 3461 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8342 "parser.cpp"
    break;

  case 440: /* constant_expr: DOUBLE_VALUE  */
#line 3466 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8352 "parser.cpp"
    break;

  case 441: /* constant_expr: LONG_VALUE  */
#line 3471 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8362 "parser.cpp"
    break;

  case 442: /* constant_expr: DATE STRING  */
#line 3476 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8372 "parser.cpp"
    break;

  case 443: /* constant_expr: TIME STRING  */
#line 3481 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8382 "parser.cpp"
    break;

  case 444: /* constant_expr: DATETIME STRING  */
#line 3486 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8392 "parser.cpp"
    break;

  case 445: /* constant_expr: TIMESTAMP STRING  */
#line 3491 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8402 "parser.cpp"
    break;

  case 446: /* constant_expr: INTERVAL interval_expr  */
#line 3496 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8410 "parser.cpp"
    break;

  case 447: /* constant_expr: interval_expr  */
#line 3499 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8418 "parser.cpp"
    break;

  case 448: /* constant_expr: common_array_expr  */
#line 3502 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8426 "parser.cpp"
    break;

  case 449: /* common_array_expr: array_expr  */
#line 3506 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8434 "parser.cpp"
    break;

  case 450: /* common_array_expr: subarray_array_expr  */
#line 3509 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8442 "parser.cpp"
    break;

  case 451: /* common_array_expr: sparse_array_expr  */
#line 3512 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8450 "parser.cpp"
    break;

  case 452: /* common_array_expr: empty_array_expr  */
#line 3515 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8458 "parser.cpp"
    break;

  case 453: /* common_sparse_array_expr: sparse_array_expr  */
#line 3519 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8466 "parser.cpp"
    break;

  case 454: /* common_sparse_array_expr: array_expr  */
#line 3522 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8474 "parser.cpp"
    break;

  case 455: /* common_sparse_array_expr: empty_array_expr  */
#line 3525 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8482 "parser.cpp"
    break;

  case 456: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3529 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8490 "parser.cpp"
    break;

  case 457: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3533 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8500 "parser.cpp"
    break;

  case 458: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3538 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8509 "parser.cpp"
    break;

  case 459: /* sparse_array_expr: long_sparse_array_expr  */
#line 3543 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8517 "parser.cpp"
    break;

  case 460: /* sparse_array_expr: double_sparse_array_expr  */
#line 3546 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8525 "parser.cpp"
    break;

  case 461: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3550 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8533 "parser.cpp"
    break;

  case 462: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3554 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8545 "parser.cpp"
    break;

  case 463: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3561 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8556 "parser.cpp"
    break;

  case 464: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3568 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8564 "parser.cpp"
    break;

  case 465: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3572 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8576 "parser.cpp"
    break;

  case 466: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3579 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8587 "parser.cpp"
    break;

  case 467: /* empty_array_expr: '[' ']'  */
#line 3586 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8595 "parser.cpp"
    break;

  case 468: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3590 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8603 "parser.cpp"
    break;

  case 469: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3594 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8611 "parser.cpp"
    break;

  case 470: /* array_expr: long_array_expr  */
#line 3598 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8619 "parser.cpp"
    break;

  case 471: /* array_expr: double_array_expr  */
#line 3601 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8627 "parser.cpp"
    break;

  case 472: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3605 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8635 "parser.cpp"
    break;

  case 473: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3609 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8645 "parser.cpp"
    break;

  case 474: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3614 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8654 "parser.cpp"
    break;

  case 475: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3619 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8662 "parser.cpp"
    break;

  case 476: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3623 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8672 "parser.cpp"
    break;

  case 477: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3628 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8681 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE SECONDS  */
#line 3633 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8692 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE SECOND  */
#line 3639 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8703 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE MINUTES  */
#line 3645 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8714 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE MINUTE  */
#line 3651 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8725 "parser.cpp"
    break;

  case 482: /* interval_expr: LONG_VALUE HOURS  */
#line 3657 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8736 "parser.cpp"
    break;

  case 483: /* interval_expr: LONG_VALUE HOUR  */
#line 3663 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8747 "parser.cpp"
    break;

  case 484: /* interval_expr: LONG_VALUE DAYS  */
#line 3669 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8758 "parser.cpp"
    break;

  case 485: /* interval_expr: LONG_VALUE DAY  */
#line 3675 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8769 "parser.cpp"
    break;

  case 486: /* interval_expr: LONG_VALUE MONTHS  */
#line 3681 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8780 "parser.cpp"
    break;

  case 487: /* interval_expr: LONG_VALUE MONTH  */
#line 3687 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8791 "parser.cpp"
    break;

  case 488: /* interval_expr: LONG_VALUE YEARS  */
#line 3693 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8802 "parser.cpp"
    break;

  case 489: /* interval_expr: LONG_VALUE YEAR  */
#line 3699 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8813 "parser.cpp"
    break;

  case 490: /* copy_option_list: copy_option  */
#line 3710 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8822 "parser.cpp"
    break;

  case 491: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3714 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8831 "parser.cpp"
    break;

  case 492: /* copy_option: FORMAT IDENTIFIER  */
#line 3719 "parser.y"
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
#line 8867 "parser.cpp"
    break;

  case 493: /* copy_option: DELIMITER STRING  */
#line 3750 "parser.y"
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
#line 8882 "parser.cpp"
    break;

  case 494: /* copy_option: HEADER  */
#line 3760 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8892 "parser.cpp"
    break;

  case 495: /* copy_option: OFFSET LONG_VALUE  */
#line 3765 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8902 "parser.cpp"
    break;

  case 496: /* copy_option: LIMIT LONG_VALUE  */
#line 3770 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8912 "parser.cpp"
    break;

  case 497: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3775 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8922 "parser.cpp"
    break;

  case 498: /* file_path: STRING  */
#line 3781 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8930 "parser.cpp"
    break;

  case 499: /* if_exists: IF EXISTS  */
#line 3785 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8936 "parser.cpp"
    break;

  case 500: /* if_exists: %empty  */
#line 3786 "parser.y"
  { (yyval.bool_value) = false; }
#line 8942 "parser.cpp"
    break;

  case 501: /* if_not_exists: IF NOT EXISTS  */
#line 3788 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8948 "parser.cpp"
    break;

  case 502: /* if_not_exists: %empty  */
#line 3789 "parser.y"
  { (yyval.bool_value) = false; }
#line 8954 "parser.cpp"
    break;

  case 505: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3804 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8967 "parser.cpp"
    break;

  case 506: /* if_not_exists_info: %empty  */
#line 3812 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8975 "parser.cpp"
    break;

  case 507: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3816 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8983 "parser.cpp"
    break;

  case 508: /* with_index_param_list: %empty  */
#line 3819 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8991 "parser.cpp"
    break;

  case 509: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3823 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8999 "parser.cpp"
    break;

  case 510: /* optional_table_properties_list: %empty  */
#line 3826 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9007 "parser.cpp"
    break;

  case 511: /* index_param_list: index_param  */
#line 3830 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9016 "parser.cpp"
    break;

  case 512: /* index_param_list: index_param_list ',' index_param  */
#line 3834 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9025 "parser.cpp"
    break;

  case 513: /* index_param: IDENTIFIER  */
#line 3839 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9036 "parser.cpp"
    break;

  case 514: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3845 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9051 "parser.cpp"
    break;

  case 515: /* index_param: IDENTIFIER '=' STRING  */
#line 3855 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9066 "parser.cpp"
    break;

  case 516: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3865 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9079 "parser.cpp"
    break;

  case 517: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3873 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9092 "parser.cpp"
    break;

  case 518: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3884 "parser.y"
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
#line 9128 "parser.cpp"
    break;

  case 519: /* index_info: '(' IDENTIFIER ')'  */
#line 3915 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9139 "parser.cpp"
    break;


#line 9143 "parser.cpp"

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

#line 3922 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

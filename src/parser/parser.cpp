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
  YYSYMBOL_ARRAY = 118,                    /* ARRAY  */
  YYSYMBOL_TUPLE = 119,                    /* TUPLE  */
  YYSYMBOL_EMBEDDING = 120,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 121,                   /* VECTOR  */
  YYSYMBOL_MULTIVECTOR = 122,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 123,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 124,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 125,              /* TENSORARRAY  */
  YYSYMBOL_BIT = 126,                      /* BIT  */
  YYSYMBOL_TEXT = 127,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 128,                  /* PRIMARY  */
  YYSYMBOL_KEY = 129,                      /* KEY  */
  YYSYMBOL_UNIQUE = 130,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 131,                 /* NULLABLE  */
  YYSYMBOL_IS = 132,                       /* IS  */
  YYSYMBOL_DEFAULT = 133,                  /* DEFAULT  */
  YYSYMBOL_COMMENT = 134,                  /* COMMENT  */
  YYSYMBOL_IGNORE = 135,                   /* IGNORE  */
  YYSYMBOL_TRUE = 136,                     /* TRUE  */
  YYSYMBOL_FALSE = 137,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 138,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 139,                   /* SECOND  */
  YYSYMBOL_SECONDS = 140,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 141,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 142,                  /* MINUTES  */
  YYSYMBOL_HOUR = 143,                     /* HOUR  */
  YYSYMBOL_HOURS = 144,                    /* HOURS  */
  YYSYMBOL_DAY = 145,                      /* DAY  */
  YYSYMBOL_DAYS = 146,                     /* DAYS  */
  YYSYMBOL_MONTH = 147,                    /* MONTH  */
  YYSYMBOL_MONTHS = 148,                   /* MONTHS  */
  YYSYMBOL_YEAR = 149,                     /* YEAR  */
  YYSYMBOL_YEARS = 150,                    /* YEARS  */
  YYSYMBOL_EQUAL = 151,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 152,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 153,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 154,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 155,                  /* BETWEEN  */
  YYSYMBOL_AND = 156,                      /* AND  */
  YYSYMBOL_OR = 157,                       /* OR  */
  YYSYMBOL_EXTRACT = 158,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 159,                     /* LIKE  */
  YYSYMBOL_DATA = 160,                     /* DATA  */
  YYSYMBOL_LOG = 161,                      /* LOG  */
  YYSYMBOL_BUFFER = 162,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 163,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 164,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 165,                 /* MEMINDEX  */
  YYSYMBOL_USING = 166,                    /* USING  */
  YYSYMBOL_SESSION = 167,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 168,                   /* GLOBAL  */
  YYSYMBOL_OFF = 169,                      /* OFF  */
  YYSYMBOL_EXPORT = 170,                   /* EXPORT  */
  YYSYMBOL_CONFIGS = 171,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 172,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 173,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 174,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 175,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 176,                    /* DELTA  */
  YYSYMBOL_LOGS = 177,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 178,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 179,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 180,                   /* SEARCH  */
  YYSYMBOL_MATCH = 181,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 182,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 183,                    /* QUERY  */
  YYSYMBOL_QUERIES = 184,                  /* QUERIES  */
  YYSYMBOL_FUSION = 185,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 186,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 187,                    /* ADMIN  */
  YYSYMBOL_LEADER = 188,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 189,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 190,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 191,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 192,               /* STANDALONE  */
  YYSYMBOL_NODES = 193,                    /* NODES  */
  YYSYMBOL_NODE = 194,                     /* NODE  */
  YYSYMBOL_REMOVE = 195,                   /* REMOVE  */
  YYSYMBOL_SNAPSHOT = 196,                 /* SNAPSHOT  */
  YYSYMBOL_SNAPSHOTS = 197,                /* SNAPSHOTS  */
  YYSYMBOL_RECOVER = 198,                  /* RECOVER  */
  YYSYMBOL_RESTORE = 199,                  /* RESTORE  */
  YYSYMBOL_PERSISTENCE = 200,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 201,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 202,                  /* OBJECTS  */
  YYSYMBOL_FILES = 203,                    /* FILES  */
  YYSYMBOL_MEMORY = 204,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 205,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 206,                  /* HISTORY  */
  YYSYMBOL_NUMBER = 207,                   /* NUMBER  */
  YYSYMBOL_208_ = 208,                     /* '='  */
  YYSYMBOL_209_ = 209,                     /* '<'  */
  YYSYMBOL_210_ = 210,                     /* '>'  */
  YYSYMBOL_211_ = 211,                     /* '+'  */
  YYSYMBOL_212_ = 212,                     /* '-'  */
  YYSYMBOL_213_ = 213,                     /* '*'  */
  YYSYMBOL_214_ = 214,                     /* '/'  */
  YYSYMBOL_215_ = 215,                     /* '%'  */
  YYSYMBOL_216_ = 216,                     /* '['  */
  YYSYMBOL_217_ = 217,                     /* ']'  */
  YYSYMBOL_218_ = 218,                     /* '('  */
  YYSYMBOL_219_ = 219,                     /* ')'  */
  YYSYMBOL_220_ = 220,                     /* '.'  */
  YYSYMBOL_221_ = 221,                     /* ';'  */
  YYSYMBOL_222_ = 222,                     /* ','  */
  YYSYMBOL_223_ = 223,                     /* '}'  */
  YYSYMBOL_224_ = 224,                     /* '{'  */
  YYSYMBOL_225_ = 225,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 226,                 /* $accept  */
  YYSYMBOL_input_pattern = 227,            /* input_pattern  */
  YYSYMBOL_statement_list = 228,           /* statement_list  */
  YYSYMBOL_statement = 229,                /* statement  */
  YYSYMBOL_explainable_statement = 230,    /* explainable_statement  */
  YYSYMBOL_create_statement = 231,         /* create_statement  */
  YYSYMBOL_table_element_array = 232,      /* table_element_array  */
  YYSYMBOL_column_def_array = 233,         /* column_def_array  */
  YYSYMBOL_table_element = 234,            /* table_element  */
  YYSYMBOL_table_column = 235,             /* table_column  */
  YYSYMBOL_column_type_array = 236,        /* column_type_array  */
  YYSYMBOL_column_type = 237,              /* column_type  */
  YYSYMBOL_column_constraints = 238,       /* column_constraints  */
  YYSYMBOL_column_constraint = 239,        /* column_constraint  */
  YYSYMBOL_default_expr = 240,             /* default_expr  */
  YYSYMBOL_table_constraint = 241,         /* table_constraint  */
  YYSYMBOL_identifier_array = 242,         /* identifier_array  */
  YYSYMBOL_delete_statement = 243,         /* delete_statement  */
  YYSYMBOL_insert_statement = 244,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 245, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 246,        /* explain_statement  */
  YYSYMBOL_update_statement = 247,         /* update_statement  */
  YYSYMBOL_update_expr_array = 248,        /* update_expr_array  */
  YYSYMBOL_update_expr = 249,              /* update_expr  */
  YYSYMBOL_drop_statement = 250,           /* drop_statement  */
  YYSYMBOL_copy_statement = 251,           /* copy_statement  */
  YYSYMBOL_select_statement = 252,         /* select_statement  */
  YYSYMBOL_select_with_paren = 253,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 254,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 255, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 256, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 257, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 258,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 259,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 260,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 261,            /* order_by_type  */
  YYSYMBOL_limit_expr = 262,               /* limit_expr  */
  YYSYMBOL_offset_expr = 263,              /* offset_expr  */
  YYSYMBOL_distinct = 264,                 /* distinct  */
  YYSYMBOL_highlight_clause = 265,         /* highlight_clause  */
  YYSYMBOL_from_clause = 266,              /* from_clause  */
  YYSYMBOL_search_clause = 267,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 268, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 269,             /* where_clause  */
  YYSYMBOL_having_clause = 270,            /* having_clause  */
  YYSYMBOL_group_by_clause = 271,          /* group_by_clause  */
  YYSYMBOL_set_operator = 272,             /* set_operator  */
  YYSYMBOL_table_reference = 273,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 274,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 275,     /* table_reference_name  */
  YYSYMBOL_table_name = 276,               /* table_name  */
  YYSYMBOL_table_alias = 277,              /* table_alias  */
  YYSYMBOL_with_clause = 278,              /* with_clause  */
  YYSYMBOL_with_expr_list = 279,           /* with_expr_list  */
  YYSYMBOL_with_expr = 280,                /* with_expr  */
  YYSYMBOL_join_clause = 281,              /* join_clause  */
  YYSYMBOL_join_type = 282,                /* join_type  */
  YYSYMBOL_show_statement = 283,           /* show_statement  */
  YYSYMBOL_flush_statement = 284,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 285,       /* optimize_statement  */
  YYSYMBOL_command_statement = 286,        /* command_statement  */
  YYSYMBOL_compact_statement = 287,        /* compact_statement  */
  YYSYMBOL_admin_statement = 288,          /* admin_statement  */
  YYSYMBOL_alter_statement = 289,          /* alter_statement  */
  YYSYMBOL_expr_array = 290,               /* expr_array  */
  YYSYMBOL_insert_row_list = 291,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 292,               /* expr_alias  */
  YYSYMBOL_expr = 293,                     /* expr  */
  YYSYMBOL_operand = 294,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 295,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 296,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 297,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 298,          /* match_text_expr  */
  YYSYMBOL_query_expr = 299,               /* query_expr  */
  YYSYMBOL_fusion_expr = 300,              /* fusion_expr  */
  YYSYMBOL_sub_search = 301,               /* sub_search  */
  YYSYMBOL_sub_search_array = 302,         /* sub_search_array  */
  YYSYMBOL_function_expr = 303,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 304,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 305,             /* between_expr  */
  YYSYMBOL_in_expr = 306,                  /* in_expr  */
  YYSYMBOL_case_expr = 307,                /* case_expr  */
  YYSYMBOL_case_check_array = 308,         /* case_check_array  */
  YYSYMBOL_cast_expr = 309,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 310,            /* subquery_expr  */
  YYSYMBOL_column_expr = 311,              /* column_expr  */
  YYSYMBOL_constant_expr = 312,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 313,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 314, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 315,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 316, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 317,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 318,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 319, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 320, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 321, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 322,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 323,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 324, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 325,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 326,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 327,               /* array_expr  */
  YYSYMBOL_long_array_expr = 328,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 329, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 330,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 331, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 332,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 333,         /* copy_option_list  */
  YYSYMBOL_copy_option = 334,              /* copy_option  */
  YYSYMBOL_file_path = 335,                /* file_path  */
  YYSYMBOL_if_exists = 336,                /* if_exists  */
  YYSYMBOL_if_not_exists = 337,            /* if_not_exists  */
  YYSYMBOL_semicolon = 338,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 339,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 340,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 341, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 342,         /* index_param_list  */
  YYSYMBOL_index_param = 343,              /* index_param  */
  YYSYMBOL_index_info = 344                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 473 "parser.cpp"

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
#define YYFINAL  128
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1555

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  226
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  119
/* YYNRULES -- Number of rules.  */
#define YYNRULES  550
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1253

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   462


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
       2,     2,     2,     2,     2,     2,     2,   215,     2,     2,
     218,   219,   213,   211,   222,   212,   220,   214,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   225,   221,
     209,   208,   210,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   216,     2,   217,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   224,     2,   223,     2,     2,     2,     2,
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
     205,   206,   207
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
    1107,  1123,  1161,  1177,  1180,  1187,  1204,  1213,  1226,  1230,
    1235,  1248,  1261,  1276,  1291,  1306,  1329,  1382,  1437,  1488,
    1491,  1494,  1503,  1513,  1516,  1520,  1525,  1552,  1555,  1560,
    1577,  1580,  1584,  1588,  1593,  1599,  1602,  1605,  1609,  1613,
    1615,  1619,  1621,  1624,  1628,  1631,  1635,  1638,  1642,  1647,
    1651,  1654,  1658,  1661,  1665,  1668,  1672,  1675,  1679,  1682,
    1685,  1688,  1696,  1699,  1714,  1714,  1716,  1730,  1739,  1744,
    1753,  1758,  1763,  1769,  1776,  1779,  1783,  1786,  1791,  1803,
    1810,  1824,  1827,  1830,  1833,  1836,  1839,  1842,  1848,  1852,
    1856,  1860,  1864,  1871,  1875,  1879,  1883,  1887,  1892,  1896,
    1901,  1905,  1909,  1913,  1919,  1925,  1931,  1942,  1953,  1964,
    1976,  1988,  2001,  2015,  2026,  2040,  2056,  2073,  2077,  2081,
    2085,  2089,  2095,  2099,  2103,  2109,  2113,  2117,  2121,  2127,
    2131,  2141,  2145,  2149,  2157,  2168,  2191,  2197,  2202,  2208,
    2214,  2222,  2228,  2234,  2240,  2246,  2254,  2260,  2266,  2272,
    2278,  2286,  2292,  2298,  2307,  2316,  2324,  2332,  2338,  2344,
    2350,  2357,  2370,  2374,  2379,  2385,  2392,  2400,  2409,  2419,
    2429,  2440,  2451,  2463,  2475,  2485,  2496,  2508,  2521,  2525,
    2530,  2535,  2541,  2545,  2549,  2555,  2559,  2563,  2569,  2575,
    2583,  2589,  2593,  2599,  2603,  2609,  2614,  2619,  2626,  2635,
    2645,  2654,  2666,  2682,  2686,  2691,  2701,  2723,  2729,  2733,
    2734,  2735,  2736,  2737,  2739,  2742,  2748,  2751,  2752,  2753,
    2754,  2755,  2756,  2757,  2758,  2759,  2760,  2764,  2780,  2797,
    2815,  2861,  2900,  2943,  2990,  3014,  3037,  3058,  3079,  3088,
    3099,  3110,  3124,  3131,  3141,  3147,  3159,  3162,  3165,  3168,
    3171,  3174,  3178,  3182,  3187,  3195,  3203,  3212,  3219,  3226,
    3233,  3240,  3247,  3254,  3261,  3268,  3275,  3282,  3289,  3297,
    3305,  3313,  3321,  3329,  3337,  3345,  3353,  3361,  3369,  3377,
    3385,  3415,  3423,  3432,  3440,  3449,  3457,  3463,  3470,  3476,
    3483,  3488,  3495,  3502,  3510,  3523,  3529,  3535,  3542,  3550,
    3557,  3564,  3569,  3579,  3584,  3589,  3594,  3599,  3604,  3609,
    3614,  3619,  3624,  3627,  3630,  3633,  3637,  3640,  3643,  3646,
    3650,  3653,  3656,  3660,  3664,  3669,  3674,  3677,  3681,  3685,
    3692,  3699,  3703,  3710,  3717,  3721,  3724,  3728,  3732,  3737,
    3741,  3745,  3748,  3752,  3756,  3761,  3766,  3770,  3775,  3780,
    3786,  3792,  3798,  3804,  3810,  3816,  3822,  3828,  3834,  3840,
    3846,  3857,  3861,  3866,  3897,  3907,  3912,  3917,  3922,  3928,
    3932,  3933,  3935,  3936,  3938,  3939,  3951,  3959,  3963,  3966,
    3970,  3973,  3977,  3981,  3986,  3992,  4002,  4012,  4020,  4031,
    4062
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
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
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
  "highlight_clause", "from_clause", "search_clause",
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

#define YYPACT_NINF (-756)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-538)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     973,    63,    23,   231,    81,   -10,    81,   264,   937,  1031,
      85,   104,   134,   157,   420,   236,   254,   292,   138,   110,
     563,   -60,   206,    83,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,   495,  -756,  -756,   315,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,   258,   258,   258,   258,    51,   336,    81,
     323,   323,   323,   323,   323,   380,   193,   405,    81,   -28,
     458,   467,   481,  1012,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,   495,  -756,  -756,  -756,  -756,  -756,   210,   487,    81,
    -756,  -756,  -756,  -756,  -756,     8,  -756,   259,   272,  -756,
     497,  -756,   297,  -756,  -756,   430,   515,  -756,   511,  -756,
     514,   141,    81,    81,    81,    81,  -756,  -756,  -756,  -756,
     -27,  -756,   479,   316,  -756,   536,   358,   360,   324,   731,
     362,   540,   372,   496,   378,   394,   385,   387,  -756,    76,
    -756,   587,  -756,  -756,    19,   560,  -756,   564,   565,   639,
      81,    81,    81,   641,   584,   586,   444,   598,   657,    81,
      81,    81,   680,  -756,   693,   695,   634,   702,   702,   646,
      72,   140,   178,  -756,   499,  -756,   443,  -756,  -756,  -756,
     725,  -756,   727,  -756,  -756,   702,  -756,  -756,   728,  -756,
    -756,  -756,  -756,   415,  -756,  -756,  -756,    81,   516,   292,
     702,  -756,   729,  -756,   569,  -756,   730,  -756,  -756,   737,
    -756,  -756,   736,  -756,   739,   741,  -756,   742,   690,   746,
     555,   749,   751,  -756,  -756,  -756,  -756,    19,  -756,  -756,
    -756,   646,   703,   687,   688,   629,   -38,  -756,   444,  -756,
      81,   406,   761,    34,  -756,  -756,  -756,  -756,  -756,   704,
    -756,   562,   -37,  -756,   646,  -756,  -756,   698,   699,   553,
    -756,  -756,  1191,   705,   568,   574,   462,   769,   777,   790,
     792,  -756,  -756,   791,   585,   591,   592,   593,   595,   604,
     605,   425,   607,   608,   796,   796,  -756,    15,   556,    26,
     142,  -756,   -17,   606,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,   610,  -756,  -756,
    -756,   -14,  -756,  -756,   182,  -756,   226,  -756,  -756,   296,
    -756,  -756,   242,  -756,   255,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,   802,   818,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,   781,   798,   770,   760,   315,  -756,  -756,  -756,   845,
     117,  -756,   847,  -756,  -756,   774,   423,  -756,   851,  -756,
    -756,   642,   647,   -63,   646,   646,   799,  -756,   856,   -60,
      79,   810,   649,   868,   869,  -756,  -756,   170,   655,  -756,
      81,   646,   695,  -756,   471,   656,   658,   313,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
     796,   659,   966,   797,   646,   646,   168,   409,  -756,  -756,
    -756,  -756,  1191,  -756,   646,   646,   646,   646,   646,   646,
     874,   662,   664,   665,   666,   881,   883,   508,   508,  -756,
     668,  -756,  -756,  -756,  -756,   670,   125,  -756,  -756,   814,
     646,   891,   646,   646,   -50,   677,     1,   796,   796,   796,
     796,   796,   796,   796,   796,   796,   796,   796,   796,   796,
     796,    25,  -756,   685,  -756,   890,  -756,   894,    75,  -756,
    -756,   898,  -756,   902,   870,   609,   694,   696,   908,   697,
    -756,   700,  -756,   914,  -756,   250,   918,   764,   765,  -756,
    -756,  -756,   646,   850,   714,  -756,    44,   471,   646,  -756,
    -756,    45,  1147,   809,   724,   266,  -756,  -756,  -756,   -60,
     949,   821,  -756,  -756,  -756,   953,   646,   735,  -756,   471,
    -756,   205,   205,   646,  -756,   280,   797,   800,   744,    -4,
     171,   455,  -756,   646,   646,   165,   175,   198,   203,   211,
     225,   882,   646,    30,   646,   960,   743,   311,   671,  -756,
    -756,   702,  -756,  -756,  -756,   813,   752,   796,   556,   840,
    -756,   197,   197,   374,   374,   901,   197,   197,   374,   374,
     508,   508,  -756,  -756,  -756,  -756,  -756,  -756,   747,  -756,
     748,  -756,  -756,  -756,  -756,   969,   970,  -756,   761,   975,
    -756,   988,  -756,  -756,   986,  -756,  -756,   989,   990,   773,
      12,   819,   646,  -756,  -756,  -756,   471,   996,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,   783,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,   787,   788,   793,   795,   811,   812,   820,   822,
     351,   826,   761,   978,    79,   495,   808,  1013,  -756,   348,
     827,  1010,  1025,  1014,  1029,  -756,  1040,   349,  -756,   400,
     402,  -756,   828,  -756,  1147,   646,  -756,   646,   -21,   180,
    -756,  -756,  -756,  -756,  -756,  -756,   796,   112,  -756,    57,
     131,   834,    42,   829,  -756,  1045,  -756,  -756,   972,   556,
     197,   839,   404,  -756,   796,  1053,  1056,  1016,  1015,   413,
     414,  -756,   855,   418,  -756,  1062,  -756,  -756,   -60,   848,
     776,  -756,    39,  -756,   370,   634,  -756,  -756,  1063,  1147,
    1147,   995,  1036,  1059,  1183,  1200,  1223,   940,   945,  -756,
    -756,   345,  -756,   942,   761,   434,   858,   943,  -756,   912,
    -756,  -756,   646,  -756,  -756,  -756,  -756,  -756,  -756,   205,
    -756,  -756,  -756,   860,   471,   105,  -756,   646,   161,   864,
     685,   865,  1078,   861,   646,  -756,   872,   866,   875,   442,
    -756,  -756,   966,  1089,  1090,  -756,  -756,   975,   573,  -756,
     988,   343,    33,    12,  1046,  -756,  -756,  -756,  -756,  -756,
    -756,  1047,  -756,  1094,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,   880,  1054,   446,   886,   447,  -756,   900,   909,
     910,   913,   915,   916,   917,   922,   923,  1032,   924,   925,
     926,   932,   936,   939,   944,   946,   947,   948,  1067,   951,
     961,   962,   964,   965,   967,   968,   979,   983,   984,  1075,
     991,   994,   997,   998,   999,  1000,  1001,  1002,  1003,  1004,
    1079,  1007,  1011,  1038,  1039,  1041,  1042,  1057,  1060,  1061,
    1064,  1096,  1073,  1074,  1077,  1080,  1081,  1085,  1086,  1088,
    1097,  1100,  1105,  1101,  -756,  -756,    75,  -756,   974,  1058,
     448,  -756,   988,  1119,  1197,   463,  -756,  -756,  -756,   471,
    -756,   684,  1102,  1103,    18,  1120,  -756,  -756,  -756,  1121,
    1146,  1034,   471,  -756,   205,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  1211,  -756,    39,   776,    12,
      12,  1066,   370,  1165,  1178,  -756,  1228,  -756,  -756,  1147,
    1272,  1279,  1294,  1295,  1305,  1306,  1312,  1338,  1339,  1124,
    1341,  1342,  1344,  1345,  1346,  1347,  1348,  1349,  1350,  1351,
    1136,  1353,  1354,  1355,  1356,  1357,  1358,  1359,  1360,  1361,
    1362,  1148,  1363,  1365,  1366,  1367,  1368,  1369,  1370,  1371,
    1372,  1373,  1158,  1375,  1376,  1377,  1378,  1379,  1380,  1381,
    1382,  1383,  1384,  1169,  1386,  1387,  1388,  1389,  1390,  1391,
    1392,  1393,  1394,  1395,  1180,  1397,  -756,  1400,  1401,  -756,
     465,  -756,   760,  -756,  -756,  1402,  1403,  1404,    52,  1190,
    -756,   469,  1405,  -756,  -756,  1352,   761,  -756,   646,   646,
    -756,  1193,  -756,  1194,  1196,  1198,  1199,  1201,  1202,  1203,
    1204,  1205,  1410,  1206,  1207,  1208,  1209,  1210,  1212,  1213,
    1214,  1215,  1216,  1413,  1217,  1218,  1219,  1220,  1221,  1222,
    1224,  1225,  1226,  1227,  1424,  1229,  1230,  1231,  1232,  1233,
    1234,  1235,  1236,  1237,  1238,  1436,  1239,  1240,  1241,  1242,
    1243,  1244,  1245,  1246,  1247,  1248,  1441,  1249,  1250,  1251,
    1252,  1253,  1254,  1255,  1256,  1257,  1258,  1472,  1260,  -756,
    -756,  -756,  -756,  1188,  1259,  1261,   861,  1314,  -756,   461,
     646,   485,   773,   471,  -756,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  1263,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  1265,  -756,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  1266,  -756,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  1267,  -756,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  1268,  -756,
    -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  -756,  1269,
    -756,  1485,  1486,    64,  1273,  1274,  1442,  1488,  1489,  -756,
    -756,  -756,   471,  -756,  -756,  -756,  -756,  -756,  -756,  -756,
    1275,  1276,   861,   760,  1330,  1281,  1494,   667,    65,  1282,
    1498,  1284,  -756,  1455,  1501,   681,  1500,  -756,   861,   760,
     861,   -24,  1289,  1290,  1502,  -756,  1462,  1292,  -756,  1293,
    1473,  1474,  -756,  1511,  -756,  -756,  -756,   -19,    70,  -756,
    1298,  1299,  1477,  1479,  -756,  1480,  1481,  1519,  -756,  -756,
    1307,  -756,  1308,  1304,  1521,  1524,   760,  1309,  1310,  -756,
     760,  -756,  -756
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     235,     0,     0,     0,     0,     0,     0,     0,   235,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   235,     0,   535,     3,     5,    10,    12,    13,    11,
       6,     7,     9,   180,   179,     0,     8,    14,    15,    16,
      17,    18,    19,   533,   533,   533,   533,   533,     0,     0,
     531,   531,   531,   531,   531,     0,   228,     0,     0,     0,
       0,     0,     0,   235,   166,    20,    25,    27,    26,    21,
      22,    24,    23,    28,    29,    30,    31,     0,     0,     0,
     249,   250,   248,   254,   258,     0,   255,     0,     0,   251,
       0,   253,     0,   277,   279,   280,     0,   256,     0,   289,
       0,   285,     0,     0,     0,     0,   291,   292,   293,   296,
     228,   294,     0,   234,   236,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     1,   235,
       2,   218,   220,   221,     0,   203,   185,   191,     0,     0,
       0,     0,     0,     0,     0,     0,   164,     0,     0,     0,
       0,     0,     0,   318,     0,     0,   213,     0,     0,     0,
       0,     0,     0,   165,     0,   265,   266,   259,   260,   261,
       0,   262,     0,   252,   278,     0,   257,   290,     0,   283,
     282,   286,   287,     0,   321,   313,   314,     0,     0,     0,
       0,   345,     0,   355,     0,   356,     0,   342,   343,     0,
     338,   322,     0,   351,   353,     0,   346,     0,     0,     0,
       0,     0,     0,   184,   183,     4,   219,     0,   181,   182,
     202,     0,     0,   199,     0,    33,     0,    34,   164,   536,
       0,     0,     0,   235,   530,   171,   173,   172,   174,     0,
     229,     0,   213,   168,     0,   160,   529,     0,     0,   459,
     463,   466,   467,     0,     0,     0,     0,     0,     0,     0,
       0,   464,   465,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   461,     0,   235,     0,
       0,   363,   368,   369,   383,   381,   384,   382,   385,   386,
     378,   373,   372,   371,   379,   380,   370,   377,   376,   474,
     477,     0,   478,   486,     0,   487,     0,   479,   475,     0,
     476,   501,     0,   502,     0,   473,   300,   302,   301,   298,
     299,   305,   307,   306,   303,   304,   310,   312,   311,   308,
     309,   288,     0,     0,   268,   267,   273,   263,   264,   281,
     284,     0,     0,     0,   539,     0,   237,   297,   348,     0,
     339,   344,   323,   352,   347,     0,     0,   354,     0,   319,
     320,     0,     0,   205,     0,     0,   201,   532,     0,   235,
       0,     0,     0,     0,     0,   317,   158,     0,     0,   162,
       0,     0,     0,   167,   212,     0,     0,     0,   510,   509,
     512,   511,   514,   513,   516,   515,   518,   517,   520,   519,
       0,     0,   425,   235,     0,     0,     0,     0,   468,   469,
     470,   471,     0,   472,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   427,   426,   507,
     504,   494,   484,   489,   492,     0,     0,   496,   497,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   483,     0,   488,     0,   491,     0,     0,   495,
     503,     0,   506,     0,   274,   269,     0,     0,     0,     0,
     295,     0,   357,     0,   340,     0,     0,     0,     0,   350,
     188,   187,     0,   207,   190,   192,   197,   198,     0,   186,
      32,    36,     0,     0,     0,     0,    42,    46,    47,   235,
       0,    40,   316,   315,   163,     0,     0,   161,   175,   170,
     169,     0,     0,     0,   414,     0,   235,     0,     0,     0,
       0,     0,   450,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   211,     0,     0,   375,
     374,     0,   364,   367,   443,   444,     0,     0,   235,     0,
     424,   434,   435,   438,   439,     0,   441,   433,   436,   437,
     429,   428,   430,   431,   432,   460,   462,   485,     0,   490,
       0,   493,   498,   505,   508,     0,     0,   270,     0,     0,
     360,     0,   238,   341,     0,   324,   349,     0,     0,   204,
       0,   209,     0,   195,   196,   194,   200,     0,    54,    57,
      58,    55,    56,    59,    60,    76,    61,    63,    62,    79,
      66,    67,    68,    64,    65,    69,    70,    71,    72,    73,
      74,    75,     0,     0,     0,     0,     0,     0,     0,     0,
     539,     0,     0,   541,     0,    39,     0,     0,   159,     0,
       0,     0,     0,     0,     0,   525,     0,     0,   521,     0,
       0,   415,     0,   455,     0,     0,   448,     0,     0,     0,
     422,   421,   420,   419,   418,   417,     0,     0,   459,     0,
       0,     0,     0,     0,   404,     0,   500,   499,     0,   235,
     442,     0,     0,   423,     0,     0,     0,   275,   271,     0,
       0,    44,   544,     0,   542,   325,   358,   359,   235,   206,
     222,   224,   233,   225,     0,   213,   193,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   151,
     152,   155,   148,   155,     0,     0,     0,    35,    43,   550,
      41,   365,     0,   527,   526,   524,   523,   528,   178,     0,
     176,   416,   456,     0,   452,     0,   451,     0,     0,     0,
       0,     0,     0,   211,     0,   402,     0,     0,     0,     0,
     457,   446,   445,     0,     0,   362,   361,     0,     0,   538,
       0,     0,     0,     0,     0,   242,   243,   244,   245,   241,
     246,     0,   231,     0,   226,   408,   406,   409,   407,   410,
     411,   412,   208,   217,     0,     0,     0,    52,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   153,   150,     0,   149,    49,    48,
       0,   157,     0,     0,     0,     0,   522,   454,   449,   453,
     440,     0,     0,     0,     0,     0,   480,   482,   481,   211,
       0,     0,   210,   405,     0,   458,   447,   276,   272,    45,
     545,   546,   548,   547,   543,     0,   326,   233,   223,     0,
       0,   230,     0,     0,   215,    78,     0,   146,   147,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   154,     0,     0,   156,
       0,    37,   539,   366,   504,     0,     0,     0,     0,     0,
     403,     0,   327,   227,   239,     0,     0,   413,     0,     0,
     189,     0,    53,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    51,
      50,   540,   549,     0,     0,   211,   211,   398,   177,     0,
       0,     0,   216,   214,    77,    83,    84,    81,    82,    85,
      86,    87,    88,    89,     0,    80,   127,   128,   125,   126,
     129,   130,   131,   132,   133,     0,   124,    94,    95,    92,
      93,    96,    97,    98,    99,   100,     0,    91,   105,   106,
     103,   104,   107,   108,   109,   110,   111,     0,   102,   138,
     139,   136,   137,   140,   141,   142,   143,   144,     0,   135,
     116,   117,   114,   115,   118,   119,   120,   121,   122,     0,
     113,     0,     0,     0,     0,     0,     0,     0,     0,   329,
     328,   334,   240,   232,    90,   134,   101,   112,   145,   123,
     211,     0,   211,   539,   399,     0,   335,   330,     0,     0,
       0,     0,   397,     0,     0,     0,     0,   331,   211,   539,
     211,   539,     0,     0,     0,   336,   332,     0,   393,     0,
       0,     0,   396,     0,   400,   337,   333,   539,   387,   395,
       0,     0,     0,     0,   392,     0,     0,     0,   401,   391,
       0,   389,     0,     0,     0,     0,   539,     0,     0,   394,
     539,   388,   390
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -756,  -756,  -756,  1406,  1467,   102,  -756,  -756,   887,  -573,
    -756,  -651,  -756,   801,   803,  -756,  -578,   147,   243,  1311,
    -756,   289,  -756,  1151,   294,   299,    -6,  1513,   -18,  1192,
    1321,   -76,  -756,  -756,   938,  -756,  -756,  -756,  -756,  -756,
    -756,  -756,  -755,  -235,  -756,  -756,  -756,  -756,   758,  -193,
      46,   625,  -756,  -756,  1364,  -756,  -756,   302,   325,   327,
     334,   337,  -756,  -756,  -220,  -756,  1104,  -244,  -231,  -678,
    -677,  -675,  -674,  -669,  -667,   621,  -756,  -756,  -756,  -756,
    -756,  -756,  1139,  -756,  -756,  1005,  -274,  -271,  -756,  -756,
    -756,   785,  -756,  -756,  -756,  -756,   786,  -756,  -756,  1084,
    1083,  -358,  -756,  -756,  -756,  -756,  1288,  -511,   805,  -149,
     541,   571,  -756,  -756,  -636,  -756,   673,   772,  -756
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    22,    23,    24,    64,    25,   505,   700,   506,   507,
     806,   640,   731,   732,   878,   508,   377,    26,    27,   233,
      28,    29,   242,   243,    30,    31,    32,    33,    34,   136,
     218,   137,   223,   494,   495,   605,   366,   499,   221,   493,
     601,   715,   683,   245,  1020,   924,   134,   709,   710,   711,
     712,   794,    35,   113,   114,   713,   791,    36,    37,    38,
      39,    40,    41,    42,   280,   517,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   801,   802,   290,   291,   292,
     293,   294,   407,   295,   296,   297,   298,   299,   895,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   433,
     434,   310,   311,   312,   313,   314,   315,   657,   658,   247,
     148,   139,   130,   144,   480,   737,   703,   704,   511
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     384,   363,    71,   127,   733,   438,   432,   383,   901,   248,
     699,   659,   406,   753,   167,    56,   701,   369,    17,   492,
     429,   430,   402,   429,   430,   556,   339,   135,   575,   244,
     250,   251,   252,   678,   436,   187,   795,   796,   441,   797,
     798,   347,   792,   427,   428,   799,   763,   800,   157,   158,
      57,   664,    59,   131,   479,   132,  1096,    71,   219,   479,
      49,   133,   111,   756,   735,   131,    58,   132,   805,   807,
    1192,  1208,   378,   133,   559,   316,  -534,   317,   318,   250,
     251,   252,   502,     1,    56,     2,     3,     4,     5,     6,
       7,     8,     9,    10,   793,   146,    43,    44,    45,    11,
      12,    13,    46,    47,   156,    14,    15,    16,   764,   557,
      65,  1220,    17,  -537,   603,   604,  1232,   116,   764,   102,
     496,   497,   117,   138,   118,   166,   119,   257,   258,   259,
     764,   764,   560,   260,   319,   442,   443,   519,   103,   442,
     443,   362,  1221,   321,  1009,   322,   323,  1233,   183,   184,
     185,   186,   442,   443,    17,    66,   880,   483,    21,   440,
     529,   530,   261,   262,   263,    65,   484,   525,   104,   402,
     535,   536,   537,   538,   539,   540,   257,   258,   259,   607,
     370,   326,   260,   327,   328,   382,   226,   227,   228,   888,
     159,   105,   577,   154,   582,   236,   237,   238,   554,   555,
     442,   443,   324,   462,   909,  1235,   128,   503,   463,   504,
      66,   261,   262,   263,   168,   379,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     708,   277,   431,   344,   527,   431,  1236,   217,   576,   109,
     329,   320,   277,   276,   795,   796,    18,   797,   798,   437,
     279,    67,   917,   799,   606,   800,   405,   110,   665,    48,
     435,   442,   443,    19,    50,    51,    52,   757,   442,   443,
      53,    54,   599,   651,   652,    20,   372,   461,  1022,   760,
     120,   442,   443,   594,   653,   654,   655,   442,   443,   668,
     669,   277,   595,   446,    21,   112,   649,    68,   677,   279,
     680,   121,    69,   660,   129,   122,    67,    70,   123,   325,
      72,   115,   447,   448,   449,   450,   249,   250,   251,   252,
     452,   442,   443,   135,   442,   443,   690,   442,   443,   446,
     138,   442,   443,    73,   759,    74,   442,   443,   692,   145,
    1174,  1175,    75,   181,   550,    76,   182,   330,  -538,  -538,
     449,   450,    68,   761,   442,   443,  -538,    69,   496,   442,
     443,   439,    70,   501,   440,    72,  1092,   442,   443,   453,
     454,   455,   456,   457,   458,   459,   460,   915,   523,   916,
     890,   442,   443,   153,   670,   528,   253,   254,    73,   514,
      74,   656,   515,  1011,   671,   147,   255,    75,   256,   464,
      76,   892,   688,   898,   465,  -538,   454,   455,   456,   457,
     458,   459,   460,   154,   257,   258,   259,   672,   727,   155,
     260,   754,   673,   755,   727,   341,   518,    55,   164,   479,
     674,    60,    61,   169,   170,  1199,    62,  1201,  1101,   373,
     374,   342,   343,   466,   675,   758,   171,   172,   467,   261,
     262,   263,   264,  1217,   265,  1219,   266,   375,   267,   470,
     268,   160,   269,   772,   471,   249,   250,   251,   252,   769,
     161,   270,   472,   728,   174,   729,   730,   473,   876,   728,
     803,   729,   730,   332,   162,   643,   333,   334,   644,   893,
     165,   335,   336,   532,   271,   533,   272,   534,   273,   661,
     173,  1177,   440,   645,  1178,  1179,   446,   175,   662,  1180,
    1181,   193,   194,   889,   177,   131,   195,   132,   468,   469,
     902,   176,   885,   133,   274,   275,   276,  -538,  -538,   277,
     684,   278,   524,   685,   188,   253,   254,   279,   189,   666,
     691,   667,   190,   534,   208,   255,   421,   256,   422,   423,
     405,   271,   424,   272,   191,   273,   192,  1202,   207,   249,
     250,   251,   252,   257,   258,   259,   209,   741,   748,   260,
     440,   749,   210,  1218,   211,  1222,   910,   911,   912,   913,
     106,   107,   108,  -538,  -538,   456,   457,   458,   459,   460,
     212,  1234,   149,   150,   151,   152,   124,   125,   261,   262,
     263,   264,   996,   265,   213,   266,   214,   267,   216,   268,
    1249,   269,   487,   488,  1252,   140,   141,   142,   143,   750,
     270,   751,   749,   771,   440,   220,   440,   442,   443,   253,
     254,   222,   775,   776,    17,   515,   777,   779,   224,   255,
     780,   256,   225,   271,   229,   272,   230,   273,   231,   249,
     250,   251,   252,   881,   586,   587,   515,   257,   258,   259,
     235,   906,   232,   260,   440,   925,   928,   999,   926,   929,
     515,   768,   234,   274,   275,   276,   686,   687,   277,   444,
     278,   445,  1003,   239,  1091,   440,   279,   780,  1098,   429,
    1004,   749,   261,   262,   263,   264,   240,   265,   241,   266,
     244,   267,   782,   268,  1183,   269,   246,   515,   249,   250,
     251,   252,  1206,  1207,   270,   178,   179,   180,   331,   253,
     254,   458,   459,   460,  1214,  1215,  1014,  1015,   337,   255,
     338,   256,   340,   348,   345,   349,   350,   271,   446,   272,
     351,   273,   352,   353,   354,   356,   355,   257,   258,   259,
     357,   358,   359,   260,   360,   365,   364,   447,   448,   449,
     450,   451,   367,   368,   376,   452,   380,   274,   275,   276,
     381,   387,   277,   408,   278,  1103,   385,   386,   400,   401,
     279,   409,   261,   262,   263,   264,   403,   265,   255,   266,
     256,   267,   404,   268,   410,   269,   411,   412,  1102,   249,
     250,   251,   252,   414,   270,   474,   257,   258,   259,   415,
     416,   417,   260,   418,   453,   454,   455,   456,   457,   458,
     459,   460,   419,   420,   475,   425,   426,   271,   476,   272,
     461,   273,   784,  -247,   785,   786,   787,   788,   479,   789,
     790,   261,   262,   263,   264,   477,   265,   478,   266,   482,
     267,   486,   268,   485,   269,   489,  1182,   274,   275,   276,
     500,   490,   277,   270,   278,   509,   491,   510,   498,   400,
     279,   512,   513,   516,   521,    17,   522,   526,   541,   255,
     542,   256,   543,   544,   545,   546,   271,   547,   272,   549,
     273,   551,   196,   548,   553,   558,   578,   257,   258,   259,
     580,   277,   197,   260,   583,   198,   199,   584,   200,   201,
     202,   590,   588,   585,   589,   591,   274,   275,   276,   592,
     593,   277,   596,   278,   203,   204,   600,   205,   206,   279,
     597,   598,   261,   262,   263,   264,   602,   265,   641,   266,
      63,   267,   642,   268,     1,   269,     2,     3,     4,     5,
       6,     7,   646,     9,   270,   647,   648,   650,   676,   557,
      11,    12,    13,   663,   681,   682,    14,    15,    16,   442,
     689,   693,   695,   696,   527,   697,   698,   271,   502,   272,
       1,   273,     2,     3,     4,     5,     6,     7,     8,     9,
      10,   702,   705,   706,   707,   440,    11,    12,    13,   714,
     717,   718,    14,    15,    16,   719,   720,   274,   275,   276,
     736,   721,   277,   722,   278,    17,   743,   740,   745,     1,
     279,     2,     3,     4,     5,     6,     7,   739,     9,   723,
     724,   744,   746,   446,    77,    11,    12,    13,   725,   527,
     726,    14,    15,    16,   734,   742,   747,   752,   765,   766,
     767,    17,   447,   448,   449,   450,   762,   694,   770,   687,
     452,   686,   774,   778,    78,    79,   773,    80,   781,   804,
     783,   874,    81,    82,   875,   876,   882,   883,   884,   887,
     891,   894,   899,   900,   904,   808,   809,   810,   811,   812,
      17,   903,   813,   814,   905,   907,   908,   921,   446,   815,
     816,   817,   922,   919,   920,   927,   923,    18,   997,   453,
     454,   455,   456,   457,   458,   459,   460,   447,   448,   449,
     450,   818,   930,  1001,   939,   452,   819,   820,   821,   822,
     823,   931,   932,   824,   825,   933,    20,   934,   935,   936,
     826,   827,   828,    18,   937,   938,   940,   941,   942,   830,
     831,   832,   833,   834,   943,    21,   835,   836,   944,   950,
      19,   945,   829,   837,   838,   839,   946,   961,   947,   948,
     949,   972,    20,   951,   453,   454,   455,   456,   457,   458,
     459,   460,    18,   952,   953,   840,   954,   955,   983,   956,
     957,    21,   998,    83,    84,    85,    86,   994,    87,    88,
    1002,   958,    89,    90,    91,   959,   960,    92,    93,    94,
      95,    20,   764,   962,    96,    97,   963,  1012,  1018,   964,
     965,   966,   967,   968,   969,   970,   971,    98,    99,   973,
      21,   100,  1019,   974,  1021,   101,   608,   609,   610,   611,
     612,   613,   614,   615,   616,   617,   618,   619,   620,   621,
     622,   623,   624,  1010,   625,   626,   627,   628,   629,   630,
     975,   976,   631,   977,   978,   632,   633,   634,   635,   636,
     637,   638,   639,   841,   842,   843,   844,   845,  1023,   979,
     846,   847,   980,   981,  1016,  1024,   982,   848,   849,   850,
     852,   853,   854,   855,   856,   984,   985,   857,   858,   986,
    1025,  1026,   987,   988,   859,   860,   861,   989,   990,   851,
     991,  1027,  1028,   863,   864,   865,   866,   867,  1029,   992,
     868,   869,   993,   995,  1005,  1006,   862,   870,   871,   872,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,  1007,  1008,  1030,  1031,  1032,  1033,  1034,   873,
    1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,
    1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1055,
    1054,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1074,
    1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1089,  1090,  1093,  1094,  1095,  1097,
    1171,  1099,  1104,  1105,  1100,  1106,  1114,  1107,  1108,  1125,
    1109,  1110,  1111,  1112,  1113,  1115,  1116,  1117,  1118,  1119,
    1136,  1120,  1121,  1122,  1123,  1124,  1126,  1127,  1128,  1129,
    1130,  1131,  1147,  1132,  1133,  1134,  1135,  1158,  1137,  1138,
    1139,  1140,  1141,  1142,  1143,  1144,  1145,  1146,  1148,  1149,
    1150,  1151,  1152,  1153,  1154,  1155,  1156,  1157,  1159,  1160,
    1161,  1162,  1163,  1164,  1165,  1166,  1167,  1168,  1169,  1170,
    1176,  1172,  1184,  1173,  1185,  1186,  1187,  1188,  1189,  1190,
    1191,  1195,  1193,  1194,  1196,  1197,  1203,  1198,  1200,  1204,
    1205,  1209,  1210,  1211,  1212,  1213,  1216,  1223,  1225,  1224,
    1226,  1227,  1228,  1229,  1230,  1231,  1237,  1239,  1238,  1240,
    1241,  1242,  1243,  1246,  1247,  1244,  1245,  1248,  1250,  1251,
     163,   738,   877,   520,   126,   215,   879,   481,   361,   371,
     716,   918,  1013,  1017,   552,   531,   896,   897,   679,   579,
     581,   413,   914,   346,   886,  1000
};

static const yytype_int16 yycheck[] =
{
     244,   221,     8,    21,   640,   279,   277,   242,   763,   158,
     588,   522,   256,   664,     6,     3,   589,    55,    78,    82,
       5,     6,   253,     5,     6,    75,   175,     8,     3,    66,
       4,     5,     6,     3,   278,    62,   714,   714,    55,   714,
     714,   190,     3,   274,   275,   714,     4,   714,    76,    77,
       4,    55,     6,    20,    78,    22,     4,    63,   134,    78,
      37,    28,    16,    84,   642,    20,    76,    22,   719,   720,
       6,     6,    38,    28,    73,     3,     0,     5,     6,     4,
       5,     6,     3,     7,     3,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    55,    49,    33,    34,    35,    23,
      24,    25,    39,    40,    58,    29,    30,    31,    66,   159,
       8,   135,    78,    62,    70,    71,   135,     7,    66,    34,
     364,   365,    12,    72,    14,    79,    16,   101,   102,   103,
      66,    66,   131,   107,    62,   156,   157,   381,    34,   156,
     157,   217,   166,     3,   899,     5,     6,   166,   102,   103,
     104,   105,   156,   157,    78,     8,   734,    40,   218,   222,
     404,   405,   136,   137,   138,    63,    49,   387,    34,   400,
     414,   415,   416,   417,   418,   419,   101,   102,   103,   134,
     218,     3,   107,     5,     6,   222,   140,   141,   142,    84,
     218,    34,   463,   220,   468,   149,   150,   151,   442,   443,
     156,   157,    62,   217,   777,   135,     0,   128,   222,   130,
      63,   136,   137,   138,   206,   233,   447,   448,   449,   450,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     218,   216,   217,   187,    73,   217,   166,   218,   213,     3,
      62,   169,   216,   213,   922,   922,   170,   922,   922,   223,
     224,     8,   219,   922,   498,   922,    88,     3,    87,   196,
     278,   156,   157,   187,    33,    34,    35,    87,   156,   157,
      39,    40,   492,    68,    69,   199,   230,   220,   929,   222,
     170,   156,   157,    33,    79,    80,    81,   156,   157,   533,
     534,   216,    42,   132,   218,     3,   516,     8,   542,   224,
     544,   191,     8,   523,   221,   195,    63,     8,   198,   169,
       8,   173,   151,   152,   153,   154,     3,     4,     5,     6,
     159,   156,   157,     8,   156,   157,   557,   156,   157,   132,
      72,   156,   157,     8,   222,     8,   156,   157,   558,     3,
    1095,  1096,     8,   202,   219,     8,   205,   169,   151,   152,
     153,   154,    63,   222,   156,   157,   159,    63,   602,   156,
     157,   219,    63,   369,   222,    63,  1002,   156,   157,   208,
     209,   210,   211,   212,   213,   214,   215,    34,    65,    36,
     219,   156,   157,     3,   219,   403,    73,    74,    63,   219,
      63,   186,   222,   904,   219,    72,    83,    63,    85,   217,
      63,   759,   551,   761,   222,   208,   209,   210,   211,   212,
     213,   214,   215,   220,   101,   102,   103,   219,    73,    14,
     107,   665,   219,   667,    73,    10,   380,   196,   218,    78,
     219,   167,   168,   174,   175,  1190,   172,  1192,  1016,    33,
      34,    26,    27,   217,   219,   676,   174,   175,   222,   136,
     137,   138,   139,  1208,   141,  1210,   143,    51,   145,   217,
     147,     3,   149,   694,   222,     3,     4,     5,     6,   689,
       3,   158,   217,   128,   177,   130,   131,   222,   133,   128,
     715,   130,   131,    40,     3,   219,    43,    44,   222,   760,
       3,    48,    49,    84,   181,    86,   183,    88,   185,   219,
       3,    40,   222,   509,    43,    44,   132,    77,   526,    48,
      49,   187,   188,   757,     3,    20,   192,    22,   222,   223,
     764,     6,   742,    28,   211,   212,   213,   153,   154,   216,
     219,   218,   219,   222,    55,    73,    74,   224,   222,    84,
     558,    86,     6,    88,     4,    83,   121,    85,   123,   124,
      88,   181,   127,   183,   196,   185,   196,  1193,   196,     3,
       4,     5,     6,   101,   102,   103,   194,   219,   219,   107,
     222,   222,    76,  1209,   196,  1211,     3,     4,     5,     6,
     160,   161,   162,   209,   210,   211,   212,   213,   214,   215,
     196,  1227,    51,    52,    53,    54,    33,    34,   136,   137,
     138,   139,   876,   141,   219,   143,   219,   145,    21,   147,
    1246,   149,   189,   190,  1250,    44,    45,    46,    47,   219,
     158,   219,   222,   219,   222,    65,   222,   156,   157,    73,
      74,    67,   219,   219,    78,   222,   222,   219,    73,    83,
     222,    85,     3,   181,     3,   183,    62,   185,    62,     3,
       4,     5,     6,   219,    45,    46,   222,   101,   102,   103,
       3,   219,   218,   107,   222,   219,   219,   219,   222,   222,
     222,   689,    74,   211,   212,   213,     5,     6,   216,    73,
     218,    75,   219,     3,   219,   222,   224,   222,   219,     5,
       6,   222,   136,   137,   138,   139,     3,   141,     3,   143,
      66,   145,   708,   147,   219,   149,     4,   222,     3,     4,
       5,     6,    45,    46,   158,   201,   202,   203,   219,    73,
      74,   213,   214,   215,    43,    44,   919,   920,     3,    83,
       3,    85,     4,     4,   218,   166,     6,   181,   132,   183,
       3,   185,     6,     4,     3,    55,     4,   101,   102,   103,
       4,   196,     3,   107,     3,    68,    53,   151,   152,   153,
     154,   155,    74,   134,     3,   159,    62,   211,   212,   213,
     208,   218,   216,     4,   218,  1019,    78,    78,    73,    74,
     224,     4,   136,   137,   138,   139,   218,   141,    83,   143,
      85,   145,   218,   147,     4,   149,     4,     6,  1018,     3,
       4,     5,     6,   218,   158,     3,   101,   102,   103,   218,
     218,   218,   107,   218,   208,   209,   210,   211,   212,   213,
     214,   215,   218,   218,     6,   218,   218,   181,    47,   183,
     220,   185,    56,    57,    58,    59,    60,    61,    78,    63,
      64,   136,   137,   138,   139,    47,   141,    77,   143,     4,
     145,    77,   147,     6,   149,     4,  1100,   211,   212,   213,
       4,   219,   216,   158,   218,    55,   219,   218,    69,    73,
     224,     3,     3,   218,   218,    78,   218,   218,     4,    83,
     218,    85,   218,   218,   218,     4,   181,     4,   183,   219,
     185,    77,   161,   225,     3,   218,     6,   101,   102,   103,
       6,   216,   171,   107,     6,   174,   175,     5,   177,   178,
     179,     3,   218,    43,   218,   218,   211,   212,   213,   219,
       6,   216,     4,   218,   193,   194,    76,   196,   197,   224,
     166,   166,   136,   137,   138,   139,   222,   141,   129,   143,
       3,   145,   218,   147,     7,   149,     9,    10,    11,    12,
      13,    14,     3,    16,   158,   134,     3,   222,    76,   159,
      23,    24,    25,   219,     4,   222,    29,    30,    31,   156,
     218,   131,   225,   225,    73,     6,     6,   181,     3,   183,
       7,   185,     9,    10,    11,    12,    13,    14,    15,    16,
      17,     3,     6,     4,     4,   222,    23,    24,    25,   180,
       4,   218,    29,    30,    31,   218,   218,   211,   212,   213,
      32,   218,   216,   218,   218,    78,     6,     4,     4,     7,
     224,     9,    10,    11,    12,    13,    14,   219,    16,   218,
     218,     6,     3,   132,     3,    23,    24,    25,   218,    73,
     218,    29,    30,    31,   218,   218,     6,   219,   219,     4,
      78,    78,   151,   152,   153,   154,   222,   156,   219,     6,
     159,     5,    47,   208,    33,    34,    50,    36,     6,     6,
     222,   131,    41,    42,   129,   133,   218,   134,   166,   219,
     216,   216,     4,   222,   218,    90,    91,    92,    93,    94,
      78,   219,    97,    98,   219,     6,     6,     3,   132,   104,
     105,   106,   222,    57,    57,   219,    52,   170,   134,   208,
     209,   210,   211,   212,   213,   214,   215,   151,   152,   153,
     154,   126,   222,     4,    92,   159,    90,    91,    92,    93,
      94,   222,   222,    97,    98,   222,   199,   222,   222,   222,
     104,   105,   106,   170,   222,   222,   222,   222,   222,    90,
      91,    92,    93,    94,   222,   218,    97,    98,   222,    92,
     187,   222,   126,   104,   105,   106,   222,    92,   222,   222,
     222,    92,   199,   222,   208,   209,   210,   211,   212,   213,
     214,   215,   170,   222,   222,   126,   222,   222,    92,   222,
     222,   218,   134,   162,   163,   164,   165,    92,   167,   168,
       3,   222,   171,   172,   173,   222,   222,   176,   177,   178,
     179,   199,    66,   222,   183,   184,   222,     6,    53,   222,
     222,   222,   222,   222,   222,   222,   222,   196,   197,   222,
     218,   200,    54,   222,     6,   204,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   219,   107,   108,   109,   110,   111,   112,
     222,   222,   115,   222,   222,   118,   119,   120,   121,   122,
     123,   124,   125,    90,    91,    92,    93,    94,     6,   222,
      97,    98,   222,   222,   218,     6,   222,   104,   105,   106,
      90,    91,    92,    93,    94,   222,   222,    97,    98,   222,
       6,     6,   222,   222,   104,   105,   106,   222,   222,   126,
     222,     6,     6,    90,    91,    92,    93,    94,     6,   222,
      97,    98,   222,   222,   222,   222,   126,   104,   105,   106,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   222,   222,     6,     6,   222,     6,     6,   126,
       6,     6,     6,     6,     6,     6,     6,     6,   222,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     222,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     222,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   222,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   222,     6,     4,     4,     4,     4,     4,   219,
     222,     6,   219,   219,    62,   219,     6,   219,   219,     6,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
       6,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,     6,   219,   219,   219,   219,     6,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,     6,   219,
     166,   222,   219,   222,   219,   219,   219,   219,   219,     4,
       4,    49,   219,   219,     6,     6,   166,   222,   222,   218,
       6,   219,     4,   219,    49,     4,     6,   218,     6,   219,
      48,   219,   219,    40,    40,     4,   218,    40,   219,    40,
      40,    40,     3,   219,     3,   218,   218,     3,   219,   219,
      63,   644,   731,   382,    21,   129,   733,   345,   217,   228,
     602,   783,   917,   922,   440,   406,   761,   761,   543,   465,
     467,   263,   780,   189,   749,   882
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    78,   170,   187,
     199,   218,   227,   228,   229,   231,   243,   244,   246,   247,
     250,   251,   252,   253,   254,   278,   283,   284,   285,   286,
     287,   288,   289,    33,    34,    35,    39,    40,   196,    37,
      33,    34,    35,    39,    40,   196,     3,   276,    76,   276,
     167,   168,   172,     3,   230,   231,   243,   244,   247,   250,
     251,   252,   283,   284,   285,   286,   287,     3,    33,    34,
      36,    41,    42,   162,   163,   164,   165,   167,   168,   171,
     172,   173,   176,   177,   178,   179,   183,   184,   196,   197,
     200,   204,    34,    34,    34,    34,   160,   161,   162,     3,
       3,   276,     3,   279,   280,   173,     7,    12,    14,    16,
     170,   191,   195,   198,    33,    34,   253,   254,     0,   221,
     338,    20,    22,    28,   272,     8,   255,   257,    72,   337,
     337,   337,   337,   337,   339,     3,   276,    72,   336,   336,
     336,   336,   336,     3,   220,    14,   276,    76,    77,   218,
       3,     3,     3,   230,   218,     3,   276,     6,   206,   174,
     175,   174,   175,     3,   177,    77,     6,     3,   201,   202,
     203,   202,   205,   276,   276,   276,   276,    62,    55,   222,
       6,   196,   196,   187,   188,   192,   161,   171,   174,   175,
     177,   178,   179,   193,   194,   196,   197,   196,     4,   194,
      76,   196,   196,   219,   219,   229,    21,   218,   256,   257,
      65,   264,    67,   258,    73,     3,   276,   276,   276,     3,
      62,    62,   218,   245,    74,     3,   276,   276,   276,     3,
       3,     3,   248,   249,    66,   269,     4,   335,   335,     3,
       4,     5,     6,    73,    74,    83,    85,   101,   102,   103,
     107,   136,   137,   138,   139,   141,   143,   145,   147,   149,
     158,   181,   183,   185,   211,   212,   213,   216,   218,   224,
     290,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     303,   304,   305,   306,   307,   309,   310,   311,   312,   313,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     327,   328,   329,   330,   331,   332,     3,     5,     6,    62,
     169,     3,     5,     6,    62,   169,     3,     5,     6,    62,
     169,   219,    40,    43,    44,    48,    49,     3,     3,   335,
       4,    10,    26,    27,   276,   218,   280,   335,     4,   166,
       6,     3,     6,     4,     3,     4,    55,     4,   196,     3,
       3,   256,   257,   290,    53,    68,   262,    74,   134,    55,
     218,   245,   276,    33,    34,    51,     3,   242,    38,   254,
      62,   208,   222,   269,   293,    78,    78,   218,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
      73,    74,   294,   218,   218,    88,   293,   308,     4,     4,
       4,     4,     6,   332,   218,   218,   218,   218,   218,   218,
     218,   121,   123,   124,   127,   218,   218,   294,   294,     5,
       6,   217,   313,   325,   326,   254,   293,   223,   312,   219,
     222,    55,   156,   157,    73,    75,   132,   151,   152,   153,
     154,   155,   159,   208,   209,   210,   211,   212,   213,   214,
     215,   220,   217,   222,   217,   222,   217,   222,   222,   223,
     217,   222,   217,   222,     3,     6,    47,    47,    77,    78,
     340,   255,     4,    40,    49,     6,    77,   189,   190,     4,
     219,   219,    82,   265,   259,   260,   293,   293,    69,   263,
       4,   252,     3,   128,   130,   232,   234,   235,   241,    55,
     218,   344,     3,     3,   219,   222,   218,   291,   276,   293,
     249,   218,   218,    65,   219,   290,   218,    73,   254,   293,
     293,   308,    84,    86,    88,   293,   293,   293,   293,   293,
     293,     4,   218,   218,   218,   218,     4,     4,   225,   219,
     219,    77,   292,     3,   293,   293,    75,   159,   218,    73,
     131,   294,   294,   294,   294,   294,   294,   294,   294,   294,
     294,   294,   294,   294,   294,     3,   213,   313,     6,   325,
       6,   326,   312,     6,     5,    43,    45,    46,   218,   218,
       3,   218,   219,     6,    33,    42,     4,   166,   166,   290,
      76,   266,   222,    70,    71,   261,   293,   134,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   107,   108,   109,   110,   111,
     112,   115,   118,   119,   120,   121,   122,   123,   124,   125,
     237,   129,   218,   219,   222,   252,     3,   134,     3,   290,
     222,    68,    69,    79,    80,    81,   186,   333,   334,   333,
     290,   219,   254,   219,    55,    87,    84,    86,   293,   293,
     219,   219,   219,   219,   219,   219,    76,   293,     3,   311,
     293,     4,   222,   268,   219,   222,     5,     6,   335,   218,
     294,   254,   290,   131,   156,   225,   225,     6,     6,   242,
     233,   235,     3,   342,   343,     6,     4,     4,   218,   273,
     274,   275,   276,   281,   180,   267,   260,     4,   218,   218,
     218,   218,   218,   218,   218,   218,   218,    73,   128,   130,
     131,   238,   239,   340,   218,   242,    32,   341,   234,   219,
       4,   219,   218,     6,     6,     4,     3,     6,   219,   222,
     219,   219,   219,   237,   293,   293,    84,    87,   294,   222,
     222,   222,   222,     4,    66,   219,     4,    78,   254,   290,
     219,   219,   294,    50,    47,   219,   219,   222,   208,   219,
     222,     6,   252,   222,    56,    58,    59,    60,    61,    63,
      64,   282,     3,    55,   277,   295,   296,   297,   298,   299,
     300,   301,   302,   269,     6,   237,   236,   237,    90,    91,
      92,    93,    94,    97,    98,   104,   105,   106,   126,    90,
      91,    92,    93,    94,    97,    98,   104,   105,   106,   126,
      90,    91,    92,    93,    94,    97,    98,   104,   105,   106,
     126,    90,    91,    92,    93,    94,    97,    98,   104,   105,
     106,   126,    90,    91,    92,    93,    94,    97,    98,   104,
     105,   106,   126,    90,    91,    92,    93,    94,    97,    98,
     104,   105,   106,   126,   131,   129,   133,   239,   240,   240,
     242,   219,   218,   134,   166,   290,   334,   219,    84,   293,
     219,   216,   327,   313,   216,   314,   317,   322,   327,     4,
     222,   268,   293,   219,   218,   219,   219,     6,     6,   235,
       3,     4,     5,     6,   343,    34,    36,   219,   274,    57,
      57,     3,   222,    52,   271,   219,   222,   219,   219,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,    92,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
      92,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,    92,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,    92,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,    92,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,    92,   222,   312,   134,   134,   219,
     342,     4,     3,   219,     6,   222,   222,   222,   222,   268,
     219,   333,     6,   277,   275,   275,   218,   301,    53,    54,
     270,     6,   237,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   222,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   222,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   222,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   222,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   222,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   222,     6,     4,
       4,   219,   340,     4,     4,     4,     4,   219,   219,     6,
      62,   242,   290,   293,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,     6,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,     6,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,     6,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,     6,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,     6,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,     6,
     219,   222,   222,   222,   268,   268,   166,    40,    43,    44,
      48,    49,   293,   219,   219,   219,   219,   219,   219,   219,
       4,     4,     6,   219,   219,    49,     6,     6,   222,   268,
     222,   268,   340,   166,   218,     6,    45,    46,     6,   219,
       4,   219,    49,     4,    43,    44,     6,   268,   340,   268,
     135,   166,   340,   218,   219,     6,    48,   219,   219,    40,
      40,     4,   135,   166,   340,   135,   166,   218,   219,    40,
      40,    40,    40,     3,   218,   218,   219,     3,     3,   340,
     219,   219,   340
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   226,   227,   228,   228,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   232,   232,   233,   233,   234,   234,   235,   235,
     235,   235,   236,   236,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   238,   238,
     239,   239,   239,   239,   240,   240,   241,   241,   242,   242,
     243,   244,   244,   245,   245,   246,   246,   247,   248,   248,
     249,   250,   250,   250,   250,   250,   251,   251,   251,   252,
     252,   252,   252,   253,   253,   254,   255,   256,   256,   257,
     258,   258,   259,   259,   260,   261,   261,   261,   262,   262,
     263,   263,   264,   264,   265,   265,   266,   266,   267,   267,
     268,   268,   269,   269,   270,   270,   271,   271,   272,   272,
     272,   272,   273,   273,   274,   274,   275,   275,   276,   276,
     277,   277,   277,   277,   278,   278,   279,   279,   280,   281,
     281,   282,   282,   282,   282,   282,   282,   282,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   284,   284,   284,   285,   285,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   287,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     289,   289,   289,   290,   290,   291,   291,   292,   292,   293,
     293,   293,   293,   293,   294,   294,   294,   294,   294,   294,
     294,   294,   294,   294,   294,   294,   294,   295,   295,   295,
     296,   296,   296,   296,   297,   297,   297,   297,   298,   298,
     298,   298,   299,   299,   300,   300,   301,   301,   301,   301,
     301,   301,   302,   302,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   304,   304,   305,   306,   306,   307,   307,
     307,   307,   308,   308,   309,   310,   310,   310,   310,   311,
     311,   311,   311,   312,   312,   312,   312,   312,   312,   312,
     312,   312,   312,   312,   312,   312,   313,   313,   313,   313,
     314,   314,   314,   315,   316,   316,   317,   317,   318,   319,
     319,   320,   321,   321,   322,   323,   323,   324,   324,   325,
     326,   327,   327,   328,   329,   329,   330,   331,   331,   332,
     332,   332,   332,   332,   332,   332,   332,   332,   332,   332,
     332,   333,   333,   334,   334,   334,   334,   334,   334,   335,
     336,   336,   337,   337,   338,   338,   339,   339,   340,   340,
     341,   341,   342,   342,   343,   343,   343,   343,   343,   344,
     344
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
       1,     3,     3,     3,     3,     2,     4,     3,     3,     9,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     2,     0,     2,     0,     3,     0,     1,     2,
       1,     1,     1,     3,     1,     1,     2,     4,     1,     3,
       2,     1,     5,     0,     2,     0,     1,     3,     5,     4,
       6,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     2,     3,     2,     2,     2,     2,     3,     2,     3,
       3,     3,     3,     4,     4,     3,     3,     4,     4,     5,
       6,     7,     9,     4,     5,     7,     9,     2,     3,     2,
       2,     4,     3,     3,     4,     2,     3,     3,     4,     2,
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
#line 2479 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2487 "parser.cpp"
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
#line 2501 "parser.cpp"
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
#line 2515 "parser.cpp"
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
#line 2529 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2538 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 184 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 321 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2558 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2566 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2575 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 332 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2584 "parser.cpp"
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
#line 2598 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 285 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2609 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2619 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2639 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2649 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2659 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 367 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2669 "parser.cpp"
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
#line 2683 "parser.cpp"
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
#line 2697 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 355 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2707 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2715 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2723 "parser.cpp"
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
#line 2737 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2746 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2754 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2762 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2778 "parser.cpp"
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
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2801 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2810 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2819 "parser.cpp"
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
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2841 "parser.cpp"
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
#line 2855 "parser.cpp"
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
#line 2869 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 361 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 350 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2888 "parser.cpp"
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
#line 2902 "parser.cpp"
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
#line 2916 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2924 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2932 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2996 "parser.cpp"
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
#line 3010 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3018 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3026 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3034 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3042 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3050 "parser.cpp"
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
#line 3063 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3071 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3079 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 337 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3151 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3159 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3167 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3175 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3183 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3191 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 382 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3199 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 386 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3207 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3215 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3223 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3231 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3239 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3247 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 341 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3255 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 328 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3263 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 218 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3274 "parser.cpp"
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
#line 3288 "parser.cpp"
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
#line 3302 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 194 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3313 "parser.cpp"
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

#line 3421 "parser.cpp"

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
#line 3636 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 521 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3647 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 527 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3658 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 534 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3664 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 535 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3670 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 536 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3676 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 537 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3682 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 538 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3688 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 539 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3694 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 540 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3700 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3706 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 542 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3712 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 543 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3718 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 544 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3724 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 545 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3730 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 546 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3736 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3742 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3748 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 550 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3754 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 551 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3760 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 552 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3766 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 553 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3772 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 554 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3778 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 555 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3784 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 556 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3790 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 557 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3796 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 558 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3802 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 559 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3808 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 560 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3814 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 561 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3820 "parser.cpp"
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
#line 3842 "parser.cpp"
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
#line 3862 "parser.cpp"
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
#line 3880 "parser.cpp"
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
#line 3913 "parser.cpp"
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
#line 3933 "parser.cpp"
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
#line 3969 "parser.cpp"
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
#line 3991 "parser.cpp"
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
#line 4012 "parser.cpp"
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
#line 4045 "parser.cpp"
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
#line 4080 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element  */
#line 789 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4089 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element_array ',' table_element  */
#line 793 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4098 "parser.cpp"
    break;

  case 44: /* column_def_array: table_column  */
#line 798 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4107 "parser.cpp"
    break;

  case 45: /* column_def_array: column_def_array ',' table_column  */
#line 802 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4116 "parser.cpp"
    break;

  case 46: /* table_element: table_column  */
#line 808 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4124 "parser.cpp"
    break;

  case 47: /* table_element: table_constraint  */
#line 811 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4132 "parser.cpp"
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
#line 4157 "parser.cpp"
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
#line 4184 "parser.cpp"
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
#line 4213 "parser.cpp"
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
#line 4243 "parser.cpp"
    break;

  case 52: /* column_type_array: column_type  */
#line 910 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4252 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type_array ',' column_type  */
#line 914 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4261 "parser.cpp"
    break;

  case 54: /* column_type: BOOLEAN  */
#line 920 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4267 "parser.cpp"
    break;

  case 55: /* column_type: TINYINT  */
#line 921 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4273 "parser.cpp"
    break;

  case 56: /* column_type: SMALLINT  */
#line 922 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4279 "parser.cpp"
    break;

  case 57: /* column_type: INTEGER  */
#line 923 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4285 "parser.cpp"
    break;

  case 58: /* column_type: INT  */
#line 924 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4291 "parser.cpp"
    break;

  case 59: /* column_type: BIGINT  */
#line 925 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4297 "parser.cpp"
    break;

  case 60: /* column_type: HUGEINT  */
#line 926 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4303 "parser.cpp"
    break;

  case 61: /* column_type: FLOAT  */
#line 927 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4309 "parser.cpp"
    break;

  case 62: /* column_type: REAL  */
#line 928 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4315 "parser.cpp"
    break;

  case 63: /* column_type: DOUBLE  */
#line 929 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4321 "parser.cpp"
    break;

  case 64: /* column_type: FLOAT16  */
#line 930 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4327 "parser.cpp"
    break;

  case 65: /* column_type: BFLOAT16  */
#line 931 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4333 "parser.cpp"
    break;

  case 66: /* column_type: DATE  */
#line 932 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4339 "parser.cpp"
    break;

  case 67: /* column_type: TIME  */
#line 933 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4345 "parser.cpp"
    break;

  case 68: /* column_type: DATETIME  */
#line 934 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4351 "parser.cpp"
    break;

  case 69: /* column_type: TIMESTAMP  */
#line 935 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4357 "parser.cpp"
    break;

  case 70: /* column_type: UUID  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4363 "parser.cpp"
    break;

  case 71: /* column_type: POINT  */
#line 937 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4369 "parser.cpp"
    break;

  case 72: /* column_type: LINE  */
#line 938 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4375 "parser.cpp"
    break;

  case 73: /* column_type: LSEG  */
#line 939 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4381 "parser.cpp"
    break;

  case 74: /* column_type: BOX  */
#line 940 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4387 "parser.cpp"
    break;

  case 75: /* column_type: CIRCLE  */
#line 943 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4393 "parser.cpp"
    break;

  case 76: /* column_type: VARCHAR  */
#line 945 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4399 "parser.cpp"
    break;

  case 77: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 946 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4405 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 947 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4411 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL  */
#line 948 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4417 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4423 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 952 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4429 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 953 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4435 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4441 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4447 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4453 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4459 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4465 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4471 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4477 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4483 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4489 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4495 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4501 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4507 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4513 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4519 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4525 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4531 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4537 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4543 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4549 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4555 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4561 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4567 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4573 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4579 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4585 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4591 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4597 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4603 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4609 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4615 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4621 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4627 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4633 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4639 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4645 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4651 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4657 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4663 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4669 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4675 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4681 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4687 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4693 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4699 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4705 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4711 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4717 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4723 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4729 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4735 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4741 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4747 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4753 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4759 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4765 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4771 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4777 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4783 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4789 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4795 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4801 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4807 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4813 "parser.cpp"
    break;

  case 146: /* column_type: ARRAY '(' column_type ')'  */
#line 1017 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4822 "parser.cpp"
    break;

  case 147: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1021 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4832 "parser.cpp"
    break;

  case 148: /* column_constraints: column_constraint  */
#line 1044 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4841 "parser.cpp"
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
#line 4855 "parser.cpp"
    break;

  case 150: /* column_constraint: PRIMARY KEY  */
#line 1058 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4863 "parser.cpp"
    break;

  case 151: /* column_constraint: UNIQUE  */
#line 1061 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4871 "parser.cpp"
    break;

  case 152: /* column_constraint: NULLABLE  */
#line 1064 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4879 "parser.cpp"
    break;

  case 153: /* column_constraint: NOT NULLABLE  */
#line 1067 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4887 "parser.cpp"
    break;

  case 154: /* default_expr: DEFAULT constant_expr  */
#line 1071 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4895 "parser.cpp"
    break;

  case 155: /* default_expr: %empty  */
#line 1074 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4903 "parser.cpp"
    break;

  case 156: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1079 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4913 "parser.cpp"
    break;

  case 157: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1084 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4923 "parser.cpp"
    break;

  case 158: /* identifier_array: IDENTIFIER  */
#line 1091 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4934 "parser.cpp"
    break;

  case 159: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1097 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4945 "parser.cpp"
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
#line 4962 "parser.cpp"
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
#line 5005 "parser.cpp"
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
#line 5025 "parser.cpp"
    break;

  case 163: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1177 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5033 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: %empty  */
#line 1180 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5041 "parser.cpp"
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
    else {
        free (yyvsp[-1].str_value);
        yyerror(&yyloc, scanner, result, "invalid explain option");
        YYERROR;
    }
    free((yyvsp[-1].str_value));
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5064 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN explainable_statement  */
#line 1204 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5074 "parser.cpp"
    break;

  case 167: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1213 "parser.y"
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
#line 5091 "parser.cpp"
    break;

  case 168: /* update_expr_array: update_expr  */
#line 1226 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5100 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1230 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5109 "parser.cpp"
    break;

  case 170: /* update_expr: IDENTIFIER '=' expr  */
#line 1235 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5121 "parser.cpp"
    break;

  case 171: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1248 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5137 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1261 "parser.y"
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
#line 5155 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1276 "parser.y"
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
#line 5173 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1291 "parser.y"
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
#line 5191 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1306 "parser.y"
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
#line 5214 "parser.cpp"
    break;

  case 176: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1329 "parser.y"
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
#line 5272 "parser.cpp"
    break;

  case 177: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1382 "parser.y"
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
#line 5332 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1437 "parser.y"
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
#line 5384 "parser.cpp"
    break;

  case 179: /* select_statement: select_without_paren  */
#line 1488 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5392 "parser.cpp"
    break;

  case 180: /* select_statement: select_with_paren  */
#line 1491 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5400 "parser.cpp"
    break;

  case 181: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1494 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5414 "parser.cpp"
    break;

  case 182: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1503 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5428 "parser.cpp"
    break;

  case 183: /* select_with_paren: '(' select_without_paren ')'  */
#line 1513 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5436 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_with_paren ')'  */
#line 1516 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5444 "parser.cpp"
    break;

  case 185: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1520 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5453 "parser.cpp"
    break;

  case 186: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1525 "parser.y"
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
#line 5484 "parser.cpp"
    break;

  case 187: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1552 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5492 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1555 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5500 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1560 "parser.y"
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
#line 5521 "parser.cpp"
    break;

  case 190: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1577 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5529 "parser.cpp"
    break;

  case 191: /* order_by_clause: %empty  */
#line 1580 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5537 "parser.cpp"
    break;

  case 192: /* order_by_expr_list: order_by_expr  */
#line 1584 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5546 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1588 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5555 "parser.cpp"
    break;

  case 194: /* order_by_expr: expr order_by_type  */
#line 1593 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5565 "parser.cpp"
    break;

  case 195: /* order_by_type: ASC  */
#line 1599 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5573 "parser.cpp"
    break;

  case 196: /* order_by_type: DESC  */
#line 1602 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5581 "parser.cpp"
    break;

  case 197: /* order_by_type: %empty  */
#line 1605 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5589 "parser.cpp"
    break;

  case 198: /* limit_expr: LIMIT expr  */
#line 1609 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5597 "parser.cpp"
    break;

  case 199: /* limit_expr: %empty  */
#line 1613 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5603 "parser.cpp"
    break;

  case 200: /* offset_expr: OFFSET expr  */
#line 1615 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5611 "parser.cpp"
    break;

  case 201: /* offset_expr: %empty  */
#line 1619 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5617 "parser.cpp"
    break;

  case 202: /* distinct: DISTINCT  */
#line 1621 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5625 "parser.cpp"
    break;

  case 203: /* distinct: %empty  */
#line 1624 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5633 "parser.cpp"
    break;

  case 204: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1628 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5641 "parser.cpp"
    break;

  case 205: /* highlight_clause: %empty  */
#line 1631 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5649 "parser.cpp"
    break;

  case 206: /* from_clause: FROM table_reference  */
#line 1635 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5657 "parser.cpp"
    break;

  case 207: /* from_clause: %empty  */
#line 1638 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5665 "parser.cpp"
    break;

  case 208: /* search_clause: SEARCH sub_search_array  */
#line 1642 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5675 "parser.cpp"
    break;

  case 209: /* search_clause: %empty  */
#line 1647 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5683 "parser.cpp"
    break;

  case 210: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1651 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5691 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: %empty  */
#line 1654 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5699 "parser.cpp"
    break;

  case 212: /* where_clause: WHERE expr  */
#line 1658 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5707 "parser.cpp"
    break;

  case 213: /* where_clause: %empty  */
#line 1661 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5715 "parser.cpp"
    break;

  case 214: /* having_clause: HAVING expr  */
#line 1665 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5723 "parser.cpp"
    break;

  case 215: /* having_clause: %empty  */
#line 1668 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5731 "parser.cpp"
    break;

  case 216: /* group_by_clause: GROUP BY expr_array  */
#line 1672 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5739 "parser.cpp"
    break;

  case 217: /* group_by_clause: %empty  */
#line 1675 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5747 "parser.cpp"
    break;

  case 218: /* set_operator: UNION  */
#line 1679 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5755 "parser.cpp"
    break;

  case 219: /* set_operator: UNION ALL  */
#line 1682 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5763 "parser.cpp"
    break;

  case 220: /* set_operator: INTERSECT  */
#line 1685 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5771 "parser.cpp"
    break;

  case 221: /* set_operator: EXCEPT  */
#line 1688 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5779 "parser.cpp"
    break;

  case 222: /* table_reference: table_reference_unit  */
#line 1696 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5787 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference ',' table_reference_unit  */
#line 1699 "parser.y"
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
#line 5805 "parser.cpp"
    break;

  case 226: /* table_reference_name: table_name table_alias  */
#line 1716 "parser.y"
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
#line 5823 "parser.cpp"
    break;

  case 227: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1730 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5834 "parser.cpp"
    break;

  case 228: /* table_name: IDENTIFIER  */
#line 1739 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5844 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1744 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5856 "parser.cpp"
    break;

  case 230: /* table_alias: AS IDENTIFIER  */
#line 1753 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5866 "parser.cpp"
    break;

  case 231: /* table_alias: IDENTIFIER  */
#line 1758 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5876 "parser.cpp"
    break;

  case 232: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1763 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5887 "parser.cpp"
    break;

  case 233: /* table_alias: %empty  */
#line 1769 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5895 "parser.cpp"
    break;

  case 234: /* with_clause: WITH with_expr_list  */
#line 1776 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5903 "parser.cpp"
    break;

  case 235: /* with_clause: %empty  */
#line 1779 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5911 "parser.cpp"
    break;

  case 236: /* with_expr_list: with_expr  */
#line 1783 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5920 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1786 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5929 "parser.cpp"
    break;

  case 238: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1791 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5941 "parser.cpp"
    break;

  case 239: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1803 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5953 "parser.cpp"
    break;

  case 240: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1810 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5966 "parser.cpp"
    break;

  case 241: /* join_type: INNER  */
#line 1824 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5974 "parser.cpp"
    break;

  case 242: /* join_type: LEFT  */
#line 1827 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5982 "parser.cpp"
    break;

  case 243: /* join_type: RIGHT  */
#line 1830 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5990 "parser.cpp"
    break;

  case 244: /* join_type: OUTER  */
#line 1833 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5998 "parser.cpp"
    break;

  case 245: /* join_type: FULL  */
#line 1836 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6006 "parser.cpp"
    break;

  case 246: /* join_type: CROSS  */
#line 1839 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6014 "parser.cpp"
    break;

  case 247: /* join_type: %empty  */
#line 1842 "parser.y"
                {
}
#line 6021 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW DATABASES  */
#line 1848 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6030 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TABLES  */
#line 1852 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6039 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW VIEWS  */
#line 1856 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 6048 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW CONFIGS  */
#line 1860 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6057 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1864 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6069 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW PROFILES  */
#line 1871 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6078 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW BUFFER  */
#line 1875 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6087 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW MEMINDEX  */
#line 1879 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6096 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW QUERIES  */
#line 1883 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6105 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1887 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6115 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TRANSACTIONS  */
#line 1892 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6124 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1896 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6134 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1901 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6143 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW SESSION VARIABLES  */
#line 1905 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6152 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1909 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6161 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1913 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6172 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1919 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6183 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1925 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6194 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name  */
#line 1931 "parser.y"
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
#line 6210 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1942 "parser.y"
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
#line 6226 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1953 "parser.y"
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
#line 6242 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1964 "parser.y"
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
#line 6259 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1976 "parser.y"
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
#line 6276 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1988 "parser.y"
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
#line 6294 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 2001 "parser.y"
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
#line 6313 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 2015 "parser.y"
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
#line 6329 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 2026 "parser.y"
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
#line 6348 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2040 "parser.y"
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
#line 6369 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2056 "parser.y"
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
#line 6391 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW LOGS  */
#line 2073 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6400 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW DELTA LOGS  */
#line 2077 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6409 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOGS  */
#line 2081 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6418 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG  */
#line 2085 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6427 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW CATALOG TO file_path  */
#line 2089 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6438 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2095 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6447 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2099 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6456 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2103 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6467 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY  */
#line 2109 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6476 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2113 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6485 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2117 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6494 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2121 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6505 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOTS  */
#line 2127 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6514 "parser.cpp"
    break;

  case 290: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2131 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6525 "parser.cpp"
    break;

  case 291: /* flush_statement: FLUSH DATA  */
#line 2141 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6534 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH LOG  */
#line 2145 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6543 "parser.cpp"
    break;

  case 293: /* flush_statement: FLUSH BUFFER  */
#line 2149 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6552 "parser.cpp"
    break;

  case 294: /* optimize_statement: OPTIMIZE table_name  */
#line 2157 "parser.y"
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
#line 6567 "parser.cpp"
    break;

  case 295: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2168 "parser.y"
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
#line 6591 "parser.cpp"
    break;

  case 296: /* command_statement: USE IDENTIFIER  */
#line 2191 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6602 "parser.cpp"
    break;

  case 297: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2197 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6612 "parser.cpp"
    break;

  case 298: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2202 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6623 "parser.cpp"
    break;

  case 299: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2208 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6634 "parser.cpp"
    break;

  case 300: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2214 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6647 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2222 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6658 "parser.cpp"
    break;

  case 302: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2228 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6669 "parser.cpp"
    break;

  case 303: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2234 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6680 "parser.cpp"
    break;

  case 304: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2240 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6691 "parser.cpp"
    break;

  case 305: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2246 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6704 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2254 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6715 "parser.cpp"
    break;

  case 307: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2260 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6726 "parser.cpp"
    break;

  case 308: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2266 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6737 "parser.cpp"
    break;

  case 309: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2272 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6748 "parser.cpp"
    break;

  case 310: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2278 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6761 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2286 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6772 "parser.cpp"
    break;

  case 312: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2292 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6783 "parser.cpp"
    break;

  case 313: /* command_statement: LOCK TABLE table_name  */
#line 2298 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6797 "parser.cpp"
    break;

  case 314: /* command_statement: UNLOCK TABLE table_name  */
#line 2307 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6811 "parser.cpp"
    break;

  case 315: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2316 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6824 "parser.cpp"
    break;

  case 316: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2324 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6837 "parser.cpp"
    break;

  case 317: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2332 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6848 "parser.cpp"
    break;

  case 318: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2338 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6859 "parser.cpp"
    break;

  case 319: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2344 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6870 "parser.cpp"
    break;

  case 320: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2350 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6881 "parser.cpp"
    break;

  case 321: /* compact_statement: COMPACT TABLE table_name  */
#line 2357 "parser.y"
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
#line 6898 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2370 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6907 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2374 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6917 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2379 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6928 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2385 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6940 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2392 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6953 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2400 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6967 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2409 "parser.y"
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
#line 6982 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2419 "parser.y"
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
#line 6997 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2429 "parser.y"
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
#line 7013 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2440 "parser.y"
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
#line 7029 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2451 "parser.y"
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
#line 7046 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2463 "parser.y"
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
#line 7063 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2475 "parser.y"
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
#line 7078 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2485 "parser.y"
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
#line 7094 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2496 "parser.y"
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
#line 7111 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2508 "parser.y"
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
#line 7129 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOGS  */
#line 2521 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7138 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2525 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7148 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2530 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7158 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2535 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7169 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2541 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7178 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2545 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7187 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2549 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7198 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2555 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7207 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2559 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7216 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2563 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7227 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2569 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7238 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2575 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7251 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2583 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7262 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODES  */
#line 2589 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7271 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2593 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7282 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN SHOW NODE  */
#line 2599 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7291 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2603 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7302 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET ADMIN  */
#line 2609 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7312 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET STANDALONE  */
#line 2614 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7322 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2619 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7334 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2626 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7348 "parser.cpp"
    break;

  case 359: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2635 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7362 "parser.cpp"
    break;

  case 360: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2645 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7376 "parser.cpp"
    break;

  case 361: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2654 "parser.y"
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
#line 7393 "parser.cpp"
    break;

  case 362: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2666 "parser.y"
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
#line 7409 "parser.cpp"
    break;

  case 363: /* expr_array: expr_alias  */
#line 2682 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7418 "parser.cpp"
    break;

  case 364: /* expr_array: expr_array ',' expr_alias  */
#line 2686 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7427 "parser.cpp"
    break;

  case 365: /* insert_row_list: '(' expr_array ')'  */
#line 2691 "parser.y"
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
#line 7442 "parser.cpp"
    break;

  case 366: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2701 "parser.y"
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
#line 7457 "parser.cpp"
    break;

  case 367: /* expr_alias: expr AS IDENTIFIER  */
#line 2723 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7468 "parser.cpp"
    break;

  case 368: /* expr_alias: expr  */
#line 2729 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7476 "parser.cpp"
    break;

  case 374: /* operand: '(' expr ')'  */
#line 2739 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7484 "parser.cpp"
    break;

  case 375: /* operand: '(' select_without_paren ')'  */
#line 2742 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7495 "parser.cpp"
    break;

  case 376: /* operand: constant_expr  */
#line 2748 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7503 "parser.cpp"
    break;

  case 387: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2764 "parser.y"
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
#line 7523 "parser.cpp"
    break;

  case 388: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2780 "parser.y"
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
#line 7544 "parser.cpp"
    break;

  case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2797 "parser.y"
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
#line 7565 "parser.cpp"
    break;

  case 390: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2815 "parser.y"
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
#line 7615 "parser.cpp"
    break;

  case 391: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2861 "parser.y"
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
#line 7658 "parser.cpp"
    break;

  case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2900 "parser.y"
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
#line 7705 "parser.cpp"
    break;

  case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2943 "parser.y"
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
#line 7753 "parser.cpp"
    break;

  case 394: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2990 "parser.y"
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
#line 7781 "parser.cpp"
    break;

  case 395: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3014 "parser.y"
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
#line 7808 "parser.cpp"
    break;

  case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3037 "parser.y"
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
#line 7833 "parser.cpp"
    break;

  case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3058 "parser.y"
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
#line 7858 "parser.cpp"
    break;

  case 398: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3079 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7872 "parser.cpp"
    break;

  case 399: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3088 "parser.y"
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
#line 7888 "parser.cpp"
    break;

  case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3099 "parser.y"
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
#line 7904 "parser.cpp"
    break;

  case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3110 "parser.y"
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
#line 7922 "parser.cpp"
    break;

  case 402: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3124 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7934 "parser.cpp"
    break;

  case 403: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3131 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7948 "parser.cpp"
    break;

  case 404: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3141 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7959 "parser.cpp"
    break;

  case 405: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3147 "parser.y"
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
#line 7975 "parser.cpp"
    break;

  case 406: /* sub_search: match_vector_expr  */
#line 3159 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7983 "parser.cpp"
    break;

  case 407: /* sub_search: match_text_expr  */
#line 3162 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7991 "parser.cpp"
    break;

  case 408: /* sub_search: match_tensor_expr  */
#line 3165 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7999 "parser.cpp"
    break;

  case 409: /* sub_search: match_sparse_expr  */
#line 3168 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8007 "parser.cpp"
    break;

  case 410: /* sub_search: query_expr  */
#line 3171 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8015 "parser.cpp"
    break;

  case 411: /* sub_search: fusion_expr  */
#line 3174 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8023 "parser.cpp"
    break;

  case 412: /* sub_search_array: sub_search  */
#line 3178 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8032 "parser.cpp"
    break;

  case 413: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3182 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8041 "parser.cpp"
    break;

  case 414: /* function_expr: IDENTIFIER '(' ')'  */
#line 3187 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8054 "parser.cpp"
    break;

  case 415: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3195 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8067 "parser.cpp"
    break;

  case 416: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3203 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8081 "parser.cpp"
    break;

  case 417: /* function_expr: YEAR '(' expr ')'  */
#line 3212 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8093 "parser.cpp"
    break;

  case 418: /* function_expr: MONTH '(' expr ')'  */
#line 3219 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8105 "parser.cpp"
    break;

  case 419: /* function_expr: DAY '(' expr ')'  */
#line 3226 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8117 "parser.cpp"
    break;

  case 420: /* function_expr: HOUR '(' expr ')'  */
#line 3233 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8129 "parser.cpp"
    break;

  case 421: /* function_expr: MINUTE '(' expr ')'  */
#line 3240 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8141 "parser.cpp"
    break;

  case 422: /* function_expr: SECOND '(' expr ')'  */
#line 3247 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8153 "parser.cpp"
    break;

  case 423: /* function_expr: operand IS NOT NULLABLE  */
#line 3254 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8165 "parser.cpp"
    break;

  case 424: /* function_expr: operand IS NULLABLE  */
#line 3261 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8177 "parser.cpp"
    break;

  case 425: /* function_expr: NOT operand  */
#line 3268 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8189 "parser.cpp"
    break;

  case 426: /* function_expr: '-' operand  */
#line 3275 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8201 "parser.cpp"
    break;

  case 427: /* function_expr: '+' operand  */
#line 3282 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8213 "parser.cpp"
    break;

  case 428: /* function_expr: operand '-' operand  */
#line 3289 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8226 "parser.cpp"
    break;

  case 429: /* function_expr: operand '+' operand  */
#line 3297 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8239 "parser.cpp"
    break;

  case 430: /* function_expr: operand '*' operand  */
#line 3305 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8252 "parser.cpp"
    break;

  case 431: /* function_expr: operand '/' operand  */
#line 3313 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8265 "parser.cpp"
    break;

  case 432: /* function_expr: operand '%' operand  */
#line 3321 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8278 "parser.cpp"
    break;

  case 433: /* function_expr: operand '=' operand  */
#line 3329 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8291 "parser.cpp"
    break;

  case 434: /* function_expr: operand EQUAL operand  */
#line 3337 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8304 "parser.cpp"
    break;

  case 435: /* function_expr: operand NOT_EQ operand  */
#line 3345 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8317 "parser.cpp"
    break;

  case 436: /* function_expr: operand '<' operand  */
#line 3353 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8330 "parser.cpp"
    break;

  case 437: /* function_expr: operand '>' operand  */
#line 3361 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8343 "parser.cpp"
    break;

  case 438: /* function_expr: operand LESS_EQ operand  */
#line 3369 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8356 "parser.cpp"
    break;

  case 439: /* function_expr: operand GREATER_EQ operand  */
#line 3377 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8369 "parser.cpp"
    break;

  case 440: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3385 "parser.y"
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
#line 8404 "parser.cpp"
    break;

  case 441: /* function_expr: operand LIKE operand  */
#line 3415 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8417 "parser.cpp"
    break;

  case 442: /* function_expr: operand NOT LIKE operand  */
#line 3423 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8430 "parser.cpp"
    break;

  case 443: /* conjunction_expr: expr AND expr  */
#line 3432 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8443 "parser.cpp"
    break;

  case 444: /* conjunction_expr: expr OR expr  */
#line 3440 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8456 "parser.cpp"
    break;

  case 445: /* between_expr: operand BETWEEN operand AND operand  */
#line 3449 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8468 "parser.cpp"
    break;

  case 446: /* in_expr: operand IN '(' expr_array ')'  */
#line 3457 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8479 "parser.cpp"
    break;

  case 447: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3463 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8490 "parser.cpp"
    break;

  case 448: /* case_expr: CASE expr case_check_array END  */
#line 3470 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8501 "parser.cpp"
    break;

  case 449: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3476 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8513 "parser.cpp"
    break;

  case 450: /* case_expr: CASE case_check_array END  */
#line 3483 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8523 "parser.cpp"
    break;

  case 451: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3488 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8534 "parser.cpp"
    break;

  case 452: /* case_check_array: WHEN expr THEN expr  */
#line 3495 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8546 "parser.cpp"
    break;

  case 453: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3502 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8558 "parser.cpp"
    break;

  case 454: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3510 "parser.y"
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
#line 8575 "parser.cpp"
    break;

  case 455: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3523 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8586 "parser.cpp"
    break;

  case 456: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3529 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8597 "parser.cpp"
    break;

  case 457: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3535 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8609 "parser.cpp"
    break;

  case 458: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3542 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8621 "parser.cpp"
    break;

  case 459: /* column_expr: IDENTIFIER  */
#line 3550 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8633 "parser.cpp"
    break;

  case 460: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3557 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8645 "parser.cpp"
    break;

  case 461: /* column_expr: '*'  */
#line 3564 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8655 "parser.cpp"
    break;

  case 462: /* column_expr: column_expr '.' '*'  */
#line 3569 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8669 "parser.cpp"
    break;

  case 463: /* constant_expr: STRING  */
#line 3579 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8679 "parser.cpp"
    break;

  case 464: /* constant_expr: TRUE  */
#line 3584 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8689 "parser.cpp"
    break;

  case 465: /* constant_expr: FALSE  */
#line 3589 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8699 "parser.cpp"
    break;

  case 466: /* constant_expr: DOUBLE_VALUE  */
#line 3594 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8709 "parser.cpp"
    break;

  case 467: /* constant_expr: LONG_VALUE  */
#line 3599 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8719 "parser.cpp"
    break;

  case 468: /* constant_expr: DATE STRING  */
#line 3604 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8729 "parser.cpp"
    break;

  case 469: /* constant_expr: TIME STRING  */
#line 3609 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8739 "parser.cpp"
    break;

  case 470: /* constant_expr: DATETIME STRING  */
#line 3614 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8749 "parser.cpp"
    break;

  case 471: /* constant_expr: TIMESTAMP STRING  */
#line 3619 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8759 "parser.cpp"
    break;

  case 472: /* constant_expr: INTERVAL interval_expr  */
#line 3624 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8767 "parser.cpp"
    break;

  case 473: /* constant_expr: interval_expr  */
#line 3627 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8775 "parser.cpp"
    break;

  case 474: /* constant_expr: common_array_expr  */
#line 3630 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8783 "parser.cpp"
    break;

  case 475: /* constant_expr: curly_brackets_expr  */
#line 3633 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8791 "parser.cpp"
    break;

  case 476: /* common_array_expr: array_expr  */
#line 3637 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8799 "parser.cpp"
    break;

  case 477: /* common_array_expr: subarray_array_expr  */
#line 3640 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8807 "parser.cpp"
    break;

  case 478: /* common_array_expr: sparse_array_expr  */
#line 3643 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8815 "parser.cpp"
    break;

  case 479: /* common_array_expr: empty_array_expr  */
#line 3646 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8823 "parser.cpp"
    break;

  case 480: /* common_sparse_array_expr: sparse_array_expr  */
#line 3650 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8831 "parser.cpp"
    break;

  case 481: /* common_sparse_array_expr: array_expr  */
#line 3653 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8839 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: empty_array_expr  */
#line 3656 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8847 "parser.cpp"
    break;

  case 483: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3660 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8855 "parser.cpp"
    break;

  case 484: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3664 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8865 "parser.cpp"
    break;

  case 485: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3669 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8874 "parser.cpp"
    break;

  case 486: /* sparse_array_expr: long_sparse_array_expr  */
#line 3674 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8882 "parser.cpp"
    break;

  case 487: /* sparse_array_expr: double_sparse_array_expr  */
#line 3677 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8890 "parser.cpp"
    break;

  case 488: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3681 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8898 "parser.cpp"
    break;

  case 489: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3685 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8910 "parser.cpp"
    break;

  case 490: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3692 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8921 "parser.cpp"
    break;

  case 491: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3699 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8929 "parser.cpp"
    break;

  case 492: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3703 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8941 "parser.cpp"
    break;

  case 493: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3710 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8952 "parser.cpp"
    break;

  case 494: /* empty_array_expr: '[' ']'  */
#line 3717 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8960 "parser.cpp"
    break;

  case 495: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3721 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8968 "parser.cpp"
    break;

  case 496: /* curly_brackets_expr: '{' '}'  */
#line 3724 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 8976 "parser.cpp"
    break;

  case 497: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3728 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 8985 "parser.cpp"
    break;

  case 498: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3732 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8994 "parser.cpp"
    break;

  case 499: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3737 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9002 "parser.cpp"
    break;

  case 500: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3741 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9010 "parser.cpp"
    break;

  case 501: /* array_expr: long_array_expr  */
#line 3745 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9018 "parser.cpp"
    break;

  case 502: /* array_expr: double_array_expr  */
#line 3748 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9026 "parser.cpp"
    break;

  case 503: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3752 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9034 "parser.cpp"
    break;

  case 504: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3756 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9044 "parser.cpp"
    break;

  case 505: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3761 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9053 "parser.cpp"
    break;

  case 506: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3766 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9061 "parser.cpp"
    break;

  case 507: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3770 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9071 "parser.cpp"
    break;

  case 508: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3775 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9080 "parser.cpp"
    break;

  case 509: /* interval_expr: LONG_VALUE SECONDS  */
#line 3780 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9091 "parser.cpp"
    break;

  case 510: /* interval_expr: LONG_VALUE SECOND  */
#line 3786 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9102 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE MINUTES  */
#line 3792 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9113 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE MINUTE  */
#line 3798 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9124 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE HOURS  */
#line 3804 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9135 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE HOUR  */
#line 3810 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9146 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE DAYS  */
#line 3816 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9157 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE DAY  */
#line 3822 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9168 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE MONTHS  */
#line 3828 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9179 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE MONTH  */
#line 3834 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9190 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE YEARS  */
#line 3840 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9201 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE YEAR  */
#line 3846 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9212 "parser.cpp"
    break;

  case 521: /* copy_option_list: copy_option  */
#line 3857 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9221 "parser.cpp"
    break;

  case 522: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3861 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9230 "parser.cpp"
    break;

  case 523: /* copy_option: FORMAT IDENTIFIER  */
#line 3866 "parser.y"
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
#line 9266 "parser.cpp"
    break;

  case 524: /* copy_option: DELIMITER STRING  */
#line 3897 "parser.y"
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
#line 9281 "parser.cpp"
    break;

  case 525: /* copy_option: HEADER  */
#line 3907 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9291 "parser.cpp"
    break;

  case 526: /* copy_option: OFFSET LONG_VALUE  */
#line 3912 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9301 "parser.cpp"
    break;

  case 527: /* copy_option: LIMIT LONG_VALUE  */
#line 3917 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9311 "parser.cpp"
    break;

  case 528: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3922 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9321 "parser.cpp"
    break;

  case 529: /* file_path: STRING  */
#line 3928 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9329 "parser.cpp"
    break;

  case 530: /* if_exists: IF EXISTS  */
#line 3932 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9335 "parser.cpp"
    break;

  case 531: /* if_exists: %empty  */
#line 3933 "parser.y"
  { (yyval.bool_value) = false; }
#line 9341 "parser.cpp"
    break;

  case 532: /* if_not_exists: IF NOT EXISTS  */
#line 3935 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9347 "parser.cpp"
    break;

  case 533: /* if_not_exists: %empty  */
#line 3936 "parser.y"
  { (yyval.bool_value) = false; }
#line 9353 "parser.cpp"
    break;

  case 536: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3951 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9366 "parser.cpp"
    break;

  case 537: /* if_not_exists_info: %empty  */
#line 3959 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9374 "parser.cpp"
    break;

  case 538: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3963 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9382 "parser.cpp"
    break;

  case 539: /* with_index_param_list: %empty  */
#line 3966 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9390 "parser.cpp"
    break;

  case 540: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3970 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9398 "parser.cpp"
    break;

  case 541: /* optional_table_properties_list: %empty  */
#line 3973 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9406 "parser.cpp"
    break;

  case 542: /* index_param_list: index_param  */
#line 3977 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9415 "parser.cpp"
    break;

  case 543: /* index_param_list: index_param_list ',' index_param  */
#line 3981 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9424 "parser.cpp"
    break;

  case 544: /* index_param: IDENTIFIER  */
#line 3986 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9435 "parser.cpp"
    break;

  case 545: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3992 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9450 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER '=' STRING  */
#line 4002 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9465 "parser.cpp"
    break;

  case 547: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4012 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9478 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4020 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9491 "parser.cpp"
    break;

  case 549: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4031 "parser.y"
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
#line 9527 "parser.cpp"
    break;

  case 550: /* index_info: '(' IDENTIFIER ')'  */
#line 4062 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9538 "parser.cpp"
    break;


#line 9542 "parser.cpp"

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

#line 4069 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

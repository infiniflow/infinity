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
  YYSYMBOL_PERSISTENCE = 198,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 199,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 200,                  /* OBJECTS  */
  YYSYMBOL_FILES = 201,                    /* FILES  */
  YYSYMBOL_MEMORY = 202,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 203,               /* ALLOCATION  */
  YYSYMBOL_HISTORY = 204,                  /* HISTORY  */
  YYSYMBOL_CHECK = 205,                    /* CHECK  */
  YYSYMBOL_CLEAN = 206,                    /* CLEAN  */
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
  YYSYMBOL_check_statement = 290,          /* check_statement  */
  YYSYMBOL_expr_array = 291,               /* expr_array  */
  YYSYMBOL_insert_row_list = 292,          /* insert_row_list  */
  YYSYMBOL_expr_alias = 293,               /* expr_alias  */
  YYSYMBOL_expr = 294,                     /* expr  */
  YYSYMBOL_operand = 295,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 296,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 297,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 298,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 299,          /* match_text_expr  */
  YYSYMBOL_query_expr = 300,               /* query_expr  */
  YYSYMBOL_fusion_expr = 301,              /* fusion_expr  */
  YYSYMBOL_sub_search = 302,               /* sub_search  */
  YYSYMBOL_sub_search_array = 303,         /* sub_search_array  */
  YYSYMBOL_function_expr = 304,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 305,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 306,             /* between_expr  */
  YYSYMBOL_in_expr = 307,                  /* in_expr  */
  YYSYMBOL_case_expr = 308,                /* case_expr  */
  YYSYMBOL_case_check_array = 309,         /* case_check_array  */
  YYSYMBOL_cast_expr = 310,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 311,            /* subquery_expr  */
  YYSYMBOL_column_expr = 312,              /* column_expr  */
  YYSYMBOL_constant_expr = 313,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 314,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 315, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 316,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 317, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 318,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 319,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 320, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 321, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 322, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 323,         /* empty_array_expr  */
  YYSYMBOL_curly_brackets_expr = 324,      /* curly_brackets_expr  */
  YYSYMBOL_unclosed_curly_brackets_expr = 325, /* unclosed_curly_brackets_expr  */
  YYSYMBOL_int_sparse_ele = 326,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 327,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 328,               /* array_expr  */
  YYSYMBOL_long_array_expr = 329,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 330, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 331,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 332, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 333,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 334,         /* copy_option_list  */
  YYSYMBOL_copy_option = 335,              /* copy_option  */
  YYSYMBOL_file_path = 336,                /* file_path  */
  YYSYMBOL_if_exists = 337,                /* if_exists  */
  YYSYMBOL_if_not_exists = 338,            /* if_not_exists  */
  YYSYMBOL_semicolon = 339,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 340,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 341,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 342, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 343,         /* index_param_list  */
  YYSYMBOL_index_param = 344,              /* index_param  */
  YYSYMBOL_index_info = 345                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 98 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 474 "parser.cpp"

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
#define YYFINAL  130
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1560

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  226
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  120
/* YYNRULES -- Number of rules.  */
#define YYNRULES  551
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1256

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
       0,   520,   520,   524,   530,   537,   538,   539,   540,   541,
     542,   543,   544,   545,   546,   547,   548,   549,   550,   551,
     552,   554,   555,   556,   557,   558,   559,   560,   561,   562,
     563,   564,   565,   572,   589,   606,   622,   651,   666,   698,
     716,   734,   762,   793,   797,   802,   806,   812,   815,   822,
     842,   864,   888,   914,   918,   924,   925,   926,   927,   928,
     929,   930,   931,   932,   933,   934,   935,   936,   937,   938,
     939,   940,   941,   942,   943,   944,   947,   949,   950,   951,
     952,   955,   956,   957,   958,   959,   960,   961,   962,   963,
     964,   965,   966,   967,   968,   969,   970,   971,   972,   973,
     974,   975,   976,   977,   978,   979,   980,   981,   982,   983,
     984,   985,   986,   987,   988,   989,   990,   991,   992,   993,
     994,   995,   996,   997,   998,   999,  1000,  1001,  1002,  1003,
    1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,  1012,  1013,
    1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1025,  1048,
    1052,  1062,  1065,  1068,  1071,  1075,  1078,  1083,  1088,  1095,
    1101,  1111,  1127,  1165,  1181,  1184,  1191,  1208,  1217,  1230,
    1234,  1239,  1252,  1265,  1280,  1295,  1310,  1333,  1386,  1441,
    1492,  1495,  1498,  1507,  1517,  1520,  1524,  1529,  1556,  1559,
    1564,  1581,  1584,  1588,  1592,  1597,  1603,  1606,  1609,  1613,
    1617,  1619,  1623,  1625,  1628,  1632,  1635,  1639,  1642,  1646,
    1651,  1655,  1658,  1662,  1665,  1669,  1672,  1676,  1679,  1683,
    1686,  1689,  1692,  1700,  1703,  1718,  1718,  1720,  1734,  1743,
    1748,  1757,  1762,  1767,  1773,  1780,  1783,  1787,  1790,  1795,
    1807,  1814,  1828,  1831,  1834,  1837,  1840,  1843,  1846,  1852,
    1856,  1860,  1864,  1868,  1875,  1879,  1883,  1887,  1891,  1896,
    1900,  1905,  1909,  1913,  1917,  1923,  1929,  1935,  1946,  1957,
    1968,  1980,  1992,  2005,  2019,  2030,  2044,  2060,  2077,  2081,
    2085,  2091,  2095,  2099,  2105,  2109,  2113,  2117,  2123,  2127,
    2137,  2141,  2145,  2153,  2164,  2187,  2193,  2198,  2204,  2210,
    2218,  2224,  2230,  2236,  2242,  2250,  2256,  2262,  2268,  2274,
    2282,  2288,  2294,  2302,  2310,  2316,  2322,  2328,  2334,  2338,
    2352,  2365,  2369,  2374,  2380,  2387,  2395,  2404,  2414,  2424,
    2435,  2446,  2458,  2470,  2480,  2491,  2503,  2516,  2520,  2525,
    2530,  2536,  2540,  2544,  2550,  2554,  2558,  2564,  2570,  2578,
    2584,  2588,  2594,  2598,  2604,  2609,  2614,  2621,  2630,  2640,
    2649,  2661,  2673,  2677,  2693,  2697,  2702,  2712,  2734,  2740,
    2744,  2745,  2746,  2747,  2748,  2750,  2753,  2759,  2762,  2763,
    2764,  2765,  2766,  2767,  2768,  2769,  2770,  2771,  2775,  2791,
    2808,  2826,  2872,  2911,  2954,  3001,  3025,  3048,  3069,  3090,
    3099,  3110,  3121,  3135,  3142,  3152,  3158,  3170,  3173,  3176,
    3179,  3182,  3185,  3189,  3193,  3198,  3206,  3214,  3223,  3230,
    3237,  3244,  3251,  3258,  3265,  3272,  3279,  3286,  3293,  3300,
    3308,  3316,  3324,  3332,  3340,  3348,  3356,  3364,  3372,  3380,
    3388,  3396,  3426,  3434,  3443,  3451,  3460,  3468,  3474,  3481,
    3487,  3494,  3499,  3506,  3513,  3521,  3534,  3540,  3546,  3553,
    3561,  3568,  3575,  3580,  3590,  3595,  3600,  3605,  3610,  3615,
    3620,  3625,  3630,  3635,  3638,  3641,  3644,  3648,  3651,  3654,
    3657,  3661,  3664,  3667,  3671,  3675,  3680,  3685,  3688,  3692,
    3696,  3703,  3710,  3714,  3721,  3728,  3732,  3735,  3739,  3743,
    3748,  3752,  3756,  3759,  3763,  3767,  3772,  3777,  3781,  3786,
    3791,  3797,  3803,  3809,  3815,  3821,  3827,  3833,  3839,  3845,
    3851,  3857,  3868,  3872,  3877,  3908,  3918,  3923,  3928,  3933,
    3939,  3943,  3944,  3946,  3947,  3949,  3950,  3962,  3970,  3974,
    3977,  3981,  3984,  3988,  3992,  3997,  4003,  4013,  4023,  4031,
    4042,  4073
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
  "RECOVER", "RESTORE", "PERSISTENCE", "OBJECT", "OBJECTS", "FILES",
  "MEMORY", "ALLOCATION", "HISTORY", "CHECK", "CLEAN", "NUMBER", "'='",
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

#define YYPACT_NINF (-758)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-539)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     945,    79,    27,   101,    96,    16,    96,   328,   890,  1007,
      77,   104,   146,   388,   160,   243,   255,   111,    88,    51,
     157,    55,   -38,   275,    92,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,   330,  -758,  -758,   301,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,   277,   277,   277,   277,   -13,
     321,    96,   280,   280,   280,   280,   280,   369,   204,   416,
      96,    -9,   440,   456,   464,   969,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,   330,  -758,  -758,  -758,  -758,  -758,   235,
     487,    96,  -758,  -758,  -758,  -758,  -758,     8,  -758,   382,
     420,  -758,   493,  -758,  -758,   437,   512,  -758,   522,  -758,
     438,   217,    96,    96,   538,  -758,  -758,  -758,  -758,   -26,
    -758,   466,   313,  -758,   560,   349,   380,   319,   472,   384,
     579,   393,   514,   421,   430,    96,  -758,  -758,   368,   394,
    -758,    64,  -758,   596,  -758,  -758,    18,   562,  -758,   577,
     582,   630,    96,    96,    96,   656,   614,   616,   447,   606,
     680,    96,    96,    96,   682,  -758,   701,   707,   647,   710,
     710,   625,    83,   103,   125,  -758,   497,  -758,   450,  -758,
    -758,  -758,   741,  -758,   752,  -758,   710,  -758,  -758,   749,
    -758,  -758,  -758,  -758,   287,  -758,   696,    96,   546,   255,
     710,  -758,   762,  -758,   603,  -758,   764,  -758,  -758,   771,
    -758,  -758,   790,  -758,   794,   802,  -758,   803,   758,   810,
     624,   816,   817,  -758,  -758,  -758,  -758,  -758,    18,  -758,
    -758,  -758,   625,   770,   756,   751,   692,   -37,  -758,   447,
    -758,    96,   419,   823,   269,  -758,  -758,  -758,  -758,  -758,
     766,  -758,   620,   -18,  -758,   625,  -758,  -758,   755,   757,
     615,  -758,  -758,   885,   715,   621,   622,   444,   838,   840,
     841,   842,  -758,  -758,   848,   629,   638,   642,   644,   650,
     652,   658,   407,   660,   661,   729,   729,  -758,    25,   534,
      14,   292,  -758,    28,   862,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,   628,  -758,
    -758,  -758,   211,  -758,  -758,   238,  -758,   240,  -758,  -758,
     116,  -758,  -758,   253,  -758,   290,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,   871,   874,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,   835,   836,   807,    96,   808,   301,  -758,  -758,
    -758,   880,   262,  -758,   881,  -758,  -758,   812,   536,  -758,
     886,  -758,  -758,   670,   672,   -40,   625,   625,   824,  -758,
     891,   -38,    50,   851,   689,   906,   908,  -758,  -758,   348,
     697,  -758,    96,   625,   707,  -758,   587,   699,   703,   261,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,   729,   705,   947,   837,   625,   625,    10,   304,
    -758,  -758,  -758,  -758,   885,  -758,   625,   625,   625,   625,
     625,   625,   918,   706,   711,   712,   714,   931,   933,   515,
     515,  -758,   713,  -758,  -758,  -758,  -758,   724,   152,  -758,
    -758,   868,   625,   943,   625,   625,   -22,   730,    26,   729,
     729,   729,   729,   729,   729,   729,   729,   729,   729,   729,
     729,   729,   729,    22,  -758,   733,  -758,   944,  -758,   957,
      23,  -758,  -758,   958,  -758,   946,   923,   704,   748,   754,
     966,  -758,   759,  -758,   767,  -758,   964,  -758,   303,   983,
     825,   826,  -758,  -758,  -758,   625,   913,   772,  -758,   132,
     587,   625,  -758,  -758,    34,  1122,   861,   778,   386,  -758,
    -758,  -758,   -38,   997,   869,  -758,  -758,  -758,   998,   625,
     785,  -758,   587,  -758,    75,    75,   625,  -758,   390,   837,
     859,   799,   124,   164,   436,  -758,   625,   625,   174,   181,
     200,   213,   226,   239,   934,   625,    30,   625,  1017,   813,
     422,   779,  -758,  -758,   710,  -758,  -758,  -758,   882,   818,
     729,   534,   911,  -758,   165,   165,   201,   201,   853,   165,
     165,   201,   201,   515,   515,  -758,  -758,  -758,  -758,  -758,
    -758,   819,  -758,   820,  -758,  -758,  -758,  -758,  1032,  1037,
    -758,   823,  1046,  -758,  1047,  -758,  -758,  1045,  -758,  -758,
    1048,  1049,   832,    13,   877,   625,  -758,  -758,  -758,   587,
    1052,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,   839,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,   863,   864,   865,   866,   867,
     870,   872,   873,   357,   875,   823,  1027,    50,   330,   850,
    1056,  -758,   439,   876,  1073,  1074,  1082,  1086,  -758,  1089,
     462,  -758,   473,   477,  -758,   883,  -758,  1122,   625,  -758,
     625,    36,   203,  -758,  -758,  -758,  -758,  -758,  -758,   729,
     148,  -758,   -12,   166,   879,    42,   884,  -758,  1088,  -758,
    -758,  1029,   534,   165,   888,   484,  -758,   729,  1098,  1110,
    1069,  1075,   489,   517,  -758,   912,   518,  -758,  1113,  -758,
    -758,   -38,   900,   540,  -758,    98,  -758,   353,   647,  -758,
    -758,  1120,  1122,  1122,   597,  1020,  1158,  1175,  1198,  1215,
     999,  1000,  -758,  -758,   336,  -758,   995,   823,   519,   914,
    1001,  -758,   968,  -758,  -758,   625,  -758,  -758,  -758,  -758,
    -758,  -758,    75,  -758,  -758,  -758,   916,   587,   130,  -758,
     625,   350,   915,   733,   920,  1133,   917,   625,  -758,   921,
     925,   922,   532,  -758,  -758,   947,  1138,  1140,  -758,  -758,
    1046,   548,  -758,  1047,   402,    35,    13,  1091,  -758,  -758,
    -758,  -758,  -758,  -758,  1092,  -758,  1146,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,   930,  1102,   537,   935,   553,
    -758,   942,   950,   954,   959,   961,   963,   970,   971,   972,
    1076,   973,   974,   975,   976,   977,   978,   981,   982,   984,
     985,  1095,   986,  1005,  1012,  1013,  1015,  1016,  1030,  1031,
    1034,  1035,  1099,  1036,  1038,  1051,  1053,  1054,  1055,  1059,
    1060,  1062,  1063,  1100,  1064,  1070,  1071,  1077,  1087,  1093,
    1094,  1103,  1104,  1105,  1168,  1106,  1107,  1108,  1109,  1111,
    1112,  1114,  1115,  1116,  1117,  1178,  1119,  -758,  -758,    23,
    -758,  1137,  1141,   557,  -758,  1047,  1161,  1177,   559,  -758,
    -758,  -758,   587,  -758,   786,  1121,  1123,    17,  1124,  -758,
    -758,  -758,  1125,  1231,  1078,   587,  -758,    75,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  1292,  -758,
      98,   540,    13,    13,  1096,   353,  1258,  1260,  -758,  1311,
    -758,  -758,  1122,  1315,  1316,  1318,  1326,  1329,  1336,  1338,
    1342,  1343,  1128,  1345,  1346,  1347,  1348,  1349,  1350,  1351,
    1352,  1353,  1354,  1139,  1356,  1357,  1358,  1359,  1360,  1361,
    1362,  1363,  1364,  1365,  1150,  1367,  1368,  1369,  1370,  1371,
    1372,  1373,  1374,  1375,  1376,  1162,  1377,  1379,  1380,  1381,
    1382,  1383,  1384,  1385,  1386,  1387,  1172,  1389,  1390,  1391,
    1392,  1393,  1394,  1395,  1396,  1397,  1398,  1183,  1400,  -758,
    1403,  1404,  -758,   561,  -758,   808,  -758,  -758,  1405,  1406,
    1407,    81,  1193,  -758,   567,  1408,  -758,  -758,  1355,   823,
    -758,   625,   625,  -758,  1194,  -758,  1196,  1199,  1200,  1201,
    1202,  1203,  1204,  1205,  1206,  1411,  1207,  1208,  1209,  1210,
    1211,  1212,  1213,  1214,  1216,  1217,  1428,  1218,  1219,  1220,
    1221,  1222,  1223,  1224,  1225,  1226,  1227,  1441,  1229,  1230,
    1232,  1233,  1234,  1235,  1236,  1237,  1238,  1239,  1444,  1240,
    1241,  1242,  1243,  1244,  1245,  1246,  1247,  1248,  1249,  1463,
    1251,  1252,  1253,  1254,  1255,  1256,  1257,  1259,  1261,  1262,
    1471,  1263,  -758,  -758,  -758,  -758,  1264,  1265,  1266,   917,
    1314,  -758,   529,   625,   571,   832,   587,  -758,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  1270,  -758,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  1271,  -758,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  1272,
    -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    1273,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  1274,  -758,  -758,  -758,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  1275,  -758,  1479,  1480,   249,  1276,  1277,  1437,
    1491,  1492,  -758,  -758,  -758,   587,  -758,  -758,  -758,  -758,
    -758,  -758,  -758,  1278,  1279,   917,   808,  1334,  1284,  1497,
     750,   266,  1285,  1501,  1287,  -758,  1459,  1504,   760,  1503,
    -758,   917,   808,   917,   129,  1293,  1291,  1506,  -758,  1466,
    1295,  -758,  1296,  1477,  1478,  -758,  1514,  -758,  -758,  -758,
     134,   -62,  -758,  1301,  1302,  1481,  1483,  -758,  1484,  1485,
    1522,  -758,  -758,  1308,  -758,  1309,  1310,  1525,  1527,   808,
    1312,  1313,  -758,   808,  -758,  -758
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
       0,   284,     0,     0,     0,   290,   291,   292,   295,   229,
     293,     0,   235,   237,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   362,   318,     0,     0,
       1,   236,     2,   219,   221,   222,     0,   204,   186,   192,
       0,     0,     0,     0,     0,     0,     0,     0,   165,     0,
       0,     0,     0,     0,     0,   315,     0,     0,   214,     0,
       0,     0,     0,     0,     0,   166,     0,   266,   267,   260,
     261,   262,     0,   263,     0,   253,     0,   258,   289,     0,
     282,   281,   285,   286,     0,   320,     0,     0,     0,     0,
       0,   344,     0,   354,     0,   355,     0,   341,   342,     0,
     337,   321,     0,   350,   352,     0,   345,     0,     0,     0,
       0,     0,     0,   363,   185,   184,     4,   220,     0,   182,
     183,   203,     0,     0,   200,     0,    34,     0,    35,   165,
     537,     0,     0,     0,   236,   531,   172,   174,   173,   175,
       0,   230,     0,   214,   169,     0,   161,   530,     0,     0,
     460,   464,   467,   468,     0,     0,     0,     0,     0,     0,
       0,     0,   465,   466,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   462,     0,   236,
       0,     0,   364,   369,   370,   384,   382,   385,   383,   386,
     387,   379,   374,   373,   372,   380,   381,   371,   378,   377,
     475,   478,     0,   479,   487,     0,   488,     0,   480,   476,
       0,   477,   502,     0,   503,     0,   474,   299,   301,   300,
     297,   298,   304,   306,   305,   302,   303,   309,   311,   310,
     307,   308,   287,     0,     0,   269,   268,   274,   264,   265,
     280,   283,     0,     0,     0,     0,   540,     0,   238,   296,
     347,     0,   338,   343,   322,   351,   346,     0,     0,   353,
       0,   316,   317,     0,     0,   206,     0,     0,   202,   533,
       0,   236,     0,     0,     0,     0,     0,   314,   159,     0,
       0,   163,     0,     0,     0,   168,   213,     0,     0,     0,
     511,   510,   513,   512,   515,   514,   517,   516,   519,   518,
     521,   520,     0,     0,   426,   236,     0,     0,     0,     0,
     469,   470,   471,   472,     0,   473,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   428,
     427,   508,   505,   495,   485,   490,   493,     0,     0,   497,
     498,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   484,     0,   489,     0,   492,     0,
       0,   496,   504,     0,   507,     0,   275,   270,     0,     0,
       0,   319,     0,   294,     0,   356,     0,   339,     0,     0,
       0,     0,   349,   189,   188,     0,   208,   191,   193,   198,
     199,     0,   187,    33,    37,     0,     0,     0,     0,    43,
      47,    48,   236,     0,    41,   313,   312,   164,     0,     0,
     162,   176,   171,   170,     0,     0,     0,   415,     0,   236,
       0,     0,     0,     0,     0,   451,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   212,
       0,     0,   376,   375,     0,   365,   368,   444,   445,     0,
       0,   236,     0,   425,   435,   436,   439,   440,     0,   442,
     434,   437,   438,   430,   429,   431,   432,   433,   461,   463,
     486,     0,   491,     0,   494,   499,   506,   509,     0,     0,
     271,     0,     0,   359,     0,   239,   340,     0,   323,   348,
       0,     0,   205,     0,   210,     0,   196,   197,   195,   201,
       0,    55,    58,    59,    56,    57,    60,    61,    77,    62,
      64,    63,    80,    67,    68,    69,    65,    66,    70,    71,
      72,    73,    74,    75,    76,     0,     0,     0,     0,     0,
       0,     0,     0,   540,     0,     0,   542,     0,    40,     0,
       0,   160,     0,     0,     0,     0,     0,     0,   526,     0,
       0,   522,     0,     0,   416,     0,   456,     0,     0,   449,
       0,     0,     0,   423,   422,   421,   420,   419,   418,     0,
       0,   460,     0,     0,     0,     0,     0,   405,     0,   501,
     500,     0,   236,   443,     0,     0,   424,     0,     0,     0,
     276,   272,     0,     0,    45,   545,     0,   543,   324,   357,
     358,   236,   207,   223,   225,   234,   226,     0,   214,   194,
      39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   152,   153,   156,   149,   156,     0,     0,     0,
      36,    44,   551,    42,   366,     0,   528,   527,   525,   524,
     529,   179,     0,   177,   417,   457,     0,   453,     0,   452,
       0,     0,     0,     0,     0,     0,   212,     0,   403,     0,
       0,     0,     0,   458,   447,   446,     0,     0,   361,   360,
       0,     0,   539,     0,     0,     0,     0,     0,   243,   244,
     245,   246,   242,   247,     0,   232,     0,   227,   409,   407,
     410,   408,   411,   412,   413,   209,   218,     0,     0,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   154,   151,     0,
     150,    50,    49,     0,   158,     0,     0,     0,     0,   523,
     455,   450,   454,   441,     0,     0,     0,     0,     0,   481,
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
       0,     0,   157,     0,    38,   540,   367,   505,     0,     0,
       0,     0,     0,   404,     0,   326,   228,   240,     0,     0,
     414,     0,     0,   190,     0,    54,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    51,   541,   550,     0,     0,   212,   212,
     399,   178,     0,     0,     0,   217,   215,    78,    84,    85,
      82,    83,    86,    87,    88,    89,    90,     0,    81,   128,
     129,   126,   127,   130,   131,   132,   133,   134,     0,   125,
      95,    96,    93,    94,    97,    98,    99,   100,   101,     0,
      92,   106,   107,   104,   105,   108,   109,   110,   111,   112,
       0,   103,   139,   140,   137,   138,   141,   142,   143,   144,
     145,     0,   136,   117,   118,   115,   116,   119,   120,   121,
     122,   123,     0,   114,     0,     0,     0,     0,     0,     0,
       0,     0,   328,   327,   333,   241,   233,    91,   135,   102,
     113,   146,   124,   212,     0,   212,   540,   400,     0,   334,
     329,     0,     0,     0,     0,   398,     0,     0,     0,     0,
     330,   212,   540,   212,   540,     0,     0,     0,   335,   331,
       0,   394,     0,     0,     0,   397,     0,   401,   336,   332,
     540,   388,   396,     0,     0,     0,     0,   393,     0,     0,
       0,   402,   392,     0,   390,     0,     0,     0,     0,   540,
       0,     0,   395,   540,   389,   391
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -758,  -758,  -758,  1402,  1469,    62,  -758,  -758,   889,  -577,
    -758,  -654,  -758,   801,   804,  -758,  -580,   197,   282,  1317,
    -758,   318,  -758,  1153,   320,   341,    -6,  1516,   -19,  1192,
    1323,   -86,  -758,  -758,   937,  -758,  -758,  -758,  -758,  -758,
    -758,  -758,  -757,  -236,  -758,  -758,  -758,  -758,   761,  -113,
      45,   623,  -758,  -758,  1366,  -758,  -758,   356,   376,   396,
     418,   423,  -758,  -758,  -758,  -221,  -758,  1118,  -245,  -233,
    -685,  -681,  -680,  -677,  -673,  -672,   619,  -758,  -758,  -758,
    -758,  -758,  -758,  1142,  -758,  -758,  1002,  -275,  -272,  -758,
    -758,  -758,   781,  -758,  -758,  -758,  -758,   787,  -758,  -758,
    1085,  1080,  -286,  -758,  -758,  -758,  -758,  1289,  -515,   805,
    -152,   566,   604,  -758,  -758,  -639,  -758,   669,   773,  -758
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    23,    24,    25,    66,    26,   508,   703,   509,   510,
     809,   643,   734,   735,   881,   511,   379,    27,    28,   234,
      29,    30,   243,   244,    31,    32,    33,    34,    35,   138,
     219,   139,   224,   497,   498,   608,   368,   502,   222,   496,
     604,   718,   686,   246,  1023,   927,   136,   712,   713,   714,
     715,   797,    36,   112,   113,   716,   794,    37,    38,    39,
      40,    41,    42,    43,    44,   281,   520,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   804,   805,   291,   292,
     293,   294,   295,   409,   296,   297,   298,   299,   300,   898,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     435,   436,   311,   312,   313,   314,   315,   316,   660,   661,
     248,   150,   141,   132,   146,   483,   740,   706,   707,   514
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     386,   365,    73,   129,   736,   440,   434,   385,   249,   904,
     662,   702,   408,   756,   169,   704,    58,   371,   251,   252,
     253,   404,   431,   432,   340,   578,   137,   251,   252,   253,
     431,   432,   798,   681,   438,   187,   799,   800,   349,    16,
     801,   495,   429,   430,   802,   803,   766,   245,  -538,    59,
     220,    61,   559,   505,   133,   133,   134,   134,   140,    73,
     110,   135,   135,    51,  -535,   738,   159,   160,   808,   810,
      67,     1,  1238,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   443,   123,   124,  1099,   317,    11,   318,   319,
      12,    60,    13,    14,    15,   115,   148,   407,   562,    58,
     116,   795,   117,  1239,   118,   158,   322,   767,   323,   324,
     102,    45,    46,    47,   258,   259,   260,    48,    49,   759,
     261,   499,   500,   258,   259,   260,   168,    67,   327,   261,
     328,   329,   364,    52,    53,    54,   560,   103,   522,    55,
      56,    16,   654,   655,   320,  1012,   767,   184,   185,   262,
     263,   264,   796,   656,   657,   658,   563,   883,   262,   263,
     264,   532,   533,   108,   325,   444,   445,   610,   528,   404,
     213,   538,   539,   540,   541,   542,   543,   506,   667,   507,
      22,   372,   442,   444,   445,   104,   330,   227,   228,   229,
     125,   444,   445,   580,   156,   585,   237,   238,   239,   557,
     558,   606,   607,   912,   384,    68,   482,   126,   463,   161,
     763,   482,   170,   891,   127,   381,   564,   565,   566,   567,
     568,   569,   570,   571,   572,   573,   574,   575,   576,   577,
     278,   711,   346,    17,   433,   579,   218,   439,   280,   278,
     798,   278,   433,   277,   799,   800,   109,   280,   801,    18,
     668,   321,   802,   803,   920,  1195,   609,   119,   111,   659,
     437,    19,    68,  1223,   250,   251,   252,   253,  1235,    20,
      21,   326,  1211,    50,   602,   130,   374,   120,  1025,   444,
     445,   121,    22,   114,   122,   444,   445,   444,   445,   760,
      69,   671,   672,   331,  1224,    57,   448,   342,   652,  1236,
     680,   486,   683,   444,   445,   663,   380,   444,   445,   137,
     487,   343,   344,   131,   767,  -539,  -539,   451,   452,   444,
     445,   444,   445,  -539,   147,   526,    70,   693,    71,   444,
     445,   767,   448,   254,   255,   597,   444,   445,   470,   471,
     695,  1177,  1178,   256,   598,   257,    16,    69,   140,    72,
     133,   149,   134,  -539,  -539,   444,   445,   135,   444,   445,
     499,   258,   259,   260,    74,   504,  1095,   261,   444,   445,
     762,   553,   155,  -539,   456,   457,   458,   459,   460,   461,
     462,   444,   445,    70,    75,    71,   531,   535,   764,   536,
     481,   537,  1014,   673,   444,   445,   262,   263,   264,   265,
     674,   266,   691,   267,    76,   268,    72,   269,   730,   270,
    -539,  -539,   458,   459,   460,   461,   462,   182,   271,   675,
     183,    74,   530,   757,   156,   758,    77,   521,   464,   730,
     157,    78,   676,   465,   482,   918,  1202,   919,  1204,  1104,
     272,    75,   273,   162,   274,   677,   761,   250,   251,   252,
     253,   375,   376,   166,  1220,   466,  1222,   468,   678,   163,
     467,    76,   469,   731,   775,   732,   733,   164,   879,   377,
     472,   772,   275,   276,   277,   473,   895,   278,   901,   279,
     527,   448,   806,    77,   731,   280,   732,   733,    78,   333,
     167,   896,   334,   335,    62,    63,   175,   336,   337,    64,
     449,   450,   451,   452,   193,   194,   648,   474,   454,   195,
     665,   441,   475,   176,   442,   892,   254,   255,   177,   669,
     188,   670,   905,   537,   888,   178,   256,   423,   257,   424,
     425,   407,   272,   426,   273,   189,   274,   250,   251,   252,
     253,   186,   694,   191,   258,   259,   260,   105,   106,   107,
     261,   913,   914,   915,   916,   171,   172,  1205,   455,   456,
     457,   458,   459,   460,   461,   462,   190,   517,  1180,   893,
     518,  1181,  1182,  1221,   192,  1225,  1183,  1184,   207,   262,
     263,   264,   265,   208,   266,   209,   267,   214,   268,   210,
     269,  1237,   270,   173,   174,   787,  -248,   788,   789,   790,
     791,   271,   792,   793,   999,   646,   254,   255,   647,   664,
    1252,    16,   442,   215,  1255,   211,   256,   217,   257,   151,
     152,   153,   154,   272,   212,   273,   221,   274,   250,   251,
     252,   253,   196,   226,   258,   259,   260,   179,   180,   181,
     261,   687,   197,   223,   688,   198,   199,   200,   201,   202,
     142,   143,   144,   145,   225,   275,   276,   277,   744,   230,
     278,   442,   279,   203,   204,   233,   205,   206,   280,   262,
     263,   264,   265,   771,   266,   231,   267,   232,   268,   235,
     269,   751,   270,   236,   752,   240,   811,   812,   813,   814,
     815,   271,   753,   816,   817,   752,   754,   254,   255,   442,
     818,   819,   820,   774,   241,   785,   442,   256,   778,   257,
     242,   518,   245,   272,   247,   273,   332,   274,   250,   251,
     252,   253,   821,   490,   491,   258,   259,   260,   460,   461,
     462,   261,   250,   251,   252,   253,   779,   782,   884,   780,
     783,   518,   444,   445,   338,   275,   276,   277,   589,   590,
     278,   909,   279,   341,   442,   339,   928,   345,   280,   929,
     262,   263,   264,   265,   347,   266,   350,   267,   351,   268,
     352,   269,   931,   270,   353,   932,  1002,  1106,  1006,   518,
    1094,   442,   271,   783,   689,   690,  1101,   402,   403,   752,
    1186,   431,  1007,   518,  1209,  1210,   354,   256,   355,   257,
    1105,   402,  1217,  1218,   272,   356,   273,   357,   274,  1017,
    1018,   256,   358,   257,   359,   258,   259,   260,   360,   361,
     362,   261,   366,   367,   369,   370,   378,   382,   383,   258,
     259,   260,   387,   389,   388,   261,   275,   276,   277,   405,
     406,   278,   410,   279,   411,   412,   413,   416,   463,   280,
     262,   263,   264,   265,   414,   266,   417,   267,  1185,   268,
     418,   269,   419,   270,   262,   263,   264,   265,   420,   266,
     421,   267,   271,   268,   476,   269,   422,   270,   427,   428,
     477,   478,   479,   480,   485,   482,   271,   488,   489,   493,
     492,   494,   501,    65,   272,   503,   273,     1,   274,     2,
       3,     4,     5,     6,     7,   512,     9,   513,   272,   515,
     273,   516,   274,    11,    16,   519,    12,   524,    13,    14,
      15,   525,   544,   529,   545,   530,   275,   276,   277,   546,
     547,   278,   548,   279,   446,   549,   447,   550,   551,   280,
     275,   276,   277,   552,   554,   278,   556,   279,   561,   278,
     581,   587,     1,   280,     2,     3,     4,     5,     6,     7,
       8,     9,    10,   583,   586,   588,   591,    16,    11,   593,
     596,    12,   592,    13,    14,    15,     1,   594,     2,     3,
       4,     5,     6,     7,   448,     9,   595,   599,   603,   644,
     600,   601,    11,   448,   605,    12,   645,    13,    14,    15,
     649,   651,   650,   449,   450,   451,   452,   653,   697,   679,
      79,   454,   449,   450,   451,   452,   453,   560,   666,   530,
     454,   684,    16,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   685,   692,   444,   700,    80,
      81,   696,    82,   701,   698,   699,    16,    83,    84,   505,
     705,   708,   709,   710,   442,   717,   720,   721,   739,    17,
     743,   455,   456,   457,   458,   459,   460,   461,   462,   742,
     455,   456,   457,   458,   459,   460,   461,   462,   448,   746,
     747,   722,   723,   724,   725,   726,   748,    19,   727,   749,
     728,   729,   769,   737,   745,   750,    21,   449,   450,   451,
     452,   765,   755,   768,   690,   454,   770,   773,    22,   822,
     823,   824,   825,   826,    17,   689,   827,   828,   776,   784,
     781,   777,   786,   829,   830,   831,   807,   879,   878,   877,
      18,   894,   885,   887,   886,   890,   897,   902,    17,   903,
     906,   908,    19,   907,   910,   832,   911,   922,   923,   924,
      20,    21,   925,   926,   930,   455,   456,   457,   458,   459,
     460,   461,   462,    22,   933,  1004,    19,   942,    85,    86,
      87,    88,   934,    89,    90,    21,   935,    91,    92,    93,
    1005,   936,    94,   937,    95,   938,   953,    22,    96,    97,
     964,   975,   939,   940,   941,   943,   944,   945,   946,   947,
     948,    98,    99,   949,   950,   100,   951,   952,   954,   101,
     611,   612,   613,   614,   615,   616,   617,   618,   619,   620,
     621,   622,   623,   624,   625,   626,   627,   955,   628,   629,
     630,   631,   632,   633,   956,   957,   634,   958,   959,   635,
     636,   637,   638,   639,   640,   641,   642,   833,   834,   835,
     836,   837,   960,   961,   838,   839,   962,   963,   965,   986,
     966,   840,   841,   842,   844,   845,   846,   847,   848,   997,
    1000,   849,   850,   967,  1001,   968,   969,   970,   851,   852,
     853,   971,   972,   843,   973,   974,   976,   855,   856,   857,
     858,   859,   977,   978,   860,   861,   767,  1013,  1015,   979,
     854,   862,   863,   864,   866,   867,   868,   869,   870,   980,
    1021,   871,   872,  1022,  1019,   981,   982,  1024,   873,   874,
     875,  1026,  1027,   865,  1028,   983,   984,   985,   987,   988,
     989,   990,  1029,   991,   992,  1030,   993,   994,   995,   996,
     876,   998,  1031,  1008,  1032,  1009,  1010,  1011,  1033,  1034,
    1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,
    1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,  1054,
    1055,  1056,  1057,  1058,  1059,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1067,  1069,  1068,  1070,  1071,  1072,  1073,  1074,
    1075,  1076,  1077,  1078,  1079,  1080,  1081,  1082,  1083,  1084,
    1085,  1086,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1096,
    1097,  1098,  1100,  1107,  1102,  1108,  1103,  1117,  1109,  1110,
    1111,  1112,  1113,  1114,  1115,  1116,  1118,  1119,  1120,  1121,
    1122,  1123,  1124,  1125,  1128,  1126,  1127,  1129,  1130,  1131,
    1132,  1133,  1134,  1135,  1136,  1137,  1138,  1139,  1140,  1141,
    1150,  1142,  1143,  1144,  1145,  1146,  1147,  1148,  1149,  1151,
    1152,  1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,  1161,
    1162,  1163,  1164,  1165,  1166,  1167,  1168,  1172,  1169,  1179,
    1170,  1171,  1173,  1193,  1194,  1198,  1174,  1175,  1176,  1187,
    1188,  1189,  1190,  1191,  1192,  1196,  1197,  1199,  1200,  1206,
    1201,  1203,  1207,  1208,  1212,  1213,  1214,  1215,  1216,  1219,
    1227,  1226,  1228,  1229,  1230,  1231,  1232,  1233,  1234,  1240,
    1242,  1241,  1243,  1244,  1245,  1246,  1247,  1248,  1250,  1249,
    1251,  1253,  1254,   216,   165,   880,   741,   523,   128,   484,
     882,   363,   719,  1016,  1020,   899,   373,   921,   682,   584,
     534,   900,   582,   415,  1003,   348,   917,   889,     0,     0,
     555
};

static const yytype_int16 yycheck[] =
{
     245,   222,     8,    22,   643,   280,   278,   243,   160,   766,
     525,   591,   257,   667,     6,   592,     3,    54,     4,     5,
       6,   254,     5,     6,   176,     3,     8,     4,     5,     6,
       5,     6,   717,     3,   279,    61,   717,   717,   190,    77,
     717,    81,   275,   276,   717,   717,     4,    65,    61,     4,
     136,     6,    74,     3,    20,    20,    22,    22,    71,    65,
      15,    27,    27,    36,     0,   645,    75,    76,   722,   723,
       8,     7,   134,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    54,    32,    33,     4,     3,    23,     5,     6,
      26,    75,    28,    29,    30,     7,    51,    87,    72,     3,
      12,     3,    14,   165,    16,    60,     3,    65,     5,     6,
      33,    32,    33,    34,   100,   101,   102,    38,    39,    83,
     106,   366,   367,   100,   101,   102,    81,    65,     3,   106,
       5,     6,   218,    32,    33,    34,   158,    33,   383,    38,
      39,    77,    67,    68,    61,   902,    65,   102,   103,   135,
     136,   137,    54,    78,    79,    80,   130,   737,   135,   136,
     137,   406,   407,     3,    61,   155,   156,   133,   389,   402,
     125,   416,   417,   418,   419,   420,   421,   127,    54,   129,
     218,   218,   222,   155,   156,    39,    61,   142,   143,   144,
      33,   155,   156,   465,   220,   470,   151,   152,   153,   444,
     445,    69,    70,   780,   222,     8,    77,    50,   220,   218,
     222,    77,   204,    83,   159,   234,   449,   450,   451,   452,
     453,   454,   455,   456,   457,   458,   459,   460,   461,   462,
     216,   218,   187,   169,   217,   213,   218,   223,   224,   216,
     925,   216,   217,   213,   925,   925,     3,   224,   925,   185,
      86,   168,   925,   925,   219,     6,   501,   169,     3,   184,
     279,   197,    65,   134,     3,     4,     5,     6,   134,   205,
     206,   168,     6,   194,   495,     0,   231,   189,   932,   155,
     156,   193,   218,   172,   196,   155,   156,   155,   156,    86,
       8,   536,   537,   168,   165,   194,   131,    10,   519,   165,
     545,    39,   547,   155,   156,   526,    37,   155,   156,     8,
      48,    24,    25,   221,    65,   150,   151,   152,   153,   155,
     156,   155,   156,   158,     3,    64,     8,   560,     8,   155,
     156,    65,   131,    72,    73,    32,   155,   156,   222,   223,
     561,  1098,  1099,    82,    41,    84,    77,    65,    71,     8,
      20,    71,    22,   152,   153,   155,   156,    27,   155,   156,
     605,   100,   101,   102,     8,   371,  1005,   106,   155,   156,
     222,   219,     3,   208,   209,   210,   211,   212,   213,   214,
     215,   155,   156,    65,     8,    65,   405,    83,   222,    85,
     345,    87,   907,   219,   155,   156,   135,   136,   137,   138,
     219,   140,   554,   142,     8,   144,    65,   146,    72,   148,
     209,   210,   211,   212,   213,   214,   215,   200,   157,   219,
     203,    65,    72,   668,   220,   670,     8,   382,   217,    72,
      14,     8,   219,   222,    77,    33,  1193,    35,  1195,  1019,
     179,    65,   181,     3,   183,   219,   679,     3,     4,     5,
       6,    32,    33,   218,  1211,   217,  1213,   217,   219,     3,
     222,    65,   222,   127,   697,   129,   130,     3,   132,    50,
     217,   692,   211,   212,   213,   222,   762,   216,   764,   218,
     219,   131,   718,    65,   127,   224,   129,   130,    65,    39,
       3,   763,    42,    43,   166,   167,     3,    47,    48,   171,
     150,   151,   152,   153,   185,   186,   512,   217,   158,   190,
     529,   219,   222,    76,   222,   760,    72,    73,     6,    83,
      54,    85,   767,    87,   745,     3,    82,   120,    84,   122,
     123,    87,   179,   126,   181,   222,   183,     3,     4,     5,
       6,     3,   561,   194,   100,   101,   102,   159,   160,   161,
     106,     3,     4,     5,     6,   173,   174,  1196,   208,   209,
     210,   211,   212,   213,   214,   215,     6,   219,    39,   219,
     222,    42,    43,  1212,   194,  1214,    47,    48,   194,   135,
     136,   137,   138,     4,   140,   192,   142,   219,   144,    75,
     146,  1230,   148,   173,   174,    55,    56,    57,    58,    59,
      60,   157,    62,    63,   879,   219,    72,    73,   222,   219,
    1249,    77,   222,   219,  1253,   194,    82,    21,    84,    53,
      54,    55,    56,   179,   194,   181,    64,   183,     3,     4,
       5,     6,   160,     3,   100,   101,   102,   199,   200,   201,
     106,   219,   170,    66,   222,   173,   174,   175,   176,   177,
      46,    47,    48,    49,    72,   211,   212,   213,   219,     3,
     216,   222,   218,   191,   192,   218,   194,   195,   224,   135,
     136,   137,   138,   692,   140,    61,   142,    61,   144,    73,
     146,   219,   148,     3,   222,     3,    89,    90,    91,    92,
      93,   157,   219,    96,    97,   222,   219,    72,    73,   222,
     103,   104,   105,   219,     3,   711,   222,    82,   219,    84,
       3,   222,    65,   179,     4,   181,   219,   183,     3,     4,
       5,     6,   125,   187,   188,   100,   101,   102,   213,   214,
     215,   106,     3,     4,     5,     6,   219,   219,   219,   222,
     222,   222,   155,   156,     3,   211,   212,   213,    44,    45,
     216,   219,   218,     4,   222,     3,   219,    61,   224,   222,
     135,   136,   137,   138,   218,   140,     4,   142,   165,   144,
       6,   146,   219,   148,     3,   222,   219,  1022,   219,   222,
     219,   222,   157,   222,     5,     6,   219,    72,    73,   222,
     219,     5,     6,   222,    44,    45,     6,    82,     4,    84,
    1021,    72,    42,    43,   179,     3,   181,     4,   183,   922,
     923,    82,    54,    84,     4,   100,   101,   102,   194,     3,
       3,   106,    52,    67,    73,   133,     3,    61,   208,   100,
     101,   102,    77,   218,    77,   106,   211,   212,   213,   218,
     218,   216,     4,   218,     4,     4,     4,   218,   220,   224,
     135,   136,   137,   138,     6,   140,   218,   142,  1103,   144,
     218,   146,   218,   148,   135,   136,   137,   138,   218,   140,
     218,   142,   157,   144,     3,   146,   218,   148,   218,   218,
       6,    46,    46,    76,     4,    77,   157,     6,    76,   219,
       4,   219,    68,     3,   179,     4,   181,     7,   183,     9,
      10,    11,    12,    13,    14,    54,    16,   218,   179,     3,
     181,     3,   183,    23,    77,   218,    26,   218,    28,    29,
      30,   218,     4,   218,   218,    72,   211,   212,   213,   218,
     218,   216,   218,   218,    72,     4,    74,     4,   225,   224,
     211,   212,   213,   219,    76,   216,     3,   218,   218,   216,
       6,     5,     7,   224,     9,    10,    11,    12,    13,    14,
      15,    16,    17,     6,     6,    42,   218,    77,    23,     3,
       6,    26,   218,    28,    29,    30,     7,   218,     9,    10,
      11,    12,    13,    14,   131,    16,   219,     4,    75,   128,
     165,   165,    23,   131,   222,    26,   218,    28,    29,    30,
       3,     3,   133,   150,   151,   152,   153,   222,   155,    75,
       3,   158,   150,   151,   152,   153,   154,   158,   219,    72,
     158,     4,    77,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   222,   218,   155,     6,    32,
      33,   130,    35,     6,   225,   225,    77,    40,    41,     3,
       3,     6,     4,     4,   222,   178,     4,   218,    31,   169,
       4,   208,   209,   210,   211,   212,   213,   214,   215,   219,
     208,   209,   210,   211,   212,   213,   214,   215,   131,     6,
       6,   218,   218,   218,   218,   218,     4,   197,   218,     3,
     218,   218,     4,   218,   218,     6,   206,   150,   151,   152,
     153,   222,   219,   219,     6,   158,    77,   219,   218,    89,
      90,    91,    92,    93,   169,     5,    96,    97,    49,     6,
     208,    46,   222,   103,   104,   105,     6,   132,   128,   130,
     185,   216,   218,   165,   133,   219,   216,     4,   169,   222,
     219,   219,   197,   218,     6,   125,     6,    56,    56,     3,
     205,   206,   222,    51,   219,   208,   209,   210,   211,   212,
     213,   214,   215,   218,   222,     4,   197,    91,   161,   162,
     163,   164,   222,   166,   167,   206,   222,   170,   171,   172,
       3,   222,   175,   222,   177,   222,    91,   218,   181,   182,
      91,    91,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   194,   195,   222,   222,   198,   222,   222,   222,   202,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   222,   106,   107,
     108,   109,   110,   111,   222,   222,   114,   222,   222,   117,
     118,   119,   120,   121,   122,   123,   124,    89,    90,    91,
      92,    93,   222,   222,    96,    97,   222,   222,   222,    91,
     222,   103,   104,   105,    89,    90,    91,    92,    93,    91,
     133,    96,    97,   222,   133,   222,   222,   222,   103,   104,
     105,   222,   222,   125,   222,   222,   222,    89,    90,    91,
      92,    93,   222,   222,    96,    97,    65,   219,     6,   222,
     125,   103,   104,   105,    89,    90,    91,    92,    93,   222,
      52,    96,    97,    53,   218,   222,   222,     6,   103,   104,
     105,     6,     6,   125,     6,   222,   222,   222,   222,   222,
     222,   222,     6,   222,   222,     6,   222,   222,   222,   222,
     125,   222,     6,   222,     6,   222,   222,   222,     6,     6,
     222,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   222,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   222,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   222,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   222,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   222,     6,     4,     4,     4,
       4,     4,   219,   219,     6,   219,    61,     6,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,     6,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,     6,   219,   219,
       6,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,     6,
     219,   219,   219,   219,   219,   219,   219,     6,   219,   165,
     219,   219,   219,     4,     4,    48,   222,   222,   222,   219,
     219,   219,   219,   219,   219,   219,   219,     6,     6,   165,
     222,   222,   218,     6,   219,     4,   219,    48,     4,     6,
     219,   218,     6,    47,   219,   219,    39,    39,     4,   218,
      39,   219,    39,    39,    39,     3,   218,   218,     3,   219,
       3,   219,   219,   131,    65,   734,   647,   384,    22,   347,
     736,   218,   605,   920,   925,   764,   229,   786,   546,   469,
     408,   764,   467,   264,   885,   189,   783,   752,    -1,    -1,
     442
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    26,    28,    29,    30,    77,   169,   185,   197,
     205,   206,   218,   227,   228,   229,   231,   243,   244,   246,
     247,   250,   251,   252,   253,   254,   278,   283,   284,   285,
     286,   287,   288,   289,   290,    32,    33,    34,    38,    39,
     194,    36,    32,    33,    34,    38,    39,   194,     3,   276,
      75,   276,   166,   167,   171,     3,   230,   231,   243,   244,
     247,   250,   251,   252,   283,   284,   285,   286,   287,     3,
      32,    33,    35,    40,    41,   161,   162,   163,   164,   166,
     167,   170,   171,   172,   175,   177,   181,   182,   194,   195,
     198,   202,    33,    33,    39,   159,   160,   161,     3,     3,
     276,     3,   279,   280,   172,     7,    12,    14,    16,   169,
     189,   193,   196,    32,    33,    33,    50,   159,   253,   254,
       0,   221,   339,    20,    22,    27,   272,     8,   255,   257,
      71,   338,   338,   338,   338,   338,   340,     3,   276,    71,
     337,   337,   337,   337,   337,     3,   220,    14,   276,    75,
      76,   218,     3,     3,     3,   230,   218,     3,   276,     6,
     204,   173,   174,   173,   174,     3,    76,     6,     3,   199,
     200,   201,   200,   203,   276,   276,     3,    61,    54,   222,
       6,   194,   194,   185,   186,   190,   160,   170,   173,   174,
     175,   176,   177,   191,   192,   194,   195,   194,     4,   192,
      75,   194,   194,   276,   219,   219,   229,    21,   218,   256,
     257,    64,   264,    66,   258,    72,     3,   276,   276,   276,
       3,    61,    61,   218,   245,    73,     3,   276,   276,   276,
       3,     3,     3,   248,   249,    65,   269,     4,   336,   336,
       3,     4,     5,     6,    72,    73,    82,    84,   100,   101,
     102,   106,   135,   136,   137,   138,   140,   142,   144,   146,
     148,   157,   179,   181,   183,   211,   212,   213,   216,   218,
     224,   291,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   304,   305,   306,   307,   308,   310,   311,   312,   313,
     314,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   328,   329,   330,   331,   332,   333,     3,     5,     6,
      61,   168,     3,     5,     6,    61,   168,     3,     5,     6,
      61,   168,   219,    39,    42,    43,    47,    48,     3,     3,
     336,     4,    10,    24,    25,    61,   276,   218,   280,   336,
       4,   165,     6,     3,     6,     4,     3,     4,    54,     4,
     194,     3,     3,   256,   257,   291,    52,    67,   262,    73,
     133,    54,   218,   245,   276,    32,    33,    50,     3,   242,
      37,   254,    61,   208,   222,   269,   294,    77,    77,   218,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,    72,    73,   295,   218,   218,    87,   294,   309,
       4,     4,     4,     4,     6,   333,   218,   218,   218,   218,
     218,   218,   218,   120,   122,   123,   126,   218,   218,   295,
     295,     5,     6,   217,   314,   326,   327,   254,   294,   223,
     313,   219,   222,    54,   155,   156,    72,    74,   131,   150,
     151,   152,   153,   154,   158,   208,   209,   210,   211,   212,
     213,   214,   215,   220,   217,   222,   217,   222,   217,   222,
     222,   223,   217,   222,   217,   222,     3,     6,    46,    46,
      76,   276,    77,   341,   255,     4,    39,    48,     6,    76,
     187,   188,     4,   219,   219,    81,   265,   259,   260,   294,
     294,    68,   263,     4,   252,     3,   127,   129,   232,   234,
     235,   241,    54,   218,   345,     3,     3,   219,   222,   218,
     292,   276,   294,   249,   218,   218,    64,   219,   291,   218,
      72,   254,   294,   294,   309,    83,    85,    87,   294,   294,
     294,   294,   294,   294,     4,   218,   218,   218,   218,     4,
       4,   225,   219,   219,    76,   293,     3,   294,   294,    74,
     158,   218,    72,   130,   295,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,     3,   213,
     314,     6,   326,     6,   327,   313,     6,     5,    42,    44,
      45,   218,   218,     3,   218,   219,     6,    32,    41,     4,
     165,   165,   291,    75,   266,   222,    69,    70,   261,   294,
     133,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   106,   107,
     108,   109,   110,   111,   114,   117,   118,   119,   120,   121,
     122,   123,   124,   237,   128,   218,   219,   222,   252,     3,
     133,     3,   291,   222,    67,    68,    78,    79,    80,   184,
     334,   335,   334,   291,   219,   254,   219,    54,    86,    83,
      85,   294,   294,   219,   219,   219,   219,   219,   219,    75,
     294,     3,   312,   294,     4,   222,   268,   219,   222,     5,
       6,   336,   218,   295,   254,   291,   130,   155,   225,   225,
       6,     6,   242,   233,   235,     3,   343,   344,     6,     4,
       4,   218,   273,   274,   275,   276,   281,   178,   267,   260,
       4,   218,   218,   218,   218,   218,   218,   218,   218,   218,
      72,   127,   129,   130,   238,   239,   341,   218,   242,    31,
     342,   234,   219,     4,   219,   218,     6,     6,     4,     3,
       6,   219,   222,   219,   219,   219,   237,   294,   294,    83,
      86,   295,   222,   222,   222,   222,     4,    65,   219,     4,
      77,   254,   291,   219,   219,   295,    49,    46,   219,   219,
     222,   208,   219,   222,     6,   252,   222,    55,    57,    58,
      59,    60,    62,    63,   282,     3,    54,   277,   296,   297,
     298,   299,   300,   301,   302,   303,   269,     6,   237,   236,
     237,    89,    90,    91,    92,    93,    96,    97,   103,   104,
     105,   125,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   125,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   125,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   125,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   125,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   125,   130,   128,   132,
     239,   240,   240,   242,   219,   218,   133,   165,   291,   335,
     219,    83,   294,   219,   216,   328,   314,   216,   315,   318,
     323,   328,     4,   222,   268,   294,   219,   218,   219,   219,
       6,     6,   235,     3,     4,     5,     6,   344,    33,    35,
     219,   274,    56,    56,     3,   222,    51,   271,   219,   222,
     219,   219,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,    91,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,    91,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,    91,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,    91,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,    91,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,    91,   222,   313,
     133,   133,   219,   343,     4,     3,   219,     6,   222,   222,
     222,   222,   268,   219,   334,     6,   277,   275,   275,   218,
     302,    52,    53,   270,     6,   237,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   222,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   222,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   222,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   222,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   222,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     222,     6,     4,     4,   219,   341,     4,     4,     4,     4,
     219,   219,     6,    61,   242,   291,   294,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,     6,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,     6,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,     6,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
       6,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,     6,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,     6,   219,   222,   222,   222,   268,   268,   165,
      39,    42,    43,    47,    48,   294,   219,   219,   219,   219,
     219,   219,   219,     4,     4,     6,   219,   219,    48,     6,
       6,   222,   268,   222,   268,   341,   165,   218,     6,    44,
      45,     6,   219,     4,   219,    48,     4,    42,    43,     6,
     268,   341,   268,   134,   165,   341,   218,   219,     6,    47,
     219,   219,    39,    39,     4,   134,   165,   341,   134,   165,
     218,   219,    39,    39,    39,    39,     3,   218,   218,   219,
       3,     3,   341,   219,   219,   341
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   226,   227,   228,   228,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   232,   232,   233,   233,   234,   234,   235,
     235,   235,   235,   236,   236,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   238,
     238,   239,   239,   239,   239,   240,   240,   241,   241,   242,
     242,   243,   244,   244,   245,   245,   246,   246,   247,   248,
     248,   249,   250,   250,   250,   250,   250,   251,   251,   251,
     252,   252,   252,   252,   253,   253,   254,   255,   256,   256,
     257,   258,   258,   259,   259,   260,   261,   261,   261,   262,
     262,   263,   263,   264,   264,   265,   265,   266,   266,   267,
     267,   268,   268,   269,   269,   270,   270,   271,   271,   272,
     272,   272,   272,   273,   273,   274,   274,   275,   275,   276,
     276,   277,   277,   277,   277,   278,   278,   279,   279,   280,
     281,   281,   282,   282,   282,   282,   282,   282,   282,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     284,   284,   284,   285,   285,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     287,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   289,
     289,   289,   290,   290,   291,   291,   292,   292,   293,   293,
     294,   294,   294,   294,   294,   295,   295,   295,   295,   295,
     295,   295,   295,   295,   295,   295,   295,   295,   296,   296,
     296,   297,   297,   297,   297,   298,   298,   298,   298,   299,
     299,   299,   299,   300,   300,   301,   301,   302,   302,   302,
     302,   302,   302,   303,   303,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   304,   304,   304,   304,   304,   304,
     304,   304,   304,   304,   305,   305,   306,   307,   307,   308,
     308,   308,   308,   309,   309,   310,   311,   311,   311,   311,
     312,   312,   312,   312,   313,   313,   313,   313,   313,   313,
     313,   313,   313,   313,   313,   313,   313,   314,   314,   314,
     314,   315,   315,   315,   316,   317,   317,   318,   318,   319,
     320,   320,   321,   322,   322,   323,   324,   324,   325,   325,
     326,   327,   328,   328,   329,   330,   330,   331,   332,   332,
     333,   333,   333,   333,   333,   333,   333,   333,   333,   333,
     333,   333,   334,   334,   335,   335,   335,   335,   335,   335,
     336,   337,   337,   338,   338,   339,   339,   340,   340,   341,
     341,   342,   342,   343,   343,   344,   344,   344,   344,   344,
     345,   345
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
       2,     2,     2,     2,     5,     2,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     6,     6,     5,     3,     4,     4,     2,     5,
       3,     3,     4,     6,     7,     9,    10,    12,    12,    13,
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
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2482 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2490 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 247 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2504 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 227 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2518 "parser.cpp"
        break;

    case YYSYMBOL_column_def_array: /* column_def_array  */
#line 237 "parser.y"
            {
    fprintf(stderr, "destroy column def array\n");
    if ((((*yyvaluep).column_def_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).column_def_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).column_def_array_t));
    }
}
#line 2532 "parser.cpp"
        break;

    case YYSYMBOL_column_type_array: /* column_type_array  */
#line 191 "parser.y"
            {
    fprintf(stderr, "destroy column_type_array\n");
    delete (((*yyvaluep).column_type_array_t));
}
#line 2541 "parser.cpp"
        break;

    case YYSYMBOL_column_type: /* column_type  */
#line 186 "parser.y"
            {
    fprintf(stderr, "destroy column_type\n");
    delete (((*yyvaluep).column_type_t));
}
#line 2550 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2569 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2578 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2587 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 294 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 287 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2612 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2622 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2632 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2642 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2662 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 369 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2672 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2686 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2700 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2710 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2726 "parser.cpp"
        break;

    case YYSYMBOL_highlight_clause: /* highlight_clause  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2749 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2757 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2765 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2773 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2781 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2795 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2804 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2813 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 314 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2835 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 304 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2858 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 304 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2872 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 363 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2882 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2891 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2905 "parser.cpp"
        break;

    case YYSYMBOL_insert_row_list: /* insert_row_list  */
#line 267 "parser.y"
            {
    fprintf(stderr, "destroy insert row list\n");
    if ((((*yyvaluep).insert_row_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).insert_row_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).insert_row_list_t));
    }
}
#line 2919 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2927 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2935 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2943 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2951 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2959 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 257 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 3013 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3021 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3029 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3037 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3045 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3053 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 375 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 3066 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3074 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3082 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 339 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 3090 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3098 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3106 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3114 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3122 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3130 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3138 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3146 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3154 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3162 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3170 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3178 "parser.cpp"
        break;

    case YYSYMBOL_curly_brackets_expr: /* curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3186 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_curly_brackets_expr: /* unclosed_curly_brackets_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3194 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 384 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3202 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 388 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3210 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3218 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3226 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3234 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3242 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3250 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 343 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3258 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 330 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3266 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3277 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 203 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3291 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 203 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3305 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 196 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3316 "parser.cpp"
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
#line 88 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 3424 "parser.cpp"

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
#line 520 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3639 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 524 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3650 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 530 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3661 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 537 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3667 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 538 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3673 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 539 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3679 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3685 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 541 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3691 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 542 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3697 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3703 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3709 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 545 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3715 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 546 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3721 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 547 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3727 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 548 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3733 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3739 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 550 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3745 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 551 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3751 "parser.cpp"
    break;

  case 20: /* statement: check_statement  */
#line 552 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].check_stmt); }
#line 3757 "parser.cpp"
    break;

  case 21: /* explainable_statement: create_statement  */
#line 554 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3763 "parser.cpp"
    break;

  case 22: /* explainable_statement: drop_statement  */
#line 555 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3769 "parser.cpp"
    break;

  case 23: /* explainable_statement: copy_statement  */
#line 556 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3775 "parser.cpp"
    break;

  case 24: /* explainable_statement: show_statement  */
#line 557 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3781 "parser.cpp"
    break;

  case 25: /* explainable_statement: select_statement  */
#line 558 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3787 "parser.cpp"
    break;

  case 26: /* explainable_statement: delete_statement  */
#line 559 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3793 "parser.cpp"
    break;

  case 27: /* explainable_statement: update_statement  */
#line 560 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3799 "parser.cpp"
    break;

  case 28: /* explainable_statement: insert_statement  */
#line 561 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3805 "parser.cpp"
    break;

  case 29: /* explainable_statement: flush_statement  */
#line 562 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3811 "parser.cpp"
    break;

  case 30: /* explainable_statement: optimize_statement  */
#line 563 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3817 "parser.cpp"
    break;

  case 31: /* explainable_statement: command_statement  */
#line 564 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3823 "parser.cpp"
    break;

  case 32: /* explainable_statement: compact_statement  */
#line 565 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3829 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER COMMENT STRING  */
#line 572 "parser.y"
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
#line 3851 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 589 "parser.y"
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
#line 3871 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 606 "parser.y"
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
#line 3889 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 622 "parser.y"
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
#line 3922 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 651 "parser.y"
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
#line 3942 "parser.cpp"
    break;

  case 38: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list COMMENT STRING  */
#line 666 "parser.y"
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
#line 3978 "parser.cpp"
    break;

  case 39: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement COMMENT STRING  */
#line 698 "parser.y"
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
#line 4000 "parser.cpp"
    break;

  case 40: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 716 "parser.y"
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
#line 4021 "parser.cpp"
    break;

  case 41: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 734 "parser.y"
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
#line 4054 "parser.cpp"
    break;

  case 42: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info COMMENT STRING  */
#line 762 "parser.y"
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
#line 4089 "parser.cpp"
    break;

  case 43: /* table_element_array: table_element  */
#line 793 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 4098 "parser.cpp"
    break;

  case 44: /* table_element_array: table_element_array ',' table_element  */
#line 797 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 4107 "parser.cpp"
    break;

  case 45: /* column_def_array: table_column  */
#line 802 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 4116 "parser.cpp"
    break;

  case 46: /* column_def_array: column_def_array ',' table_column  */
#line 806 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 4125 "parser.cpp"
    break;

  case 47: /* table_element: table_column  */
#line 812 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 4133 "parser.cpp"
    break;

  case 48: /* table_element: table_constraint  */
#line 815 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 4141 "parser.cpp"
    break;

  case 49: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 822 "parser.y"
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
#line 4166 "parser.cpp"
    break;

  case 50: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 842 "parser.y"
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
#line 4193 "parser.cpp"
    break;

  case 51: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 864 "parser.y"
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
#line 4222 "parser.cpp"
    break;

  case 52: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 888 "parser.y"
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
#line 4252 "parser.cpp"
    break;

  case 53: /* column_type_array: column_type  */
#line 914 "parser.y"
                                {
    (yyval.column_type_array_t) = new std::vector<std::unique_ptr<infinity::ColumnType>>();
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4261 "parser.cpp"
    break;

  case 54: /* column_type_array: column_type_array ',' column_type  */
#line 918 "parser.y"
                                    {
    (yyval.column_type_array_t) = (yyvsp[-2].column_type_array_t);
    (yyval.column_type_array_t)->emplace_back((yyvsp[0].column_type_t));
}
#line 4270 "parser.cpp"
    break;

  case 55: /* column_type: BOOLEAN  */
#line 924 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4276 "parser.cpp"
    break;

  case 56: /* column_type: TINYINT  */
#line 925 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4282 "parser.cpp"
    break;

  case 57: /* column_type: SMALLINT  */
#line 926 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4288 "parser.cpp"
    break;

  case 58: /* column_type: INTEGER  */
#line 927 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4294 "parser.cpp"
    break;

  case 59: /* column_type: INT  */
#line 928 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4300 "parser.cpp"
    break;

  case 60: /* column_type: BIGINT  */
#line 929 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4306 "parser.cpp"
    break;

  case 61: /* column_type: HUGEINT  */
#line 930 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4312 "parser.cpp"
    break;

  case 62: /* column_type: FLOAT  */
#line 931 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4318 "parser.cpp"
    break;

  case 63: /* column_type: REAL  */
#line 932 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4324 "parser.cpp"
    break;

  case 64: /* column_type: DOUBLE  */
#line 933 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4330 "parser.cpp"
    break;

  case 65: /* column_type: FLOAT16  */
#line 934 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4336 "parser.cpp"
    break;

  case 66: /* column_type: BFLOAT16  */
#line 935 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4342 "parser.cpp"
    break;

  case 67: /* column_type: DATE  */
#line 936 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4348 "parser.cpp"
    break;

  case 68: /* column_type: TIME  */
#line 937 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4354 "parser.cpp"
    break;

  case 69: /* column_type: DATETIME  */
#line 938 "parser.y"
           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4360 "parser.cpp"
    break;

  case 70: /* column_type: TIMESTAMP  */
#line 939 "parser.y"
            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4366 "parser.cpp"
    break;

  case 71: /* column_type: UUID  */
#line 940 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4372 "parser.cpp"
    break;

  case 72: /* column_type: POINT  */
#line 941 "parser.y"
        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4378 "parser.cpp"
    break;

  case 73: /* column_type: LINE  */
#line 942 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4384 "parser.cpp"
    break;

  case 74: /* column_type: LSEG  */
#line 943 "parser.y"
       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4390 "parser.cpp"
    break;

  case 75: /* column_type: BOX  */
#line 944 "parser.y"
      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4396 "parser.cpp"
    break;

  case 76: /* column_type: CIRCLE  */
#line 947 "parser.y"
         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4402 "parser.cpp"
    break;

  case 77: /* column_type: VARCHAR  */
#line 949 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4408 "parser.cpp"
    break;

  case 78: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 950 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4414 "parser.cpp"
    break;

  case 79: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 951 "parser.y"
                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4420 "parser.cpp"
    break;

  case 80: /* column_type: DECIMAL  */
#line 952 "parser.y"
          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4426 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4432 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4438 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4444 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4450 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4456 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4462 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4468 "parser.cpp"
    break;

  case 88: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                          { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4474 "parser.cpp"
    break;

  case 89: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4480 "parser.cpp"
    break;

  case 90: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4486 "parser.cpp"
    break;

  case 91: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4492 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4498 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4504 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4510 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4516 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4522 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4528 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4534 "parser.cpp"
    break;

  case 99: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4540 "parser.cpp"
    break;

  case 100: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4546 "parser.cpp"
    break;

  case 101: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4552 "parser.cpp"
    break;

  case 102: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4558 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4564 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4570 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4576 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4582 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4588 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4594 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4600 "parser.cpp"
    break;

  case 110: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4606 "parser.cpp"
    break;

  case 111: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4612 "parser.cpp"
    break;

  case 112: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4618 "parser.cpp"
    break;

  case 113: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4624 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4630 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4636 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4642 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4648 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4654 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4660 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                           { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4666 "parser.cpp"
    break;

  case 121: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                            { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4672 "parser.cpp"
    break;

  case 122: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                             { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4678 "parser.cpp"
    break;

  case 123: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                              { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4684 "parser.cpp"
    break;

  case 124: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4690 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4696 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4702 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4708 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4714 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4720 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4726 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4732 "parser.cpp"
    break;

  case 132: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4738 "parser.cpp"
    break;

  case 133: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4744 "parser.cpp"
    break;

  case 134: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4750 "parser.cpp"
    break;

  case 135: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4756 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4762 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4768 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4774 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4780 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                    { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4786 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4792 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                      { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4798 "parser.cpp"
    break;

  case 143: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                       { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4804 "parser.cpp"
    break;

  case 144: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                        { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4810 "parser.cpp"
    break;

  case 145: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                         { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4816 "parser.cpp"
    break;

  case 146: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                                 { (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4822 "parser.cpp"
    break;

  case 147: /* column_type: ARRAY '(' column_type ')'  */
#line 1021 "parser.y"
                            {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kArray, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_.emplace_back((yyvsp[-1].column_type_t));
}
#line 4831 "parser.cpp"
    break;

  case 148: /* column_type: TUPLE '(' column_type_array ')'  */
#line 1025 "parser.y"
                                  {
  (yyval.column_type_t) = new infinity::ColumnType{infinity::LogicalType::kTuple, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid};
  (yyval.column_type_t)->element_types_ = std::move(*((yyvsp[-1].column_type_array_t)));
  delete (yyvsp[-1].column_type_array_t);
}
#line 4841 "parser.cpp"
    break;

  case 149: /* column_constraints: column_constraint  */
#line 1048 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4850 "parser.cpp"
    break;

  case 150: /* column_constraints: column_constraints column_constraint  */
#line 1052 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4864 "parser.cpp"
    break;

  case 151: /* column_constraint: PRIMARY KEY  */
#line 1062 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4872 "parser.cpp"
    break;

  case 152: /* column_constraint: UNIQUE  */
#line 1065 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4880 "parser.cpp"
    break;

  case 153: /* column_constraint: NULLABLE  */
#line 1068 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4888 "parser.cpp"
    break;

  case 154: /* column_constraint: NOT NULLABLE  */
#line 1071 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4896 "parser.cpp"
    break;

  case 155: /* default_expr: DEFAULT constant_expr  */
#line 1075 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4904 "parser.cpp"
    break;

  case 156: /* default_expr: %empty  */
#line 1078 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4912 "parser.cpp"
    break;

  case 157: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1083 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4922 "parser.cpp"
    break;

  case 158: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1088 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4932 "parser.cpp"
    break;

  case 159: /* identifier_array: IDENTIFIER  */
#line 1095 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4943 "parser.cpp"
    break;

  case 160: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1101 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4954 "parser.cpp"
    break;

  case 161: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1111 "parser.y"
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
#line 4971 "parser.cpp"
    break;

  case 162: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES insert_row_list  */
#line 1127 "parser.y"
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
#line 5014 "parser.cpp"
    break;

  case 163: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1165 "parser.y"
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
#line 5034 "parser.cpp"
    break;

  case 164: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1181 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 5042 "parser.cpp"
    break;

  case 165: /* optional_identifier_array: %empty  */
#line 1184 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 5050 "parser.cpp"
    break;

  case 166: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1191 "parser.y"
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
#line 5073 "parser.cpp"
    break;

  case 167: /* explain_statement: EXPLAIN explainable_statement  */
#line 1208 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 5083 "parser.cpp"
    break;

  case 168: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1217 "parser.y"
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
#line 5100 "parser.cpp"
    break;

  case 169: /* update_expr_array: update_expr  */
#line 1230 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 5109 "parser.cpp"
    break;

  case 170: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1234 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 5118 "parser.cpp"
    break;

  case 171: /* update_expr: IDENTIFIER '=' expr  */
#line 1239 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 5130 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1252 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 5146 "parser.cpp"
    break;

  case 173: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1265 "parser.y"
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
#line 5164 "parser.cpp"
    break;

  case 174: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1280 "parser.y"
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
#line 5182 "parser.cpp"
    break;

  case 175: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1295 "parser.y"
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
#line 5200 "parser.cpp"
    break;

  case 176: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1310 "parser.y"
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
#line 5223 "parser.cpp"
    break;

  case 177: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1333 "parser.y"
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
#line 5281 "parser.cpp"
    break;

  case 178: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1386 "parser.y"
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
#line 5341 "parser.cpp"
    break;

  case 179: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1441 "parser.y"
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
#line 5393 "parser.cpp"
    break;

  case 180: /* select_statement: select_without_paren  */
#line 1492 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5401 "parser.cpp"
    break;

  case 181: /* select_statement: select_with_paren  */
#line 1495 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5409 "parser.cpp"
    break;

  case 182: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
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
#line 5423 "parser.cpp"
    break;

  case 183: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1507 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5437 "parser.cpp"
    break;

  case 184: /* select_with_paren: '(' select_without_paren ')'  */
#line 1517 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5445 "parser.cpp"
    break;

  case 185: /* select_with_paren: '(' select_with_paren ')'  */
#line 1520 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5453 "parser.cpp"
    break;

  case 186: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1524 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5462 "parser.cpp"
    break;

  case 187: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1529 "parser.y"
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
#line 5493 "parser.cpp"
    break;

  case 188: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1556 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5501 "parser.cpp"
    break;

  case 189: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1559 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5509 "parser.cpp"
    break;

  case 190: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1564 "parser.y"
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
#line 5530 "parser.cpp"
    break;

  case 191: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1581 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5538 "parser.cpp"
    break;

  case 192: /* order_by_clause: %empty  */
#line 1584 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5546 "parser.cpp"
    break;

  case 193: /* order_by_expr_list: order_by_expr  */
#line 1588 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5555 "parser.cpp"
    break;

  case 194: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1592 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5564 "parser.cpp"
    break;

  case 195: /* order_by_expr: expr order_by_type  */
#line 1597 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5574 "parser.cpp"
    break;

  case 196: /* order_by_type: ASC  */
#line 1603 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5582 "parser.cpp"
    break;

  case 197: /* order_by_type: DESC  */
#line 1606 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5590 "parser.cpp"
    break;

  case 198: /* order_by_type: %empty  */
#line 1609 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5598 "parser.cpp"
    break;

  case 199: /* limit_expr: LIMIT expr  */
#line 1613 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5606 "parser.cpp"
    break;

  case 200: /* limit_expr: %empty  */
#line 1617 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5612 "parser.cpp"
    break;

  case 201: /* offset_expr: OFFSET expr  */
#line 1619 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5620 "parser.cpp"
    break;

  case 202: /* offset_expr: %empty  */
#line 1623 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5626 "parser.cpp"
    break;

  case 203: /* distinct: DISTINCT  */
#line 1625 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5634 "parser.cpp"
    break;

  case 204: /* distinct: %empty  */
#line 1628 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5642 "parser.cpp"
    break;

  case 205: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1632 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5650 "parser.cpp"
    break;

  case 206: /* highlight_clause: %empty  */
#line 1635 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5658 "parser.cpp"
    break;

  case 207: /* from_clause: FROM table_reference  */
#line 1639 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5666 "parser.cpp"
    break;

  case 208: /* from_clause: %empty  */
#line 1642 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5674 "parser.cpp"
    break;

  case 209: /* search_clause: SEARCH sub_search_array  */
#line 1646 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5684 "parser.cpp"
    break;

  case 210: /* search_clause: %empty  */
#line 1651 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5692 "parser.cpp"
    break;

  case 211: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1655 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5700 "parser.cpp"
    break;

  case 212: /* optional_search_filter_expr: %empty  */
#line 1658 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5708 "parser.cpp"
    break;

  case 213: /* where_clause: WHERE expr  */
#line 1662 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5716 "parser.cpp"
    break;

  case 214: /* where_clause: %empty  */
#line 1665 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5724 "parser.cpp"
    break;

  case 215: /* having_clause: HAVING expr  */
#line 1669 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5732 "parser.cpp"
    break;

  case 216: /* having_clause: %empty  */
#line 1672 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5740 "parser.cpp"
    break;

  case 217: /* group_by_clause: GROUP BY expr_array  */
#line 1676 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5748 "parser.cpp"
    break;

  case 218: /* group_by_clause: %empty  */
#line 1679 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5756 "parser.cpp"
    break;

  case 219: /* set_operator: UNION  */
#line 1683 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5764 "parser.cpp"
    break;

  case 220: /* set_operator: UNION ALL  */
#line 1686 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5772 "parser.cpp"
    break;

  case 221: /* set_operator: INTERSECT  */
#line 1689 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5780 "parser.cpp"
    break;

  case 222: /* set_operator: EXCEPT  */
#line 1692 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5788 "parser.cpp"
    break;

  case 223: /* table_reference: table_reference_unit  */
#line 1700 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5796 "parser.cpp"
    break;

  case 224: /* table_reference: table_reference ',' table_reference_unit  */
#line 1703 "parser.y"
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
#line 5814 "parser.cpp"
    break;

  case 227: /* table_reference_name: table_name table_alias  */
#line 1720 "parser.y"
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
#line 5832 "parser.cpp"
    break;

  case 228: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1734 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5843 "parser.cpp"
    break;

  case 229: /* table_name: IDENTIFIER  */
#line 1743 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5853 "parser.cpp"
    break;

  case 230: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1748 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5865 "parser.cpp"
    break;

  case 231: /* table_alias: AS IDENTIFIER  */
#line 1757 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5875 "parser.cpp"
    break;

  case 232: /* table_alias: IDENTIFIER  */
#line 1762 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5885 "parser.cpp"
    break;

  case 233: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1767 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5896 "parser.cpp"
    break;

  case 234: /* table_alias: %empty  */
#line 1773 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5904 "parser.cpp"
    break;

  case 235: /* with_clause: WITH with_expr_list  */
#line 1780 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5912 "parser.cpp"
    break;

  case 236: /* with_clause: %empty  */
#line 1783 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5920 "parser.cpp"
    break;

  case 237: /* with_expr_list: with_expr  */
#line 1787 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5929 "parser.cpp"
    break;

  case 238: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1790 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5938 "parser.cpp"
    break;

  case 239: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1795 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5950 "parser.cpp"
    break;

  case 240: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1807 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5962 "parser.cpp"
    break;

  case 241: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1814 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5975 "parser.cpp"
    break;

  case 242: /* join_type: INNER  */
#line 1828 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5983 "parser.cpp"
    break;

  case 243: /* join_type: LEFT  */
#line 1831 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5991 "parser.cpp"
    break;

  case 244: /* join_type: RIGHT  */
#line 1834 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5999 "parser.cpp"
    break;

  case 245: /* join_type: OUTER  */
#line 1837 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6007 "parser.cpp"
    break;

  case 246: /* join_type: FULL  */
#line 1840 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 6015 "parser.cpp"
    break;

  case 247: /* join_type: CROSS  */
#line 1843 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 6023 "parser.cpp"
    break;

  case 248: /* join_type: %empty  */
#line 1846 "parser.y"
                {
}
#line 6030 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW DATABASES  */
#line 1852 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 6039 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TABLES  */
#line 1856 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 6048 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TASKS  */
#line 1860 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTasks;
}
#line 6057 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW CONFIGS  */
#line 1864 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 6066 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1868 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6078 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW PROFILES  */
#line 1875 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 6087 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW BUFFER  */
#line 1879 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 6096 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW MEMINDEX  */
#line 1883 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 6105 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW QUERIES  */
#line 1887 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 6114 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1891 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 6124 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TRANSACTIONS  */
#line 1896 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6133 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1900 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6143 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TRANSACTION HISTORY  */
#line 1905 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactionHistory;
}
#line 6152 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW SESSION VARIABLES  */
#line 1909 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6161 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1913 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6170 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1917 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6181 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1923 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6192 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1929 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6203 "parser.cpp"
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
#line 6219 "parser.cpp"
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
#line 6235 "parser.cpp"
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
#line 6251 "parser.cpp"
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
#line 6268 "parser.cpp"
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
#line 6285 "parser.cpp"
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
#line 6303 "parser.cpp"
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
#line 6322 "parser.cpp"
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
#line 6338 "parser.cpp"
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

    (yyval.show_stmt)->index_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6357 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 2044 "parser.y"
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
#line 6378 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2060 "parser.y"
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
#line 6400 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW LOGS  */
#line 2077 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6409 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW CATALOG  */
#line 2081 "parser.y"
               {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalog;
}
#line 6418 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW CATALOG TO file_path  */
#line 2085 "parser.y"
                            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogToFile;
      (yyval.show_stmt)->file_path_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6429 "parser.cpp"
    break;

  case 281: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2091 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6438 "parser.cpp"
    break;

  case 282: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2095 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6447 "parser.cpp"
    break;

  case 283: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2099 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6458 "parser.cpp"
    break;

  case 284: /* show_statement: SHOW MEMORY  */
#line 2105 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6467 "parser.cpp"
    break;

  case 285: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2109 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6476 "parser.cpp"
    break;

  case 286: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2113 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6485 "parser.cpp"
    break;

  case 287: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2117 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6496 "parser.cpp"
    break;

  case 288: /* show_statement: SHOW SNAPSHOTS  */
#line 2123 "parser.y"
                 {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kListSnapshots;
}
#line 6505 "parser.cpp"
    break;

  case 289: /* show_statement: SHOW SNAPSHOT IDENTIFIER  */
#line 2127 "parser.y"
                           {
     (yyval.show_stmt) = new infinity::ShowStatement();
     (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kShowSnapshot;
     (yyval.show_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6516 "parser.cpp"
    break;

  case 290: /* flush_statement: FLUSH DATA  */
#line 2137 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6525 "parser.cpp"
    break;

  case 291: /* flush_statement: FLUSH LOG  */
#line 2141 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6534 "parser.cpp"
    break;

  case 292: /* flush_statement: FLUSH BUFFER  */
#line 2145 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6543 "parser.cpp"
    break;

  case 293: /* optimize_statement: OPTIMIZE table_name  */
#line 2153 "parser.y"
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
#line 6558 "parser.cpp"
    break;

  case 294: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2164 "parser.y"
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
#line 6582 "parser.cpp"
    break;

  case 295: /* command_statement: USE IDENTIFIER  */
#line 2187 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6593 "parser.cpp"
    break;

  case 296: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2193 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6603 "parser.cpp"
    break;

  case 297: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2198 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6614 "parser.cpp"
    break;

  case 298: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2204 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6625 "parser.cpp"
    break;

  case 299: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2210 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6638 "parser.cpp"
    break;

  case 300: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2218 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6649 "parser.cpp"
    break;

  case 301: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2224 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6660 "parser.cpp"
    break;

  case 302: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2230 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6671 "parser.cpp"
    break;

  case 303: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2236 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6682 "parser.cpp"
    break;

  case 304: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2242 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6695 "parser.cpp"
    break;

  case 305: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2250 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6706 "parser.cpp"
    break;

  case 306: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2256 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6717 "parser.cpp"
    break;

  case 307: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2262 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6728 "parser.cpp"
    break;

  case 308: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2268 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6739 "parser.cpp"
    break;

  case 309: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2274 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6752 "parser.cpp"
    break;

  case 310: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2282 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6763 "parser.cpp"
    break;

  case 311: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2288 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6774 "parser.cpp"
    break;

  case 312: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON TABLE IDENTIFIER  */
#line 2294 "parser.y"
                                                 {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kTable, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6787 "parser.cpp"
    break;

  case 313: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON DATABASE IDENTIFIER  */
#line 2302 "parser.y"
                                                    {
    ParserHelper::ToLower((yyvsp[-3].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-3].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kDatabase, (yyvsp[0].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[0].str_value));
}
#line 6800 "parser.cpp"
    break;

  case 314: /* command_statement: CREATE SNAPSHOT IDENTIFIER ON SYSTEM  */
#line 2310 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[-2].str_value), infinity::SnapshotOp::kCreate, infinity::SnapshotScope::kSystem);
    free((yyvsp[-2].str_value));
}
#line 6811 "parser.cpp"
    break;

  case 315: /* command_statement: DROP SNAPSHOT IDENTIFIER  */
#line 2316 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kDrop, infinity::SnapshotScope::kIgnore);
    free((yyvsp[0].str_value));
}
#line 6822 "parser.cpp"
    break;

  case 316: /* command_statement: RESTORE DATABASE SNAPSHOT IDENTIFIER  */
#line 2322 "parser.y"
                                       {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kDatabase);
    free((yyvsp[0].str_value));
}
#line 6833 "parser.cpp"
    break;

  case 317: /* command_statement: RESTORE TABLE SNAPSHOT IDENTIFIER  */
#line 2328 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SnapshotCmd>((yyvsp[0].str_value), infinity::SnapshotOp::kRestore, infinity::SnapshotScope::kTable);
    free((yyvsp[0].str_value));
}
#line 6844 "parser.cpp"
    break;

  case 318: /* command_statement: CLEAN DATA  */
#line 2334 "parser.y"
             {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::CleanupCmd>();
}
#line 6853 "parser.cpp"
    break;

  case 319: /* command_statement: DUMP INDEX IDENTIFIER ON table_name  */
#line 2338 "parser.y"
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
#line 6871 "parser.cpp"
    break;

  case 320: /* compact_statement: COMPACT TABLE table_name  */
#line 2352 "parser.y"
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
#line 6888 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2365 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6897 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2369 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6907 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2374 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6918 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2380 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6930 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2387 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6943 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2395 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6957 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2404 "parser.y"
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
#line 6972 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2414 "parser.y"
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
#line 6987 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2424 "parser.y"
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
#line 7003 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2435 "parser.y"
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
#line 7019 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2446 "parser.y"
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
#line 7036 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2458 "parser.y"
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
#line 7053 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2470 "parser.y"
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
#line 7068 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2480 "parser.y"
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
#line 7084 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2491 "parser.y"
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
#line 7101 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2503 "parser.y"
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
#line 7119 "parser.cpp"
    break;

  case 337: /* admin_statement: ADMIN SHOW LOGS  */
#line 2516 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 7128 "parser.cpp"
    break;

  case 338: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2520 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 7138 "parser.cpp"
    break;

  case 339: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2525 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 7148 "parser.cpp"
    break;

  case 340: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2530 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 7159 "parser.cpp"
    break;

  case 341: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2536 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 7168 "parser.cpp"
    break;

  case 342: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2540 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 7177 "parser.cpp"
    break;

  case 343: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2544 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7188 "parser.cpp"
    break;

  case 344: /* admin_statement: ADMIN CREATE SNAPSHOT  */
#line 2550 "parser.y"
                        {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kCreateSnapshot;
}
#line 7197 "parser.cpp"
    break;

  case 345: /* admin_statement: ADMIN SHOW SNAPSHOTS  */
#line 2554 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListSnapshots;
}
#line 7206 "parser.cpp"
    break;

  case 346: /* admin_statement: ADMIN SHOW SNAPSHOT IDENTIFIER  */
#line 2558 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7217 "parser.cpp"
    break;

  case 347: /* admin_statement: ADMIN DELETE SNAPSHOT STRING  */
#line 2564 "parser.y"
                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kDeleteSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7228 "parser.cpp"
    break;

  case 348: /* admin_statement: ADMIN EXPORT SNAPSHOT STRING TO STRING  */
#line 2570 "parser.y"
                                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kExportSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[-2].str_value);
     (yyval.admin_stmt)->export_path_ = (yyvsp[0].str_value);
     free((yyvsp[-2].str_value));
     free((yyvsp[0].str_value));
}
#line 7241 "parser.cpp"
    break;

  case 349: /* admin_statement: ADMIN RECOVER FROM SNAPSHOT STRING  */
#line 2578 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRecoverFromSnapshot;
     (yyval.admin_stmt)->snapshot_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7252 "parser.cpp"
    break;

  case 350: /* admin_statement: ADMIN SHOW NODES  */
#line 2584 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 7261 "parser.cpp"
    break;

  case 351: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2588 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7272 "parser.cpp"
    break;

  case 352: /* admin_statement: ADMIN SHOW NODE  */
#line 2594 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 7281 "parser.cpp"
    break;

  case 353: /* admin_statement: ADMIN REMOVE NODE STRING  */
#line 2598 "parser.y"
                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kRemoveNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7292 "parser.cpp"
    break;

  case 354: /* admin_statement: ADMIN SET ADMIN  */
#line 2604 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kAdmin;
}
#line 7302 "parser.cpp"
    break;

  case 355: /* admin_statement: ADMIN SET STANDALONE  */
#line 2609 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kStandalone;
}
#line 7312 "parser.cpp"
    break;

  case 356: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2614 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7324 "parser.cpp"
    break;

  case 357: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2621 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7338 "parser.cpp"
    break;

  case 358: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2630 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->node_role_ = infinity::NodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7352 "parser.cpp"
    break;

  case 359: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2640 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7366 "parser.cpp"
    break;

  case 360: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2649 "parser.y"
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
#line 7383 "parser.cpp"
    break;

  case 361: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2661 "parser.y"
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
#line 7399 "parser.cpp"
    break;

  case 362: /* check_statement: CHECK SYSTEM  */
#line 2673 "parser.y"
                               {
    (yyval.check_stmt) = new infinity::CheckStatement();
    (yyval.check_stmt)->check_type_ = infinity::CheckStmtType::kSystem;
}
#line 7408 "parser.cpp"
    break;

  case 363: /* check_statement: CHECK TABLE table_name  */
#line 2677 "parser.y"
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
#line 7424 "parser.cpp"
    break;

  case 364: /* expr_array: expr_alias  */
#line 2693 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7433 "parser.cpp"
    break;

  case 365: /* expr_array: expr_array ',' expr_alias  */
#line 2697 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7442 "parser.cpp"
    break;

  case 366: /* insert_row_list: '(' expr_array ')'  */
#line 2702 "parser.y"
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
#line 7457 "parser.cpp"
    break;

  case 367: /* insert_row_list: insert_row_list ',' '(' expr_array ')'  */
#line 2712 "parser.y"
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
#line 7472 "parser.cpp"
    break;

  case 368: /* expr_alias: expr AS IDENTIFIER  */
#line 2734 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7483 "parser.cpp"
    break;

  case 369: /* expr_alias: expr  */
#line 2740 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7491 "parser.cpp"
    break;

  case 375: /* operand: '(' expr ')'  */
#line 2750 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7499 "parser.cpp"
    break;

  case 376: /* operand: '(' select_without_paren ')'  */
#line 2753 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7510 "parser.cpp"
    break;

  case 377: /* operand: constant_expr  */
#line 2759 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7518 "parser.cpp"
    break;

  case 388: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2775 "parser.y"
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
#line 7538 "parser.cpp"
    break;

  case 389: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')'  */
#line 2791 "parser.y"
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
#line 7559 "parser.cpp"
    break;

  case 390: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')' IGNORE INDEX  */
#line 2808 "parser.y"
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
#line 7580 "parser.cpp"
    break;

  case 391: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2826 "parser.y"
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
#line 7630 "parser.cpp"
    break;

  case 392: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2872 "parser.y"
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
#line 7673 "parser.cpp"
    break;

  case 393: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2911 "parser.y"
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
#line 7720 "parser.cpp"
    break;

  case 394: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2954 "parser.y"
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
#line 7768 "parser.cpp"
    break;

  case 395: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 3001 "parser.y"
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
#line 7796 "parser.cpp"
    break;

  case 396: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 3025 "parser.y"
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
#line 7823 "parser.cpp"
    break;

  case 397: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 3048 "parser.y"
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
#line 7848 "parser.cpp"
    break;

  case 398: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 3069 "parser.y"
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
#line 7873 "parser.cpp"
    break;

  case 399: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3090 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7887 "parser.cpp"
    break;

  case 400: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3099 "parser.y"
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
#line 7903 "parser.cpp"
    break;

  case 401: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3110 "parser.y"
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
#line 7919 "parser.cpp"
    break;

  case 402: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')' USING INDEXES '(' STRING ')'  */
#line 3121 "parser.y"
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
#line 7937 "parser.cpp"
    break;

  case 403: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 3135 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7949 "parser.cpp"
    break;

  case 404: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 3142 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7963 "parser.cpp"
    break;

  case 405: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3152 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7974 "parser.cpp"
    break;

  case 406: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3158 "parser.y"
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
#line 7990 "parser.cpp"
    break;

  case 407: /* sub_search: match_vector_expr  */
#line 3170 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7998 "parser.cpp"
    break;

  case 408: /* sub_search: match_text_expr  */
#line 3173 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8006 "parser.cpp"
    break;

  case 409: /* sub_search: match_tensor_expr  */
#line 3176 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8014 "parser.cpp"
    break;

  case 410: /* sub_search: match_sparse_expr  */
#line 3179 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8022 "parser.cpp"
    break;

  case 411: /* sub_search: query_expr  */
#line 3182 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8030 "parser.cpp"
    break;

  case 412: /* sub_search: fusion_expr  */
#line 3185 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 8038 "parser.cpp"
    break;

  case 413: /* sub_search_array: sub_search  */
#line 3189 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 8047 "parser.cpp"
    break;

  case 414: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3193 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 8056 "parser.cpp"
    break;

  case 415: /* function_expr: IDENTIFIER '(' ')'  */
#line 3198 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 8069 "parser.cpp"
    break;

  case 416: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3206 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 8082 "parser.cpp"
    break;

  case 417: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3214 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 8096 "parser.cpp"
    break;

  case 418: /* function_expr: YEAR '(' expr ')'  */
#line 3223 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "year";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8108 "parser.cpp"
    break;

  case 419: /* function_expr: MONTH '(' expr ')'  */
#line 3230 "parser.y"
                     {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "month";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8120 "parser.cpp"
    break;

  case 420: /* function_expr: DAY '(' expr ')'  */
#line 3237 "parser.y"
                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "day";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8132 "parser.cpp"
    break;

  case 421: /* function_expr: HOUR '(' expr ')'  */
#line 3244 "parser.y"
                    {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "hour";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8144 "parser.cpp"
    break;

  case 422: /* function_expr: MINUTE '(' expr ')'  */
#line 3251 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "minute";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8156 "parser.cpp"
    break;

  case 423: /* function_expr: SECOND '(' expr ')'  */
#line 3258 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "second";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-1].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8168 "parser.cpp"
    break;

  case 424: /* function_expr: operand IS NOT NULLABLE  */
#line 3265 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8180 "parser.cpp"
    break;

  case 425: /* function_expr: operand IS NULLABLE  */
#line 3272 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8192 "parser.cpp"
    break;

  case 426: /* function_expr: NOT operand  */
#line 3279 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8204 "parser.cpp"
    break;

  case 427: /* function_expr: '-' operand  */
#line 3286 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8216 "parser.cpp"
    break;

  case 428: /* function_expr: '+' operand  */
#line 3293 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8228 "parser.cpp"
    break;

  case 429: /* function_expr: operand '-' operand  */
#line 3300 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8241 "parser.cpp"
    break;

  case 430: /* function_expr: operand '+' operand  */
#line 3308 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8254 "parser.cpp"
    break;

  case 431: /* function_expr: operand '*' operand  */
#line 3316 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8267 "parser.cpp"
    break;

  case 432: /* function_expr: operand '/' operand  */
#line 3324 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8280 "parser.cpp"
    break;

  case 433: /* function_expr: operand '%' operand  */
#line 3332 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8293 "parser.cpp"
    break;

  case 434: /* function_expr: operand '=' operand  */
#line 3340 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8306 "parser.cpp"
    break;

  case 435: /* function_expr: operand EQUAL operand  */
#line 3348 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8319 "parser.cpp"
    break;

  case 436: /* function_expr: operand NOT_EQ operand  */
#line 3356 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8332 "parser.cpp"
    break;

  case 437: /* function_expr: operand '<' operand  */
#line 3364 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8345 "parser.cpp"
    break;

  case 438: /* function_expr: operand '>' operand  */
#line 3372 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8358 "parser.cpp"
    break;

  case 439: /* function_expr: operand LESS_EQ operand  */
#line 3380 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8371 "parser.cpp"
    break;

  case 440: /* function_expr: operand GREATER_EQ operand  */
#line 3388 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8384 "parser.cpp"
    break;

  case 441: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3396 "parser.y"
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
#line 8419 "parser.cpp"
    break;

  case 442: /* function_expr: operand LIKE operand  */
#line 3426 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8432 "parser.cpp"
    break;

  case 443: /* function_expr: operand NOT LIKE operand  */
#line 3434 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8445 "parser.cpp"
    break;

  case 444: /* conjunction_expr: expr AND expr  */
#line 3443 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8458 "parser.cpp"
    break;

  case 445: /* conjunction_expr: expr OR expr  */
#line 3451 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8471 "parser.cpp"
    break;

  case 446: /* between_expr: operand BETWEEN operand AND operand  */
#line 3460 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8483 "parser.cpp"
    break;

  case 447: /* in_expr: operand IN '(' expr_array ')'  */
#line 3468 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8494 "parser.cpp"
    break;

  case 448: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3474 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8505 "parser.cpp"
    break;

  case 449: /* case_expr: CASE expr case_check_array END  */
#line 3481 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8516 "parser.cpp"
    break;

  case 450: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3487 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8528 "parser.cpp"
    break;

  case 451: /* case_expr: CASE case_check_array END  */
#line 3494 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8538 "parser.cpp"
    break;

  case 452: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3499 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8549 "parser.cpp"
    break;

  case 453: /* case_check_array: WHEN expr THEN expr  */
#line 3506 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8561 "parser.cpp"
    break;

  case 454: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3513 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8573 "parser.cpp"
    break;

  case 455: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3521 "parser.y"
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
#line 8590 "parser.cpp"
    break;

  case 456: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3534 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8601 "parser.cpp"
    break;

  case 457: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3540 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8612 "parser.cpp"
    break;

  case 458: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3546 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8624 "parser.cpp"
    break;

  case 459: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3553 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8636 "parser.cpp"
    break;

  case 460: /* column_expr: IDENTIFIER  */
#line 3561 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8648 "parser.cpp"
    break;

  case 461: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3568 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8660 "parser.cpp"
    break;

  case 462: /* column_expr: '*'  */
#line 3575 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8670 "parser.cpp"
    break;

  case 463: /* column_expr: column_expr '.' '*'  */
#line 3580 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8684 "parser.cpp"
    break;

  case 464: /* constant_expr: STRING  */
#line 3590 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8694 "parser.cpp"
    break;

  case 465: /* constant_expr: TRUE  */
#line 3595 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8704 "parser.cpp"
    break;

  case 466: /* constant_expr: FALSE  */
#line 3600 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8714 "parser.cpp"
    break;

  case 467: /* constant_expr: DOUBLE_VALUE  */
#line 3605 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8724 "parser.cpp"
    break;

  case 468: /* constant_expr: LONG_VALUE  */
#line 3610 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8734 "parser.cpp"
    break;

  case 469: /* constant_expr: DATE STRING  */
#line 3615 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8744 "parser.cpp"
    break;

  case 470: /* constant_expr: TIME STRING  */
#line 3620 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8754 "parser.cpp"
    break;

  case 471: /* constant_expr: DATETIME STRING  */
#line 3625 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8764 "parser.cpp"
    break;

  case 472: /* constant_expr: TIMESTAMP STRING  */
#line 3630 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8774 "parser.cpp"
    break;

  case 473: /* constant_expr: INTERVAL interval_expr  */
#line 3635 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8782 "parser.cpp"
    break;

  case 474: /* constant_expr: interval_expr  */
#line 3638 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8790 "parser.cpp"
    break;

  case 475: /* constant_expr: common_array_expr  */
#line 3641 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8798 "parser.cpp"
    break;

  case 476: /* constant_expr: curly_brackets_expr  */
#line 3644 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8806 "parser.cpp"
    break;

  case 477: /* common_array_expr: array_expr  */
#line 3648 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8814 "parser.cpp"
    break;

  case 478: /* common_array_expr: subarray_array_expr  */
#line 3651 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8822 "parser.cpp"
    break;

  case 479: /* common_array_expr: sparse_array_expr  */
#line 3654 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8830 "parser.cpp"
    break;

  case 480: /* common_array_expr: empty_array_expr  */
#line 3657 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8838 "parser.cpp"
    break;

  case 481: /* common_sparse_array_expr: sparse_array_expr  */
#line 3661 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8846 "parser.cpp"
    break;

  case 482: /* common_sparse_array_expr: array_expr  */
#line 3664 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8854 "parser.cpp"
    break;

  case 483: /* common_sparse_array_expr: empty_array_expr  */
#line 3667 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8862 "parser.cpp"
    break;

  case 484: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3671 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8870 "parser.cpp"
    break;

  case 485: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3675 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8880 "parser.cpp"
    break;

  case 486: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3680 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8889 "parser.cpp"
    break;

  case 487: /* sparse_array_expr: long_sparse_array_expr  */
#line 3685 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8897 "parser.cpp"
    break;

  case 488: /* sparse_array_expr: double_sparse_array_expr  */
#line 3688 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8905 "parser.cpp"
    break;

  case 489: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3692 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8913 "parser.cpp"
    break;

  case 490: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3696 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8925 "parser.cpp"
    break;

  case 491: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3703 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8936 "parser.cpp"
    break;

  case 492: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3710 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8944 "parser.cpp"
    break;

  case 493: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3714 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8956 "parser.cpp"
    break;

  case 494: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3721 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8967 "parser.cpp"
    break;

  case 495: /* empty_array_expr: '[' ']'  */
#line 3728 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8975 "parser.cpp"
    break;

  case 496: /* curly_brackets_expr: unclosed_curly_brackets_expr '}'  */
#line 3732 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8983 "parser.cpp"
    break;

  case 497: /* curly_brackets_expr: '{' '}'  */
#line 3735 "parser.y"
          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
}
#line 8991 "parser.cpp"
    break;

  case 498: /* unclosed_curly_brackets_expr: '{' constant_expr  */
#line 3739 "parser.y"
                                                {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kCurlyBracketsArray);
    (yyval.const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
}
#line 9000 "parser.cpp"
    break;

  case 499: /* unclosed_curly_brackets_expr: unclosed_curly_brackets_expr ',' constant_expr  */
#line 3743 "parser.y"
                                                 {
    (yyvsp[-2].const_expr_t)->curly_brackets_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9009 "parser.cpp"
    break;

  case 500: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3748 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 9017 "parser.cpp"
    break;

  case 501: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3752 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 9025 "parser.cpp"
    break;

  case 502: /* array_expr: long_array_expr  */
#line 3756 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9033 "parser.cpp"
    break;

  case 503: /* array_expr: double_array_expr  */
#line 3759 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 9041 "parser.cpp"
    break;

  case 504: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3763 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9049 "parser.cpp"
    break;

  case 505: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3767 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9059 "parser.cpp"
    break;

  case 506: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3772 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9068 "parser.cpp"
    break;

  case 507: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3777 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 9076 "parser.cpp"
    break;

  case 508: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3781 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 9086 "parser.cpp"
    break;

  case 509: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3786 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 9095 "parser.cpp"
    break;

  case 510: /* interval_expr: LONG_VALUE SECONDS  */
#line 3791 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9106 "parser.cpp"
    break;

  case 511: /* interval_expr: LONG_VALUE SECOND  */
#line 3797 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9117 "parser.cpp"
    break;

  case 512: /* interval_expr: LONG_VALUE MINUTES  */
#line 3803 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9128 "parser.cpp"
    break;

  case 513: /* interval_expr: LONG_VALUE MINUTE  */
#line 3809 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9139 "parser.cpp"
    break;

  case 514: /* interval_expr: LONG_VALUE HOURS  */
#line 3815 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9150 "parser.cpp"
    break;

  case 515: /* interval_expr: LONG_VALUE HOUR  */
#line 3821 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9161 "parser.cpp"
    break;

  case 516: /* interval_expr: LONG_VALUE DAYS  */
#line 3827 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9172 "parser.cpp"
    break;

  case 517: /* interval_expr: LONG_VALUE DAY  */
#line 3833 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9183 "parser.cpp"
    break;

  case 518: /* interval_expr: LONG_VALUE MONTHS  */
#line 3839 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9194 "parser.cpp"
    break;

  case 519: /* interval_expr: LONG_VALUE MONTH  */
#line 3845 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9205 "parser.cpp"
    break;

  case 520: /* interval_expr: LONG_VALUE YEARS  */
#line 3851 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9216 "parser.cpp"
    break;

  case 521: /* interval_expr: LONG_VALUE YEAR  */
#line 3857 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 9227 "parser.cpp"
    break;

  case 522: /* copy_option_list: copy_option  */
#line 3868 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 9236 "parser.cpp"
    break;

  case 523: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3872 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 9245 "parser.cpp"
    break;

  case 524: /* copy_option: FORMAT IDENTIFIER  */
#line 3877 "parser.y"
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
#line 9281 "parser.cpp"
    break;

  case 525: /* copy_option: DELIMITER STRING  */
#line 3908 "parser.y"
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
#line 9296 "parser.cpp"
    break;

  case 526: /* copy_option: HEADER  */
#line 3918 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 9306 "parser.cpp"
    break;

  case 527: /* copy_option: OFFSET LONG_VALUE  */
#line 3923 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 9316 "parser.cpp"
    break;

  case 528: /* copy_option: LIMIT LONG_VALUE  */
#line 3928 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 9326 "parser.cpp"
    break;

  case 529: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3933 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 9336 "parser.cpp"
    break;

  case 530: /* file_path: STRING  */
#line 3939 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 9344 "parser.cpp"
    break;

  case 531: /* if_exists: IF EXISTS  */
#line 3943 "parser.y"
                     { (yyval.bool_value) = true; }
#line 9350 "parser.cpp"
    break;

  case 532: /* if_exists: %empty  */
#line 3944 "parser.y"
  { (yyval.bool_value) = false; }
#line 9356 "parser.cpp"
    break;

  case 533: /* if_not_exists: IF NOT EXISTS  */
#line 3946 "parser.y"
                              { (yyval.bool_value) = true; }
#line 9362 "parser.cpp"
    break;

  case 534: /* if_not_exists: %empty  */
#line 3947 "parser.y"
  { (yyval.bool_value) = false; }
#line 9368 "parser.cpp"
    break;

  case 537: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3962 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9381 "parser.cpp"
    break;

  case 538: /* if_not_exists_info: %empty  */
#line 3970 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 9389 "parser.cpp"
    break;

  case 539: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3974 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9397 "parser.cpp"
    break;

  case 540: /* with_index_param_list: %empty  */
#line 3977 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 9405 "parser.cpp"
    break;

  case 541: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3981 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 9413 "parser.cpp"
    break;

  case 542: /* optional_table_properties_list: %empty  */
#line 3984 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 9421 "parser.cpp"
    break;

  case 543: /* index_param_list: index_param  */
#line 3988 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 9430 "parser.cpp"
    break;

  case 544: /* index_param_list: index_param_list ',' index_param  */
#line 3992 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 9439 "parser.cpp"
    break;

  case 545: /* index_param: IDENTIFIER  */
#line 3997 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9450 "parser.cpp"
    break;

  case 546: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 4003 "parser.y"
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

  case 547: /* index_param: IDENTIFIER '=' STRING  */
#line 4013 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 9480 "parser.cpp"
    break;

  case 548: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 4023 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 9493 "parser.cpp"
    break;

  case 549: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 4031 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9506 "parser.cpp"
    break;

  case 550: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 4042 "parser.y"
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
#line 9542 "parser.cpp"
    break;

  case 551: /* index_info: '(' IDENTIFIER ')'  */
#line 4073 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9553 "parser.cpp"
    break;


#line 9557 "parser.cpp"

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

#line 4080 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

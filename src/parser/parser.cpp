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
  YYSYMBOL_AST = 39,                       /* AST  */
  YYSYMBOL_PIPELINE = 40,                  /* PIPELINE  */
  YYSYMBOL_RAW = 41,                       /* RAW  */
  YYSYMBOL_LOGICAL = 42,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 43,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 44,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 45,                      /* VIEW  */
  YYSYMBOL_INDEX = 46,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 47,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 48,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 49,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 50,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 51,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 52,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 53,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 54,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 55,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 56,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 57,                     /* CHUNK  */
  YYSYMBOL_GROUP = 58,                     /* GROUP  */
  YYSYMBOL_BY = 59,                        /* BY  */
  YYSYMBOL_HAVING = 60,                    /* HAVING  */
  YYSYMBOL_AS = 61,                        /* AS  */
  YYSYMBOL_NATURAL = 62,                   /* NATURAL  */
  YYSYMBOL_JOIN = 63,                      /* JOIN  */
  YYSYMBOL_LEFT = 64,                      /* LEFT  */
  YYSYMBOL_RIGHT = 65,                     /* RIGHT  */
  YYSYMBOL_OUTER = 66,                     /* OUTER  */
  YYSYMBOL_FULL = 67,                      /* FULL  */
  YYSYMBOL_ON = 68,                        /* ON  */
  YYSYMBOL_INNER = 69,                     /* INNER  */
  YYSYMBOL_CROSS = 70,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 71,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 72,                     /* WHERE  */
  YYSYMBOL_ORDER = 73,                     /* ORDER  */
  YYSYMBOL_LIMIT = 74,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 75,                    /* OFFSET  */
  YYSYMBOL_ASC = 76,                       /* ASC  */
  YYSYMBOL_DESC = 77,                      /* DESC  */
  YYSYMBOL_IF = 78,                        /* IF  */
  YYSYMBOL_NOT = 79,                       /* NOT  */
  YYSYMBOL_EXISTS = 80,                    /* EXISTS  */
  YYSYMBOL_IN = 81,                        /* IN  */
  YYSYMBOL_FROM = 82,                      /* FROM  */
  YYSYMBOL_TO = 83,                        /* TO  */
  YYSYMBOL_WITH = 84,                      /* WITH  */
  YYSYMBOL_DELIMITER = 85,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 86,                    /* FORMAT  */
  YYSYMBOL_HEADER = 87,                    /* HEADER  */
  YYSYMBOL_CAST = 88,                      /* CAST  */
  YYSYMBOL_END = 89,                       /* END  */
  YYSYMBOL_CASE = 90,                      /* CASE  */
  YYSYMBOL_ELSE = 91,                      /* ELSE  */
  YYSYMBOL_THEN = 92,                      /* THEN  */
  YYSYMBOL_WHEN = 93,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 94,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 95,                   /* INTEGER  */
  YYSYMBOL_INT = 96,                       /* INT  */
  YYSYMBOL_TINYINT = 97,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 98,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 99,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 100,                  /* HUGEINT  */
  YYSYMBOL_VARCHAR = 101,                  /* VARCHAR  */
  YYSYMBOL_FLOAT = 102,                    /* FLOAT  */
  YYSYMBOL_DOUBLE = 103,                   /* DOUBLE  */
  YYSYMBOL_REAL = 104,                     /* REAL  */
  YYSYMBOL_DECIMAL = 105,                  /* DECIMAL  */
  YYSYMBOL_DATE = 106,                     /* DATE  */
  YYSYMBOL_TIME = 107,                     /* TIME  */
  YYSYMBOL_DATETIME = 108,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 109,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 110,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 111,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 112,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 113,                     /* UUID  */
  YYSYMBOL_POINT = 114,                    /* POINT  */
  YYSYMBOL_LINE = 115,                     /* LINE  */
  YYSYMBOL_LSEG = 116,                     /* LSEG  */
  YYSYMBOL_BOX = 117,                      /* BOX  */
  YYSYMBOL_PATH = 118,                     /* PATH  */
  YYSYMBOL_POLYGON = 119,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 120,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 121,                     /* BLOB  */
  YYSYMBOL_BITMAP = 122,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 123,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 124,                   /* VECTOR  */
  YYSYMBOL_BIT = 125,                      /* BIT  */
  YYSYMBOL_TEXT = 126,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 127,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 128,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 129,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 130,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 131,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 132,                  /* PRIMARY  */
  YYSYMBOL_KEY = 133,                      /* KEY  */
  YYSYMBOL_UNIQUE = 134,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 135,                 /* NULLABLE  */
  YYSYMBOL_IS = 136,                       /* IS  */
  YYSYMBOL_DEFAULT = 137,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 138,                     /* TRUE  */
  YYSYMBOL_FALSE = 139,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 140,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 141,                   /* SECOND  */
  YYSYMBOL_SECONDS = 142,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 143,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 144,                  /* MINUTES  */
  YYSYMBOL_HOUR = 145,                     /* HOUR  */
  YYSYMBOL_HOURS = 146,                    /* HOURS  */
  YYSYMBOL_DAY = 147,                      /* DAY  */
  YYSYMBOL_DAYS = 148,                     /* DAYS  */
  YYSYMBOL_MONTH = 149,                    /* MONTH  */
  YYSYMBOL_MONTHS = 150,                   /* MONTHS  */
  YYSYMBOL_YEAR = 151,                     /* YEAR  */
  YYSYMBOL_YEARS = 152,                    /* YEARS  */
  YYSYMBOL_EQUAL = 153,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 154,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 155,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 156,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 157,                  /* BETWEEN  */
  YYSYMBOL_AND = 158,                      /* AND  */
  YYSYMBOL_OR = 159,                       /* OR  */
  YYSYMBOL_EXTRACT = 160,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 161,                     /* LIKE  */
  YYSYMBOL_DATA = 162,                     /* DATA  */
  YYSYMBOL_LOG = 163,                      /* LOG  */
  YYSYMBOL_BUFFER = 164,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 165,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 166,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 167,                 /* MEMINDEX  */
  YYSYMBOL_USING = 168,                    /* USING  */
  YYSYMBOL_SESSION = 169,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 170,                   /* GLOBAL  */
  YYSYMBOL_OFF = 171,                      /* OFF  */
  YYSYMBOL_EXPORT = 172,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 173,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 174,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 175,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 176,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 177,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 178,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 179,                    /* DELTA  */
  YYSYMBOL_LOGS = 180,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 181,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 182,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 183,                   /* SEARCH  */
  YYSYMBOL_MATCH = 184,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 185,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 186,                    /* QUERY  */
  YYSYMBOL_QUERIES = 187,                  /* QUERIES  */
  YYSYMBOL_FUSION = 188,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 189,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 190,                    /* ADMIN  */
  YYSYMBOL_LEADER = 191,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 192,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 193,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 194,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 195,               /* STANDALONE  */
  YYSYMBOL_NODES = 196,                    /* NODES  */
  YYSYMBOL_NODE = 197,                     /* NODE  */
  YYSYMBOL_PERSISTENCE = 198,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 199,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 200,                  /* OBJECTS  */
  YYSYMBOL_FILES = 201,                    /* FILES  */
  YYSYMBOL_MEMORY = 202,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 203,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 204,                   /* NUMBER  */
  YYSYMBOL_205_ = 205,                     /* '='  */
  YYSYMBOL_206_ = 206,                     /* '<'  */
  YYSYMBOL_207_ = 207,                     /* '>'  */
  YYSYMBOL_208_ = 208,                     /* '+'  */
  YYSYMBOL_209_ = 209,                     /* '-'  */
  YYSYMBOL_210_ = 210,                     /* '*'  */
  YYSYMBOL_211_ = 211,                     /* '/'  */
  YYSYMBOL_212_ = 212,                     /* '%'  */
  YYSYMBOL_213_ = 213,                     /* '['  */
  YYSYMBOL_214_ = 214,                     /* ']'  */
  YYSYMBOL_215_ = 215,                     /* '('  */
  YYSYMBOL_216_ = 216,                     /* ')'  */
  YYSYMBOL_217_ = 217,                     /* '.'  */
  YYSYMBOL_218_ = 218,                     /* ';'  */
  YYSYMBOL_219_ = 219,                     /* ','  */
  YYSYMBOL_220_ = 220,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 221,                 /* $accept  */
  YYSYMBOL_input_pattern = 222,            /* input_pattern  */
  YYSYMBOL_statement_list = 223,           /* statement_list  */
  YYSYMBOL_statement = 224,                /* statement  */
  YYSYMBOL_explainable_statement = 225,    /* explainable_statement  */
  YYSYMBOL_create_statement = 226,         /* create_statement  */
  YYSYMBOL_table_element_array = 227,      /* table_element_array  */
  YYSYMBOL_column_def_array = 228,         /* column_def_array  */
  YYSYMBOL_table_element = 229,            /* table_element  */
  YYSYMBOL_table_column = 230,             /* table_column  */
  YYSYMBOL_column_type = 231,              /* column_type  */
  YYSYMBOL_column_constraints = 232,       /* column_constraints  */
  YYSYMBOL_column_constraint = 233,        /* column_constraint  */
  YYSYMBOL_default_expr = 234,             /* default_expr  */
  YYSYMBOL_table_constraint = 235,         /* table_constraint  */
  YYSYMBOL_identifier_array = 236,         /* identifier_array  */
  YYSYMBOL_delete_statement = 237,         /* delete_statement  */
  YYSYMBOL_insert_statement = 238,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 239, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 240,        /* explain_statement  */
  YYSYMBOL_explain_type = 241,             /* explain_type  */
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
  YYSYMBOL_expr_array_list = 285,          /* expr_array_list  */
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
#define YYFINAL  108
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1358

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  221
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  513
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1145

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   459


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
       2,     2,     2,     2,     2,     2,     2,   212,     2,     2,
     215,   216,   210,   208,   219,   209,   217,   211,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   220,   218,
     206,   205,   207,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   213,     2,   214,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   507,   507,   511,   517,   524,   525,   526,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
     540,   541,   542,   543,   544,   545,   546,   547,   548,   549,
     550,   551,   558,   575,   591,   620,   636,   654,   683,   687,
     692,   696,   702,   705,   712,   763,   802,   803,   804,   805,
     806,   807,   808,   809,   810,   811,   812,   813,   814,   815,
     816,   817,   818,   819,   820,   821,   822,   825,   827,   828,
     829,   830,   833,   834,   835,   836,   837,   838,   839,   840,
     841,   842,   843,   844,   845,   846,   847,   848,   849,   850,
     851,   852,   853,   854,   855,   856,   857,   858,   859,   860,
     861,   862,   863,   864,   865,   866,   867,   868,   869,   870,
     871,   872,   873,   874,   875,   876,   877,   878,   879,   880,
     881,   882,   883,   884,   885,   886,   887,   888,   889,   890,
     891,   892,   893,   894,   895,   896,   897,   898,   917,   921,
     931,   934,   937,   940,   944,   947,   952,   957,   964,   970,
     980,   996,  1030,  1043,  1046,  1053,  1059,  1062,  1065,  1068,
    1071,  1074,  1077,  1080,  1087,  1100,  1104,  1109,  1122,  1135,
    1150,  1165,  1180,  1203,  1256,  1311,  1362,  1365,  1368,  1377,
    1387,  1390,  1394,  1399,  1426,  1429,  1434,  1450,  1453,  1457,
    1461,  1466,  1472,  1475,  1478,  1482,  1486,  1488,  1492,  1494,
    1497,  1501,  1504,  1508,  1513,  1517,  1520,  1524,  1527,  1531,
    1534,  1538,  1541,  1545,  1548,  1551,  1554,  1562,  1565,  1580,
    1580,  1582,  1596,  1605,  1610,  1619,  1624,  1629,  1635,  1642,
    1645,  1649,  1652,  1657,  1669,  1676,  1690,  1693,  1696,  1699,
    1702,  1705,  1708,  1714,  1718,  1722,  1726,  1730,  1737,  1741,
    1745,  1749,  1753,  1758,  1762,  1767,  1771,  1775,  1781,  1787,
    1793,  1804,  1815,  1826,  1838,  1850,  1863,  1877,  1888,  1902,
    1918,  1935,  1939,  1943,  1947,  1951,  1955,  1961,  1965,  1969,
    1977,  1981,  1985,  1993,  2004,  2027,  2033,  2038,  2044,  2050,
    2058,  2064,  2070,  2076,  2082,  2090,  2096,  2102,  2108,  2114,
    2122,  2128,  2134,  2143,  2153,  2166,  2170,  2175,  2181,  2188,
    2196,  2205,  2215,  2225,  2236,  2247,  2259,  2271,  2281,  2292,
    2304,  2317,  2321,  2326,  2331,  2337,  2341,  2345,  2351,  2355,
    2361,  2365,  2370,  2375,  2382,  2391,  2401,  2407,  2416,  2424,
    2429,  2442,  2446,  2451,  2455,  2488,  2494,  2498,  2499,  2500,
    2501,  2502,  2504,  2507,  2513,  2516,  2517,  2518,  2519,  2520,
    2521,  2522,  2523,  2524,  2525,  2529,  2547,  2593,  2632,  2675,
    2722,  2746,  2769,  2790,  2811,  2820,  2832,  2839,  2849,  2855,
    2867,  2870,  2873,  2876,  2879,  2882,  2886,  2890,  2895,  2903,
    2911,  2920,  2927,  2934,  2941,  2948,  2955,  2963,  2971,  2979,
    2987,  2995,  3003,  3011,  3019,  3027,  3035,  3043,  3051,  3081,
    3089,  3098,  3106,  3115,  3123,  3129,  3136,  3142,  3149,  3154,
    3161,  3168,  3176,  3203,  3209,  3215,  3222,  3230,  3237,  3244,
    3249,  3259,  3264,  3269,  3274,  3279,  3284,  3289,  3294,  3299,
    3304,  3307,  3310,  3314,  3317,  3320,  3323,  3327,  3330,  3333,
    3337,  3341,  3346,  3351,  3354,  3358,  3362,  3369,  3376,  3380,
    3387,  3394,  3398,  3402,  3406,  3409,  3413,  3417,  3422,  3427,
    3431,  3436,  3441,  3447,  3453,  3459,  3465,  3471,  3477,  3483,
    3489,  3495,  3501,  3507,  3518,  3522,  3527,  3558,  3568,  3573,
    3578,  3583,  3589,  3593,  3594,  3596,  3597,  3599,  3600,  3612,
    3620,  3624,  3627,  3631,  3634,  3638,  3642,  3647,  3653,  3663,
    3673,  3681,  3692,  3723
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
  "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES", "AST",
  "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "FRAGMENT", "VIEW", "INDEX",
  "ANALYZE", "VIEWS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK",
  "BLOCKS", "COLUMN", "COLUMNS", "INDEXES", "CHUNK", "GROUP", "BY",
  "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL",
  "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET",
  "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH",
  "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE", "ELSE", "THEN",
  "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT",
  "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE",
  "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP",
  "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE",
  "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT", "MULTIVECTOR",
  "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER",
  "FOLLOWER", "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE",
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "':'", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "column_def_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "default_expr", "table_constraint",
  "identifier_array", "delete_statement", "insert_statement",
  "optional_identifier_array", "explain_statement", "explain_type",
  "update_statement", "update_expr_array", "update_expr", "drop_statement",
  "copy_statement", "select_statement", "select_with_paren",
  "select_without_paren", "select_clause_with_modifier",
  "select_clause_without_modifier_paren", "select_clause_without_modifier",
  "order_by_clause", "order_by_expr_list", "order_by_expr",
  "order_by_type", "limit_expr", "offset_expr", "distinct", "from_clause",
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

#define YYPACT_NINF (-701)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-501)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      70,   292,    60,   319,   100,    68,   100,   180,   519,   700,
     161,   210,   215,   239,   208,   235,   261,   300,   122,    24,
     -48,   339,   172,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,   308,  -701,  -701,   348,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,   318,   318,   318,   318,    94,   100,   336,   336,
     336,   336,   336,   201,   410,   100,   -26,   433,   445,   469,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,   267,   471,   100,
    -701,  -701,  -701,  -701,  -701,   473,  -701,   -89,   -68,  -701,
     478,  -701,   303,  -701,  -701,   317,  -701,   179,  -129,   100,
     100,   100,   100,  -701,  -701,  -701,  -701,   -51,  -701,   429,
     273,  -701,   494,   111,   487,   506,   298,   311,  -701,   191,
    -701,   501,  -701,  -701,     8,   464,  -701,   475,   458,   543,
     100,   100,   100,   565,   502,   354,   495,   583,   100,   100,
     100,   587,   589,   593,   538,   608,   608,   551,    48,    61,
     143,  -701,  -701,  -701,  -701,  -701,  -701,  -701,   308,  -701,
    -701,  -701,  -701,  -701,  -701,   338,  -701,  -701,   614,  -701,
     621,  -701,  -701,   620,   630,  -701,  -701,  -701,  -701,    81,
    -701,  -701,  -701,   100,   422,   300,   608,  -701,   470,  -701,
     634,  -701,  -701,   643,  -701,  -701,   641,  -701,   645,   588,
    -701,  -701,  -701,  -701,     8,  -701,  -701,  -701,   551,   592,
     582,   580,  -701,   -47,  -701,   354,  -701,   100,   659,    27,
    -701,  -701,  -701,  -701,  -701,   598,  -701,   465,   -31,  -701,
     551,  -701,  -701,   590,   601,   456,  -701,  -701,   980,   639,
     457,   461,   405,   669,   673,   678,   682,  -701,  -701,   681,
     477,   158,   479,   480,   675,   675,  -701,    13,   515,   129,
    -701,    -3,   753,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,   476,  -701,  -701,  -701,
      39,  -701,  -701,    75,  -701,   115,  -701,  -701,  -701,   128,
    -701,   173,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,   685,   690,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,   644,   646,   648,
     163,   613,   348,  -701,  -701,   701,   205,  -701,   698,  -701,
     -57,   490,   491,   -32,   551,   551,   633,  -701,   -48,    29,
     649,   497,  -701,   204,   499,  -701,   100,   551,   593,  -701,
     164,   500,   505,   363,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,   675,   507,   776,   625,
     551,   551,   150,   269,  -701,  -701,  -701,  -701,   980,  -701,
     712,   511,   516,   517,   523,   713,   717,   249,   249,  -701,
     520,  -701,  -701,  -701,  -701,   525,   -83,   660,   551,   739,
     551,   551,   -39,   529,   -45,   675,   675,   675,   675,   675,
     675,   675,   675,   675,   675,   675,   675,   675,   675,    18,
    -701,   537,  -701,   746,  -701,   747,  -701,   749,  -701,   751,
     707,   279,   547,   755,   552,   765,   766,   555,  -701,   556,
    -701,   767,  -701,   222,   603,   606,  -701,  -701,    10,   597,
     557,  -701,    54,   164,   551,  -701,   308,   905,   642,   569,
     233,  -701,  -701,  -701,   -48,   782,  -701,  -701,   783,   551,
     574,  -701,   164,  -701,   230,   230,   551,  -701,   272,   625,
     636,   578,    35,   160,   306,  -701,   551,   551,   716,   551,
     796,    30,   551,   586,   278,   435,  -701,  -701,   608,  -701,
    -701,  -701,   650,   591,   675,   515,   666,  -701,   786,   786,
     322,   322,   763,   786,   786,   322,   322,   249,   249,  -701,
    -701,  -701,  -701,  -701,  -701,   596,  -701,   599,  -701,  -701,
    -701,   801,   803,  -701,   659,  -701,   755,   727,  -701,   808,
    -701,  -701,   806,  -701,   813,   816,   -48,   602,   518,  -701,
     103,  -701,   219,   538,   551,  -701,  -701,  -701,   164,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
     607,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,   609,   611,   615,   616,   618,   622,   209,
     623,   659,   797,    29,   308,   612,  -701,   280,   624,   830,
     834,   837,   840,  -701,   838,   288,  -701,   289,   290,  -701,
     629,  -701,   905,   551,  -701,   551,   108,   162,   675,   -98,
     627,  -701,   183,   -96,    65,   635,  -701,   846,  -701,  -701,
     769,   515,   786,   640,   320,  -701,   675,   851,   853,   805,
     814,   334,   358,  -701,   857,   667,   381,  -701,   865,  -701,
    -701,    15,    10,   810,  -701,  -701,  -701,  -701,  -701,  -701,
     815,  -701,   874,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,   663,   833,  -701,   886,   693,   941,   958,   975,   992,
    1009,   758,   761,  -701,  -701,   113,  -701,   759,   659,   382,
     680,  -701,  -701,   729,  -701,   551,  -701,  -701,  -701,  -701,
    -701,  -701,   230,  -701,  -701,  -701,   684,   164,   110,  -701,
     551,   221,   688,   899,   537,   691,   686,   551,  -701,   695,
     705,   697,   388,  -701,  -701,   776,   909,   917,  -701,  -701,
     755,  -701,   441,  -701,   808,   277,   103,   518,    10,    10,
     710,   219,   867,   868,   390,   708,   714,   715,   719,   724,
     725,   726,   730,   731,   839,   732,   733,   734,   735,   736,
     737,   738,   748,   757,   760,   849,   770,   771,   804,   807,
     811,   812,   822,   823,   826,   827,   869,   828,   829,   843,
     844,   845,   856,   860,   861,   862,   863,   880,   873,   877,
     878,   879,   890,   891,   894,   895,   896,   897,   881,   914,
     916,   918,   919,   920,   921,   922,   923,   924,   925,   883,
     926,  -701,  -701,    20,  -701,  -701,  -701,   392,  -701,   808,
     932,   400,  -701,  -701,  -701,   164,  -701,   462,   927,   928,
     929,    25,   930,  -701,  -701,  -701,   876,   800,   164,  -701,
     230,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  1018,  -701,  -701,  -701,   959,   659,  -701,   551,   551,
    -701,  -701,  1043,  1052,  1053,  1059,  1070,  1087,  1093,  1130,
    1144,  1145,   933,  1147,  1148,  1149,  1150,  1151,  1152,  1153,
    1154,  1155,  1156,   944,  1158,  1159,  1160,  1161,  1162,  1163,
    1164,  1165,  1166,  1167,   955,  1169,  1170,  1171,  1172,  1173,
    1174,  1175,  1176,  1177,  1178,   966,  1180,  1181,  1182,  1183,
    1184,  1185,  1186,  1187,  1188,  1189,   977,  1191,  1192,  1193,
    1194,  1195,  1196,  1197,  1198,  1199,  1200,   988,  1202,  -701,
    -701,   409,   613,  -701,  -701,  1205,   138,   994,  1207,  1208,
    -701,   416,  1209,   551,   417,   995,   164,   997,  1000,  1001,
    1002,  1003,  1004,  1005,  1006,  1007,  1008,  1219,  1010,  1011,
    1012,  1013,  1014,  1015,  1016,  1017,  1019,  1020,  1228,  1021,
    1022,  1023,  1024,  1025,  1026,  1027,  1028,  1029,  1030,  1241,
    1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,
    1252,  1044,  1045,  1046,  1047,  1048,  1049,  1050,  1051,  1054,
    1055,  1253,  1056,  1057,  1058,  1060,  1061,  1062,  1063,  1064,
    1065,  1066,  1262,  1067,  -701,  -701,  1068,   686,  -701,  1069,
    1071,  -701,   366,   164,  -701,  -701,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  1073,  -701,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  1075,  -701,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  1076,  -701,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  1077,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  1078,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,
    1079,  -701,  1265,  1080,  1271,    53,  1081,  1278,  1279,  -701,
    -701,  -701,  -701,  -701,  -701,  -701,  -701,  -701,  1082,  -701,
    1083,   686,   613,  1280,   463,    66,  1084,  1294,  1088,  -701,
     474,  1293,  -701,   686,   613,   686,   -29,  1297,  -701,  1250,
    1090,  -701,  1091,  1263,  1264,  -701,  -701,  -701,   -11,  -701,
    -701,  1096,  1266,  1267,  -701,  1305,  -701,  1099,  1100,  1312,
     613,  1101,  -701,   613,  -701
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     230,     0,     0,     0,     0,     0,     0,     0,   163,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     230,     0,   498,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   177,   176,     0,     8,    14,    15,    16,    17,
      18,    19,   496,   496,   496,   496,   496,     0,   494,   494,
     494,   494,   494,   223,     0,     0,     0,     0,     0,     0,
     157,   161,   158,   159,   160,   162,   156,   230,     0,     0,
     244,   245,   243,   249,   253,     0,   250,     0,     0,   246,
       0,   248,     0,   271,   273,     0,   251,     0,   277,     0,
       0,     0,     0,   280,   281,   282,   285,   223,   283,     0,
     229,   231,     0,     0,     0,     0,     0,     0,     1,   230,
       2,   213,   215,   216,     0,   200,   182,   188,     0,     0,
       0,     0,     0,     0,     0,   154,     0,     0,     0,     0,
       0,     0,     0,     0,   208,     0,     0,     0,     0,     0,
       0,   155,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,   259,   260,   254,   255,     0,   256,
       0,   247,   272,     0,     0,   275,   274,   278,   279,     0,
     304,   302,   303,     0,     0,     0,     0,   331,     0,   332,
       0,   325,   326,     0,   321,   305,     0,   328,   330,     0,
     181,   180,     4,   214,     0,   178,   179,   199,     0,     0,
     196,     0,    32,     0,    33,   154,   499,     0,     0,   230,
     493,   168,   170,   169,   171,     0,   224,     0,   208,   165,
       0,   150,   492,     0,     0,   427,   431,   434,   435,     0,
       0,     0,     0,     0,     0,     0,     0,   432,   433,     0,
       0,     0,     0,     0,     0,     0,   429,     0,   230,     0,
     341,   346,   347,   361,   359,   362,   360,   363,   364,   356,
     351,   350,   349,   357,   358,   348,   355,   354,   442,   444,
       0,   445,   453,     0,   454,     0,   446,   443,   464,     0,
     465,     0,   441,   289,   291,   290,   287,   288,   294,   296,
     295,   292,   293,   299,   301,   300,   297,   298,     0,     0,
     262,   261,   267,   257,   258,   252,   276,     0,     0,     0,
       0,   502,     0,   232,   286,     0,   322,   327,   306,   329,
       0,     0,     0,   202,     0,     0,   198,   495,   230,     0,
       0,     0,   148,     0,     0,   152,     0,     0,     0,   164,
     207,     0,     0,     0,   473,   472,   475,   474,   477,   476,
     479,   478,   481,   480,   483,   482,     0,     0,   393,   230,
       0,     0,     0,     0,   436,   437,   438,   439,     0,   440,
       0,     0,     0,     0,     0,     0,     0,   395,   394,   470,
     467,   461,   451,   456,   459,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     450,     0,   455,     0,   458,     0,   466,     0,   469,     0,
     268,   263,     0,     0,     0,     0,     0,     0,   284,     0,
     333,     0,   323,     0,     0,     0,   185,   184,     0,   204,
     187,   189,   194,   195,     0,   183,    35,     0,     0,     0,
       0,    38,    42,    43,   230,     0,    37,   153,     0,     0,
     151,   172,   167,   166,     0,     0,     0,   388,     0,   230,
       0,     0,     0,     0,     0,   418,     0,     0,     0,     0,
       0,     0,     0,   206,     0,     0,   353,   352,     0,   342,
     345,   411,   412,     0,     0,   230,     0,   392,   402,   403,
     406,   407,     0,   409,   401,   404,   405,   397,   396,   398,
     399,   400,   428,   430,   452,     0,   457,     0,   460,   468,
     471,     0,     0,   264,     0,   339,     0,     0,   336,     0,
     233,   324,     0,   307,     0,     0,   230,   201,   217,   219,
     228,   220,     0,   208,     0,   192,   193,   191,   197,    46,
      49,    50,    47,    48,    51,    52,    68,    53,    55,    54,
      71,    58,    59,    60,    56,    57,    61,    62,    63,    64,
      65,    66,    67,     0,     0,     0,     0,     0,     0,   502,
       0,     0,   504,     0,    36,     0,   149,     0,     0,     0,
       0,     0,     0,   488,     0,     0,   484,     0,     0,   389,
       0,   423,     0,     0,   416,     0,     0,     0,     0,     0,
       0,   427,     0,     0,     0,     0,   378,     0,   463,   462,
       0,   230,   410,     0,     0,   391,     0,     0,     0,   269,
     265,     0,     0,    40,     0,   507,     0,   505,   308,   334,
     335,     0,     0,     0,   237,   238,   239,   240,   236,   241,
       0,   226,     0,   221,   382,   380,   383,   381,   384,   385,
     386,   203,   212,   190,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,   145,   138,   145,     0,     0,
       0,    34,    39,   513,   343,     0,   490,   489,   487,   486,
     491,   175,     0,   173,   390,   424,     0,   420,     0,   419,
       0,     0,     0,     0,     0,     0,   206,     0,   376,     0,
       0,     0,     0,   425,   414,   413,     0,     0,   338,   337,
       0,   340,     0,   501,     0,     0,   228,   218,     0,     0,
     225,     0,     0,   210,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   143,   140,     0,   139,    45,    44,     0,   147,     0,
       0,     0,   485,   422,   417,   421,   408,     0,     0,   206,
       0,     0,     0,   447,   449,   448,     0,     0,   205,   379,
       0,   426,   415,   270,   266,    41,   508,   509,   511,   510,
     506,     0,   309,   222,   234,     0,     0,   387,     0,     0,
     186,    70,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   144,
     146,     0,   502,   344,   467,     0,     0,     0,     0,     0,
     377,     0,   310,     0,     0,   211,   209,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   503,   512,     0,   206,   374,     0,
     206,   174,     0,   235,   227,    69,    75,    76,    73,    74,
      77,    78,    79,    80,    81,     0,    72,   119,   120,   117,
     118,   121,   122,   123,   124,   125,     0,   116,    86,    87,
      84,    85,    88,    89,    90,    91,    92,     0,    83,    97,
      98,    95,    96,    99,   100,   101,   102,   103,     0,    94,
     130,   131,   128,   129,   132,   133,   134,   135,   136,     0,
     127,   108,   109,   106,   107,   110,   111,   112,   113,   114,
       0,   105,     0,     0,     0,     0,     0,     0,     0,   312,
     311,   317,    82,   126,    93,   104,   137,   115,   206,   375,
       0,   206,   502,   318,   313,     0,     0,     0,     0,   373,
       0,     0,   314,   206,   502,   206,   502,     0,   319,   315,
       0,   369,     0,     0,     0,   372,   320,   316,   502,   365,
     371,     0,     0,     0,   368,     0,   367,     0,     0,     0,
     502,     0,   370,   502,   366
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -701,  -701,  -701,  1210,  -701,  1251,  -701,  -701,   740,  -408,
     718,  -701,   647,   651,  -701,  -513,  1254,  1257,  1120,  -701,
    -701,  1259,  -701,   989,  1268,  1269,   -63,  1309,   -19,  1031,
    1136,   -70,  -701,  -701,   787,  -701,  -701,  -701,  -701,  -701,
    -701,  -700,  -208,  -701,  -701,  -701,  -701,   692,  -187,    23,
     619,  -701,  -701,  1157,  -701,  -701,  1270,  1272,  1273,  1274,
    1275,  -701,  -701,  -196,  -701,   945,  -220,  -222,  -522,  -497,
    -496,  -495,  -494,  -490,   617,  -701,  -701,  -701,  -701,  -701,
    -701,   976,  -701,  -701,   866,   541,  -242,  -701,  -701,  -701,
     652,  -701,  -701,  -701,  -701,   653,   936,   931,  -267,  -701,
    -701,  -701,  -701,  1111,  -457,   661,  -127,   414,   411,  -701,
    -701,  -576,  -701,   542,   628,  -701
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,   141,    24,   450,   632,   451,   452,
     579,   675,   676,   805,   453,   333,    25,    26,   209,    27,
      67,    28,   218,   219,    29,    30,    31,    32,    33,   116,
     195,   117,   200,   440,   441,   547,   326,   445,   198,   439,
     543,   615,   221,   850,   733,   114,   537,   538,   539,   540,
     653,    34,   100,   101,   541,   650,    35,    36,    37,    38,
      39,    40,    41,   249,   460,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   660,   661,   259,   260,   261,   262,
     263,   363,   264,   265,   266,   267,   268,   822,   269,   270,
     271,   272,   273,   274,   275,   276,   383,   384,   277,   278,
     279,   280,   281,   282,   595,   596,   223,   127,   119,   110,
     124,   428,   681,   636,   637,   456
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     340,   107,   323,   677,   148,   382,   827,   358,   597,   224,
     339,   631,   362,    53,   328,   525,   115,   173,   379,   380,
     654,   512,   377,   378,   226,   227,   228,    54,   386,    56,
     379,   380,   447,   611,   496,   111,    17,   112,   103,    98,
     104,   220,   493,   113,   196,   655,   656,   657,   658,   314,
     438,   283,   659,   284,   285,   427,   135,   136,   389,  1101,
     390,   391,   390,   391,   288,   334,   289,   290,   679,   706,
     125,   167,  1113,   427,   168,   390,   391,     1,   134,     2,
       3,     4,     5,     6,     7,     8,     9,    10,   157,   158,
     497,   307,   155,    11,    12,    13,   602,    47,   308,    14,
      15,    16,  1123,    53,   442,   443,   651,   309,   310,   159,
     160,    17,   169,   170,   171,   172,   286,   462,   633,   927,
    1132,   702,   494,   705,   322,   707,   233,   234,   235,   291,
     545,   546,   236,   487,   358,   434,   435,   707,   707,  1124,
     472,   473,  1007,   203,   204,   205,   293,   468,   294,   295,
      55,   212,   213,   214,    17,   390,   391,  1133,   237,   238,
     239,   448,  -500,   449,   652,   807,   132,    20,   329,   514,
     491,   492,   118,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   388,   338,   137,
     335,  -497,   671,   390,   391,    89,   311,   699,     1,   814,
       2,     3,     4,     5,     6,     7,     8,     9,    10,   654,
     707,   296,   390,   391,    11,    12,    13,   425,   105,   287,
      14,    15,    16,   194,   548,   536,   247,   381,   513,   385,
     331,   726,   292,   247,   655,   656,   657,   658,    96,   381,
     246,   659,    18,   361,    90,   672,   426,   673,   674,    91,
     803,   431,   603,   410,   700,   532,   606,   607,   411,   609,
      19,   432,   613,   587,    97,   446,   390,   391,   390,   391,
     598,   533,   622,    92,     1,    17,     2,     3,     4,     5,
       6,     7,   371,     9,   372,    20,   373,   374,   671,   412,
      11,    12,    13,   427,   413,   102,    14,    15,    16,   624,
     470,   177,   178,    99,   589,   590,   179,  1083,   390,   391,
    1086,   841,   835,   842,   297,   591,   592,   593,   390,   391,
     390,   391,   390,   391,   442,    42,    43,    44,   111,   414,
     112,   522,   523,   934,   415,   662,   113,    45,    46,   108,
     471,   672,   416,   673,   674,   387,  1005,   417,   388,    57,
      58,    17,    48,    49,    50,    59,   115,   394,   475,   461,
     476,   620,   477,    18,    51,    52,   225,   226,   227,   228,
      93,    94,    95,   931,   395,   396,   397,   398,   164,   165,
     166,    19,   400,   697,   298,   698,   701,   418,   299,   300,
     109,   584,   419,   301,   302,   604,   118,   605,  1106,   477,
     409,  1108,   704,   241,   715,   242,    20,   243,   225,   226,
     227,   228,  1087,  1120,   126,  1122,  1088,  1089,   132,   594,
     457,  1090,  1091,   458,   133,   712,   401,   402,   403,   404,
     405,   406,   407,   408,   466,   818,   138,   816,   825,    18,
     618,   619,   229,   230,   836,   837,   838,   839,   139,   582,
     600,   231,   583,   232,   120,   121,   122,   123,   394,   406,
     407,   408,   820,   128,   129,   130,   131,   379,   924,   233,
     234,   235,   140,   641,   154,   236,   623,  -501,  -501,   156,
     815,   161,    20,   162,   229,   230,   163,   828,   599,   811,
     174,   388,   175,   231,   616,   232,   684,   617,   361,   388,
     176,   237,   238,   239,   691,   693,   694,   692,   692,   388,
     189,   233,   234,   235,   190,  1111,  1112,   236,   225,   226,
     227,   228,   193,   240,  1117,  1118,  1109,   191,  -501,  -501,
     404,   405,   406,   407,   408,   197,   714,   201,  1121,   388,
    1125,   844,   845,   237,   238,   239,   202,   241,   199,   242,
     718,   243,  1134,   458,   225,   226,   227,   228,    60,    61,
      62,    63,    64,    65,  1142,   240,    66,  1144,   206,   208,
     207,   244,   245,   246,   719,   210,   247,   720,   248,   467,
     643,  -242,   644,   645,   646,   647,   211,   648,   649,   241,
     215,   242,   216,   243,   229,   230,   217,   723,   808,    17,
     724,   458,   711,   231,   832,   232,   851,   388,   920,   852,
     220,   458,   222,   244,   245,   246,   923,   303,   247,   388,
     248,   233,   234,   235,   304,  1004,   305,   236,   724,   936,
     229,   230,  1011,  1014,   306,   692,   458,   312,   315,   231,
     316,   232,   225,   226,   227,   228,   317,   318,   319,   320,
     180,   324,   935,   237,   238,   239,   325,   233,   234,   235,
     327,   181,   332,   236,   182,   183,   336,   184,   185,   186,
     337,   343,   359,   364,   341,   240,   360,   365,   225,   226,
     227,   228,   366,   187,   188,   342,   367,   368,   420,   237,
     238,   239,   370,   409,   375,   376,   421,   427,   422,   241,
     423,   242,   424,   243,   433,   430,   436,   437,   444,    17,
     454,   240,   455,  1013,   459,   464,   478,   483,   356,   357,
     465,   484,   469,   244,   245,   246,   479,   231,   247,   232,
     248,   480,   481,    68,    69,   241,    70,   242,   482,   243,
     485,   486,   490,   488,   495,   233,   234,   235,    71,    72,
     247,   236,   515,   517,   356,   519,   520,   521,   447,   244,
     245,   246,   524,   231,   247,   232,   248,   526,   527,   528,
     529,   534,   530,   531,   535,   580,   544,   237,   238,   239,
     542,   233,   234,   235,   581,   585,   586,   236,   735,   736,
     737,   738,   739,   588,   601,   740,   741,   494,   608,   240,
     610,   625,   742,   743,   744,   614,   621,   629,   390,   630,
     634,   635,   638,   237,   238,   239,   627,   639,   745,   628,
     640,   642,   664,   241,   665,   242,   666,   243,   683,   680,
     667,   668,   392,   669,   393,   240,   686,   670,   678,   685,
     687,   688,   470,   689,   690,   695,   703,   244,   245,   246,
     709,   708,   247,   710,   248,   470,   713,   619,   618,   241,
     721,   242,   716,   243,    73,    74,    75,    76,   717,    77,
      78,   725,   722,   728,    79,    80,    81,   730,   729,    82,
      83,    84,   731,   244,   245,   246,    85,    86,   247,   394,
     248,   732,   734,   801,   802,   809,   803,   810,    87,   394,
     813,   817,    88,   819,   821,   826,   395,   396,   397,   398,
     399,   829,   394,   831,   400,   833,   395,   396,   397,   398,
     830,   626,   394,   834,   400,   846,   848,   853,   849,   395,
     396,   397,   398,   854,   855,   922,   862,   400,   856,  -501,
    -501,   397,   398,   857,   858,   859,   873,  -501,   707,   860,
     861,   863,   864,   865,   866,   867,   868,   869,   401,   402,
     403,   404,   405,   406,   407,   408,   884,   870,   401,   402,
     403,   404,   405,   406,   407,   408,   871,   895,   906,   872,
     917,   401,   402,   403,   404,   405,   406,   407,   408,   874,
     875,  -501,   402,   403,   404,   405,   406,   407,   408,   549,
     550,   551,   552,   553,   554,   555,   556,   557,   558,   559,
     560,   561,   562,   563,   564,   565,   930,   566,   567,   568,
     569,   570,   571,   876,   932,   572,   877,   933,   573,   574,
     878,   879,   575,   576,   577,   578,   746,   747,   748,   749,
     750,   880,   881,   751,   752,   882,   883,   885,   886,   937,
     753,   754,   755,   757,   758,   759,   760,   761,   938,   939,
     762,   763,   887,   888,   889,   940,   756,   764,   765,   766,
     768,   769,   770,   771,   772,   890,   941,   773,   774,   891,
     892,   893,   894,   767,   775,   776,   777,   779,   780,   781,
     782,   783,   896,   942,   784,   785,   897,   898,   899,   943,
     778,   786,   787,   788,   790,   791,   792,   793,   794,   900,
     901,   795,   796,   902,   903,   904,   905,   789,   797,   798,
     799,   344,   345,   346,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   907,   800,   908,   944,   909,   910,   911,
     912,   913,   914,   915,   916,   918,   925,   926,   928,   929,
     945,   946,   947,   948,   949,   950,   951,   952,   953,   954,
     955,   956,   957,   958,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   969,   970,   971,   972,   973,   974,
     975,   976,   977,   978,   979,   980,   981,   982,   983,   984,
     985,   986,   987,   988,   989,   990,   991,   992,   993,   994,
     995,   996,   997,   998,   999,  1000,  1001,  1002,  1003,  1006,
    1008,  1009,  1010,  1015,   388,  1012,  1016,  1017,  1018,  1019,
    1020,  1021,  1022,  1023,  1024,  1025,  1026,  1027,  1028,  1029,
    1030,  1031,  1032,  1033,  1036,  1034,  1035,  1037,  1038,  1039,
    1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,  1069,
    1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1080,  1098,
    1067,  1068,  1070,  1071,  1072,  1100,  1073,  1074,  1075,  1076,
    1077,  1078,  1079,  1081,  1103,  1104,  1110,  1082,  1084,  1092,
    1085,  1093,  1094,  1095,  1096,  1097,  1099,  1102,  1115,  1119,
    1114,  1105,  1107,  1126,  1116,  1127,  1128,  1129,  1138,  1130,
    1131,  1135,  1136,  1137,  1139,  1141,  1140,  1143,   142,   192,
     696,   143,   804,   682,   144,   330,   145,   463,   806,   106,
     321,   663,   313,   489,   727,   146,   147,   149,   474,   150,
     151,   152,   153,   429,   919,   843,   518,   612,   847,   516,
     369,   921,   840,   812,     0,     0,     0,   823,   824
};

static const yytype_int16 yycheck[] =
{
     220,    20,   198,   579,    67,   247,   706,   229,   465,   136,
     218,   524,   232,     3,    61,   423,     8,    68,     5,     6,
     542,     3,   244,   245,     4,     5,     6,     4,   248,     6,
       5,     6,     3,     3,    79,    20,    84,    22,    14,    16,
      16,    72,    81,    28,   114,   542,   542,   542,   542,   176,
      82,     3,   542,     5,     6,    84,    82,    83,    61,     6,
     158,   159,   158,   159,     3,    38,     5,     6,   581,     4,
      47,   200,     6,    84,   203,   158,   159,     7,    55,     9,
      10,    11,    12,    13,    14,    15,    16,    17,   177,   178,
     135,    10,    69,    23,    24,    25,    61,    37,    17,    29,
      30,    31,   131,     3,   324,   325,     3,    26,    27,   177,
     178,    84,    89,    90,    91,    92,    68,   337,   526,   819,
     131,   219,   161,   219,   194,    72,   106,   107,   108,    68,
      76,    77,   112,   216,   356,   192,   193,    72,    72,   168,
     360,   361,     4,   120,   121,   122,     3,   343,     5,     6,
      82,   128,   129,   130,    84,   158,   159,   168,   138,   139,
     140,   132,    68,   134,    61,   678,   217,   215,   215,   411,
     390,   391,    78,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,   219,   219,   215,
     209,     0,    79,   158,   159,    34,   173,    89,     7,    89,
       9,    10,    11,    12,    13,    14,    15,    16,    17,   731,
      72,    68,   158,   159,    23,    24,    25,    54,   194,   171,
      29,    30,    31,   215,   444,   215,   213,   214,   210,   248,
     207,   216,   171,   213,   731,   731,   731,   731,     3,   214,
     210,   731,   172,    93,    34,   132,    83,   134,   135,    34,
     137,    46,    92,   214,    92,    33,   476,   477,   219,   479,
     190,    56,   482,   459,     3,   328,   158,   159,   158,   159,
     466,    49,   494,    34,     7,    84,     9,    10,    11,    12,
      13,    14,   124,    16,   126,   215,   128,   129,    79,   214,
      23,    24,    25,    84,   219,   173,    29,    30,    31,   495,
      79,   190,   191,     3,    74,    75,   195,  1007,   158,   159,
    1010,    34,   720,    36,   171,    85,    86,    87,   158,   159,
     158,   159,   158,   159,   544,    33,    34,    35,    20,   214,
      22,    52,    53,   846,   219,   543,    28,    45,    46,     0,
     359,   132,   214,   134,   135,   216,   922,   219,   219,   169,
     170,    84,    33,    34,    35,   175,     8,   136,    89,   336,
      91,   488,    93,   172,    45,    46,     3,     4,     5,     6,
     162,   163,   164,   830,   153,   154,   155,   156,   199,   200,
     201,   190,   161,   603,    46,   605,   608,   214,    50,    51,
     218,   454,   219,    55,    56,    89,    78,    91,  1098,    93,
     217,  1101,   219,   184,   626,   186,   215,   188,     3,     4,
       5,     6,    46,  1113,    78,  1115,    50,    51,   217,   189,
     216,    55,    56,   219,    14,   621,   205,   206,   207,   208,
     209,   210,   211,   212,    71,   702,     3,   216,   705,   172,
       5,     6,    79,    80,     3,     4,     5,     6,     3,   216,
     469,    88,   219,    90,    43,    44,    45,    46,   136,   210,
     211,   212,   704,    49,    50,    51,    52,     5,     6,   106,
     107,   108,     3,   536,     3,   112,   495,   155,   156,     6,
     700,     3,   215,   180,    79,    80,   169,   707,   216,   685,
      61,   219,   219,    88,   216,    90,   216,   219,    93,   219,
       6,   138,   139,   140,   216,   216,   216,   219,   219,   219,
       4,   106,   107,   108,   216,    52,    53,   112,     3,     4,
       5,     6,    21,   160,    50,    51,  1102,   216,   206,   207,
     208,   209,   210,   211,   212,    71,   216,    79,  1114,   219,
    1116,   728,   729,   138,   139,   140,     3,   184,    73,   186,
     216,   188,  1128,   219,     3,     4,     5,     6,    39,    40,
      41,    42,    43,    44,  1140,   160,    47,  1143,     3,   215,
      68,   208,   209,   210,   216,    80,   213,   219,   215,   216,
      62,    63,    64,    65,    66,    67,     3,    69,    70,   184,
       3,   186,     3,   188,    79,    80,     3,   216,   216,    84,
     219,   219,   621,    88,   216,    90,   216,   219,   216,   219,
      72,   219,     4,   208,   209,   210,   216,     3,   213,   219,
     215,   106,   107,   108,     3,   216,     6,   112,   219,   849,
      79,    80,   216,   216,     4,   219,   219,   215,   168,    88,
       6,    90,     3,     4,     5,     6,     3,     6,     3,    61,
     163,    59,   848,   138,   139,   140,    74,   106,   107,   108,
      80,   174,     3,   112,   177,   178,    68,   180,   181,   182,
     205,   215,   215,     4,    84,   160,   215,     4,     3,     4,
       5,     6,     4,   196,   197,    84,     4,     6,     3,   138,
     139,   140,   215,   217,   215,   215,     6,    84,    54,   184,
      54,   186,    54,   188,     6,     4,   216,   216,    75,    84,
      61,   160,   215,   933,   215,   215,     4,     4,    79,    80,
     215,     4,   215,   208,   209,   210,   215,    88,   213,    90,
     215,   215,   215,    33,    34,   184,    36,   186,   215,   188,
     220,   216,     3,    83,   215,   106,   107,   108,    48,    49,
     213,   112,     6,     6,    79,     6,     5,    50,     3,   208,
     209,   210,   215,    88,   213,    90,   215,   215,     3,     3,
     215,   168,   216,     6,   168,   133,   219,   138,   139,   140,
     183,   106,   107,   108,   215,     3,     3,   112,    95,    96,
      97,    98,    99,   219,   216,   102,   103,   161,    82,   160,
       4,   135,   109,   110,   111,   219,   215,     6,   158,     6,
      83,     3,     6,   138,   139,   140,   220,     4,   125,   220,
       4,   219,   215,   184,   215,   186,   215,   188,   216,    32,
     215,   215,    79,   215,    81,   160,     6,   215,   215,   215,
       6,     4,    79,     3,     6,   216,   219,   208,   209,   210,
       4,   216,   213,    84,   215,    79,   216,     6,     5,   184,
       3,   186,    57,   188,   164,   165,   166,   167,    54,   169,
     170,     6,   205,    63,   174,   175,   176,     3,    63,   179,
     180,   181,   219,   208,   209,   210,   186,   187,   213,   136,
     215,    58,     6,   135,   133,   215,   137,   168,   198,   136,
     216,   213,   202,     4,   213,   219,   153,   154,   155,   156,
     157,   216,   136,   216,   161,     6,   153,   154,   155,   156,
     215,   158,   136,     6,   161,   215,    59,   219,    60,   153,
     154,   155,   156,   219,   219,     3,    97,   161,   219,   153,
     154,   155,   156,   219,   219,   219,    97,   161,    72,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   205,   206,
     207,   208,   209,   210,   211,   212,    97,   219,   205,   206,
     207,   208,   209,   210,   211,   212,   219,    97,    97,   219,
      97,   205,   206,   207,   208,   209,   210,   211,   212,   219,
     219,   205,   206,   207,   208,   209,   210,   211,   212,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   216,   112,   113,   114,
     115,   116,   117,   219,     6,   120,   219,    68,   123,   124,
     219,   219,   127,   128,   129,   130,    95,    96,    97,    98,
      99,   219,   219,   102,   103,   219,   219,   219,   219,     6,
     109,   110,   111,    95,    96,    97,    98,    99,     6,     6,
     102,   103,   219,   219,   219,     6,   125,   109,   110,   111,
      95,    96,    97,    98,    99,   219,     6,   102,   103,   219,
     219,   219,   219,   125,   109,   110,   111,    95,    96,    97,
      98,    99,   219,     6,   102,   103,   219,   219,   219,     6,
     125,   109,   110,   111,    95,    96,    97,    98,    99,   219,
     219,   102,   103,   219,   219,   219,   219,   125,   109,   110,
     111,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   219,   125,   219,     6,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
       6,     6,   219,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   219,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   219,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   219,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   219,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   219,     6,     4,
     216,     4,     4,   216,   219,     6,   216,   216,   216,   216,
     216,   216,   216,   216,   216,     6,   216,   216,   216,   216,
     216,   216,   216,   216,     6,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,     6,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,     6,     6,
     216,   216,   216,   216,   216,   216,   216,   216,     6,     4,
     216,   216,   216,   216,   216,     4,   216,   216,   216,   216,
     216,   216,   216,   216,     6,     6,     6,   219,   219,   216,
     219,   216,   216,   216,   216,   216,   216,   216,     4,     6,
     216,   219,   219,     6,   216,    55,   216,   216,     3,    46,
      46,   215,    46,    46,   215,     3,   216,   216,    67,   109,
     602,    67,   675,   583,    67,   205,    67,   338,   677,    20,
     194,   544,   175,   388,   642,    67,    67,    67,   362,    67,
      67,    67,    67,   312,   803,   726,   415,   481,   731,   413,
     239,   809,   724,   692,    -1,    -1,    -1,   705,   705
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    84,   172,   190,
     215,   222,   223,   224,   226,   237,   238,   240,   242,   245,
     246,   247,   248,   249,   272,   277,   278,   279,   280,   281,
     282,   283,    33,    34,    35,    45,    46,    37,    33,    34,
      35,    45,    46,     3,   270,    82,   270,   169,   170,   175,
      39,    40,    41,    42,    43,    44,    47,   241,    33,    34,
      36,    48,    49,   164,   165,   166,   167,   169,   170,   174,
     175,   176,   179,   180,   181,   186,   187,   198,   202,    34,
      34,    34,    34,   162,   163,   164,     3,     3,   270,     3,
     273,   274,   173,    14,    16,   194,   248,   249,     0,   218,
     330,    20,    22,    28,   266,     8,   250,   252,    78,   329,
     329,   329,   329,   329,   331,   270,    78,   328,   328,   328,
     328,   328,   217,    14,   270,    82,    83,   215,     3,     3,
       3,   225,   226,   237,   238,   242,   245,   246,   247,   277,
     278,   279,   280,   281,     3,   270,     6,   177,   178,   177,
     178,     3,   180,   169,   199,   200,   201,   200,   203,   270,
     270,   270,   270,    68,    61,   219,     6,   190,   191,   195,
     163,   174,   177,   178,   180,   181,   182,   196,   197,     4,
     216,   216,   224,    21,   215,   251,   252,    71,   259,    73,
     253,    79,     3,   270,   270,   270,     3,    68,   215,   239,
      80,     3,   270,   270,   270,     3,     3,     3,   243,   244,
      72,   263,     4,   327,   327,     3,     4,     5,     6,    79,
      80,    88,    90,   106,   107,   108,   112,   138,   139,   140,
     160,   184,   186,   188,   208,   209,   210,   213,   215,   284,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   297,
     298,   299,   300,   301,   303,   304,   305,   306,   307,   309,
     310,   311,   312,   313,   314,   315,   316,   319,   320,   321,
     322,   323,   324,     3,     5,     6,    68,   171,     3,     5,
       6,    68,   171,     3,     5,     6,    68,   171,    46,    50,
      51,    55,    56,     3,     3,     6,     4,    10,    17,    26,
      27,   270,   215,   274,   327,   168,     6,     3,     6,     3,
      61,   251,   252,   284,    59,    74,   257,    80,    61,   215,
     239,   270,     3,   236,    38,   249,    68,   205,   219,   263,
     287,    84,    84,   215,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,    79,    80,   288,   215,
     215,    93,   287,   302,     4,     4,     4,     4,     6,   324,
     215,   124,   126,   128,   129,   215,   215,   288,   288,     5,
       6,   214,   307,   317,   318,   249,   287,   216,   219,    61,
     158,   159,    79,    81,   136,   153,   154,   155,   156,   157,
     161,   205,   206,   207,   208,   209,   210,   211,   212,   217,
     214,   219,   214,   219,   214,   219,   214,   219,   214,   219,
       3,     6,    54,    54,    54,    54,    83,    84,   332,   250,
       4,    46,    56,     6,   192,   193,   216,   216,    82,   260,
     254,   255,   287,   287,    75,   258,   247,     3,   132,   134,
     227,   229,   230,   235,    61,   215,   336,   216,   219,   215,
     285,   270,   287,   244,   215,   215,    71,   216,   284,   215,
      79,   249,   287,   287,   302,    89,    91,    93,     4,   215,
     215,   215,   215,     4,     4,   220,   216,   216,    83,   286,
       3,   287,   287,    81,   161,   215,    79,   135,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,     3,   210,   307,     6,   317,     6,   318,     6,
       5,    50,    52,    53,   215,   230,   215,     3,     3,   215,
     216,     6,    33,    49,   168,   168,   215,   267,   268,   269,
     270,   275,   183,   261,   219,    76,    77,   256,   287,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   112,   113,   114,   115,
     116,   117,   120,   123,   124,   127,   128,   129,   130,   231,
     133,   215,   216,   219,   247,     3,     3,   284,   219,    74,
      75,    85,    86,    87,   189,   325,   326,   325,   284,   216,
     249,   216,    61,    92,    89,    91,   287,   287,    82,   287,
       4,     3,   305,   287,   219,   262,   216,   219,     5,     6,
     327,   215,   288,   249,   284,   135,   158,   220,   220,     6,
       6,   236,   228,   230,    83,     3,   334,   335,     6,     4,
       4,   247,   219,    62,    64,    65,    66,    67,    69,    70,
     276,     3,    61,   271,   289,   290,   291,   292,   293,   294,
     295,   296,   263,   255,   215,   215,   215,   215,   215,   215,
     215,    79,   132,   134,   135,   232,   233,   332,   215,   236,
      32,   333,   229,   216,   216,   215,     6,     6,     4,     3,
       6,   216,   219,   216,   216,   216,   231,   287,   287,    89,
      92,   288,   219,   219,   219,   219,     4,    72,   216,     4,
      84,   249,   284,   216,   216,   288,    57,    54,   216,   216,
     219,     3,   205,   216,   219,     6,   216,   268,    63,    63,
       3,   219,    58,   265,     6,    95,    96,    97,    98,    99,
     102,   103,   109,   110,   111,   125,    95,    96,    97,    98,
      99,   102,   103,   109,   110,   111,   125,    95,    96,    97,
      98,    99,   102,   103,   109,   110,   111,   125,    95,    96,
      97,    98,    99,   102,   103,   109,   110,   111,   125,    95,
      96,    97,    98,    99,   102,   103,   109,   110,   111,   125,
      95,    96,    97,    98,    99,   102,   103,   109,   110,   111,
     125,   135,   133,   137,   233,   234,   234,   236,   216,   215,
     168,   284,   326,   216,    89,   287,   216,   213,   319,     4,
     307,   213,   308,   311,   316,   319,   219,   262,   287,   216,
     215,   216,   216,     6,     6,   230,     3,     4,     5,     6,
     335,    34,    36,   271,   269,   269,   215,   295,    59,    60,
     264,   216,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,    97,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,    97,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,    97,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,    97,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,    97,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,    97,   219,   306,
     216,   334,     3,   216,     6,   219,   219,   262,   219,   219,
     216,   325,     6,    68,   236,   284,   287,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   219,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   219,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   219,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     219,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   219,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   219,     6,   216,   332,     4,     4,   216,     4,
       4,   216,     6,   287,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,     6,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,     6,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,     6,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,     6,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,     6,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
       6,   216,   219,   262,   219,   219,   262,    46,    50,    51,
      55,    56,   216,   216,   216,   216,   216,   216,     4,   216,
       4,     6,   216,     6,     6,   219,   262,   219,   262,   332,
       6,    52,    53,     6,   216,     4,   216,    50,    51,     6,
     262,   332,   262,   131,   168,   332,     6,    55,   216,   216,
      46,    46,   131,   168,   332,   215,    46,    46,     3,   215,
     216,     3,   332,   216,   332
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   221,   222,   223,   223,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   226,   226,   226,   226,   226,   226,   227,   227,
     228,   228,   229,   229,   230,   230,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   232,   232,
     233,   233,   233,   233,   234,   234,   235,   235,   236,   236,
     237,   238,   238,   239,   239,   240,   241,   241,   241,   241,
     241,   241,   241,   241,   242,   243,   243,   244,   245,   245,
     245,   245,   245,   246,   246,   246,   247,   247,   247,   247,
     248,   248,   249,   250,   251,   251,   252,   253,   253,   254,
     254,   255,   256,   256,   256,   257,   257,   258,   258,   259,
     259,   260,   260,   261,   261,   262,   262,   263,   263,   264,
     264,   265,   265,   266,   266,   266,   266,   267,   267,   268,
     268,   269,   269,   270,   270,   271,   271,   271,   271,   272,
     272,   273,   273,   274,   275,   275,   276,   276,   276,   276,
     276,   276,   276,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     277,   277,   277,   277,   277,   277,   277,   277,   277,   277,
     278,   278,   278,   279,   279,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   281,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   283,   283,   283,   283,
     283,   284,   284,   285,   285,   286,   286,   287,   287,   287,
     287,   287,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   289,   290,   290,   290,   290,
     291,   291,   291,   291,   292,   292,   293,   293,   294,   294,
     295,   295,   295,   295,   295,   295,   296,   296,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   298,   298,   299,   300,   300,   301,   301,   301,   301,
     302,   302,   303,   304,   304,   304,   304,   305,   305,   305,
     305,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   306,   306,   307,   307,   307,   307,   308,   308,   308,
     309,   310,   310,   311,   311,   312,   313,   313,   314,   315,
     315,   316,   317,   318,   319,   319,   320,   321,   321,   322,
     323,   323,   324,   324,   324,   324,   324,   324,   324,   324,
     324,   324,   324,   324,   325,   325,   326,   326,   326,   326,
     326,   326,   327,   328,   328,   329,   329,   330,   330,   331,
     331,   332,   332,   333,   333,   334,   334,   335,   335,   335,
     335,   335,   336,   336
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     8,     6,     7,     6,     1,     3,
       1,     3,     1,     1,     4,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     6,
       4,     1,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     1,     2,
       2,     1,     1,     2,     2,     0,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     1,     3,     3,     4,     4,
       4,     4,     6,     8,    11,     8,     1,     1,     3,     3,
       3,     3,     2,     4,     3,     3,     8,     3,     0,     1,
       3,     2,     1,     1,     0,     2,     0,     2,     0,     1,
       0,     2,     0,     2,     0,     3,     0,     2,     0,     2,
       0,     3,     0,     1,     2,     1,     1,     1,     3,     1,
       1,     2,     4,     1,     3,     2,     1,     5,     0,     2,
       0,     1,     3,     5,     4,     6,     1,     1,     1,     1,
       1,     1,     0,     2,     2,     2,     2,     3,     2,     2,
       2,     2,     4,     2,     3,     3,     3,     4,     4,     3,
       3,     4,     4,     5,     6,     7,     9,     4,     5,     7,
       9,     2,     3,     2,     3,     3,     4,     2,     3,     3,
       2,     2,     2,     2,     5,     2,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     3,     3,     3,     3,     4,     6,     7,     9,
      10,    12,    12,    13,    14,    15,    16,    12,    13,    15,
      16,     3,     4,     5,     6,     3,     3,     4,     3,     4,
       3,     3,     3,     5,     7,     7,     6,     8,     8,     6,
       8,     1,     3,     3,     5,     3,     1,     1,     1,     1,
       1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,    14,    20,    16,    15,    13,
      18,    14,    13,    11,     8,    10,     5,     7,     4,     6,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     4,
       5,     4,     3,     2,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     3,
       4,     3,     3,     5,     5,     6,     4,     6,     3,     5,
       4,     5,     6,     4,     5,     5,     6,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     2,     3,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     3,     3,     1,     1,     2,     2,     3,     2,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     2,
       2,     2,     1,     2,     0,     3,     0,     1,     0,     2,
       0,     4,     0,     4,     0,     1,     3,     1,     3,     3,
       3,     3,     6,     3
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
#line 2382 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2390 "parser.cpp"
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
#line 2404 "parser.cpp"
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
#line 2418 "parser.cpp"
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
#line 2432 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2443 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2451 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2460 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2469 "parser.cpp"
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
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2494 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2504 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2514 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2524 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2534 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2544 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2554 "parser.cpp"
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
#line 2568 "parser.cpp"
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
#line 2582 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2592 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2617 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2625 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2641 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2649 "parser.cpp"
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
#line 2663 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2672 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2681 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2690 "parser.cpp"
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
#line 2703 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2712 "parser.cpp"
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
#line 2726 "parser.cpp"
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
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2750 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2759 "parser.cpp"
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
#line 2773 "parser.cpp"
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
#line 2790 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2798 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2814 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2830 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2838 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2846 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2854 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2862 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2870 "parser.cpp"
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
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2892 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2900 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2908 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2916 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2924 "parser.cpp"
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
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3009 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3017 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3025 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3033 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3041 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3049 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3057 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3065 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3073 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3081 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3089 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3097 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3105 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3113 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3121 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3132 "parser.cpp"
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
#line 3146 "parser.cpp"
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
#line 3160 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3171 "parser.cpp"
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

#line 3279 "parser.cpp"

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
#line 507 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3494 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 511 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3505 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 517 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3516 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 524 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3522 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3528 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3534 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3540 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3546 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3552 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3558 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3564 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3570 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3576 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 534 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3582 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3588 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3594 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3600 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 538 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3606 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 540 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3612 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3618 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3624 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 543 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3630 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3636 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3642 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3648 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 547 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3654 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3660 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 549 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3666 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3672 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 551 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3678 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 558 "parser.y"
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
#line 3698 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 575 "parser.y"
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
#line 3716 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 591 "parser.y"
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
#line 3749 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 620 "parser.y"
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
#line 3769 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 636 "parser.y"
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
#line 3790 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 654 "parser.y"
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
#line 3823 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 683 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3832 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 687 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3841 "parser.cpp"
    break;

  case 40: /* column_def_array: table_column  */
#line 692 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3850 "parser.cpp"
    break;

  case 41: /* column_def_array: column_def_array ',' table_column  */
#line 696 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3859 "parser.cpp"
    break;

  case 42: /* table_element: table_column  */
#line 702 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3867 "parser.cpp"
    break;

  case 43: /* table_element: table_constraint  */
#line 705 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3875 "parser.cpp"
    break;

  case 44: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 712 "parser.y"
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
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, std::move(default_expr));

    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3931 "parser.cpp"
    break;

  case 45: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 763 "parser.y"
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
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-2].column_type_t).logical_type_, type_info_ptr, default_expr);

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
#line 3973 "parser.cpp"
    break;

  case 46: /* column_type: BOOLEAN  */
#line 802 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3979 "parser.cpp"
    break;

  case 47: /* column_type: TINYINT  */
#line 803 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3985 "parser.cpp"
    break;

  case 48: /* column_type: SMALLINT  */
#line 804 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3991 "parser.cpp"
    break;

  case 49: /* column_type: INTEGER  */
#line 805 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3997 "parser.cpp"
    break;

  case 50: /* column_type: INT  */
#line 806 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4003 "parser.cpp"
    break;

  case 51: /* column_type: BIGINT  */
#line 807 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4009 "parser.cpp"
    break;

  case 52: /* column_type: HUGEINT  */
#line 808 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4015 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT  */
#line 809 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4021 "parser.cpp"
    break;

  case 54: /* column_type: REAL  */
#line 810 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4027 "parser.cpp"
    break;

  case 55: /* column_type: DOUBLE  */
#line 811 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4033 "parser.cpp"
    break;

  case 56: /* column_type: FLOAT16  */
#line 812 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4039 "parser.cpp"
    break;

  case 57: /* column_type: BFLOAT16  */
#line 813 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4045 "parser.cpp"
    break;

  case 58: /* column_type: DATE  */
#line 814 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4051 "parser.cpp"
    break;

  case 59: /* column_type: TIME  */
#line 815 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4057 "parser.cpp"
    break;

  case 60: /* column_type: DATETIME  */
#line 816 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4063 "parser.cpp"
    break;

  case 61: /* column_type: TIMESTAMP  */
#line 817 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4069 "parser.cpp"
    break;

  case 62: /* column_type: UUID  */
#line 818 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4075 "parser.cpp"
    break;

  case 63: /* column_type: POINT  */
#line 819 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4081 "parser.cpp"
    break;

  case 64: /* column_type: LINE  */
#line 820 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4087 "parser.cpp"
    break;

  case 65: /* column_type: LSEG  */
#line 821 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4093 "parser.cpp"
    break;

  case 66: /* column_type: BOX  */
#line 822 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4099 "parser.cpp"
    break;

  case 67: /* column_type: CIRCLE  */
#line 825 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4105 "parser.cpp"
    break;

  case 68: /* column_type: VARCHAR  */
#line 827 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4111 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4117 "parser.cpp"
    break;

  case 70: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 829 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4123 "parser.cpp"
    break;

  case 71: /* column_type: DECIMAL  */
#line 830 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4129 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4135 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4141 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4147 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4153 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4159 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4165 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4171 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4177 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4183 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4189 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4195 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4201 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4207 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4213 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4219 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4225 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4231 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4237 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4243 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4249 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4255 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4261 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4267 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4273 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4279 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4285 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4291 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4297 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4303 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4309 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4315 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4321 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4327 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4333 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4339 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4345 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4351 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4357 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4363 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4369 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4375 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4381 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4387 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4393 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4399 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 878 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4405 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 879 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4411 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 880 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4417 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 881 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4423 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 882 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4429 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 883 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4435 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 884 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4441 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 885 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4447 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 886 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4453 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 887 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4459 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 888 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4465 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 889 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4471 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 890 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4477 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 891 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4483 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 892 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4489 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 893 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4495 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 894 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4501 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 895 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4507 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 896 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4513 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 897 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4519 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 898 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4525 "parser.cpp"
    break;

  case 138: /* column_constraints: column_constraint  */
#line 917 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4534 "parser.cpp"
    break;

  case 139: /* column_constraints: column_constraints column_constraint  */
#line 921 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4548 "parser.cpp"
    break;

  case 140: /* column_constraint: PRIMARY KEY  */
#line 931 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4556 "parser.cpp"
    break;

  case 141: /* column_constraint: UNIQUE  */
#line 934 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4564 "parser.cpp"
    break;

  case 142: /* column_constraint: NULLABLE  */
#line 937 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4572 "parser.cpp"
    break;

  case 143: /* column_constraint: NOT NULLABLE  */
#line 940 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4580 "parser.cpp"
    break;

  case 144: /* default_expr: DEFAULT constant_expr  */
#line 944 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4588 "parser.cpp"
    break;

  case 145: /* default_expr: %empty  */
#line 947 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4596 "parser.cpp"
    break;

  case 146: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 952 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4606 "parser.cpp"
    break;

  case 147: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 957 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4616 "parser.cpp"
    break;

  case 148: /* identifier_array: IDENTIFIER  */
#line 964 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4627 "parser.cpp"
    break;

  case 149: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 970 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4638 "parser.cpp"
    break;

  case 150: /* delete_statement: DELETE FROM table_name where_clause  */
#line 980 "parser.y"
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
#line 4655 "parser.cpp"
    break;

  case 151: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 996 "parser.y"
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
#line 4694 "parser.cpp"
    break;

  case 152: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1030 "parser.y"
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
#line 4711 "parser.cpp"
    break;

  case 153: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1043 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4719 "parser.cpp"
    break;

  case 154: /* optional_identifier_array: %empty  */
#line 1046 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4727 "parser.cpp"
    break;

  case 155: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1053 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4737 "parser.cpp"
    break;

  case 156: /* explain_type: ANALYZE  */
#line 1059 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4745 "parser.cpp"
    break;

  case 157: /* explain_type: AST  */
#line 1062 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4753 "parser.cpp"
    break;

  case 158: /* explain_type: RAW  */
#line 1065 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4761 "parser.cpp"
    break;

  case 159: /* explain_type: LOGICAL  */
#line 1068 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4769 "parser.cpp"
    break;

  case 160: /* explain_type: PHYSICAL  */
#line 1071 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4777 "parser.cpp"
    break;

  case 161: /* explain_type: PIPELINE  */
#line 1074 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4785 "parser.cpp"
    break;

  case 162: /* explain_type: FRAGMENT  */
#line 1077 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4793 "parser.cpp"
    break;

  case 163: /* explain_type: %empty  */
#line 1080 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4801 "parser.cpp"
    break;

  case 164: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1087 "parser.y"
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
#line 4818 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr  */
#line 1100 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4827 "parser.cpp"
    break;

  case 166: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1104 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4836 "parser.cpp"
    break;

  case 167: /* update_expr: IDENTIFIER '=' expr  */
#line 1109 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4848 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1122 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4864 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1135 "parser.y"
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
#line 4882 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1150 "parser.y"
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
#line 4900 "parser.cpp"
    break;

  case 171: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1165 "parser.y"
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
#line 4918 "parser.cpp"
    break;

  case 172: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1180 "parser.y"
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
#line 4941 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1203 "parser.y"
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
#line 4999 "parser.cpp"
    break;

  case 174: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1256 "parser.y"
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
#line 5059 "parser.cpp"
    break;

  case 175: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1311 "parser.y"
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
#line 5111 "parser.cpp"
    break;

  case 176: /* select_statement: select_without_paren  */
#line 1362 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5119 "parser.cpp"
    break;

  case 177: /* select_statement: select_with_paren  */
#line 1365 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5127 "parser.cpp"
    break;

  case 178: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1368 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5141 "parser.cpp"
    break;

  case 179: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1377 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5155 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_without_paren ')'  */
#line 1387 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5163 "parser.cpp"
    break;

  case 181: /* select_with_paren: '(' select_with_paren ')'  */
#line 1390 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5171 "parser.cpp"
    break;

  case 182: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1394 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5180 "parser.cpp"
    break;

  case 183: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1399 "parser.y"
                                                                                                   {
    if((yyvsp[-1].expr_t) == nullptr and (yyvsp[0].expr_t) != nullptr) {
        delete (yyvsp[-3].select_stmt);
        delete (yyvsp[-2].order_by_expr_list_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Offset expression isn't valid without Limit expression");
        YYERROR;
    }
    if((yyvsp[-3].select_stmt)->search_expr_ != nullptr and ((yyvsp[-2].order_by_expr_list_t) != nullptr or (yyvsp[-1].expr_t) != nullptr or (yyvsp[0].expr_t) != nullptr)) {
        delete (yyvsp[-3].select_stmt);
        if ((yyvsp[-2].order_by_expr_list_t)) {
            for (auto ptr : *((yyvsp[-2].order_by_expr_list_t))) {
                delete ptr;
            }
            delete (yyvsp[-2].order_by_expr_list_t);
        }
        delete (yyvsp[-1].expr_t);
        delete (yyvsp[0].expr_t);
        yyerror(&yyloc, scanner, result, "Result modifier(ORDER BY, LIMIT, OFFSET) is conflict with SEARCH expression.");
        YYERROR;
    }
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 5211 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1426 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5219 "parser.cpp"
    break;

  case 185: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1429 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5227 "parser.cpp"
    break;

  case 186: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1434 "parser.y"
                                                                                                {
    (yyval.select_stmt) = new infinity::SelectStatement();
    (yyval.select_stmt)->select_list_ = (yyvsp[-5].expr_array_t);
    (yyval.select_stmt)->select_distinct_ = (yyvsp[-6].bool_value);
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
#line 5247 "parser.cpp"
    break;

  case 187: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1450 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5255 "parser.cpp"
    break;

  case 188: /* order_by_clause: %empty  */
#line 1453 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5263 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr  */
#line 1457 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5272 "parser.cpp"
    break;

  case 190: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1461 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5281 "parser.cpp"
    break;

  case 191: /* order_by_expr: expr order_by_type  */
#line 1466 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5291 "parser.cpp"
    break;

  case 192: /* order_by_type: ASC  */
#line 1472 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5299 "parser.cpp"
    break;

  case 193: /* order_by_type: DESC  */
#line 1475 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5307 "parser.cpp"
    break;

  case 194: /* order_by_type: %empty  */
#line 1478 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5315 "parser.cpp"
    break;

  case 195: /* limit_expr: LIMIT expr  */
#line 1482 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5323 "parser.cpp"
    break;

  case 196: /* limit_expr: %empty  */
#line 1486 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5329 "parser.cpp"
    break;

  case 197: /* offset_expr: OFFSET expr  */
#line 1488 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5337 "parser.cpp"
    break;

  case 198: /* offset_expr: %empty  */
#line 1492 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5343 "parser.cpp"
    break;

  case 199: /* distinct: DISTINCT  */
#line 1494 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5351 "parser.cpp"
    break;

  case 200: /* distinct: %empty  */
#line 1497 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5359 "parser.cpp"
    break;

  case 201: /* from_clause: FROM table_reference  */
#line 1501 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5367 "parser.cpp"
    break;

  case 202: /* from_clause: %empty  */
#line 1504 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5375 "parser.cpp"
    break;

  case 203: /* search_clause: SEARCH sub_search_array  */
#line 1508 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5385 "parser.cpp"
    break;

  case 204: /* search_clause: %empty  */
#line 1513 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5393 "parser.cpp"
    break;

  case 205: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1517 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5401 "parser.cpp"
    break;

  case 206: /* optional_search_filter_expr: %empty  */
#line 1520 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5409 "parser.cpp"
    break;

  case 207: /* where_clause: WHERE expr  */
#line 1524 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5417 "parser.cpp"
    break;

  case 208: /* where_clause: %empty  */
#line 1527 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5425 "parser.cpp"
    break;

  case 209: /* having_clause: HAVING expr  */
#line 1531 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5433 "parser.cpp"
    break;

  case 210: /* having_clause: %empty  */
#line 1534 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5441 "parser.cpp"
    break;

  case 211: /* group_by_clause: GROUP BY expr_array  */
#line 1538 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5449 "parser.cpp"
    break;

  case 212: /* group_by_clause: %empty  */
#line 1541 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5457 "parser.cpp"
    break;

  case 213: /* set_operator: UNION  */
#line 1545 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5465 "parser.cpp"
    break;

  case 214: /* set_operator: UNION ALL  */
#line 1548 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5473 "parser.cpp"
    break;

  case 215: /* set_operator: INTERSECT  */
#line 1551 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5481 "parser.cpp"
    break;

  case 216: /* set_operator: EXCEPT  */
#line 1554 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5489 "parser.cpp"
    break;

  case 217: /* table_reference: table_reference_unit  */
#line 1562 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5497 "parser.cpp"
    break;

  case 218: /* table_reference: table_reference ',' table_reference_unit  */
#line 1565 "parser.y"
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
#line 5515 "parser.cpp"
    break;

  case 221: /* table_reference_name: table_name table_alias  */
#line 1582 "parser.y"
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
#line 5533 "parser.cpp"
    break;

  case 222: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1596 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5544 "parser.cpp"
    break;

  case 223: /* table_name: IDENTIFIER  */
#line 1605 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5554 "parser.cpp"
    break;

  case 224: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1610 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5566 "parser.cpp"
    break;

  case 225: /* table_alias: AS IDENTIFIER  */
#line 1619 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5576 "parser.cpp"
    break;

  case 226: /* table_alias: IDENTIFIER  */
#line 1624 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5586 "parser.cpp"
    break;

  case 227: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1629 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5597 "parser.cpp"
    break;

  case 228: /* table_alias: %empty  */
#line 1635 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5605 "parser.cpp"
    break;

  case 229: /* with_clause: WITH with_expr_list  */
#line 1642 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5613 "parser.cpp"
    break;

  case 230: /* with_clause: %empty  */
#line 1645 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5621 "parser.cpp"
    break;

  case 231: /* with_expr_list: with_expr  */
#line 1649 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5630 "parser.cpp"
    break;

  case 232: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1652 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5639 "parser.cpp"
    break;

  case 233: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1657 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5651 "parser.cpp"
    break;

  case 234: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1669 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5663 "parser.cpp"
    break;

  case 235: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1676 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5676 "parser.cpp"
    break;

  case 236: /* join_type: INNER  */
#line 1690 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5684 "parser.cpp"
    break;

  case 237: /* join_type: LEFT  */
#line 1693 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5692 "parser.cpp"
    break;

  case 238: /* join_type: RIGHT  */
#line 1696 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5700 "parser.cpp"
    break;

  case 239: /* join_type: OUTER  */
#line 1699 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5708 "parser.cpp"
    break;

  case 240: /* join_type: FULL  */
#line 1702 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5716 "parser.cpp"
    break;

  case 241: /* join_type: CROSS  */
#line 1705 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5724 "parser.cpp"
    break;

  case 242: /* join_type: %empty  */
#line 1708 "parser.y"
                {
}
#line 5731 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASES  */
#line 1714 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5740 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLES  */
#line 1718 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5749 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW VIEWS  */
#line 1722 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5758 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW CONFIGS  */
#line 1726 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5767 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1730 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5779 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW PROFILES  */
#line 1737 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5788 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW BUFFER  */
#line 1741 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5797 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW MEMINDEX  */
#line 1745 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5806 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW QUERIES  */
#line 1749 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5815 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1753 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5825 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TRANSACTIONS  */
#line 1758 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5834 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1762 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5844 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW SESSION VARIABLES  */
#line 1767 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5853 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1771 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5862 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1775 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5873 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1781 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5884 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1787 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5895 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name  */
#line 1793 "parser.y"
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
#line 5911 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1804 "parser.y"
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
#line 5927 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1815 "parser.y"
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
#line 5943 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1826 "parser.y"
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
#line 5960 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1838 "parser.y"
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
#line 5977 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1850 "parser.y"
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
#line 5995 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1863 "parser.y"
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
#line 6014 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1877 "parser.y"
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
#line 6030 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1888 "parser.y"
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
#line 6049 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1902 "parser.y"
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
#line 6070 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1918 "parser.y"
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
#line 6092 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW LOGS  */
#line 1935 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6101 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW DELTA LOGS  */
#line 1939 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6110 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW CATALOGS  */
#line 1943 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6119 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1947 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6128 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1951 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6137 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1955 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6148 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW MEMORY  */
#line 1961 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6157 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1965 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6166 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1969 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6175 "parser.cpp"
    break;

  case 280: /* flush_statement: FLUSH DATA  */
#line 1977 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6184 "parser.cpp"
    break;

  case 281: /* flush_statement: FLUSH LOG  */
#line 1981 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6193 "parser.cpp"
    break;

  case 282: /* flush_statement: FLUSH BUFFER  */
#line 1985 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6202 "parser.cpp"
    break;

  case 283: /* optimize_statement: OPTIMIZE table_name  */
#line 1993 "parser.y"
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
#line 6217 "parser.cpp"
    break;

  case 284: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2004 "parser.y"
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
#line 6241 "parser.cpp"
    break;

  case 285: /* command_statement: USE IDENTIFIER  */
#line 2027 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6252 "parser.cpp"
    break;

  case 286: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 2033 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6262 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2038 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6273 "parser.cpp"
    break;

  case 288: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2044 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6284 "parser.cpp"
    break;

  case 289: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2050 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6297 "parser.cpp"
    break;

  case 290: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2058 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6308 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2064 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6319 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2070 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6330 "parser.cpp"
    break;

  case 293: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2076 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6341 "parser.cpp"
    break;

  case 294: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2082 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6354 "parser.cpp"
    break;

  case 295: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2090 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6365 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2096 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6376 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2102 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6387 "parser.cpp"
    break;

  case 298: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2108 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6398 "parser.cpp"
    break;

  case 299: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2114 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6411 "parser.cpp"
    break;

  case 300: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2122 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6422 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2128 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6433 "parser.cpp"
    break;

  case 302: /* command_statement: LOCK TABLE table_name  */
#line 2134 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6447 "parser.cpp"
    break;

  case 303: /* command_statement: UNLOCK TABLE table_name  */
#line 2143 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6461 "parser.cpp"
    break;

  case 304: /* compact_statement: COMPACT TABLE table_name  */
#line 2153 "parser.y"
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
#line 6478 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2166 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6487 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2170 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6497 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2175 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6508 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2181 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6520 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2188 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6533 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2196 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6547 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2205 "parser.y"
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
#line 6562 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2215 "parser.y"
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
#line 6577 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2225 "parser.y"
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
#line 6593 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2236 "parser.y"
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
#line 6609 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2247 "parser.y"
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
#line 6626 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2259 "parser.y"
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
#line 6643 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2271 "parser.y"
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
#line 6658 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2281 "parser.y"
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
#line 6674 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2292 "parser.y"
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
#line 6691 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2304 "parser.y"
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
#line 6709 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW LOGS  */
#line 2317 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6718 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2321 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6728 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2326 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6738 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2331 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6749 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2337 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6758 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2341 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6767 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2345 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6778 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW NODES  */
#line 2351 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6787 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW NODE IDENTIFIER  */
#line 2355 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6798 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW NODE  */
#line 2361 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6807 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SET ADMIN  */
#line 2365 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6817 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SET STANDALONE  */
#line 2370 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6827 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2375 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6839 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2382 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6853 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2391 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6867 "parser.cpp"
    break;

  case 336: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2401 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6878 "parser.cpp"
    break;

  case 337: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2407 "parser.y"
                                                             {
    auto *ret = new infinity::AddColumnsStatement((yyvsp[-5].table_name_t));
    (yyval.alter_stmt) = ret;

    for (infinity::ColumnDef*& column_def : *(yyvsp[-1].column_def_array_t)) {
        ret->column_defs_.emplace_back(column_def);
    }
    delete (yyvsp[-1].column_def_array_t);
}
#line 6892 "parser.cpp"
    break;

  case 338: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2416 "parser.y"
                                                              {
    auto *ret = new infinity::DropColumnsStatement((yyvsp[-5].table_name_t));
    (yyval.alter_stmt) = ret;
    for (std::string &column_name : *(yyvsp[-1].identifier_array_t)) {
        ret->column_names_.emplace_back(std::move(column_name));
    }
    free((yyvsp[-1].identifier_array_t));
}
#line 6905 "parser.cpp"
    break;

  case 339: /* alter_statement: ALTER TABLE table_name ALTER COLUMN table_column  */
#line 2424 "parser.y"
                                                   {
    auto *ret = new infinity::AlterColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_def_ = (yyvsp[0].table_column_t);
}
#line 6915 "parser.cpp"
    break;

  case 340: /* alter_statement: ALTER TABLE table_name RENAME COLUMN IDENTIFIER TO IDENTIFIER  */
#line 2429 "parser.y"
                                                                {
    auto *ret = new infinity::RenameColumnStatement((yyvsp[-5].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    ret->new_column_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6928 "parser.cpp"
    break;

  case 341: /* expr_array: expr_alias  */
#line 2442 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6937 "parser.cpp"
    break;

  case 342: /* expr_array: expr_array ',' expr_alias  */
#line 2446 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6946 "parser.cpp"
    break;

  case 343: /* expr_array_list: '(' expr_array ')'  */
#line 2451 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6955 "parser.cpp"
    break;

  case 344: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2455 "parser.y"
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
#line 6981 "parser.cpp"
    break;

  case 345: /* expr_alias: expr AS IDENTIFIER  */
#line 2488 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6992 "parser.cpp"
    break;

  case 346: /* expr_alias: expr  */
#line 2494 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7000 "parser.cpp"
    break;

  case 352: /* operand: '(' expr ')'  */
#line 2504 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7008 "parser.cpp"
    break;

  case 353: /* operand: '(' select_without_paren ')'  */
#line 2507 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7019 "parser.cpp"
    break;

  case 354: /* operand: constant_expr  */
#line 2513 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7027 "parser.cpp"
    break;

  case 365: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2529 "parser.y"
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
#line 7047 "parser.cpp"
    break;

  case 366: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2547 "parser.y"
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
#line 7097 "parser.cpp"
    break;

  case 367: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2593 "parser.y"
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
#line 7140 "parser.cpp"
    break;

  case 368: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2632 "parser.y"
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
#line 7187 "parser.cpp"
    break;

  case 369: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2675 "parser.y"
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
#line 7235 "parser.cpp"
    break;

  case 370: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2722 "parser.y"
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
#line 7263 "parser.cpp"
    break;

  case 371: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2746 "parser.y"
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
#line 7290 "parser.cpp"
    break;

  case 372: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2769 "parser.y"
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
#line 7315 "parser.cpp"
    break;

  case 373: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2790 "parser.y"
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
#line 7340 "parser.cpp"
    break;

  case 374: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2811 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7354 "parser.cpp"
    break;

  case 375: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2820 "parser.y"
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
#line 7370 "parser.cpp"
    break;

  case 376: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2832 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7382 "parser.cpp"
    break;

  case 377: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2839 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7396 "parser.cpp"
    break;

  case 378: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2849 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7407 "parser.cpp"
    break;

  case 379: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2855 "parser.y"
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
#line 7423 "parser.cpp"
    break;

  case 380: /* sub_search: match_vector_expr  */
#line 2867 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7431 "parser.cpp"
    break;

  case 381: /* sub_search: match_text_expr  */
#line 2870 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7439 "parser.cpp"
    break;

  case 382: /* sub_search: match_tensor_expr  */
#line 2873 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7447 "parser.cpp"
    break;

  case 383: /* sub_search: match_sparse_expr  */
#line 2876 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7455 "parser.cpp"
    break;

  case 384: /* sub_search: query_expr  */
#line 2879 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7463 "parser.cpp"
    break;

  case 385: /* sub_search: fusion_expr  */
#line 2882 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7471 "parser.cpp"
    break;

  case 386: /* sub_search_array: sub_search  */
#line 2886 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7480 "parser.cpp"
    break;

  case 387: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2890 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7489 "parser.cpp"
    break;

  case 388: /* function_expr: IDENTIFIER '(' ')'  */
#line 2895 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7502 "parser.cpp"
    break;

  case 389: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2903 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7515 "parser.cpp"
    break;

  case 390: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2911 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7529 "parser.cpp"
    break;

  case 391: /* function_expr: operand IS NOT NULLABLE  */
#line 2920 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7541 "parser.cpp"
    break;

  case 392: /* function_expr: operand IS NULLABLE  */
#line 2927 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7553 "parser.cpp"
    break;

  case 393: /* function_expr: NOT operand  */
#line 2934 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7565 "parser.cpp"
    break;

  case 394: /* function_expr: '-' operand  */
#line 2941 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7577 "parser.cpp"
    break;

  case 395: /* function_expr: '+' operand  */
#line 2948 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7589 "parser.cpp"
    break;

  case 396: /* function_expr: operand '-' operand  */
#line 2955 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7602 "parser.cpp"
    break;

  case 397: /* function_expr: operand '+' operand  */
#line 2963 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7615 "parser.cpp"
    break;

  case 398: /* function_expr: operand '*' operand  */
#line 2971 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7628 "parser.cpp"
    break;

  case 399: /* function_expr: operand '/' operand  */
#line 2979 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7641 "parser.cpp"
    break;

  case 400: /* function_expr: operand '%' operand  */
#line 2987 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7654 "parser.cpp"
    break;

  case 401: /* function_expr: operand '=' operand  */
#line 2995 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7667 "parser.cpp"
    break;

  case 402: /* function_expr: operand EQUAL operand  */
#line 3003 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7680 "parser.cpp"
    break;

  case 403: /* function_expr: operand NOT_EQ operand  */
#line 3011 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7693 "parser.cpp"
    break;

  case 404: /* function_expr: operand '<' operand  */
#line 3019 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7706 "parser.cpp"
    break;

  case 405: /* function_expr: operand '>' operand  */
#line 3027 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7719 "parser.cpp"
    break;

  case 406: /* function_expr: operand LESS_EQ operand  */
#line 3035 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7732 "parser.cpp"
    break;

  case 407: /* function_expr: operand GREATER_EQ operand  */
#line 3043 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7745 "parser.cpp"
    break;

  case 408: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3051 "parser.y"
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
#line 7780 "parser.cpp"
    break;

  case 409: /* function_expr: operand LIKE operand  */
#line 3081 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7793 "parser.cpp"
    break;

  case 410: /* function_expr: operand NOT LIKE operand  */
#line 3089 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7806 "parser.cpp"
    break;

  case 411: /* conjunction_expr: expr AND expr  */
#line 3098 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7819 "parser.cpp"
    break;

  case 412: /* conjunction_expr: expr OR expr  */
#line 3106 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7832 "parser.cpp"
    break;

  case 413: /* between_expr: operand BETWEEN operand AND operand  */
#line 3115 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7844 "parser.cpp"
    break;

  case 414: /* in_expr: operand IN '(' expr_array ')'  */
#line 3123 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7855 "parser.cpp"
    break;

  case 415: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3129 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7866 "parser.cpp"
    break;

  case 416: /* case_expr: CASE expr case_check_array END  */
#line 3136 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7877 "parser.cpp"
    break;

  case 417: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3142 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7889 "parser.cpp"
    break;

  case 418: /* case_expr: CASE case_check_array END  */
#line 3149 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7899 "parser.cpp"
    break;

  case 419: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3154 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7910 "parser.cpp"
    break;

  case 420: /* case_check_array: WHEN expr THEN expr  */
#line 3161 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7922 "parser.cpp"
    break;

  case 421: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3168 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7934 "parser.cpp"
    break;

  case 422: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3176 "parser.y"
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
#line 7965 "parser.cpp"
    break;

  case 423: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3203 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7976 "parser.cpp"
    break;

  case 424: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3209 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7987 "parser.cpp"
    break;

  case 425: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3215 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7999 "parser.cpp"
    break;

  case 426: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3222 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8011 "parser.cpp"
    break;

  case 427: /* column_expr: IDENTIFIER  */
#line 3230 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8023 "parser.cpp"
    break;

  case 428: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3237 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8035 "parser.cpp"
    break;

  case 429: /* column_expr: '*'  */
#line 3244 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8045 "parser.cpp"
    break;

  case 430: /* column_expr: column_expr '.' '*'  */
#line 3249 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8059 "parser.cpp"
    break;

  case 431: /* constant_expr: STRING  */
#line 3259 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8069 "parser.cpp"
    break;

  case 432: /* constant_expr: TRUE  */
#line 3264 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8079 "parser.cpp"
    break;

  case 433: /* constant_expr: FALSE  */
#line 3269 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8089 "parser.cpp"
    break;

  case 434: /* constant_expr: DOUBLE_VALUE  */
#line 3274 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8099 "parser.cpp"
    break;

  case 435: /* constant_expr: LONG_VALUE  */
#line 3279 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8109 "parser.cpp"
    break;

  case 436: /* constant_expr: DATE STRING  */
#line 3284 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8119 "parser.cpp"
    break;

  case 437: /* constant_expr: TIME STRING  */
#line 3289 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8129 "parser.cpp"
    break;

  case 438: /* constant_expr: DATETIME STRING  */
#line 3294 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8139 "parser.cpp"
    break;

  case 439: /* constant_expr: TIMESTAMP STRING  */
#line 3299 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8149 "parser.cpp"
    break;

  case 440: /* constant_expr: INTERVAL interval_expr  */
#line 3304 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8157 "parser.cpp"
    break;

  case 441: /* constant_expr: interval_expr  */
#line 3307 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8165 "parser.cpp"
    break;

  case 442: /* constant_expr: common_array_expr  */
#line 3310 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8173 "parser.cpp"
    break;

  case 443: /* common_array_expr: array_expr  */
#line 3314 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8181 "parser.cpp"
    break;

  case 444: /* common_array_expr: subarray_array_expr  */
#line 3317 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8189 "parser.cpp"
    break;

  case 445: /* common_array_expr: sparse_array_expr  */
#line 3320 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8197 "parser.cpp"
    break;

  case 446: /* common_array_expr: empty_array_expr  */
#line 3323 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8205 "parser.cpp"
    break;

  case 447: /* common_sparse_array_expr: sparse_array_expr  */
#line 3327 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8213 "parser.cpp"
    break;

  case 448: /* common_sparse_array_expr: array_expr  */
#line 3330 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8221 "parser.cpp"
    break;

  case 449: /* common_sparse_array_expr: empty_array_expr  */
#line 3333 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8229 "parser.cpp"
    break;

  case 450: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3337 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8237 "parser.cpp"
    break;

  case 451: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3341 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8247 "parser.cpp"
    break;

  case 452: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3346 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8256 "parser.cpp"
    break;

  case 453: /* sparse_array_expr: long_sparse_array_expr  */
#line 3351 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8264 "parser.cpp"
    break;

  case 454: /* sparse_array_expr: double_sparse_array_expr  */
#line 3354 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8272 "parser.cpp"
    break;

  case 455: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3358 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8280 "parser.cpp"
    break;

  case 456: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3362 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8292 "parser.cpp"
    break;

  case 457: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3369 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8303 "parser.cpp"
    break;

  case 458: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3376 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8311 "parser.cpp"
    break;

  case 459: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3380 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8323 "parser.cpp"
    break;

  case 460: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3387 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8334 "parser.cpp"
    break;

  case 461: /* empty_array_expr: '[' ']'  */
#line 3394 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8342 "parser.cpp"
    break;

  case 462: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3398 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8350 "parser.cpp"
    break;

  case 463: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3402 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8358 "parser.cpp"
    break;

  case 464: /* array_expr: long_array_expr  */
#line 3406 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8366 "parser.cpp"
    break;

  case 465: /* array_expr: double_array_expr  */
#line 3409 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8374 "parser.cpp"
    break;

  case 466: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3413 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8382 "parser.cpp"
    break;

  case 467: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3417 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8392 "parser.cpp"
    break;

  case 468: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3422 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8401 "parser.cpp"
    break;

  case 469: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3427 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8409 "parser.cpp"
    break;

  case 470: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3431 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8419 "parser.cpp"
    break;

  case 471: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3436 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8428 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE SECONDS  */
#line 3441 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8439 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE SECOND  */
#line 3447 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8450 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE MINUTES  */
#line 3453 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8461 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE MINUTE  */
#line 3459 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8472 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE HOURS  */
#line 3465 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8483 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE HOUR  */
#line 3471 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8494 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE DAYS  */
#line 3477 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8505 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE DAY  */
#line 3483 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8516 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE MONTHS  */
#line 3489 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8527 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE MONTH  */
#line 3495 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8538 "parser.cpp"
    break;

  case 482: /* interval_expr: LONG_VALUE YEARS  */
#line 3501 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8549 "parser.cpp"
    break;

  case 483: /* interval_expr: LONG_VALUE YEAR  */
#line 3507 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8560 "parser.cpp"
    break;

  case 484: /* copy_option_list: copy_option  */
#line 3518 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8569 "parser.cpp"
    break;

  case 485: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3522 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8578 "parser.cpp"
    break;

  case 486: /* copy_option: FORMAT IDENTIFIER  */
#line 3527 "parser.y"
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
#line 8614 "parser.cpp"
    break;

  case 487: /* copy_option: DELIMITER STRING  */
#line 3558 "parser.y"
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
#line 8629 "parser.cpp"
    break;

  case 488: /* copy_option: HEADER  */
#line 3568 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8639 "parser.cpp"
    break;

  case 489: /* copy_option: OFFSET LONG_VALUE  */
#line 3573 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8649 "parser.cpp"
    break;

  case 490: /* copy_option: LIMIT LONG_VALUE  */
#line 3578 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8659 "parser.cpp"
    break;

  case 491: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3583 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8669 "parser.cpp"
    break;

  case 492: /* file_path: STRING  */
#line 3589 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8677 "parser.cpp"
    break;

  case 493: /* if_exists: IF EXISTS  */
#line 3593 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8683 "parser.cpp"
    break;

  case 494: /* if_exists: %empty  */
#line 3594 "parser.y"
  { (yyval.bool_value) = false; }
#line 8689 "parser.cpp"
    break;

  case 495: /* if_not_exists: IF NOT EXISTS  */
#line 3596 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8695 "parser.cpp"
    break;

  case 496: /* if_not_exists: %empty  */
#line 3597 "parser.y"
  { (yyval.bool_value) = false; }
#line 8701 "parser.cpp"
    break;

  case 499: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3612 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8714 "parser.cpp"
    break;

  case 500: /* if_not_exists_info: %empty  */
#line 3620 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8722 "parser.cpp"
    break;

  case 501: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3624 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8730 "parser.cpp"
    break;

  case 502: /* with_index_param_list: %empty  */
#line 3627 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8738 "parser.cpp"
    break;

  case 503: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3631 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8746 "parser.cpp"
    break;

  case 504: /* optional_table_properties_list: %empty  */
#line 3634 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8754 "parser.cpp"
    break;

  case 505: /* index_param_list: index_param  */
#line 3638 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8763 "parser.cpp"
    break;

  case 506: /* index_param_list: index_param_list ',' index_param  */
#line 3642 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8772 "parser.cpp"
    break;

  case 507: /* index_param: IDENTIFIER  */
#line 3647 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8783 "parser.cpp"
    break;

  case 508: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3653 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8798 "parser.cpp"
    break;

  case 509: /* index_param: IDENTIFIER '=' STRING  */
#line 3663 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8813 "parser.cpp"
    break;

  case 510: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3673 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8826 "parser.cpp"
    break;

  case 511: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3681 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8839 "parser.cpp"
    break;

  case 512: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3692 "parser.y"
                                                                       {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "bmp") == 0) {
        index_type = infinity::IndexType::kBMP;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
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
#line 8875 "parser.cpp"
    break;

  case 513: /* index_info: '(' IDENTIFIER ')'  */
#line 3723 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8886 "parser.cpp"
    break;


#line 8890 "parser.cpp"

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

#line 3730 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

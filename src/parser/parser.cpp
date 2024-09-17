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
#define YYLAST   1349

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  221
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  511
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1138

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
    2361,  2365,  2370,  2375,  2382,  2391,  2401,  2407,  2416,  2429,
    2433,  2438,  2442,  2475,  2481,  2485,  2486,  2487,  2488,  2489,
    2491,  2494,  2500,  2503,  2504,  2505,  2506,  2507,  2508,  2509,
    2510,  2511,  2512,  2516,  2534,  2580,  2619,  2662,  2709,  2733,
    2756,  2777,  2798,  2807,  2819,  2826,  2836,  2842,  2854,  2857,
    2860,  2863,  2866,  2869,  2873,  2877,  2882,  2890,  2898,  2907,
    2914,  2921,  2928,  2935,  2942,  2950,  2958,  2966,  2974,  2982,
    2990,  2998,  3006,  3014,  3022,  3030,  3038,  3068,  3076,  3085,
    3093,  3102,  3110,  3116,  3123,  3129,  3136,  3141,  3148,  3155,
    3163,  3190,  3196,  3202,  3209,  3217,  3224,  3231,  3236,  3246,
    3251,  3256,  3261,  3266,  3271,  3276,  3281,  3286,  3291,  3294,
    3297,  3301,  3304,  3307,  3310,  3314,  3317,  3320,  3324,  3328,
    3333,  3338,  3341,  3345,  3349,  3356,  3363,  3367,  3374,  3381,
    3385,  3389,  3393,  3396,  3400,  3404,  3409,  3414,  3418,  3423,
    3428,  3434,  3440,  3446,  3452,  3458,  3464,  3470,  3476,  3482,
    3488,  3494,  3505,  3509,  3514,  3545,  3555,  3560,  3565,  3570,
    3576,  3580,  3581,  3583,  3584,  3586,  3587,  3599,  3607,  3611,
    3614,  3618,  3621,  3625,  3629,  3634,  3640,  3650,  3660,  3668,
    3679,  3710
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

#define YYPACT_NINF (-695)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-499)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     713,   268,     5,   277,    91,   128,    91,   197,   613,   640,
      71,    93,   133,   159,   255,   213,   221,   232,    98,    20,
     -28,   244,    44,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,   306,  -695,  -695,   309,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,   259,   259,   259,   259,    24,    91,   265,   265,
     265,   265,   265,   130,   354,    91,    -8,   358,   367,   371,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,   188,   379,    91,
    -695,  -695,  -695,  -695,  -695,   386,  -695,   104,   209,  -695,
     399,  -695,   260,  -695,  -695,   273,  -695,   289,    52,    91,
      91,    91,    91,  -695,  -695,  -695,  -695,   -51,  -695,   352,
     229,  -695,   451,   164,   168,   466,   256,   262,  -695,    48,
    -695,   455,  -695,  -695,    16,   411,  -695,   407,   420,   492,
      91,    91,    91,   499,   440,   295,   442,   527,    91,    91,
      91,   534,   567,   577,   514,   583,   583,   433,    90,    95,
     103,  -695,  -695,  -695,  -695,  -695,  -695,  -695,   306,  -695,
    -695,  -695,  -695,  -695,  -695,   360,  -695,  -695,   591,  -695,
     596,  -695,  -695,   594,   602,  -695,  -695,  -695,  -695,   267,
    -695,  -695,  -695,    91,   401,   232,   583,  -695,   446,  -695,
     620,  -695,  -695,   630,  -695,  -695,   631,  -695,   634,   578,
    -695,  -695,  -695,  -695,    16,  -695,  -695,  -695,   433,   581,
     575,   582,  -695,   -29,  -695,   295,  -695,    91,   660,    46,
    -695,  -695,  -695,  -695,  -695,   597,  -695,   459,   -31,  -695,
     433,  -695,  -695,   584,   586,   456,  -695,  -695,   551,   571,
     457,   460,   283,   662,   676,   677,   678,  -695,  -695,   684,
     469,   212,   489,   490,   579,   579,  -695,     9,   421,    89,
    -695,   -14,   712,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,  -695,   491,  -695,  -695,  -695,
     -85,  -695,  -695,    54,  -695,    64,  -695,  -695,  -695,    85,
    -695,   113,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,   704,   706,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,   659,   661,   633,
     637,   309,  -695,  -695,   710,    51,  -695,   726,  -695,   216,
     517,   519,   -30,   433,   433,   665,  -695,   -28,   111,   673,
     526,  -695,   177,   530,  -695,    91,   433,   577,  -695,   276,
     536,   537,   245,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,   579,   541,   777,   674,   433,
     433,   -23,   286,  -695,  -695,  -695,  -695,   551,  -695,   760,
     553,   554,   555,   556,   762,   768,   332,   332,  -695,   557,
    -695,  -695,  -695,  -695,   558,    99,   690,   433,   772,   433,
     433,    -1,   568,    68,   579,   579,   579,   579,   579,   579,
     579,   579,   579,   579,   579,   579,   579,   579,    23,  -695,
     569,  -695,   779,  -695,   784,  -695,   789,  -695,   791,   748,
     413,   585,   587,   796,   588,  -695,   592,  -695,   795,  -695,
      82,   643,   644,  -695,  -695,    10,   635,   598,  -695,    47,
     276,   433,  -695,   306,   917,   689,   608,   182,  -695,  -695,
    -695,   -28,   821,  -695,  -695,   822,   433,   609,  -695,   276,
    -695,   205,   205,   433,  -695,   233,   674,   679,   623,     6,
      33,   339,  -695,   433,   433,   759,   433,   839,    26,   433,
     625,   243,   520,  -695,  -695,   583,  -695,  -695,  -695,   687,
     632,   579,   421,   711,  -695,   798,   798,   308,   308,   734,
     798,   798,   308,   308,   332,   332,  -695,  -695,  -695,  -695,
    -695,  -695,   629,  -695,   638,  -695,  -695,  -695,   844,   845,
    -695,   660,   854,  -695,   856,  -695,  -695,   855,  -695,   858,
     859,   -28,   645,   767,  -695,   135,  -695,   291,   514,   433,
    -695,  -695,  -695,   276,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,   656,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,   657,   663,
     664,   666,   667,   668,   186,   669,   660,   842,   111,   306,
     670,  -695,   278,   681,   869,   870,   873,   877,  -695,   885,
     287,  -695,   288,   315,  -695,   682,  -695,   917,   433,  -695,
     433,    -3,   117,   579,   -71,   675,  -695,  -134,    88,    17,
     683,  -695,   889,  -695,  -695,   813,   421,   798,   685,   316,
    -695,   579,   894,   897,   857,   862,   347,   348,  -695,   707,
     382,  -695,   919,  -695,  -695,    18,    10,   863,  -695,  -695,
    -695,  -695,  -695,  -695,   864,  -695,   926,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,   716,   878,  -695,   931,   493,
     651,   865,   953,   970,   987,   812,   815,  -695,  -695,   163,
    -695,   818,   660,   392,   735,  -695,  -695,   781,  -695,   433,
    -695,  -695,  -695,  -695,  -695,  -695,   205,  -695,  -695,  -695,
     740,   276,   126,  -695,   433,   699,   744,   954,   569,   752,
     747,   433,  -695,   753,   755,   756,   394,  -695,  -695,   777,
     965,   967,  -695,  -695,   854,   481,  -695,   856,   123,   135,
     767,    10,    10,   763,   291,   918,   920,   396,   773,   774,
     775,   776,   778,   780,   782,   783,   809,   882,   816,   817,
     819,   820,   823,   824,   834,   835,   838,   840,   884,   841,
     851,   852,   868,   872,   874,   875,   880,   881,   883,   899,
     886,   887,   888,   890,   891,   892,   895,   896,   898,   900,
     901,   902,   903,   904,   905,   906,   907,   908,   909,   910,
     911,   961,   912,   913,   914,   915,   916,   921,   922,   923,
     924,   925,   964,   927,  -695,  -695,    14,  -695,  -695,  -695,
     404,  -695,   856,   988,   408,  -695,  -695,  -695,   276,  -695,
     550,   928,   929,   930,    25,   932,  -695,  -695,  -695,  1002,
     860,   276,  -695,   205,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,   994,  -695,  -695,  -695,  1007,   660,
    -695,   433,   433,  -695,  -695,  1071,  1082,  1086,  1095,  1097,
    1098,  1102,  1107,  1110,  1112,   933,  1114,  1130,  1131,  1132,
    1133,  1139,  1144,  1147,  1148,  1149,   937,  1151,  1152,  1153,
    1154,  1155,  1156,  1157,  1158,  1159,  1160,   948,  1162,  1163,
    1164,  1165,  1166,  1167,  1168,  1169,  1170,  1171,   959,  1173,
    1174,  1175,  1176,  1177,  1178,  1179,  1180,  1181,  1182,   971,
    1183,  1185,  1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,
     981,  1195,  -695,  -695,   409,   637,  -695,  -695,  1198,    65,
     989,  1199,  1200,  -695,   416,  1201,   433,   428,   990,   276,
     992,   995,   996,   997,   998,   999,  1000,  1001,  1003,  1004,
    1204,  1005,  1006,  1008,  1009,  1010,  1011,  1012,  1013,  1014,
    1015,  1212,  1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,
    1024,  1025,  1217,  1026,  1027,  1028,  1029,  1030,  1031,  1032,
    1033,  1034,  1035,  1246,  1037,  1038,  1039,  1040,  1041,  1042,
    1043,  1044,  1045,  1046,  1257,  1048,  1049,  1050,  1051,  1052,
    1053,  1054,  1055,  1056,  1057,  1268,  1059,  -695,  -695,  1058,
     747,  -695,  1060,  1061,  -695,   395,   276,  -695,  -695,  -695,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  1062,  -695,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  1065,
    -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    1066,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  1067,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  1068,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  -695,  -695,  1069,  -695,  1202,  1070,  1272,    27,  1072,
    1281,  1283,  -695,  -695,  -695,  -695,  -695,  -695,  -695,  -695,
    -695,  1073,  -695,  1074,   747,   637,  1284,   505,    45,  1075,
    1290,  1079,  -695,   518,  1291,  -695,   747,   637,   747,   -49,
    1292,  -695,  1241,  1083,  -695,  1084,  1255,  1256,  -695,  -695,
    -695,   -40,  -695,  -695,  1088,  1258,  1259,  -695,  1303,  -695,
    1092,  1093,  1305,   637,  1094,  -695,   637,  -695
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     230,     0,     0,     0,     0,     0,     0,     0,   163,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     230,     0,   496,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   177,   176,     0,     8,    14,    15,    16,    17,
      18,    19,   494,   494,   494,   494,   494,     0,   492,   492,
     492,   492,   492,   223,     0,     0,     0,     0,     0,     0,
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
     196,     0,    32,     0,    33,   154,   497,     0,     0,   230,
     491,   168,   170,   169,   171,     0,   224,     0,   208,   165,
       0,   150,   490,     0,     0,   425,   429,   432,   433,     0,
       0,     0,     0,     0,     0,     0,     0,   430,   431,     0,
       0,     0,     0,     0,     0,     0,   427,     0,   230,     0,
     339,   344,   345,   359,   357,   360,   358,   361,   362,   354,
     349,   348,   347,   355,   356,   346,   353,   352,   440,   442,
       0,   443,   451,     0,   452,     0,   444,   441,   462,     0,
     463,     0,   439,   289,   291,   290,   287,   288,   294,   296,
     295,   292,   293,   299,   301,   300,   297,   298,     0,     0,
     262,   261,   267,   257,   258,   252,   276,     0,     0,     0,
     500,     0,   232,   286,     0,   322,   327,   306,   329,     0,
       0,     0,   202,     0,     0,   198,   493,   230,     0,     0,
       0,   148,     0,     0,   152,     0,     0,     0,   164,   207,
       0,     0,     0,   471,   470,   473,   472,   475,   474,   477,
     476,   479,   478,   481,   480,     0,     0,   391,   230,     0,
       0,     0,     0,   434,   435,   436,   437,     0,   438,     0,
       0,     0,     0,     0,     0,     0,   393,   392,   468,   465,
     459,   449,   454,   457,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   448,
       0,   453,     0,   456,     0,   464,     0,   467,     0,   268,
     263,     0,     0,     0,     0,   284,     0,   333,     0,   323,
       0,     0,     0,   185,   184,     0,   204,   187,   189,   194,
     195,     0,   183,    35,     0,     0,     0,     0,    38,    42,
      43,   230,     0,    37,   153,     0,     0,   151,   172,   167,
     166,     0,     0,     0,   386,     0,   230,     0,     0,     0,
       0,     0,   416,     0,     0,     0,     0,     0,     0,     0,
     206,     0,     0,   351,   350,     0,   340,   343,   409,   410,
       0,     0,   230,     0,   390,   400,   401,   404,   405,     0,
     407,   399,   402,   403,   395,   394,   396,   397,   398,   426,
     428,   450,     0,   455,     0,   458,   466,   469,     0,     0,
     264,     0,     0,   336,     0,   233,   324,     0,   307,     0,
       0,   230,   201,   217,   219,   228,   220,     0,   208,     0,
     192,   193,   191,   197,    46,    49,    50,    47,    48,    51,
      52,    68,    53,    55,    54,    71,    58,    59,    60,    56,
      57,    61,    62,    63,    64,    65,    66,    67,     0,     0,
       0,     0,     0,     0,   500,     0,     0,   502,     0,    36,
       0,   149,     0,     0,     0,     0,     0,     0,   486,     0,
       0,   482,     0,     0,   387,     0,   421,     0,     0,   414,
       0,     0,     0,     0,     0,     0,   425,     0,     0,     0,
       0,   376,     0,   461,   460,     0,   230,   408,     0,     0,
     389,     0,     0,     0,   269,   265,     0,     0,    40,   505,
       0,   503,   308,   334,   335,     0,     0,     0,   237,   238,
     239,   240,   236,   241,     0,   226,     0,   221,   380,   378,
     381,   379,   382,   383,   384,   203,   212,   190,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   141,   142,   145,
     138,   145,     0,     0,     0,    34,    39,   511,   341,     0,
     488,   487,   485,   484,   489,   175,     0,   173,   388,   422,
       0,   418,     0,   417,     0,     0,     0,     0,     0,     0,
     206,     0,   374,     0,     0,     0,     0,   423,   412,   411,
       0,     0,   338,   337,     0,     0,   499,     0,     0,   228,
     218,     0,     0,   225,     0,     0,   210,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   143,   140,     0,   139,    45,    44,
       0,   147,     0,     0,     0,   483,   420,   415,   419,   406,
       0,     0,   206,     0,     0,     0,   445,   447,   446,     0,
       0,   205,   377,     0,   424,   413,   270,   266,    41,   506,
     507,   509,   508,   504,     0,   309,   222,   234,     0,     0,
     385,     0,     0,   186,    70,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   144,   146,     0,   500,   342,   465,     0,     0,
       0,     0,     0,   375,     0,   310,     0,     0,   211,   209,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   501,   510,     0,
     206,   372,     0,   206,   174,     0,   235,   227,    69,    75,
      76,    73,    74,    77,    78,    79,    80,    81,     0,    72,
     119,   120,   117,   118,   121,   122,   123,   124,   125,     0,
     116,    86,    87,    84,    85,    88,    89,    90,    91,    92,
       0,    83,    97,    98,    95,    96,    99,   100,   101,   102,
     103,     0,    94,   130,   131,   128,   129,   132,   133,   134,
     135,   136,     0,   127,   108,   109,   106,   107,   110,   111,
     112,   113,   114,     0,   105,     0,     0,     0,     0,     0,
       0,     0,   312,   311,   317,    82,   126,    93,   104,   137,
     115,   206,   373,     0,   206,   500,   318,   313,     0,     0,
       0,     0,   371,     0,     0,   314,   206,   500,   206,   500,
       0,   319,   315,     0,   367,     0,     0,     0,   370,   320,
     316,   500,   363,   369,     0,     0,     0,   366,     0,   365,
       0,     0,     0,   500,     0,   368,   500,   364
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -695,  -695,  -695,  1203,  -695,  1244,  -695,  -695,   736,  -506,
     718,  -695,   647,   642,  -695,  -510,  1250,  1251,  1115,  -695,
    -695,  1252,  -695,   984,  1260,  1261,   -63,  1302,   -19,  1036,
    1129,   -33,  -695,  -695,   785,  -695,  -695,  -695,  -695,  -695,
    -695,  -694,  -208,  -695,  -695,  -695,  -695,   693,  -143,    21,
     606,  -695,  -695,  1161,  -695,  -695,  1263,  1264,  1265,  1266,
    1267,  -695,  -695,  -196,  -695,   939,  -220,  -222,  -498,  -494,
    -492,  -487,  -484,  -483,   611,  -695,  -695,  -695,  -695,  -695,
    -695,   976,  -695,  -695,   861,   542,  -242,  -695,  -695,  -695,
     641,  -695,  -695,  -695,  -695,   646,   934,   935,  -292,  -695,
    -695,  -695,  -695,  1103,  -454,   655,  -127,   497,   508,  -695,
    -695,  -571,  -695,   546,   626,  -695
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,   141,    24,   447,   627,   448,   449,
     574,   669,   670,   798,   450,   332,    25,    26,   209,    27,
      67,    28,   218,   219,    29,    30,    31,    32,    33,   116,
     195,   117,   200,   437,   438,   542,   325,   442,   198,   436,
     538,   610,   221,   843,   726,   114,   532,   533,   534,   535,
     647,    34,   100,   101,   536,   644,    35,    36,    37,    38,
      39,    40,    41,   249,   457,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   654,   655,   259,   260,   261,   262,
     263,   362,   264,   265,   266,   267,   268,   815,   269,   270,
     271,   272,   273,   274,   275,   276,   382,   383,   277,   278,
     279,   280,   281,   282,   590,   591,   223,   127,   119,   110,
     124,   425,   675,   630,   631,   453
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     339,   107,   322,   671,   148,   381,   820,   357,   592,   224,
     338,   626,   361,    53,   378,   379,   628,   173,   226,   227,
     228,   700,   376,   377,   115,    54,   509,    56,   385,   606,
     378,   379,   327,  1094,   103,   424,   104,    98,   111,   648,
     112,   220,    47,   649,   424,   650,   113,   388,  -495,   313,
     651,  1106,   435,   652,   653,     1,    17,     2,     3,     4,
       5,     6,     7,     8,     9,    10,   673,   597,   125,  1000,
     360,    11,    12,    13,   135,   136,   134,    14,    15,    16,
     490,   196,  1116,   408,   333,   698,   693,   389,   390,   701,
     155,  1125,  -498,   283,    53,   284,   285,   428,   288,   701,
     289,   290,   118,   439,   440,    89,   293,   429,   294,   295,
     169,   170,   171,   172,   444,   527,   459,   701,   920,  1117,
     233,   234,   235,   540,   541,   598,   236,    90,  1126,   409,
      17,   528,    17,   357,   410,   389,   390,   701,   645,   469,
     470,   203,   204,   205,   389,   390,   465,   493,   696,   212,
     213,   214,   237,   238,   239,   389,   390,   834,   286,   835,
     491,   321,   800,   291,   389,   390,   132,    91,   511,   488,
     489,   296,   495,   496,   497,   498,   499,   500,   501,   502,
     503,   504,   505,   506,   507,   508,   328,    20,   337,   387,
     334,   389,   390,    92,   310,     1,   646,     2,     3,     4,
       5,     6,     7,   494,     9,   389,   390,   137,   828,   694,
      55,    11,    12,    13,   105,   807,    96,    14,    15,    16,
      18,   543,   247,   380,    97,   531,   648,   247,   330,   384,
     649,   194,   650,   510,   719,    99,   246,   651,    19,   380,
     652,   653,   665,   445,   108,   446,   389,   390,   225,   226,
     227,   228,   167,   601,   602,   168,   604,   389,   390,   608,
     582,   287,   109,    20,   443,   665,   292,   593,   411,   617,
     424,   102,    17,   412,   297,   389,   390,   307,   413,   584,
     585,   157,   158,   414,   389,   390,   225,   226,   227,   228,
     586,   587,   588,   308,   309,   666,   619,   667,   668,   415,
     796,    42,    43,    44,   416,   386,  1076,   699,   387,  1079,
      48,    49,    50,    45,    46,   484,   463,   115,   666,   439,
     667,   668,    51,    52,   229,   230,   111,   417,   112,   927,
     656,   180,   418,   231,   113,   232,   370,   118,   371,   468,
     372,   373,   181,   126,   998,   182,   183,   132,   184,   185,
     186,   233,   234,   235,   177,   178,   458,   236,   615,   179,
      18,   138,   229,   230,   187,   188,    57,    58,   133,   924,
     139,   231,    59,   232,   140,   472,   360,   473,   691,   474,
     692,   695,   154,   237,   238,   239,   159,   160,   579,   233,
     234,   235,   156,   454,   589,   236,   455,  1099,   577,   709,
    1101,   578,   161,    20,   811,   240,   298,   818,   431,   432,
     299,   300,  1113,   174,  1115,   301,   302,    93,    94,    95,
     706,   237,   238,   239,   225,   226,   227,   228,   599,   241,
     600,   242,   474,   243,   389,   390,   225,   226,   227,   228,
     162,  1080,   163,   240,   393,  1081,  1082,   595,   175,   594,
    1083,  1084,   387,   244,   245,   246,   813,   176,   247,   611,
     248,   464,   612,  -499,  -499,   519,   520,   241,   635,   242,
     189,   243,   190,   618,   808,   241,   193,   242,   191,   243,
     199,   821,   197,   804,   829,   830,   831,   832,   164,   165,
     166,   244,   245,   246,   678,   202,   247,   387,   248,   201,
     229,   230,   206,   685,   687,    17,   686,   686,   207,   231,
     208,   232,   229,   230,  -499,  -499,   403,   404,   405,   406,
     407,   231,   210,   232,  1102,   613,   614,   233,   234,   235,
     211,   688,   708,   236,   387,   387,  1114,   215,  1118,   233,
     234,   235,   405,   406,   407,   236,   128,   129,   130,   131,
    1127,   120,   121,   122,   123,   378,   917,  1104,  1105,   237,
     238,   239,  1135,   712,   713,  1137,   455,   714,  1110,  1111,
     216,   237,   238,   239,   225,   226,   227,   228,   837,   838,
     217,   240,   225,   226,   227,   228,   220,   222,   728,   729,
     730,   731,   732,   240,   303,   733,   734,   705,   716,   304,
     305,   717,   735,   736,   737,   241,   306,   242,   801,   243,
     825,   455,   844,   387,   314,   845,   311,   241,   738,   242,
     913,   243,   929,   455,   916,   997,   315,   387,   717,   244,
     245,   246,  1004,   316,   247,   686,   248,   317,   318,   319,
     323,   244,   245,   246,  1007,   928,   247,   455,   248,   324,
     355,   356,    60,    61,    62,    63,    64,    65,   355,   231,
      66,   232,   326,   331,   336,   335,   363,   231,   340,   232,
     341,   342,   358,    68,    69,   359,    70,   233,   234,   235,
     364,   365,   366,   236,   369,   233,   234,   235,    71,    72,
     367,   236,   343,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   374,   375,  1006,   419,   408,   237,
     238,   239,   420,   421,   427,   422,   423,   237,   238,   239,
       1,   424,     2,     3,     4,     5,     6,     7,     8,     9,
      10,   240,   430,   433,   451,   434,    11,    12,    13,   240,
     441,   452,    14,    15,    16,   456,   739,   740,   741,   742,
     743,   461,   462,   744,   745,   241,   466,   242,    17,   243,
     746,   747,   748,   241,   475,   242,   480,   243,   476,   477,
     478,   479,   481,   485,   483,   487,   749,   482,   467,   244,
     245,   246,   247,   492,   247,   512,   248,   244,   245,   246,
     514,   391,   247,   392,   248,   516,   517,    17,   518,   523,
     521,   526,   522,   524,    73,    74,    75,    76,   525,    77,
      78,   529,   530,   467,    79,    80,    81,   539,   537,    82,
      83,    84,   575,   576,   580,   581,    85,    86,   583,   637,
    -242,   638,   639,   640,   641,   393,   642,   643,    87,   596,
     491,   603,    88,   605,   609,   389,   620,   616,   393,   622,
     624,   625,   394,   395,   396,   397,   467,   444,   623,   629,
     399,   632,   633,   634,   636,   394,   395,   396,   397,   398,
     393,   658,   659,   399,   674,   680,   681,   682,   660,   661,
     683,   662,   663,   664,   672,    18,   677,   394,   395,   396,
     397,   684,   621,   703,   697,   399,   679,   704,   689,   702,
     614,   707,   613,    19,   400,   401,   402,   403,   404,   405,
     406,   407,   715,   393,   710,   809,   711,   400,   401,   402,
     403,   404,   405,   406,   407,   718,   721,   722,    20,   723,
     394,   395,   396,   397,   393,   724,   725,   727,   399,   400,
     401,   402,   403,   404,   405,   406,   407,   794,   795,   803,
     802,  -499,  -499,   396,   397,   796,   806,   810,   812,  -499,
     750,   751,   752,   753,   754,   814,   819,   755,   756,   822,
     823,   826,   824,   827,   757,   758,   759,   841,   839,   855,
     842,   866,   400,   401,   402,   403,   404,   405,   406,   407,
     760,   915,   846,   847,   848,   849,   877,   850,   888,   851,
     925,   852,   853,  -499,   401,   402,   403,   404,   405,   406,
     407,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,   559,   560,   854,   561,
     562,   563,   564,   565,   566,   856,   857,   567,   858,   859,
     568,   569,   860,   861,   570,   571,   572,   573,   761,   762,
     763,   764,   765,   862,   863,   766,   767,   864,   899,   865,
     867,   910,   768,   769,   770,   772,   773,   774,   775,   776,
     868,   869,   777,   778,   701,   926,   923,   930,   771,   779,
     780,   781,   783,   784,   785,   786,   787,   870,   931,   788,
     789,   871,   932,   872,   873,   782,   790,   791,   792,   874,
     875,   933,   876,   934,   935,   878,   879,   880,   936,   881,
     882,   883,   793,   937,   884,   885,   938,   886,   939,   887,
     941,   889,   890,   891,   892,   893,   894,   895,   896,   897,
     898,   900,   901,   902,   903,   904,   942,   943,   944,   945,
     905,   906,   907,   908,   909,   946,   911,   918,   919,   921,
     947,   922,   940,   948,   949,   950,   951,   952,   953,   954,
     955,   956,   957,   958,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   969,   970,   971,   972,   973,   974,
     975,   976,   977,   978,   979,   980,   981,   982,   983,   985,
     984,   986,   987,   988,   989,   990,   991,   992,   993,   994,
     995,   996,   999,  1002,  1003,  1001,  1091,  1005,  1008,   387,
    1018,  1009,  1010,  1011,  1012,  1013,  1014,  1015,  1029,  1016,
    1017,  1019,  1020,  1040,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,  1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,
    1038,  1039,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,
    1049,  1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,
    1059,  1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,
    1069,  1070,  1071,  1072,  1073,  1074,  1093,  1075,  1085,  1077,
    1078,  1086,  1087,  1088,  1089,  1090,  1092,  1096,  1095,  1097,
    1103,  1107,  1098,  1100,  1108,  1109,  1120,  1112,  1119,  1121,
    1122,  1123,  1124,  1128,  1129,  1130,  1131,  1132,  1134,  1133,
    1136,   142,   192,   799,   676,   690,   797,   143,   144,   145,
     329,   460,   106,   320,   657,   836,   486,   146,   147,   720,
     149,   150,   151,   152,   153,   840,   312,   471,   912,   607,
     816,   805,   368,   833,     0,   817,   513,   426,   914,   515
};

static const yytype_int16 yycheck[] =
{
     220,    20,   198,   574,    67,   247,   700,   229,   462,   136,
     218,   521,   232,     3,     5,     6,   522,    68,     4,     5,
       6,     4,   244,   245,     8,     4,     3,     6,   248,     3,
       5,     6,    61,     6,    14,    84,    16,    16,    20,   537,
      22,    72,    37,   537,    84,   537,    28,    61,     0,   176,
     537,     6,    82,   537,   537,     7,    84,     9,    10,    11,
      12,    13,    14,    15,    16,    17,   576,    61,    47,     4,
      93,    23,    24,    25,    82,    83,    55,    29,    30,    31,
      81,   114,   131,   217,    38,   219,    89,   158,   159,    72,
      69,   131,    68,     3,     3,     5,     6,    46,     3,    72,
       5,     6,    78,   323,   324,    34,     3,    56,     5,     6,
      89,    90,    91,    92,     3,    33,   336,    72,   812,   168,
     106,   107,   108,    76,    77,    92,   112,    34,   168,   214,
      84,    49,    84,   355,   219,   158,   159,    72,     3,   359,
     360,   120,   121,   122,   158,   159,   342,    79,   219,   128,
     129,   130,   138,   139,   140,   158,   159,    34,    68,    36,
     161,   194,   672,    68,   158,   159,   217,    34,   410,   389,
     390,    68,   394,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   215,   215,   219,   219,
     209,   158,   159,    34,   173,     7,    61,     9,    10,    11,
      12,    13,    14,   135,    16,   158,   159,   215,   714,    92,
      82,    23,    24,    25,   194,    89,     3,    29,    30,    31,
     172,   441,   213,   214,     3,   215,   724,   213,   207,   248,
     724,   215,   724,   210,   216,     3,   210,   724,   190,   214,
     724,   724,    79,   132,     0,   134,   158,   159,     3,     4,
       5,     6,   200,   473,   474,   203,   476,   158,   159,   479,
     456,   171,   218,   215,   327,    79,   171,   463,   214,   491,
      84,   173,    84,   219,   171,   158,   159,    10,   214,    74,
      75,   177,   178,   219,   158,   159,     3,     4,     5,     6,
      85,    86,    87,    26,    27,   132,   492,   134,   135,   214,
     137,    33,    34,    35,   219,   216,  1000,   219,   219,  1003,
      33,    34,    35,    45,    46,   216,    71,     8,   132,   539,
     134,   135,    45,    46,    79,    80,    20,   214,    22,   839,
     538,   163,   219,    88,    28,    90,   124,    78,   126,   358,
     128,   129,   174,    78,   915,   177,   178,   217,   180,   181,
     182,   106,   107,   108,   190,   191,   335,   112,   485,   195,
     172,     3,    79,    80,   196,   197,   169,   170,    14,   823,
       3,    88,   175,    90,     3,    89,    93,    91,   598,    93,
     600,   603,     3,   138,   139,   140,   177,   178,   451,   106,
     107,   108,     6,   216,   189,   112,   219,  1091,   216,   621,
    1094,   219,     3,   215,   696,   160,    46,   699,   192,   193,
      50,    51,  1106,    61,  1108,    55,    56,   162,   163,   164,
     616,   138,   139,   140,     3,     4,     5,     6,    89,   184,
      91,   186,    93,   188,   158,   159,     3,     4,     5,     6,
     180,    46,   169,   160,   136,    50,    51,   466,   219,   216,
      55,    56,   219,   208,   209,   210,   698,     6,   213,   216,
     215,   216,   219,   155,   156,    52,    53,   184,   531,   186,
       4,   188,   216,   492,   694,   184,    21,   186,   216,   188,
      73,   701,    71,   679,     3,     4,     5,     6,   199,   200,
     201,   208,   209,   210,   216,     3,   213,   219,   215,    79,
      79,    80,     3,   216,   216,    84,   219,   219,    68,    88,
     215,    90,    79,    80,   206,   207,   208,   209,   210,   211,
     212,    88,    80,    90,  1095,     5,     6,   106,   107,   108,
       3,   216,   216,   112,   219,   219,  1107,     3,  1109,   106,
     107,   108,   210,   211,   212,   112,    49,    50,    51,    52,
    1121,    43,    44,    45,    46,     5,     6,    52,    53,   138,
     139,   140,  1133,   216,   216,  1136,   219,   219,    50,    51,
       3,   138,   139,   140,     3,     4,     5,     6,   721,   722,
       3,   160,     3,     4,     5,     6,    72,     4,    95,    96,
      97,    98,    99,   160,     3,   102,   103,   616,   216,     3,
       6,   219,   109,   110,   111,   184,     4,   186,   216,   188,
     216,   219,   216,   219,   168,   219,   215,   184,   125,   186,
     216,   188,   842,   219,   216,   216,     6,   219,   219,   208,
     209,   210,   216,     3,   213,   219,   215,     6,     4,    61,
      59,   208,   209,   210,   216,   841,   213,   219,   215,    74,
      79,    80,    39,    40,    41,    42,    43,    44,    79,    88,
      47,    90,    80,     3,   205,    68,     4,    88,    84,    90,
      84,   215,   215,    33,    34,   215,    36,   106,   107,   108,
       4,     4,     4,   112,   215,   106,   107,   108,    48,    49,
       6,   112,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   215,   215,   926,     3,   217,   138,
     139,   140,     6,    54,     4,    54,    83,   138,   139,   140,
       7,    84,     9,    10,    11,    12,    13,    14,    15,    16,
      17,   160,     6,   216,    61,   216,    23,    24,    25,   160,
      75,   215,    29,    30,    31,   215,    95,    96,    97,    98,
      99,   215,   215,   102,   103,   184,   215,   186,    84,   188,
     109,   110,   111,   184,     4,   186,     4,   188,   215,   215,
     215,   215,     4,    83,   216,     3,   125,   220,    79,   208,
     209,   210,   213,   215,   213,     6,   215,   208,   209,   210,
       6,    79,   213,    81,   215,     6,     5,    84,    50,     3,
     215,     6,   215,   215,   164,   165,   166,   167,   216,   169,
     170,   168,   168,    79,   174,   175,   176,   219,   183,   179,
     180,   181,   133,   215,     3,     3,   186,   187,   219,    62,
      63,    64,    65,    66,    67,   136,    69,    70,   198,   216,
     161,    82,   202,     4,   219,   158,   135,   215,   136,   220,
       6,     6,   153,   154,   155,   156,    79,     3,   220,     3,
     161,     6,     4,     4,   219,   153,   154,   155,   156,   157,
     136,   215,   215,   161,    32,     6,     6,     4,   215,   215,
       3,   215,   215,   215,   215,   172,   216,   153,   154,   155,
     156,     6,   158,     4,   219,   161,   215,    84,   216,   216,
       6,   216,     5,   190,   205,   206,   207,   208,   209,   210,
     211,   212,   205,   136,    57,   216,    54,   205,   206,   207,
     208,   209,   210,   211,   212,     6,    63,    63,   215,     3,
     153,   154,   155,   156,   136,   219,    58,     6,   161,   205,
     206,   207,   208,   209,   210,   211,   212,   135,   133,   168,
     215,   153,   154,   155,   156,   137,   216,   213,     4,   161,
      95,    96,    97,    98,    99,   213,   219,   102,   103,   216,
     215,     6,   216,     6,   109,   110,   111,    59,   215,    97,
      60,    97,   205,   206,   207,   208,   209,   210,   211,   212,
     125,     3,   219,   219,   219,   219,    97,   219,    97,   219,
       6,   219,   219,   205,   206,   207,   208,   209,   210,   211,
     212,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   219,   112,
     113,   114,   115,   116,   117,   219,   219,   120,   219,   219,
     123,   124,   219,   219,   127,   128,   129,   130,    95,    96,
      97,    98,    99,   219,   219,   102,   103,   219,    97,   219,
     219,    97,   109,   110,   111,    95,    96,    97,    98,    99,
     219,   219,   102,   103,    72,    68,   216,     6,   125,   109,
     110,   111,    95,    96,    97,    98,    99,   219,     6,   102,
     103,   219,     6,   219,   219,   125,   109,   110,   111,   219,
     219,     6,   219,     6,     6,   219,   219,   219,     6,   219,
     219,   219,   125,     6,   219,   219,     6,   219,     6,   219,
       6,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,     6,     6,     6,     6,
     219,   219,   219,   219,   219,     6,   219,   219,   219,   219,
       6,   219,   219,     6,     6,     6,   219,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   219,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   219,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     219,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     219,     6,     4,     4,     4,   216,     4,     6,   216,   219,
       6,   216,   216,   216,   216,   216,   216,   216,     6,   216,
     216,   216,   216,     6,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,     6,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,     6,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,     6,   216,     4,   219,   216,   219,
     219,   216,   216,   216,   216,   216,   216,     6,   216,     6,
       6,   216,   219,   219,     4,   216,    55,     6,     6,   216,
     216,    46,    46,   215,    46,    46,     3,   215,     3,   216,
     216,    67,   109,   671,   578,   597,   669,    67,    67,    67,
     205,   337,    20,   194,   539,   719,   387,    67,    67,   636,
      67,    67,    67,    67,    67,   724,   175,   361,   796,   478,
     699,   686,   239,   717,    -1,   699,   412,   311,   802,   414
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
      51,    55,    56,     3,     3,     6,     4,    10,    26,    27,
     270,   215,   274,   327,   168,     6,     3,     6,     4,    61,
     251,   252,   284,    59,    74,   257,    80,    61,   215,   239,
     270,     3,   236,    38,   249,    68,   205,   219,   263,   287,
      84,    84,   215,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,    79,    80,   288,   215,   215,
      93,   287,   302,     4,     4,     4,     4,     6,   324,   215,
     124,   126,   128,   129,   215,   215,   288,   288,     5,     6,
     214,   307,   317,   318,   249,   287,   216,   219,    61,   158,
     159,    79,    81,   136,   153,   154,   155,   156,   157,   161,
     205,   206,   207,   208,   209,   210,   211,   212,   217,   214,
     219,   214,   219,   214,   219,   214,   219,   214,   219,     3,
       6,    54,    54,    83,    84,   332,   250,     4,    46,    56,
       6,   192,   193,   216,   216,    82,   260,   254,   255,   287,
     287,    75,   258,   247,     3,   132,   134,   227,   229,   230,
     235,    61,   215,   336,   216,   219,   215,   285,   270,   287,
     244,   215,   215,    71,   216,   284,   215,    79,   249,   287,
     287,   302,    89,    91,    93,     4,   215,   215,   215,   215,
       4,     4,   220,   216,   216,    83,   286,     3,   287,   287,
      81,   161,   215,    79,   135,   288,   288,   288,   288,   288,
     288,   288,   288,   288,   288,   288,   288,   288,   288,     3,
     210,   307,     6,   317,     6,   318,     6,     5,    50,    52,
      53,   215,   215,     3,   215,   216,     6,    33,    49,   168,
     168,   215,   267,   268,   269,   270,   275,   183,   261,   219,
      76,    77,   256,   287,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   112,   113,   114,   115,   116,   117,   120,   123,   124,
     127,   128,   129,   130,   231,   133,   215,   216,   219,   247,
       3,     3,   284,   219,    74,    75,    85,    86,    87,   189,
     325,   326,   325,   284,   216,   249,   216,    61,    92,    89,
      91,   287,   287,    82,   287,     4,     3,   305,   287,   219,
     262,   216,   219,     5,     6,   327,   215,   288,   249,   284,
     135,   158,   220,   220,     6,     6,   236,   228,   230,     3,
     334,   335,     6,     4,     4,   247,   219,    62,    64,    65,
      66,    67,    69,    70,   276,     3,    61,   271,   289,   290,
     291,   292,   293,   294,   295,   296,   263,   255,   215,   215,
     215,   215,   215,   215,   215,    79,   132,   134,   135,   232,
     233,   332,   215,   236,    32,   333,   229,   216,   216,   215,
       6,     6,     4,     3,     6,   216,   219,   216,   216,   216,
     231,   287,   287,    89,    92,   288,   219,   219,   219,   219,
       4,    72,   216,     4,    84,   249,   284,   216,   216,   288,
      57,    54,   216,   216,   219,   205,   216,   219,     6,   216,
     268,    63,    63,     3,   219,    58,   265,     6,    95,    96,
      97,    98,    99,   102,   103,   109,   110,   111,   125,    95,
      96,    97,    98,    99,   102,   103,   109,   110,   111,   125,
      95,    96,    97,    98,    99,   102,   103,   109,   110,   111,
     125,    95,    96,    97,    98,    99,   102,   103,   109,   110,
     111,   125,    95,    96,    97,    98,    99,   102,   103,   109,
     110,   111,   125,    95,    96,    97,    98,    99,   102,   103,
     109,   110,   111,   125,   135,   133,   137,   233,   234,   234,
     236,   216,   215,   168,   284,   326,   216,    89,   287,   216,
     213,   319,     4,   307,   213,   308,   311,   316,   319,   219,
     262,   287,   216,   215,   216,   216,     6,     6,   230,     3,
       4,     5,     6,   335,    34,    36,   271,   269,   269,   215,
     295,    59,    60,   264,   216,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,    97,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,    97,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,    97,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,    97,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,    97,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
      97,   219,   306,   216,   334,     3,   216,     6,   219,   219,
     262,   219,   219,   216,   325,     6,    68,   236,   284,   287,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     219,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   219,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   219,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   219,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   219,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   219,     6,   216,   332,     4,
       4,   216,     4,     4,   216,     6,   287,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,     6,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,     6,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
       6,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,     6,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,     6,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,     6,   216,   219,   262,   219,   219,   262,
      46,    50,    51,    55,    56,   216,   216,   216,   216,   216,
     216,     4,   216,     4,     6,   216,     6,     6,   219,   262,
     219,   262,   332,     6,    52,    53,     6,   216,     4,   216,
      50,    51,     6,   262,   332,   262,   131,   168,   332,     6,
      55,   216,   216,    46,    46,   131,   168,   332,   215,    46,
      46,     3,   215,   216,     3,   332,   216,   332
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
     282,   282,   282,   282,   282,   282,   283,   283,   283,   284,
     284,   285,   285,   286,   286,   287,   287,   287,   287,   287,
     288,   288,   288,   288,   288,   288,   288,   288,   288,   288,
     288,   288,   288,   289,   290,   290,   290,   290,   291,   291,
     291,   291,   292,   292,   293,   293,   294,   294,   295,   295,
     295,   295,   295,   295,   296,   296,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   297,
     297,   297,   297,   297,   297,   297,   297,   297,   297,   298,
     298,   299,   300,   300,   301,   301,   301,   301,   302,   302,
     303,   304,   304,   304,   304,   305,   305,   305,   305,   306,
     306,   306,   306,   306,   306,   306,   306,   306,   306,   306,
     306,   307,   307,   307,   307,   308,   308,   308,   309,   310,
     310,   311,   311,   312,   313,   313,   314,   315,   315,   316,
     317,   318,   319,   319,   320,   321,   321,   322,   323,   323,
     324,   324,   324,   324,   324,   324,   324,   324,   324,   324,
     324,   324,   325,   325,   326,   326,   326,   326,   326,   326,
     327,   328,   328,   329,   329,   330,   330,   331,   331,   332,
     332,   333,   333,   334,   334,   335,   335,   335,   335,   335,
     336,   336
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
       3,     3,     3,     5,     7,     7,     6,     8,     8,     1,
       3,     3,     5,     3,     1,     1,     1,     1,     1,     1,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,    14,    20,    16,    15,    13,    18,    14,
      13,    11,     8,    10,     5,     7,     4,     6,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     4,     5,     4,
       3,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     6,     3,     4,     3,
       3,     5,     5,     6,     4,     6,     3,     5,     4,     5,
       6,     4,     5,     5,     6,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     2,     2,     2,     2,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       3,     1,     1,     2,     2,     3,     2,     2,     3,     2,
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
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2377 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2385 "parser.cpp"
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
#line 2399 "parser.cpp"
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
#line 2413 "parser.cpp"
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
#line 2427 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2446 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2455 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2464 "parser.cpp"
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
#line 2478 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2489 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2509 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2519 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2529 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2549 "parser.cpp"
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
#line 2563 "parser.cpp"
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
#line 2577 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2587 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2595 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2603 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2612 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2620 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2628 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2636 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2644 "parser.cpp"
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
#line 2658 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2667 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2685 "parser.cpp"
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
#line 2698 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2707 "parser.cpp"
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
#line 2721 "parser.cpp"
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
#line 2735 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2745 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2754 "parser.cpp"
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
#line 2768 "parser.cpp"
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
#line 2785 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2793 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2801 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2809 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2817 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2825 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2833 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2841 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2849 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2857 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2865 "parser.cpp"
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
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2887 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2895 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2911 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2919 "parser.cpp"
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
#line 2932 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2940 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2948 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2956 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2964 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2972 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2980 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2988 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2996 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3004 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3012 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3020 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3028 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3036 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3052 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3060 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3068 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3076 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3084 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3100 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3108 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3116 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3127 "parser.cpp"
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
#line 3141 "parser.cpp"
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
#line 3155 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3166 "parser.cpp"
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

#line 3274 "parser.cpp"

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
#line 3489 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 511 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3500 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 517 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3511 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 524 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3517 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3523 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3529 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3535 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3541 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3547 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3553 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3559 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 532 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3565 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 533 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3571 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 534 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3577 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3583 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3589 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3595 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 538 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3601 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 540 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3607 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3613 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3619 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 543 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3625 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3631 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3637 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3643 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 547 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3649 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 548 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3655 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 549 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3661 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3667 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 551 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3673 "parser.cpp"
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
#line 3693 "parser.cpp"
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
#line 3711 "parser.cpp"
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
#line 3744 "parser.cpp"
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
#line 3764 "parser.cpp"
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
#line 3785 "parser.cpp"
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
#line 3818 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 683 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3827 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 687 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3836 "parser.cpp"
    break;

  case 40: /* column_def_array: table_column  */
#line 692 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3845 "parser.cpp"
    break;

  case 41: /* column_def_array: column_def_array ',' table_column  */
#line 696 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3854 "parser.cpp"
    break;

  case 42: /* table_element: table_column  */
#line 702 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3862 "parser.cpp"
    break;

  case 43: /* table_element: table_constraint  */
#line 705 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3870 "parser.cpp"
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
#line 3926 "parser.cpp"
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
#line 3968 "parser.cpp"
    break;

  case 46: /* column_type: BOOLEAN  */
#line 802 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3974 "parser.cpp"
    break;

  case 47: /* column_type: TINYINT  */
#line 803 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3980 "parser.cpp"
    break;

  case 48: /* column_type: SMALLINT  */
#line 804 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3986 "parser.cpp"
    break;

  case 49: /* column_type: INTEGER  */
#line 805 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3992 "parser.cpp"
    break;

  case 50: /* column_type: INT  */
#line 806 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3998 "parser.cpp"
    break;

  case 51: /* column_type: BIGINT  */
#line 807 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4004 "parser.cpp"
    break;

  case 52: /* column_type: HUGEINT  */
#line 808 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4010 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT  */
#line 809 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4016 "parser.cpp"
    break;

  case 54: /* column_type: REAL  */
#line 810 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4022 "parser.cpp"
    break;

  case 55: /* column_type: DOUBLE  */
#line 811 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4028 "parser.cpp"
    break;

  case 56: /* column_type: FLOAT16  */
#line 812 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4034 "parser.cpp"
    break;

  case 57: /* column_type: BFLOAT16  */
#line 813 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4040 "parser.cpp"
    break;

  case 58: /* column_type: DATE  */
#line 814 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4046 "parser.cpp"
    break;

  case 59: /* column_type: TIME  */
#line 815 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4052 "parser.cpp"
    break;

  case 60: /* column_type: DATETIME  */
#line 816 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4058 "parser.cpp"
    break;

  case 61: /* column_type: TIMESTAMP  */
#line 817 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4064 "parser.cpp"
    break;

  case 62: /* column_type: UUID  */
#line 818 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4070 "parser.cpp"
    break;

  case 63: /* column_type: POINT  */
#line 819 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4076 "parser.cpp"
    break;

  case 64: /* column_type: LINE  */
#line 820 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4082 "parser.cpp"
    break;

  case 65: /* column_type: LSEG  */
#line 821 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4088 "parser.cpp"
    break;

  case 66: /* column_type: BOX  */
#line 822 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4094 "parser.cpp"
    break;

  case 67: /* column_type: CIRCLE  */
#line 825 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4100 "parser.cpp"
    break;

  case 68: /* column_type: VARCHAR  */
#line 827 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4106 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4112 "parser.cpp"
    break;

  case 70: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 829 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4118 "parser.cpp"
    break;

  case 71: /* column_type: DECIMAL  */
#line 830 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4124 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4130 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4136 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4142 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4148 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4154 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4160 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4166 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4172 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4178 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4184 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4190 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4196 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4202 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4208 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4214 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4220 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4226 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4232 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4238 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4244 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4250 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4256 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4262 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4268 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4274 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4280 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4286 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4292 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4298 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4304 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4310 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4316 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4322 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4328 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4334 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4340 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4346 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4352 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4358 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4364 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4370 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4376 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4382 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4388 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4394 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 878 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4400 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 879 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4406 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 880 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4412 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 881 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4418 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 882 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4424 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 883 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4430 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 884 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4436 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 885 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4442 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 886 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4448 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 887 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4454 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 888 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4460 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 889 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4466 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 890 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4472 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 891 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4478 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 892 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4484 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 893 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4490 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 894 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4496 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 895 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4502 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 896 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4508 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 897 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4514 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 898 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4520 "parser.cpp"
    break;

  case 138: /* column_constraints: column_constraint  */
#line 917 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4529 "parser.cpp"
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
#line 4543 "parser.cpp"
    break;

  case 140: /* column_constraint: PRIMARY KEY  */
#line 931 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4551 "parser.cpp"
    break;

  case 141: /* column_constraint: UNIQUE  */
#line 934 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4559 "parser.cpp"
    break;

  case 142: /* column_constraint: NULLABLE  */
#line 937 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4567 "parser.cpp"
    break;

  case 143: /* column_constraint: NOT NULLABLE  */
#line 940 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4575 "parser.cpp"
    break;

  case 144: /* default_expr: DEFAULT constant_expr  */
#line 944 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4583 "parser.cpp"
    break;

  case 145: /* default_expr: %empty  */
#line 947 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4591 "parser.cpp"
    break;

  case 146: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 952 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4601 "parser.cpp"
    break;

  case 147: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 957 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4611 "parser.cpp"
    break;

  case 148: /* identifier_array: IDENTIFIER  */
#line 964 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4622 "parser.cpp"
    break;

  case 149: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 970 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4633 "parser.cpp"
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
#line 4650 "parser.cpp"
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
#line 4689 "parser.cpp"
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
#line 4706 "parser.cpp"
    break;

  case 153: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1043 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4714 "parser.cpp"
    break;

  case 154: /* optional_identifier_array: %empty  */
#line 1046 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4722 "parser.cpp"
    break;

  case 155: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1053 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4732 "parser.cpp"
    break;

  case 156: /* explain_type: ANALYZE  */
#line 1059 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4740 "parser.cpp"
    break;

  case 157: /* explain_type: AST  */
#line 1062 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4748 "parser.cpp"
    break;

  case 158: /* explain_type: RAW  */
#line 1065 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4756 "parser.cpp"
    break;

  case 159: /* explain_type: LOGICAL  */
#line 1068 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4764 "parser.cpp"
    break;

  case 160: /* explain_type: PHYSICAL  */
#line 1071 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4772 "parser.cpp"
    break;

  case 161: /* explain_type: PIPELINE  */
#line 1074 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4780 "parser.cpp"
    break;

  case 162: /* explain_type: FRAGMENT  */
#line 1077 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4788 "parser.cpp"
    break;

  case 163: /* explain_type: %empty  */
#line 1080 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4796 "parser.cpp"
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
#line 4813 "parser.cpp"
    break;

  case 165: /* update_expr_array: update_expr  */
#line 1100 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4822 "parser.cpp"
    break;

  case 166: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1104 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4831 "parser.cpp"
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
#line 4843 "parser.cpp"
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
#line 4859 "parser.cpp"
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
#line 4877 "parser.cpp"
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
#line 4895 "parser.cpp"
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
#line 4913 "parser.cpp"
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
#line 4936 "parser.cpp"
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
#line 4994 "parser.cpp"
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
#line 5054 "parser.cpp"
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
#line 5106 "parser.cpp"
    break;

  case 176: /* select_statement: select_without_paren  */
#line 1362 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5114 "parser.cpp"
    break;

  case 177: /* select_statement: select_with_paren  */
#line 1365 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5122 "parser.cpp"
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
#line 5136 "parser.cpp"
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
#line 5150 "parser.cpp"
    break;

  case 180: /* select_with_paren: '(' select_without_paren ')'  */
#line 1387 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5158 "parser.cpp"
    break;

  case 181: /* select_with_paren: '(' select_with_paren ')'  */
#line 1390 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5166 "parser.cpp"
    break;

  case 182: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1394 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5175 "parser.cpp"
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
#line 5206 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1426 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5214 "parser.cpp"
    break;

  case 185: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1429 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5222 "parser.cpp"
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
#line 5242 "parser.cpp"
    break;

  case 187: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1450 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5250 "parser.cpp"
    break;

  case 188: /* order_by_clause: %empty  */
#line 1453 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5258 "parser.cpp"
    break;

  case 189: /* order_by_expr_list: order_by_expr  */
#line 1457 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5267 "parser.cpp"
    break;

  case 190: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1461 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5276 "parser.cpp"
    break;

  case 191: /* order_by_expr: expr order_by_type  */
#line 1466 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5286 "parser.cpp"
    break;

  case 192: /* order_by_type: ASC  */
#line 1472 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5294 "parser.cpp"
    break;

  case 193: /* order_by_type: DESC  */
#line 1475 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5302 "parser.cpp"
    break;

  case 194: /* order_by_type: %empty  */
#line 1478 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5310 "parser.cpp"
    break;

  case 195: /* limit_expr: LIMIT expr  */
#line 1482 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5318 "parser.cpp"
    break;

  case 196: /* limit_expr: %empty  */
#line 1486 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5324 "parser.cpp"
    break;

  case 197: /* offset_expr: OFFSET expr  */
#line 1488 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5332 "parser.cpp"
    break;

  case 198: /* offset_expr: %empty  */
#line 1492 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5338 "parser.cpp"
    break;

  case 199: /* distinct: DISTINCT  */
#line 1494 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5346 "parser.cpp"
    break;

  case 200: /* distinct: %empty  */
#line 1497 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5354 "parser.cpp"
    break;

  case 201: /* from_clause: FROM table_reference  */
#line 1501 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5362 "parser.cpp"
    break;

  case 202: /* from_clause: %empty  */
#line 1504 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5370 "parser.cpp"
    break;

  case 203: /* search_clause: SEARCH sub_search_array  */
#line 1508 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5380 "parser.cpp"
    break;

  case 204: /* search_clause: %empty  */
#line 1513 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5388 "parser.cpp"
    break;

  case 205: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1517 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5396 "parser.cpp"
    break;

  case 206: /* optional_search_filter_expr: %empty  */
#line 1520 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5404 "parser.cpp"
    break;

  case 207: /* where_clause: WHERE expr  */
#line 1524 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5412 "parser.cpp"
    break;

  case 208: /* where_clause: %empty  */
#line 1527 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5420 "parser.cpp"
    break;

  case 209: /* having_clause: HAVING expr  */
#line 1531 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5428 "parser.cpp"
    break;

  case 210: /* having_clause: %empty  */
#line 1534 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5436 "parser.cpp"
    break;

  case 211: /* group_by_clause: GROUP BY expr_array  */
#line 1538 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5444 "parser.cpp"
    break;

  case 212: /* group_by_clause: %empty  */
#line 1541 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5452 "parser.cpp"
    break;

  case 213: /* set_operator: UNION  */
#line 1545 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5460 "parser.cpp"
    break;

  case 214: /* set_operator: UNION ALL  */
#line 1548 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5468 "parser.cpp"
    break;

  case 215: /* set_operator: INTERSECT  */
#line 1551 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5476 "parser.cpp"
    break;

  case 216: /* set_operator: EXCEPT  */
#line 1554 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5484 "parser.cpp"
    break;

  case 217: /* table_reference: table_reference_unit  */
#line 1562 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5492 "parser.cpp"
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
#line 5510 "parser.cpp"
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
#line 5528 "parser.cpp"
    break;

  case 222: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1596 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5539 "parser.cpp"
    break;

  case 223: /* table_name: IDENTIFIER  */
#line 1605 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5549 "parser.cpp"
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
#line 5561 "parser.cpp"
    break;

  case 225: /* table_alias: AS IDENTIFIER  */
#line 1619 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5571 "parser.cpp"
    break;

  case 226: /* table_alias: IDENTIFIER  */
#line 1624 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5581 "parser.cpp"
    break;

  case 227: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1629 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5592 "parser.cpp"
    break;

  case 228: /* table_alias: %empty  */
#line 1635 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5600 "parser.cpp"
    break;

  case 229: /* with_clause: WITH with_expr_list  */
#line 1642 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5608 "parser.cpp"
    break;

  case 230: /* with_clause: %empty  */
#line 1645 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5616 "parser.cpp"
    break;

  case 231: /* with_expr_list: with_expr  */
#line 1649 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5625 "parser.cpp"
    break;

  case 232: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1652 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5634 "parser.cpp"
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
#line 5646 "parser.cpp"
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
#line 5658 "parser.cpp"
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
#line 5671 "parser.cpp"
    break;

  case 236: /* join_type: INNER  */
#line 1690 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5679 "parser.cpp"
    break;

  case 237: /* join_type: LEFT  */
#line 1693 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5687 "parser.cpp"
    break;

  case 238: /* join_type: RIGHT  */
#line 1696 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5695 "parser.cpp"
    break;

  case 239: /* join_type: OUTER  */
#line 1699 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5703 "parser.cpp"
    break;

  case 240: /* join_type: FULL  */
#line 1702 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5711 "parser.cpp"
    break;

  case 241: /* join_type: CROSS  */
#line 1705 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5719 "parser.cpp"
    break;

  case 242: /* join_type: %empty  */
#line 1708 "parser.y"
                {
}
#line 5726 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASES  */
#line 1714 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5735 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLES  */
#line 1718 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5744 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW VIEWS  */
#line 1722 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5753 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW CONFIGS  */
#line 1726 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5762 "parser.cpp"
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
#line 5774 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW PROFILES  */
#line 1737 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5783 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW BUFFER  */
#line 1741 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5792 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW MEMINDEX  */
#line 1745 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5801 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW QUERIES  */
#line 1749 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5810 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1753 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5820 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TRANSACTIONS  */
#line 1758 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5829 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1762 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5839 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW SESSION VARIABLES  */
#line 1767 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5848 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1771 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5857 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1775 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5868 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1781 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5879 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1787 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5890 "parser.cpp"
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
#line 5906 "parser.cpp"
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
#line 5922 "parser.cpp"
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
#line 5938 "parser.cpp"
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
#line 5955 "parser.cpp"
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
#line 5972 "parser.cpp"
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
#line 5990 "parser.cpp"
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
#line 6009 "parser.cpp"
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
#line 6025 "parser.cpp"
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
#line 6044 "parser.cpp"
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
#line 6065 "parser.cpp"
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
#line 6087 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW LOGS  */
#line 1935 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6096 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW DELTA LOGS  */
#line 1939 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6105 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW CATALOGS  */
#line 1943 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6114 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1947 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6123 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1951 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6132 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1955 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6143 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW MEMORY  */
#line 1961 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6152 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1965 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6161 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1969 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6170 "parser.cpp"
    break;

  case 280: /* flush_statement: FLUSH DATA  */
#line 1977 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6179 "parser.cpp"
    break;

  case 281: /* flush_statement: FLUSH LOG  */
#line 1981 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6188 "parser.cpp"
    break;

  case 282: /* flush_statement: FLUSH BUFFER  */
#line 1985 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6197 "parser.cpp"
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
#line 6212 "parser.cpp"
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
#line 6236 "parser.cpp"
    break;

  case 285: /* command_statement: USE IDENTIFIER  */
#line 2027 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6247 "parser.cpp"
    break;

  case 286: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 2033 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6257 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2038 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6268 "parser.cpp"
    break;

  case 288: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2044 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6279 "parser.cpp"
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
#line 6292 "parser.cpp"
    break;

  case 290: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2058 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6303 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2064 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6314 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2070 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6325 "parser.cpp"
    break;

  case 293: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2076 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6336 "parser.cpp"
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
#line 6349 "parser.cpp"
    break;

  case 295: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2090 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6360 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2096 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6371 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2102 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6382 "parser.cpp"
    break;

  case 298: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2108 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6393 "parser.cpp"
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
#line 6406 "parser.cpp"
    break;

  case 300: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2122 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6417 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2128 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6428 "parser.cpp"
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
#line 6442 "parser.cpp"
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
#line 6456 "parser.cpp"
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
#line 6473 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2166 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6482 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2170 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6492 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2175 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6503 "parser.cpp"
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
#line 6515 "parser.cpp"
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
#line 6528 "parser.cpp"
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
#line 6542 "parser.cpp"
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
#line 6557 "parser.cpp"
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
#line 6572 "parser.cpp"
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
#line 6588 "parser.cpp"
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
#line 6604 "parser.cpp"
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
#line 6621 "parser.cpp"
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
#line 6638 "parser.cpp"
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
#line 6653 "parser.cpp"
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
#line 6669 "parser.cpp"
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
#line 6686 "parser.cpp"
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
#line 6704 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW LOGS  */
#line 2317 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6713 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2321 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6723 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2326 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6733 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2331 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6744 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2337 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6753 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2341 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6762 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2345 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6773 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW NODES  */
#line 2351 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6782 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2355 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6793 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW NODE  */
#line 2361 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6802 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SET ADMIN  */
#line 2365 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6812 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SET STANDALONE  */
#line 2370 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6822 "parser.cpp"
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
#line 6834 "parser.cpp"
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
#line 6848 "parser.cpp"
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
#line 6862 "parser.cpp"
    break;

  case 336: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2401 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6873 "parser.cpp"
    break;

  case 337: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2407 "parser.y"
                                                             {
    auto *ret = new infinity::AddColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;

    for (infinity::ColumnDef*& column_def : *(yyvsp[-1].column_def_array_t)) {
        ret->column_defs_.emplace_back(column_def);
    }
    delete (yyvsp[-1].column_def_array_t);
}
#line 6887 "parser.cpp"
    break;

  case 338: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2416 "parser.y"
                                                              {
    auto *ret = new infinity::DropColumnsStatement((yyvsp[-5].table_name_t)->schema_name_ptr_, (yyvsp[-5].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    for (std::string &column_name : *(yyvsp[-1].identifier_array_t)) {
        ret->column_names_.emplace_back(std::move(column_name));
    }
    free((yyvsp[-1].identifier_array_t));
}
#line 6900 "parser.cpp"
    break;

  case 339: /* expr_array: expr_alias  */
#line 2429 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6909 "parser.cpp"
    break;

  case 340: /* expr_array: expr_array ',' expr_alias  */
#line 2433 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6918 "parser.cpp"
    break;

  case 341: /* expr_array_list: '(' expr_array ')'  */
#line 2438 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6927 "parser.cpp"
    break;

  case 342: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2442 "parser.y"
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
#line 6953 "parser.cpp"
    break;

  case 343: /* expr_alias: expr AS IDENTIFIER  */
#line 2475 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6964 "parser.cpp"
    break;

  case 344: /* expr_alias: expr  */
#line 2481 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6972 "parser.cpp"
    break;

  case 350: /* operand: '(' expr ')'  */
#line 2491 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6980 "parser.cpp"
    break;

  case 351: /* operand: '(' select_without_paren ')'  */
#line 2494 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6991 "parser.cpp"
    break;

  case 352: /* operand: constant_expr  */
#line 2500 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6999 "parser.cpp"
    break;

  case 363: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2516 "parser.y"
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
#line 7019 "parser.cpp"
    break;

  case 364: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2534 "parser.y"
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
#line 7069 "parser.cpp"
    break;

  case 365: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2580 "parser.y"
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
#line 7112 "parser.cpp"
    break;

  case 366: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2619 "parser.y"
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
#line 7159 "parser.cpp"
    break;

  case 367: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2662 "parser.y"
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
#line 7207 "parser.cpp"
    break;

  case 368: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2709 "parser.y"
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
#line 7235 "parser.cpp"
    break;

  case 369: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2733 "parser.y"
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
#line 7262 "parser.cpp"
    break;

  case 370: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2756 "parser.y"
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
#line 7287 "parser.cpp"
    break;

  case 371: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2777 "parser.y"
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
#line 7312 "parser.cpp"
    break;

  case 372: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2798 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7326 "parser.cpp"
    break;

  case 373: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2807 "parser.y"
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
#line 7342 "parser.cpp"
    break;

  case 374: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2819 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7354 "parser.cpp"
    break;

  case 375: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2826 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7368 "parser.cpp"
    break;

  case 376: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2836 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7379 "parser.cpp"
    break;

  case 377: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2842 "parser.y"
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
#line 7395 "parser.cpp"
    break;

  case 378: /* sub_search: match_vector_expr  */
#line 2854 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7403 "parser.cpp"
    break;

  case 379: /* sub_search: match_text_expr  */
#line 2857 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7411 "parser.cpp"
    break;

  case 380: /* sub_search: match_tensor_expr  */
#line 2860 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7419 "parser.cpp"
    break;

  case 381: /* sub_search: match_sparse_expr  */
#line 2863 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7427 "parser.cpp"
    break;

  case 382: /* sub_search: query_expr  */
#line 2866 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7435 "parser.cpp"
    break;

  case 383: /* sub_search: fusion_expr  */
#line 2869 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7443 "parser.cpp"
    break;

  case 384: /* sub_search_array: sub_search  */
#line 2873 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7452 "parser.cpp"
    break;

  case 385: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2877 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7461 "parser.cpp"
    break;

  case 386: /* function_expr: IDENTIFIER '(' ')'  */
#line 2882 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7474 "parser.cpp"
    break;

  case 387: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2890 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7487 "parser.cpp"
    break;

  case 388: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2898 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7501 "parser.cpp"
    break;

  case 389: /* function_expr: operand IS NOT NULLABLE  */
#line 2907 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7513 "parser.cpp"
    break;

  case 390: /* function_expr: operand IS NULLABLE  */
#line 2914 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7525 "parser.cpp"
    break;

  case 391: /* function_expr: NOT operand  */
#line 2921 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7537 "parser.cpp"
    break;

  case 392: /* function_expr: '-' operand  */
#line 2928 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7549 "parser.cpp"
    break;

  case 393: /* function_expr: '+' operand  */
#line 2935 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7561 "parser.cpp"
    break;

  case 394: /* function_expr: operand '-' operand  */
#line 2942 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7574 "parser.cpp"
    break;

  case 395: /* function_expr: operand '+' operand  */
#line 2950 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7587 "parser.cpp"
    break;

  case 396: /* function_expr: operand '*' operand  */
#line 2958 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7600 "parser.cpp"
    break;

  case 397: /* function_expr: operand '/' operand  */
#line 2966 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7613 "parser.cpp"
    break;

  case 398: /* function_expr: operand '%' operand  */
#line 2974 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7626 "parser.cpp"
    break;

  case 399: /* function_expr: operand '=' operand  */
#line 2982 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7639 "parser.cpp"
    break;

  case 400: /* function_expr: operand EQUAL operand  */
#line 2990 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7652 "parser.cpp"
    break;

  case 401: /* function_expr: operand NOT_EQ operand  */
#line 2998 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7665 "parser.cpp"
    break;

  case 402: /* function_expr: operand '<' operand  */
#line 3006 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7678 "parser.cpp"
    break;

  case 403: /* function_expr: operand '>' operand  */
#line 3014 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7691 "parser.cpp"
    break;

  case 404: /* function_expr: operand LESS_EQ operand  */
#line 3022 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7704 "parser.cpp"
    break;

  case 405: /* function_expr: operand GREATER_EQ operand  */
#line 3030 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7717 "parser.cpp"
    break;

  case 406: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3038 "parser.y"
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
#line 7752 "parser.cpp"
    break;

  case 407: /* function_expr: operand LIKE operand  */
#line 3068 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7765 "parser.cpp"
    break;

  case 408: /* function_expr: operand NOT LIKE operand  */
#line 3076 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7778 "parser.cpp"
    break;

  case 409: /* conjunction_expr: expr AND expr  */
#line 3085 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7791 "parser.cpp"
    break;

  case 410: /* conjunction_expr: expr OR expr  */
#line 3093 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7804 "parser.cpp"
    break;

  case 411: /* between_expr: operand BETWEEN operand AND operand  */
#line 3102 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7816 "parser.cpp"
    break;

  case 412: /* in_expr: operand IN '(' expr_array ')'  */
#line 3110 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7827 "parser.cpp"
    break;

  case 413: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3116 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7838 "parser.cpp"
    break;

  case 414: /* case_expr: CASE expr case_check_array END  */
#line 3123 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7849 "parser.cpp"
    break;

  case 415: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3129 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7861 "parser.cpp"
    break;

  case 416: /* case_expr: CASE case_check_array END  */
#line 3136 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7871 "parser.cpp"
    break;

  case 417: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3141 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7882 "parser.cpp"
    break;

  case 418: /* case_check_array: WHEN expr THEN expr  */
#line 3148 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7894 "parser.cpp"
    break;

  case 419: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3155 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7906 "parser.cpp"
    break;

  case 420: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3163 "parser.y"
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
#line 7937 "parser.cpp"
    break;

  case 421: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3190 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7948 "parser.cpp"
    break;

  case 422: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3196 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7959 "parser.cpp"
    break;

  case 423: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3202 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7971 "parser.cpp"
    break;

  case 424: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3209 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7983 "parser.cpp"
    break;

  case 425: /* column_expr: IDENTIFIER  */
#line 3217 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7995 "parser.cpp"
    break;

  case 426: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3224 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8007 "parser.cpp"
    break;

  case 427: /* column_expr: '*'  */
#line 3231 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8017 "parser.cpp"
    break;

  case 428: /* column_expr: column_expr '.' '*'  */
#line 3236 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8031 "parser.cpp"
    break;

  case 429: /* constant_expr: STRING  */
#line 3246 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8041 "parser.cpp"
    break;

  case 430: /* constant_expr: TRUE  */
#line 3251 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8051 "parser.cpp"
    break;

  case 431: /* constant_expr: FALSE  */
#line 3256 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8061 "parser.cpp"
    break;

  case 432: /* constant_expr: DOUBLE_VALUE  */
#line 3261 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8071 "parser.cpp"
    break;

  case 433: /* constant_expr: LONG_VALUE  */
#line 3266 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8081 "parser.cpp"
    break;

  case 434: /* constant_expr: DATE STRING  */
#line 3271 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8091 "parser.cpp"
    break;

  case 435: /* constant_expr: TIME STRING  */
#line 3276 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8101 "parser.cpp"
    break;

  case 436: /* constant_expr: DATETIME STRING  */
#line 3281 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8111 "parser.cpp"
    break;

  case 437: /* constant_expr: TIMESTAMP STRING  */
#line 3286 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8121 "parser.cpp"
    break;

  case 438: /* constant_expr: INTERVAL interval_expr  */
#line 3291 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8129 "parser.cpp"
    break;

  case 439: /* constant_expr: interval_expr  */
#line 3294 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8137 "parser.cpp"
    break;

  case 440: /* constant_expr: common_array_expr  */
#line 3297 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8145 "parser.cpp"
    break;

  case 441: /* common_array_expr: array_expr  */
#line 3301 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8153 "parser.cpp"
    break;

  case 442: /* common_array_expr: subarray_array_expr  */
#line 3304 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8161 "parser.cpp"
    break;

  case 443: /* common_array_expr: sparse_array_expr  */
#line 3307 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8169 "parser.cpp"
    break;

  case 444: /* common_array_expr: empty_array_expr  */
#line 3310 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8177 "parser.cpp"
    break;

  case 445: /* common_sparse_array_expr: sparse_array_expr  */
#line 3314 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8185 "parser.cpp"
    break;

  case 446: /* common_sparse_array_expr: array_expr  */
#line 3317 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8193 "parser.cpp"
    break;

  case 447: /* common_sparse_array_expr: empty_array_expr  */
#line 3320 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8201 "parser.cpp"
    break;

  case 448: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3324 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8209 "parser.cpp"
    break;

  case 449: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3328 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8219 "parser.cpp"
    break;

  case 450: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3333 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8228 "parser.cpp"
    break;

  case 451: /* sparse_array_expr: long_sparse_array_expr  */
#line 3338 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8236 "parser.cpp"
    break;

  case 452: /* sparse_array_expr: double_sparse_array_expr  */
#line 3341 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8244 "parser.cpp"
    break;

  case 453: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3345 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8252 "parser.cpp"
    break;

  case 454: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3349 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8264 "parser.cpp"
    break;

  case 455: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3356 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8275 "parser.cpp"
    break;

  case 456: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3363 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8283 "parser.cpp"
    break;

  case 457: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3367 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8295 "parser.cpp"
    break;

  case 458: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3374 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8306 "parser.cpp"
    break;

  case 459: /* empty_array_expr: '[' ']'  */
#line 3381 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8314 "parser.cpp"
    break;

  case 460: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3385 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8322 "parser.cpp"
    break;

  case 461: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3389 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8330 "parser.cpp"
    break;

  case 462: /* array_expr: long_array_expr  */
#line 3393 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8338 "parser.cpp"
    break;

  case 463: /* array_expr: double_array_expr  */
#line 3396 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8346 "parser.cpp"
    break;

  case 464: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3400 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8354 "parser.cpp"
    break;

  case 465: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3404 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8364 "parser.cpp"
    break;

  case 466: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3409 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8373 "parser.cpp"
    break;

  case 467: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3414 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8381 "parser.cpp"
    break;

  case 468: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3418 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8391 "parser.cpp"
    break;

  case 469: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3423 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8400 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE SECONDS  */
#line 3428 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8411 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE SECOND  */
#line 3434 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8422 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE MINUTES  */
#line 3440 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8433 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE MINUTE  */
#line 3446 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8444 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE HOURS  */
#line 3452 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8455 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE HOUR  */
#line 3458 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8466 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE DAYS  */
#line 3464 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8477 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE DAY  */
#line 3470 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8488 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE MONTHS  */
#line 3476 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8499 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE MONTH  */
#line 3482 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8510 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE YEARS  */
#line 3488 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8521 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE YEAR  */
#line 3494 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8532 "parser.cpp"
    break;

  case 482: /* copy_option_list: copy_option  */
#line 3505 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8541 "parser.cpp"
    break;

  case 483: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3509 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8550 "parser.cpp"
    break;

  case 484: /* copy_option: FORMAT IDENTIFIER  */
#line 3514 "parser.y"
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
#line 8586 "parser.cpp"
    break;

  case 485: /* copy_option: DELIMITER STRING  */
#line 3545 "parser.y"
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
#line 8601 "parser.cpp"
    break;

  case 486: /* copy_option: HEADER  */
#line 3555 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8611 "parser.cpp"
    break;

  case 487: /* copy_option: OFFSET LONG_VALUE  */
#line 3560 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8621 "parser.cpp"
    break;

  case 488: /* copy_option: LIMIT LONG_VALUE  */
#line 3565 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8631 "parser.cpp"
    break;

  case 489: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3570 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8641 "parser.cpp"
    break;

  case 490: /* file_path: STRING  */
#line 3576 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8649 "parser.cpp"
    break;

  case 491: /* if_exists: IF EXISTS  */
#line 3580 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8655 "parser.cpp"
    break;

  case 492: /* if_exists: %empty  */
#line 3581 "parser.y"
  { (yyval.bool_value) = false; }
#line 8661 "parser.cpp"
    break;

  case 493: /* if_not_exists: IF NOT EXISTS  */
#line 3583 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8667 "parser.cpp"
    break;

  case 494: /* if_not_exists: %empty  */
#line 3584 "parser.y"
  { (yyval.bool_value) = false; }
#line 8673 "parser.cpp"
    break;

  case 497: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3599 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8686 "parser.cpp"
    break;

  case 498: /* if_not_exists_info: %empty  */
#line 3607 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8694 "parser.cpp"
    break;

  case 499: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3611 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8702 "parser.cpp"
    break;

  case 500: /* with_index_param_list: %empty  */
#line 3614 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8710 "parser.cpp"
    break;

  case 501: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3618 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8718 "parser.cpp"
    break;

  case 502: /* optional_table_properties_list: %empty  */
#line 3621 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8726 "parser.cpp"
    break;

  case 503: /* index_param_list: index_param  */
#line 3625 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8735 "parser.cpp"
    break;

  case 504: /* index_param_list: index_param_list ',' index_param  */
#line 3629 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8744 "parser.cpp"
    break;

  case 505: /* index_param: IDENTIFIER  */
#line 3634 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8755 "parser.cpp"
    break;

  case 506: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3640 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8770 "parser.cpp"
    break;

  case 507: /* index_param: IDENTIFIER '=' STRING  */
#line 3650 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8785 "parser.cpp"
    break;

  case 508: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3660 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8798 "parser.cpp"
    break;

  case 509: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3668 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8811 "parser.cpp"
    break;

  case 510: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3679 "parser.y"
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
#line 8847 "parser.cpp"
    break;

  case 511: /* index_info: '(' IDENTIFIER ')'  */
#line 3710 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8858 "parser.cpp"
    break;


#line 8862 "parser.cpp"

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

#line 3717 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

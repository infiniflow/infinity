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
  YYSYMBOL_EXCEPT = 26,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 27,                     /* FLUSH  */
  YYSYMBOL_USE = 28,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 29,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 30,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 31,                  /* DATABASE  */
  YYSYMBOL_TABLE = 32,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 33,                /* COLLECTION  */
  YYSYMBOL_TABLES = 34,                    /* TABLES  */
  YYSYMBOL_INTO = 35,                      /* INTO  */
  YYSYMBOL_VALUES = 36,                    /* VALUES  */
  YYSYMBOL_AST = 37,                       /* AST  */
  YYSYMBOL_PIPELINE = 38,                  /* PIPELINE  */
  YYSYMBOL_RAW = 39,                       /* RAW  */
  YYSYMBOL_LOGICAL = 40,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 41,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 42,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 43,                      /* VIEW  */
  YYSYMBOL_INDEX = 44,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 45,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 46,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 47,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 48,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 49,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 50,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 51,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 52,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 53,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 54,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 55,                     /* CHUNK  */
  YYSYMBOL_GROUP = 56,                     /* GROUP  */
  YYSYMBOL_BY = 57,                        /* BY  */
  YYSYMBOL_HAVING = 58,                    /* HAVING  */
  YYSYMBOL_AS = 59,                        /* AS  */
  YYSYMBOL_NATURAL = 60,                   /* NATURAL  */
  YYSYMBOL_JOIN = 61,                      /* JOIN  */
  YYSYMBOL_LEFT = 62,                      /* LEFT  */
  YYSYMBOL_RIGHT = 63,                     /* RIGHT  */
  YYSYMBOL_OUTER = 64,                     /* OUTER  */
  YYSYMBOL_FULL = 65,                      /* FULL  */
  YYSYMBOL_ON = 66,                        /* ON  */
  YYSYMBOL_INNER = 67,                     /* INNER  */
  YYSYMBOL_CROSS = 68,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 69,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 70,                     /* WHERE  */
  YYSYMBOL_ORDER = 71,                     /* ORDER  */
  YYSYMBOL_LIMIT = 72,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 73,                    /* OFFSET  */
  YYSYMBOL_ASC = 74,                       /* ASC  */
  YYSYMBOL_DESC = 75,                      /* DESC  */
  YYSYMBOL_IF = 76,                        /* IF  */
  YYSYMBOL_NOT = 77,                       /* NOT  */
  YYSYMBOL_EXISTS = 78,                    /* EXISTS  */
  YYSYMBOL_IN = 79,                        /* IN  */
  YYSYMBOL_FROM = 80,                      /* FROM  */
  YYSYMBOL_TO = 81,                        /* TO  */
  YYSYMBOL_WITH = 82,                      /* WITH  */
  YYSYMBOL_DELIMITER = 83,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 84,                    /* FORMAT  */
  YYSYMBOL_HEADER = 85,                    /* HEADER  */
  YYSYMBOL_CAST = 86,                      /* CAST  */
  YYSYMBOL_END = 87,                       /* END  */
  YYSYMBOL_CASE = 88,                      /* CASE  */
  YYSYMBOL_ELSE = 89,                      /* ELSE  */
  YYSYMBOL_THEN = 90,                      /* THEN  */
  YYSYMBOL_WHEN = 91,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 92,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 93,                   /* INTEGER  */
  YYSYMBOL_INT = 94,                       /* INT  */
  YYSYMBOL_TINYINT = 95,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 96,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 97,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 98,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 99,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 100,                    /* FLOAT  */
  YYSYMBOL_DOUBLE = 101,                   /* DOUBLE  */
  YYSYMBOL_REAL = 102,                     /* REAL  */
  YYSYMBOL_DECIMAL = 103,                  /* DECIMAL  */
  YYSYMBOL_DATE = 104,                     /* DATE  */
  YYSYMBOL_TIME = 105,                     /* TIME  */
  YYSYMBOL_DATETIME = 106,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 107,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 108,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 109,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 110,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 111,                     /* UUID  */
  YYSYMBOL_POINT = 112,                    /* POINT  */
  YYSYMBOL_LINE = 113,                     /* LINE  */
  YYSYMBOL_LSEG = 114,                     /* LSEG  */
  YYSYMBOL_BOX = 115,                      /* BOX  */
  YYSYMBOL_PATH = 116,                     /* PATH  */
  YYSYMBOL_POLYGON = 117,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 118,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 119,                     /* BLOB  */
  YYSYMBOL_BITMAP = 120,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 121,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 122,                   /* VECTOR  */
  YYSYMBOL_BIT = 123,                      /* BIT  */
  YYSYMBOL_TEXT = 124,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 125,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 126,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 127,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 128,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 129,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 130,                  /* PRIMARY  */
  YYSYMBOL_KEY = 131,                      /* KEY  */
  YYSYMBOL_UNIQUE = 132,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 133,                 /* NULLABLE  */
  YYSYMBOL_IS = 134,                       /* IS  */
  YYSYMBOL_DEFAULT = 135,                  /* DEFAULT  */
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
  YYSYMBOL_PROFILE = 171,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 172,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 173,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 174,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 175,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 176,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 177,                    /* DELTA  */
  YYSYMBOL_LOGS = 178,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 179,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 180,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 181,                   /* SEARCH  */
  YYSYMBOL_MATCH = 182,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 183,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 184,                    /* QUERY  */
  YYSYMBOL_QUERIES = 185,                  /* QUERIES  */
  YYSYMBOL_FUSION = 186,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 187,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 188,                    /* ADMIN  */
  YYSYMBOL_LEADER = 189,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 190,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 191,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 192,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 193,               /* STANDALONE  */
  YYSYMBOL_NODES = 194,                    /* NODES  */
  YYSYMBOL_NODE = 195,                     /* NODE  */
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
  YYSYMBOL_table_element = 226,            /* table_element  */
  YYSYMBOL_table_column = 227,             /* table_column  */
  YYSYMBOL_column_type = 228,              /* column_type  */
  YYSYMBOL_column_constraints = 229,       /* column_constraints  */
  YYSYMBOL_column_constraint = 230,        /* column_constraint  */
  YYSYMBOL_default_expr = 231,             /* default_expr  */
  YYSYMBOL_table_constraint = 232,         /* table_constraint  */
  YYSYMBOL_identifier_array = 233,         /* identifier_array  */
  YYSYMBOL_delete_statement = 234,         /* delete_statement  */
  YYSYMBOL_insert_statement = 235,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 236, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 237,        /* explain_statement  */
  YYSYMBOL_explain_type = 238,             /* explain_type  */
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
  YYSYMBOL_from_clause = 257,              /* from_clause  */
  YYSYMBOL_search_clause = 258,            /* search_clause  */
  YYSYMBOL_where_clause = 259,             /* where_clause  */
  YYSYMBOL_having_clause = 260,            /* having_clause  */
  YYSYMBOL_group_by_clause = 261,          /* group_by_clause  */
  YYSYMBOL_set_operator = 262,             /* set_operator  */
  YYSYMBOL_table_reference = 263,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 264,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 265,     /* table_reference_name  */
  YYSYMBOL_table_name = 266,               /* table_name  */
  YYSYMBOL_table_alias = 267,              /* table_alias  */
  YYSYMBOL_with_clause = 268,              /* with_clause  */
  YYSYMBOL_with_expr_list = 269,           /* with_expr_list  */
  YYSYMBOL_with_expr = 270,                /* with_expr  */
  YYSYMBOL_join_clause = 271,              /* join_clause  */
  YYSYMBOL_join_type = 272,                /* join_type  */
  YYSYMBOL_show_statement = 273,           /* show_statement  */
  YYSYMBOL_flush_statement = 274,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 275,       /* optimize_statement  */
  YYSYMBOL_command_statement = 276,        /* command_statement  */
  YYSYMBOL_compact_statement = 277,        /* compact_statement  */
  YYSYMBOL_admin_statement = 278,          /* admin_statement  */
  YYSYMBOL_expr_array = 279,               /* expr_array  */
  YYSYMBOL_expr_array_list = 280,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 281,               /* expr_alias  */
  YYSYMBOL_expr = 282,                     /* expr  */
  YYSYMBOL_operand = 283,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 284, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 285,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 286,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 287,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 288,          /* match_text_expr  */
  YYSYMBOL_query_expr = 289,               /* query_expr  */
  YYSYMBOL_fusion_expr = 290,              /* fusion_expr  */
  YYSYMBOL_sub_search = 291,               /* sub_search  */
  YYSYMBOL_sub_search_array = 292,         /* sub_search_array  */
  YYSYMBOL_function_expr = 293,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 294,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 295,             /* between_expr  */
  YYSYMBOL_in_expr = 296,                  /* in_expr  */
  YYSYMBOL_case_expr = 297,                /* case_expr  */
  YYSYMBOL_case_check_array = 298,         /* case_check_array  */
  YYSYMBOL_cast_expr = 299,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 300,            /* subquery_expr  */
  YYSYMBOL_column_expr = 301,              /* column_expr  */
  YYSYMBOL_constant_expr = 302,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 303,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 304, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 305,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 306, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 307,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 308,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 309, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 310, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 311, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 312,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 313,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 314,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 315,               /* array_expr  */
  YYSYMBOL_long_array_expr = 316,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 317, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 318,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 319, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 320,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 321,         /* copy_option_list  */
  YYSYMBOL_copy_option = 322,              /* copy_option  */
  YYSYMBOL_file_path = 323,                /* file_path  */
  YYSYMBOL_if_exists = 324,                /* if_exists  */
  YYSYMBOL_if_not_exists = 325,            /* if_not_exists  */
  YYSYMBOL_semicolon = 326,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 327,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 328,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 329, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 330,         /* index_param_list  */
  YYSYMBOL_index_param = 331,              /* index_param  */
  YYSYMBOL_index_info = 332                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 461 "parser.cpp"

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
#define YYFINAL  105
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1322

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  219
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  504
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1107

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
       0,   494,   494,   498,   504,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   544,   561,   577,   606,   622,   640,   669,   673,   679,
     682,   689,   740,   779,   780,   781,   782,   783,   784,   785,
     786,   787,   788,   789,   790,   791,   792,   793,   794,   795,
     796,   797,   798,   799,   802,   804,   805,   806,   807,   810,
     811,   812,   813,   814,   815,   816,   817,   818,   819,   820,
     821,   822,   823,   824,   825,   826,   827,   828,   829,   830,
     831,   832,   833,   834,   835,   836,   837,   838,   839,   840,
     841,   842,   843,   844,   845,   846,   847,   848,   849,   850,
     851,   852,   853,   854,   855,   856,   857,   858,   859,   860,
     861,   862,   863,   864,   865,   866,   867,   868,   869,   870,
     871,   872,   873,   874,   875,   894,   898,   908,   911,   914,
     917,   921,   924,   929,   934,   941,   947,   957,   973,  1007,
    1020,  1023,  1030,  1036,  1039,  1042,  1045,  1048,  1051,  1054,
    1057,  1064,  1077,  1081,  1086,  1099,  1112,  1127,  1142,  1157,
    1180,  1233,  1288,  1339,  1342,  1345,  1354,  1364,  1367,  1371,
    1376,  1398,  1401,  1406,  1422,  1425,  1429,  1433,  1438,  1444,
    1447,  1450,  1454,  1458,  1460,  1464,  1466,  1469,  1473,  1476,
    1480,  1485,  1489,  1492,  1496,  1499,  1503,  1506,  1510,  1513,
    1516,  1519,  1527,  1530,  1545,  1545,  1547,  1561,  1570,  1575,
    1584,  1589,  1594,  1600,  1607,  1610,  1614,  1617,  1622,  1634,
    1641,  1655,  1658,  1661,  1664,  1667,  1670,  1673,  1679,  1683,
    1687,  1691,  1695,  1702,  1706,  1710,  1714,  1718,  1723,  1727,
    1732,  1736,  1740,  1746,  1752,  1758,  1769,  1780,  1791,  1803,
    1815,  1828,  1842,  1853,  1867,  1883,  1900,  1904,  1908,  1912,
    1916,  1920,  1926,  1930,  1934,  1942,  1946,  1950,  1958,  1969,
    1992,  1998,  2003,  2009,  2015,  2023,  2029,  2035,  2041,  2047,
    2055,  2061,  2067,  2073,  2079,  2087,  2093,  2099,  2108,  2118,
    2131,  2135,  2140,  2146,  2153,  2161,  2170,  2180,  2190,  2201,
    2212,  2224,  2236,  2246,  2257,  2269,  2282,  2286,  2291,  2296,
    2302,  2306,  2310,  2316,  2320,  2326,  2331,  2336,  2343,  2352,
    2366,  2370,  2375,  2379,  2412,  2418,  2422,  2423,  2424,  2425,
    2426,  2428,  2431,  2437,  2440,  2441,  2442,  2443,  2444,  2445,
    2446,  2447,  2448,  2449,  2451,  2454,  2460,  2479,  2524,  2562,
    2604,  2650,  2671,  2691,  2709,  2727,  2735,  2746,  2752,  2761,
    2767,  2779,  2782,  2785,  2788,  2791,  2794,  2798,  2802,  2807,
    2815,  2823,  2832,  2839,  2846,  2853,  2860,  2867,  2875,  2883,
    2891,  2899,  2907,  2915,  2923,  2931,  2939,  2947,  2955,  2963,
    2993,  3001,  3010,  3018,  3027,  3035,  3041,  3048,  3054,  3061,
    3066,  3073,  3080,  3088,  3115,  3121,  3127,  3134,  3142,  3149,
    3156,  3161,  3171,  3176,  3181,  3186,  3191,  3196,  3201,  3206,
    3211,  3216,  3219,  3222,  3226,  3229,  3232,  3235,  3239,  3242,
    3245,  3249,  3253,  3258,  3263,  3266,  3270,  3274,  3281,  3288,
    3292,  3299,  3306,  3310,  3314,  3318,  3321,  3325,  3329,  3334,
    3339,  3343,  3348,  3353,  3359,  3365,  3371,  3377,  3383,  3389,
    3395,  3401,  3407,  3413,  3419,  3430,  3434,  3439,  3470,  3480,
    3485,  3490,  3495,  3501,  3505,  3506,  3508,  3509,  3511,  3512,
    3524,  3532,  3536,  3539,  3543,  3546,  3550,  3554,  3559,  3565,
    3575,  3585,  3593,  3604,  3635
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
  "EXCEPT", "FLUSH", "USE", "OPTIMIZE", "PROPERTIES", "DATABASE", "TABLE",
  "COLLECTION", "TABLES", "INTO", "VALUES", "AST", "PIPELINE", "RAW",
  "LOGICAL", "PHYSICAL", "FRAGMENT", "VIEW", "INDEX", "ANALYZE", "VIEWS",
  "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS", "COLUMN",
  "COLUMNS", "INDEXES", "CHUNK", "GROUP", "BY", "HAVING", "AS", "NATURAL",
  "JOIN", "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS",
  "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF",
  "NOT", "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT",
  "HEADER", "CAST", "END", "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN",
  "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR",
  "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME", "DATETIME",
  "FLOAT16", "BFLOAT16", "UNSIGNED", "TIMESTAMP", "UUID", "POINT", "LINE",
  "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "TEXT", "MULTIVECTOR", "TENSOR", "SPARSE",
  "TENSORARRAY", "IGNORE", "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS",
  "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "TRANSACTIONS",
  "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT",
  "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES", "VARIABLE",
  "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH", "MAXSIM",
  "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "LEADER", "FOLLOWER",
  "LEARNER", "CONNECT", "STANDALONE", "NODES", "NODE", "PERSISTENCE",
  "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION", "NUMBER", "'='",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('",
  "')'", "'.'", "';'", "','", "':'", "$accept", "input_pattern",
  "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "table_element",
  "table_column", "column_type", "column_constraints", "column_constraint",
  "default_expr", "table_constraint", "identifier_array",
  "delete_statement", "insert_statement", "optional_identifier_array",
  "explain_statement", "explain_type", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "from_clause", "search_clause",
  "where_clause", "having_clause", "group_by_clause", "set_operator",
  "table_reference", "table_reference_unit", "table_reference_name",
  "table_name", "table_alias", "with_clause", "with_expr_list",
  "with_expr", "join_clause", "join_type", "show_statement",
  "flush_statement", "optimize_statement", "command_statement",
  "compact_statement", "admin_statement", "expr_array", "expr_array_list",
  "expr_alias", "expr", "operand", "extra_match_tensor_option",
  "match_tensor_expr", "match_vector_expr", "match_sparse_expr",
  "match_text_expr", "query_expr", "fusion_expr", "sub_search",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
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

#define YYPACT_NINF (-558)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-492)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     709,   264,    59,   268,    74,    41,    74,   146,   325,   585,
      93,   107,   167,   242,   249,   254,   261,   101,    48,   -15,
     188,    86,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
     102,  -558,  -558,   308,  -558,  -558,  -558,  -558,  -558,  -558,
     263,   263,   263,   263,    42,    74,   277,   277,   277,   277,
     277,   162,   373,    74,    18,   414,   427,   432,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,    62,   437,    74,  -558,  -558,
    -558,  -558,  -558,   416,  -558,   233,   282,  -558,   443,  -558,
     272,  -558,  -558,   296,  -558,   279,   -63,    74,    74,    74,
    -558,  -558,  -558,  -558,     4,  -558,   411,   289,  -558,   488,
      67,   221,   507,   301,   320,  -558,    25,  -558,   501,  -558,
    -558,     3,   467,  -558,   471,   483,   541,    74,    74,    74,
     562,   506,   365,   498,   581,    74,    74,    74,   584,   586,
     592,   548,   607,   607,   469,    39,    60,    90,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,   102,  -558,  -558,  -558,  -558,
    -558,  -558,   225,  -558,  -558,   610,  -558,   621,  -558,  -558,
     620,   624,  -558,  -558,  -558,  -558,  -558,  -558,  -558,    74,
     417,   261,   607,  -558,   463,  -558,   627,  -558,  -558,   631,
    -558,  -558,   629,  -558,   633,   578,  -558,  -558,  -558,  -558,
       3,  -558,  -558,  -558,   469,   582,   566,   565,  -558,   -13,
    -558,   365,  -558,    74,   641,    73,  -558,  -558,  -558,  -558,
    -558,   579,  -558,   444,   -11,  -558,   469,  -558,  -558,   567,
     568,   433,  -558,  -558,   794,   504,   435,   439,   343,   650,
     652,   659,   662,  -558,  -558,   661,   455,   218,   456,   465,
     593,   593,  -558,    12,   355,   138,  -558,    29,   695,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,   459,  -558,  -558,  -558,  -100,  -558,  -558,    81,
    -558,   139,  -558,  -558,  -558,   156,  -558,   163,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,   680,   678,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,   609,   308,  -558,  -558,   686,    75,  -558,
     687,  -558,    53,   478,   480,   -56,   469,   469,   622,  -558,
     -15,    77,   637,   487,  -558,   175,   489,  -558,    74,   469,
     592,  -558,   331,   491,   492,   199,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,   593,   493,
     757,   619,   469,   469,   126,   241,  -558,  -558,  -558,  -558,
     794,  -558,   703,   495,   496,   500,   513,   710,   723,   361,
     361,  -558,   510,  -558,  -558,  -558,  -558,   525,    -3,   660,
     469,   737,   469,   469,    32,   529,    68,   593,   593,   593,
     593,   593,   593,   593,   593,   593,   593,   593,   593,   593,
     593,    17,  -558,   532,  -558,   738,  -558,   739,  -558,   740,
    -558,   749,   707,   449,   552,  -558,   546,  -558,   755,  -558,
      50,   600,   601,  -558,  -558,     7,   587,   554,  -558,    85,
     331,   469,  -558,   102,   876,   642,   563,   177,  -558,  -558,
    -558,   -15,   775,  -558,  -558,   777,   469,   569,  -558,   331,
    -558,    79,    79,   469,  -558,   181,   619,   623,   570,    33,
      -7,   287,  -558,   469,   469,   708,   469,   779,    26,   469,
     222,   228,   547,  -558,  -558,   607,  -558,  -558,  -558,   634,
     574,   593,   355,   656,  -558,   722,   722,   117,   117,   679,
     722,   722,   117,   117,   361,   361,  -558,  -558,  -558,  -558,
    -558,  -558,   575,  -558,   576,  -558,  -558,  -558,   789,   790,
    -558,   795,  -558,  -558,   791,  -558,   799,   800,   -15,   588,
     758,  -558,   180,  -558,   202,   548,   469,  -558,  -558,  -558,
     331,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,   594,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,   595,   602,   603,   611,   614,
     615,   185,   626,   641,   784,    77,   102,   628,  -558,   237,
     630,   830,   831,   836,   838,  -558,   839,   238,  -558,   302,
     303,  -558,   638,  -558,   876,   469,  -558,   469,    36,   124,
     593,    71,   636,  -558,    69,    89,  -558,   840,  -558,   847,
    -558,  -558,   773,   355,   722,   643,   307,  -558,   593,   852,
     854,   805,   817,   667,   309,  -558,   865,  -558,  -558,    35,
       7,   816,  -558,  -558,  -558,  -558,  -558,  -558,   819,  -558,
     875,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,   673,
     837,  -558,   886,   389,   564,   912,   929,   946,   963,   761,
     764,  -558,  -558,   159,  -558,   771,   641,   314,   683,  -558,
    -558,   741,  -558,   469,  -558,  -558,  -558,  -558,  -558,  -558,
      79,  -558,  -558,  -558,   698,   331,    40,  -558,   469,   658,
     702,   910,   532,   704,   705,   706,   711,   731,   316,  -558,
    -558,   757,   911,   915,   422,  -558,   795,   297,   180,   758,
       7,     7,   733,   202,   861,   889,   321,   732,   734,   735,
     736,   742,   768,   776,   778,   782,   828,   783,   793,   797,
     798,   801,   810,   811,   814,   815,   827,   853,   832,   833,
     834,   844,   845,   848,   849,   850,   851,   856,   855,   857,
     858,   859,   860,   862,   863,   864,   866,   867,   868,   897,
     870,   871,   872,   873,   874,   877,   878,   879,   880,   881,
     901,   882,   883,   884,   885,   887,   888,   890,   891,   892,
     893,   916,   894,  -558,  -558,    10,  -558,  -558,  -558,   326,
    -558,   795,   951,   350,  -558,  -558,  -558,   331,  -558,   553,
     895,   363,   896,    21,   898,  -558,  -558,  -558,  -558,  -558,
      79,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
     949,  -558,  -558,  -558,   950,   641,  -558,   469,   469,  -558,
    -558,   952,  1011,  1027,  1028,  1039,  1042,  1072,  1076,  1086,
    1087,   899,  1097,  1100,  1108,  1111,  1112,  1113,  1114,  1115,
    1116,  1117,   907,  1119,  1120,  1121,  1122,  1123,  1124,  1125,
    1126,  1127,  1128,   918,  1130,  1131,  1132,  1133,  1134,  1135,
    1136,  1137,  1138,  1139,   930,  1140,  1142,  1143,  1144,  1145,
    1146,  1147,  1148,  1149,  1150,   940,  1152,  1153,  1154,  1155,
    1156,  1157,  1158,  1159,  1160,  1161,   953,  1162,  -558,  -558,
     369,   609,  -558,  -558,  1165,  -558,  1167,  1168,  1169,   374,
    1170,   469,   386,   957,   331,   743,   961,   964,   965,   966,
     967,   968,   969,   970,   971,  1171,   972,   973,   974,   975,
     976,   977,   978,   979,   980,   981,  1190,   983,   984,   985,
     986,   987,   988,   989,   990,   991,   992,  1201,   994,   995,
     996,   997,   998,   999,  1000,  1001,  1002,  1003,  1212,  1005,
    1006,  1007,  1008,  1009,  1010,  1012,  1013,  1014,  1015,  1219,
    1016,  1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,
    1234,  1029,  -558,  -558,  1030,  1031,  1032,   387,  -558,   370,
     331,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  1034,  -558,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  1036,  -558,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  1037,  -558,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  1038,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  1040,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,  -558,  -558,  1041,  -558,  1237,
    -558,  1238,   609,  1240,  1247,  1250,  -558,  -558,  -558,  -558,
    -558,  -558,  -558,  -558,  -558,   398,  1043,  -558,  1044,  1251,
     543,   609,  1253,  1257,  1048,     2,   572,  1258,  -558,  -558,
    1049,  -558,  -558,  1200,  1221,  -558,  1260,  -558,   903,    28,
    -558,  1054,  -558,  -558,  1224,  1225,  -558,  1267,  -558,  1058,
    1059,  1269,   609,  1060,  -558,   609,  -558
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     225,     0,     0,     0,     0,     0,     0,     0,   160,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   225,
       0,   489,     3,     5,    10,    12,    13,    11,     6,     7,
       9,   174,   173,     0,     8,    14,    15,    16,    17,    18,
     487,   487,   487,   487,   487,     0,   485,   485,   485,   485,
     485,   218,     0,     0,     0,     0,     0,     0,   154,   158,
     155,   156,   157,   159,   153,   225,     0,     0,   239,   240,
     238,   244,   248,     0,   245,     0,     0,   241,     0,   243,
       0,   266,   268,     0,   246,     0,   272,     0,     0,     0,
     275,   276,   277,   280,   218,   278,     0,   224,   226,     0,
       0,     0,     0,     0,     0,     1,   225,     2,   208,   210,
     211,     0,   197,   179,   185,     0,     0,     0,     0,     0,
       0,     0,   151,     0,     0,     0,     0,     0,     0,     0,
       0,   203,     0,     0,     0,     0,     0,     0,   152,    19,
      24,    26,    25,    20,    21,    23,    22,    27,    28,    29,
      30,   254,   255,   249,   250,     0,   251,     0,   242,   267,
       0,     0,   270,   269,   273,   274,   299,   297,   298,     0,
       0,     0,     0,   325,     0,   326,     0,   320,   321,     0,
     316,   300,     0,   323,     0,     0,   178,   177,     4,   209,
       0,   175,   176,   196,     0,     0,   193,     0,    31,     0,
      32,   151,   490,     0,     0,   225,   484,   165,   167,   166,
     168,     0,   219,     0,   203,   162,     0,   147,   483,     0,
       0,   418,   422,   425,   426,     0,     0,     0,     0,     0,
       0,     0,     0,   423,   424,     0,     0,     0,     0,     0,
       0,     0,   420,     0,   225,     0,   330,   335,   336,   350,
     348,   351,   349,   352,   353,   345,   340,   339,   338,   346,
     347,   337,   344,   343,   433,   435,     0,   436,   444,     0,
     445,     0,   437,   434,   455,     0,   456,     0,   432,   284,
     286,   285,   282,   283,   289,   291,   290,   287,   288,   294,
     296,   295,   292,   293,     0,     0,   257,   256,   262,   252,
     253,   247,   271,   493,     0,   227,   281,     0,   317,   322,
     301,   324,     0,     0,     0,   199,     0,     0,   195,   486,
     225,     0,     0,     0,   145,     0,     0,   149,     0,     0,
       0,   161,   202,     0,     0,     0,   464,   463,   466,   465,
     468,   467,   470,   469,   472,   471,   474,   473,     0,     0,
     384,   225,     0,     0,     0,     0,   427,   428,   429,   430,
       0,   431,     0,     0,     0,     0,     0,     0,     0,   386,
     385,   461,   458,   452,   442,   447,   450,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   441,     0,   446,     0,   449,     0,   457,     0,
     460,     0,   263,   258,     0,   279,     0,   327,     0,   318,
       0,     0,     0,   182,   181,     0,   201,   184,   186,   191,
     192,     0,   180,    34,     0,     0,     0,     0,    37,    39,
      40,   225,     0,    36,   150,     0,     0,   148,   169,   164,
     163,     0,     0,     0,   379,     0,   225,     0,     0,     0,
       0,     0,   409,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   342,   341,     0,   331,   334,   402,   403,
       0,     0,   225,     0,   383,   393,   394,   397,   398,     0,
     400,   392,   395,   396,   388,   387,   389,   390,   391,   419,
     421,   443,     0,   448,     0,   451,   459,   462,     0,     0,
     259,     0,   228,   319,     0,   302,     0,     0,   225,   198,
     212,   214,   223,   215,     0,   203,     0,   189,   190,   188,
     194,    43,    46,    47,    44,    45,    48,    49,    65,    50,
      52,    51,    68,    55,    56,    57,    53,    54,    58,    59,
      60,    61,    62,    63,    64,     0,     0,     0,     0,     0,
       0,   493,     0,     0,   495,     0,    35,     0,   146,     0,
       0,     0,     0,     0,     0,   479,     0,     0,   475,     0,
       0,   380,     0,   414,     0,     0,   407,     0,     0,     0,
       0,     0,     0,   418,     0,     0,   367,     0,   369,     0,
     454,   453,     0,   225,   401,     0,     0,   382,     0,     0,
       0,   264,   260,   498,     0,   496,   303,   328,   329,     0,
       0,     0,   232,   233,   234,   235,   231,   236,     0,   221,
       0,   216,   373,   371,   374,   372,   375,   376,   377,   200,
     207,   187,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   138,   139,   142,   135,   142,     0,     0,     0,    33,
      38,   504,   332,     0,   481,   480,   478,   477,   482,   172,
       0,   170,   381,   415,     0,   411,     0,   410,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   416,
     405,   404,     0,     0,     0,   492,     0,     0,   223,   213,
       0,     0,   220,     0,     0,   205,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   140,   137,     0,   136,    42,    41,     0,
     144,     0,     0,     0,   476,   413,   408,   412,   399,     0,
       0,     0,     0,     0,     0,   438,   440,   439,   368,   370,
       0,   417,   406,   265,   261,   499,   500,   502,   501,   497,
       0,   304,   217,   229,     0,     0,   378,     0,     0,   183,
      67,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   141,   143,
       0,   493,   333,   458,     0,   365,     0,     0,     0,     0,
     305,     0,     0,   206,   204,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   494,   503,     0,     0,     0,     0,   171,     0,
     230,   222,    66,    72,    73,    70,    71,    74,    75,    76,
      77,    78,     0,    69,   116,   117,   114,   115,   118,   119,
     120,   121,   122,     0,   113,    83,    84,    81,    82,    85,
      86,    87,    88,    89,     0,    80,    94,    95,    92,    93,
      96,    97,    98,    99,   100,     0,    91,   127,   128,   125,
     126,   129,   130,   131,   132,   133,     0,   124,   105,   106,
     103,   104,   107,   108,   109,   110,   111,     0,   102,     0,
     366,     0,   493,     0,     0,     0,   307,   306,   312,    79,
     123,    90,   101,   134,   112,     0,   355,   364,     0,   313,
     308,   493,     0,     0,     0,   493,     0,     0,   309,   360,
       0,   354,   356,     0,     0,   363,     0,   314,   310,   493,
     362,     0,   315,   311,     0,     0,   359,     0,   358,     0,
       0,     0,   493,     0,   361,   493,   357
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -558,  -558,  -558,  1172,  -558,  1210,  -558,   712,  -558,   692,
    -558,   632,   625,  -558,  -550,  1214,  1216,  1081,  -558,  -558,
    1218,  -558,   954,  1222,  1223,   -62,  1270,   -18,   982,  1101,
     -32,  -558,  -558,   766,  -558,  -558,  -558,  -558,  -558,  -558,
    -205,  -558,  -558,  -558,  -558,   670,   -78,    15,   596,  -558,
    -558,  1129,  -558,  -558,  1228,  1230,  1231,  1232,  1233,  -558,
    -192,  -558,   919,  -216,  -218,  -558,  -493,  -491,  -481,  -473,
    -468,  -466,   598,  -558,  -558,  -558,  -558,  -558,  -558,   948,
    -558,  -558,   835,   530,  -238,  -558,  -558,  -558,   635,  -558,
    -558,  -558,  -558,   639,   902,   904,  -342,  -558,  -558,  -558,
    -558,  1069,  -446,   640,  -125,   419,   461,  -558,  -558,  -557,
    -558,   527,   613,  -558
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    20,    21,    22,   138,    23,   437,   438,   439,   561,
     653,   654,   777,   440,   325,    24,    25,   205,    26,    65,
      27,   214,   215,    28,    29,    30,    31,    32,   113,   191,
     114,   196,   427,   428,   529,   318,   432,   194,   426,   525,
     217,   819,   705,   111,   519,   520,   521,   522,   631,    33,
      97,    98,   523,   628,    34,    35,    36,    37,    38,    39,
     245,   447,   246,   247,   248,  1074,   249,   250,   251,   252,
     253,   254,   638,   639,   255,   256,   257,   258,   259,   355,
     260,   261,   262,   263,   264,   794,   265,   266,   267,   268,
     269,   270,   271,   272,   375,   376,   273,   274,   275,   276,
     277,   278,   577,   578,   219,   124,   116,   107,   121,   415,
     659,   614,   615,   443
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     332,   104,   315,   145,   655,   374,   579,   350,   220,   331,
      51,   112,   354,   657,   222,   223,   224,   371,   372,    52,
     499,    54,   369,   370,   425,  -488,   371,   372,   378,   593,
      95,   632,     1,   633,     2,     3,     4,     5,     6,     7,
       8,     9,   279,   634,   280,   281,   320,   306,    10,    11,
      12,   635,    13,    14,    15,   108,   636,   109,   637,   216,
     122,   110,   100,   284,   101,   285,   286,    16,   131,     1,
     169,     2,     3,     4,     5,     6,     7,    51,     9,   192,
     434,   514,   152,   585,   414,    10,    11,    12,   381,    13,
      14,    15,   584,   289,    45,   290,   291,   515,   132,   133,
     429,   430,   166,   167,   168,   282,   779,    16,  -491,   326,
     414,   480,   402,   449,   229,   230,   231,   403,   115,   418,
     232,    53,   108,   677,   109,    87,   287,   786,   110,   419,
     350,  1083,   199,   200,   201,   164,   459,   460,   165,    88,
     208,   209,   210,   455,    16,   483,   233,   234,   235,   382,
     383,   571,   572,   382,   383,    16,   292,  1094,   314,   527,
     528,   380,   573,   574,   575,   501,   478,   479,  1084,   485,
     486,   487,   488,   489,   490,   491,   492,   493,   494,   495,
     496,   497,   498,   629,   303,   382,   383,   327,   105,   382,
     383,   481,   382,   383,  1095,    17,   382,   383,    19,    89,
     321,   484,   221,   222,   223,   224,   330,   435,   283,   436,
     632,   474,   633,    18,   678,   530,   190,   353,   323,   129,
     518,   243,   634,   243,   373,   500,   377,   382,   383,   288,
     635,   134,    17,   373,   242,   636,   649,   637,    19,   630,
     102,   382,   383,   421,   422,   382,   383,   588,   589,   698,
     591,   386,    93,   595,   569,   173,   174,    94,   433,   293,
     175,   580,   649,   604,    96,   902,   576,   414,   453,   294,
    -492,  -492,    99,   295,   296,    19,   225,   226,   297,   298,
     382,   383,   382,   383,   401,   227,   682,   228,   680,   650,
     606,   651,   652,   404,   775,    40,    41,    42,   405,    46,
      47,    48,   106,   229,   230,   231,   683,    43,    44,   232,
     429,    49,    50,    55,    56,   650,   112,   651,   652,    57,
     640,  -492,  -492,   396,   397,   398,   399,   400,   462,   810,
     463,   811,   464,   458,   973,   233,   234,   235,   790,   115,
     363,   797,   364,   448,   365,   366,   221,   222,   223,   224,
     602,   406,   379,   123,   899,   380,   407,   236,   221,   222,
     223,   224,    58,    59,    60,    61,    62,    63,   408,   675,
      64,   676,   679,   409,   586,   410,   587,   129,   464,   566,
     411,   237,   176,   238,   237,   239,   238,   130,   239,   444,
     691,   564,   445,   177,   565,   581,   178,   179,   380,   180,
     181,   182,    90,    91,    92,   240,   241,   242,   154,   155,
     243,   688,   244,   454,  1054,   183,   184,   135,  1055,  1056,
     225,   226,   153,  1057,  1058,   805,   806,   807,   808,   227,
     136,   228,   225,   226,   353,   137,   596,    16,   582,   597,
     151,   227,   598,   228,   792,   599,   158,   229,   230,   231,
     159,   662,   669,   232,   380,   670,   619,   156,   157,   229,
     230,   231,   787,   160,   605,   232,   125,   126,   127,   128,
     170,   783,   221,   222,   223,   224,   161,   162,   163,   233,
     234,   235,   707,   708,   709,   710,   711,   382,   383,   712,
     713,   233,   234,   235,   172,  1067,   714,   715,   716,   509,
     510,   236,   117,   118,   119,   120,   171,   221,   222,   223,
     224,   185,   717,   236,  1079,   186,   671,   672,  1085,   670,
     380,   690,   189,   695,   380,   237,   696,   238,   780,   239,
     802,   445,  1096,   380,   187,   820,   193,   237,   821,   238,
     889,   239,   195,   445,   198,  1104,   225,   226,  1106,   240,
     241,   242,   600,   601,   243,   227,   244,   228,   371,   893,
     197,   240,   241,   242,   892,   202,   243,   380,   244,   398,
     399,   400,   203,   229,   230,   231,   206,   895,   204,   232,
     896,   348,   349,   972,   207,   687,   696,   211,   978,   212,
     227,   670,   228,  1077,  1078,   213,   221,   222,   223,   224,
     981,  1052,   904,   445,  1053,   233,   234,   235,   229,   230,
     231,   218,  1071,   299,   232,  1072,    66,    67,   216,    68,
    1086,  1087,   813,   814,   300,   903,   301,   236,   302,   307,
     304,    69,    70,   308,   309,   310,   311,   312,   317,   316,
     233,   234,   235,   319,   324,   328,   335,   329,   351,   333,
     334,   237,   352,   238,   356,   239,   357,   718,   719,   720,
     721,   722,   236,   358,   723,   724,   359,   360,   362,   367,
     348,   725,   726,   727,   401,   240,   241,   242,   368,   227,
     243,   228,   244,   412,   413,   980,   237,   728,   238,   417,
     239,   414,   423,   420,   424,   431,   441,   229,   230,   231,
     442,    16,   446,   232,   451,   452,   456,   465,   466,   467,
     240,   241,   242,   468,   470,   243,     1,   244,     2,     3,
       4,     5,     6,     7,     8,     9,   469,   471,   472,   233,
     234,   235,    10,    11,    12,   457,    13,    14,    15,   473,
     477,   475,   482,   243,   502,   504,   506,    71,    72,    73,
      74,   236,    75,    76,   507,   508,   457,    77,    78,    79,
     512,   513,    80,    81,    82,   511,   516,   517,   524,    83,
      84,   526,   384,   562,   385,   237,   563,   238,   567,   239,
     568,    85,   481,   592,   583,    86,   570,   603,   590,   607,
     382,    16,   386,   609,   610,   611,   612,   616,   613,   240,
     241,   242,   617,   618,   243,   620,   244,   642,   643,   387,
     388,   389,   390,   386,   658,   644,   645,   392,   621,  -237,
     622,   623,   624,   625,   646,   626,   627,   647,   648,   386,
     387,   388,   389,   390,   457,   608,   664,   665,   392,   656,
     666,   667,   661,   663,   684,   668,   387,   388,   389,   390,
     391,   685,   673,   681,   392,   686,   386,   689,   601,   600,
     692,   393,   394,   395,   396,   397,   398,   399,   400,   693,
     694,   697,   788,  -492,  -492,   389,   390,   700,   702,    17,
     701,  -492,   393,   394,   395,   396,   397,   398,   399,   400,
     703,   386,   706,   704,   773,   774,   781,    18,   393,   394,
     395,   396,   397,   398,   399,   400,   775,   782,   387,   388,
     389,   390,   785,   789,   791,   793,   392,   803,   817,   798,
     799,   804,    19,   831,   800,  -492,   394,   395,   396,   397,
     398,   399,   400,   336,   337,   338,   339,   340,   341,   342,
     343,   344,   345,   346,   347,   801,   815,   818,   842,   822,
     853,   823,   824,   825,   891,   900,  1093,   982,   905,   826,
     393,   394,   395,   396,   397,   398,   399,   400,   531,   532,
     533,   534,   535,   536,   537,   538,   539,   540,   541,   542,
     543,   544,   545,   546,   547,   827,   548,   549,   550,   551,
     552,   553,   864,   828,   554,   829,   875,   555,   556,   830,
     832,   557,   558,   559,   560,   729,   730,   731,   732,   733,
     833,   886,   734,   735,   834,   835,   901,   906,   836,   736,
     737,   738,   740,   741,   742,   743,   744,   837,   838,   745,
     746,   839,   840,   907,   908,   739,   747,   748,   749,   751,
     752,   753,   754,   755,   841,   909,   756,   757,   910,   843,
     844,   845,   750,   758,   759,   760,   762,   763,   764,   765,
     766,   846,   847,   767,   768,   848,   849,   850,   851,   761,
     769,   770,   771,   852,   854,   855,   856,   857,   911,   858,
     859,   860,   912,   861,   862,   863,   772,   865,   866,   867,
     868,   869,   913,   914,   870,   871,   872,   873,   874,   876,
     877,   878,   879,   916,   880,   881,   917,   882,   883,   884,
     885,   887,   894,   897,   918,   898,   915,   919,   920,   921,
     922,   923,   924,   925,   926,   927,   928,   929,   930,   931,
     932,   933,   934,   935,   936,   937,   938,   939,   940,   941,
     942,   943,   944,   945,   946,   947,   949,   948,   950,   951,
     952,   953,   954,   955,   956,   957,   958,   959,   960,   961,
     962,   963,   964,   965,   966,   967,   968,   969,   971,   974,
     970,   975,   976,   977,   380,   983,   979,   992,   984,   985,
     986,   987,   988,   989,   990,   991,   993,   994,   995,   996,
     997,   998,   999,  1000,  1001,  1002,  1003,  1004,  1005,  1006,
    1007,  1008,  1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,
    1017,  1018,  1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,
    1027,  1028,  1029,  1030,  1031,  1036,  1032,  1033,  1034,  1035,
    1037,  1038,  1039,  1040,  1041,  1042,  1043,  1044,  1045,  1046,
    1047,  1065,  1066,  1048,  1090,  1050,  1068,  1049,  1059,  1051,
    1060,  1061,  1062,  1069,  1063,  1064,  1070,  1076,  1075,  1080,
    1073,  1081,  1082,  1089,  1088,  1091,  1092,  1097,  1098,  1099,
    1100,  1101,  1103,  1102,  1105,   139,   674,   660,   188,   140,
     778,   141,   322,   142,   450,   776,   416,   143,   144,   103,
     699,   313,   641,   146,   812,   147,   148,   149,   150,   476,
     305,   816,   461,   594,   361,   888,     0,   503,   890,   809,
     784,   505,     0,     0,     0,     0,     0,     0,   795,     0,
       0,     0,   796
};

static const yytype_int16 yycheck[] =
{
     216,    19,   194,    65,   561,   243,   452,   225,   133,   214,
       3,     8,   228,   563,     4,     5,     6,     5,     6,     4,
       3,     6,   240,   241,    80,     0,     5,     6,   244,     3,
      15,   524,     7,   524,     9,    10,    11,    12,    13,    14,
      15,    16,     3,   524,     5,     6,    59,   172,    23,    24,
      25,   524,    27,    28,    29,    20,   524,    22,   524,    70,
      45,    26,    14,     3,    16,     5,     6,    82,    53,     7,
      66,     9,    10,    11,    12,    13,    14,     3,    16,   111,
       3,    31,    67,    90,    82,    23,    24,    25,    59,    27,
      28,    29,    59,     3,    35,     5,     6,    47,    80,    81,
     316,   317,    87,    88,    89,    66,   656,    82,    66,    36,
      82,    79,   212,   329,   104,   105,   106,   217,    76,    44,
     110,    80,    20,    87,    22,    32,    66,    87,    26,    54,
     348,   129,   117,   118,   119,   198,   352,   353,   201,    32,
     125,   126,   127,   335,    82,    77,   136,   137,   138,   156,
     157,    72,    73,   156,   157,    82,    66,   129,   190,    74,
      75,   217,    83,    84,    85,   403,   382,   383,   166,   387,
     388,   389,   390,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,     3,   169,   156,   157,   205,     0,   156,
     157,   159,   156,   157,   166,   170,   156,   157,   213,    32,
     213,   133,     3,     4,     5,     6,   217,   130,   169,   132,
     703,   214,   703,   188,    90,   431,   213,    91,   203,   215,
     213,   211,   703,   211,   212,   208,   244,   156,   157,   169,
     703,   213,   170,   212,   208,   703,    77,   703,   213,    59,
     192,   156,   157,   190,   191,   156,   157,   463,   464,   214,
     466,   134,     3,   469,   446,   188,   189,     3,   320,   169,
     193,   453,    77,   481,     3,   815,   187,    82,    69,    44,
     153,   154,   171,    48,    49,   213,    77,    78,    53,    54,
     156,   157,   156,   157,   215,    86,   217,    88,   217,   130,
     482,   132,   133,   212,   135,    31,    32,    33,   217,    31,
      32,    33,   216,   104,   105,   106,   217,    43,    44,   110,
     526,    43,    44,   167,   168,   130,     8,   132,   133,   173,
     525,   204,   205,   206,   207,   208,   209,   210,    87,    32,
      89,    34,    91,   351,   891,   136,   137,   138,   680,    76,
     122,   683,   124,   328,   126,   127,     3,     4,     5,     6,
     475,   212,   214,    76,   800,   217,   217,   158,     3,     4,
       5,     6,    37,    38,    39,    40,    41,    42,   212,   585,
      45,   587,   590,   217,    87,   212,    89,   215,    91,   441,
     217,   182,   161,   184,   182,   186,   184,    14,   186,   214,
     608,   214,   217,   172,   217,   214,   175,   176,   217,   178,
     179,   180,   160,   161,   162,   206,   207,   208,   175,   176,
     211,   603,   213,   214,    44,   194,   195,     3,    48,    49,
      77,    78,     6,    53,    54,     3,     4,     5,     6,    86,
       3,    88,    77,    78,    91,     3,   214,    82,   456,   217,
       3,    86,   214,    88,   682,   217,     3,   104,   105,   106,
     178,   214,   214,   110,   217,   217,   518,   175,   176,   104,
     105,   106,   678,   167,   482,   110,    47,    48,    49,    50,
      59,   663,     3,     4,     5,     6,   197,   198,   199,   136,
     137,   138,    93,    94,    95,    96,    97,   156,   157,   100,
     101,   136,   137,   138,     6,  1052,   107,   108,   109,    50,
      51,   158,    41,    42,    43,    44,   217,     3,     4,     5,
       6,     4,   123,   158,  1071,   214,   214,   214,  1075,   217,
     217,   214,    21,   214,   217,   182,   217,   184,   214,   186,
     214,   217,  1089,   217,   214,   214,    69,   182,   217,   184,
     214,   186,    71,   217,     3,  1102,    77,    78,  1105,   206,
     207,   208,     5,     6,   211,    86,   213,    88,     5,     6,
      77,   206,   207,   208,   214,     3,   211,   217,   213,   208,
     209,   210,    66,   104,   105,   106,    78,   214,   213,   110,
     217,    77,    78,   214,     3,   603,   217,     3,   214,     3,
      86,   217,    88,    50,    51,     3,     3,     4,     5,     6,
     214,   214,   818,   217,   217,   136,   137,   138,   104,   105,
     106,     4,   214,     3,   110,   217,    31,    32,    70,    34,
      48,    49,   700,   701,     3,   817,     6,   158,     4,   166,
     213,    46,    47,     6,     3,     6,     3,    59,    72,    57,
     136,   137,   138,    78,     3,    66,   213,   203,   213,    82,
      82,   182,   213,   184,     4,   186,     4,    93,    94,    95,
      96,    97,   158,     4,   100,   101,     4,     6,   213,   213,
      77,   107,   108,   109,   215,   206,   207,   208,   213,    86,
     211,    88,   213,     3,     6,   901,   182,   123,   184,     3,
     186,    82,   214,     6,   214,    73,    59,   104,   105,   106,
     213,    82,   213,   110,   213,   213,   213,     4,   213,   213,
     206,   207,   208,   213,     4,   211,     7,   213,     9,    10,
      11,    12,    13,    14,    15,    16,   213,     4,   218,   136,
     137,   138,    23,    24,    25,    77,    27,    28,    29,   214,
       3,    81,   213,   211,     6,     6,     6,   162,   163,   164,
     165,   158,   167,   168,     5,    48,    77,   172,   173,   174,
     214,     6,   177,   178,   179,   213,   166,   166,   181,   184,
     185,   217,    77,   131,    79,   182,   213,   184,     3,   186,
       3,   196,   159,     4,   214,   200,   217,   213,    80,   133,
     156,    82,   134,   218,   218,     6,     6,     6,     3,   206,
     207,   208,     3,     3,   211,   217,   213,   213,   213,   151,
     152,   153,   154,   134,    30,   213,   213,   159,    60,    61,
      62,    63,    64,    65,   213,    67,    68,   213,   213,   134,
     151,   152,   153,   154,    77,   156,     6,     6,   159,   213,
       4,     3,   214,   213,     4,     6,   151,   152,   153,   154,
     155,     4,   214,   217,   159,    82,   134,   214,     6,     5,
      55,   203,   204,   205,   206,   207,   208,   209,   210,    52,
     203,     6,   214,   151,   152,   153,   154,    61,     3,   170,
      61,   159,   203,   204,   205,   206,   207,   208,   209,   210,
     217,   134,     6,    56,   133,   131,   213,   188,   203,   204,
     205,   206,   207,   208,   209,   210,   135,   166,   151,   152,
     153,   154,   214,   211,     4,   211,   159,     6,    57,   214,
     214,     6,   213,    95,   213,   203,   204,   205,   206,   207,
     208,   209,   210,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   214,   213,    58,    95,   217,
      95,   217,   217,   217,     3,     6,    53,   214,     6,   217,
     203,   204,   205,   206,   207,   208,   209,   210,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   217,   110,   111,   112,   113,
     114,   115,    95,   217,   118,   217,    95,   121,   122,   217,
     217,   125,   126,   127,   128,    93,    94,    95,    96,    97,
     217,    95,   100,   101,   217,   217,    66,     6,   217,   107,
     108,   109,    93,    94,    95,    96,    97,   217,   217,   100,
     101,   217,   217,     6,     6,   123,   107,   108,   109,    93,
      94,    95,    96,    97,   217,     6,   100,   101,     6,   217,
     217,   217,   123,   107,   108,   109,    93,    94,    95,    96,
      97,   217,   217,   100,   101,   217,   217,   217,   217,   123,
     107,   108,   109,   217,   217,   217,   217,   217,     6,   217,
     217,   217,     6,   217,   217,   217,   123,   217,   217,   217,
     217,   217,     6,     6,   217,   217,   217,   217,   217,   217,
     217,   217,   217,     6,   217,   217,     6,   217,   217,   217,
     217,   217,   217,   217,     6,   217,   217,     6,     6,     6,
       6,     6,     6,     6,   217,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   217,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     4,
     217,     4,     4,     4,   217,   214,     6,     6,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,     6,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,     6,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,     6,   214,
     214,   214,   214,   214,   214,     6,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
       6,     4,     4,   214,    44,   214,     6,   217,   214,   217,
     214,   214,   214,     6,   214,   214,     6,     6,   214,     6,
     217,     4,   214,   214,     6,    44,     6,   213,    44,    44,
       3,   213,     3,   214,   214,    65,   584,   565,   106,    65,
     655,    65,   201,    65,   330,   653,   304,    65,    65,    19,
     620,   190,   526,    65,   698,    65,    65,    65,    65,   380,
     171,   703,   354,   468,   235,   775,    -1,   405,   781,   696,
     670,   407,    -1,    -1,    -1,    -1,    -1,    -1,   683,    -1,
      -1,    -1,   683
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    24,    25,    27,    28,    29,    82,   170,   188,   213,
     220,   221,   222,   224,   234,   235,   237,   239,   242,   243,
     244,   245,   246,   268,   273,   274,   275,   276,   277,   278,
      31,    32,    33,    43,    44,    35,    31,    32,    33,    43,
      44,     3,   266,    80,   266,   167,   168,   173,    37,    38,
      39,    40,    41,    42,    45,   238,    31,    32,    34,    46,
      47,   162,   163,   164,   165,   167,   168,   172,   173,   174,
     177,   178,   179,   184,   185,   196,   200,    32,    32,    32,
     160,   161,   162,     3,     3,   266,     3,   269,   270,   171,
      14,    16,   192,   245,   246,     0,   216,   326,    20,    22,
      26,   262,     8,   247,   249,    76,   325,   325,   325,   325,
     325,   327,   266,    76,   324,   324,   324,   324,   324,   215,
      14,   266,    80,    81,   213,     3,     3,     3,   223,   224,
     234,   235,   239,   242,   243,   244,   273,   274,   275,   276,
     277,     3,   266,     6,   175,   176,   175,   176,     3,   178,
     167,   197,   198,   199,   198,   201,   266,   266,   266,    66,
      59,   217,     6,   188,   189,   193,   161,   172,   175,   176,
     178,   179,   180,   194,   195,     4,   214,   214,   222,    21,
     213,   248,   249,    69,   256,    71,   250,    77,     3,   266,
     266,   266,     3,    66,   213,   236,    78,     3,   266,   266,
     266,     3,     3,     3,   240,   241,    70,   259,     4,   323,
     323,     3,     4,     5,     6,    77,    78,    86,    88,   104,
     105,   106,   110,   136,   137,   138,   158,   182,   184,   186,
     206,   207,   208,   211,   213,   279,   281,   282,   283,   285,
     286,   287,   288,   289,   290,   293,   294,   295,   296,   297,
     299,   300,   301,   302,   303,   305,   306,   307,   308,   309,
     310,   311,   312,   315,   316,   317,   318,   319,   320,     3,
       5,     6,    66,   169,     3,     5,     6,    66,   169,     3,
       5,     6,    66,   169,    44,    48,    49,    53,    54,     3,
       3,     6,     4,   266,   213,   270,   323,   166,     6,     3,
       6,     3,    59,   248,   249,   279,    57,    72,   254,    78,
      59,   213,   236,   266,     3,   233,    36,   246,    66,   203,
     217,   259,   282,    82,    82,   213,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,    77,    78,
     283,   213,   213,    91,   282,   298,     4,     4,     4,     4,
       6,   320,   213,   122,   124,   126,   127,   213,   213,   283,
     283,     5,     6,   212,   303,   313,   314,   246,   282,   214,
     217,    59,   156,   157,    77,    79,   134,   151,   152,   153,
     154,   155,   159,   203,   204,   205,   206,   207,   208,   209,
     210,   215,   212,   217,   212,   217,   212,   217,   212,   217,
     212,   217,     3,     6,    82,   328,   247,     3,    44,    54,
       6,   190,   191,   214,   214,    80,   257,   251,   252,   282,
     282,    73,   255,   244,     3,   130,   132,   225,   226,   227,
     232,    59,   213,   332,   214,   217,   213,   280,   266,   282,
     241,   213,   213,    69,   214,   279,   213,    77,   246,   282,
     282,   298,    87,    89,    91,     4,   213,   213,   213,   213,
       4,     4,   218,   214,   214,    81,   281,     3,   282,   282,
      79,   159,   213,    77,   133,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   283,   283,   283,   283,   283,     3,
     208,   303,     6,   313,     6,   314,     6,     5,    48,    50,
      51,   213,   214,     6,    31,    47,   166,   166,   213,   263,
     264,   265,   266,   271,   181,   258,   217,    74,    75,   253,
     282,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   110,   111,
     112,   113,   114,   115,   118,   121,   122,   125,   126,   127,
     128,   228,   131,   213,   214,   217,   244,     3,     3,   279,
     217,    72,    73,    83,    84,    85,   187,   321,   322,   321,
     279,   214,   246,   214,    59,    90,    87,    89,   282,   282,
      80,   282,     4,     3,   301,   282,   214,   217,   214,   217,
       5,     6,   323,   213,   283,   246,   279,   133,   156,   218,
     218,     6,     6,     3,   330,   331,     6,     3,     3,   244,
     217,    60,    62,    63,    64,    65,    67,    68,   272,     3,
      59,   267,   285,   286,   287,   288,   289,   290,   291,   292,
     259,   252,   213,   213,   213,   213,   213,   213,   213,    77,
     130,   132,   133,   229,   230,   328,   213,   233,    30,   329,
     226,   214,   214,   213,     6,     6,     4,     3,     6,   214,
     217,   214,   214,   214,   228,   282,   282,    87,    90,   283,
     217,   217,   217,   217,     4,     4,    82,   246,   279,   214,
     214,   283,    55,    52,   203,   214,   217,     6,   214,   264,
      61,    61,     3,   217,    56,   261,     6,    93,    94,    95,
      96,    97,   100,   101,   107,   108,   109,   123,    93,    94,
      95,    96,    97,   100,   101,   107,   108,   109,   123,    93,
      94,    95,    96,    97,   100,   101,   107,   108,   109,   123,
      93,    94,    95,    96,    97,   100,   101,   107,   108,   109,
     123,    93,    94,    95,    96,    97,   100,   101,   107,   108,
     109,   123,    93,    94,    95,    96,    97,   100,   101,   107,
     108,   109,   123,   133,   131,   135,   230,   231,   231,   233,
     214,   213,   166,   279,   322,   214,    87,   282,   214,   211,
     315,     4,   303,   211,   304,   307,   312,   315,   214,   214,
     213,   214,   214,     6,     6,     3,     4,     5,     6,   331,
      32,    34,   267,   265,   265,   213,   291,    57,    58,   260,
     214,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     217,    95,   217,   217,   217,   217,   217,   217,   217,   217,
     217,   217,    95,   217,   217,   217,   217,   217,   217,   217,
     217,   217,   217,    95,   217,   217,   217,   217,   217,   217,
     217,   217,   217,   217,    95,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,    95,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,    95,   217,   302,   214,
     330,     3,   214,     6,   217,   214,   217,   217,   217,   321,
       6,    66,   233,   279,   282,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   217,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   217,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   217,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   217,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   217,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     217,     6,   214,   328,     4,     4,     4,     4,   214,     6,
     282,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,     6,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,     6,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,     6,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,     6,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,     6,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,     6,   214,   217,
     214,   217,   214,   217,    44,    48,    49,    53,    54,   214,
     214,   214,   214,   214,   214,     4,     4,   328,     6,     6,
       6,   214,   217,   217,   284,   214,     6,    50,    51,   328,
       6,     4,   214,   129,   166,   328,    48,    49,     6,   214,
      44,    44,     6,    53,   129,   166,   328,   213,    44,    44,
       3,   213,   214,     3,   328,   214,   328
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   219,   220,   221,   221,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   224,   224,   224,   224,   224,   224,   225,   225,   226,
     226,   227,   227,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   228,   228,   228,   229,   229,   230,   230,   230,
     230,   231,   231,   232,   232,   233,   233,   234,   235,   235,
     236,   236,   237,   238,   238,   238,   238,   238,   238,   238,
     238,   239,   240,   240,   241,   242,   242,   242,   242,   242,
     243,   243,   243,   244,   244,   244,   244,   245,   245,   246,
     247,   248,   248,   249,   250,   250,   251,   251,   252,   253,
     253,   253,   254,   254,   255,   255,   256,   256,   257,   257,
     258,   258,   259,   259,   260,   260,   261,   261,   262,   262,
     262,   262,   263,   263,   264,   264,   265,   265,   266,   266,
     267,   267,   267,   267,   268,   268,   269,   269,   270,   271,
     271,   272,   272,   272,   272,   272,   272,   272,   273,   273,
     273,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   273,   273,   273,   273,   273,   273,   273,   273,   273,
     273,   273,   273,   273,   273,   274,   274,   274,   275,   275,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   277,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     279,   279,   280,   280,   281,   281,   282,   282,   282,   282,
     282,   283,   283,   283,   283,   283,   283,   283,   283,   283,
     283,   283,   283,   283,   284,   284,   285,   286,   286,   286,
     286,   287,   287,   287,   287,   288,   288,   289,   289,   290,
     290,   291,   291,   291,   291,   291,   291,   292,   292,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   294,   294,   295,   296,   296,   297,   297,   297,
     297,   298,   298,   299,   300,   300,   300,   300,   301,   301,
     301,   301,   302,   302,   302,   302,   302,   302,   302,   302,
     302,   302,   302,   302,   303,   303,   303,   303,   304,   304,
     304,   305,   306,   306,   307,   307,   308,   309,   309,   310,
     311,   311,   312,   313,   314,   315,   315,   316,   317,   317,
     318,   319,   319,   320,   320,   320,   320,   320,   320,   320,
     320,   320,   320,   320,   320,   321,   321,   322,   322,   322,
     322,   322,   322,   323,   324,   324,   325,   325,   326,   326,
     327,   327,   328,   328,   329,   329,   330,   330,   331,   331,
     331,   331,   331,   332,   332
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     4,     8,     6,     7,     6,     1,     3,     1,
       1,     4,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     6,     4,     1,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     1,     2,     2,     1,     1,
       2,     2,     0,     5,     4,     1,     3,     4,     6,     5,
       3,     0,     3,     1,     1,     1,     1,     1,     1,     1,
       0,     5,     1,     3,     3,     4,     4,     4,     4,     6,
       8,    11,     8,     1,     1,     3,     3,     3,     3,     2,
       4,     3,     3,     8,     3,     0,     1,     3,     2,     1,
       1,     0,     2,     0,     2,     0,     1,     0,     2,     0,
       2,     0,     2,     0,     2,     0,     3,     0,     1,     2,
       1,     1,     1,     3,     1,     1,     2,     4,     1,     3,
       2,     1,     5,     0,     2,     0,     1,     3,     5,     4,
       6,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     2,     3,     2,     2,     2,     2,     4,     2,     3,
       3,     3,     4,     4,     3,     3,     4,     4,     5,     6,
       7,     9,     4,     5,     7,     9,     2,     3,     2,     3,
       3,     4,     2,     3,     3,     2,     2,     2,     2,     5,
       2,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     3,     3,     3,
       3,     4,     6,     7,     9,    10,    12,    12,    13,    14,
      15,    16,    12,    13,    15,    16,     3,     4,     5,     6,
       3,     3,     4,     3,     4,     3,     3,     5,     7,     7,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     0,    12,    19,    15,    14,
      12,    17,    13,    12,    10,     7,     9,     4,     6,     4,
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
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2356 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2364 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 223 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2378 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 213 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2392 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2403 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2411 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2420 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2429 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 273 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2443 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2454 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2464 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2484 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2494 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2504 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2514 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2528 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 256 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2542 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2552 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2568 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2577 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2585 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2593 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2615 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2624 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2633 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2642 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 293 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2655 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2664 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 283 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2678 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 283 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2692 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2702 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2711 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2725 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 243 "parser.y"
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
#line 2742 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2750 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2758 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2766 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2774 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2782 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2790 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2798 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2806 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2814 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2822 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2830 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 233 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 354 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2897 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2905 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2913 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2921 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2929 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3009 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3017 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3025 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3033 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3041 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3049 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3057 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3065 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3073 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3081 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3092 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3106 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 189 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3120 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3131 "parser.cpp"
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

#line 3239 "parser.cpp"

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
#line 494 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3454 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 498 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3465 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 504 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3476 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 511 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3482 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3488 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3494 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3500 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3506 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3512 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3518 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3524 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3530 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 520 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3536 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 521 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3542 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 522 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3548 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3554 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 524 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3560 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 526 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3566 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3572 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 528 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3578 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3584 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3590 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3596 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3602 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3608 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3614 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 535 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3620 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3626 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 537 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3632 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 544 "parser.y"
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
#line 3652 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 561 "parser.y"
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
#line 3670 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 577 "parser.y"
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
#line 3703 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 606 "parser.y"
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
#line 3723 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 622 "parser.y"
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
#line 3744 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 640 "parser.y"
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
#line 3777 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 669 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3786 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 673 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3795 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 679 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3803 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 682 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3811 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 689 "parser.y"
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
#line 3867 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 740 "parser.y"
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
#line 3909 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3915 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3921 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 781 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3927 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3933 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 783 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3939 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 784 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3945 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 785 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3951 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 786 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3957 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 787 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3963 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 788 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3969 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 789 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3975 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 790 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3981 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3987 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 792 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3993 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 793 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3999 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 794 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4005 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4011 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 796 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4017 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4023 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 798 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4029 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 799 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4035 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 802 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4041 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 804 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4047 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4053 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 806 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4059 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 807 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4065 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4071 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4077 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4083 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4089 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4095 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4101 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4107 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4113 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4119 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4125 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4131 "parser.cpp"
    break;

  case 80: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4137 "parser.cpp"
    break;

  case 81: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4143 "parser.cpp"
    break;

  case 82: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4149 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4155 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4161 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4167 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4173 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4179 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4185 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4191 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4197 "parser.cpp"
    break;

  case 91: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4203 "parser.cpp"
    break;

  case 92: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4209 "parser.cpp"
    break;

  case 93: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4215 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4221 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4227 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4233 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4239 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4245 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4251 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4257 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4263 "parser.cpp"
    break;

  case 102: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4269 "parser.cpp"
    break;

  case 103: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4275 "parser.cpp"
    break;

  case 104: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4281 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4287 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4293 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4299 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4305 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4311 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4317 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4323 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4329 "parser.cpp"
    break;

  case 113: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4335 "parser.cpp"
    break;

  case 114: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4341 "parser.cpp"
    break;

  case 115: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4347 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4353 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4359 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4365 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4371 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4377 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4383 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4389 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4395 "parser.cpp"
    break;

  case 124: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4401 "parser.cpp"
    break;

  case 125: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4407 "parser.cpp"
    break;

  case 126: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4413 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4419 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4425 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4431 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4437 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4443 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4449 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4455 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4461 "parser.cpp"
    break;

  case 135: /* column_constraints: column_constraint  */
#line 894 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4470 "parser.cpp"
    break;

  case 136: /* column_constraints: column_constraints column_constraint  */
#line 898 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4484 "parser.cpp"
    break;

  case 137: /* column_constraint: PRIMARY KEY  */
#line 908 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4492 "parser.cpp"
    break;

  case 138: /* column_constraint: UNIQUE  */
#line 911 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4500 "parser.cpp"
    break;

  case 139: /* column_constraint: NULLABLE  */
#line 914 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4508 "parser.cpp"
    break;

  case 140: /* column_constraint: NOT NULLABLE  */
#line 917 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4516 "parser.cpp"
    break;

  case 141: /* default_expr: DEFAULT constant_expr  */
#line 921 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4524 "parser.cpp"
    break;

  case 142: /* default_expr: %empty  */
#line 924 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4532 "parser.cpp"
    break;

  case 143: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 929 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4542 "parser.cpp"
    break;

  case 144: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 934 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4552 "parser.cpp"
    break;

  case 145: /* identifier_array: IDENTIFIER  */
#line 941 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4563 "parser.cpp"
    break;

  case 146: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 947 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4574 "parser.cpp"
    break;

  case 147: /* delete_statement: DELETE FROM table_name where_clause  */
#line 957 "parser.y"
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
#line 4591 "parser.cpp"
    break;

  case 148: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 973 "parser.y"
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
#line 4630 "parser.cpp"
    break;

  case 149: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1007 "parser.y"
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
#line 4647 "parser.cpp"
    break;

  case 150: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1020 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4655 "parser.cpp"
    break;

  case 151: /* optional_identifier_array: %empty  */
#line 1023 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4663 "parser.cpp"
    break;

  case 152: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1030 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4673 "parser.cpp"
    break;

  case 153: /* explain_type: ANALYZE  */
#line 1036 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4681 "parser.cpp"
    break;

  case 154: /* explain_type: AST  */
#line 1039 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4689 "parser.cpp"
    break;

  case 155: /* explain_type: RAW  */
#line 1042 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4697 "parser.cpp"
    break;

  case 156: /* explain_type: LOGICAL  */
#line 1045 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4705 "parser.cpp"
    break;

  case 157: /* explain_type: PHYSICAL  */
#line 1048 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4713 "parser.cpp"
    break;

  case 158: /* explain_type: PIPELINE  */
#line 1051 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4721 "parser.cpp"
    break;

  case 159: /* explain_type: FRAGMENT  */
#line 1054 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4729 "parser.cpp"
    break;

  case 160: /* explain_type: %empty  */
#line 1057 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4737 "parser.cpp"
    break;

  case 161: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1064 "parser.y"
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
#line 4754 "parser.cpp"
    break;

  case 162: /* update_expr_array: update_expr  */
#line 1077 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4763 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1081 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4772 "parser.cpp"
    break;

  case 164: /* update_expr: IDENTIFIER '=' expr  */
#line 1086 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4784 "parser.cpp"
    break;

  case 165: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1099 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4800 "parser.cpp"
    break;

  case 166: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1112 "parser.y"
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
#line 4818 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1127 "parser.y"
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
#line 4836 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1142 "parser.y"
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
#line 4854 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1157 "parser.y"
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
#line 4877 "parser.cpp"
    break;

  case 170: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1180 "parser.y"
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
#line 4935 "parser.cpp"
    break;

  case 171: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1233 "parser.y"
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
#line 4995 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1288 "parser.y"
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
#line 5047 "parser.cpp"
    break;

  case 173: /* select_statement: select_without_paren  */
#line 1339 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5055 "parser.cpp"
    break;

  case 174: /* select_statement: select_with_paren  */
#line 1342 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5063 "parser.cpp"
    break;

  case 175: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1345 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5077 "parser.cpp"
    break;

  case 176: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1354 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5091 "parser.cpp"
    break;

  case 177: /* select_with_paren: '(' select_without_paren ')'  */
#line 1364 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5099 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_with_paren ')'  */
#line 1367 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5107 "parser.cpp"
    break;

  case 179: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1371 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5116 "parser.cpp"
    break;

  case 180: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1376 "parser.y"
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
        delete (yyvsp[-2].order_by_expr_list_t);
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
#line 5142 "parser.cpp"
    break;

  case 181: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1398 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5150 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1401 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5158 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1406 "parser.y"
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
#line 5178 "parser.cpp"
    break;

  case 184: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1422 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5186 "parser.cpp"
    break;

  case 185: /* order_by_clause: %empty  */
#line 1425 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5194 "parser.cpp"
    break;

  case 186: /* order_by_expr_list: order_by_expr  */
#line 1429 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5203 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1433 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5212 "parser.cpp"
    break;

  case 188: /* order_by_expr: expr order_by_type  */
#line 1438 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5222 "parser.cpp"
    break;

  case 189: /* order_by_type: ASC  */
#line 1444 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5230 "parser.cpp"
    break;

  case 190: /* order_by_type: DESC  */
#line 1447 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5238 "parser.cpp"
    break;

  case 191: /* order_by_type: %empty  */
#line 1450 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5246 "parser.cpp"
    break;

  case 192: /* limit_expr: LIMIT expr  */
#line 1454 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5254 "parser.cpp"
    break;

  case 193: /* limit_expr: %empty  */
#line 1458 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5260 "parser.cpp"
    break;

  case 194: /* offset_expr: OFFSET expr  */
#line 1460 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5268 "parser.cpp"
    break;

  case 195: /* offset_expr: %empty  */
#line 1464 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5274 "parser.cpp"
    break;

  case 196: /* distinct: DISTINCT  */
#line 1466 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5282 "parser.cpp"
    break;

  case 197: /* distinct: %empty  */
#line 1469 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5290 "parser.cpp"
    break;

  case 198: /* from_clause: FROM table_reference  */
#line 1473 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5298 "parser.cpp"
    break;

  case 199: /* from_clause: %empty  */
#line 1476 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5306 "parser.cpp"
    break;

  case 200: /* search_clause: SEARCH sub_search_array  */
#line 1480 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5316 "parser.cpp"
    break;

  case 201: /* search_clause: %empty  */
#line 1485 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5324 "parser.cpp"
    break;

  case 202: /* where_clause: WHERE expr  */
#line 1489 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5332 "parser.cpp"
    break;

  case 203: /* where_clause: %empty  */
#line 1492 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5340 "parser.cpp"
    break;

  case 204: /* having_clause: HAVING expr  */
#line 1496 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5348 "parser.cpp"
    break;

  case 205: /* having_clause: %empty  */
#line 1499 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5356 "parser.cpp"
    break;

  case 206: /* group_by_clause: GROUP BY expr_array  */
#line 1503 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5364 "parser.cpp"
    break;

  case 207: /* group_by_clause: %empty  */
#line 1506 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5372 "parser.cpp"
    break;

  case 208: /* set_operator: UNION  */
#line 1510 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5380 "parser.cpp"
    break;

  case 209: /* set_operator: UNION ALL  */
#line 1513 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5388 "parser.cpp"
    break;

  case 210: /* set_operator: INTERSECT  */
#line 1516 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5396 "parser.cpp"
    break;

  case 211: /* set_operator: EXCEPT  */
#line 1519 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5404 "parser.cpp"
    break;

  case 212: /* table_reference: table_reference_unit  */
#line 1527 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5412 "parser.cpp"
    break;

  case 213: /* table_reference: table_reference ',' table_reference_unit  */
#line 1530 "parser.y"
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
#line 5430 "parser.cpp"
    break;

  case 216: /* table_reference_name: table_name table_alias  */
#line 1547 "parser.y"
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
#line 5448 "parser.cpp"
    break;

  case 217: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1561 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5459 "parser.cpp"
    break;

  case 218: /* table_name: IDENTIFIER  */
#line 1570 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5469 "parser.cpp"
    break;

  case 219: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1575 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5481 "parser.cpp"
    break;

  case 220: /* table_alias: AS IDENTIFIER  */
#line 1584 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5491 "parser.cpp"
    break;

  case 221: /* table_alias: IDENTIFIER  */
#line 1589 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5501 "parser.cpp"
    break;

  case 222: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1594 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5512 "parser.cpp"
    break;

  case 223: /* table_alias: %empty  */
#line 1600 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5520 "parser.cpp"
    break;

  case 224: /* with_clause: WITH with_expr_list  */
#line 1607 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5528 "parser.cpp"
    break;

  case 225: /* with_clause: %empty  */
#line 1610 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5536 "parser.cpp"
    break;

  case 226: /* with_expr_list: with_expr  */
#line 1614 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5545 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1617 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5554 "parser.cpp"
    break;

  case 228: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1622 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5566 "parser.cpp"
    break;

  case 229: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1634 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5578 "parser.cpp"
    break;

  case 230: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1641 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5591 "parser.cpp"
    break;

  case 231: /* join_type: INNER  */
#line 1655 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5599 "parser.cpp"
    break;

  case 232: /* join_type: LEFT  */
#line 1658 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5607 "parser.cpp"
    break;

  case 233: /* join_type: RIGHT  */
#line 1661 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5615 "parser.cpp"
    break;

  case 234: /* join_type: OUTER  */
#line 1664 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5623 "parser.cpp"
    break;

  case 235: /* join_type: FULL  */
#line 1667 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5631 "parser.cpp"
    break;

  case 236: /* join_type: CROSS  */
#line 1670 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5639 "parser.cpp"
    break;

  case 237: /* join_type: %empty  */
#line 1673 "parser.y"
                {
}
#line 5646 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW DATABASES  */
#line 1679 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5655 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLES  */
#line 1683 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5664 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW VIEWS  */
#line 1687 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5673 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW CONFIGS  */
#line 1691 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5682 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1695 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5694 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW PROFILES  */
#line 1702 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5703 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW BUFFER  */
#line 1706 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5712 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW MEMINDEX  */
#line 1710 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5721 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW QUERIES  */
#line 1714 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5730 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1718 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5740 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TRANSACTIONS  */
#line 1723 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5749 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1727 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5759 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW SESSION VARIABLES  */
#line 1732 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5768 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1736 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5777 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1740 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5788 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1746 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5799 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1752 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5810 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW TABLE table_name  */
#line 1758 "parser.y"
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
#line 5826 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1769 "parser.y"
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
#line 5842 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1780 "parser.y"
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
#line 5858 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1791 "parser.y"
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
#line 5875 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1803 "parser.y"
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
#line 5892 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1815 "parser.y"
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
#line 5910 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1828 "parser.y"
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
#line 5929 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1842 "parser.y"
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
#line 5945 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1853 "parser.y"
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
#line 5964 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1867 "parser.y"
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
#line 5985 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1883 "parser.y"
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
#line 6007 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW LOGS  */
#line 1900 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6016 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DELTA LOGS  */
#line 1904 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6025 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW CATALOGS  */
#line 1908 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6034 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1912 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6043 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1916 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6052 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1920 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6063 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW MEMORY  */
#line 1926 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6072 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1930 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6081 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1934 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6090 "parser.cpp"
    break;

  case 275: /* flush_statement: FLUSH DATA  */
#line 1942 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6099 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH LOG  */
#line 1946 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6108 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH BUFFER  */
#line 1950 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6117 "parser.cpp"
    break;

  case 278: /* optimize_statement: OPTIMIZE table_name  */
#line 1958 "parser.y"
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
#line 6132 "parser.cpp"
    break;

  case 279: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1969 "parser.y"
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
#line 6156 "parser.cpp"
    break;

  case 280: /* command_statement: USE IDENTIFIER  */
#line 1992 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6167 "parser.cpp"
    break;

  case 281: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1998 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6177 "parser.cpp"
    break;

  case 282: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2003 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6188 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2009 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 284: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2015 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6212 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2023 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6223 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2029 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6234 "parser.cpp"
    break;

  case 287: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2035 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6245 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2041 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6256 "parser.cpp"
    break;

  case 289: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2047 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6269 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2055 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6280 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2061 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6291 "parser.cpp"
    break;

  case 292: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2067 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6302 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2073 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6313 "parser.cpp"
    break;

  case 294: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2079 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6326 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2087 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6337 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2093 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6348 "parser.cpp"
    break;

  case 297: /* command_statement: LOCK TABLE table_name  */
#line 2099 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6362 "parser.cpp"
    break;

  case 298: /* command_statement: UNLOCK TABLE table_name  */
#line 2108 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6376 "parser.cpp"
    break;

  case 299: /* compact_statement: COMPACT TABLE table_name  */
#line 2118 "parser.y"
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
#line 6393 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2131 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6402 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2135 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6412 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2140 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6423 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2146 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6435 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2153 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6448 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2161 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6462 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2170 "parser.y"
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
#line 6477 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2180 "parser.y"
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
#line 6492 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2190 "parser.y"
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
#line 6508 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2201 "parser.y"
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
#line 6524 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2212 "parser.y"
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
#line 6541 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2224 "parser.y"
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
#line 6558 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2236 "parser.y"
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
#line 6573 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2246 "parser.y"
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
#line 6589 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2257 "parser.y"
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
#line 6606 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2269 "parser.y"
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
#line 6624 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW LOGS  */
#line 2282 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6633 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2286 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6643 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2291 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6653 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2296 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6664 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2302 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6673 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2306 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6682 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2310 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6693 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW NODES  */
#line 2316 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6702 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW NODE IDENTIFIER  */
#line 2320 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6713 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SET ADMIN  */
#line 2326 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6723 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SET STANDALONE  */
#line 2331 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6733 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SET LEADER USING IDENTIFIER  */
#line 2336 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6745 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING IDENTIFIER  */
#line 2343 "parser.y"
                                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6759 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING IDENTIFIER  */
#line 2352 "parser.y"
                                                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6773 "parser.cpp"
    break;

  case 330: /* expr_array: expr_alias  */
#line 2366 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6782 "parser.cpp"
    break;

  case 331: /* expr_array: expr_array ',' expr_alias  */
#line 2370 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6791 "parser.cpp"
    break;

  case 332: /* expr_array_list: '(' expr_array ')'  */
#line 2375 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6800 "parser.cpp"
    break;

  case 333: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2379 "parser.y"
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
#line 6826 "parser.cpp"
    break;

  case 334: /* expr_alias: expr AS IDENTIFIER  */
#line 2412 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6837 "parser.cpp"
    break;

  case 335: /* expr_alias: expr  */
#line 2418 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6845 "parser.cpp"
    break;

  case 341: /* operand: '(' expr ')'  */
#line 2428 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6853 "parser.cpp"
    break;

  case 342: /* operand: '(' select_without_paren ')'  */
#line 2431 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6864 "parser.cpp"
    break;

  case 343: /* operand: constant_expr  */
#line 2437 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6872 "parser.cpp"
    break;

  case 354: /* extra_match_tensor_option: ',' STRING  */
#line 2451 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6880 "parser.cpp"
    break;

  case 355: /* extra_match_tensor_option: %empty  */
#line 2454 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6888 "parser.cpp"
    break;

  case 356: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2460 "parser.y"
                                                                                                                           {
    auto match_tensor_expr = std::make_unique<infinity::MatchTensorExpr>();
    // search column
    match_tensor_expr->SetSearchColumn((yyvsp[-8].expr_t));
    // search tensor
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_tensor_expr->SetQueryTensor((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    // search method
    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_tensor_expr->SetSearchMethod((yyvsp[-2].str_value));
    // search options
    if ((yyvsp[-1].str_value)) {
        match_tensor_expr->SetExtraOptions((yyvsp[-1].str_value));
    }
    (yyval.expr_t) = match_tensor_expr.release();
}
#line 6909 "parser.cpp"
    break;

  case 357: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2479 "parser.y"
                                                                                                                                                       {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-15].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-9].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-9].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-11].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-11].str_value), (yyvsp[-13].const_expr_t));
    if (!check) {
        goto Error1;
    }
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    delete (yyvsp[-13].const_expr_t);

    match_vector_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    match_vector_expr->topn_ = (yyvsp[-7].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-11].str_value));
    free((yyvsp[-9].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-13].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return1:
    ;
}
#line 6958 "parser.cpp"
    break;

  case 358: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2524 "parser.y"
                                                                                           {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-11].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-5].str_value));
    if (!check) {
        goto Error2;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-7].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-7].str_value), (yyvsp[-9].const_expr_t));
    if (!check) {
        goto Error2;
    }
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-3].long_value);
    match_vector_expr->ignore_index_ = true;
    goto Return2;
Error2:
    free((yyvsp[-7].str_value));
    free((yyvsp[-5].str_value));
    delete (yyvsp[-9].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return2:
    ;
}
#line 7000 "parser.cpp"
    break;

  case 359: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2562 "parser.y"
                                                                                                    {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-4].str_value));
    if (!check) {
        goto Error3;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    if (!check) {
        goto Error3;
    }
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return3;
Error3:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return3:
    ;
}
#line 7046 "parser.cpp"
    break;

  case 360: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2604 "parser.y"
                                                                                     {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-8].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-2].str_value));
    if (!check) {
        goto Error4;
    }

    // vector search data type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    if (!check) {
        goto Error4;
    }
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return4;

Error4:
    for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
        delete param_ptr;
    }
    delete (yyvsp[0].with_index_param_list_t);
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);
    delete (yyval.expr_t);
    yyerror(&yyloc, scanner, result, "Invalid vector search distance type");
    YYERROR;
Return4:
    ;
}
#line 7093 "parser.cpp"
    break;

  case 361: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2650 "parser.y"
                                                                                                                                                         {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-13].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-11].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-9].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-9].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-7].long_value), (yyvsp[0].with_index_param_list_t));

    match_sparse_expr->index_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
}
#line 7118 "parser.cpp"
    break;

  case 362: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2671 "parser.y"
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

    // topn and options
    match_sparse_expr->topn_ = (yyvsp[-3].long_value);

    match_sparse_expr->ignore_index_ = true;
}
#line 7142 "parser.cpp"
    break;

  case 363: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2691 "parser.y"
                                                                                                       {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-8].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-6].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-4].str_value));

    // topn and options
    match_sparse_expr->SetOptParams((yyvsp[-2].long_value), (yyvsp[0].with_index_param_list_t));
}
#line 7164 "parser.cpp"
    break;

  case 364: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2709 "parser.y"
                                                                                        {
    auto match_sparse_expr = new infinity::MatchSparseExpr();
    (yyval.expr_t) = match_sparse_expr;

    // search column
    match_sparse_expr->SetSearchColumn((yyvsp[-6].expr_t));

    // search sparse and data type
    match_sparse_expr->SetQuerySparse((yyvsp[-4].const_expr_t));

    // metric type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    match_sparse_expr->SetMetricType((yyvsp[-2].str_value));

    // topn and options
    match_sparse_expr->SetOptParams(infinity::DEFAULT_MATCH_SPARSE_TOP_N, (yyvsp[0].with_index_param_list_t));
}
#line 7186 "parser.cpp"
    break;

  case 365: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2727 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7199 "parser.cpp"
    break;

  case 366: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2735 "parser.y"
                                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-5].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7214 "parser.cpp"
    break;

  case 367: /* query_expr: QUERY '(' STRING ')'  */
#line 2746 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7225 "parser.cpp"
    break;

  case 368: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2752 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7238 "parser.cpp"
    break;

  case 369: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2761 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7249 "parser.cpp"
    break;

  case 370: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2767 "parser.y"
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
#line 7265 "parser.cpp"
    break;

  case 371: /* sub_search: match_vector_expr  */
#line 2779 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7273 "parser.cpp"
    break;

  case 372: /* sub_search: match_text_expr  */
#line 2782 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7281 "parser.cpp"
    break;

  case 373: /* sub_search: match_tensor_expr  */
#line 2785 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7289 "parser.cpp"
    break;

  case 374: /* sub_search: match_sparse_expr  */
#line 2788 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7297 "parser.cpp"
    break;

  case 375: /* sub_search: query_expr  */
#line 2791 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7305 "parser.cpp"
    break;

  case 376: /* sub_search: fusion_expr  */
#line 2794 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7313 "parser.cpp"
    break;

  case 377: /* sub_search_array: sub_search  */
#line 2798 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7322 "parser.cpp"
    break;

  case 378: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2802 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7331 "parser.cpp"
    break;

  case 379: /* function_expr: IDENTIFIER '(' ')'  */
#line 2807 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7344 "parser.cpp"
    break;

  case 380: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2815 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7357 "parser.cpp"
    break;

  case 381: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2823 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7371 "parser.cpp"
    break;

  case 382: /* function_expr: operand IS NOT NULLABLE  */
#line 2832 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7383 "parser.cpp"
    break;

  case 383: /* function_expr: operand IS NULLABLE  */
#line 2839 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7395 "parser.cpp"
    break;

  case 384: /* function_expr: NOT operand  */
#line 2846 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7407 "parser.cpp"
    break;

  case 385: /* function_expr: '-' operand  */
#line 2853 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7419 "parser.cpp"
    break;

  case 386: /* function_expr: '+' operand  */
#line 2860 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7431 "parser.cpp"
    break;

  case 387: /* function_expr: operand '-' operand  */
#line 2867 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7444 "parser.cpp"
    break;

  case 388: /* function_expr: operand '+' operand  */
#line 2875 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7457 "parser.cpp"
    break;

  case 389: /* function_expr: operand '*' operand  */
#line 2883 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7470 "parser.cpp"
    break;

  case 390: /* function_expr: operand '/' operand  */
#line 2891 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7483 "parser.cpp"
    break;

  case 391: /* function_expr: operand '%' operand  */
#line 2899 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7496 "parser.cpp"
    break;

  case 392: /* function_expr: operand '=' operand  */
#line 2907 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7509 "parser.cpp"
    break;

  case 393: /* function_expr: operand EQUAL operand  */
#line 2915 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7522 "parser.cpp"
    break;

  case 394: /* function_expr: operand NOT_EQ operand  */
#line 2923 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7535 "parser.cpp"
    break;

  case 395: /* function_expr: operand '<' operand  */
#line 2931 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7548 "parser.cpp"
    break;

  case 396: /* function_expr: operand '>' operand  */
#line 2939 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7561 "parser.cpp"
    break;

  case 397: /* function_expr: operand LESS_EQ operand  */
#line 2947 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7574 "parser.cpp"
    break;

  case 398: /* function_expr: operand GREATER_EQ operand  */
#line 2955 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7587 "parser.cpp"
    break;

  case 399: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2963 "parser.y"
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
#line 7622 "parser.cpp"
    break;

  case 400: /* function_expr: operand LIKE operand  */
#line 2993 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7635 "parser.cpp"
    break;

  case 401: /* function_expr: operand NOT LIKE operand  */
#line 3001 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7648 "parser.cpp"
    break;

  case 402: /* conjunction_expr: expr AND expr  */
#line 3010 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7661 "parser.cpp"
    break;

  case 403: /* conjunction_expr: expr OR expr  */
#line 3018 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7674 "parser.cpp"
    break;

  case 404: /* between_expr: operand BETWEEN operand AND operand  */
#line 3027 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7686 "parser.cpp"
    break;

  case 405: /* in_expr: operand IN '(' expr_array ')'  */
#line 3035 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7697 "parser.cpp"
    break;

  case 406: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3041 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7708 "parser.cpp"
    break;

  case 407: /* case_expr: CASE expr case_check_array END  */
#line 3048 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7719 "parser.cpp"
    break;

  case 408: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3054 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7731 "parser.cpp"
    break;

  case 409: /* case_expr: CASE case_check_array END  */
#line 3061 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7741 "parser.cpp"
    break;

  case 410: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3066 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7752 "parser.cpp"
    break;

  case 411: /* case_check_array: WHEN expr THEN expr  */
#line 3073 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7764 "parser.cpp"
    break;

  case 412: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3080 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7776 "parser.cpp"
    break;

  case 413: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3088 "parser.y"
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
#line 7807 "parser.cpp"
    break;

  case 414: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3115 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7818 "parser.cpp"
    break;

  case 415: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3121 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7829 "parser.cpp"
    break;

  case 416: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3127 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7841 "parser.cpp"
    break;

  case 417: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3134 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7853 "parser.cpp"
    break;

  case 418: /* column_expr: IDENTIFIER  */
#line 3142 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7865 "parser.cpp"
    break;

  case 419: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3149 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7877 "parser.cpp"
    break;

  case 420: /* column_expr: '*'  */
#line 3156 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7887 "parser.cpp"
    break;

  case 421: /* column_expr: column_expr '.' '*'  */
#line 3161 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7901 "parser.cpp"
    break;

  case 422: /* constant_expr: STRING  */
#line 3171 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7911 "parser.cpp"
    break;

  case 423: /* constant_expr: TRUE  */
#line 3176 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7921 "parser.cpp"
    break;

  case 424: /* constant_expr: FALSE  */
#line 3181 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7931 "parser.cpp"
    break;

  case 425: /* constant_expr: DOUBLE_VALUE  */
#line 3186 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7941 "parser.cpp"
    break;

  case 426: /* constant_expr: LONG_VALUE  */
#line 3191 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7951 "parser.cpp"
    break;

  case 427: /* constant_expr: DATE STRING  */
#line 3196 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7961 "parser.cpp"
    break;

  case 428: /* constant_expr: TIME STRING  */
#line 3201 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7971 "parser.cpp"
    break;

  case 429: /* constant_expr: DATETIME STRING  */
#line 3206 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7981 "parser.cpp"
    break;

  case 430: /* constant_expr: TIMESTAMP STRING  */
#line 3211 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7991 "parser.cpp"
    break;

  case 431: /* constant_expr: INTERVAL interval_expr  */
#line 3216 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7999 "parser.cpp"
    break;

  case 432: /* constant_expr: interval_expr  */
#line 3219 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8007 "parser.cpp"
    break;

  case 433: /* constant_expr: common_array_expr  */
#line 3222 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8015 "parser.cpp"
    break;

  case 434: /* common_array_expr: array_expr  */
#line 3226 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8023 "parser.cpp"
    break;

  case 435: /* common_array_expr: subarray_array_expr  */
#line 3229 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8031 "parser.cpp"
    break;

  case 436: /* common_array_expr: sparse_array_expr  */
#line 3232 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8039 "parser.cpp"
    break;

  case 437: /* common_array_expr: empty_array_expr  */
#line 3235 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8047 "parser.cpp"
    break;

  case 438: /* common_sparse_array_expr: sparse_array_expr  */
#line 3239 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8055 "parser.cpp"
    break;

  case 439: /* common_sparse_array_expr: array_expr  */
#line 3242 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8063 "parser.cpp"
    break;

  case 440: /* common_sparse_array_expr: empty_array_expr  */
#line 3245 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8071 "parser.cpp"
    break;

  case 441: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3249 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8079 "parser.cpp"
    break;

  case 442: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3253 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8089 "parser.cpp"
    break;

  case 443: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3258 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8098 "parser.cpp"
    break;

  case 444: /* sparse_array_expr: long_sparse_array_expr  */
#line 3263 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8106 "parser.cpp"
    break;

  case 445: /* sparse_array_expr: double_sparse_array_expr  */
#line 3266 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8114 "parser.cpp"
    break;

  case 446: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3270 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8122 "parser.cpp"
    break;

  case 447: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3274 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8134 "parser.cpp"
    break;

  case 448: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3281 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8145 "parser.cpp"
    break;

  case 449: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3288 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8153 "parser.cpp"
    break;

  case 450: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3292 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8165 "parser.cpp"
    break;

  case 451: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3299 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8176 "parser.cpp"
    break;

  case 452: /* empty_array_expr: '[' ']'  */
#line 3306 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8184 "parser.cpp"
    break;

  case 453: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3310 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8192 "parser.cpp"
    break;

  case 454: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3314 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8200 "parser.cpp"
    break;

  case 455: /* array_expr: long_array_expr  */
#line 3318 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8208 "parser.cpp"
    break;

  case 456: /* array_expr: double_array_expr  */
#line 3321 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8216 "parser.cpp"
    break;

  case 457: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3325 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8224 "parser.cpp"
    break;

  case 458: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3329 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8234 "parser.cpp"
    break;

  case 459: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3334 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8243 "parser.cpp"
    break;

  case 460: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3339 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8251 "parser.cpp"
    break;

  case 461: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3343 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8261 "parser.cpp"
    break;

  case 462: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3348 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8270 "parser.cpp"
    break;

  case 463: /* interval_expr: LONG_VALUE SECONDS  */
#line 3353 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8281 "parser.cpp"
    break;

  case 464: /* interval_expr: LONG_VALUE SECOND  */
#line 3359 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8292 "parser.cpp"
    break;

  case 465: /* interval_expr: LONG_VALUE MINUTES  */
#line 3365 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8303 "parser.cpp"
    break;

  case 466: /* interval_expr: LONG_VALUE MINUTE  */
#line 3371 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8314 "parser.cpp"
    break;

  case 467: /* interval_expr: LONG_VALUE HOURS  */
#line 3377 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8325 "parser.cpp"
    break;

  case 468: /* interval_expr: LONG_VALUE HOUR  */
#line 3383 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8336 "parser.cpp"
    break;

  case 469: /* interval_expr: LONG_VALUE DAYS  */
#line 3389 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8347 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE DAY  */
#line 3395 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8358 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE MONTHS  */
#line 3401 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8369 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE MONTH  */
#line 3407 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8380 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE YEARS  */
#line 3413 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8391 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE YEAR  */
#line 3419 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8402 "parser.cpp"
    break;

  case 475: /* copy_option_list: copy_option  */
#line 3430 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8411 "parser.cpp"
    break;

  case 476: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3434 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8420 "parser.cpp"
    break;

  case 477: /* copy_option: FORMAT IDENTIFIER  */
#line 3439 "parser.y"
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
#line 8456 "parser.cpp"
    break;

  case 478: /* copy_option: DELIMITER STRING  */
#line 3470 "parser.y"
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
#line 8471 "parser.cpp"
    break;

  case 479: /* copy_option: HEADER  */
#line 3480 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8481 "parser.cpp"
    break;

  case 480: /* copy_option: OFFSET LONG_VALUE  */
#line 3485 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8491 "parser.cpp"
    break;

  case 481: /* copy_option: LIMIT LONG_VALUE  */
#line 3490 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8501 "parser.cpp"
    break;

  case 482: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3495 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8511 "parser.cpp"
    break;

  case 483: /* file_path: STRING  */
#line 3501 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8519 "parser.cpp"
    break;

  case 484: /* if_exists: IF EXISTS  */
#line 3505 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8525 "parser.cpp"
    break;

  case 485: /* if_exists: %empty  */
#line 3506 "parser.y"
  { (yyval.bool_value) = false; }
#line 8531 "parser.cpp"
    break;

  case 486: /* if_not_exists: IF NOT EXISTS  */
#line 3508 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8537 "parser.cpp"
    break;

  case 487: /* if_not_exists: %empty  */
#line 3509 "parser.y"
  { (yyval.bool_value) = false; }
#line 8543 "parser.cpp"
    break;

  case 490: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3524 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8556 "parser.cpp"
    break;

  case 491: /* if_not_exists_info: %empty  */
#line 3532 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8564 "parser.cpp"
    break;

  case 492: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3536 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8572 "parser.cpp"
    break;

  case 493: /* with_index_param_list: %empty  */
#line 3539 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8580 "parser.cpp"
    break;

  case 494: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3543 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8588 "parser.cpp"
    break;

  case 495: /* optional_table_properties_list: %empty  */
#line 3546 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8596 "parser.cpp"
    break;

  case 496: /* index_param_list: index_param  */
#line 3550 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8605 "parser.cpp"
    break;

  case 497: /* index_param_list: index_param_list ',' index_param  */
#line 3554 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8614 "parser.cpp"
    break;

  case 498: /* index_param: IDENTIFIER  */
#line 3559 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8625 "parser.cpp"
    break;

  case 499: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3565 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8640 "parser.cpp"
    break;

  case 500: /* index_param: IDENTIFIER '=' STRING  */
#line 3575 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8655 "parser.cpp"
    break;

  case 501: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3585 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8668 "parser.cpp"
    break;

  case 502: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3593 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8681 "parser.cpp"
    break;

  case 503: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3604 "parser.y"
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
#line 8717 "parser.cpp"
    break;

  case 504: /* index_info: '(' IDENTIFIER ')'  */
#line 3635 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8728 "parser.cpp"
    break;


#line 8732 "parser.cpp"

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

#line 3642 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

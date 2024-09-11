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
  YYSYMBOL_explain_type = 240,             /* explain_type  */
  YYSYMBOL_update_statement = 241,         /* update_statement  */
  YYSYMBOL_update_expr_array = 242,        /* update_expr_array  */
  YYSYMBOL_update_expr = 243,              /* update_expr  */
  YYSYMBOL_drop_statement = 244,           /* drop_statement  */
  YYSYMBOL_copy_statement = 245,           /* copy_statement  */
  YYSYMBOL_select_statement = 246,         /* select_statement  */
  YYSYMBOL_select_with_paren = 247,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 248,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 249, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 250, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 251, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 252,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 253,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 254,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 255,            /* order_by_type  */
  YYSYMBOL_limit_expr = 256,               /* limit_expr  */
  YYSYMBOL_offset_expr = 257,              /* offset_expr  */
  YYSYMBOL_distinct = 258,                 /* distinct  */
  YYSYMBOL_from_clause = 259,              /* from_clause  */
  YYSYMBOL_search_clause = 260,            /* search_clause  */
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
  YYSYMBOL_extra_match_tensor_option = 287, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 288,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 289,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 290,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 291,          /* match_text_expr  */
  YYSYMBOL_query_expr = 292,               /* query_expr  */
  YYSYMBOL_fusion_expr = 293,              /* fusion_expr  */
  YYSYMBOL_sub_search = 294,               /* sub_search  */
  YYSYMBOL_sub_search_array = 295,         /* sub_search_array  */
  YYSYMBOL_function_expr = 296,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 297,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 298,             /* between_expr  */
  YYSYMBOL_in_expr = 299,                  /* in_expr  */
  YYSYMBOL_case_expr = 300,                /* case_expr  */
  YYSYMBOL_case_check_array = 301,         /* case_check_array  */
  YYSYMBOL_cast_expr = 302,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 303,            /* subquery_expr  */
  YYSYMBOL_column_expr = 304,              /* column_expr  */
  YYSYMBOL_constant_expr = 305,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 306,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 307, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 308,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 309, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 310,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 311,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 312, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 313, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 314, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 315,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 316,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 317,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 318,               /* array_expr  */
  YYSYMBOL_long_array_expr = 319,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 320, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 321,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 322, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 323,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 324,         /* copy_option_list  */
  YYSYMBOL_copy_option = 325,              /* copy_option  */
  YYSYMBOL_file_path = 326,                /* file_path  */
  YYSYMBOL_if_exists = 327,                /* if_exists  */
  YYSYMBOL_if_not_exists = 328,            /* if_not_exists  */
  YYSYMBOL_semicolon = 329,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 330,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 331,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 332, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 333,         /* index_param_list  */
  YYSYMBOL_index_param = 334,              /* index_param  */
  YYSYMBOL_index_info = 335                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 464 "parser.cpp"

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
#define YYLAST   1398

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  221
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  511
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1127

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
       0,   495,   495,   499,   505,   512,   513,   514,   515,   516,
     517,   518,   519,   520,   521,   522,   523,   524,   525,   526,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     538,   539,   546,   563,   579,   608,   624,   642,   671,   675,
     681,   684,   691,   742,   781,   782,   783,   784,   785,   786,
     787,   788,   789,   790,   791,   792,   793,   794,   795,   796,
     797,   798,   799,   800,   801,   804,   806,   807,   808,   809,
     812,   813,   814,   815,   816,   817,   818,   819,   820,   821,
     822,   823,   824,   825,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   836,   837,   838,   839,   840,   841,
     842,   843,   844,   845,   846,   847,   848,   849,   850,   851,
     852,   853,   854,   855,   856,   857,   858,   859,   860,   861,
     862,   863,   864,   865,   866,   867,   868,   869,   870,   871,
     872,   873,   874,   875,   876,   877,   896,   900,   910,   913,
     916,   919,   923,   926,   931,   936,   943,   949,   959,   975,
    1009,  1022,  1025,  1032,  1038,  1041,  1044,  1047,  1050,  1053,
    1056,  1059,  1066,  1079,  1083,  1088,  1101,  1114,  1129,  1144,
    1159,  1182,  1235,  1290,  1341,  1344,  1347,  1356,  1366,  1369,
    1373,  1378,  1400,  1403,  1408,  1424,  1427,  1431,  1435,  1440,
    1446,  1449,  1452,  1456,  1460,  1462,  1466,  1468,  1471,  1475,
    1478,  1482,  1487,  1491,  1494,  1498,  1501,  1505,  1508,  1512,
    1515,  1518,  1521,  1529,  1532,  1547,  1547,  1549,  1563,  1572,
    1577,  1586,  1591,  1596,  1602,  1609,  1612,  1616,  1619,  1624,
    1636,  1643,  1657,  1660,  1663,  1666,  1669,  1672,  1675,  1681,
    1685,  1689,  1693,  1697,  1704,  1708,  1712,  1716,  1720,  1725,
    1729,  1734,  1738,  1742,  1748,  1754,  1760,  1771,  1782,  1793,
    1805,  1817,  1830,  1844,  1855,  1869,  1885,  1902,  1906,  1910,
    1914,  1918,  1922,  1928,  1932,  1936,  1944,  1948,  1952,  1960,
    1971,  1994,  2000,  2005,  2011,  2017,  2025,  2031,  2037,  2043,
    2049,  2057,  2063,  2069,  2075,  2081,  2089,  2095,  2101,  2110,
    2120,  2133,  2137,  2142,  2148,  2155,  2163,  2172,  2182,  2192,
    2203,  2214,  2226,  2238,  2248,  2259,  2271,  2284,  2288,  2293,
    2298,  2304,  2308,  2312,  2318,  2322,  2328,  2332,  2337,  2342,
    2349,  2358,  2368,  2374,  2379,  2385,  2390,  2403,  2407,  2412,
    2416,  2449,  2455,  2459,  2460,  2461,  2462,  2463,  2465,  2468,
    2474,  2477,  2478,  2479,  2480,  2481,  2482,  2483,  2484,  2485,
    2486,  2488,  2491,  2497,  2516,  2561,  2599,  2641,  2687,  2708,
    2728,  2746,  2764,  2772,  2783,  2789,  2798,  2804,  2816,  2819,
    2822,  2825,  2828,  2831,  2835,  2839,  2844,  2852,  2860,  2869,
    2876,  2883,  2890,  2897,  2904,  2912,  2920,  2928,  2936,  2944,
    2952,  2960,  2968,  2976,  2984,  2992,  3000,  3030,  3038,  3047,
    3055,  3064,  3072,  3078,  3085,  3091,  3098,  3103,  3110,  3117,
    3125,  3152,  3158,  3164,  3171,  3179,  3186,  3193,  3198,  3208,
    3213,  3218,  3223,  3228,  3233,  3238,  3243,  3248,  3253,  3256,
    3259,  3263,  3266,  3269,  3272,  3276,  3279,  3282,  3286,  3290,
    3295,  3300,  3303,  3307,  3311,  3318,  3325,  3329,  3336,  3343,
    3347,  3351,  3355,  3358,  3362,  3366,  3371,  3376,  3380,  3385,
    3390,  3396,  3402,  3408,  3414,  3420,  3426,  3432,  3438,  3444,
    3450,  3456,  3467,  3471,  3476,  3507,  3517,  3522,  3527,  3532,
    3538,  3542,  3543,  3545,  3546,  3548,  3549,  3561,  3569,  3573,
    3576,  3580,  3583,  3587,  3591,  3596,  3602,  3612,  3622,  3630,
    3641,  3672
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
  "compact_statement", "admin_statement", "alter_statement", "expr_array",
  "expr_array_list", "expr_alias", "expr", "operand",
  "extra_match_tensor_option", "match_tensor_expr", "match_vector_expr",
  "match_sparse_expr", "match_text_expr", "query_expr", "fusion_expr",
  "sub_search", "sub_search_array", "function_expr", "conjunction_expr",
  "between_expr", "in_expr", "case_expr", "case_check_array", "cast_expr",
  "subquery_expr", "column_expr", "constant_expr", "common_array_expr",
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

#define YYPACT_NINF (-577)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-499)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      70,   280,    32,   335,   100,    29,   100,    71,   440,   573,
      95,   131,   165,   201,   125,   261,   271,   303,   127,    22,
     -13,   308,    99,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,   251,  -577,  -577,   330,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,   267,   267,   267,   267,    48,   100,   274,   274,
     274,   274,   274,   141,   349,   100,     6,   362,   395,   403,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,   601,   407,   100,
    -577,  -577,  -577,  -577,  -577,   383,  -577,   283,   408,  -577,
     415,  -577,   244,  -577,  -577,   257,  -577,   356,   147,   100,
     100,   100,   100,  -577,  -577,  -577,  -577,    -4,  -577,   372,
     220,  -577,   429,    60,   219,   439,   262,   269,  -577,    43,
    -577,   451,  -577,  -577,     3,   449,  -577,   454,   443,   499,
     100,   100,   100,   528,   469,   328,   467,   559,   100,   100,
     100,   594,   600,   602,   551,   625,   625,   460,   119,   133,
     186,  -577,  -577,  -577,  -577,  -577,  -577,  -577,   251,  -577,
    -577,  -577,  -577,  -577,  -577,   316,  -577,  -577,   630,  -577,
     632,  -577,  -577,   639,   645,  -577,  -577,  -577,  -577,   226,
    -577,  -577,  -577,   100,   444,   303,   625,  -577,   503,  -577,
     672,  -577,  -577,   680,  -577,  -577,   678,  -577,   686,   633,
    -577,  -577,  -577,  -577,     3,  -577,  -577,  -577,   460,   634,
     621,   617,  -577,   -27,  -577,   328,  -577,   100,   695,    24,
    -577,  -577,  -577,  -577,  -577,   631,  -577,   495,   -21,  -577,
     460,  -577,  -577,   618,   619,   489,  -577,  -577,   432,   548,
     490,   491,   315,   703,   705,   707,   708,  -577,  -577,   709,
     498,   152,   501,   502,   584,   584,  -577,    12,   350,   168,
    -577,     0,   652,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,   497,  -577,  -577,  -577,
     -77,  -577,  -577,   -52,  -577,    47,  -577,  -577,  -577,    72,
    -577,   117,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,   715,   713,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,   667,   671,   692,
      -7,   636,   330,  -577,  -577,   722,   193,  -577,   735,  -577,
      55,   534,   535,   -47,   460,   460,   689,  -577,   -13,    34,
     701,   550,  -577,   212,   552,  -577,   100,   460,   602,  -577,
     493,   554,   561,   204,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,   584,   562,   676,   682,
     460,   460,   110,   216,  -577,  -577,  -577,  -577,   432,  -577,
     770,   563,   572,   576,   580,   785,   792,   359,   359,  -577,
     578,  -577,  -577,  -577,  -577,   585,    86,   717,   460,   800,
     460,   460,    -6,   589,   -37,   584,   584,   584,   584,   584,
     584,   584,   584,   584,   584,   584,   584,   584,   584,    17,
    -577,   597,  -577,   805,  -577,   808,  -577,   809,  -577,   812,
     768,   605,   822,   823,   823,   825,   830,   620,  -577,   622,
    -577,   828,  -577,    69,   668,   677,  -577,  -577,     7,   661,
     627,  -577,    73,   493,   460,  -577,   251,   806,   706,   635,
     255,  -577,  -577,  -577,   -13,   844,  -577,  -577,   846,   460,
     637,  -577,   493,  -577,    45,    45,   460,  -577,   279,   682,
     690,   638,     2,    -2,   248,  -577,   460,   460,   771,   460,
     848,    18,   460,   281,   292,   656,  -577,  -577,   625,  -577,
    -577,  -577,   697,   650,   584,   350,   731,  -577,   687,   687,
     167,   167,   666,   687,   687,   167,   167,   359,   359,  -577,
    -577,  -577,  -577,  -577,  -577,   647,  -577,   648,  -577,  -577,
    -577,   863,   864,  -577,  -577,  -577,  -577,   796,  -577,   877,
    -577,  -577,   883,  -577,   886,   887,   -13,   698,   382,  -577,
      62,  -577,   223,   551,   460,  -577,  -577,  -577,   493,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
     710,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,   712,   716,   727,   728,   732,   733,   198,
     734,   695,   892,    34,   251,   730,  -577,   293,   744,   922,
     926,   946,   957,  -577,   958,   297,  -577,   298,   302,  -577,
     747,  -577,   806,   460,  -577,   460,   -49,     5,   584,  -113,
     746,  -577,   -71,    79,  -577,   962,  -577,   972,  -577,  -577,
     896,   350,   687,   761,   310,  -577,   584,   975,   977,   936,
     929,   991,   793,   325,  -577,   993,  -577,  -577,     4,     7,
     934,  -577,  -577,  -577,  -577,  -577,  -577,   937,  -577,  1007,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,   795,   953,
    -577,  1009,   842,   859,   876,   893,   910,   927,   881,   884,
    -577,  -577,   214,  -577,   890,   695,   326,   813,  -577,  -577,
     865,  -577,   460,  -577,  -577,  -577,  -577,  -577,  -577,    45,
    -577,  -577,  -577,   815,   493,    46,  -577,   460,   574,   819,
    1030,   597,   826,   824,   827,   829,   831,   345,  -577,  -577,
     676,  1035,  1036,  -577,   464,  -577,   877,   183,    62,   382,
       7,     7,   833,   223,   986,   989,   375,   832,   834,   835,
     836,   837,   838,   839,   840,   841,   949,   843,   845,   847,
     849,   850,   851,   852,   853,   854,   855,   964,   856,   857,
     858,   860,   861,   862,   866,   867,   868,   869,   966,   870,
     871,   872,   873,   874,   875,   878,   879,   880,   882,   968,
     885,   888,   889,   891,   894,   895,   897,   898,   899,   900,
     970,   901,   902,   903,   904,   905,   906,   907,   908,   909,
     911,   981,   912,  -577,  -577,   189,  -577,  -577,  -577,   376,
    -577,   877,  1047,   377,  -577,  -577,  -577,   493,  -577,   659,
     913,   385,   914,     9,   915,  -577,  -577,  -577,  -577,  -577,
      45,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    1076,  -577,  -577,  -577,  1015,   695,  -577,   460,   460,  -577,
    -577,  1078,  1089,  1090,  1094,  1096,  1097,  1099,  1100,  1103,
    1105,   916,  1106,  1109,  1123,  1130,  1131,  1132,  1133,  1134,
    1135,  1136,   924,  1138,  1139,  1140,  1141,  1142,  1143,  1144,
    1145,  1146,  1147,   935,  1149,  1150,  1151,  1152,  1153,  1154,
    1155,  1156,  1157,  1158,   947,  1159,  1161,  1162,  1163,  1164,
    1165,  1166,  1167,  1168,  1169,   959,  1170,  1171,  1173,  1174,
    1175,  1176,  1177,  1178,  1179,  1180,   969,  1181,  -577,  -577,
     400,   636,  -577,  -577,  1185,  -577,  1186,  1187,  1188,   418,
    1189,   460,   423,   974,   493,   978,   980,   982,   983,   984,
     985,   987,   988,   990,   992,  1191,   994,   995,   996,   997,
     998,   999,  1000,  1001,  1002,  1003,  1196,  1004,  1005,  1006,
    1008,  1010,  1011,  1012,  1013,  1014,  1016,  1199,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1024,  1025,  1026,  1201,  1027,
    1028,  1029,  1031,  1032,  1033,  1034,  1037,  1038,  1039,  1203,
    1040,  1041,  1042,  1043,  1044,  1045,  1046,  1048,  1049,  1050,
    1217,  1051,  -577,  -577,  1052,  1053,  1054,   424,  -577,   386,
     493,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  1056,  -577,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  1058,  -577,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  1059,  -577,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  1060,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,  1061,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,  -577,  -577,  1062,  -577,  1221,
    -577,  1227,   636,  1240,  1245,  1246,  -577,  -577,  -577,  -577,
    -577,  -577,  -577,  -577,  -577,   431,  1063,  -577,  1064,  1257,
     614,   636,  1262,  1266,  1065,   -40,   626,  1273,  -577,  -577,
    1067,  -577,  -577,  1238,  1239,  -577,  1280,  -577,  1232,   -35,
    -577,  1073,  -577,  -577,  1243,  1244,  -577,  1288,  -577,  1077,
    1079,  1290,   636,  1080,  -577,   636,  -577
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     226,     0,     0,     0,     0,     0,     0,     0,   161,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     226,     0,   496,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   175,   174,     0,     8,    14,    15,    16,    17,
      18,    19,   494,   494,   494,   494,   494,     0,   492,   492,
     492,   492,   492,   219,     0,     0,     0,     0,     0,     0,
     155,   159,   156,   157,   158,   160,   154,   226,     0,     0,
     240,   241,   239,   245,   249,     0,   246,     0,     0,   242,
       0,   244,     0,   267,   269,     0,   247,     0,   273,     0,
       0,     0,     0,   276,   277,   278,   281,   219,   279,     0,
     225,   227,     0,     0,     0,     0,     0,     0,     1,   226,
       2,   209,   211,   212,     0,   198,   180,   186,     0,     0,
       0,     0,     0,     0,     0,   152,     0,     0,     0,     0,
       0,     0,     0,     0,   204,     0,     0,     0,     0,     0,
       0,   153,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,   255,   256,   250,   251,     0,   252,
       0,   243,   268,     0,     0,   271,   270,   274,   275,     0,
     300,   298,   299,     0,     0,     0,     0,   327,     0,   328,
       0,   321,   322,     0,   317,   301,     0,   324,   326,     0,
     179,   178,     4,   210,     0,   176,   177,   197,     0,     0,
     194,     0,    32,     0,    33,   152,   497,     0,     0,   226,
     491,   166,   168,   167,   169,     0,   220,     0,   204,   163,
       0,   148,   490,     0,     0,   425,   429,   432,   433,     0,
       0,     0,     0,     0,     0,     0,     0,   430,   431,     0,
       0,     0,     0,     0,     0,     0,   427,     0,   226,     0,
     337,   342,   343,   357,   355,   358,   356,   359,   360,   352,
     347,   346,   345,   353,   354,   344,   351,   350,   440,   442,
       0,   443,   451,     0,   452,     0,   444,   441,   462,     0,
     463,     0,   439,   285,   287,   286,   283,   284,   290,   292,
     291,   288,   289,   295,   297,   296,   293,   294,     0,     0,
     258,   257,   263,   253,   254,   248,   272,     0,     0,     0,
       0,   500,     0,   228,   282,     0,   318,   323,   302,   325,
       0,     0,     0,   200,     0,     0,   196,   493,   226,     0,
       0,     0,   146,     0,     0,   150,     0,     0,     0,   162,
     203,     0,     0,     0,   471,   470,   473,   472,   475,   474,
     477,   476,   479,   478,   481,   480,     0,     0,   391,   226,
       0,     0,     0,     0,   434,   435,   436,   437,     0,   438,
       0,     0,     0,     0,     0,     0,     0,   393,   392,   468,
     465,   459,   449,   454,   457,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     448,     0,   453,     0,   456,     0,   464,     0,   467,     0,
     264,   259,     0,     0,     0,     0,     0,     0,   280,     0,
     329,     0,   319,     0,     0,     0,   183,   182,     0,   202,
     185,   187,   192,   193,     0,   181,    35,     0,     0,     0,
       0,    38,    40,    41,   226,     0,    37,   151,     0,     0,
     149,   170,   165,   164,     0,     0,     0,   386,     0,   226,
       0,     0,     0,     0,     0,   416,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   349,   348,     0,   338,
     341,   409,   410,     0,     0,   226,     0,   390,   400,   401,
     404,   405,     0,   407,   399,   402,   403,   395,   394,   396,
     397,   398,   426,   428,   450,     0,   455,     0,   458,   466,
     469,     0,     0,   260,   334,   335,   333,     0,   332,     0,
     229,   320,     0,   303,     0,     0,   226,   199,   213,   215,
     224,   216,     0,   204,     0,   190,   191,   189,   195,    44,
      47,    48,    45,    46,    49,    50,    66,    51,    53,    52,
      69,    56,    57,    58,    54,    55,    59,    60,    61,    62,
      63,    64,    65,     0,     0,     0,     0,     0,     0,   500,
       0,     0,   502,     0,    36,     0,   147,     0,     0,     0,
       0,     0,     0,   486,     0,     0,   482,     0,     0,   387,
       0,   421,     0,     0,   414,     0,     0,     0,     0,     0,
       0,   425,     0,     0,   374,     0,   376,     0,   461,   460,
       0,   226,   408,     0,     0,   389,     0,     0,     0,   265,
     261,     0,   505,     0,   503,   304,   330,   331,     0,     0,
       0,   233,   234,   235,   236,   232,   237,     0,   222,     0,
     217,   380,   378,   381,   379,   382,   383,   384,   201,   208,
     188,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     139,   140,   143,   136,   143,     0,     0,     0,    34,    39,
     511,   339,     0,   488,   487,   485,   484,   489,   173,     0,
     171,   388,   422,     0,   418,     0,   417,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   423,   412,
     411,     0,     0,   336,     0,   499,     0,     0,   224,   214,
       0,     0,   221,     0,     0,   206,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   138,     0,   137,    43,    42,     0,
     145,     0,     0,     0,   483,   420,   415,   419,   406,     0,
       0,     0,     0,     0,     0,   445,   447,   446,   375,   377,
       0,   424,   413,   266,   262,   506,   507,   509,   508,   504,
       0,   305,   218,   230,     0,     0,   385,     0,     0,   184,
      68,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,   144,
       0,   500,   340,   465,     0,   372,     0,     0,     0,     0,
     306,     0,     0,   207,   205,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   501,   510,     0,     0,     0,     0,   172,     0,
     231,   223,    67,    73,    74,    71,    72,    75,    76,    77,
      78,    79,     0,    70,   117,   118,   115,   116,   119,   120,
     121,   122,   123,     0,   114,    84,    85,    82,    83,    86,
      87,    88,    89,    90,     0,    81,    95,    96,    93,    94,
      97,    98,    99,   100,   101,     0,    92,   128,   129,   126,
     127,   130,   131,   132,   133,   134,     0,   125,   106,   107,
     104,   105,   108,   109,   110,   111,   112,     0,   103,     0,
     373,     0,   500,     0,     0,     0,   308,   307,   313,    80,
     124,    91,   102,   135,   113,     0,   362,   371,     0,   314,
     309,   500,     0,     0,     0,   500,     0,     0,   310,   367,
       0,   361,   363,     0,     0,   370,     0,   315,   311,   500,
     369,     0,   316,   312,     0,     0,   366,     0,   365,     0,
       0,     0,   500,     0,   368,   500,   364
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -577,  -577,  -577,  1190,  -577,  1230,  -577,   711,   256,   696,
    -577,   628,   629,  -577,  -568,  1234,  1235,  1101,  -577,  -577,
    1237,  -577,   967,  1241,  1242,   -63,  1287,   -19,  1055,  1116,
     -73,  -577,  -577,   767,  -577,  -577,  -577,  -577,  -577,  -577,
    -209,  -577,  -577,  -577,  -577,   673,   -39,    23,   595,  -577,
    -577,  1148,  -577,  -577,  1247,  1248,  1249,  1250,  1251,  -577,
    -577,  -196,  -577,   931,  -220,  -222,  -577,  -526,  -523,  -517,
    -512,  -511,  -509,   598,  -577,  -577,  -577,  -577,  -577,  -577,
     960,  -577,  -577,   917,   525,  -242,  -577,  -577,  -577,   623,
    -577,  -577,  -577,  -577,   624,   918,   919,  -167,  -577,  -577,
    -577,  -577,  1085,  -459,   640,  -128,   442,   461,  -577,  -577,
    -576,  -577,   526,   612,  -577
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,   141,    24,   450,   451,   452,   579,
     672,   673,   797,   453,   333,    25,    26,   209,    27,    67,
      28,   218,   219,    29,    30,    31,    32,    33,   116,   195,
     117,   200,   440,   441,   547,   326,   445,   198,   439,   543,
     221,   839,   725,   114,   537,   538,   539,   540,   650,    34,
     100,   101,   541,   647,    35,    36,    37,    38,    39,    40,
      41,   249,   460,   250,   251,   252,  1094,   253,   254,   255,
     256,   257,   258,   657,   658,   259,   260,   261,   262,   263,
     363,   264,   265,   266,   267,   268,   814,   269,   270,   271,
     272,   273,   274,   275,   276,   383,   384,   277,   278,   279,
     280,   281,   282,   595,   596,   223,   127,   119,   110,   124,
     428,   678,   633,   634,   456
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     340,   107,   323,   674,   148,   382,   597,   358,   224,   339,
      53,   115,   362,   676,   379,   380,   651,   379,   380,   652,
     512,   611,   377,   378,   111,   653,   112,    54,   386,    56,
     654,   655,   113,   656,   328,   438,   103,   447,   104,    98,
     696,   196,   496,  -495,   427,   390,   391,   425,   314,   427,
       1,   220,     2,     3,     4,     5,     6,     7,     8,     9,
      10,   389,   334,   602,   173,   648,    11,    12,    13,    47,
     125,    17,    14,    15,    16,   493,   426,     1,   134,     2,
       3,     4,     5,     6,     7,     8,     9,    10,   135,   136,
     603,  1103,   155,    11,    12,    13,  1114,   697,   497,    14,
      15,    16,   532,    53,   442,   443,   699,   799,    17,   390,
     391,    55,   169,   170,   171,   172,  -498,   462,   533,   589,
     590,   322,   283,   649,   284,   285,   118,    17,  1104,    89,
     591,   592,   593,  1115,   358,   806,   288,   410,   289,   290,
     472,   473,   411,   203,   204,   205,   409,   468,   701,   545,
     546,   212,   213,   214,    17,   494,   390,   391,   390,   391,
     390,   391,   412,   390,   391,    90,   448,   413,   449,   514,
     491,   492,   388,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   286,   329,   293,
     335,   294,   295,   226,   227,   228,   311,   651,   338,    91,
     652,   291,    20,   361,   390,   391,   653,   225,   226,   227,
     228,   654,   655,   132,   656,    18,   105,   830,   194,   831,
     718,   137,   536,   381,   548,   247,   381,   513,   246,   385,
     331,   390,   391,    19,   594,    92,   307,   390,   391,   431,
      57,    58,    18,   308,   390,   391,    59,   434,   435,   432,
     177,   178,   309,   310,   296,   179,   606,   607,    20,   609,
      19,   414,   613,   587,    96,   446,   415,   922,   390,   391,
     598,   111,   622,   112,    97,   466,   371,   668,   372,   113,
     373,   374,   427,   229,   230,    20,   416,    93,    94,    95,
     287,   417,   231,   668,   232,   233,   234,   235,   702,   624,
     102,   236,   487,   394,   292,   475,    99,   476,   108,   477,
     233,   234,   235,    42,    43,    44,   236,   109,   225,   226,
     227,   228,  -499,  -499,   442,    45,    46,   237,   238,   239,
     669,   418,   670,   671,   659,   993,   419,   604,   115,   605,
     471,   477,   237,   238,   239,   118,   669,   167,   670,   671,
     168,   795,   126,   225,   226,   227,   228,   297,   132,   461,
     620,   919,   298,   133,   240,   138,   299,   300,    48,    49,
      50,   301,   302,  -499,  -499,   404,   405,   406,   407,   408,
      51,    52,   180,   694,   387,   695,   698,   388,   241,   156,
     242,   584,   243,   181,   229,   230,   182,   183,   139,   184,
     185,   186,   247,   231,   710,   232,   140,   241,   361,   242,
     154,   243,   244,   245,   246,   187,   188,   247,   161,   248,
     467,   233,   234,   235,   162,   707,   163,   236,   457,   229,
     230,   458,  1074,   174,    17,   176,  1075,  1076,   231,   175,
     232,  1077,  1078,   189,   640,  -238,   641,   642,   643,   644,
     600,   645,   646,   237,   238,   239,   233,   234,   235,   812,
     157,   158,   236,   225,   226,   227,   228,   825,   826,   827,
     828,   582,   193,   638,   583,   240,   623,   807,   190,    60,
      61,    62,    63,    64,    65,   191,   803,    66,   237,   238,
     239,   128,   129,   130,   131,   599,  1087,   614,   388,   241,
     615,   242,   202,   243,   120,   121,   122,   123,   616,   681,
     240,   617,   388,   688,   690,  1099,   689,   689,   691,  1105,
     197,   388,   201,   244,   245,   246,   709,   199,   247,   388,
     248,   206,   810,  1116,   241,   817,   242,   207,   243,   229,
     230,   715,   800,   208,   716,   458,  1124,   210,   231,  1126,
     232,   225,   226,   227,   228,   164,   165,   166,   244,   245,
     246,   822,   211,   247,   388,   248,   233,   234,   235,   406,
     407,   408,   236,   344,   345,   346,   347,   348,   349,   350,
     351,   352,   353,   354,   355,   159,   160,   225,   226,   227,
     228,   840,   909,   912,   841,   458,   388,   215,   237,   238,
     239,   915,   706,   216,   916,   217,    68,    69,     1,    70,
       2,     3,     4,     5,     6,     7,   992,     9,   924,   716,
     240,    71,    72,   220,    11,    12,    13,   356,   357,   222,
      14,    15,    16,   303,   998,   304,   231,   689,   232,  1001,
    1072,   923,   458,  1073,   241,   305,   242,  1091,   243,   306,
    1092,   390,   391,   470,   233,   234,   235,   522,   523,   312,
     236,   618,   619,   356,   379,   913,  1097,  1098,   244,   245,
     246,   315,   231,   247,   232,   248,  1106,  1107,   316,   525,
     526,   833,   834,   317,   318,    17,   237,   238,   239,   319,
     233,   234,   235,   324,   320,   325,   236,   327,   332,   336,
     337,  1000,   341,   342,   343,   359,   360,   364,   240,   365,
     394,   366,   367,   370,   409,   368,   375,   376,   420,   421,
     427,   422,   237,   238,   239,   423,   430,   395,   396,   397,
     398,   392,   241,   393,   242,   400,   243,    73,    74,    75,
      76,   433,    77,    78,   240,   470,   424,    79,    80,    81,
     436,   437,    82,    83,    84,   470,   244,   245,   246,    85,
      86,   247,   454,   248,   444,   455,    17,   459,   241,   464,
     242,    87,   243,    18,   478,    88,   465,   469,   479,   401,
     402,   403,   404,   405,   406,   407,   408,   480,   394,   483,
     808,   481,   244,   245,   246,   482,   484,   247,   485,   248,
     488,   486,   394,   490,   495,   395,   396,   397,   398,   399,
     247,   515,   394,   400,   517,   519,    20,   520,   521,   395,
     396,   397,   398,   394,   626,   524,   447,   400,   527,   395,
     396,   397,   398,   528,   531,   529,   534,   400,   530,   580,
    -499,  -499,   397,   398,   542,   535,   544,   585,  -499,   586,
     581,   494,   610,   608,   601,   390,   588,   401,   402,   403,
     404,   405,   406,   407,   408,   621,   625,   627,   628,   629,
     630,   401,   402,   403,   404,   405,   406,   407,   408,   631,
     632,   401,   402,   403,   404,   405,   406,   407,   408,   635,
     636,   637,  -499,   402,   403,   404,   405,   406,   407,   408,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   560,   561,   562,   563,   564,   565,   639,   566,   567,
     568,   569,   570,   571,   677,   661,   572,   662,   683,   573,
     574,   663,   684,   575,   576,   577,   578,   727,   728,   729,
     730,   731,   664,   665,   732,   733,   680,   666,   667,   675,
     685,   734,   735,   736,   738,   739,   740,   741,   742,   682,
     686,   743,   744,   692,   687,   700,   703,   737,   745,   746,
     747,   749,   750,   751,   752,   753,   704,   708,   754,   755,
     705,   619,   618,   712,   748,   756,   757,   758,   760,   761,
     762,   763,   764,   711,   713,   765,   766,   720,   714,   717,
     721,   759,   767,   768,   769,   771,   772,   773,   774,   775,
     722,   724,   776,   777,   723,   726,   793,   794,   770,   778,
     779,   780,   782,   783,   784,   785,   786,   795,   801,   787,
     788,   805,   809,   802,   811,   781,   789,   790,   791,   813,
     818,   823,   824,   819,   820,   837,   851,   821,   835,   838,
     911,   842,   792,   843,   844,   845,   846,   847,   848,   849,
     850,   862,   852,   873,   853,   884,   854,   895,   855,   856,
     857,   858,   859,   860,   861,   863,   864,   865,   906,   866,
     867,   868,   920,   921,   925,   869,   870,   871,   872,   874,
     875,   876,   877,   878,   879,   926,   927,   880,   881,   882,
     928,   883,   929,   930,   885,   931,   932,   886,   887,   933,
     888,   934,   936,   889,   890,   937,   891,   892,   893,   894,
     896,   897,   898,   899,   900,   901,   902,   903,   904,   938,
     905,   907,   914,   917,   918,   935,   939,   940,   941,   942,
     943,   944,   945,   946,   947,   948,   949,   950,   951,   952,
     953,   954,   955,   956,   957,   958,   959,   960,   961,   962,
     963,   964,   965,   966,   967,   969,   968,   970,   971,   972,
     973,   974,   975,   976,   977,   978,   980,   981,   979,   982,
     983,   984,   985,   986,   987,   988,   989,   991,   990,   994,
     995,   996,   997,   388,  1002,   999,  1003,  1012,  1004,  1005,
    1006,  1007,  1023,  1008,  1009,  1034,  1010,  1045,  1011,  1056,
    1013,  1014,  1015,  1016,  1017,  1018,  1019,  1020,  1021,  1022,
    1024,  1025,  1026,  1067,  1027,  1085,  1028,  1029,  1030,  1031,
    1032,  1086,  1033,  1035,  1036,  1037,  1038,  1039,  1040,  1041,
    1042,  1043,  1044,  1046,  1047,  1048,  1088,  1049,  1050,  1051,
    1052,  1089,  1090,  1053,  1054,  1055,  1057,  1058,  1059,  1060,
    1061,  1062,  1063,  1096,  1064,  1065,  1066,  1068,  1100,  1070,
    1101,  1069,  1079,  1071,  1080,  1081,  1082,  1083,  1084,  1108,
    1095,  1102,  1093,  1109,  1110,  1111,  1112,  1113,  1117,  1118,
    1119,  1120,  1121,  1123,   679,  1122,  1125,   142,   693,   192,
     796,   143,   144,   798,   145,   463,   330,   106,   146,   147,
     321,   660,   719,   832,   149,   150,   151,   152,   153,   489,
     908,   836,   474,   313,   369,   815,   816,   910,   829,   804,
       0,   516,     0,     0,   518,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   429,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   612
};

static const yytype_int16 yycheck[] =
{
     220,    20,   198,   579,    67,   247,   465,   229,   136,   218,
       3,     8,   232,   581,     5,     6,   542,     5,     6,   542,
       3,     3,   244,   245,    20,   542,    22,     4,   248,     6,
     542,   542,    28,   542,    61,    82,    14,     3,    16,    16,
      89,   114,    79,     0,    84,   158,   159,    54,   176,    84,
       7,    72,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    61,    38,    61,    68,     3,    23,    24,    25,    37,
      47,    84,    29,    30,    31,    81,    83,     7,    55,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    82,    83,
      92,   131,    69,    23,    24,    25,   131,    92,   135,    29,
      30,    31,    33,     3,   324,   325,   219,   675,    84,   158,
     159,    82,    89,    90,    91,    92,    68,   337,    49,    74,
      75,   194,     3,    61,     5,     6,    78,    84,   168,    34,
      85,    86,    87,   168,   356,    89,     3,   214,     5,     6,
     360,   361,   219,   120,   121,   122,   217,   343,   219,    76,
      77,   128,   129,   130,    84,   161,   158,   159,   158,   159,
     158,   159,   214,   158,   159,    34,   132,   219,   134,   411,
     390,   391,   219,   395,   396,   397,   398,   399,   400,   401,
     402,   403,   404,   405,   406,   407,   408,    68,   215,     3,
     209,     5,     6,     4,     5,     6,   173,   723,   219,    34,
     723,    68,   215,    93,   158,   159,   723,     3,     4,     5,
       6,   723,   723,   217,   723,   172,   194,    34,   215,    36,
     216,   215,   215,   214,   444,   213,   214,   210,   210,   248,
     207,   158,   159,   190,   189,    34,    10,   158,   159,    46,
     169,   170,   172,    17,   158,   159,   175,   192,   193,    56,
     190,   191,    26,    27,    68,   195,   476,   477,   215,   479,
     190,   214,   482,   459,     3,   328,   219,   835,   158,   159,
     466,    20,   494,    22,     3,    71,   124,    79,   126,    28,
     128,   129,    84,    79,    80,   215,   214,   162,   163,   164,
     171,   219,    88,    79,    90,   106,   107,   108,   219,   495,
     173,   112,   216,   136,   171,    89,     3,    91,     0,    93,
     106,   107,   108,    33,    34,    35,   112,   218,     3,     4,
       5,     6,   155,   156,   544,    45,    46,   138,   139,   140,
     132,   214,   134,   135,   543,   911,   219,    89,     8,    91,
     359,    93,   138,   139,   140,    78,   132,   200,   134,   135,
     203,   137,    78,     3,     4,     5,     6,   171,   217,   336,
     488,   820,    46,    14,   160,     3,    50,    51,    33,    34,
      35,    55,    56,   206,   207,   208,   209,   210,   211,   212,
      45,    46,   163,   603,   216,   605,   608,   219,   184,     6,
     186,   454,   188,   174,    79,    80,   177,   178,     3,   180,
     181,   182,   213,    88,   626,    90,     3,   184,    93,   186,
       3,   188,   208,   209,   210,   196,   197,   213,     3,   215,
     216,   106,   107,   108,   180,   621,   169,   112,   216,    79,
      80,   219,    46,    61,    84,     6,    50,    51,    88,   219,
      90,    55,    56,     4,    62,    63,    64,    65,    66,    67,
     469,    69,    70,   138,   139,   140,   106,   107,   108,   701,
     177,   178,   112,     3,     4,     5,     6,     3,     4,     5,
       6,   216,    21,   536,   219,   160,   495,   697,   216,    39,
      40,    41,    42,    43,    44,   216,   682,    47,   138,   139,
     140,    49,    50,    51,    52,   216,  1072,   216,   219,   184,
     219,   186,     3,   188,    43,    44,    45,    46,   216,   216,
     160,   219,   219,   216,   216,  1091,   219,   219,   216,  1095,
      71,   219,    79,   208,   209,   210,   216,    73,   213,   219,
     215,     3,   699,  1109,   184,   702,   186,    68,   188,    79,
      80,   216,   216,   215,   219,   219,  1122,    80,    88,  1125,
      90,     3,     4,     5,     6,   199,   200,   201,   208,   209,
     210,   216,     3,   213,   219,   215,   106,   107,   108,   210,
     211,   212,   112,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   177,   178,     3,     4,     5,
       6,   216,   216,   216,   219,   219,   219,     3,   138,   139,
     140,   216,   621,     3,   219,     3,    33,    34,     7,    36,
       9,    10,    11,    12,    13,    14,   216,    16,   838,   219,
     160,    48,    49,    72,    23,    24,    25,    79,    80,     4,
      29,    30,    31,     3,   216,     3,    88,   219,    90,   216,
     216,   837,   219,   219,   184,     6,   186,   216,   188,     4,
     219,   158,   159,    79,   106,   107,   108,    52,    53,   215,
     112,     5,     6,    79,     5,     6,    52,    53,   208,   209,
     210,   168,    88,   213,    90,   215,    50,    51,     6,   423,
     424,   720,   721,     3,     6,    84,   138,   139,   140,     3,
     106,   107,   108,    59,    61,    74,   112,    80,     3,    68,
     205,   921,    84,    84,   215,   215,   215,     4,   160,     4,
     136,     4,     4,   215,   217,     6,   215,   215,     3,     6,
      84,    54,   138,   139,   140,    54,     4,   153,   154,   155,
     156,    79,   184,    81,   186,   161,   188,   164,   165,   166,
     167,     6,   169,   170,   160,    79,    54,   174,   175,   176,
     216,   216,   179,   180,   181,    79,   208,   209,   210,   186,
     187,   213,    61,   215,    75,   215,    84,   215,   184,   215,
     186,   198,   188,   172,     4,   202,   215,   215,   215,   205,
     206,   207,   208,   209,   210,   211,   212,   215,   136,     4,
     216,   215,   208,   209,   210,   215,     4,   213,   220,   215,
      83,   216,   136,     3,   215,   153,   154,   155,   156,   157,
     213,     6,   136,   161,     6,     6,   215,     5,    50,   153,
     154,   155,   156,   136,   158,     3,     3,   161,     3,   153,
     154,   155,   156,     3,     6,   215,   168,   161,   216,   133,
     153,   154,   155,   156,   183,   168,   219,     3,   161,     3,
     215,   161,     4,    82,   216,   158,   219,   205,   206,   207,
     208,   209,   210,   211,   212,   215,   135,   220,   220,     6,
       6,   205,   206,   207,   208,   209,   210,   211,   212,    83,
       3,   205,   206,   207,   208,   209,   210,   211,   212,     6,
       4,     4,   205,   206,   207,   208,   209,   210,   211,   212,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   219,   112,   113,
     114,   115,   116,   117,    32,   215,   120,   215,     6,   123,
     124,   215,     6,   127,   128,   129,   130,    95,    96,    97,
      98,    99,   215,   215,   102,   103,   216,   215,   215,   215,
       4,   109,   110,   111,    95,    96,    97,    98,    99,   215,
       3,   102,   103,   216,     6,   219,     4,   125,   109,   110,
     111,    95,    96,    97,    98,    99,     4,   216,   102,   103,
      84,     6,     5,    54,   125,   109,   110,   111,    95,    96,
      97,    98,    99,    57,     3,   102,   103,    63,   205,     6,
      63,   125,   109,   110,   111,    95,    96,    97,    98,    99,
       3,    58,   102,   103,   219,     6,   135,   133,   125,   109,
     110,   111,    95,    96,    97,    98,    99,   137,   215,   102,
     103,   216,   213,   168,     4,   125,   109,   110,   111,   213,
     216,     6,     6,   216,   215,    59,    97,   216,   215,    60,
       3,   219,   125,   219,   219,   219,   219,   219,   219,   219,
     219,    97,   219,    97,   219,    97,   219,    97,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,    97,   219,
     219,   219,     6,    68,     6,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,     6,     6,   219,   219,   219,
       6,   219,     6,     6,   219,     6,     6,   219,   219,     6,
     219,     6,     6,   219,   219,     6,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,     6,
     219,   219,   219,   219,   219,   219,     6,     6,     6,     6,
       6,     6,     6,   219,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   219,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   219,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   219,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   219,     4,
       4,     4,     4,   219,   216,     6,   216,     6,   216,   216,
     216,   216,     6,   216,   216,     6,   216,     6,   216,     6,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,     6,   216,     4,   216,   216,   216,   216,
     216,     4,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,     6,   216,   216,   216,
     216,     6,     6,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,     6,   216,   216,   216,   216,     6,   216,
       4,   219,   216,   219,   216,   216,   216,   216,   216,     6,
     216,   216,   219,   216,    46,    46,     6,    55,   215,    46,
      46,     3,   215,     3,   583,   216,   216,    67,   602,   109,
     672,    67,    67,   674,    67,   338,   205,    20,    67,    67,
     194,   544,   639,   718,    67,    67,    67,    67,    67,   388,
     795,   723,   362,   175,   239,   702,   702,   801,   716,   689,
      -1,   413,    -1,    -1,   415,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   312,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   481
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    84,   172,   190,
     215,   222,   223,   224,   226,   236,   237,   239,   241,   244,
     245,   246,   247,   248,   270,   275,   276,   277,   278,   279,
     280,   281,    33,    34,    35,    45,    46,    37,    33,    34,
      35,    45,    46,     3,   268,    82,   268,   169,   170,   175,
      39,    40,    41,    42,    43,    44,    47,   240,    33,    34,
      36,    48,    49,   164,   165,   166,   167,   169,   170,   174,
     175,   176,   179,   180,   181,   186,   187,   198,   202,    34,
      34,    34,    34,   162,   163,   164,     3,     3,   268,     3,
     271,   272,   173,    14,    16,   194,   247,   248,     0,   218,
     329,    20,    22,    28,   264,     8,   249,   251,    78,   328,
     328,   328,   328,   328,   330,   268,    78,   327,   327,   327,
     327,   327,   217,    14,   268,    82,    83,   215,     3,     3,
       3,   225,   226,   236,   237,   241,   244,   245,   246,   275,
     276,   277,   278,   279,     3,   268,     6,   177,   178,   177,
     178,     3,   180,   169,   199,   200,   201,   200,   203,   268,
     268,   268,   268,    68,    61,   219,     6,   190,   191,   195,
     163,   174,   177,   178,   180,   181,   182,   196,   197,     4,
     216,   216,   224,    21,   215,   250,   251,    71,   258,    73,
     252,    79,     3,   268,   268,   268,     3,    68,   215,   238,
      80,     3,   268,   268,   268,     3,     3,     3,   242,   243,
      72,   261,     4,   326,   326,     3,     4,     5,     6,    79,
      80,    88,    90,   106,   107,   108,   112,   138,   139,   140,
     160,   184,   186,   188,   208,   209,   210,   213,   215,   282,
     284,   285,   286,   288,   289,   290,   291,   292,   293,   296,
     297,   298,   299,   300,   302,   303,   304,   305,   306,   308,
     309,   310,   311,   312,   313,   314,   315,   318,   319,   320,
     321,   322,   323,     3,     5,     6,    68,   171,     3,     5,
       6,    68,   171,     3,     5,     6,    68,   171,    46,    50,
      51,    55,    56,     3,     3,     6,     4,    10,    17,    26,
      27,   268,   215,   272,   326,   168,     6,     3,     6,     3,
      61,   250,   251,   282,    59,    74,   256,    80,    61,   215,
     238,   268,     3,   235,    38,   248,    68,   205,   219,   261,
     285,    84,    84,   215,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,    79,    80,   286,   215,
     215,    93,   285,   301,     4,     4,     4,     4,     6,   323,
     215,   124,   126,   128,   129,   215,   215,   286,   286,     5,
       6,   214,   306,   316,   317,   248,   285,   216,   219,    61,
     158,   159,    79,    81,   136,   153,   154,   155,   156,   157,
     161,   205,   206,   207,   208,   209,   210,   211,   212,   217,
     214,   219,   214,   219,   214,   219,   214,   219,   214,   219,
       3,     6,    54,    54,    54,    54,    83,    84,   331,   249,
       4,    46,    56,     6,   192,   193,   216,   216,    82,   259,
     253,   254,   285,   285,    75,   257,   246,     3,   132,   134,
     227,   228,   229,   234,    61,   215,   335,   216,   219,   215,
     283,   268,   285,   243,   215,   215,    71,   216,   282,   215,
      79,   248,   285,   285,   301,    89,    91,    93,     4,   215,
     215,   215,   215,     4,     4,   220,   216,   216,    83,   284,
       3,   285,   285,    81,   161,   215,    79,   135,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   286,     3,   210,   306,     6,   316,     6,   317,     6,
       5,    50,    52,    53,     3,   229,   229,     3,     3,   215,
     216,     6,    33,    49,   168,   168,   215,   265,   266,   267,
     268,   273,   183,   260,   219,    76,    77,   255,   285,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   112,   113,   114,   115,
     116,   117,   120,   123,   124,   127,   128,   129,   130,   230,
     133,   215,   216,   219,   246,     3,     3,   282,   219,    74,
      75,    85,    86,    87,   189,   324,   325,   324,   282,   216,
     248,   216,    61,    92,    89,    91,   285,   285,    82,   285,
       4,     3,   304,   285,   216,   219,   216,   219,     5,     6,
     326,   215,   286,   248,   282,   135,   158,   220,   220,     6,
       6,    83,     3,   333,   334,     6,     4,     4,   246,   219,
      62,    64,    65,    66,    67,    69,    70,   274,     3,    61,
     269,   288,   289,   290,   291,   292,   293,   294,   295,   261,
     254,   215,   215,   215,   215,   215,   215,   215,    79,   132,
     134,   135,   231,   232,   331,   215,   235,    32,   332,   228,
     216,   216,   215,     6,     6,     4,     3,     6,   216,   219,
     216,   216,   216,   230,   285,   285,    89,    92,   286,   219,
     219,   219,   219,     4,     4,    84,   248,   282,   216,   216,
     286,    57,    54,     3,   205,   216,   219,     6,   216,   266,
      63,    63,     3,   219,    58,   263,     6,    95,    96,    97,
      98,    99,   102,   103,   109,   110,   111,   125,    95,    96,
      97,    98,    99,   102,   103,   109,   110,   111,   125,    95,
      96,    97,    98,    99,   102,   103,   109,   110,   111,   125,
      95,    96,    97,    98,    99,   102,   103,   109,   110,   111,
     125,    95,    96,    97,    98,    99,   102,   103,   109,   110,
     111,   125,    95,    96,    97,    98,    99,   102,   103,   109,
     110,   111,   125,   135,   133,   137,   232,   233,   233,   235,
     216,   215,   168,   282,   325,   216,    89,   285,   216,   213,
     318,     4,   306,   213,   307,   310,   315,   318,   216,   216,
     215,   216,   216,     6,     6,     3,     4,     5,     6,   334,
      34,    36,   269,   267,   267,   215,   294,    59,    60,   262,
     216,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,    97,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,    97,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,    97,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,    97,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,    97,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,    97,   219,   305,   216,
     333,     3,   216,     6,   219,   216,   219,   219,   219,   324,
       6,    68,   235,   282,   285,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   219,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   219,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   219,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   219,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   219,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     219,     6,   216,   331,     4,     4,     4,     4,   216,     6,
     285,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,     6,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,     6,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   216,     6,   216,   216,   216,   216,   216,
     216,   216,   216,   216,   216,     6,   216,   216,   216,   216,
     216,   216,   216,   216,   216,   216,     6,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,     6,   216,   219,
     216,   219,   216,   219,    46,    50,    51,    55,    56,   216,
     216,   216,   216,   216,   216,     4,     4,   331,     6,     6,
       6,   216,   219,   219,   287,   216,     6,    52,    53,   331,
       6,     4,   216,   131,   168,   331,    50,    51,     6,   216,
      46,    46,     6,    55,   131,   168,   331,   215,    46,    46,
       3,   215,   216,     3,   331,   216,   331
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   221,   222,   223,   223,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   226,   226,   226,   226,   226,   226,   227,   227,
     228,   228,   229,   229,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   231,   231,   232,   232,
     232,   232,   233,   233,   234,   234,   235,   235,   236,   237,
     237,   238,   238,   239,   240,   240,   240,   240,   240,   240,
     240,   240,   241,   242,   242,   243,   244,   244,   244,   244,
     244,   245,   245,   245,   246,   246,   246,   246,   247,   247,
     248,   249,   250,   250,   251,   252,   252,   253,   253,   254,
     255,   255,   255,   256,   256,   257,   257,   258,   258,   259,
     259,   260,   260,   261,   261,   262,   262,   263,   263,   264,
     264,   264,   264,   265,   265,   266,   266,   267,   267,   268,
     268,   269,   269,   269,   269,   270,   270,   271,   271,   272,
     273,   273,   274,   274,   274,   274,   274,   274,   274,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   275,   275,   275,   275,
     275,   275,   275,   275,   275,   275,   276,   276,   276,   277,
     277,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     278,   278,   278,   278,   278,   278,   278,   278,   278,   278,
     279,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   281,   281,   281,   281,   281,   282,   282,   283,
     283,   284,   284,   285,   285,   285,   285,   285,   286,   286,
     286,   286,   286,   286,   286,   286,   286,   286,   286,   286,
     286,   287,   287,   288,   289,   289,   289,   289,   290,   290,
     290,   290,   291,   291,   292,   292,   293,   293,   294,   294,
     294,   294,   294,   294,   295,   295,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   296,
     296,   296,   296,   296,   296,   296,   296,   296,   296,   297,
     297,   298,   299,   299,   300,   300,   300,   300,   301,   301,
     302,   303,   303,   303,   303,   304,   304,   304,   304,   305,
     305,   305,   305,   305,   305,   305,   305,   305,   305,   305,
     305,   306,   306,   306,   306,   307,   307,   307,   308,   309,
     309,   310,   310,   311,   312,   312,   313,   314,   314,   315,
     316,   317,   318,   318,   319,   320,   320,   321,   322,   322,
     323,   323,   323,   323,   323,   323,   323,   323,   323,   323,
     323,   323,   324,   324,   325,   325,   325,   325,   325,   325,
     326,   327,   327,   328,   328,   329,   329,   330,   330,   331,
     331,   332,   332,   333,   333,   334,   334,   334,   334,   334,
     335,   335
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     8,     6,     7,     6,     1,     3,
       1,     1,     4,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     6,     4,     1,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       7,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     7,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     7,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     7,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     1,     2,     2,     1,
       1,     2,     2,     0,     5,     4,     1,     3,     4,     6,
       5,     3,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     0,     5,     1,     3,     3,     4,     4,     4,     4,
       6,     8,    11,     8,     1,     1,     3,     3,     3,     3,
       2,     4,     3,     3,     8,     3,     0,     1,     3,     2,
       1,     1,     0,     2,     0,     2,     0,     1,     0,     2,
       0,     2,     0,     2,     0,     2,     0,     3,     0,     1,
       2,     1,     1,     1,     3,     1,     1,     2,     4,     1,
       3,     2,     1,     5,     0,     2,     0,     1,     3,     5,
       4,     6,     1,     1,     1,     1,     1,     1,     0,     2,
       2,     2,     2,     3,     2,     2,     2,     2,     4,     2,
       3,     3,     3,     4,     4,     3,     3,     4,     4,     5,
       6,     7,     9,     4,     5,     7,     9,     2,     3,     2,
       3,     3,     4,     2,     3,     3,     2,     2,     2,     2,
       5,     2,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     3,     3,
       3,     3,     4,     6,     7,     9,    10,    12,    12,    13,
      14,    15,    16,    12,    13,    15,    16,     3,     4,     5,
       6,     3,     3,     4,     3,     4,     3,     3,     3,     5,
       7,     7,     6,     6,     6,     6,     8,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     2,     0,    12,    19,    15,    14,    12,    17,    13,
      12,    10,     7,     9,     4,     6,     4,     6,     1,     1,
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
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2382 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2390 "parser.cpp"
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
#line 2404 "parser.cpp"
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
#line 2418 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2429 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2437 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2446 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2455 "parser.cpp"
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
#line 2469 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2480 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2490 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2500 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2510 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2520 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2530 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2540 "parser.cpp"
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
#line 2554 "parser.cpp"
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
#line 2568 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2578 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2586 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2594 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2603 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2611 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2619 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2627 "parser.cpp"
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
#line 2641 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2650 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2659 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2668 "parser.cpp"
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
#line 2681 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2690 "parser.cpp"
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
#line 2704 "parser.cpp"
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
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2728 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2737 "parser.cpp"
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
#line 2751 "parser.cpp"
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
#line 2768 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2776 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2784 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2792 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2800 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2808 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2816 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2824 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2832 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2840 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2848 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2856 "parser.cpp"
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
#line 2870 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2878 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2886 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2894 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2902 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2910 "parser.cpp"
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
#line 2923 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2931 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2939 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2947 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2955 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2963 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2971 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2979 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2987 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2995 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3003 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3011 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3019 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3027 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3035 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3043 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3051 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3059 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3067 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3075 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3083 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3091 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3099 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3107 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3118 "parser.cpp"
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
#line 3132 "parser.cpp"
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
#line 3146 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3157 "parser.cpp"
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

#line 3265 "parser.cpp"

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
#line 495 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3480 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 499 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3491 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 505 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3502 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 512 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3508 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3514 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3520 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3526 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3532 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3538 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3544 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3550 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 520 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3556 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 521 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3562 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 522 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3568 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3574 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 524 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3580 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 525 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3586 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 526 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3592 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 528 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3598 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3604 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3610 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3616 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3622 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3628 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3634 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3640 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3646 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 537 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3652 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 538 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3658 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3664 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 546 "parser.y"
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
#line 3684 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 563 "parser.y"
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
#line 3702 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 579 "parser.y"
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
#line 3735 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 608 "parser.y"
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
#line 3755 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 624 "parser.y"
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
#line 3776 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 642 "parser.y"
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
#line 3809 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 671 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3818 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 675 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3827 "parser.cpp"
    break;

  case 40: /* table_element: table_column  */
#line 681 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3835 "parser.cpp"
    break;

  case 41: /* table_element: table_constraint  */
#line 684 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3843 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 691 "parser.y"
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
#line 3899 "parser.cpp"
    break;

  case 43: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 742 "parser.y"
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
#line 3941 "parser.cpp"
    break;

  case 44: /* column_type: BOOLEAN  */
#line 781 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3947 "parser.cpp"
    break;

  case 45: /* column_type: TINYINT  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3953 "parser.cpp"
    break;

  case 46: /* column_type: SMALLINT  */
#line 783 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3959 "parser.cpp"
    break;

  case 47: /* column_type: INTEGER  */
#line 784 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3965 "parser.cpp"
    break;

  case 48: /* column_type: INT  */
#line 785 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3971 "parser.cpp"
    break;

  case 49: /* column_type: BIGINT  */
#line 786 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3977 "parser.cpp"
    break;

  case 50: /* column_type: HUGEINT  */
#line 787 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3983 "parser.cpp"
    break;

  case 51: /* column_type: FLOAT  */
#line 788 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3989 "parser.cpp"
    break;

  case 52: /* column_type: REAL  */
#line 789 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3995 "parser.cpp"
    break;

  case 53: /* column_type: DOUBLE  */
#line 790 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4001 "parser.cpp"
    break;

  case 54: /* column_type: FLOAT16  */
#line 791 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4007 "parser.cpp"
    break;

  case 55: /* column_type: BFLOAT16  */
#line 792 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4013 "parser.cpp"
    break;

  case 56: /* column_type: DATE  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4019 "parser.cpp"
    break;

  case 57: /* column_type: TIME  */
#line 794 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4025 "parser.cpp"
    break;

  case 58: /* column_type: DATETIME  */
#line 795 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4031 "parser.cpp"
    break;

  case 59: /* column_type: TIMESTAMP  */
#line 796 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4037 "parser.cpp"
    break;

  case 60: /* column_type: UUID  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4043 "parser.cpp"
    break;

  case 61: /* column_type: POINT  */
#line 798 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4049 "parser.cpp"
    break;

  case 62: /* column_type: LINE  */
#line 799 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4055 "parser.cpp"
    break;

  case 63: /* column_type: LSEG  */
#line 800 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4061 "parser.cpp"
    break;

  case 64: /* column_type: BOX  */
#line 801 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4067 "parser.cpp"
    break;

  case 65: /* column_type: CIRCLE  */
#line 804 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4073 "parser.cpp"
    break;

  case 66: /* column_type: VARCHAR  */
#line 806 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4079 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 807 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4085 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 808 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4091 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL  */
#line 809 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4097 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4103 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4109 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4115 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4121 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4127 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4133 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4139 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4145 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4151 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4157 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4163 "parser.cpp"
    break;

  case 81: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4169 "parser.cpp"
    break;

  case 82: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4175 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4181 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4187 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4193 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4199 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4205 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4211 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4217 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4223 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4229 "parser.cpp"
    break;

  case 92: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4235 "parser.cpp"
    break;

  case 93: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4241 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4247 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4253 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4259 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4265 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4271 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4277 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4283 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4289 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4295 "parser.cpp"
    break;

  case 103: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4301 "parser.cpp"
    break;

  case 104: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4307 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4313 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4319 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4325 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4331 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4337 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4343 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4349 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4355 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4361 "parser.cpp"
    break;

  case 114: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4367 "parser.cpp"
    break;

  case 115: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4373 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4379 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4385 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4391 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4397 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4403 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4409 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4415 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4421 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4427 "parser.cpp"
    break;

  case 125: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4433 "parser.cpp"
    break;

  case 126: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4439 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4445 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4451 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4457 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4463 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4469 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4475 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4481 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4487 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4493 "parser.cpp"
    break;

  case 136: /* column_constraints: column_constraint  */
#line 896 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4502 "parser.cpp"
    break;

  case 137: /* column_constraints: column_constraints column_constraint  */
#line 900 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4516 "parser.cpp"
    break;

  case 138: /* column_constraint: PRIMARY KEY  */
#line 910 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4524 "parser.cpp"
    break;

  case 139: /* column_constraint: UNIQUE  */
#line 913 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4532 "parser.cpp"
    break;

  case 140: /* column_constraint: NULLABLE  */
#line 916 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4540 "parser.cpp"
    break;

  case 141: /* column_constraint: NOT NULLABLE  */
#line 919 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4548 "parser.cpp"
    break;

  case 142: /* default_expr: DEFAULT constant_expr  */
#line 923 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4556 "parser.cpp"
    break;

  case 143: /* default_expr: %empty  */
#line 926 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4564 "parser.cpp"
    break;

  case 144: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 931 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4574 "parser.cpp"
    break;

  case 145: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 936 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4584 "parser.cpp"
    break;

  case 146: /* identifier_array: IDENTIFIER  */
#line 943 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4595 "parser.cpp"
    break;

  case 147: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 949 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4606 "parser.cpp"
    break;

  case 148: /* delete_statement: DELETE FROM table_name where_clause  */
#line 959 "parser.y"
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
#line 4623 "parser.cpp"
    break;

  case 149: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 975 "parser.y"
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
#line 4662 "parser.cpp"
    break;

  case 150: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1009 "parser.y"
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
#line 4679 "parser.cpp"
    break;

  case 151: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1022 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4687 "parser.cpp"
    break;

  case 152: /* optional_identifier_array: %empty  */
#line 1025 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4695 "parser.cpp"
    break;

  case 153: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1032 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4705 "parser.cpp"
    break;

  case 154: /* explain_type: ANALYZE  */
#line 1038 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4713 "parser.cpp"
    break;

  case 155: /* explain_type: AST  */
#line 1041 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4721 "parser.cpp"
    break;

  case 156: /* explain_type: RAW  */
#line 1044 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4729 "parser.cpp"
    break;

  case 157: /* explain_type: LOGICAL  */
#line 1047 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4737 "parser.cpp"
    break;

  case 158: /* explain_type: PHYSICAL  */
#line 1050 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4745 "parser.cpp"
    break;

  case 159: /* explain_type: PIPELINE  */
#line 1053 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4753 "parser.cpp"
    break;

  case 160: /* explain_type: FRAGMENT  */
#line 1056 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4761 "parser.cpp"
    break;

  case 161: /* explain_type: %empty  */
#line 1059 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4769 "parser.cpp"
    break;

  case 162: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1066 "parser.y"
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
#line 4786 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr  */
#line 1079 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4795 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1083 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4804 "parser.cpp"
    break;

  case 165: /* update_expr: IDENTIFIER '=' expr  */
#line 1088 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4816 "parser.cpp"
    break;

  case 166: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1101 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4832 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1114 "parser.y"
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
#line 4850 "parser.cpp"
    break;

  case 168: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1129 "parser.y"
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
#line 4868 "parser.cpp"
    break;

  case 169: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1144 "parser.y"
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
#line 4886 "parser.cpp"
    break;

  case 170: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1159 "parser.y"
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
#line 4909 "parser.cpp"
    break;

  case 171: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1182 "parser.y"
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
#line 4967 "parser.cpp"
    break;

  case 172: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1235 "parser.y"
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
#line 5027 "parser.cpp"
    break;

  case 173: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1290 "parser.y"
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
#line 5079 "parser.cpp"
    break;

  case 174: /* select_statement: select_without_paren  */
#line 1341 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5087 "parser.cpp"
    break;

  case 175: /* select_statement: select_with_paren  */
#line 1344 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5095 "parser.cpp"
    break;

  case 176: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1347 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5109 "parser.cpp"
    break;

  case 177: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1356 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5123 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_without_paren ')'  */
#line 1366 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5131 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_with_paren ')'  */
#line 1369 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5139 "parser.cpp"
    break;

  case 180: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1373 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5148 "parser.cpp"
    break;

  case 181: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1378 "parser.y"
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
#line 5174 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1400 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5182 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1403 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5190 "parser.cpp"
    break;

  case 184: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1408 "parser.y"
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
#line 5210 "parser.cpp"
    break;

  case 185: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1424 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5218 "parser.cpp"
    break;

  case 186: /* order_by_clause: %empty  */
#line 1427 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5226 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr  */
#line 1431 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5235 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1435 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5244 "parser.cpp"
    break;

  case 189: /* order_by_expr: expr order_by_type  */
#line 1440 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5254 "parser.cpp"
    break;

  case 190: /* order_by_type: ASC  */
#line 1446 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5262 "parser.cpp"
    break;

  case 191: /* order_by_type: DESC  */
#line 1449 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5270 "parser.cpp"
    break;

  case 192: /* order_by_type: %empty  */
#line 1452 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5278 "parser.cpp"
    break;

  case 193: /* limit_expr: LIMIT expr  */
#line 1456 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5286 "parser.cpp"
    break;

  case 194: /* limit_expr: %empty  */
#line 1460 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5292 "parser.cpp"
    break;

  case 195: /* offset_expr: OFFSET expr  */
#line 1462 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5300 "parser.cpp"
    break;

  case 196: /* offset_expr: %empty  */
#line 1466 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5306 "parser.cpp"
    break;

  case 197: /* distinct: DISTINCT  */
#line 1468 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5314 "parser.cpp"
    break;

  case 198: /* distinct: %empty  */
#line 1471 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5322 "parser.cpp"
    break;

  case 199: /* from_clause: FROM table_reference  */
#line 1475 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5330 "parser.cpp"
    break;

  case 200: /* from_clause: %empty  */
#line 1478 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5338 "parser.cpp"
    break;

  case 201: /* search_clause: SEARCH sub_search_array  */
#line 1482 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5348 "parser.cpp"
    break;

  case 202: /* search_clause: %empty  */
#line 1487 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5356 "parser.cpp"
    break;

  case 203: /* where_clause: WHERE expr  */
#line 1491 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5364 "parser.cpp"
    break;

  case 204: /* where_clause: %empty  */
#line 1494 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5372 "parser.cpp"
    break;

  case 205: /* having_clause: HAVING expr  */
#line 1498 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5380 "parser.cpp"
    break;

  case 206: /* having_clause: %empty  */
#line 1501 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5388 "parser.cpp"
    break;

  case 207: /* group_by_clause: GROUP BY expr_array  */
#line 1505 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5396 "parser.cpp"
    break;

  case 208: /* group_by_clause: %empty  */
#line 1508 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5404 "parser.cpp"
    break;

  case 209: /* set_operator: UNION  */
#line 1512 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5412 "parser.cpp"
    break;

  case 210: /* set_operator: UNION ALL  */
#line 1515 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5420 "parser.cpp"
    break;

  case 211: /* set_operator: INTERSECT  */
#line 1518 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5428 "parser.cpp"
    break;

  case 212: /* set_operator: EXCEPT  */
#line 1521 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5436 "parser.cpp"
    break;

  case 213: /* table_reference: table_reference_unit  */
#line 1529 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5444 "parser.cpp"
    break;

  case 214: /* table_reference: table_reference ',' table_reference_unit  */
#line 1532 "parser.y"
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
#line 5462 "parser.cpp"
    break;

  case 217: /* table_reference_name: table_name table_alias  */
#line 1549 "parser.y"
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
#line 5480 "parser.cpp"
    break;

  case 218: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1563 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5491 "parser.cpp"
    break;

  case 219: /* table_name: IDENTIFIER  */
#line 1572 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5501 "parser.cpp"
    break;

  case 220: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1577 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5513 "parser.cpp"
    break;

  case 221: /* table_alias: AS IDENTIFIER  */
#line 1586 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5523 "parser.cpp"
    break;

  case 222: /* table_alias: IDENTIFIER  */
#line 1591 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5533 "parser.cpp"
    break;

  case 223: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1596 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5544 "parser.cpp"
    break;

  case 224: /* table_alias: %empty  */
#line 1602 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5552 "parser.cpp"
    break;

  case 225: /* with_clause: WITH with_expr_list  */
#line 1609 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5560 "parser.cpp"
    break;

  case 226: /* with_clause: %empty  */
#line 1612 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5568 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr  */
#line 1616 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5577 "parser.cpp"
    break;

  case 228: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1619 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5586 "parser.cpp"
    break;

  case 229: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1624 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5598 "parser.cpp"
    break;

  case 230: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1636 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5610 "parser.cpp"
    break;

  case 231: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1643 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5623 "parser.cpp"
    break;

  case 232: /* join_type: INNER  */
#line 1657 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5631 "parser.cpp"
    break;

  case 233: /* join_type: LEFT  */
#line 1660 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5639 "parser.cpp"
    break;

  case 234: /* join_type: RIGHT  */
#line 1663 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5647 "parser.cpp"
    break;

  case 235: /* join_type: OUTER  */
#line 1666 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5655 "parser.cpp"
    break;

  case 236: /* join_type: FULL  */
#line 1669 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5663 "parser.cpp"
    break;

  case 237: /* join_type: CROSS  */
#line 1672 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5671 "parser.cpp"
    break;

  case 238: /* join_type: %empty  */
#line 1675 "parser.y"
                {
}
#line 5678 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW DATABASES  */
#line 1681 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5687 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW TABLES  */
#line 1685 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5696 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW VIEWS  */
#line 1689 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5705 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW CONFIGS  */
#line 1693 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5714 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1697 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5726 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW PROFILES  */
#line 1704 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5735 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW BUFFER  */
#line 1708 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5744 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW MEMINDEX  */
#line 1712 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5753 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERIES  */
#line 1716 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5762 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1720 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5772 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTIONS  */
#line 1725 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5781 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1729 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5791 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW SESSION VARIABLES  */
#line 1734 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5800 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1738 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5809 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1742 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5820 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1748 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5831 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1754 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5842 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TABLE table_name  */
#line 1760 "parser.y"
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
#line 5858 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1771 "parser.y"
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
#line 5874 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1782 "parser.y"
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
#line 5890 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1793 "parser.y"
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
#line 5907 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1805 "parser.y"
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
#line 5924 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1817 "parser.y"
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
#line 5942 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1830 "parser.y"
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
#line 5961 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1844 "parser.y"
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
#line 5977 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1855 "parser.y"
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
#line 5996 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1869 "parser.y"
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
#line 6017 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1885 "parser.y"
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
#line 6039 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW LOGS  */
#line 1902 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6048 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW DELTA LOGS  */
#line 1906 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6057 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW CATALOGS  */
#line 1910 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6066 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1914 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6075 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1918 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6084 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1922 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6095 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY  */
#line 1928 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6104 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1932 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6113 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1936 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6122 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH DATA  */
#line 1944 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6131 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH LOG  */
#line 1948 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6140 "parser.cpp"
    break;

  case 278: /* flush_statement: FLUSH BUFFER  */
#line 1952 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6149 "parser.cpp"
    break;

  case 279: /* optimize_statement: OPTIMIZE table_name  */
#line 1960 "parser.y"
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
#line 6164 "parser.cpp"
    break;

  case 280: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1971 "parser.y"
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
#line 6188 "parser.cpp"
    break;

  case 281: /* command_statement: USE IDENTIFIER  */
#line 1994 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 282: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 2000 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6209 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2005 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6220 "parser.cpp"
    break;

  case 284: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2011 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6231 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2017 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6244 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2025 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6255 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2031 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6266 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2037 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6277 "parser.cpp"
    break;

  case 289: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2043 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6288 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2049 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6301 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2057 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6312 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2063 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6323 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2069 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6334 "parser.cpp"
    break;

  case 294: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2075 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6345 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2081 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6358 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2089 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6369 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2095 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6380 "parser.cpp"
    break;

  case 298: /* command_statement: LOCK TABLE table_name  */
#line 2101 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6394 "parser.cpp"
    break;

  case 299: /* command_statement: UNLOCK TABLE table_name  */
#line 2110 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6408 "parser.cpp"
    break;

  case 300: /* compact_statement: COMPACT TABLE table_name  */
#line 2120 "parser.y"
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
#line 6425 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2133 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6434 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2137 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6444 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2142 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6455 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2148 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6467 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2155 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6480 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2163 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6494 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2172 "parser.y"
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
#line 6509 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2182 "parser.y"
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
#line 6524 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2192 "parser.y"
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
#line 6540 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2203 "parser.y"
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
#line 6556 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2214 "parser.y"
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
#line 6573 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2226 "parser.y"
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
#line 6590 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2238 "parser.y"
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
#line 6605 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2248 "parser.y"
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
#line 6621 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2259 "parser.y"
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
#line 6638 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2271 "parser.y"
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
#line 6656 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOGS  */
#line 2284 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6665 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2288 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6675 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2293 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6685 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2298 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6696 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2304 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6705 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2308 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6714 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2312 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6725 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW NODES  */
#line 2318 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6734 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW NODE IDENTIFIER  */
#line 2322 "parser.y"
                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6745 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW NODE  */
#line 2328 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6754 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SET ADMIN  */
#line 2332 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6764 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SET STANDALONE  */
#line 2337 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6774 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2342 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6786 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2349 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6800 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2358 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6814 "parser.cpp"
    break;

  case 332: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2368 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6825 "parser.cpp"
    break;

  case 333: /* alter_statement: ALTER TABLE table_name ADD COLUMN table_column  */
#line 2374 "parser.y"
                                                 {
    auto *ret = new infinity::AddColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_def_ = (yyvsp[0].table_column_t);
}
#line 6835 "parser.cpp"
    break;

  case 334: /* alter_statement: ALTER TABLE table_name DROP COLUMN IDENTIFIER  */
#line 2379 "parser.y"
                                                {
    auto *ret = new infinity::DropColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6846 "parser.cpp"
    break;

  case 335: /* alter_statement: ALTER TABLE table_name ALTER COLUMN table_column  */
#line 2385 "parser.y"
                                                   {
    auto *ret = new infinity::AlterColumnStatement((yyvsp[-3].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_def_ = (yyvsp[0].table_column_t);
}
#line 6856 "parser.cpp"
    break;

  case 336: /* alter_statement: ALTER TABLE table_name RENAME COLUMN IDENTIFIER TO IDENTIFIER  */
#line 2390 "parser.y"
                                                                {
    auto *ret = new infinity::RenameColumnStatement((yyvsp[-5].table_name_t));
    (yyval.alter_stmt) = ret;
    ret->column_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    ret->new_column_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6869 "parser.cpp"
    break;

  case 337: /* expr_array: expr_alias  */
#line 2403 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6878 "parser.cpp"
    break;

  case 338: /* expr_array: expr_array ',' expr_alias  */
#line 2407 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6887 "parser.cpp"
    break;

  case 339: /* expr_array_list: '(' expr_array ')'  */
#line 2412 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6896 "parser.cpp"
    break;

  case 340: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2416 "parser.y"
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
#line 6922 "parser.cpp"
    break;

  case 341: /* expr_alias: expr AS IDENTIFIER  */
#line 2449 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6933 "parser.cpp"
    break;

  case 342: /* expr_alias: expr  */
#line 2455 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6941 "parser.cpp"
    break;

  case 348: /* operand: '(' expr ')'  */
#line 2465 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6949 "parser.cpp"
    break;

  case 349: /* operand: '(' select_without_paren ')'  */
#line 2468 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6960 "parser.cpp"
    break;

  case 350: /* operand: constant_expr  */
#line 2474 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6968 "parser.cpp"
    break;

  case 361: /* extra_match_tensor_option: ',' STRING  */
#line 2488 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6976 "parser.cpp"
    break;

  case 362: /* extra_match_tensor_option: %empty  */
#line 2491 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6984 "parser.cpp"
    break;

  case 363: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2497 "parser.y"
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
#line 7005 "parser.cpp"
    break;

  case 364: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2516 "parser.y"
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
#line 7054 "parser.cpp"
    break;

  case 365: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2561 "parser.y"
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
#line 7096 "parser.cpp"
    break;

  case 366: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2599 "parser.y"
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
#line 7142 "parser.cpp"
    break;

  case 367: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2641 "parser.y"
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
#line 7189 "parser.cpp"
    break;

  case 368: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2687 "parser.y"
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
#line 7214 "parser.cpp"
    break;

  case 369: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2708 "parser.y"
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
#line 7238 "parser.cpp"
    break;

  case 370: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2728 "parser.y"
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
#line 7260 "parser.cpp"
    break;

  case 371: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2746 "parser.y"
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
#line 7282 "parser.cpp"
    break;

  case 372: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2764 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7295 "parser.cpp"
    break;

  case 373: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2772 "parser.y"
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
#line 7310 "parser.cpp"
    break;

  case 374: /* query_expr: QUERY '(' STRING ')'  */
#line 2783 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7321 "parser.cpp"
    break;

  case 375: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2789 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7334 "parser.cpp"
    break;

  case 376: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2798 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7345 "parser.cpp"
    break;

  case 377: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2804 "parser.y"
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
#line 7361 "parser.cpp"
    break;

  case 378: /* sub_search: match_vector_expr  */
#line 2816 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7369 "parser.cpp"
    break;

  case 379: /* sub_search: match_text_expr  */
#line 2819 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7377 "parser.cpp"
    break;

  case 380: /* sub_search: match_tensor_expr  */
#line 2822 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7385 "parser.cpp"
    break;

  case 381: /* sub_search: match_sparse_expr  */
#line 2825 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7393 "parser.cpp"
    break;

  case 382: /* sub_search: query_expr  */
#line 2828 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7401 "parser.cpp"
    break;

  case 383: /* sub_search: fusion_expr  */
#line 2831 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7409 "parser.cpp"
    break;

  case 384: /* sub_search_array: sub_search  */
#line 2835 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7418 "parser.cpp"
    break;

  case 385: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2839 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7427 "parser.cpp"
    break;

  case 386: /* function_expr: IDENTIFIER '(' ')'  */
#line 2844 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7440 "parser.cpp"
    break;

  case 387: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2852 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7453 "parser.cpp"
    break;

  case 388: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2860 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7467 "parser.cpp"
    break;

  case 389: /* function_expr: operand IS NOT NULLABLE  */
#line 2869 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7479 "parser.cpp"
    break;

  case 390: /* function_expr: operand IS NULLABLE  */
#line 2876 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7491 "parser.cpp"
    break;

  case 391: /* function_expr: NOT operand  */
#line 2883 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7503 "parser.cpp"
    break;

  case 392: /* function_expr: '-' operand  */
#line 2890 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7515 "parser.cpp"
    break;

  case 393: /* function_expr: '+' operand  */
#line 2897 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7527 "parser.cpp"
    break;

  case 394: /* function_expr: operand '-' operand  */
#line 2904 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7540 "parser.cpp"
    break;

  case 395: /* function_expr: operand '+' operand  */
#line 2912 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7553 "parser.cpp"
    break;

  case 396: /* function_expr: operand '*' operand  */
#line 2920 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7566 "parser.cpp"
    break;

  case 397: /* function_expr: operand '/' operand  */
#line 2928 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7579 "parser.cpp"
    break;

  case 398: /* function_expr: operand '%' operand  */
#line 2936 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7592 "parser.cpp"
    break;

  case 399: /* function_expr: operand '=' operand  */
#line 2944 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7605 "parser.cpp"
    break;

  case 400: /* function_expr: operand EQUAL operand  */
#line 2952 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7618 "parser.cpp"
    break;

  case 401: /* function_expr: operand NOT_EQ operand  */
#line 2960 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7631 "parser.cpp"
    break;

  case 402: /* function_expr: operand '<' operand  */
#line 2968 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7644 "parser.cpp"
    break;

  case 403: /* function_expr: operand '>' operand  */
#line 2976 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7657 "parser.cpp"
    break;

  case 404: /* function_expr: operand LESS_EQ operand  */
#line 2984 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7670 "parser.cpp"
    break;

  case 405: /* function_expr: operand GREATER_EQ operand  */
#line 2992 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7683 "parser.cpp"
    break;

  case 406: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3000 "parser.y"
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
#line 7718 "parser.cpp"
    break;

  case 407: /* function_expr: operand LIKE operand  */
#line 3030 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7731 "parser.cpp"
    break;

  case 408: /* function_expr: operand NOT LIKE operand  */
#line 3038 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7744 "parser.cpp"
    break;

  case 409: /* conjunction_expr: expr AND expr  */
#line 3047 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7757 "parser.cpp"
    break;

  case 410: /* conjunction_expr: expr OR expr  */
#line 3055 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7770 "parser.cpp"
    break;

  case 411: /* between_expr: operand BETWEEN operand AND operand  */
#line 3064 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7782 "parser.cpp"
    break;

  case 412: /* in_expr: operand IN '(' expr_array ')'  */
#line 3072 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7793 "parser.cpp"
    break;

  case 413: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3078 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7804 "parser.cpp"
    break;

  case 414: /* case_expr: CASE expr case_check_array END  */
#line 3085 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7815 "parser.cpp"
    break;

  case 415: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3091 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7827 "parser.cpp"
    break;

  case 416: /* case_expr: CASE case_check_array END  */
#line 3098 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7837 "parser.cpp"
    break;

  case 417: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3103 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7848 "parser.cpp"
    break;

  case 418: /* case_check_array: WHEN expr THEN expr  */
#line 3110 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7860 "parser.cpp"
    break;

  case 419: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3117 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7872 "parser.cpp"
    break;

  case 420: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3125 "parser.y"
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
#line 7903 "parser.cpp"
    break;

  case 421: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3152 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7914 "parser.cpp"
    break;

  case 422: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3158 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7925 "parser.cpp"
    break;

  case 423: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3164 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7937 "parser.cpp"
    break;

  case 424: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3171 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7949 "parser.cpp"
    break;

  case 425: /* column_expr: IDENTIFIER  */
#line 3179 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7961 "parser.cpp"
    break;

  case 426: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3186 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7973 "parser.cpp"
    break;

  case 427: /* column_expr: '*'  */
#line 3193 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7983 "parser.cpp"
    break;

  case 428: /* column_expr: column_expr '.' '*'  */
#line 3198 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7997 "parser.cpp"
    break;

  case 429: /* constant_expr: STRING  */
#line 3208 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8007 "parser.cpp"
    break;

  case 430: /* constant_expr: TRUE  */
#line 3213 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8017 "parser.cpp"
    break;

  case 431: /* constant_expr: FALSE  */
#line 3218 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8027 "parser.cpp"
    break;

  case 432: /* constant_expr: DOUBLE_VALUE  */
#line 3223 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8037 "parser.cpp"
    break;

  case 433: /* constant_expr: LONG_VALUE  */
#line 3228 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8047 "parser.cpp"
    break;

  case 434: /* constant_expr: DATE STRING  */
#line 3233 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8057 "parser.cpp"
    break;

  case 435: /* constant_expr: TIME STRING  */
#line 3238 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8067 "parser.cpp"
    break;

  case 436: /* constant_expr: DATETIME STRING  */
#line 3243 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8077 "parser.cpp"
    break;

  case 437: /* constant_expr: TIMESTAMP STRING  */
#line 3248 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8087 "parser.cpp"
    break;

  case 438: /* constant_expr: INTERVAL interval_expr  */
#line 3253 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8095 "parser.cpp"
    break;

  case 439: /* constant_expr: interval_expr  */
#line 3256 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8103 "parser.cpp"
    break;

  case 440: /* constant_expr: common_array_expr  */
#line 3259 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8111 "parser.cpp"
    break;

  case 441: /* common_array_expr: array_expr  */
#line 3263 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8119 "parser.cpp"
    break;

  case 442: /* common_array_expr: subarray_array_expr  */
#line 3266 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8127 "parser.cpp"
    break;

  case 443: /* common_array_expr: sparse_array_expr  */
#line 3269 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8135 "parser.cpp"
    break;

  case 444: /* common_array_expr: empty_array_expr  */
#line 3272 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8143 "parser.cpp"
    break;

  case 445: /* common_sparse_array_expr: sparse_array_expr  */
#line 3276 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8151 "parser.cpp"
    break;

  case 446: /* common_sparse_array_expr: array_expr  */
#line 3279 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8159 "parser.cpp"
    break;

  case 447: /* common_sparse_array_expr: empty_array_expr  */
#line 3282 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8167 "parser.cpp"
    break;

  case 448: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3286 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8175 "parser.cpp"
    break;

  case 449: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3290 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8185 "parser.cpp"
    break;

  case 450: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3295 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8194 "parser.cpp"
    break;

  case 451: /* sparse_array_expr: long_sparse_array_expr  */
#line 3300 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8202 "parser.cpp"
    break;

  case 452: /* sparse_array_expr: double_sparse_array_expr  */
#line 3303 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8210 "parser.cpp"
    break;

  case 453: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3307 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8218 "parser.cpp"
    break;

  case 454: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3311 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8230 "parser.cpp"
    break;

  case 455: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3318 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8241 "parser.cpp"
    break;

  case 456: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3325 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8249 "parser.cpp"
    break;

  case 457: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3329 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8261 "parser.cpp"
    break;

  case 458: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3336 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8272 "parser.cpp"
    break;

  case 459: /* empty_array_expr: '[' ']'  */
#line 3343 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8280 "parser.cpp"
    break;

  case 460: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3347 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8288 "parser.cpp"
    break;

  case 461: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3351 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8296 "parser.cpp"
    break;

  case 462: /* array_expr: long_array_expr  */
#line 3355 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8304 "parser.cpp"
    break;

  case 463: /* array_expr: double_array_expr  */
#line 3358 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8312 "parser.cpp"
    break;

  case 464: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3362 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8320 "parser.cpp"
    break;

  case 465: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3366 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8330 "parser.cpp"
    break;

  case 466: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3371 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8339 "parser.cpp"
    break;

  case 467: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3376 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8347 "parser.cpp"
    break;

  case 468: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3380 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8357 "parser.cpp"
    break;

  case 469: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3385 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8366 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE SECONDS  */
#line 3390 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8377 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE SECOND  */
#line 3396 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8388 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE MINUTES  */
#line 3402 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8399 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE MINUTE  */
#line 3408 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8410 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE HOURS  */
#line 3414 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8421 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE HOUR  */
#line 3420 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8432 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE DAYS  */
#line 3426 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8443 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE DAY  */
#line 3432 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8454 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE MONTHS  */
#line 3438 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8465 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE MONTH  */
#line 3444 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8476 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE YEARS  */
#line 3450 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8487 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE YEAR  */
#line 3456 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8498 "parser.cpp"
    break;

  case 482: /* copy_option_list: copy_option  */
#line 3467 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8507 "parser.cpp"
    break;

  case 483: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3471 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8516 "parser.cpp"
    break;

  case 484: /* copy_option: FORMAT IDENTIFIER  */
#line 3476 "parser.y"
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
#line 8552 "parser.cpp"
    break;

  case 485: /* copy_option: DELIMITER STRING  */
#line 3507 "parser.y"
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
#line 8567 "parser.cpp"
    break;

  case 486: /* copy_option: HEADER  */
#line 3517 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8577 "parser.cpp"
    break;

  case 487: /* copy_option: OFFSET LONG_VALUE  */
#line 3522 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8587 "parser.cpp"
    break;

  case 488: /* copy_option: LIMIT LONG_VALUE  */
#line 3527 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8597 "parser.cpp"
    break;

  case 489: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3532 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8607 "parser.cpp"
    break;

  case 490: /* file_path: STRING  */
#line 3538 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8615 "parser.cpp"
    break;

  case 491: /* if_exists: IF EXISTS  */
#line 3542 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8621 "parser.cpp"
    break;

  case 492: /* if_exists: %empty  */
#line 3543 "parser.y"
  { (yyval.bool_value) = false; }
#line 8627 "parser.cpp"
    break;

  case 493: /* if_not_exists: IF NOT EXISTS  */
#line 3545 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8633 "parser.cpp"
    break;

  case 494: /* if_not_exists: %empty  */
#line 3546 "parser.y"
  { (yyval.bool_value) = false; }
#line 8639 "parser.cpp"
    break;

  case 497: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3561 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8652 "parser.cpp"
    break;

  case 498: /* if_not_exists_info: %empty  */
#line 3569 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8660 "parser.cpp"
    break;

  case 499: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3573 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8668 "parser.cpp"
    break;

  case 500: /* with_index_param_list: %empty  */
#line 3576 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8676 "parser.cpp"
    break;

  case 501: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3580 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8684 "parser.cpp"
    break;

  case 502: /* optional_table_properties_list: %empty  */
#line 3583 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8692 "parser.cpp"
    break;

  case 503: /* index_param_list: index_param  */
#line 3587 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8701 "parser.cpp"
    break;

  case 504: /* index_param_list: index_param_list ',' index_param  */
#line 3591 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8710 "parser.cpp"
    break;

  case 505: /* index_param: IDENTIFIER  */
#line 3596 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8721 "parser.cpp"
    break;

  case 506: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3602 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8736 "parser.cpp"
    break;

  case 507: /* index_param: IDENTIFIER '=' STRING  */
#line 3612 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8751 "parser.cpp"
    break;

  case 508: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3622 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8764 "parser.cpp"
    break;

  case 509: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3630 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8777 "parser.cpp"
    break;

  case 510: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3641 "parser.y"
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
#line 8813 "parser.cpp"
    break;

  case 511: /* index_info: '(' IDENTIFIER ')'  */
#line 3672 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8824 "parser.cpp"
    break;


#line 8828 "parser.cpp"

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

#line 3679 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
  YYSYMBOL_PERSISTENCE = 192,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 193,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 194,                  /* OBJECTS  */
  YYSYMBOL_FILES = 195,                    /* FILES  */
  YYSYMBOL_MEMORY = 196,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 197,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 198,                   /* NUMBER  */
  YYSYMBOL_199_ = 199,                     /* '='  */
  YYSYMBOL_200_ = 200,                     /* '<'  */
  YYSYMBOL_201_ = 201,                     /* '>'  */
  YYSYMBOL_202_ = 202,                     /* '+'  */
  YYSYMBOL_203_ = 203,                     /* '-'  */
  YYSYMBOL_204_ = 204,                     /* '*'  */
  YYSYMBOL_205_ = 205,                     /* '/'  */
  YYSYMBOL_206_ = 206,                     /* '%'  */
  YYSYMBOL_207_ = 207,                     /* '['  */
  YYSYMBOL_208_ = 208,                     /* ']'  */
  YYSYMBOL_209_ = 209,                     /* '('  */
  YYSYMBOL_210_ = 210,                     /* ')'  */
  YYSYMBOL_211_ = 211,                     /* '.'  */
  YYSYMBOL_212_ = 212,                     /* ';'  */
  YYSYMBOL_213_ = 213,                     /* ','  */
  YYSYMBOL_214_ = 214,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 215,                 /* $accept  */
  YYSYMBOL_input_pattern = 216,            /* input_pattern  */
  YYSYMBOL_statement_list = 217,           /* statement_list  */
  YYSYMBOL_statement = 218,                /* statement  */
  YYSYMBOL_explainable_statement = 219,    /* explainable_statement  */
  YYSYMBOL_create_statement = 220,         /* create_statement  */
  YYSYMBOL_table_element_array = 221,      /* table_element_array  */
  YYSYMBOL_column_def_array = 222,         /* column_def_array  */
  YYSYMBOL_table_element = 223,            /* table_element  */
  YYSYMBOL_table_column = 224,             /* table_column  */
  YYSYMBOL_column_type = 225,              /* column_type  */
  YYSYMBOL_column_constraints = 226,       /* column_constraints  */
  YYSYMBOL_column_constraint = 227,        /* column_constraint  */
  YYSYMBOL_default_expr = 228,             /* default_expr  */
  YYSYMBOL_table_constraint = 229,         /* table_constraint  */
  YYSYMBOL_identifier_array = 230,         /* identifier_array  */
  YYSYMBOL_delete_statement = 231,         /* delete_statement  */
  YYSYMBOL_insert_statement = 232,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 233, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 234,        /* explain_statement  */
  YYSYMBOL_update_statement = 235,         /* update_statement  */
  YYSYMBOL_update_expr_array = 236,        /* update_expr_array  */
  YYSYMBOL_update_expr = 237,              /* update_expr  */
  YYSYMBOL_drop_statement = 238,           /* drop_statement  */
  YYSYMBOL_copy_statement = 239,           /* copy_statement  */
  YYSYMBOL_select_statement = 240,         /* select_statement  */
  YYSYMBOL_select_with_paren = 241,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 242,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 243, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 244, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 245, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 246,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 247,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 248,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 249,            /* order_by_type  */
  YYSYMBOL_limit_expr = 250,               /* limit_expr  */
  YYSYMBOL_offset_expr = 251,              /* offset_expr  */
  YYSYMBOL_distinct = 252,                 /* distinct  */
  YYSYMBOL_highlight_clause = 253,         /* highlight_clause  */
  YYSYMBOL_from_clause = 254,              /* from_clause  */
  YYSYMBOL_search_clause = 255,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 256, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 257,             /* where_clause  */
  YYSYMBOL_having_clause = 258,            /* having_clause  */
  YYSYMBOL_group_by_clause = 259,          /* group_by_clause  */
  YYSYMBOL_set_operator = 260,             /* set_operator  */
  YYSYMBOL_table_reference = 261,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 262,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 263,     /* table_reference_name  */
  YYSYMBOL_table_name = 264,               /* table_name  */
  YYSYMBOL_table_alias = 265,              /* table_alias  */
  YYSYMBOL_with_clause = 266,              /* with_clause  */
  YYSYMBOL_with_expr_list = 267,           /* with_expr_list  */
  YYSYMBOL_with_expr = 268,                /* with_expr  */
  YYSYMBOL_join_clause = 269,              /* join_clause  */
  YYSYMBOL_join_type = 270,                /* join_type  */
  YYSYMBOL_show_statement = 271,           /* show_statement  */
  YYSYMBOL_flush_statement = 272,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 273,       /* optimize_statement  */
  YYSYMBOL_command_statement = 274,        /* command_statement  */
  YYSYMBOL_compact_statement = 275,        /* compact_statement  */
  YYSYMBOL_admin_statement = 276,          /* admin_statement  */
  YYSYMBOL_alter_statement = 277,          /* alter_statement  */
  YYSYMBOL_expr_array = 278,               /* expr_array  */
  YYSYMBOL_expr_array_list = 279,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 280,               /* expr_alias  */
  YYSYMBOL_expr = 281,                     /* expr  */
  YYSYMBOL_operand = 282,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 283,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 284,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 285,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 286,          /* match_text_expr  */
  YYSYMBOL_query_expr = 287,               /* query_expr  */
  YYSYMBOL_fusion_expr = 288,              /* fusion_expr  */
  YYSYMBOL_sub_search = 289,               /* sub_search  */
  YYSYMBOL_sub_search_array = 290,         /* sub_search_array  */
  YYSYMBOL_function_expr = 291,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 292,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 293,             /* between_expr  */
  YYSYMBOL_in_expr = 294,                  /* in_expr  */
  YYSYMBOL_case_expr = 295,                /* case_expr  */
  YYSYMBOL_case_check_array = 296,         /* case_check_array  */
  YYSYMBOL_cast_expr = 297,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 298,            /* subquery_expr  */
  YYSYMBOL_column_expr = 299,              /* column_expr  */
  YYSYMBOL_constant_expr = 300,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 301,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 302, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 303,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 304, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 305,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 306,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 307, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 308, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 309, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 310,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 311,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 312,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 313,               /* array_expr  */
  YYSYMBOL_long_array_expr = 314,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 315, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 316,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 317, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 318,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 319,         /* copy_option_list  */
  YYSYMBOL_copy_option = 320,              /* copy_option  */
  YYSYMBOL_file_path = 321,                /* file_path  */
  YYSYMBOL_if_exists = 322,                /* if_exists  */
  YYSYMBOL_if_not_exists = 323,            /* if_not_exists  */
  YYSYMBOL_semicolon = 324,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 325,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 326,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 327, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 328,         /* index_param_list  */
  YYSYMBOL_index_param = 329,              /* index_param  */
  YYSYMBOL_index_info = 330                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 459 "parser.cpp"

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
#define YYFINAL  115
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1437

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  215
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  512
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1147

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   453


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
       2,     2,     2,     2,     2,     2,     2,   206,     2,     2,
     209,   210,   204,   202,   213,   203,   211,   205,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   214,   212,
     200,   199,   201,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   207,     2,   208,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197,   198
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
    2511,  2517,  2521,  2526,  2531,  2538,  2547,  2557,  2566,  2578,
    2594,  2598,  2603,  2607,  2640,  2646,  2650,  2651,  2652,  2653,
    2654,  2656,  2659,  2665,  2668,  2669,  2670,  2671,  2672,  2673,
    2674,  2675,  2676,  2677,  2681,  2699,  2745,  2784,  2827,  2874,
    2898,  2921,  2942,  2963,  2972,  2984,  2991,  3001,  3007,  3019,
    3022,  3025,  3028,  3031,  3034,  3038,  3042,  3047,  3055,  3063,
    3072,  3079,  3086,  3093,  3100,  3107,  3115,  3123,  3131,  3139,
    3147,  3155,  3163,  3171,  3179,  3187,  3195,  3203,  3233,  3241,
    3250,  3258,  3267,  3275,  3281,  3288,  3294,  3301,  3306,  3313,
    3320,  3328,  3355,  3361,  3367,  3374,  3382,  3389,  3396,  3401,
    3411,  3416,  3421,  3426,  3431,  3436,  3441,  3446,  3451,  3456,
    3459,  3462,  3466,  3469,  3472,  3475,  3479,  3482,  3485,  3489,
    3493,  3498,  3503,  3506,  3510,  3514,  3521,  3528,  3532,  3539,
    3546,  3550,  3554,  3558,  3561,  3565,  3569,  3574,  3579,  3583,
    3588,  3593,  3599,  3605,  3611,  3617,  3623,  3629,  3635,  3641,
    3647,  3653,  3659,  3670,  3674,  3679,  3710,  3720,  3725,  3730,
    3735,  3741,  3745,  3746,  3748,  3749,  3751,  3752,  3764,  3772,
    3776,  3779,  3783,  3786,  3790,  3794,  3799,  3805,  3815,  3825,
    3833,  3844,  3875
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
  "PERSISTENCE", "OBJECT", "OBJECTS", "FILES", "MEMORY", "ALLOCATION",
  "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['",
  "']'", "'('", "')'", "'.'", "';'", "','", "':'", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "column_def_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "default_expr", "table_constraint",
  "identifier_array", "delete_statement", "insert_statement",
  "optional_identifier_array", "explain_statement", "update_statement",
  "update_expr_array", "update_expr", "drop_statement", "copy_statement",
  "select_statement", "select_with_paren", "select_without_paren",
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

#define YYPACT_NINF (-677)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-500)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     732,   366,    47,   397,    74,    37,    74,   -63,    84,   643,
     195,   293,   299,   377,   321,   115,   285,   360,   174,    22,
     -52,   416,   173,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,   338,  -677,  -677,   415,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,   354,   354,   354,   354,    21,    74,   374,   374,
     374,   374,   374,   227,   436,    74,   -20,   467,   471,   482,
     757,  -677,  -677,  -677,  -677,  -677,  -677,  -677,   338,  -677,
    -677,  -677,  -677,  -677,   288,   487,    74,  -677,  -677,  -677,
    -677,  -677,   493,  -677,   -46,   -23,  -677,   498,  -677,   334,
    -677,  -677,   497,  -677,   424,   106,    74,    74,    74,    74,
    -677,  -677,  -677,  -677,   -44,  -677,   463,   308,  -677,   520,
     188,   729,   524,   343,   350,  -677,    34,  -677,   509,  -677,
    -677,    16,   505,  -677,   513,   503,   568,    74,    74,    74,
     584,   538,   380,   532,   618,    74,    74,    74,   662,   670,
     679,   595,   694,   694,   320,    70,   203,   267,  -677,   492,
    -677,   347,  -677,  -677,   702,  -677,   704,  -677,  -677,  -677,
     705,  -677,  -677,  -677,  -677,   250,  -677,  -677,  -677,    74,
     502,   360,   694,  -677,   549,  -677,   710,  -677,  -677,   714,
    -677,  -677,   712,  -677,   709,   665,  -677,  -677,  -677,  -677,
      16,  -677,  -677,  -677,   320,   668,   655,   651,   591,   -22,
    -677,   380,  -677,    74,   722,   177,  -677,  -677,  -677,  -677,
    -677,   666,  -677,   527,   -45,  -677,   320,  -677,  -677,   652,
     656,   519,  -677,  -677,   789,   490,   525,   531,   280,   746,
     747,   749,   754,  -677,  -677,   730,   550,   389,   551,   556,
     528,   528,  -677,    13,   404,    86,  -677,     0,   680,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,   563,  -677,  -677,  -677,   175,  -677,  -677,   176,
    -677,   190,  -677,  -677,  -677,   204,  -677,   221,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,   772,   770,  -677,  -677,  -677,
    -677,  -677,  -677,   731,   736,   701,   708,   415,  -677,  -677,
     780,   281,  -677,   783,  -677,   349,   586,   587,   -50,   320,
     320,   726,  -677,   795,   -52,    69,   752,   605,  -677,   230,
     606,  -677,    74,   320,   679,  -677,   473,   616,   617,   132,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,   528,   624,   453,   723,   320,   320,    -8,   274,
    -677,  -677,  -677,  -677,   789,  -677,   834,   631,   640,   641,
     644,   847,   850,   427,   427,  -677,   647,  -677,  -677,  -677,
    -677,   649,   -68,   786,   320,   861,   320,   320,     6,   658,
      -5,   528,   528,   528,   528,   528,   528,   528,   528,   528,
     528,   528,   528,   528,   528,    27,  -677,   659,  -677,   859,
    -677,   862,  -677,   863,  -677,   873,   845,   329,   681,   682,
     886,   683,  -677,   684,  -677,   887,  -677,   289,   733,   735,
    -677,  -677,   320,   820,   689,  -677,    58,   473,   320,  -677,
    -677,    61,   854,   787,   706,   236,  -677,  -677,  -677,   -52,
     914,  -677,  -677,   915,   320,   724,  -677,   473,  -677,   211,
     211,   320,  -677,   254,   723,   765,   713,    12,   121,   378,
    -677,   320,   320,   864,   320,   918,    49,   320,   725,   259,
     645,  -677,  -677,   694,  -677,  -677,  -677,   806,   758,   528,
     404,   811,  -677,   671,   671,   341,   341,   707,   671,   671,
     341,   341,   427,   427,  -677,  -677,  -677,  -677,  -677,  -677,
     755,  -677,   756,  -677,  -677,  -677,   967,   968,  -677,   722,
     981,  -677,   982,  -677,  -677,   983,  -677,   984,   986,   778,
      10,   815,   320,  -677,  -677,  -677,   473,   997,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,   793,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,   796,   797,   798,   799,   809,   810,   220,   813,
     722,   991,    69,   338,   814,  -677,   351,   816,  1029,  1030,
    1035,  1037,  -677,  1036,   355,  -677,   357,   363,  -677,   831,
    -677,   854,   320,  -677,   320,    46,   127,   528,    92,   839,
    -677,  -123,    94,    65,   843,  -677,  1052,  -677,  -677,   980,
     404,   671,   848,   367,  -677,   528,  1053,  1064,  1020,  1026,
     375,   384,  -677,   875,   385,  -677,  1069,  -677,  -677,   -52,
     868,   580,  -677,   102,  -677,   165,   595,  -677,  -677,  1070,
     890,   907,   924,   941,   958,   975,   953,   956,  -677,  -677,
     180,  -677,   954,   722,   431,   877,   952,  -677,   925,  -677,
     320,  -677,  -677,  -677,  -677,  -677,  -677,   211,  -677,  -677,
    -677,   879,   473,   141,  -677,   320,   642,   880,  1086,   659,
     884,   882,   320,  -677,   883,   888,   889,   454,  -677,  -677,
     453,  1090,  1092,  -677,  -677,   981,   510,  -677,   982,   223,
      40,    10,  1044,  -677,  -677,  -677,  -677,  -677,  -677,  1045,
    -677,  1089,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
     891,  1051,   456,   892,   893,   894,   895,   896,   897,   898,
     899,   900,  1012,   901,   902,   903,   904,   905,   906,   908,
     909,   910,   911,  1034,   913,   916,   917,   919,   920,   921,
     922,   923,   926,   927,  1040,   928,   929,   930,   931,   932,
     933,   934,   935,   936,   937,  1046,   938,   939,   940,   942,
     943,   944,   945,   946,   947,   948,  1047,   949,   950,   951,
     955,   957,   959,   960,   961,   962,   963,  1063,   964,  -677,
    -677,   514,  -677,   988,   995,   461,  -677,   982,  1124,  1162,
     465,  -677,  -677,  -677,   473,  -677,   675,   965,   966,   969,
       9,   970,  -677,  -677,  -677,  1101,   971,   473,  -677,   211,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    1161,  -677,   102,   580,    10,    10,   976,   165,  1117,  1118,
    -677,  1174,  1178,  1180,  1181,  1182,  1183,  1184,  1185,  1186,
    1187,   985,  1188,  1189,  1190,  1191,  1193,  1194,  1195,  1196,
    1197,  1198,   992,  1200,  1201,  1202,  1203,  1204,  1205,  1206,
    1207,  1208,  1209,  1003,  1211,  1212,  1213,  1214,  1215,  1216,
    1217,  1218,  1219,  1220,  1014,  1222,  1223,  1224,  1225,  1226,
    1227,  1228,  1229,  1230,  1231,  1025,  1233,  1234,  1235,  1236,
    1237,  1238,  1239,  1240,  1241,  1242,  1038,  1243,  -677,  1246,
    1248,  -677,   476,  -677,   708,  -677,  -677,  1249,   129,  1048,
    1250,  1251,  -677,   477,  1253,  -677,  -677,  1199,   722,  -677,
     320,   320,  -677,  1054,  1055,  1056,  1057,  1058,  1059,  1060,
    1061,  1062,  1065,  1255,  1066,  1067,  1068,  1071,  1072,  1073,
    1074,  1075,  1076,  1077,  1256,  1078,  1079,  1080,  1081,  1082,
    1083,  1084,  1085,  1087,  1088,  1257,  1091,  1093,  1094,  1095,
    1096,  1097,  1098,  1099,  1100,  1102,  1267,  1103,  1104,  1105,
    1106,  1107,  1108,  1109,  1110,  1111,  1112,  1268,  1113,  1114,
    1115,  1116,  1119,  1120,  1121,  1122,  1123,  1125,  1273,  1126,
    -677,  -677,  -677,  -677,  1043,   882,  -677,  1127,  1128,  -677,
     408,   320,   478,   778,   473,  -677,  -677,  -677,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  1129,  -677,  -677,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  1132,  -677,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  1133,  -677,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  1134,  -677,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  1135,
    -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    1136,  -677,  1276,  1137,  1292,   104,  1138,  1293,  1294,  -677,
    -677,  -677,   473,  -677,  -677,  -677,  -677,  -677,  -677,  -677,
    1139,  -677,  1140,   882,   708,  1296,   650,   133,  1141,  1307,
    1144,  -677,   657,  1321,  -677,   882,   708,   882,    39,  1322,
    -677,  1286,  1145,  -677,  1146,  1297,  1298,  -677,  -677,  -677,
      78,  -677,  -677,  1148,  1309,  1310,  -677,  1254,  -677,  1149,
    1150,  1356,   708,  1151,  -677,   708,  -677
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     230,     0,     0,     0,     0,     0,     0,     0,   230,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     230,     0,   497,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   175,   174,     0,     8,    14,    15,    16,    17,
      18,    19,   495,   495,   495,   495,   495,     0,   493,   493,
     493,   493,   493,   223,     0,     0,     0,     0,     0,     0,
     230,   161,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   244,   245,   243,
     249,   253,     0,   250,     0,     0,   246,     0,   248,     0,
     271,   273,     0,   251,     0,   277,     0,     0,     0,     0,
     281,   282,   283,   286,   223,   284,     0,   229,   231,     0,
       0,     0,     0,     0,     0,     1,   230,     2,   213,   215,
     216,     0,   198,   180,   186,     0,     0,     0,     0,     0,
       0,     0,   159,     0,     0,     0,     0,     0,     0,     0,
       0,   208,     0,     0,     0,     0,     0,     0,   160,     0,
     259,   260,   254,   255,     0,   256,     0,   247,   272,   252,
       0,   275,   274,   278,   279,     0,   305,   303,   304,     0,
       0,     0,     0,   332,     0,   333,     0,   326,   327,     0,
     322,   306,     0,   329,   331,     0,   179,   178,     4,   214,
       0,   176,   177,   197,     0,     0,   194,     0,    33,     0,
      34,   159,   498,     0,     0,   230,   492,   166,   168,   167,
     169,     0,   224,     0,   208,   163,     0,   155,   491,     0,
       0,   426,   430,   433,   434,     0,     0,     0,     0,     0,
       0,     0,     0,   431,   432,     0,     0,     0,     0,     0,
       0,     0,   428,     0,   230,     0,   340,   345,   346,   360,
     358,   361,   359,   362,   363,   355,   350,   349,   348,   356,
     357,   347,   354,   353,   441,   443,     0,   444,   452,     0,
     453,     0,   445,   442,   463,     0,   464,     0,   440,   290,
     292,   291,   288,   289,   295,   297,   296,   293,   294,   300,
     302,   301,   298,   299,   280,     0,     0,   262,   261,   267,
     257,   258,   276,     0,     0,     0,   501,     0,   232,   287,
       0,   323,   328,   307,   330,     0,     0,     0,   200,     0,
       0,   196,   494,     0,   230,     0,     0,     0,   153,     0,
       0,   157,     0,     0,     0,   162,   207,     0,     0,     0,
     472,   471,   474,   473,   476,   475,   478,   477,   480,   479,
     482,   481,     0,     0,   392,   230,     0,     0,     0,     0,
     435,   436,   437,   438,     0,   439,     0,     0,     0,     0,
       0,     0,     0,   394,   393,   469,   466,   460,   450,   455,
     458,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   449,     0,   454,     0,
     457,     0,   465,     0,   468,     0,   268,   263,     0,     0,
       0,     0,   285,     0,   334,     0,   324,     0,     0,     0,
     183,   182,     0,   202,   185,   187,   192,   193,     0,   181,
      32,    36,     0,     0,     0,     0,    41,    45,    46,   230,
       0,    40,   158,     0,     0,   156,   170,   165,   164,     0,
       0,     0,   387,     0,   230,     0,     0,     0,     0,     0,
     417,     0,     0,     0,     0,     0,     0,     0,   206,     0,
       0,   352,   351,     0,   341,   344,   410,   411,     0,     0,
     230,     0,   391,   401,   402,   405,   406,     0,   408,   400,
     403,   404,   396,   395,   397,   398,   399,   427,   429,   451,
       0,   456,     0,   459,   467,   470,     0,     0,   264,     0,
       0,   337,     0,   233,   325,     0,   308,     0,     0,   199,
       0,   204,     0,   190,   191,   189,   195,     0,    51,    54,
      55,    52,    53,    56,    57,    73,    58,    60,    59,    76,
      63,    64,    65,    61,    62,    66,    67,    68,    69,    70,
      71,    72,     0,     0,     0,     0,     0,     0,   501,     0,
       0,   503,     0,    39,     0,   154,     0,     0,     0,     0,
       0,     0,   487,     0,     0,   483,     0,     0,   388,     0,
     422,     0,     0,   415,     0,     0,     0,     0,     0,     0,
     426,     0,     0,     0,     0,   377,     0,   462,   461,     0,
     230,   409,     0,     0,   390,     0,     0,     0,   269,   265,
       0,     0,    43,   506,     0,   504,   309,   335,   336,   230,
     201,   217,   219,   228,   220,     0,   208,   188,    38,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,   147,
     150,   143,   150,     0,     0,     0,    35,    42,   512,   342,
       0,   489,   488,   486,   485,   490,   173,     0,   171,   389,
     423,     0,   419,     0,   418,     0,     0,     0,     0,     0,
       0,   206,     0,   375,     0,     0,     0,     0,   424,   413,
     412,     0,     0,   339,   338,     0,     0,   500,     0,     0,
       0,     0,     0,   237,   238,   239,   240,   236,   241,     0,
     226,     0,   221,   381,   379,   382,   380,   383,   384,   385,
     203,   212,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   148,
     145,     0,   144,    48,    47,     0,   152,     0,     0,     0,
       0,   484,   421,   416,   420,   407,     0,     0,   206,     0,
       0,     0,   446,   448,   447,     0,     0,   205,   378,     0,
     425,   414,   270,   266,    44,   507,   508,   510,   509,   505,
       0,   310,   228,   218,     0,     0,   225,     0,     0,   210,
      75,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   149,     0,
       0,   151,     0,    37,   501,   343,   466,     0,     0,     0,
       0,     0,   376,     0,   311,   222,   234,     0,     0,   386,
       0,     0,   184,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      50,    49,   502,   511,     0,   206,   373,     0,   206,   172,
       0,     0,     0,   211,   209,    74,    80,    81,    78,    79,
      82,    83,    84,    85,    86,     0,    77,   124,   125,   122,
     123,   126,   127,   128,   129,   130,     0,   121,    91,    92,
      89,    90,    93,    94,    95,    96,    97,     0,    88,   102,
     103,   100,   101,   104,   105,   106,   107,   108,     0,    99,
     135,   136,   133,   134,   137,   138,   139,   140,   141,     0,
     132,   113,   114,   111,   112,   115,   116,   117,   118,   119,
       0,   110,     0,     0,     0,     0,     0,     0,     0,   313,
     312,   318,   235,   227,    87,   131,    98,   109,   142,   120,
     206,   374,     0,   206,   501,   319,   314,     0,     0,     0,
       0,   372,     0,     0,   315,   206,   501,   206,   501,     0,
     320,   316,     0,   368,     0,     0,     0,   371,   321,   317,
     501,   364,   370,     0,     0,     0,   367,     0,   366,     0,
       0,     0,   501,     0,   369,   501,   365
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -677,  -677,  -677,  1247,  1302,    63,  -677,  -677,   792,  -504,
     774,  -677,   716,   715,  -677,  -509,   182,   222,  1167,  -677,
     241,  -677,  1039,   255,   260,    -7,  1349,   -17,  1130,  1192,
     -47,  -677,  -677,   838,  -677,  -677,  -677,  -677,  -677,  -677,
    -677,  -676,  -208,  -677,  -677,  -677,  -677,   673,  -131,    23,
     539,  -677,  -677,  1210,  -677,  -677,   294,   296,   305,   310,
     311,  -677,  -677,  -192,  -677,   993,  -216,  -218,  -614,  -609,
    -602,  -600,  -598,  -593,   535,  -677,  -677,  -677,  -677,  -677,
    -677,  1017,  -677,  -677,   912,   585,  -235,  -677,  -677,  -677,
     698,  -677,  -677,  -677,  -677,   699,   974,   973,  -340,  -677,
    -677,  -677,  -677,  1152,  -451,   718,  -132,   499,   512,  -677,
    -677,  -564,  -677,   583,   688,  -677
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   445,   621,   446,   447,
     568,   650,   651,   793,   448,   329,    25,    26,   205,    27,
      28,   214,   215,    29,    30,    31,    32,    33,   123,   191,
     124,   196,   434,   435,   535,   321,   439,   194,   433,   531,
     636,   604,   217,   932,   839,   121,   630,   631,   632,   633,
     712,    34,   107,   108,   634,   709,    35,    36,    37,    38,
      39,    40,    41,   245,   455,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   719,   720,   255,   256,   257,   258,
     259,   359,   260,   261,   262,   263,   264,   811,   265,   266,
     267,   268,   269,   270,   271,   272,   379,   380,   273,   274,
     275,   276,   277,   278,   584,   585,   219,   134,   126,   117,
     131,   422,   656,   624,   625,   451
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     336,    68,   318,   114,   652,   816,   335,   354,   378,   586,
     620,   220,   358,    53,   375,   376,   622,   169,   375,   376,
     216,   713,   373,   374,   122,    17,   714,    54,   382,    56,
     507,   432,   324,   715,  -496,   716,   110,   717,   111,   105,
     309,     1,   718,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   600,    68,   385,   142,   143,    11,    12,    13,
     118,   654,   119,    14,    15,    16,   591,   491,   120,   681,
     132,    62,   442,   279,   192,   280,   281,    53,   141,   357,
     488,   118,  -499,   119,    47,   386,   387,    60,   405,   120,
     679,     1,   125,     2,     3,     4,     5,     6,     7,   151,
       9,    57,    58,   436,   437,   710,    59,    11,    12,    13,
    1103,    17,    55,    14,    15,    16,   421,   457,   103,   165,
     166,   167,   168,    62,   492,   153,   154,   533,   534,   674,
     682,   282,   919,  1005,   354,   221,   222,   223,   224,  1115,
     467,   468,   482,   317,   795,   386,   387,   463,   155,   156,
     199,   200,   201,   386,   387,   421,   711,    20,   208,   209,
     210,    17,   489,   384,  1125,   386,   387,   139,   334,   682,
     486,   487,   509,   493,   494,   495,   496,   497,   498,   499,
     500,   501,   502,   503,   504,   505,   506,   325,   331,   144,
      63,   824,   306,   537,   682,   443,   461,   444,   682,   386,
     387,    18,  1126,  1134,   225,   226,   284,   592,   285,   286,
     112,   386,   387,   675,   227,   330,   228,   377,    19,   629,
     243,   377,   536,   713,   803,   190,   327,   381,   714,    96,
      64,   508,   229,   230,   231,   715,   283,   716,   232,   717,
     529,  1135,    63,    20,   718,   386,   387,   386,   387,    65,
     832,    18,   646,   242,    17,   595,   596,   830,   598,   831,
     303,   602,   576,    66,   287,   233,   234,   235,    67,   587,
     289,   611,   290,   291,   386,   387,   304,   305,   578,   579,
     386,   387,    64,   221,   222,   223,   224,   236,   104,   580,
     581,   582,   646,    20,   386,   387,   383,   421,   613,   384,
     163,    65,    69,   164,    70,   677,   647,   680,   648,   649,
     237,   791,   238,    71,   239,    66,   436,   441,    72,    73,
      67,   425,   525,   221,   222,   223,   224,    97,   292,  1083,
     426,   526,  1086,    98,   240,   241,   242,   807,   466,   243,
     814,   244,   462,   237,   109,   238,   647,   239,   648,   649,
    1003,   609,   225,   226,    69,   456,    70,   470,   118,   471,
     119,   472,   227,   106,   228,    71,   120,   357,   923,   288,
      72,    73,   173,   174,   517,   518,   672,   175,   673,   676,
     229,   230,   231,   406,   408,   116,   232,   295,   407,   409,
     296,   297,   225,   226,   583,   298,   299,   690,   410,    42,
      43,    44,   227,   411,   228,    45,    46,   221,   222,   223,
     224,    99,   412,   233,   234,   235,   115,   413,   687,  1012,
     229,   230,   231,   122,  1108,   125,   232,  1110,   721,   414,
      48,    49,    50,   293,   415,   236,    51,    52,   139,  1122,
     452,  1124,   573,   453,   809,   133,   571,   589,  1087,   572,
     140,  1088,  1089,   233,   234,   235,  1090,  1091,   237,   804,
     238,   593,   239,   594,   588,   472,   817,   384,   800,   605,
     145,   390,   606,   612,   146,   236,   225,   226,   100,   101,
     102,    17,   240,   241,   242,   147,   227,   243,   228,   244,
     150,  -500,  -500,   221,   222,   223,   224,   149,   237,   152,
     238,   157,   239,   159,   229,   230,   231,   367,   158,   368,
     232,   369,   370,   825,   826,   827,   828,   170,   222,   223,
     224,   171,   240,   241,   242,   465,   172,   243,   185,   244,
     189,   221,   222,   223,   224,   428,   429,   233,   234,   235,
    1111,  -500,  -500,   400,   401,   402,   403,   404,   135,   136,
     137,   138,  1123,   186,  1127,   127,   128,   129,   130,   236,
     187,   659,   352,   353,   384,   666,  1136,   668,   667,   193,
     667,   198,   227,   669,   228,   197,   384,   689,  1144,   195,
     384,  1146,   237,   390,   238,   693,   239,   202,   453,   204,
     229,   230,   231,   686,   694,   697,   232,   695,   698,   203,
     352,   391,   392,   393,   394,   206,   240,   241,   242,   396,
     227,   243,   228,   244,   229,   230,   231,   160,   161,   162,
     232,   207,   700,   233,   234,   235,   386,   387,   229,   230,
     231,   402,   403,   404,   232,   702,  -242,   703,   704,   705,
     706,   796,   707,   708,   453,   236,    74,   233,   234,   235,
     607,   608,   397,   398,   399,   400,   401,   402,   403,   404,
     216,   233,   234,   235,   821,   211,   840,   384,   237,   841,
     238,   911,   239,   212,   453,   915,    75,    76,   384,    77,
     375,   916,   213,   236,    78,    79,  1002,  1009,  1093,   698,
     667,   453,   240,   241,   242,  1113,  1114,   243,   218,   244,
    1119,  1120,   294,   926,   927,   300,   237,   301,   238,   302,
     239,   307,   310,   314,   465,  1014,   311,   312,   313,   315,
     319,   243,   320,   323,   322,   328,   333,   332,   339,   337,
     240,   241,   242,   338,   355,   243,   364,   244,  1013,     1,
     356,     2,     3,     4,     5,     6,     7,     8,     9,    10,
     360,   361,   388,   362,   389,    11,    12,    13,   363,   366,
     371,    14,    15,    16,     1,   372,     2,     3,     4,     5,
       6,     7,   390,     9,   405,   416,   417,   420,   418,   465,
      11,    12,    13,   419,   424,   421,    14,    15,    16,   427,
     391,   392,   393,   394,   438,  1092,   430,   431,   396,   440,
      17,   390,    80,    81,    82,    83,   449,    84,    85,    17,
     390,    86,    87,    88,   450,   454,    89,    90,    91,  -500,
    -500,   393,   394,    92,    93,   459,   460,  -500,   391,   392,
     393,   394,   395,   464,    17,    94,   396,   390,   473,    95,
     474,   397,   398,   399,   400,   401,   402,   403,   404,   475,
     476,   478,   805,   477,   479,   391,   392,   393,   394,   481,
     615,   480,   483,   396,   485,   510,   243,   490,   512,   514,
    -500,   398,   399,   400,   401,   402,   403,   404,   515,   397,
     398,   399,   400,   401,   402,   403,   404,   176,   516,   521,
     519,   520,   522,   524,   523,   530,   527,   177,   528,    18,
     178,   179,   532,   180,   181,   182,   397,   398,   399,   400,
     401,   402,   403,   404,   569,   570,    19,   574,   575,   183,
     184,   489,   599,   590,    18,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   577,   603,   597,
     614,    20,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   386,
     555,   556,   557,   558,   559,   560,    20,   610,   561,   616,
     617,   562,   563,   618,   619,   564,   565,   566,   567,   723,
     724,   725,   726,   727,   442,   623,   728,   729,   627,   626,
     628,   384,   635,   730,   731,   732,   734,   735,   736,   737,
     738,   638,   639,   739,   740,   640,   641,   642,   643,   733,
     741,   742,   743,   745,   746,   747,   748,   749,   644,   645,
     750,   751,   653,   655,   658,   660,   744,   752,   753,   754,
     756,   757,   758,   759,   760,   661,   662,   761,   762,   663,
     664,   670,   665,   755,   763,   764,   765,   767,   768,   769,
     770,   771,   678,   683,   772,   773,   684,   685,   688,   608,
     766,   774,   775,   776,   778,   779,   780,   781,   782,   607,
     691,   783,   784,   692,   696,   699,   722,   777,   785,   786,
     787,   701,   789,   790,   798,   791,   797,   806,   799,   802,
     808,   810,   836,   818,   788,   815,   822,   819,   823,   820,
     834,   835,   838,   851,   837,   842,   843,   844,   845,   846,
     847,   848,   849,   850,   852,   853,   854,   855,   856,   857,
     909,   858,   859,   860,   861,   862,   863,   910,   913,   864,
     865,   873,   866,   867,   868,   869,   870,   884,   895,   871,
     872,   874,   875,   876,   877,   878,   879,   880,   881,   882,
     883,   885,   886,   887,   906,   888,   889,   890,   891,   892,
     893,   894,   896,   897,   898,   914,   682,   924,   899,   930,
     900,   931,   901,   902,   903,   904,   905,   907,   917,   918,
     933,   922,   920,   921,   934,   928,   935,   936,   937,   938,
     939,   940,   941,   942,   944,   945,   946,   947,   943,   948,
     949,   950,   951,   952,   953,   954,   955,   956,   957,   958,
     959,   960,   961,   962,   963,   964,   965,   966,   967,   968,
     969,   970,   971,   972,   973,   974,   975,   976,   977,   978,
     979,   980,   981,   982,   983,   984,   985,   986,   987,   988,
     989,   990,   991,   992,   993,   994,   995,   996,   997,   999,
    1000,   998,  1001,  1004,  1007,  1008,  1082,  1140,  1006,  1010,
    1011,  1025,  1036,  1047,  1015,  1016,  1017,  1018,  1019,  1020,
    1021,  1022,  1023,  1058,  1069,  1024,  1026,  1027,  1028,  1080,
    1100,  1029,  1030,  1031,  1032,  1033,  1034,  1035,  1037,  1038,
    1039,  1040,  1041,  1042,  1043,  1044,  1102,  1045,  1046,  1105,
    1106,  1048,  1112,  1049,  1050,  1051,  1052,  1053,  1054,  1055,
    1056,  1117,  1057,  1059,  1060,  1061,  1062,  1063,  1064,  1065,
    1066,  1067,  1068,  1070,  1071,  1072,  1073,  1121,  1128,  1074,
    1075,  1076,  1077,  1078,  1129,  1079,  1081,  1132,  1133,  1094,
    1084,  1085,  1095,  1096,  1097,  1098,  1099,  1101,  1104,  1138,
    1139,  1116,  1107,  1109,  1118,  1130,  1131,  1137,  1141,  1143,
    1142,  1145,   148,   188,   657,   671,   792,   794,   326,   113,
     637,   925,   929,   458,   833,   469,   908,   484,   812,   813,
     912,   308,   316,   511,   513,   801,   829,   365,   601,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   423
};

static const yytype_int16 yycheck[] =
{
     216,     8,   194,    20,   568,   681,   214,   225,   243,   460,
     519,   143,   228,     3,     5,     6,   520,    61,     5,     6,
      65,   635,   240,   241,     8,    77,   635,     4,   244,     6,
       3,    81,    54,   635,     0,   635,    14,   635,    16,    16,
     172,     7,   635,     9,    10,    11,    12,    13,    14,    15,
      16,    17,     3,    60,    54,    75,    76,    23,    24,    25,
      20,   570,    22,    29,    30,    31,    54,    72,    28,     4,
      47,     8,     3,     3,   121,     5,     6,     3,    55,    87,
      74,    20,    61,    22,    37,   153,   154,     3,   211,    28,
     213,     7,    71,     9,    10,    11,    12,    13,    14,    76,
      16,   164,   165,   319,   320,     3,   169,    23,    24,    25,
       6,    77,    75,    29,    30,    31,    77,   333,     3,    96,
      97,    98,    99,    60,   129,   171,   172,    69,    70,    83,
      65,    61,   808,     4,   352,     3,     4,     5,     6,     6,
     356,   357,   210,   190,   653,   153,   154,   339,   171,   172,
     127,   128,   129,   153,   154,    77,    54,   209,   135,   136,
     137,    77,   156,   213,   125,   153,   154,   211,   213,    65,
     386,   387,   407,   391,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   209,   205,   209,
       8,   695,   169,   132,    65,   126,    64,   128,    65,   153,
     154,   167,   163,   125,    72,    73,     3,    86,     5,     6,
     188,   153,   154,    86,    82,    38,    84,   208,   184,   209,
     207,   208,   438,   837,    83,   209,   203,   244,   837,    34,
       8,   204,   100,   101,   102,   837,   166,   837,   106,   837,
     432,   163,    60,   209,   837,   153,   154,   153,   154,     8,
     210,   167,    72,   204,    77,   471,   472,    34,   474,    36,
      10,   477,   454,     8,    61,   133,   134,   135,     8,   461,
       3,   489,     5,     6,   153,   154,    26,    27,    67,    68,
     153,   154,    60,     3,     4,     5,     6,   155,     3,    78,
      79,    80,    72,   209,   153,   154,   210,    77,   490,   213,
     194,    60,     8,   197,     8,   213,   126,   213,   128,   129,
     178,   131,   180,     8,   182,    60,   532,   324,     8,     8,
      60,    40,    33,     3,     4,     5,     6,    34,    61,  1005,
      49,    42,  1008,    34,   202,   203,   204,   677,   355,   207,
     680,   209,   210,   178,   170,   180,   126,   182,   128,   129,
     914,   483,    72,    73,    60,   332,    60,    83,    20,    85,
      22,    87,    82,     3,    84,    60,    28,    87,   819,   166,
      60,    60,   184,   185,    45,    46,   592,   189,   594,   597,
     100,   101,   102,   208,   208,   212,   106,    40,   213,   213,
      43,    44,    72,    73,   183,    48,    49,   615,   208,    33,
      34,    35,    82,   213,    84,    39,    40,     3,     4,     5,
       6,    34,   208,   133,   134,   135,     0,   213,   610,   928,
     100,   101,   102,     8,  1100,    71,   106,  1103,   636,   208,
      33,    34,    35,   166,   213,   155,    39,    40,   211,  1115,
     210,  1117,   449,   213,   679,    71,   210,   464,    40,   213,
      14,    43,    44,   133,   134,   135,    48,    49,   178,   675,
     180,    83,   182,    85,   210,    87,   682,   213,   660,   210,
       3,   130,   213,   490,     3,   155,    72,    73,   157,   158,
     159,    77,   202,   203,   204,     3,    82,   207,    84,   209,
       3,   150,   151,     3,     4,     5,     6,   209,   178,     6,
     180,     3,   182,     6,   100,   101,   102,   118,   174,   120,
     106,   122,   123,     3,     4,     5,     6,    54,     4,     5,
       6,   213,   202,   203,   204,    72,     6,   207,     4,   209,
      21,     3,     4,     5,     6,   186,   187,   133,   134,   135,
    1104,   200,   201,   202,   203,   204,   205,   206,    49,    50,
      51,    52,  1116,   210,  1118,    43,    44,    45,    46,   155,
     210,   210,    72,    73,   213,   210,  1130,   210,   213,    64,
     213,     3,    82,   210,    84,    72,   213,   210,  1142,    66,
     213,  1145,   178,   130,   180,   210,   182,     3,   213,   209,
     100,   101,   102,   610,   210,   210,   106,   213,   213,    61,
      72,   148,   149,   150,   151,    73,   202,   203,   204,   156,
      82,   207,    84,   209,   100,   101,   102,   193,   194,   195,
     106,     3,   629,   133,   134,   135,   153,   154,   100,   101,
     102,   204,   205,   206,   106,    55,    56,    57,    58,    59,
      60,   210,    62,    63,   213,   155,     3,   133,   134,   135,
       5,     6,   199,   200,   201,   202,   203,   204,   205,   206,
      65,   133,   134,   135,   210,     3,   210,   213,   178,   213,
     180,   210,   182,     3,   213,   210,    33,    34,   213,    36,
       5,     6,     3,   155,    41,    42,   210,   210,   210,   213,
     213,   213,   202,   203,   204,    45,    46,   207,     4,   209,
      43,    44,   210,   834,   835,     3,   178,     3,   180,     4,
     182,   209,   163,     4,    72,   931,     6,     3,     6,    54,
      52,   207,    67,   132,    73,     3,   199,    61,   209,    77,
     202,   203,   204,    77,   209,   207,     6,   209,   930,     7,
     209,     9,    10,    11,    12,    13,    14,    15,    16,    17,
       4,     4,    72,     4,    74,    23,    24,    25,     4,   209,
     209,    29,    30,    31,     7,   209,     9,    10,    11,    12,
      13,    14,   130,    16,   211,     3,     6,    76,    47,    72,
      23,    24,    25,    47,     4,    77,    29,    30,    31,     6,
     148,   149,   150,   151,    68,  1011,   210,   210,   156,     4,
      77,   130,   159,   160,   161,   162,    54,   164,   165,    77,
     130,   168,   169,   170,   209,   209,   173,   174,   175,   148,
     149,   150,   151,   180,   181,   209,   209,   156,   148,   149,
     150,   151,   152,   209,    77,   192,   156,   130,     4,   196,
     209,   199,   200,   201,   202,   203,   204,   205,   206,   209,
     209,     4,   210,   209,     4,   148,   149,   150,   151,   210,
     153,   214,    76,   156,     3,     6,   207,   209,     6,     6,
     199,   200,   201,   202,   203,   204,   205,   206,     5,   199,
     200,   201,   202,   203,   204,   205,   206,   158,    43,     3,
     209,   209,   209,     6,   210,    75,   163,   168,   163,   167,
     171,   172,   213,   174,   175,   176,   199,   200,   201,   202,
     203,   204,   205,   206,   127,   209,   184,     3,     3,   190,
     191,   156,     4,   210,   167,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   213,   213,    75,
     129,   209,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   153,
     106,   107,   108,   109,   110,   111,   209,   209,   114,   214,
     214,   117,   118,     6,     6,   121,   122,   123,   124,    89,
      90,    91,    92,    93,     3,     3,    96,    97,     4,     6,
       4,   213,   177,   103,   104,   105,    89,    90,    91,    92,
      93,     4,   209,    96,    97,   209,   209,   209,   209,   119,
     103,   104,   105,    89,    90,    91,    92,    93,   209,   209,
      96,    97,   209,    32,   210,   209,   119,   103,   104,   105,
      89,    90,    91,    92,    93,     6,     6,    96,    97,     4,
       3,   210,     6,   119,   103,   104,   105,    89,    90,    91,
      92,    93,   213,   210,    96,    97,     4,    77,   210,     6,
     119,   103,   104,   105,    89,    90,    91,    92,    93,     5,
      50,    96,    97,    47,   199,     6,     6,   119,   103,   104,
     105,   213,   129,   127,   132,   131,   209,   207,   163,   210,
       4,   207,     3,   210,   119,   213,     6,   209,     6,   210,
      56,    56,    51,    91,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     132,   213,   213,   213,   213,    91,   213,   132,     4,   213,
     213,    91,   213,   213,   213,   213,   213,    91,    91,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,    91,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,     3,    65,     6,   213,    52,
     213,    53,   213,   213,   213,   213,   213,   213,   213,   213,
       6,   210,   213,   213,     6,   209,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   213,     6,
       6,     6,     6,     6,     6,   213,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   213,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   213,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   213,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       4,   213,     4,     4,     4,     4,   213,     3,   210,     6,
      61,     6,     6,     6,   210,   210,   210,   210,   210,   210,
     210,   210,   210,     6,     6,   210,   210,   210,   210,     6,
       4,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,     4,   210,   210,     6,
       6,   210,     6,   210,   210,   210,   210,   210,   210,   210,
     210,     4,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,     6,     6,   210,
     210,   210,   210,   210,    48,   210,   210,    40,    40,   210,
     213,   213,   210,   210,   210,   210,   210,   210,   210,    40,
      40,   210,   213,   213,   210,   210,   210,   209,   209,     3,
     210,   210,    60,   116,   572,   591,   650,   652,   201,    20,
     532,   832,   837,   334,   701,   358,   791,   384,   680,   680,
     797,   171,   190,   409,   411,   667,   698,   235,   476,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   307
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    77,   167,   184,
     209,   216,   217,   218,   220,   231,   232,   234,   235,   238,
     239,   240,   241,   242,   266,   271,   272,   273,   274,   275,
     276,   277,    33,    34,    35,    39,    40,    37,    33,    34,
      35,    39,    40,     3,   264,    75,   264,   164,   165,   169,
       3,   219,   220,   231,   232,   235,   238,   239,   240,   271,
     272,   273,   274,   275,     3,    33,    34,    36,    41,    42,
     159,   160,   161,   162,   164,   165,   168,   169,   170,   173,
     174,   175,   180,   181,   192,   196,    34,    34,    34,    34,
     157,   158,   159,     3,     3,   264,     3,   267,   268,   170,
      14,    16,   188,   241,   242,     0,   212,   324,    20,    22,
      28,   260,     8,   243,   245,    71,   323,   323,   323,   323,
     323,   325,   264,    71,   322,   322,   322,   322,   322,   211,
      14,   264,    75,    76,   209,     3,     3,     3,   219,   209,
       3,   264,     6,   171,   172,   171,   172,     3,   174,     6,
     193,   194,   195,   194,   197,   264,   264,   264,   264,    61,
      54,   213,     6,   184,   185,   189,   158,   168,   171,   172,
     174,   175,   176,   190,   191,     4,   210,   210,   218,    21,
     209,   244,   245,    64,   252,    66,   246,    72,     3,   264,
     264,   264,     3,    61,   209,   233,    73,     3,   264,   264,
     264,     3,     3,     3,   236,   237,    65,   257,     4,   321,
     321,     3,     4,     5,     6,    72,    73,    82,    84,   100,
     101,   102,   106,   133,   134,   135,   155,   178,   180,   182,
     202,   203,   204,   207,   209,   278,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   291,   292,   293,   294,   295,
     297,   298,   299,   300,   301,   303,   304,   305,   306,   307,
     308,   309,   310,   313,   314,   315,   316,   317,   318,     3,
       5,     6,    61,   166,     3,     5,     6,    61,   166,     3,
       5,     6,    61,   166,   210,    40,    43,    44,    48,    49,
       3,     3,     4,    10,    26,    27,   264,   209,   268,   321,
     163,     6,     3,     6,     4,    54,   244,   245,   278,    52,
      67,   250,    73,   132,    54,   209,   233,   264,     3,   230,
      38,   242,    61,   199,   213,   257,   281,    77,    77,   209,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,    72,    73,   282,   209,   209,    87,   281,   296,
       4,     4,     4,     4,     6,   318,   209,   118,   120,   122,
     123,   209,   209,   282,   282,     5,     6,   208,   301,   311,
     312,   242,   281,   210,   213,    54,   153,   154,    72,    74,
     130,   148,   149,   150,   151,   152,   156,   199,   200,   201,
     202,   203,   204,   205,   206,   211,   208,   213,   208,   213,
     208,   213,   208,   213,   208,   213,     3,     6,    47,    47,
      76,    77,   326,   243,     4,    40,    49,     6,   186,   187,
     210,   210,    81,   253,   247,   248,   281,   281,    68,   251,
       4,   240,     3,   126,   128,   221,   223,   224,   229,    54,
     209,   330,   210,   213,   209,   279,   264,   281,   237,   209,
     209,    64,   210,   278,   209,    72,   242,   281,   281,   296,
      83,    85,    87,     4,   209,   209,   209,   209,     4,     4,
     214,   210,   210,    76,   280,     3,   281,   281,    74,   156,
     209,    72,   129,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,   282,   282,     3,   204,   301,
       6,   311,     6,   312,     6,     5,    43,    45,    46,   209,
     209,     3,   209,   210,     6,    33,    42,   163,   163,   278,
      75,   254,   213,    69,    70,   249,   281,   132,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   106,   107,   108,   109,   110,
     111,   114,   117,   118,   121,   122,   123,   124,   225,   127,
     209,   210,   213,   240,     3,     3,   278,   213,    67,    68,
      78,    79,    80,   183,   319,   320,   319,   278,   210,   242,
     210,    54,    86,    83,    85,   281,   281,    75,   281,     4,
       3,   299,   281,   213,   256,   210,   213,     5,     6,   321,
     209,   282,   242,   278,   129,   153,   214,   214,     6,     6,
     230,   222,   224,     3,   328,   329,     6,     4,     4,   209,
     261,   262,   263,   264,   269,   177,   255,   248,     4,   209,
     209,   209,   209,   209,   209,   209,    72,   126,   128,   129,
     226,   227,   326,   209,   230,    32,   327,   223,   210,   210,
     209,     6,     6,     4,     3,     6,   210,   213,   210,   210,
     210,   225,   281,   281,    83,    86,   282,   213,   213,   213,
     213,     4,    65,   210,     4,    77,   242,   278,   210,   210,
     282,    50,    47,   210,   210,   213,   199,   210,   213,     6,
     240,   213,    55,    57,    58,    59,    60,    62,    63,   270,
       3,    54,   265,   283,   284,   285,   286,   287,   288,   289,
     290,   257,     6,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   119,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   119,    89,    90,    91,    92,    93,
      96,    97,   103,   104,   105,   119,    89,    90,    91,    92,
      93,    96,    97,   103,   104,   105,   119,    89,    90,    91,
      92,    93,    96,    97,   103,   104,   105,   119,    89,    90,
      91,    92,    93,    96,    97,   103,   104,   105,   119,   129,
     127,   131,   227,   228,   228,   230,   210,   209,   132,   163,
     278,   320,   210,    83,   281,   210,   207,   313,     4,   301,
     207,   302,   305,   310,   313,   213,   256,   281,   210,   209,
     210,   210,     6,     6,   224,     3,     4,     5,     6,   329,
      34,    36,   210,   262,    56,    56,     3,   213,    51,   259,
     210,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,    91,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,    91,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,    91,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,    91,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,    91,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,    91,   213,   300,   132,
     132,   210,   328,     4,     3,   210,     6,   213,   213,   256,
     213,   213,   210,   319,     6,   265,   263,   263,   209,   289,
      52,    53,   258,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   213,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   213,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   213,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   213,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   213,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   213,     6,
       4,     4,   210,   326,     4,     4,   210,     4,     4,   210,
       6,    61,   230,   278,   281,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,     6,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,     6,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,     6,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,     6,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,     6,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
       6,   210,   213,   256,   213,   213,   256,    40,    43,    44,
      48,    49,   281,   210,   210,   210,   210,   210,   210,   210,
       4,   210,     4,     6,   210,     6,     6,   213,   256,   213,
     256,   326,     6,    45,    46,     6,   210,     4,   210,    43,
      44,     6,   256,   326,   256,   125,   163,   326,     6,    48,
     210,   210,    40,    40,   125,   163,   326,   209,    40,    40,
       3,   209,   210,     3,   326,   210,   326
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   215,   216,   217,   217,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   221,   221,   222,   222,   223,   223,   224,   224,   224,
     224,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   226,   226,   227,   227,   227,   227,   228,
     228,   229,   229,   230,   230,   231,   232,   232,   233,   233,
     234,   234,   235,   236,   236,   237,   238,   238,   238,   238,
     238,   239,   239,   239,   240,   240,   240,   240,   241,   241,
     242,   243,   244,   244,   245,   246,   246,   247,   247,   248,
     249,   249,   249,   250,   250,   251,   251,   252,   252,   253,
     253,   254,   254,   255,   255,   256,   256,   257,   257,   258,
     258,   259,   259,   260,   260,   260,   260,   261,   261,   262,
     262,   263,   263,   264,   264,   265,   265,   265,   265,   266,
     266,   267,   267,   268,   269,   269,   270,   270,   270,   270,
     270,   270,   270,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   272,   272,   272,   273,   273,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   275,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   277,   277,   277,
     278,   278,   279,   279,   280,   280,   281,   281,   281,   281,
     281,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   283,   284,   284,   284,   284,   285,
     285,   285,   285,   286,   286,   287,   287,   288,   288,   289,
     289,   289,   289,   289,   289,   290,   290,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     292,   292,   293,   294,   294,   295,   295,   295,   295,   296,
     296,   297,   298,   298,   298,   298,   299,   299,   299,   299,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   300,
     300,   300,   301,   301,   301,   301,   302,   302,   302,   303,
     304,   304,   305,   305,   306,   307,   307,   308,   309,   309,
     310,   311,   312,   313,   313,   314,   315,   315,   316,   317,
     317,   318,   318,   318,   318,   318,   318,   318,   318,   318,
     318,   318,   318,   319,   319,   320,   320,   320,   320,   320,
     320,   321,   322,   322,   323,   323,   324,   324,   325,   325,
     326,   326,   327,   327,   328,   328,   329,   329,   329,   329,
     329,   330,   330
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
       4,     3,     3,     3,     5,     7,     7,     6,     8,     8,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,    14,    20,    16,    15,    13,    18,
      14,    13,    11,     8,    10,     5,     7,     4,     6,     1,
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
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2391 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2399 "parser.cpp"
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
#line 2413 "parser.cpp"
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
#line 2427 "parser.cpp"
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
#line 2441 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2452 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2460 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2469 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2478 "parser.cpp"
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
#line 2492 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2513 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2533 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2543 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2563 "parser.cpp"
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
#line 2577 "parser.cpp"
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
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2601 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2609 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2617 "parser.cpp"
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
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2640 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2648 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2656 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2664 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2672 "parser.cpp"
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
#line 2686 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2695 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2704 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2713 "parser.cpp"
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
#line 2726 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2735 "parser.cpp"
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
#line 2749 "parser.cpp"
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
#line 2763 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2773 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2782 "parser.cpp"
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
#line 2796 "parser.cpp"
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
#line 2813 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2821 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2829 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2837 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2845 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2853 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2861 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2869 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2877 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2885 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2893 "parser.cpp"
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
#line 2907 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2915 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2923 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2931 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2939 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2947 "parser.cpp"
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
#line 2960 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2968 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2976 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2984 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2992 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3000 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3008 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3016 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3024 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3032 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3040 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3048 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3056 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3064 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3072 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3080 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3088 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3096 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3104 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3112 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3120 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3128 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3136 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3144 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3155 "parser.cpp"
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
#line 3169 "parser.cpp"
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
#line 3183 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3194 "parser.cpp"
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

#line 3302 "parser.cpp"

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
#line 3517 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 510 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3528 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 516 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3539 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 523 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3545 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3551 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3557 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3563 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3569 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3575 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3581 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3587 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3593 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 532 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3599 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 533 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3605 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3611 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3617 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3623 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3629 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 539 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3635 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3641 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3647 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3653 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3659 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3665 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3671 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3677 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3683 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3689 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3695 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3701 "parser.cpp"
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
#line 3723 "parser.cpp"
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
#line 3743 "parser.cpp"
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
#line 3761 "parser.cpp"
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
#line 3794 "parser.cpp"
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
#line 3814 "parser.cpp"
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
#line 3850 "parser.cpp"
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
#line 3872 "parser.cpp"
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
#line 3893 "parser.cpp"
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
#line 3926 "parser.cpp"
    break;

  case 41: /* table_element_array: table_element  */
#line 748 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3935 "parser.cpp"
    break;

  case 42: /* table_element_array: table_element_array ',' table_element  */
#line 752 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3944 "parser.cpp"
    break;

  case 43: /* column_def_array: table_column  */
#line 757 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3953 "parser.cpp"
    break;

  case 44: /* column_def_array: column_def_array ',' table_column  */
#line 761 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3962 "parser.cpp"
    break;

  case 45: /* table_element: table_column  */
#line 767 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3970 "parser.cpp"
    break;

  case 46: /* table_element: table_constraint  */
#line 770 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3978 "parser.cpp"
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
#line 4034 "parser.cpp"
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
#line 4076 "parser.cpp"
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
#line 4133 "parser.cpp"
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
#line 4176 "parser.cpp"
    break;

  case 51: /* column_type: BOOLEAN  */
#line 957 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4182 "parser.cpp"
    break;

  case 52: /* column_type: TINYINT  */
#line 958 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4188 "parser.cpp"
    break;

  case 53: /* column_type: SMALLINT  */
#line 959 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4194 "parser.cpp"
    break;

  case 54: /* column_type: INTEGER  */
#line 960 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4200 "parser.cpp"
    break;

  case 55: /* column_type: INT  */
#line 961 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4206 "parser.cpp"
    break;

  case 56: /* column_type: BIGINT  */
#line 962 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4212 "parser.cpp"
    break;

  case 57: /* column_type: HUGEINT  */
#line 963 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4218 "parser.cpp"
    break;

  case 58: /* column_type: FLOAT  */
#line 964 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4224 "parser.cpp"
    break;

  case 59: /* column_type: REAL  */
#line 965 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4230 "parser.cpp"
    break;

  case 60: /* column_type: DOUBLE  */
#line 966 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4236 "parser.cpp"
    break;

  case 61: /* column_type: FLOAT16  */
#line 967 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4242 "parser.cpp"
    break;

  case 62: /* column_type: BFLOAT16  */
#line 968 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4248 "parser.cpp"
    break;

  case 63: /* column_type: DATE  */
#line 969 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4254 "parser.cpp"
    break;

  case 64: /* column_type: TIME  */
#line 970 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4260 "parser.cpp"
    break;

  case 65: /* column_type: DATETIME  */
#line 971 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4266 "parser.cpp"
    break;

  case 66: /* column_type: TIMESTAMP  */
#line 972 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4272 "parser.cpp"
    break;

  case 67: /* column_type: UUID  */
#line 973 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4278 "parser.cpp"
    break;

  case 68: /* column_type: POINT  */
#line 974 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4284 "parser.cpp"
    break;

  case 69: /* column_type: LINE  */
#line 975 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4290 "parser.cpp"
    break;

  case 70: /* column_type: LSEG  */
#line 976 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4296 "parser.cpp"
    break;

  case 71: /* column_type: BOX  */
#line 977 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4302 "parser.cpp"
    break;

  case 72: /* column_type: CIRCLE  */
#line 980 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4308 "parser.cpp"
    break;

  case 73: /* column_type: VARCHAR  */
#line 982 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4314 "parser.cpp"
    break;

  case 74: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4320 "parser.cpp"
    break;

  case 75: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 984 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4326 "parser.cpp"
    break;

  case 76: /* column_type: DECIMAL  */
#line 985 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4332 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 988 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4338 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 989 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4344 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 990 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4350 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 991 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4356 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 992 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4362 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 993 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4368 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 994 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4374 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 995 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4380 "parser.cpp"
    break;

  case 85: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 996 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4386 "parser.cpp"
    break;

  case 86: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 997 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4392 "parser.cpp"
    break;

  case 87: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 998 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4398 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 999 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4404 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1000 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4410 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1001 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4416 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1002 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4422 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1003 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4428 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1004 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4434 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1005 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4440 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1006 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4446 "parser.cpp"
    break;

  case 96: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1007 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4452 "parser.cpp"
    break;

  case 97: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1008 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4458 "parser.cpp"
    break;

  case 98: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1009 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4464 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 1010 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4470 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1011 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4476 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1012 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4482 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1013 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4488 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 1014 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4494 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1015 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4500 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1016 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4506 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1017 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4512 "parser.cpp"
    break;

  case 107: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1018 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4518 "parser.cpp"
    break;

  case 108: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1019 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4524 "parser.cpp"
    break;

  case 109: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1020 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4530 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 1021 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4536 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 1022 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4542 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1023 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4548 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 1024 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4554 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 1025 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4560 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 1026 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4566 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 1027 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4572 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1028 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4578 "parser.cpp"
    break;

  case 118: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1029 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4584 "parser.cpp"
    break;

  case 119: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1030 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4590 "parser.cpp"
    break;

  case 120: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1031 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4596 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 1032 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4602 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 1033 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4608 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1034 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4614 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 1035 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4620 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 1036 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4626 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 1037 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4632 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 1038 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4638 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1039 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4644 "parser.cpp"
    break;

  case 129: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1040 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4650 "parser.cpp"
    break;

  case 130: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1041 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4656 "parser.cpp"
    break;

  case 131: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1042 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4662 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 1043 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4668 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 1044 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4674 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 1045 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4680 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 1046 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4686 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 1047 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4692 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 1048 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4698 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 1049 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4704 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 1050 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4710 "parser.cpp"
    break;

  case 140: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 1051 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4716 "parser.cpp"
    break;

  case 141: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 1052 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4722 "parser.cpp"
    break;

  case 142: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 1053 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4728 "parser.cpp"
    break;

  case 143: /* column_constraints: column_constraint  */
#line 1072 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4737 "parser.cpp"
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
#line 4751 "parser.cpp"
    break;

  case 145: /* column_constraint: PRIMARY KEY  */
#line 1086 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4759 "parser.cpp"
    break;

  case 146: /* column_constraint: UNIQUE  */
#line 1089 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4767 "parser.cpp"
    break;

  case 147: /* column_constraint: NULLABLE  */
#line 1092 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4775 "parser.cpp"
    break;

  case 148: /* column_constraint: NOT NULLABLE  */
#line 1095 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4783 "parser.cpp"
    break;

  case 149: /* default_expr: DEFAULT constant_expr  */
#line 1099 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4791 "parser.cpp"
    break;

  case 150: /* default_expr: %empty  */
#line 1102 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4799 "parser.cpp"
    break;

  case 151: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1107 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4809 "parser.cpp"
    break;

  case 152: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1112 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4819 "parser.cpp"
    break;

  case 153: /* identifier_array: IDENTIFIER  */
#line 1119 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4830 "parser.cpp"
    break;

  case 154: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1125 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4841 "parser.cpp"
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
#line 4858 "parser.cpp"
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
#line 4897 "parser.cpp"
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
#line 4914 "parser.cpp"
    break;

  case 158: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1198 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4922 "parser.cpp"
    break;

  case 159: /* optional_identifier_array: %empty  */
#line 1201 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4930 "parser.cpp"
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
#line 4948 "parser.cpp"
    break;

  case 161: /* explain_statement: EXPLAIN explainable_statement  */
#line 1220 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4958 "parser.cpp"
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
#line 4975 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr  */
#line 1242 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4984 "parser.cpp"
    break;

  case 164: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1246 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4993 "parser.cpp"
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
#line 5005 "parser.cpp"
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
#line 5021 "parser.cpp"
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
#line 5039 "parser.cpp"
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
#line 5057 "parser.cpp"
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
#line 5075 "parser.cpp"
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
#line 5098 "parser.cpp"
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
#line 5156 "parser.cpp"
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
#line 5216 "parser.cpp"
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
#line 5268 "parser.cpp"
    break;

  case 174: /* select_statement: select_without_paren  */
#line 1504 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5276 "parser.cpp"
    break;

  case 175: /* select_statement: select_with_paren  */
#line 1507 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5284 "parser.cpp"
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
#line 5298 "parser.cpp"
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
#line 5312 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_without_paren ')'  */
#line 1529 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5320 "parser.cpp"
    break;

  case 179: /* select_with_paren: '(' select_with_paren ')'  */
#line 1532 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5328 "parser.cpp"
    break;

  case 180: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1536 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5337 "parser.cpp"
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
#line 5368 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1568 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5376 "parser.cpp"
    break;

  case 183: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1571 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5384 "parser.cpp"
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
#line 5405 "parser.cpp"
    break;

  case 185: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1593 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5413 "parser.cpp"
    break;

  case 186: /* order_by_clause: %empty  */
#line 1596 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5421 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr  */
#line 1600 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5430 "parser.cpp"
    break;

  case 188: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1604 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5439 "parser.cpp"
    break;

  case 189: /* order_by_expr: expr order_by_type  */
#line 1609 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5449 "parser.cpp"
    break;

  case 190: /* order_by_type: ASC  */
#line 1615 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5457 "parser.cpp"
    break;

  case 191: /* order_by_type: DESC  */
#line 1618 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5465 "parser.cpp"
    break;

  case 192: /* order_by_type: %empty  */
#line 1621 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5473 "parser.cpp"
    break;

  case 193: /* limit_expr: LIMIT expr  */
#line 1625 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5481 "parser.cpp"
    break;

  case 194: /* limit_expr: %empty  */
#line 1629 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5487 "parser.cpp"
    break;

  case 195: /* offset_expr: OFFSET expr  */
#line 1631 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5495 "parser.cpp"
    break;

  case 196: /* offset_expr: %empty  */
#line 1635 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5501 "parser.cpp"
    break;

  case 197: /* distinct: DISTINCT  */
#line 1637 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5509 "parser.cpp"
    break;

  case 198: /* distinct: %empty  */
#line 1640 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5517 "parser.cpp"
    break;

  case 199: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1644 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5525 "parser.cpp"
    break;

  case 200: /* highlight_clause: %empty  */
#line 1647 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5533 "parser.cpp"
    break;

  case 201: /* from_clause: FROM table_reference  */
#line 1651 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5541 "parser.cpp"
    break;

  case 202: /* from_clause: %empty  */
#line 1654 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5549 "parser.cpp"
    break;

  case 203: /* search_clause: SEARCH sub_search_array  */
#line 1658 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5559 "parser.cpp"
    break;

  case 204: /* search_clause: %empty  */
#line 1663 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5567 "parser.cpp"
    break;

  case 205: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1667 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5575 "parser.cpp"
    break;

  case 206: /* optional_search_filter_expr: %empty  */
#line 1670 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5583 "parser.cpp"
    break;

  case 207: /* where_clause: WHERE expr  */
#line 1674 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5591 "parser.cpp"
    break;

  case 208: /* where_clause: %empty  */
#line 1677 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5599 "parser.cpp"
    break;

  case 209: /* having_clause: HAVING expr  */
#line 1681 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5607 "parser.cpp"
    break;

  case 210: /* having_clause: %empty  */
#line 1684 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5615 "parser.cpp"
    break;

  case 211: /* group_by_clause: GROUP BY expr_array  */
#line 1688 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5623 "parser.cpp"
    break;

  case 212: /* group_by_clause: %empty  */
#line 1691 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5631 "parser.cpp"
    break;

  case 213: /* set_operator: UNION  */
#line 1695 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5639 "parser.cpp"
    break;

  case 214: /* set_operator: UNION ALL  */
#line 1698 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5647 "parser.cpp"
    break;

  case 215: /* set_operator: INTERSECT  */
#line 1701 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5655 "parser.cpp"
    break;

  case 216: /* set_operator: EXCEPT  */
#line 1704 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5663 "parser.cpp"
    break;

  case 217: /* table_reference: table_reference_unit  */
#line 1712 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5671 "parser.cpp"
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
#line 5689 "parser.cpp"
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
#line 5707 "parser.cpp"
    break;

  case 222: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1746 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5718 "parser.cpp"
    break;

  case 223: /* table_name: IDENTIFIER  */
#line 1755 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5728 "parser.cpp"
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
#line 5740 "parser.cpp"
    break;

  case 225: /* table_alias: AS IDENTIFIER  */
#line 1769 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5750 "parser.cpp"
    break;

  case 226: /* table_alias: IDENTIFIER  */
#line 1774 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5760 "parser.cpp"
    break;

  case 227: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1779 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5771 "parser.cpp"
    break;

  case 228: /* table_alias: %empty  */
#line 1785 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5779 "parser.cpp"
    break;

  case 229: /* with_clause: WITH with_expr_list  */
#line 1792 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5787 "parser.cpp"
    break;

  case 230: /* with_clause: %empty  */
#line 1795 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5795 "parser.cpp"
    break;

  case 231: /* with_expr_list: with_expr  */
#line 1799 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5804 "parser.cpp"
    break;

  case 232: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1802 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5813 "parser.cpp"
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
#line 5825 "parser.cpp"
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
#line 5837 "parser.cpp"
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
#line 5850 "parser.cpp"
    break;

  case 236: /* join_type: INNER  */
#line 1840 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5858 "parser.cpp"
    break;

  case 237: /* join_type: LEFT  */
#line 1843 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5866 "parser.cpp"
    break;

  case 238: /* join_type: RIGHT  */
#line 1846 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5874 "parser.cpp"
    break;

  case 239: /* join_type: OUTER  */
#line 1849 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5882 "parser.cpp"
    break;

  case 240: /* join_type: FULL  */
#line 1852 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5890 "parser.cpp"
    break;

  case 241: /* join_type: CROSS  */
#line 1855 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5898 "parser.cpp"
    break;

  case 242: /* join_type: %empty  */
#line 1858 "parser.y"
                {
}
#line 5905 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW DATABASES  */
#line 1864 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5914 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLES  */
#line 1868 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5923 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW VIEWS  */
#line 1872 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5932 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW CONFIGS  */
#line 1876 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5941 "parser.cpp"
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
#line 5953 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW PROFILES  */
#line 1887 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5962 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW BUFFER  */
#line 1891 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5971 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW MEMINDEX  */
#line 1895 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5980 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW QUERIES  */
#line 1899 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5989 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1903 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5999 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TRANSACTIONS  */
#line 1908 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 6008 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1912 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 6018 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW SESSION VARIABLES  */
#line 1917 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 6027 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1921 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 6036 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1925 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6047 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1931 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6058 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1937 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6069 "parser.cpp"
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
#line 6085 "parser.cpp"
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
#line 6101 "parser.cpp"
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
#line 6117 "parser.cpp"
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
#line 6134 "parser.cpp"
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
#line 6151 "parser.cpp"
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
#line 6169 "parser.cpp"
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
#line 6188 "parser.cpp"
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
#line 6204 "parser.cpp"
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
#line 6223 "parser.cpp"
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
#line 6244 "parser.cpp"
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
#line 6266 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW LOGS  */
#line 2085 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6275 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW DELTA LOGS  */
#line 2089 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6284 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW CATALOGS  */
#line 2093 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6293 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2097 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6302 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2101 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6311 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2105 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6322 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW MEMORY  */
#line 2111 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6331 "parser.cpp"
    break;

  case 278: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2115 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6340 "parser.cpp"
    break;

  case 279: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2119 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6349 "parser.cpp"
    break;

  case 280: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2123 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6360 "parser.cpp"
    break;

  case 281: /* flush_statement: FLUSH DATA  */
#line 2133 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6369 "parser.cpp"
    break;

  case 282: /* flush_statement: FLUSH LOG  */
#line 2137 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6378 "parser.cpp"
    break;

  case 283: /* flush_statement: FLUSH BUFFER  */
#line 2141 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6387 "parser.cpp"
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
#line 6402 "parser.cpp"
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
#line 6426 "parser.cpp"
    break;

  case 286: /* command_statement: USE IDENTIFIER  */
#line 2183 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6437 "parser.cpp"
    break;

  case 287: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2189 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6447 "parser.cpp"
    break;

  case 288: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2194 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6458 "parser.cpp"
    break;

  case 289: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2200 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6469 "parser.cpp"
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
#line 6482 "parser.cpp"
    break;

  case 291: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2214 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6493 "parser.cpp"
    break;

  case 292: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2220 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6504 "parser.cpp"
    break;

  case 293: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2226 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6515 "parser.cpp"
    break;

  case 294: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2232 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6526 "parser.cpp"
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
#line 6539 "parser.cpp"
    break;

  case 296: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2246 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6550 "parser.cpp"
    break;

  case 297: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2252 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6561 "parser.cpp"
    break;

  case 298: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2258 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6572 "parser.cpp"
    break;

  case 299: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2264 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6583 "parser.cpp"
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
#line 6596 "parser.cpp"
    break;

  case 301: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2278 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6607 "parser.cpp"
    break;

  case 302: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2284 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6618 "parser.cpp"
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
#line 6632 "parser.cpp"
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
#line 6646 "parser.cpp"
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
#line 6663 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2322 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6672 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2326 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6682 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2331 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6693 "parser.cpp"
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
#line 6705 "parser.cpp"
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
#line 6718 "parser.cpp"
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
#line 6732 "parser.cpp"
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
#line 6747 "parser.cpp"
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
#line 6762 "parser.cpp"
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
#line 6778 "parser.cpp"
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
#line 6794 "parser.cpp"
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
#line 6811 "parser.cpp"
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
#line 6828 "parser.cpp"
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
#line 6843 "parser.cpp"
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
#line 6859 "parser.cpp"
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
#line 6876 "parser.cpp"
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
#line 6894 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW LOGS  */
#line 2473 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6903 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2477 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6913 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2482 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6923 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2487 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6934 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2493 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6943 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2497 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6952 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2501 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6963 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SHOW NODES  */
#line 2507 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6972 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2511 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6983 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SHOW NODE  */
#line 2517 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6992 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN SET ADMIN  */
#line 2521 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 7002 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN SET STANDALONE  */
#line 2526 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 7012 "parser.cpp"
    break;

  case 334: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2531 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 7024 "parser.cpp"
    break;

  case 335: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2538 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7038 "parser.cpp"
    break;

  case 336: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2547 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 7052 "parser.cpp"
    break;

  case 337: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2557 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 7066 "parser.cpp"
    break;

  case 338: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2566 "parser.y"
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
#line 7083 "parser.cpp"
    break;

  case 339: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2578 "parser.y"
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
#line 7099 "parser.cpp"
    break;

  case 340: /* expr_array: expr_alias  */
#line 2594 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7108 "parser.cpp"
    break;

  case 341: /* expr_array: expr_array ',' expr_alias  */
#line 2598 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7117 "parser.cpp"
    break;

  case 342: /* expr_array_list: '(' expr_array ')'  */
#line 2603 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 7126 "parser.cpp"
    break;

  case 343: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2607 "parser.y"
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
#line 7152 "parser.cpp"
    break;

  case 344: /* expr_alias: expr AS IDENTIFIER  */
#line 2640 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7163 "parser.cpp"
    break;

  case 345: /* expr_alias: expr  */
#line 2646 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7171 "parser.cpp"
    break;

  case 351: /* operand: '(' expr ')'  */
#line 2656 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7179 "parser.cpp"
    break;

  case 352: /* operand: '(' select_without_paren ')'  */
#line 2659 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7190 "parser.cpp"
    break;

  case 353: /* operand: constant_expr  */
#line 2665 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7198 "parser.cpp"
    break;

  case 364: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2681 "parser.y"
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
#line 7218 "parser.cpp"
    break;

  case 365: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2699 "parser.y"
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
#line 7268 "parser.cpp"
    break;

  case 366: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2745 "parser.y"
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
#line 7311 "parser.cpp"
    break;

  case 367: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2784 "parser.y"
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
#line 7358 "parser.cpp"
    break;

  case 368: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2827 "parser.y"
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
#line 7406 "parser.cpp"
    break;

  case 369: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2874 "parser.y"
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
#line 7434 "parser.cpp"
    break;

  case 370: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2898 "parser.y"
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
#line 7461 "parser.cpp"
    break;

  case 371: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2921 "parser.y"
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
#line 7486 "parser.cpp"
    break;

  case 372: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2942 "parser.y"
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
#line 7511 "parser.cpp"
    break;

  case 373: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2963 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7525 "parser.cpp"
    break;

  case 374: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2972 "parser.y"
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
#line 7541 "parser.cpp"
    break;

  case 375: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2984 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7553 "parser.cpp"
    break;

  case 376: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2991 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7567 "parser.cpp"
    break;

  case 377: /* fusion_expr: FUSION '(' STRING ')'  */
#line 3001 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7578 "parser.cpp"
    break;

  case 378: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 3007 "parser.y"
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
#line 7594 "parser.cpp"
    break;

  case 379: /* sub_search: match_vector_expr  */
#line 3019 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7602 "parser.cpp"
    break;

  case 380: /* sub_search: match_text_expr  */
#line 3022 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7610 "parser.cpp"
    break;

  case 381: /* sub_search: match_tensor_expr  */
#line 3025 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7618 "parser.cpp"
    break;

  case 382: /* sub_search: match_sparse_expr  */
#line 3028 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7626 "parser.cpp"
    break;

  case 383: /* sub_search: query_expr  */
#line 3031 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7634 "parser.cpp"
    break;

  case 384: /* sub_search: fusion_expr  */
#line 3034 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7642 "parser.cpp"
    break;

  case 385: /* sub_search_array: sub_search  */
#line 3038 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7651 "parser.cpp"
    break;

  case 386: /* sub_search_array: sub_search_array ',' sub_search  */
#line 3042 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7660 "parser.cpp"
    break;

  case 387: /* function_expr: IDENTIFIER '(' ')'  */
#line 3047 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7673 "parser.cpp"
    break;

  case 388: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 3055 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7686 "parser.cpp"
    break;

  case 389: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 3063 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7700 "parser.cpp"
    break;

  case 390: /* function_expr: operand IS NOT NULLABLE  */
#line 3072 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7712 "parser.cpp"
    break;

  case 391: /* function_expr: operand IS NULLABLE  */
#line 3079 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7724 "parser.cpp"
    break;

  case 392: /* function_expr: NOT operand  */
#line 3086 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7736 "parser.cpp"
    break;

  case 393: /* function_expr: '-' operand  */
#line 3093 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7748 "parser.cpp"
    break;

  case 394: /* function_expr: '+' operand  */
#line 3100 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7760 "parser.cpp"
    break;

  case 395: /* function_expr: operand '-' operand  */
#line 3107 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7773 "parser.cpp"
    break;

  case 396: /* function_expr: operand '+' operand  */
#line 3115 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7786 "parser.cpp"
    break;

  case 397: /* function_expr: operand '*' operand  */
#line 3123 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7799 "parser.cpp"
    break;

  case 398: /* function_expr: operand '/' operand  */
#line 3131 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7812 "parser.cpp"
    break;

  case 399: /* function_expr: operand '%' operand  */
#line 3139 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7825 "parser.cpp"
    break;

  case 400: /* function_expr: operand '=' operand  */
#line 3147 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7838 "parser.cpp"
    break;

  case 401: /* function_expr: operand EQUAL operand  */
#line 3155 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7851 "parser.cpp"
    break;

  case 402: /* function_expr: operand NOT_EQ operand  */
#line 3163 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7864 "parser.cpp"
    break;

  case 403: /* function_expr: operand '<' operand  */
#line 3171 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7877 "parser.cpp"
    break;

  case 404: /* function_expr: operand '>' operand  */
#line 3179 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7890 "parser.cpp"
    break;

  case 405: /* function_expr: operand LESS_EQ operand  */
#line 3187 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7903 "parser.cpp"
    break;

  case 406: /* function_expr: operand GREATER_EQ operand  */
#line 3195 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7916 "parser.cpp"
    break;

  case 407: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3203 "parser.y"
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
#line 7951 "parser.cpp"
    break;

  case 408: /* function_expr: operand LIKE operand  */
#line 3233 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7964 "parser.cpp"
    break;

  case 409: /* function_expr: operand NOT LIKE operand  */
#line 3241 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7977 "parser.cpp"
    break;

  case 410: /* conjunction_expr: expr AND expr  */
#line 3250 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7990 "parser.cpp"
    break;

  case 411: /* conjunction_expr: expr OR expr  */
#line 3258 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 8003 "parser.cpp"
    break;

  case 412: /* between_expr: operand BETWEEN operand AND operand  */
#line 3267 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 8015 "parser.cpp"
    break;

  case 413: /* in_expr: operand IN '(' expr_array ')'  */
#line 3275 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8026 "parser.cpp"
    break;

  case 414: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3281 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 8037 "parser.cpp"
    break;

  case 415: /* case_expr: CASE expr case_check_array END  */
#line 3288 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8048 "parser.cpp"
    break;

  case 416: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3294 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8060 "parser.cpp"
    break;

  case 417: /* case_expr: CASE case_check_array END  */
#line 3301 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 8070 "parser.cpp"
    break;

  case 418: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3306 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8081 "parser.cpp"
    break;

  case 419: /* case_check_array: WHEN expr THEN expr  */
#line 3313 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8093 "parser.cpp"
    break;

  case 420: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3320 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8105 "parser.cpp"
    break;

  case 421: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3328 "parser.y"
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
#line 8136 "parser.cpp"
    break;

  case 422: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3355 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8147 "parser.cpp"
    break;

  case 423: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3361 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8158 "parser.cpp"
    break;

  case 424: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3367 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8170 "parser.cpp"
    break;

  case 425: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3374 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8182 "parser.cpp"
    break;

  case 426: /* column_expr: IDENTIFIER  */
#line 3382 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8194 "parser.cpp"
    break;

  case 427: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3389 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8206 "parser.cpp"
    break;

  case 428: /* column_expr: '*'  */
#line 3396 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8216 "parser.cpp"
    break;

  case 429: /* column_expr: column_expr '.' '*'  */
#line 3401 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8230 "parser.cpp"
    break;

  case 430: /* constant_expr: STRING  */
#line 3411 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8240 "parser.cpp"
    break;

  case 431: /* constant_expr: TRUE  */
#line 3416 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8250 "parser.cpp"
    break;

  case 432: /* constant_expr: FALSE  */
#line 3421 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8260 "parser.cpp"
    break;

  case 433: /* constant_expr: DOUBLE_VALUE  */
#line 3426 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8270 "parser.cpp"
    break;

  case 434: /* constant_expr: LONG_VALUE  */
#line 3431 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8280 "parser.cpp"
    break;

  case 435: /* constant_expr: DATE STRING  */
#line 3436 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8290 "parser.cpp"
    break;

  case 436: /* constant_expr: TIME STRING  */
#line 3441 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8300 "parser.cpp"
    break;

  case 437: /* constant_expr: DATETIME STRING  */
#line 3446 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8310 "parser.cpp"
    break;

  case 438: /* constant_expr: TIMESTAMP STRING  */
#line 3451 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8320 "parser.cpp"
    break;

  case 439: /* constant_expr: INTERVAL interval_expr  */
#line 3456 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8328 "parser.cpp"
    break;

  case 440: /* constant_expr: interval_expr  */
#line 3459 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8336 "parser.cpp"
    break;

  case 441: /* constant_expr: common_array_expr  */
#line 3462 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8344 "parser.cpp"
    break;

  case 442: /* common_array_expr: array_expr  */
#line 3466 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8352 "parser.cpp"
    break;

  case 443: /* common_array_expr: subarray_array_expr  */
#line 3469 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8360 "parser.cpp"
    break;

  case 444: /* common_array_expr: sparse_array_expr  */
#line 3472 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8368 "parser.cpp"
    break;

  case 445: /* common_array_expr: empty_array_expr  */
#line 3475 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8376 "parser.cpp"
    break;

  case 446: /* common_sparse_array_expr: sparse_array_expr  */
#line 3479 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8384 "parser.cpp"
    break;

  case 447: /* common_sparse_array_expr: array_expr  */
#line 3482 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8392 "parser.cpp"
    break;

  case 448: /* common_sparse_array_expr: empty_array_expr  */
#line 3485 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8400 "parser.cpp"
    break;

  case 449: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3489 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8408 "parser.cpp"
    break;

  case 450: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3493 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8418 "parser.cpp"
    break;

  case 451: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3498 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8427 "parser.cpp"
    break;

  case 452: /* sparse_array_expr: long_sparse_array_expr  */
#line 3503 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8435 "parser.cpp"
    break;

  case 453: /* sparse_array_expr: double_sparse_array_expr  */
#line 3506 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8443 "parser.cpp"
    break;

  case 454: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3510 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8451 "parser.cpp"
    break;

  case 455: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3514 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8463 "parser.cpp"
    break;

  case 456: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3521 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8474 "parser.cpp"
    break;

  case 457: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3528 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8482 "parser.cpp"
    break;

  case 458: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3532 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8494 "parser.cpp"
    break;

  case 459: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3539 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8505 "parser.cpp"
    break;

  case 460: /* empty_array_expr: '[' ']'  */
#line 3546 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8513 "parser.cpp"
    break;

  case 461: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3550 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8521 "parser.cpp"
    break;

  case 462: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3554 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8529 "parser.cpp"
    break;

  case 463: /* array_expr: long_array_expr  */
#line 3558 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8537 "parser.cpp"
    break;

  case 464: /* array_expr: double_array_expr  */
#line 3561 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8545 "parser.cpp"
    break;

  case 465: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3565 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8553 "parser.cpp"
    break;

  case 466: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3569 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8563 "parser.cpp"
    break;

  case 467: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3574 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8572 "parser.cpp"
    break;

  case 468: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3579 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8580 "parser.cpp"
    break;

  case 469: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3583 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8590 "parser.cpp"
    break;

  case 470: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3588 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8599 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE SECONDS  */
#line 3593 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8610 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE SECOND  */
#line 3599 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8621 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE MINUTES  */
#line 3605 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8632 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE MINUTE  */
#line 3611 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8643 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE HOURS  */
#line 3617 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8654 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE HOUR  */
#line 3623 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8665 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE DAYS  */
#line 3629 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8676 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE DAY  */
#line 3635 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8687 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE MONTHS  */
#line 3641 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8698 "parser.cpp"
    break;

  case 480: /* interval_expr: LONG_VALUE MONTH  */
#line 3647 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8709 "parser.cpp"
    break;

  case 481: /* interval_expr: LONG_VALUE YEARS  */
#line 3653 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8720 "parser.cpp"
    break;

  case 482: /* interval_expr: LONG_VALUE YEAR  */
#line 3659 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8731 "parser.cpp"
    break;

  case 483: /* copy_option_list: copy_option  */
#line 3670 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8740 "parser.cpp"
    break;

  case 484: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3674 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8749 "parser.cpp"
    break;

  case 485: /* copy_option: FORMAT IDENTIFIER  */
#line 3679 "parser.y"
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
#line 8785 "parser.cpp"
    break;

  case 486: /* copy_option: DELIMITER STRING  */
#line 3710 "parser.y"
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
#line 8800 "parser.cpp"
    break;

  case 487: /* copy_option: HEADER  */
#line 3720 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8810 "parser.cpp"
    break;

  case 488: /* copy_option: OFFSET LONG_VALUE  */
#line 3725 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8820 "parser.cpp"
    break;

  case 489: /* copy_option: LIMIT LONG_VALUE  */
#line 3730 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8830 "parser.cpp"
    break;

  case 490: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3735 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8840 "parser.cpp"
    break;

  case 491: /* file_path: STRING  */
#line 3741 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8848 "parser.cpp"
    break;

  case 492: /* if_exists: IF EXISTS  */
#line 3745 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8854 "parser.cpp"
    break;

  case 493: /* if_exists: %empty  */
#line 3746 "parser.y"
  { (yyval.bool_value) = false; }
#line 8860 "parser.cpp"
    break;

  case 494: /* if_not_exists: IF NOT EXISTS  */
#line 3748 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8866 "parser.cpp"
    break;

  case 495: /* if_not_exists: %empty  */
#line 3749 "parser.y"
  { (yyval.bool_value) = false; }
#line 8872 "parser.cpp"
    break;

  case 498: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3764 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8885 "parser.cpp"
    break;

  case 499: /* if_not_exists_info: %empty  */
#line 3772 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8893 "parser.cpp"
    break;

  case 500: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3776 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8901 "parser.cpp"
    break;

  case 501: /* with_index_param_list: %empty  */
#line 3779 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8909 "parser.cpp"
    break;

  case 502: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3783 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8917 "parser.cpp"
    break;

  case 503: /* optional_table_properties_list: %empty  */
#line 3786 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8925 "parser.cpp"
    break;

  case 504: /* index_param_list: index_param  */
#line 3790 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8934 "parser.cpp"
    break;

  case 505: /* index_param_list: index_param_list ',' index_param  */
#line 3794 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8943 "parser.cpp"
    break;

  case 506: /* index_param: IDENTIFIER  */
#line 3799 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8954 "parser.cpp"
    break;

  case 507: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3805 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8969 "parser.cpp"
    break;

  case 508: /* index_param: IDENTIFIER '=' STRING  */
#line 3815 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8984 "parser.cpp"
    break;

  case 509: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3825 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8997 "parser.cpp"
    break;

  case 510: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3833 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 9010 "parser.cpp"
    break;

  case 511: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3844 "parser.y"
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
#line 9046 "parser.cpp"
    break;

  case 512: /* index_info: '(' IDENTIFIER ')'  */
#line 3875 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 9057 "parser.cpp"
    break;


#line 9061 "parser.cpp"

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

#line 3882 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

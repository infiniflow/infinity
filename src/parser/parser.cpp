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
#define YYLAST   1445

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  215
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  116
/* YYNRULES -- Number of rules.  */
#define YYNRULES  509
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1141

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
     549,   550,   557,   574,   590,   619,   635,   653,   682,   686,
     691,   695,   701,   704,   711,   762,   799,   851,   891,   892,
     893,   894,   895,   896,   897,   898,   899,   900,   901,   902,
     903,   904,   905,   906,   907,   908,   909,   910,   911,   914,
     916,   917,   918,   919,   922,   923,   924,   925,   926,   927,
     928,   929,   930,   931,   932,   933,   934,   935,   936,   937,
     938,   939,   940,   941,   942,   943,   944,   945,   946,   947,
     948,   949,   950,   951,   952,   953,   954,   955,   956,   957,
     958,   959,   960,   961,   962,   963,   964,   965,   966,   967,
     968,   969,   970,   971,   972,   973,   974,   975,   976,   977,
     978,   979,   980,   981,   982,   983,   984,   985,   986,   987,
    1006,  1010,  1020,  1023,  1026,  1029,  1033,  1036,  1041,  1046,
    1053,  1059,  1069,  1085,  1119,  1132,  1135,  1142,  1154,  1163,
    1176,  1180,  1185,  1198,  1211,  1226,  1241,  1256,  1279,  1332,
    1387,  1438,  1441,  1444,  1453,  1463,  1466,  1470,  1475,  1502,
    1505,  1510,  1527,  1530,  1534,  1538,  1543,  1549,  1552,  1555,
    1559,  1563,  1565,  1569,  1571,  1574,  1578,  1581,  1585,  1588,
    1592,  1597,  1601,  1604,  1608,  1611,  1615,  1618,  1622,  1625,
    1629,  1632,  1635,  1638,  1646,  1649,  1664,  1664,  1666,  1680,
    1689,  1694,  1703,  1708,  1713,  1719,  1726,  1729,  1733,  1736,
    1741,  1753,  1760,  1774,  1777,  1780,  1783,  1786,  1789,  1792,
    1798,  1802,  1806,  1810,  1814,  1821,  1825,  1829,  1833,  1837,
    1842,  1846,  1851,  1855,  1859,  1865,  1871,  1877,  1888,  1899,
    1910,  1922,  1934,  1947,  1961,  1972,  1986,  2002,  2019,  2023,
    2027,  2031,  2035,  2039,  2045,  2049,  2053,  2057,  2067,  2071,
    2075,  2083,  2094,  2117,  2123,  2128,  2134,  2140,  2148,  2154,
    2160,  2166,  2172,  2180,  2186,  2192,  2198,  2204,  2212,  2218,
    2224,  2233,  2243,  2256,  2260,  2265,  2271,  2278,  2286,  2295,
    2305,  2315,  2326,  2337,  2349,  2361,  2371,  2382,  2394,  2407,
    2411,  2416,  2421,  2427,  2431,  2435,  2441,  2445,  2451,  2455,
    2460,  2465,  2472,  2481,  2491,  2500,  2512,  2528,  2532,  2537,
    2541,  2574,  2580,  2584,  2585,  2586,  2587,  2588,  2590,  2593,
    2599,  2602,  2603,  2604,  2605,  2606,  2607,  2608,  2609,  2610,
    2611,  2615,  2633,  2679,  2718,  2761,  2808,  2832,  2855,  2876,
    2897,  2906,  2918,  2925,  2935,  2941,  2953,  2956,  2959,  2962,
    2965,  2968,  2972,  2976,  2981,  2989,  2997,  3006,  3013,  3020,
    3027,  3034,  3041,  3049,  3057,  3065,  3073,  3081,  3089,  3097,
    3105,  3113,  3121,  3129,  3137,  3167,  3175,  3184,  3192,  3201,
    3209,  3215,  3222,  3228,  3235,  3240,  3247,  3254,  3262,  3289,
    3295,  3301,  3308,  3316,  3323,  3330,  3335,  3345,  3350,  3355,
    3360,  3365,  3370,  3375,  3380,  3385,  3390,  3393,  3396,  3400,
    3403,  3406,  3409,  3413,  3416,  3419,  3423,  3427,  3432,  3437,
    3440,  3444,  3448,  3455,  3462,  3466,  3473,  3480,  3484,  3488,
    3492,  3495,  3499,  3503,  3508,  3513,  3517,  3522,  3527,  3533,
    3539,  3545,  3551,  3557,  3563,  3569,  3575,  3581,  3587,  3593,
    3604,  3608,  3613,  3644,  3654,  3659,  3664,  3669,  3675,  3679,
    3680,  3682,  3683,  3685,  3686,  3698,  3706,  3710,  3713,  3717,
    3720,  3724,  3728,  3733,  3739,  3749,  3759,  3767,  3778,  3809
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

#define YYPACT_NINF (-673)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-497)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     792,   472,   125,   483,    86,    -4,    86,   225,    70,   734,
     134,   165,   221,   251,   255,   288,   294,   308,   180,    56,
     -38,   356,   148,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,    93,  -673,  -673,   375,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,   321,   321,   321,   321,    35,    86,   362,   362,
     362,   362,   362,   216,   428,    86,    16,   441,   451,   462,
     184,  -673,  -673,  -673,  -673,  -673,  -673,  -673,    93,  -673,
    -673,  -673,  -673,  -673,   226,   469,    86,  -673,  -673,  -673,
    -673,  -673,   477,  -673,   249,   318,  -673,   499,  -673,   335,
    -673,  -673,   547,  -673,   284,   116,    86,    86,    86,    86,
    -673,  -673,  -673,  -673,    -9,  -673,   466,   349,  -673,   586,
     239,   285,   605,   408,   422,  -673,    34,  -673,   621,  -673,
    -673,    13,   590,  -673,   598,   610,   681,    86,    86,    86,
     684,   632,   492,   634,   702,    86,    86,    86,   703,   705,
     707,   646,   711,   711,   539,    82,   102,   149,  -673,   506,
    -673,   444,  -673,  -673,   717,  -673,   719,  -673,  -673,  -673,
     720,  -673,  -673,  -673,  -673,   302,  -673,  -673,  -673,    86,
     516,   308,   711,  -673,   560,  -673,   721,  -673,  -673,   723,
    -673,  -673,   722,  -673,   725,   676,  -673,  -673,  -673,  -673,
      13,  -673,  -673,  -673,   539,   679,   666,   665,  -673,    36,
    -673,   492,  -673,    86,   733,    76,  -673,  -673,  -673,  -673,
    -673,   678,  -673,   541,   -46,  -673,   539,  -673,  -673,   667,
     672,   542,  -673,  -673,   827,   579,   544,   545,   413,   746,
     751,   752,   754,  -673,  -673,   756,   551,   416,   557,   562,
     663,   663,  -673,     9,   426,   111,  -673,    87,   783,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,   561,  -673,  -673,  -673,   -71,  -673,  -673,    73,
    -673,   166,  -673,  -673,  -673,   178,  -673,   250,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,   770,   768,  -673,  -673,  -673,
    -673,  -673,  -673,   730,   731,   704,   708,   375,  -673,  -673,
     775,    96,  -673,   778,  -673,   354,   577,   580,   -26,   539,
     539,   724,  -673,   -38,   121,   737,   584,  -673,   195,   585,
    -673,    86,   539,   707,  -673,   460,   591,   602,   200,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,   663,   603,   806,   718,   539,   539,    11,   351,  -673,
    -673,  -673,  -673,   827,  -673,   809,   611,   615,   616,   617,
     810,   815,   432,   432,  -673,   613,  -673,  -673,  -673,  -673,
     622,   -44,   758,   539,   830,   539,   539,   -54,   626,     6,
     663,   663,   663,   663,   663,   663,   663,   663,   663,   663,
     663,   663,   663,   663,    19,  -673,   630,  -673,   832,  -673,
     833,  -673,   834,  -673,   837,   801,   534,   642,   647,   849,
     649,  -673,   653,  -673,   853,  -673,   229,   701,   710,  -673,
    -673,   539,   796,   655,  -673,   205,   460,   539,  -673,    93,
     925,   747,   668,   281,  -673,  -673,  -673,   -38,   872,  -673,
    -673,   884,   539,   675,  -673,   460,  -673,   198,   198,   539,
    -673,   291,   718,   735,   682,    97,   -24,   448,  -673,   539,
     539,   814,   539,   893,    30,   539,   687,   384,   620,  -673,
    -673,   711,  -673,  -673,  -673,   748,   696,   663,   426,   777,
    -673,   793,   793,   246,   246,   139,   793,   793,   246,   246,
     432,   432,  -673,  -673,  -673,  -673,  -673,  -673,   697,  -673,
     698,  -673,  -673,  -673,   904,   910,  -673,   733,   914,  -673,
     915,  -673,  -673,   913,  -673,   916,   917,   709,    10,   750,
     539,  -673,  -673,  -673,   460,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,   715,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,   716,
     728,   729,   736,   738,   739,   259,   741,   733,   896,   121,
      93,   742,  -673,   386,   744,   923,   934,   942,   948,  -673,
     952,   388,  -673,   390,   392,  -673,   765,  -673,   925,   539,
    -673,   539,   145,   172,   663,   140,   764,  -673,   157,   150,
      57,   769,  -673,   956,  -673,  -673,   901,   426,   793,   771,
     397,  -673,   663,   955,   975,   940,   944,   414,   421,  -673,
     803,   433,  -673,   994,  -673,  -673,   -38,   790,   514,  -673,
     209,  -673,   372,   646,  -673,   998,   377,   599,   757,   961,
     978,   995,   908,   903,  -673,  -673,   236,  -673,   907,   733,
     434,   831,  -673,  -673,   878,  -673,   539,  -673,  -673,  -673,
    -673,  -673,  -673,   198,  -673,  -673,  -673,   835,   460,   169,
    -673,   539,   680,   848,  1040,   630,   852,   843,   539,  -673,
     850,   854,   851,   440,  -673,  -673,   806,  1056,  1066,  -673,
    -673,   914,   552,  -673,   915,   365,    47,    10,  1017,  -673,
    -673,  -673,  -673,  -673,  -673,  1020,  -673,  1074,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,   865,  1028,   445,   876,
     877,   880,   881,   882,   883,   888,   889,   890,  1013,   892,
     894,   895,   897,   898,   899,   900,   902,   905,   906,  1015,
     909,   911,   912,   918,   919,   920,   921,   922,   924,   926,
    1018,   927,   928,   929,   930,   931,   932,   933,   935,   936,
     937,  1025,   938,   939,   941,   943,   945,   946,   947,   949,
     950,   951,  1026,   953,   954,   957,   958,   959,   960,   962,
     963,   964,   965,  1029,   966,  -673,  -673,    26,  -673,   989,
     991,   446,  -673,   915,  1123,   447,  -673,  -673,  -673,   460,
    -673,   643,   967,   968,   969,    12,   970,  -673,  -673,  -673,
    1062,   974,   460,  -673,   198,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  -673,  1122,  -673,   209,   514,    10,
      10,   976,   372,  1077,  1083,  -673,  1124,  1132,  1141,  1147,
    1149,  1151,  1155,  1159,  1162,  1163,   973,  1168,  1181,  1182,
    1183,  1184,  1185,  1186,  1187,  1188,  1189,   983,  1191,  1192,
    1193,  1194,  1195,  1196,  1197,  1198,  1199,  1200,   996,  1201,
    1202,  1204,  1205,  1206,  1207,  1208,  1209,  1210,  1211,  1005,
    1213,  1214,  1215,  1216,  1217,  1218,  1219,  1220,  1221,  1222,
    1016,  1224,  1225,  1226,  1227,  1228,  1229,  1230,  1231,  1232,
    1233,  1027,  1235,  -673,  1238,  1239,  -673,   452,   708,  -673,
    -673,  1240,   242,  1035,  1242,  1243,  -673,   465,  1244,  -673,
    -673,  1190,   733,  -673,   539,   539,  -673,  1038,  1039,  1042,
    1043,  1044,  1045,  1046,  1047,  1048,  1049,  1254,  1051,  1052,
    1053,  1054,  1055,  1057,  1058,  1059,  1060,  1061,  1260,  1063,
    1064,  1065,  1067,  1068,  1069,  1070,  1071,  1072,  1073,  1266,
    1075,  1076,  1078,  1079,  1080,  1081,  1082,  1084,  1085,  1086,
    1270,  1087,  1088,  1089,  1090,  1091,  1092,  1093,  1094,  1095,
    1096,  1278,  1097,  1098,  1099,  1100,  1101,  1102,  1103,  1104,
    1105,  1106,  1281,  1107,  -673,  -673,  -673,  -673,  1108,   843,
    -673,  1109,  1110,  -673,   481,   539,   473,   709,   460,  -673,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  1114,
    -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    1115,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  1116,  -673,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  1117,  -673,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  1118,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  1119,  -673,  1289,  1120,  1314,    60,
    1121,  1313,  1326,  -673,  -673,  -673,   460,  -673,  -673,  -673,
    -673,  -673,  -673,  -673,  1125,  -673,  1126,   843,   708,  1327,
     625,    68,  1127,  1316,  1130,  -673,   633,  1328,  -673,   843,
     708,   843,    67,  1329,  -673,  1288,  1131,  -673,  1133,  1302,
    1304,  -673,  -673,  -673,   155,  -673,  -673,  1136,  1306,  1307,
    -673,  1345,  -673,  1140,  1142,  1347,   708,  1143,  -673,   708,
    -673
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     227,     0,     0,     0,     0,     0,     0,     0,   227,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     227,     0,   494,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   172,   171,     0,     8,    14,    15,    16,    17,
      18,    19,   492,   492,   492,   492,   492,     0,   490,   490,
     490,   490,   490,   220,     0,     0,     0,     0,     0,     0,
     227,   158,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,     0,   241,   242,   240,
     246,   250,     0,   247,     0,     0,   243,     0,   245,     0,
     268,   270,     0,   248,     0,   274,     0,     0,     0,     0,
     278,   279,   280,   283,   220,   281,     0,   226,   228,     0,
       0,     0,     0,     0,     0,     1,   227,     2,   210,   212,
     213,     0,   195,   177,   183,     0,     0,     0,     0,     0,
       0,     0,   156,     0,     0,     0,     0,     0,     0,     0,
       0,   205,     0,     0,     0,     0,     0,     0,   157,     0,
     256,   257,   251,   252,     0,   253,     0,   244,   269,   249,
       0,   272,   271,   275,   276,     0,   302,   300,   301,     0,
       0,     0,     0,   329,     0,   330,     0,   323,   324,     0,
     319,   303,     0,   326,   328,     0,   176,   175,     4,   211,
       0,   173,   174,   194,     0,     0,   191,     0,    32,     0,
      33,   156,   495,     0,     0,   227,   489,   163,   165,   164,
     166,     0,   221,     0,   205,   160,     0,   152,   488,     0,
       0,   423,   427,   430,   431,     0,     0,     0,     0,     0,
       0,     0,     0,   428,   429,     0,     0,     0,     0,     0,
       0,     0,   425,     0,   227,     0,   337,   342,   343,   357,
     355,   358,   356,   359,   360,   352,   347,   346,   345,   353,
     354,   344,   351,   350,   438,   440,     0,   441,   449,     0,
     450,     0,   442,   439,   460,     0,   461,     0,   437,   287,
     289,   288,   285,   286,   292,   294,   293,   290,   291,   297,
     299,   298,   295,   296,   277,     0,     0,   259,   258,   264,
     254,   255,   273,     0,     0,     0,   498,     0,   229,   284,
       0,   320,   325,   304,   327,     0,     0,     0,   197,     0,
       0,   193,   491,   227,     0,     0,     0,   150,     0,     0,
     154,     0,     0,     0,   159,   204,     0,     0,     0,   469,
     468,   471,   470,   473,   472,   475,   474,   477,   476,   479,
     478,     0,     0,   389,   227,     0,     0,     0,     0,   432,
     433,   434,   435,     0,   436,     0,     0,     0,     0,     0,
       0,     0,   391,   390,   466,   463,   457,   447,   452,   455,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   446,     0,   451,     0,   454,
       0,   462,     0,   465,     0,   265,   260,     0,     0,     0,
       0,   282,     0,   331,     0,   321,     0,     0,     0,   180,
     179,     0,   199,   182,   184,   189,   190,     0,   178,    35,
       0,     0,     0,     0,    38,    42,    43,   227,     0,    37,
     155,     0,     0,   153,   167,   162,   161,     0,     0,     0,
     384,     0,   227,     0,     0,     0,     0,     0,   414,     0,
       0,     0,     0,     0,     0,     0,   203,     0,     0,   349,
     348,     0,   338,   341,   407,   408,     0,     0,   227,     0,
     388,   398,   399,   402,   403,     0,   405,   397,   400,   401,
     393,   392,   394,   395,   396,   424,   426,   448,     0,   453,
       0,   456,   464,   467,     0,     0,   261,     0,     0,   334,
       0,   230,   322,     0,   305,     0,     0,   196,     0,   201,
       0,   187,   188,   186,   192,    48,    51,    52,    49,    50,
      53,    54,    70,    55,    57,    56,    73,    60,    61,    62,
      58,    59,    63,    64,    65,    66,    67,    68,    69,     0,
       0,     0,     0,     0,     0,   498,     0,     0,   500,     0,
      36,     0,   151,     0,     0,     0,     0,     0,     0,   484,
       0,     0,   480,     0,     0,   385,     0,   419,     0,     0,
     412,     0,     0,     0,     0,     0,     0,   423,     0,     0,
       0,     0,   374,     0,   459,   458,     0,   227,   406,     0,
       0,   387,     0,     0,     0,   266,   262,     0,     0,    40,
     503,     0,   501,   306,   332,   333,   227,   198,   214,   216,
     225,   217,     0,   205,   185,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   143,   144,   147,   140,   147,     0,
       0,     0,    34,    39,   509,   339,     0,   486,   485,   483,
     482,   487,   170,     0,   168,   386,   420,     0,   416,     0,
     415,     0,     0,     0,     0,     0,     0,   203,     0,   372,
       0,     0,     0,     0,   421,   410,   409,     0,     0,   336,
     335,     0,     0,   497,     0,     0,     0,     0,     0,   234,
     235,   236,   237,   233,   238,     0,   223,     0,   218,   378,
     376,   379,   377,   380,   381,   382,   200,   209,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   145,   142,     0,   141,    45,
      44,     0,   149,     0,     0,     0,   481,   418,   413,   417,
     404,     0,     0,   203,     0,     0,     0,   443,   445,   444,
       0,     0,   202,   375,     0,   422,   411,   267,   263,    41,
     504,   505,   507,   506,   502,     0,   307,   225,   215,     0,
       0,   222,     0,     0,   207,    72,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   146,     0,     0,   148,     0,   498,   340,
     463,     0,     0,     0,     0,     0,   373,     0,   308,   219,
     231,     0,     0,   383,     0,     0,   181,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    47,    46,   499,   508,     0,   203,
     370,     0,   203,   169,     0,     0,     0,   208,   206,    71,
      77,    78,    75,    76,    79,    80,    81,    82,    83,     0,
      74,   121,   122,   119,   120,   123,   124,   125,   126,   127,
       0,   118,    88,    89,    86,    87,    90,    91,    92,    93,
      94,     0,    85,    99,   100,    97,    98,   101,   102,   103,
     104,   105,     0,    96,   132,   133,   130,   131,   134,   135,
     136,   137,   138,     0,   129,   110,   111,   108,   109,   112,
     113,   114,   115,   116,     0,   107,     0,     0,     0,     0,
       0,     0,     0,   310,   309,   315,   232,   224,    84,   128,
      95,   106,   139,   117,   203,   371,     0,   203,   498,   316,
     311,     0,     0,     0,     0,   369,     0,     0,   312,   203,
     498,   203,   498,     0,   317,   313,     0,   365,     0,     0,
       0,   368,   318,   314,   498,   361,   367,     0,     0,     0,
     364,     0,   363,     0,     0,     0,   498,     0,   366,   498,
     362
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -673,  -673,  -673,  1241,  1291,    52,  -673,  -673,   785,  -502,
     767,  -673,   712,   713,  -673,  -511,   223,   297,  1158,  -673,
     301,  -673,  1023,   309,   311,    -7,  1340,   -17,  1111,  1172,
     -67,  -673,  -673,   836,  -673,  -673,  -673,  -673,  -673,  -673,
    -673,  -672,  -207,  -673,  -673,  -673,  -673,   670,  -131,    21,
     536,  -673,  -673,  1203,  -673,  -673,   312,   324,   338,   340,
     346,  -673,  -673,  -192,  -673,   981,  -216,  -217,  -606,  -603,
    -597,  -596,  -594,  -590,   533,  -673,  -673,  -673,  -673,  -673,
    -673,  1011,  -673,  -673,   971,   582,  -234,  -673,  -673,  -673,
     694,  -673,  -673,  -673,  -673,   695,   972,   977,    24,  -673,
    -673,  -673,  -673,  1137,  -448,   714,  -132,   515,   543,  -673,
    -673,  -561,  -673,   583,   685,  -673
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   443,   618,   444,   445,
     565,   646,   647,   789,   446,   328,    25,    26,   205,    27,
      28,   214,   215,    29,    30,    31,    32,    33,   123,   191,
     124,   196,   433,   434,   533,   321,   438,   194,   432,   529,
     633,   601,   217,   926,   834,   121,   627,   628,   629,   630,
     708,    34,   107,   108,   631,   705,    35,    36,    37,    38,
      39,    40,    41,   245,   453,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   715,   716,   255,   256,   257,   258,
     259,   358,   260,   261,   262,   263,   264,   806,   265,   266,
     267,   268,   269,   270,   271,   272,   378,   379,   273,   274,
     275,   276,   277,   278,   581,   582,   219,   134,   126,   117,
     131,   421,   652,   621,   622,   449
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     335,    68,   318,   114,   648,   811,   617,   334,   353,   377,
     583,   220,   357,    53,   374,   375,   619,   374,   375,   216,
     486,   122,   505,   372,   373,    54,   709,    56,   381,   710,
     222,   223,   224,   597,  -493,   711,   712,   105,   713,    17,
     309,     1,   714,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   169,    68,   192,   431,   650,    11,    12,    13,
      62,   677,   589,    14,    15,    16,  1097,   118,   132,   119,
     110,    55,   111,    60,  1109,   120,   141,     1,   489,     2,
       3,     4,     5,     6,     7,   279,     9,   280,   281,    53,
     323,   142,   143,    11,    12,    13,  -496,   151,   356,    14,
      15,    16,   487,   435,   436,   284,   125,   285,   286,   385,
     386,    17,    62,   118,   329,   119,   455,   165,   166,   167,
     168,   120,   678,   317,   440,   678,   229,   230,   231,   385,
     386,   913,   232,   678,   353,   490,   424,   405,   791,   465,
     466,   384,   406,   282,   420,   425,   461,    17,   199,   200,
     201,   588,   289,    17,   290,   291,   208,   209,   210,   233,
     234,   235,    47,   287,   385,   386,   480,   333,    96,   484,
     485,    20,   507,   491,   492,   493,   494,   495,   496,   497,
     498,   499,   500,   501,   502,   503,   504,   383,   330,   819,
     306,     1,  1119,     2,     3,     4,     5,     6,     7,    97,
       9,    18,   139,   221,   222,   223,   224,    11,    12,    13,
     292,   463,   706,    14,    15,    16,   243,   376,    19,   626,
     376,   534,   190,   506,   326,   144,   709,   380,   670,   710,
    1120,    63,   420,   243,   242,   711,   712,    18,   713,   527,
     385,   386,   714,    20,   112,   324,   999,   441,   283,   442,
     385,   386,   798,   592,   593,    98,   595,   827,   671,   599,
     573,    17,   523,   707,   459,   575,   576,   584,   288,   389,
     608,   524,   225,   226,   531,   532,   577,   578,   579,    20,
    1128,   407,   227,    63,   228,    99,   408,   390,   391,   392,
     393,   103,   612,   385,   386,   395,   610,   104,   385,   386,
     229,   230,   231,   385,   386,    64,   232,   678,   642,    65,
     163,   106,   303,   164,   435,   293,   439,    66,  1129,    67,
      69,   382,   385,   386,   383,   385,   386,  1077,   304,   305,
    1080,   642,    70,   233,   234,   235,   420,   464,   396,   397,
     398,   399,   400,   401,   402,   403,    71,   997,    72,   606,
     109,    18,   454,   673,    73,   236,   115,    64,   385,   386,
     116,    65,   643,   676,   644,   645,   917,   787,   404,    66,
     675,    67,    69,   668,   409,   669,   389,   672,   237,   410,
     238,   580,   239,   122,    70,   643,   411,   644,   645,    57,
      58,   412,   125,    20,    59,   686,  -497,  -497,    71,   825,
      72,   826,   240,   241,   242,   450,    73,   243,   451,   244,
     460,  1006,   100,   101,   102,   683,   221,   222,   223,   224,
     153,   154,  1102,   173,   174,  1104,   717,   139,   175,   221,
     222,   223,   224,   133,   468,   149,   469,  1116,   470,  1118,
     570,   804,   140,   176,   145,   586,  -497,  -497,   399,   400,
     401,   402,   403,   177,   146,   799,   178,   179,   413,   180,
     181,   182,   812,   414,   795,   147,   719,   720,   721,   722,
     723,   609,   150,   724,   725,   183,   184,   160,   161,   162,
     726,   727,   728,   152,   295,   225,   226,   296,   297,   155,
     156,   568,   298,   299,   569,   227,   729,   228,   225,   226,
     356,   585,   157,    17,   383,    42,    43,    44,   227,   158,
     228,    45,    46,   229,   230,   231,    48,    49,    50,   232,
     170,  1081,    51,    52,  1082,  1083,   229,   230,   231,  1084,
    1085,   590,   232,   591,   366,   470,   367,  1105,   368,   369,
     427,   428,   221,   222,   223,   224,   233,   234,   235,  1117,
     237,  1121,   238,   159,   239,   820,   821,   822,   823,   233,
     234,   235,   171,  1130,   135,   136,   137,   138,   236,   698,
    -239,   699,   700,   701,   702,  1138,   703,   704,  1140,   515,
     516,   236,   221,   222,   223,   224,   127,   128,   129,   130,
     682,   237,   172,   238,   602,   239,   655,   603,   662,   383,
     664,   663,   665,   663,   237,   383,   238,   685,   239,   185,
     383,   225,   226,   385,   386,   240,   241,   242,   186,   696,
     243,   227,   244,   228,   689,   604,   605,   451,   240,   241,
     242,   690,   187,   243,   691,   244,   401,   402,   403,   229,
     230,   231,   189,   693,   792,   232,   694,   451,   374,   910,
     816,   351,   352,   383,   193,   835,   906,   909,   836,   451,
     383,   227,   996,   228,   195,   694,   221,   222,   223,   224,
    1107,  1108,   233,   234,   235,  1003,  1113,  1114,   663,   229,
     230,   231,   197,  1087,   198,   232,   451,   202,   730,   731,
     732,   733,   734,   203,   236,   735,   736,   802,   920,   921,
     809,   204,   737,   738,   739,   207,   211,   206,   212,  1008,
     213,   216,   233,   234,   235,   218,   294,   237,   740,   238,
     300,   239,   301,   310,   302,   307,   312,   311,   313,   314,
     315,   319,  1007,   320,   236,   351,   327,    74,   322,   331,
     332,   240,   241,   242,   336,   227,   243,   228,   244,   337,
     359,   338,   463,   354,   355,   360,   361,   237,   362,   238,
     365,   239,   363,   229,   230,   231,   370,    75,    76,   232,
      77,   371,   404,   415,   416,    78,    79,   417,   418,   423,
     419,   240,   241,   242,   426,   420,   243,   429,   244,  1086,
     430,   447,   437,   448,   452,    17,   233,   234,   235,     1,
     457,     2,     3,     4,     5,     6,     7,     8,     9,    10,
     389,   458,   462,   471,   476,    11,    12,    13,   236,   477,
     472,    14,    15,    16,   473,   474,   475,   478,   390,   391,
     392,   393,   479,   483,   481,   488,   395,   243,   508,   510,
     512,   237,   513,   238,   514,   239,   741,   742,   743,   744,
     745,   517,   519,   746,   747,   387,   518,   388,   520,   522,
     748,   749,   750,   521,   525,   240,   241,   242,   530,    17,
     243,   528,   244,   526,   566,   571,   751,   567,   463,   396,
     397,   398,   399,   400,   401,   402,   403,   572,   574,   594,
     800,   487,   587,    80,    81,    82,    83,   596,    84,    85,
     600,   385,    86,    87,    88,   607,   611,    89,    90,    91,
     615,   613,   614,   389,    92,    93,   616,   440,   620,   623,
     624,   625,   383,   389,   635,   636,    94,   632,   651,   657,
      95,   390,   391,   392,   393,   394,   389,   637,   638,   395,
     658,  -497,  -497,   392,   393,   639,   659,   640,   641,  -497,
     649,   660,   654,   656,   390,   391,   392,   393,   661,    18,
     680,   605,   395,   339,   340,   341,   342,   343,   344,   345,
     346,   347,   348,   349,   350,   666,    19,   674,   681,   679,
     604,   684,   396,   397,   398,   399,   400,   401,   402,   403,
     687,   688,  -497,   397,   398,   399,   400,   401,   402,   403,
     695,    20,   692,   697,   718,   396,   397,   398,   399,   400,
     401,   402,   403,   535,   536,   537,   538,   539,   540,   541,
     542,   543,   544,   545,   546,   547,   548,   549,   550,   551,
     786,   552,   553,   554,   555,   556,   557,   785,   787,   558,
     793,   794,   559,   560,   803,   797,   561,   562,   563,   564,
     752,   753,   754,   755,   756,   801,   810,   757,   758,   805,
     813,   815,   817,   814,   759,   760,   761,   763,   764,   765,
     766,   767,   818,   829,   768,   769,   830,   831,   832,   833,
     762,   770,   771,   772,   774,   775,   776,   777,   778,   837,
     838,   779,   780,   839,   840,   841,   842,   773,   781,   782,
     783,   843,   844,   845,   846,   847,   857,   848,   849,   868,
     850,   851,   852,   853,   784,   854,   879,   890,   855,   856,
     901,   904,   858,   905,   859,   860,   908,   678,   918,   924,
     927,   861,   862,   863,   864,   865,   925,   866,   928,   867,
     869,   870,   871,   872,   873,   874,   875,   929,   876,   877,
     878,   880,   881,   930,   882,   931,   883,   932,   884,   885,
     886,   933,   887,   888,   889,   934,   891,   892,   935,   936,
     893,   894,   895,   896,   938,   897,   898,   899,   900,   902,
     911,   912,   914,   915,   916,   922,   937,   939,   940,   941,
     942,   943,   944,   945,   946,   947,   948,   949,   950,   951,
     952,   953,   954,   955,   956,   957,   958,   960,   961,   959,
     962,   963,   964,   965,   966,   967,   968,   969,   970,   971,
     972,   973,   974,   975,   976,   977,   978,   979,   980,   981,
     982,   983,   984,   985,   986,   987,   988,   989,   990,   991,
     992,   993,   994,   995,   998,  1000,  1001,  1002,  1009,  1010,
    1004,  1005,  1011,  1012,  1013,  1014,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1023,  1024,  1030,  1025,  1026,  1027,
    1028,  1029,  1041,  1031,  1032,  1033,  1052,  1034,  1035,  1036,
    1037,  1038,  1039,  1040,  1063,  1042,  1043,  1074,  1044,  1045,
    1046,  1047,  1048,  1094,  1049,  1050,  1051,  1053,  1054,  1055,
    1056,  1057,  1058,  1059,  1060,  1061,  1062,  1064,  1065,  1066,
    1067,  1068,  1069,  1070,  1071,  1072,  1073,  1075,  1096,  1099,
    1111,  1076,  1078,  1079,  1088,  1089,  1090,  1091,  1092,  1093,
    1095,  1098,  1100,  1106,  1115,  1122,  1123,  1110,  1101,  1103,
    1112,  1124,  1126,  1125,  1127,  1131,  1132,  1133,  1134,  1135,
    1137,   148,  1136,  1139,   653,   667,   456,   188,   788,   325,
     113,   790,   316,   919,   482,   923,   634,   828,   467,   903,
     807,   808,   364,     0,   308,     0,   907,   796,     0,   824,
     509,     0,     0,     0,     0,     0,     0,   511,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   422,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   598
};

static const yytype_int16 yycheck[] =
{
     216,     8,   194,    20,   565,   677,   517,   214,   225,   243,
     458,   143,   228,     3,     5,     6,   518,     5,     6,    65,
      74,     8,     3,   240,   241,     4,   632,     6,   244,   632,
       4,     5,     6,     3,     0,   632,   632,    16,   632,    77,
     172,     7,   632,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    61,    60,   121,    81,   567,    23,    24,    25,
       8,     4,    86,    29,    30,    31,     6,    20,    47,    22,
      14,    75,    16,     3,     6,    28,    55,     7,    72,     9,
      10,    11,    12,    13,    14,     3,    16,     5,     6,     3,
      54,    75,    76,    23,    24,    25,    61,    76,    87,    29,
      30,    31,   156,   319,   320,     3,    71,     5,     6,   153,
     154,    77,    60,    20,    38,    22,   332,    96,    97,    98,
      99,    28,    65,   190,     3,    65,   100,   101,   102,   153,
     154,   803,   106,    65,   351,   129,    40,   208,   649,   355,
     356,    54,   213,    61,    77,    49,   338,    77,   127,   128,
     129,    54,     3,    77,     5,     6,   135,   136,   137,   133,
     134,   135,    37,    61,   153,   154,   210,   213,    34,   385,
     386,   209,   406,   390,   391,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   402,   403,   213,   205,   691,
     169,     7,   125,     9,    10,    11,    12,    13,    14,    34,
      16,   167,   211,     3,     4,     5,     6,    23,    24,    25,
      61,    72,     3,    29,    30,    31,   207,   208,   184,   209,
     208,   437,   209,   204,   203,   209,   832,   244,    83,   832,
     163,     8,    77,   207,   204,   832,   832,   167,   832,   431,
     153,   154,   832,   209,   188,   209,     4,   126,   166,   128,
     153,   154,    83,   469,   470,    34,   472,   210,    86,   475,
     452,    77,    33,    54,    64,    67,    68,   459,   166,   130,
     487,    42,    72,    73,    69,    70,    78,    79,    80,   209,
     125,   208,    82,    60,    84,    34,   213,   148,   149,   150,
     151,     3,   153,   153,   154,   156,   488,     3,   153,   154,
     100,   101,   102,   153,   154,     8,   106,    65,    72,     8,
     194,     3,    10,   197,   530,   166,   323,     8,   163,     8,
       8,   210,   153,   154,   213,   153,   154,   999,    26,    27,
    1002,    72,     8,   133,   134,   135,    77,   354,   199,   200,
     201,   202,   203,   204,   205,   206,     8,   908,     8,   481,
     170,   167,   331,   213,     8,   155,     0,    60,   153,   154,
     212,    60,   126,   213,   128,   129,   814,   131,   211,    60,
     213,    60,    60,   589,   208,   591,   130,   594,   178,   213,
     180,   183,   182,     8,    60,   126,   208,   128,   129,   164,
     165,   213,    71,   209,   169,   612,   150,   151,    60,    34,
      60,    36,   202,   203,   204,   210,    60,   207,   213,   209,
     210,   922,   157,   158,   159,   607,     3,     4,     5,     6,
     171,   172,  1094,   184,   185,  1097,   633,   211,   189,     3,
       4,     5,     6,    71,    83,   209,    85,  1109,    87,  1111,
     447,   675,    14,   158,     3,   462,   200,   201,   202,   203,
     204,   205,   206,   168,     3,   671,   171,   172,   208,   174,
     175,   176,   678,   213,   656,     3,    89,    90,    91,    92,
      93,   488,     3,    96,    97,   190,   191,   193,   194,   195,
     103,   104,   105,     6,    40,    72,    73,    43,    44,   171,
     172,   210,    48,    49,   213,    82,   119,    84,    72,    73,
      87,   210,     3,    77,   213,    33,    34,    35,    82,   174,
      84,    39,    40,   100,   101,   102,    33,    34,    35,   106,
      54,    40,    39,    40,    43,    44,   100,   101,   102,    48,
      49,    83,   106,    85,   118,    87,   120,  1098,   122,   123,
     186,   187,     3,     4,     5,     6,   133,   134,   135,  1110,
     178,  1112,   180,     6,   182,     3,     4,     5,     6,   133,
     134,   135,   213,  1124,    49,    50,    51,    52,   155,    55,
      56,    57,    58,    59,    60,  1136,    62,    63,  1139,    45,
      46,   155,     3,     4,     5,     6,    43,    44,    45,    46,
     607,   178,     6,   180,   210,   182,   210,   213,   210,   213,
     210,   213,   210,   213,   178,   213,   180,   210,   182,     4,
     213,    72,    73,   153,   154,   202,   203,   204,   210,   626,
     207,    82,   209,    84,   210,     5,     6,   213,   202,   203,
     204,   210,   210,   207,   213,   209,   204,   205,   206,   100,
     101,   102,    21,   210,   210,   106,   213,   213,     5,     6,
     210,    72,    73,   213,    64,   210,   210,   210,   213,   213,
     213,    82,   210,    84,    66,   213,     3,     4,     5,     6,
      45,    46,   133,   134,   135,   210,    43,    44,   213,   100,
     101,   102,    72,   210,     3,   106,   213,     3,    89,    90,
      91,    92,    93,    61,   155,    96,    97,   673,   829,   830,
     676,   209,   103,   104,   105,     3,     3,    73,     3,   925,
       3,    65,   133,   134,   135,     4,   210,   178,   119,   180,
       3,   182,     3,   163,     4,   209,     3,     6,     6,     4,
      54,    52,   924,    67,   155,    72,     3,     3,    73,    61,
     199,   202,   203,   204,    77,    82,   207,    84,   209,    77,
       4,   209,    72,   209,   209,     4,     4,   178,     4,   180,
     209,   182,     6,   100,   101,   102,   209,    33,    34,   106,
      36,   209,   211,     3,     6,    41,    42,    47,    47,     4,
      76,   202,   203,   204,     6,    77,   207,   210,   209,  1005,
     210,    54,    68,   209,   209,    77,   133,   134,   135,     7,
     209,     9,    10,    11,    12,    13,    14,    15,    16,    17,
     130,   209,   209,     4,     4,    23,    24,    25,   155,     4,
     209,    29,    30,    31,   209,   209,   209,   214,   148,   149,
     150,   151,   210,     3,    76,   209,   156,   207,     6,     6,
       6,   178,     5,   180,    43,   182,    89,    90,    91,    92,
      93,   209,     3,    96,    97,    72,   209,    74,   209,     6,
     103,   104,   105,   210,   163,   202,   203,   204,   213,    77,
     207,    75,   209,   163,   127,     3,   119,   209,    72,   199,
     200,   201,   202,   203,   204,   205,   206,     3,   213,    75,
     210,   156,   210,   159,   160,   161,   162,     4,   164,   165,
     213,   153,   168,   169,   170,   209,   129,   173,   174,   175,
       6,   214,   214,   130,   180,   181,     6,     3,     3,     6,
       4,     4,   213,   130,   209,   209,   192,   177,    32,     6,
     196,   148,   149,   150,   151,   152,   130,   209,   209,   156,
       6,   148,   149,   150,   151,   209,     4,   209,   209,   156,
     209,     3,   210,   209,   148,   149,   150,   151,     6,   167,
       4,     6,   156,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   210,   184,   213,    77,   210,
       5,   210,   199,   200,   201,   202,   203,   204,   205,   206,
      50,    47,   199,   200,   201,   202,   203,   204,   205,   206,
       6,   209,   199,   213,     6,   199,   200,   201,   202,   203,
     204,   205,   206,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     127,   106,   107,   108,   109,   110,   111,   129,   131,   114,
     209,   163,   117,   118,     4,   210,   121,   122,   123,   124,
      89,    90,    91,    92,    93,   207,   213,    96,    97,   207,
     210,   210,     6,   209,   103,   104,   105,    89,    90,    91,
      92,    93,     6,    56,    96,    97,    56,     3,   213,    51,
     119,   103,   104,   105,    89,    90,    91,    92,    93,   213,
     213,    96,    97,   213,   213,   213,   213,   119,   103,   104,
     105,   213,   213,   213,    91,   213,    91,   213,   213,    91,
     213,   213,   213,   213,   119,   213,    91,    91,   213,   213,
      91,   132,   213,   132,   213,   213,     3,    65,     6,    52,
       6,   213,   213,   213,   213,   213,    53,   213,     6,   213,
     213,   213,   213,   213,   213,   213,   213,     6,   213,   213,
     213,   213,   213,     6,   213,     6,   213,     6,   213,   213,
     213,     6,   213,   213,   213,     6,   213,   213,     6,     6,
     213,   213,   213,   213,     6,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   210,   209,   213,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   213,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   213,
       6,     6,     6,     6,     6,     6,     6,     6,   213,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   213,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     213,     6,     4,     4,     4,   210,     4,     4,   210,   210,
       6,    61,   210,   210,   210,   210,   210,   210,   210,   210,
       6,   210,   210,   210,   210,   210,     6,   210,   210,   210,
     210,   210,     6,   210,   210,   210,     6,   210,   210,   210,
     210,   210,   210,   210,     6,   210,   210,     6,   210,   210,
     210,   210,   210,     4,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,     4,     6,
       4,   213,   213,   213,   210,   210,   210,   210,   210,   210,
     210,   210,     6,     6,     6,     6,    48,   210,   213,   213,
     210,   210,    40,   210,    40,   209,    40,    40,     3,   209,
       3,    60,   210,   210,   569,   588,   333,   116,   646,   201,
      20,   648,   190,   827,   383,   832,   530,   697,   357,   787,
     676,   676,   235,    -1,   171,    -1,   793,   663,    -1,   694,
     408,    -1,    -1,    -1,    -1,    -1,    -1,   410,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   307,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   474
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
      67,   250,    73,    54,   209,   233,   264,     3,   230,    38,
     242,    61,   199,   213,   257,   281,    77,    77,   209,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    72,    73,   282,   209,   209,    87,   281,   296,     4,
       4,     4,     4,     6,   318,   209,   118,   120,   122,   123,
     209,   209,   282,   282,     5,     6,   208,   301,   311,   312,
     242,   281,   210,   213,    54,   153,   154,    72,    74,   130,
     148,   149,   150,   151,   152,   156,   199,   200,   201,   202,
     203,   204,   205,   206,   211,   208,   213,   208,   213,   208,
     213,   208,   213,   208,   213,     3,     6,    47,    47,    76,
      77,   326,   243,     4,    40,    49,     6,   186,   187,   210,
     210,    81,   253,   247,   248,   281,   281,    68,   251,   240,
       3,   126,   128,   221,   223,   224,   229,    54,   209,   330,
     210,   213,   209,   279,   264,   281,   237,   209,   209,    64,
     210,   278,   209,    72,   242,   281,   281,   296,    83,    85,
      87,     4,   209,   209,   209,   209,     4,     4,   214,   210,
     210,    76,   280,     3,   281,   281,    74,   156,   209,    72,
     129,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   282,   282,   282,   282,     3,   204,   301,     6,   311,
       6,   312,     6,     5,    43,    45,    46,   209,   209,     3,
     209,   210,     6,    33,    42,   163,   163,   278,    75,   254,
     213,    69,    70,   249,   281,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   106,   107,   108,   109,   110,   111,   114,   117,
     118,   121,   122,   123,   124,   225,   127,   209,   210,   213,
     240,     3,     3,   278,   213,    67,    68,    78,    79,    80,
     183,   319,   320,   319,   278,   210,   242,   210,    54,    86,
      83,    85,   281,   281,    75,   281,     4,     3,   299,   281,
     213,   256,   210,   213,     5,     6,   321,   209,   282,   242,
     278,   129,   153,   214,   214,     6,     6,   230,   222,   224,
       3,   328,   329,     6,     4,     4,   209,   261,   262,   263,
     264,   269,   177,   255,   248,   209,   209,   209,   209,   209,
     209,   209,    72,   126,   128,   129,   226,   227,   326,   209,
     230,    32,   327,   223,   210,   210,   209,     6,     6,     4,
       3,     6,   210,   213,   210,   210,   210,   225,   281,   281,
      83,    86,   282,   213,   213,   213,   213,     4,    65,   210,
       4,    77,   242,   278,   210,   210,   282,    50,    47,   210,
     210,   213,   199,   210,   213,     6,   240,   213,    55,    57,
      58,    59,    60,    62,    63,   270,     3,    54,   265,   283,
     284,   285,   286,   287,   288,   289,   290,   257,     6,    89,
      90,    91,    92,    93,    96,    97,   103,   104,   105,   119,
      89,    90,    91,    92,    93,    96,    97,   103,   104,   105,
     119,    89,    90,    91,    92,    93,    96,    97,   103,   104,
     105,   119,    89,    90,    91,    92,    93,    96,    97,   103,
     104,   105,   119,    89,    90,    91,    92,    93,    96,    97,
     103,   104,   105,   119,    89,    90,    91,    92,    93,    96,
      97,   103,   104,   105,   119,   129,   127,   131,   227,   228,
     228,   230,   210,   209,   163,   278,   320,   210,    83,   281,
     210,   207,   313,     4,   301,   207,   302,   305,   310,   313,
     213,   256,   281,   210,   209,   210,   210,     6,     6,   224,
       3,     4,     5,     6,   329,    34,    36,   210,   262,    56,
      56,     3,   213,    51,   259,   210,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,    91,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,    91,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,    91,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,    91,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
      91,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,    91,   213,   300,   132,   132,   210,   328,     3,   210,
       6,   213,   213,   256,   213,   213,   210,   319,     6,   265,
     263,   263,   209,   289,    52,    53,   258,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   213,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   213,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   213,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     213,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   213,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   213,     6,     4,     4,   210,   326,     4,     4,
     210,     4,     4,   210,     6,    61,   230,   278,   281,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,     6,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
       6,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,     6,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,     6,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   210,     6,   210,   210,   210,   210,   210,   210,
     210,   210,   210,   210,     6,   210,   213,   256,   213,   213,
     256,    40,    43,    44,    48,    49,   281,   210,   210,   210,
     210,   210,   210,   210,     4,   210,     4,     6,   210,     6,
       6,   213,   256,   213,   256,   326,     6,    45,    46,     6,
     210,     4,   210,    43,    44,     6,   256,   326,   256,   125,
     163,   326,     6,    48,   210,   210,    40,    40,   125,   163,
     326,   209,    40,    40,     3,   209,   210,     3,   326,   210,
     326
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   215,   216,   217,   217,   218,   218,   218,   218,   218,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   220,   220,   220,   220,   220,   220,   221,   221,
     222,   222,   223,   223,   224,   224,   224,   224,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   225,   225,   225,   225,   225,   225,   225,   225,   225,
     226,   226,   227,   227,   227,   227,   228,   228,   229,   229,
     230,   230,   231,   232,   232,   233,   233,   234,   234,   235,
     236,   236,   237,   238,   238,   238,   238,   238,   239,   239,
     239,   240,   240,   240,   240,   241,   241,   242,   243,   244,
     244,   245,   246,   246,   247,   247,   248,   249,   249,   249,
     250,   250,   251,   251,   252,   252,   253,   253,   254,   254,
     255,   255,   256,   256,   257,   257,   258,   258,   259,   259,
     260,   260,   260,   260,   261,   261,   262,   262,   263,   263,
     264,   264,   265,   265,   265,   265,   266,   266,   267,   267,
     268,   269,   269,   270,   270,   270,   270,   270,   270,   270,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   271,   271,
     271,   271,   271,   271,   271,   271,   271,   271,   272,   272,
     272,   273,   273,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   275,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   277,   277,   277,   278,   278,   279,
     279,   280,   280,   281,   281,   281,   281,   281,   282,   282,
     282,   282,   282,   282,   282,   282,   282,   282,   282,   282,
     282,   283,   284,   284,   284,   284,   285,   285,   285,   285,
     286,   286,   287,   287,   288,   288,   289,   289,   289,   289,
     289,   289,   290,   290,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   291,   291,   291,
     291,   291,   291,   291,   291,   291,   291,   292,   292,   293,
     294,   294,   295,   295,   295,   295,   296,   296,   297,   298,
     298,   298,   298,   299,   299,   299,   299,   300,   300,   300,
     300,   300,   300,   300,   300,   300,   300,   300,   300,   301,
     301,   301,   301,   302,   302,   302,   303,   304,   304,   305,
     305,   306,   307,   307,   308,   309,   309,   310,   311,   312,
     313,   313,   314,   315,   315,   316,   317,   317,   318,   318,
     318,   318,   318,   318,   318,   318,   318,   318,   318,   318,
     319,   319,   320,   320,   320,   320,   320,   320,   321,   322,
     322,   323,   323,   324,   324,   325,   325,   326,   326,   327,
     327,   328,   328,   329,   329,   329,   329,   329,   330,   330
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     8,     6,     7,     6,     1,     3,
       1,     3,     1,     1,     4,     4,     6,     6,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     6,     4,     1,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     7,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     7,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     7,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     7,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       1,     2,     2,     1,     1,     2,     2,     0,     5,     4,
       1,     3,     4,     6,     5,     3,     0,     3,     2,     5,
       1,     3,     3,     4,     4,     4,     4,     6,     8,    11,
       8,     1,     1,     3,     3,     3,     3,     2,     4,     3,
       3,     9,     3,     0,     1,     3,     2,     1,     1,     0,
       2,     0,     2,     0,     1,     0,     2,     0,     2,     0,
       2,     0,     3,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     3,     2,     2,     2,     2,     3,
       2,     3,     3,     3,     4,     4,     3,     3,     4,     4,
       5,     6,     7,     9,     4,     5,     7,     9,     2,     3,
       2,     3,     3,     4,     2,     3,     3,     4,     2,     2,
       2,     2,     5,     2,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       3,     3,     3,     3,     4,     6,     7,     9,    10,    12,
      12,    13,    14,    15,    16,    12,    13,    15,    16,     3,
       4,     5,     6,     3,     3,     4,     3,     4,     3,     3,
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
#line 2390 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2398 "parser.cpp"
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
#line 2412 "parser.cpp"
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
#line 2426 "parser.cpp"
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
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2451 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2459 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2468 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2477 "parser.cpp"
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
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2502 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2512 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2522 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2532 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2542 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2552 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2562 "parser.cpp"
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
#line 2576 "parser.cpp"
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
#line 2590 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2608 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2616 "parser.cpp"
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
#line 2630 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2639 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2647 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2655 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2663 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2671 "parser.cpp"
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
#line 2685 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2694 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2703 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2712 "parser.cpp"
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
#line 2725 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2734 "parser.cpp"
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
#line 2748 "parser.cpp"
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
#line 2762 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2772 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2781 "parser.cpp"
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
#line 2795 "parser.cpp"
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
#line 2812 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2860 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2868 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2876 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2884 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2892 "parser.cpp"
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
#line 2906 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2914 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2922 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2930 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2938 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2946 "parser.cpp"
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
#line 2959 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3007 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3015 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3023 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3055 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3063 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3071 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3079 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3087 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3095 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3103 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3111 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3119 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3127 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3135 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3143 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3154 "parser.cpp"
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
#line 3168 "parser.cpp"
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
#line 3182 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3193 "parser.cpp"
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

#line 3301 "parser.cpp"

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
#line 3516 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 510 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3527 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 516 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3538 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 523 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3544 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3550 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3556 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3562 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3568 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3574 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3580 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3586 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3592 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 532 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3598 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 533 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3604 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3610 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3616 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3622 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3628 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 539 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3634 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3640 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3646 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3652 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3658 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3664 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3670 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3676 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3682 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3688 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3694 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3700 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 557 "parser.y"
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
#line 3720 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 574 "parser.y"
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
#line 3738 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 590 "parser.y"
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
#line 3771 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 619 "parser.y"
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
#line 3791 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 635 "parser.y"
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
#line 3812 "parser.cpp"
    break;

  case 37: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info  */
#line 653 "parser.y"
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
#line 3845 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 682 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3854 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 686 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3863 "parser.cpp"
    break;

  case 40: /* column_def_array: table_column  */
#line 691 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3872 "parser.cpp"
    break;

  case 41: /* column_def_array: column_def_array ',' table_column  */
#line 695 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3881 "parser.cpp"
    break;

  case 42: /* table_element: table_column  */
#line 701 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3889 "parser.cpp"
    break;

  case 43: /* table_element: table_constraint  */
#line 704 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3897 "parser.cpp"
    break;

  case 44: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
#line 711 "parser.y"
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
#line 3953 "parser.cpp"
    break;

  case 45: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 762 "parser.y"
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
#line 3995 "parser.cpp"
    break;

  case 46: /* table_column: IDENTIFIER column_type with_index_param_list default_expr COMMENT STRING  */
#line 799 "parser.y"
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
#line 4052 "parser.cpp"
    break;

  case 47: /* table_column: IDENTIFIER column_type column_constraints default_expr COMMENT STRING  */
#line 851 "parser.y"
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
#line 4095 "parser.cpp"
    break;

  case 48: /* column_type: BOOLEAN  */
#line 891 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4101 "parser.cpp"
    break;

  case 49: /* column_type: TINYINT  */
#line 892 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4107 "parser.cpp"
    break;

  case 50: /* column_type: SMALLINT  */
#line 893 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4113 "parser.cpp"
    break;

  case 51: /* column_type: INTEGER  */
#line 894 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4119 "parser.cpp"
    break;

  case 52: /* column_type: INT  */
#line 895 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4125 "parser.cpp"
    break;

  case 53: /* column_type: BIGINT  */
#line 896 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4131 "parser.cpp"
    break;

  case 54: /* column_type: HUGEINT  */
#line 897 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4137 "parser.cpp"
    break;

  case 55: /* column_type: FLOAT  */
#line 898 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4143 "parser.cpp"
    break;

  case 56: /* column_type: REAL  */
#line 899 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4149 "parser.cpp"
    break;

  case 57: /* column_type: DOUBLE  */
#line 900 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4155 "parser.cpp"
    break;

  case 58: /* column_type: FLOAT16  */
#line 901 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4161 "parser.cpp"
    break;

  case 59: /* column_type: BFLOAT16  */
#line 902 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4167 "parser.cpp"
    break;

  case 60: /* column_type: DATE  */
#line 903 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4173 "parser.cpp"
    break;

  case 61: /* column_type: TIME  */
#line 904 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4179 "parser.cpp"
    break;

  case 62: /* column_type: DATETIME  */
#line 905 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4185 "parser.cpp"
    break;

  case 63: /* column_type: TIMESTAMP  */
#line 906 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4191 "parser.cpp"
    break;

  case 64: /* column_type: UUID  */
#line 907 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4197 "parser.cpp"
    break;

  case 65: /* column_type: POINT  */
#line 908 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4203 "parser.cpp"
    break;

  case 66: /* column_type: LINE  */
#line 909 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4209 "parser.cpp"
    break;

  case 67: /* column_type: LSEG  */
#line 910 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4215 "parser.cpp"
    break;

  case 68: /* column_type: BOX  */
#line 911 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4221 "parser.cpp"
    break;

  case 69: /* column_type: CIRCLE  */
#line 914 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4227 "parser.cpp"
    break;

  case 70: /* column_type: VARCHAR  */
#line 916 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4233 "parser.cpp"
    break;

  case 71: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 917 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4239 "parser.cpp"
    break;

  case 72: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 918 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4245 "parser.cpp"
    break;

  case 73: /* column_type: DECIMAL  */
#line 919 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4251 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 922 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4257 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 923 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4263 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 924 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4269 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 925 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4275 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 926 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4281 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 927 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4287 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 928 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4293 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 929 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4299 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 930 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4305 "parser.cpp"
    break;

  case 83: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 931 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4311 "parser.cpp"
    break;

  case 84: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 932 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4317 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 933 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4323 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 934 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4329 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 935 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4335 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 936 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4341 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 937 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4347 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 938 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4353 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 939 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4359 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 940 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4365 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 941 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4371 "parser.cpp"
    break;

  case 94: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 942 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4377 "parser.cpp"
    break;

  case 95: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 943 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4383 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 944 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4389 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 945 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4395 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 946 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4401 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 947 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4407 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 948 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4413 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 949 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4419 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 950 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4425 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 951 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4431 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 952 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4437 "parser.cpp"
    break;

  case 105: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 953 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4443 "parser.cpp"
    break;

  case 106: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 954 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4449 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 955 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4455 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 956 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4461 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 957 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4467 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 958 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4473 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 959 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4479 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 960 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4485 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 961 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4491 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 962 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4497 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 963 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4503 "parser.cpp"
    break;

  case 116: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 964 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4509 "parser.cpp"
    break;

  case 117: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 965 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4515 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 966 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4521 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 967 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4527 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 968 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4533 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 969 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4539 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 970 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4545 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 971 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4551 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 972 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4557 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 973 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4563 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 974 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4569 "parser.cpp"
    break;

  case 127: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 975 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4575 "parser.cpp"
    break;

  case 128: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 976 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4581 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 977 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4587 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 978 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4593 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 979 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4599 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 980 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4605 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 981 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4611 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 982 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4617 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 983 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4623 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 984 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4629 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 985 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4635 "parser.cpp"
    break;

  case 138: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 986 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4641 "parser.cpp"
    break;

  case 139: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 987 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4647 "parser.cpp"
    break;

  case 140: /* column_constraints: column_constraint  */
#line 1006 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4656 "parser.cpp"
    break;

  case 141: /* column_constraints: column_constraints column_constraint  */
#line 1010 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4670 "parser.cpp"
    break;

  case 142: /* column_constraint: PRIMARY KEY  */
#line 1020 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4678 "parser.cpp"
    break;

  case 143: /* column_constraint: UNIQUE  */
#line 1023 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4686 "parser.cpp"
    break;

  case 144: /* column_constraint: NULLABLE  */
#line 1026 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4694 "parser.cpp"
    break;

  case 145: /* column_constraint: NOT NULLABLE  */
#line 1029 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4702 "parser.cpp"
    break;

  case 146: /* default_expr: DEFAULT constant_expr  */
#line 1033 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4710 "parser.cpp"
    break;

  case 147: /* default_expr: %empty  */
#line 1036 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4718 "parser.cpp"
    break;

  case 148: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 1041 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4728 "parser.cpp"
    break;

  case 149: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 1046 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4738 "parser.cpp"
    break;

  case 150: /* identifier_array: IDENTIFIER  */
#line 1053 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4749 "parser.cpp"
    break;

  case 151: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 1059 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4760 "parser.cpp"
    break;

  case 152: /* delete_statement: DELETE FROM table_name where_clause  */
#line 1069 "parser.y"
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
#line 4777 "parser.cpp"
    break;

  case 153: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 1085 "parser.y"
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
#line 4816 "parser.cpp"
    break;

  case 154: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1119 "parser.y"
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
#line 4833 "parser.cpp"
    break;

  case 155: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1132 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4841 "parser.cpp"
    break;

  case 156: /* optional_identifier_array: %empty  */
#line 1135 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4849 "parser.cpp"
    break;

  case 157: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1142 "parser.y"
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
#line 4867 "parser.cpp"
    break;

  case 158: /* explain_statement: EXPLAIN explainable_statement  */
#line 1154 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4877 "parser.cpp"
    break;

  case 159: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1163 "parser.y"
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
#line 4894 "parser.cpp"
    break;

  case 160: /* update_expr_array: update_expr  */
#line 1176 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4903 "parser.cpp"
    break;

  case 161: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1180 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4912 "parser.cpp"
    break;

  case 162: /* update_expr: IDENTIFIER '=' expr  */
#line 1185 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4924 "parser.cpp"
    break;

  case 163: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1198 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4940 "parser.cpp"
    break;

  case 164: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1211 "parser.y"
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
#line 4958 "parser.cpp"
    break;

  case 165: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1226 "parser.y"
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
#line 4976 "parser.cpp"
    break;

  case 166: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1241 "parser.y"
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
#line 4994 "parser.cpp"
    break;

  case 167: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1256 "parser.y"
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
#line 5017 "parser.cpp"
    break;

  case 168: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1279 "parser.y"
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
#line 5075 "parser.cpp"
    break;

  case 169: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1332 "parser.y"
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
#line 5135 "parser.cpp"
    break;

  case 170: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1387 "parser.y"
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
#line 5187 "parser.cpp"
    break;

  case 171: /* select_statement: select_without_paren  */
#line 1438 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5195 "parser.cpp"
    break;

  case 172: /* select_statement: select_with_paren  */
#line 1441 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5203 "parser.cpp"
    break;

  case 173: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1444 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5217 "parser.cpp"
    break;

  case 174: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1453 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5231 "parser.cpp"
    break;

  case 175: /* select_with_paren: '(' select_without_paren ')'  */
#line 1463 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5239 "parser.cpp"
    break;

  case 176: /* select_with_paren: '(' select_with_paren ')'  */
#line 1466 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5247 "parser.cpp"
    break;

  case 177: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1470 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5256 "parser.cpp"
    break;

  case 178: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1475 "parser.y"
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
#line 5287 "parser.cpp"
    break;

  case 179: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1502 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5295 "parser.cpp"
    break;

  case 180: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1505 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5303 "parser.cpp"
    break;

  case 181: /* select_clause_without_modifier: SELECT distinct expr_array highlight_clause from_clause search_clause where_clause group_by_clause having_clause  */
#line 1510 "parser.y"
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
#line 5324 "parser.cpp"
    break;

  case 182: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1527 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5332 "parser.cpp"
    break;

  case 183: /* order_by_clause: %empty  */
#line 1530 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5340 "parser.cpp"
    break;

  case 184: /* order_by_expr_list: order_by_expr  */
#line 1534 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5349 "parser.cpp"
    break;

  case 185: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1538 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5358 "parser.cpp"
    break;

  case 186: /* order_by_expr: expr order_by_type  */
#line 1543 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5368 "parser.cpp"
    break;

  case 187: /* order_by_type: ASC  */
#line 1549 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5376 "parser.cpp"
    break;

  case 188: /* order_by_type: DESC  */
#line 1552 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5384 "parser.cpp"
    break;

  case 189: /* order_by_type: %empty  */
#line 1555 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5392 "parser.cpp"
    break;

  case 190: /* limit_expr: LIMIT expr  */
#line 1559 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5400 "parser.cpp"
    break;

  case 191: /* limit_expr: %empty  */
#line 1563 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5406 "parser.cpp"
    break;

  case 192: /* offset_expr: OFFSET expr  */
#line 1565 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5414 "parser.cpp"
    break;

  case 193: /* offset_expr: %empty  */
#line 1569 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5420 "parser.cpp"
    break;

  case 194: /* distinct: DISTINCT  */
#line 1571 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5428 "parser.cpp"
    break;

  case 195: /* distinct: %empty  */
#line 1574 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5436 "parser.cpp"
    break;

  case 196: /* highlight_clause: HIGHLIGHT expr_array  */
#line 1578 "parser.y"
                                       {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5444 "parser.cpp"
    break;

  case 197: /* highlight_clause: %empty  */
#line 1581 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5452 "parser.cpp"
    break;

  case 198: /* from_clause: FROM table_reference  */
#line 1585 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5460 "parser.cpp"
    break;

  case 199: /* from_clause: %empty  */
#line 1588 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5468 "parser.cpp"
    break;

  case 200: /* search_clause: SEARCH sub_search_array  */
#line 1592 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5478 "parser.cpp"
    break;

  case 201: /* search_clause: %empty  */
#line 1597 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5486 "parser.cpp"
    break;

  case 202: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1601 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5494 "parser.cpp"
    break;

  case 203: /* optional_search_filter_expr: %empty  */
#line 1604 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5502 "parser.cpp"
    break;

  case 204: /* where_clause: WHERE expr  */
#line 1608 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5510 "parser.cpp"
    break;

  case 205: /* where_clause: %empty  */
#line 1611 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5518 "parser.cpp"
    break;

  case 206: /* having_clause: HAVING expr  */
#line 1615 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5526 "parser.cpp"
    break;

  case 207: /* having_clause: %empty  */
#line 1618 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5534 "parser.cpp"
    break;

  case 208: /* group_by_clause: GROUP BY expr_array  */
#line 1622 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5542 "parser.cpp"
    break;

  case 209: /* group_by_clause: %empty  */
#line 1625 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5550 "parser.cpp"
    break;

  case 210: /* set_operator: UNION  */
#line 1629 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5558 "parser.cpp"
    break;

  case 211: /* set_operator: UNION ALL  */
#line 1632 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5566 "parser.cpp"
    break;

  case 212: /* set_operator: INTERSECT  */
#line 1635 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5574 "parser.cpp"
    break;

  case 213: /* set_operator: EXCEPT  */
#line 1638 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5582 "parser.cpp"
    break;

  case 214: /* table_reference: table_reference_unit  */
#line 1646 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5590 "parser.cpp"
    break;

  case 215: /* table_reference: table_reference ',' table_reference_unit  */
#line 1649 "parser.y"
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
#line 5608 "parser.cpp"
    break;

  case 218: /* table_reference_name: table_name table_alias  */
#line 1666 "parser.y"
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
#line 5626 "parser.cpp"
    break;

  case 219: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1680 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5637 "parser.cpp"
    break;

  case 220: /* table_name: IDENTIFIER  */
#line 1689 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5647 "parser.cpp"
    break;

  case 221: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1694 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5659 "parser.cpp"
    break;

  case 222: /* table_alias: AS IDENTIFIER  */
#line 1703 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5669 "parser.cpp"
    break;

  case 223: /* table_alias: IDENTIFIER  */
#line 1708 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5679 "parser.cpp"
    break;

  case 224: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1713 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5690 "parser.cpp"
    break;

  case 225: /* table_alias: %empty  */
#line 1719 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5698 "parser.cpp"
    break;

  case 226: /* with_clause: WITH with_expr_list  */
#line 1726 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5706 "parser.cpp"
    break;

  case 227: /* with_clause: %empty  */
#line 1729 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5714 "parser.cpp"
    break;

  case 228: /* with_expr_list: with_expr  */
#line 1733 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5723 "parser.cpp"
    break;

  case 229: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1736 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5732 "parser.cpp"
    break;

  case 230: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1741 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5744 "parser.cpp"
    break;

  case 231: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1753 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5756 "parser.cpp"
    break;

  case 232: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1760 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5769 "parser.cpp"
    break;

  case 233: /* join_type: INNER  */
#line 1774 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5777 "parser.cpp"
    break;

  case 234: /* join_type: LEFT  */
#line 1777 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5785 "parser.cpp"
    break;

  case 235: /* join_type: RIGHT  */
#line 1780 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5793 "parser.cpp"
    break;

  case 236: /* join_type: OUTER  */
#line 1783 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5801 "parser.cpp"
    break;

  case 237: /* join_type: FULL  */
#line 1786 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5809 "parser.cpp"
    break;

  case 238: /* join_type: CROSS  */
#line 1789 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5817 "parser.cpp"
    break;

  case 239: /* join_type: %empty  */
#line 1792 "parser.y"
                {
}
#line 5824 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW DATABASES  */
#line 1798 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5833 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW TABLES  */
#line 1802 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5842 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW VIEWS  */
#line 1806 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5851 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW CONFIGS  */
#line 1810 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5860 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1814 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5872 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW PROFILES  */
#line 1821 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5881 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW BUFFER  */
#line 1825 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5890 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW MEMINDEX  */
#line 1829 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5899 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW QUERIES  */
#line 1833 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5908 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW QUERY LONG_VALUE  */
#line 1837 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5918 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW TRANSACTIONS  */
#line 1842 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5927 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1846 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5937 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW SESSION VARIABLES  */
#line 1851 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5946 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1855 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5955 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1859 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5966 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1865 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5977 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1871 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5988 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name  */
#line 1877 "parser.y"
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
#line 6004 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1888 "parser.y"
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
#line 6020 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1899 "parser.y"
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
#line 6036 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1910 "parser.y"
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
#line 6053 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1922 "parser.y"
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
#line 6070 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1934 "parser.y"
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
#line 6088 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1947 "parser.y"
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
#line 6107 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1961 "parser.y"
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
#line 6123 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1972 "parser.y"
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
#line 6142 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1986 "parser.y"
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
#line 6163 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 2002 "parser.y"
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
#line 6185 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW LOGS  */
#line 2019 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6194 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW DELTA LOGS  */
#line 2023 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6203 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW CATALOGS  */
#line 2027 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6212 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE FILES  */
#line 2031 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6221 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 2035 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6230 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 2039 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6241 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY  */
#line 2045 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6250 "parser.cpp"
    break;

  case 275: /* show_statement: SHOW MEMORY OBJECTS  */
#line 2049 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6259 "parser.cpp"
    break;

  case 276: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 2053 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6268 "parser.cpp"
    break;

  case 277: /* show_statement: SHOW IDENTIFIER '(' ')'  */
#line 2057 "parser.y"
                          {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kFunction;
      (yyval.show_stmt)->function_name_ = (yyvsp[-2].str_value);
      free((yyvsp[-2].str_value));
}
#line 6279 "parser.cpp"
    break;

  case 278: /* flush_statement: FLUSH DATA  */
#line 2067 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6288 "parser.cpp"
    break;

  case 279: /* flush_statement: FLUSH LOG  */
#line 2071 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6297 "parser.cpp"
    break;

  case 280: /* flush_statement: FLUSH BUFFER  */
#line 2075 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6306 "parser.cpp"
    break;

  case 281: /* optimize_statement: OPTIMIZE table_name  */
#line 2083 "parser.y"
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
#line 6321 "parser.cpp"
    break;

  case 282: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 2094 "parser.y"
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
#line 6345 "parser.cpp"
    break;

  case 283: /* command_statement: USE IDENTIFIER  */
#line 2117 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6356 "parser.cpp"
    break;

  case 284: /* command_statement: EXPORT PROFILES LONG_VALUE file_path  */
#line 2123 "parser.y"
                                       {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6366 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2128 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6377 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2134 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6388 "parser.cpp"
    break;

  case 287: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2140 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6401 "parser.cpp"
    break;

  case 288: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2148 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6412 "parser.cpp"
    break;

  case 289: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2154 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6423 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2160 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6434 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2166 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6445 "parser.cpp"
    break;

  case 292: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2172 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6458 "parser.cpp"
    break;

  case 293: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2180 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6469 "parser.cpp"
    break;

  case 294: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2186 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6480 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2192 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6491 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2198 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6502 "parser.cpp"
    break;

  case 297: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2204 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6515 "parser.cpp"
    break;

  case 298: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2212 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6526 "parser.cpp"
    break;

  case 299: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2218 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6537 "parser.cpp"
    break;

  case 300: /* command_statement: LOCK TABLE table_name  */
#line 2224 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6551 "parser.cpp"
    break;

  case 301: /* command_statement: UNLOCK TABLE table_name  */
#line 2233 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6565 "parser.cpp"
    break;

  case 302: /* compact_statement: COMPACT TABLE table_name  */
#line 2243 "parser.y"
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
#line 6582 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2256 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6591 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2260 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6601 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2265 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6612 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2271 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6624 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2278 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6637 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2286 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6651 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2295 "parser.y"
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
#line 6666 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2305 "parser.y"
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
#line 6681 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2315 "parser.y"
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
#line 6697 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2326 "parser.y"
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
#line 6713 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2337 "parser.y"
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
#line 6730 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2349 "parser.y"
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
#line 6747 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2361 "parser.y"
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
#line 6762 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2371 "parser.y"
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
#line 6778 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2382 "parser.y"
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
#line 6795 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2394 "parser.y"
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
#line 6813 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW LOGS  */
#line 2407 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6822 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2411 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6832 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2416 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6842 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2421 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6853 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2427 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6862 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2431 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6871 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2435 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6882 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SHOW NODES  */
#line 2441 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6891 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2445 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6902 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN SHOW NODE  */
#line 2451 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6911 "parser.cpp"
    break;

  case 329: /* admin_statement: ADMIN SET ADMIN  */
#line 2455 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6921 "parser.cpp"
    break;

  case 330: /* admin_statement: ADMIN SET STANDALONE  */
#line 2460 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6931 "parser.cpp"
    break;

  case 331: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2465 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6943 "parser.cpp"
    break;

  case 332: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2472 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6957 "parser.cpp"
    break;

  case 333: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2481 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6971 "parser.cpp"
    break;

  case 334: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2491 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 6985 "parser.cpp"
    break;

  case 335: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2500 "parser.y"
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
#line 7002 "parser.cpp"
    break;

  case 336: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2512 "parser.y"
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
#line 7018 "parser.cpp"
    break;

  case 337: /* expr_array: expr_alias  */
#line 2528 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7027 "parser.cpp"
    break;

  case 338: /* expr_array: expr_array ',' expr_alias  */
#line 2532 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7036 "parser.cpp"
    break;

  case 339: /* expr_array_list: '(' expr_array ')'  */
#line 2537 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 7045 "parser.cpp"
    break;

  case 340: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2541 "parser.y"
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
#line 7071 "parser.cpp"
    break;

  case 341: /* expr_alias: expr AS IDENTIFIER  */
#line 2574 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7082 "parser.cpp"
    break;

  case 342: /* expr_alias: expr  */
#line 2580 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7090 "parser.cpp"
    break;

  case 348: /* operand: '(' expr ')'  */
#line 2590 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 7098 "parser.cpp"
    break;

  case 349: /* operand: '(' select_without_paren ')'  */
#line 2593 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7109 "parser.cpp"
    break;

  case 350: /* operand: constant_expr  */
#line 2599 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 7117 "parser.cpp"
    break;

  case 361: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2615 "parser.y"
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
#line 7137 "parser.cpp"
    break;

  case 362: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2633 "parser.y"
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
#line 7187 "parser.cpp"
    break;

  case 363: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2679 "parser.y"
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
#line 7230 "parser.cpp"
    break;

  case 364: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2718 "parser.y"
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
#line 7277 "parser.cpp"
    break;

  case 365: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2761 "parser.y"
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
#line 7325 "parser.cpp"
    break;

  case 366: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2808 "parser.y"
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
#line 7353 "parser.cpp"
    break;

  case 367: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2832 "parser.y"
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
#line 7380 "parser.cpp"
    break;

  case 368: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2855 "parser.y"
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
#line 7405 "parser.cpp"
    break;

  case 369: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2876 "parser.y"
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
#line 7430 "parser.cpp"
    break;

  case 370: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2897 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7444 "parser.cpp"
    break;

  case 371: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2906 "parser.y"
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
#line 7460 "parser.cpp"
    break;

  case 372: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2918 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7472 "parser.cpp"
    break;

  case 373: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2925 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7486 "parser.cpp"
    break;

  case 374: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2935 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7497 "parser.cpp"
    break;

  case 375: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2941 "parser.y"
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
#line 7513 "parser.cpp"
    break;

  case 376: /* sub_search: match_vector_expr  */
#line 2953 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7521 "parser.cpp"
    break;

  case 377: /* sub_search: match_text_expr  */
#line 2956 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7529 "parser.cpp"
    break;

  case 378: /* sub_search: match_tensor_expr  */
#line 2959 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7537 "parser.cpp"
    break;

  case 379: /* sub_search: match_sparse_expr  */
#line 2962 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7545 "parser.cpp"
    break;

  case 380: /* sub_search: query_expr  */
#line 2965 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7553 "parser.cpp"
    break;

  case 381: /* sub_search: fusion_expr  */
#line 2968 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7561 "parser.cpp"
    break;

  case 382: /* sub_search_array: sub_search  */
#line 2972 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7570 "parser.cpp"
    break;

  case 383: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2976 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7579 "parser.cpp"
    break;

  case 384: /* function_expr: IDENTIFIER '(' ')'  */
#line 2981 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7592 "parser.cpp"
    break;

  case 385: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2989 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7605 "parser.cpp"
    break;

  case 386: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2997 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7619 "parser.cpp"
    break;

  case 387: /* function_expr: operand IS NOT NULLABLE  */
#line 3006 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7631 "parser.cpp"
    break;

  case 388: /* function_expr: operand IS NULLABLE  */
#line 3013 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7643 "parser.cpp"
    break;

  case 389: /* function_expr: NOT operand  */
#line 3020 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7655 "parser.cpp"
    break;

  case 390: /* function_expr: '-' operand  */
#line 3027 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7667 "parser.cpp"
    break;

  case 391: /* function_expr: '+' operand  */
#line 3034 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7679 "parser.cpp"
    break;

  case 392: /* function_expr: operand '-' operand  */
#line 3041 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7692 "parser.cpp"
    break;

  case 393: /* function_expr: operand '+' operand  */
#line 3049 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7705 "parser.cpp"
    break;

  case 394: /* function_expr: operand '*' operand  */
#line 3057 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7718 "parser.cpp"
    break;

  case 395: /* function_expr: operand '/' operand  */
#line 3065 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7731 "parser.cpp"
    break;

  case 396: /* function_expr: operand '%' operand  */
#line 3073 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7744 "parser.cpp"
    break;

  case 397: /* function_expr: operand '=' operand  */
#line 3081 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7757 "parser.cpp"
    break;

  case 398: /* function_expr: operand EQUAL operand  */
#line 3089 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7770 "parser.cpp"
    break;

  case 399: /* function_expr: operand NOT_EQ operand  */
#line 3097 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7783 "parser.cpp"
    break;

  case 400: /* function_expr: operand '<' operand  */
#line 3105 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7796 "parser.cpp"
    break;

  case 401: /* function_expr: operand '>' operand  */
#line 3113 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7809 "parser.cpp"
    break;

  case 402: /* function_expr: operand LESS_EQ operand  */
#line 3121 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7822 "parser.cpp"
    break;

  case 403: /* function_expr: operand GREATER_EQ operand  */
#line 3129 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7835 "parser.cpp"
    break;

  case 404: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3137 "parser.y"
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
#line 7870 "parser.cpp"
    break;

  case 405: /* function_expr: operand LIKE operand  */
#line 3167 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7883 "parser.cpp"
    break;

  case 406: /* function_expr: operand NOT LIKE operand  */
#line 3175 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7896 "parser.cpp"
    break;

  case 407: /* conjunction_expr: expr AND expr  */
#line 3184 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7909 "parser.cpp"
    break;

  case 408: /* conjunction_expr: expr OR expr  */
#line 3192 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7922 "parser.cpp"
    break;

  case 409: /* between_expr: operand BETWEEN operand AND operand  */
#line 3201 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7934 "parser.cpp"
    break;

  case 410: /* in_expr: operand IN '(' expr_array ')'  */
#line 3209 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7945 "parser.cpp"
    break;

  case 411: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3215 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7956 "parser.cpp"
    break;

  case 412: /* case_expr: CASE expr case_check_array END  */
#line 3222 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7967 "parser.cpp"
    break;

  case 413: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3228 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7979 "parser.cpp"
    break;

  case 414: /* case_expr: CASE case_check_array END  */
#line 3235 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7989 "parser.cpp"
    break;

  case 415: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3240 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 8000 "parser.cpp"
    break;

  case 416: /* case_check_array: WHEN expr THEN expr  */
#line 3247 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 8012 "parser.cpp"
    break;

  case 417: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3254 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 8024 "parser.cpp"
    break;

  case 418: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3262 "parser.y"
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
#line 8055 "parser.cpp"
    break;

  case 419: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3289 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8066 "parser.cpp"
    break;

  case 420: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3295 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8077 "parser.cpp"
    break;

  case 421: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3301 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8089 "parser.cpp"
    break;

  case 422: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3308 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 8101 "parser.cpp"
    break;

  case 423: /* column_expr: IDENTIFIER  */
#line 3316 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8113 "parser.cpp"
    break;

  case 424: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3323 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 8125 "parser.cpp"
    break;

  case 425: /* column_expr: '*'  */
#line 3330 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8135 "parser.cpp"
    break;

  case 426: /* column_expr: column_expr '.' '*'  */
#line 3335 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 8149 "parser.cpp"
    break;

  case 427: /* constant_expr: STRING  */
#line 3345 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8159 "parser.cpp"
    break;

  case 428: /* constant_expr: TRUE  */
#line 3350 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8169 "parser.cpp"
    break;

  case 429: /* constant_expr: FALSE  */
#line 3355 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8179 "parser.cpp"
    break;

  case 430: /* constant_expr: DOUBLE_VALUE  */
#line 3360 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8189 "parser.cpp"
    break;

  case 431: /* constant_expr: LONG_VALUE  */
#line 3365 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8199 "parser.cpp"
    break;

  case 432: /* constant_expr: DATE STRING  */
#line 3370 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8209 "parser.cpp"
    break;

  case 433: /* constant_expr: TIME STRING  */
#line 3375 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8219 "parser.cpp"
    break;

  case 434: /* constant_expr: DATETIME STRING  */
#line 3380 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8229 "parser.cpp"
    break;

  case 435: /* constant_expr: TIMESTAMP STRING  */
#line 3385 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8239 "parser.cpp"
    break;

  case 436: /* constant_expr: INTERVAL interval_expr  */
#line 3390 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8247 "parser.cpp"
    break;

  case 437: /* constant_expr: interval_expr  */
#line 3393 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8255 "parser.cpp"
    break;

  case 438: /* constant_expr: common_array_expr  */
#line 3396 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8263 "parser.cpp"
    break;

  case 439: /* common_array_expr: array_expr  */
#line 3400 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8271 "parser.cpp"
    break;

  case 440: /* common_array_expr: subarray_array_expr  */
#line 3403 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8279 "parser.cpp"
    break;

  case 441: /* common_array_expr: sparse_array_expr  */
#line 3406 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8287 "parser.cpp"
    break;

  case 442: /* common_array_expr: empty_array_expr  */
#line 3409 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8295 "parser.cpp"
    break;

  case 443: /* common_sparse_array_expr: sparse_array_expr  */
#line 3413 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8303 "parser.cpp"
    break;

  case 444: /* common_sparse_array_expr: array_expr  */
#line 3416 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8311 "parser.cpp"
    break;

  case 445: /* common_sparse_array_expr: empty_array_expr  */
#line 3419 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8319 "parser.cpp"
    break;

  case 446: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3423 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8327 "parser.cpp"
    break;

  case 447: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3427 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8337 "parser.cpp"
    break;

  case 448: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3432 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8346 "parser.cpp"
    break;

  case 449: /* sparse_array_expr: long_sparse_array_expr  */
#line 3437 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8354 "parser.cpp"
    break;

  case 450: /* sparse_array_expr: double_sparse_array_expr  */
#line 3440 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8362 "parser.cpp"
    break;

  case 451: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3444 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8370 "parser.cpp"
    break;

  case 452: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3448 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8382 "parser.cpp"
    break;

  case 453: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3455 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8393 "parser.cpp"
    break;

  case 454: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3462 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8401 "parser.cpp"
    break;

  case 455: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3466 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8413 "parser.cpp"
    break;

  case 456: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3473 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8424 "parser.cpp"
    break;

  case 457: /* empty_array_expr: '[' ']'  */
#line 3480 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8432 "parser.cpp"
    break;

  case 458: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3484 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8440 "parser.cpp"
    break;

  case 459: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3488 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8448 "parser.cpp"
    break;

  case 460: /* array_expr: long_array_expr  */
#line 3492 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8456 "parser.cpp"
    break;

  case 461: /* array_expr: double_array_expr  */
#line 3495 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8464 "parser.cpp"
    break;

  case 462: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3499 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8472 "parser.cpp"
    break;

  case 463: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3503 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8482 "parser.cpp"
    break;

  case 464: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3508 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8491 "parser.cpp"
    break;

  case 465: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3513 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8499 "parser.cpp"
    break;

  case 466: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3517 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8509 "parser.cpp"
    break;

  case 467: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3522 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8518 "parser.cpp"
    break;

  case 468: /* interval_expr: LONG_VALUE SECONDS  */
#line 3527 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8529 "parser.cpp"
    break;

  case 469: /* interval_expr: LONG_VALUE SECOND  */
#line 3533 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8540 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE MINUTES  */
#line 3539 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8551 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE MINUTE  */
#line 3545 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8562 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE HOURS  */
#line 3551 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8573 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE HOUR  */
#line 3557 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8584 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE DAYS  */
#line 3563 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8595 "parser.cpp"
    break;

  case 475: /* interval_expr: LONG_VALUE DAY  */
#line 3569 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8606 "parser.cpp"
    break;

  case 476: /* interval_expr: LONG_VALUE MONTHS  */
#line 3575 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8617 "parser.cpp"
    break;

  case 477: /* interval_expr: LONG_VALUE MONTH  */
#line 3581 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8628 "parser.cpp"
    break;

  case 478: /* interval_expr: LONG_VALUE YEARS  */
#line 3587 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8639 "parser.cpp"
    break;

  case 479: /* interval_expr: LONG_VALUE YEAR  */
#line 3593 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8650 "parser.cpp"
    break;

  case 480: /* copy_option_list: copy_option  */
#line 3604 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8659 "parser.cpp"
    break;

  case 481: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3608 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8668 "parser.cpp"
    break;

  case 482: /* copy_option: FORMAT IDENTIFIER  */
#line 3613 "parser.y"
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
#line 8704 "parser.cpp"
    break;

  case 483: /* copy_option: DELIMITER STRING  */
#line 3644 "parser.y"
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
#line 8719 "parser.cpp"
    break;

  case 484: /* copy_option: HEADER  */
#line 3654 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8729 "parser.cpp"
    break;

  case 485: /* copy_option: OFFSET LONG_VALUE  */
#line 3659 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8739 "parser.cpp"
    break;

  case 486: /* copy_option: LIMIT LONG_VALUE  */
#line 3664 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8749 "parser.cpp"
    break;

  case 487: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3669 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8759 "parser.cpp"
    break;

  case 488: /* file_path: STRING  */
#line 3675 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8767 "parser.cpp"
    break;

  case 489: /* if_exists: IF EXISTS  */
#line 3679 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8773 "parser.cpp"
    break;

  case 490: /* if_exists: %empty  */
#line 3680 "parser.y"
  { (yyval.bool_value) = false; }
#line 8779 "parser.cpp"
    break;

  case 491: /* if_not_exists: IF NOT EXISTS  */
#line 3682 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8785 "parser.cpp"
    break;

  case 492: /* if_not_exists: %empty  */
#line 3683 "parser.y"
  { (yyval.bool_value) = false; }
#line 8791 "parser.cpp"
    break;

  case 495: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3698 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8804 "parser.cpp"
    break;

  case 496: /* if_not_exists_info: %empty  */
#line 3706 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8812 "parser.cpp"
    break;

  case 497: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3710 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8820 "parser.cpp"
    break;

  case 498: /* with_index_param_list: %empty  */
#line 3713 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8828 "parser.cpp"
    break;

  case 499: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3717 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8836 "parser.cpp"
    break;

  case 500: /* optional_table_properties_list: %empty  */
#line 3720 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8844 "parser.cpp"
    break;

  case 501: /* index_param_list: index_param  */
#line 3724 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8853 "parser.cpp"
    break;

  case 502: /* index_param_list: index_param_list ',' index_param  */
#line 3728 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8862 "parser.cpp"
    break;

  case 503: /* index_param: IDENTIFIER  */
#line 3733 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8873 "parser.cpp"
    break;

  case 504: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3739 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8888 "parser.cpp"
    break;

  case 505: /* index_param: IDENTIFIER '=' STRING  */
#line 3749 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8903 "parser.cpp"
    break;

  case 506: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3759 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8916 "parser.cpp"
    break;

  case 507: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3767 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8929 "parser.cpp"
    break;

  case 508: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3778 "parser.y"
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
#line 8965 "parser.cpp"
    break;

  case 509: /* index_info: '(' IDENTIFIER ')'  */
#line 3809 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8976 "parser.cpp"
    break;


#line 8980 "parser.cpp"

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

#line 3816 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

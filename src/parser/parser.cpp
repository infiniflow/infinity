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
  YYSYMBOL_CAST = 81,                      /* CAST  */
  YYSYMBOL_END = 82,                       /* END  */
  YYSYMBOL_CASE = 83,                      /* CASE  */
  YYSYMBOL_ELSE = 84,                      /* ELSE  */
  YYSYMBOL_THEN = 85,                      /* THEN  */
  YYSYMBOL_WHEN = 86,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 87,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 88,                   /* INTEGER  */
  YYSYMBOL_INT = 89,                       /* INT  */
  YYSYMBOL_TINYINT = 90,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 91,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 92,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 93,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 94,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 95,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 96,                    /* DOUBLE  */
  YYSYMBOL_REAL = 97,                      /* REAL  */
  YYSYMBOL_DECIMAL = 98,                   /* DECIMAL  */
  YYSYMBOL_DATE = 99,                      /* DATE  */
  YYSYMBOL_TIME = 100,                     /* TIME  */
  YYSYMBOL_DATETIME = 101,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 102,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 103,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 104,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 105,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 106,                     /* UUID  */
  YYSYMBOL_POINT = 107,                    /* POINT  */
  YYSYMBOL_LINE = 108,                     /* LINE  */
  YYSYMBOL_LSEG = 109,                     /* LSEG  */
  YYSYMBOL_BOX = 110,                      /* BOX  */
  YYSYMBOL_PATH = 111,                     /* PATH  */
  YYSYMBOL_POLYGON = 112,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 113,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 114,                     /* BLOB  */
  YYSYMBOL_BITMAP = 115,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 116,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 117,                   /* VECTOR  */
  YYSYMBOL_BIT = 118,                      /* BIT  */
  YYSYMBOL_TEXT = 119,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 120,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 121,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 122,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 123,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 124,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 125,                  /* PRIMARY  */
  YYSYMBOL_KEY = 126,                      /* KEY  */
  YYSYMBOL_UNIQUE = 127,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 128,                 /* NULLABLE  */
  YYSYMBOL_IS = 129,                       /* IS  */
  YYSYMBOL_DEFAULT = 130,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 131,                     /* TRUE  */
  YYSYMBOL_FALSE = 132,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 133,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 134,                   /* SECOND  */
  YYSYMBOL_SECONDS = 135,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 136,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 137,                  /* MINUTES  */
  YYSYMBOL_HOUR = 138,                     /* HOUR  */
  YYSYMBOL_HOURS = 139,                    /* HOURS  */
  YYSYMBOL_DAY = 140,                      /* DAY  */
  YYSYMBOL_DAYS = 141,                     /* DAYS  */
  YYSYMBOL_MONTH = 142,                    /* MONTH  */
  YYSYMBOL_MONTHS = 143,                   /* MONTHS  */
  YYSYMBOL_YEAR = 144,                     /* YEAR  */
  YYSYMBOL_YEARS = 145,                    /* YEARS  */
  YYSYMBOL_EQUAL = 146,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 147,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 148,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 149,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 150,                  /* BETWEEN  */
  YYSYMBOL_AND = 151,                      /* AND  */
  YYSYMBOL_OR = 152,                       /* OR  */
  YYSYMBOL_EXTRACT = 153,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 154,                     /* LIKE  */
  YYSYMBOL_DATA = 155,                     /* DATA  */
  YYSYMBOL_LOG = 156,                      /* LOG  */
  YYSYMBOL_BUFFER = 157,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 158,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 159,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 160,                 /* MEMINDEX  */
  YYSYMBOL_USING = 161,                    /* USING  */
  YYSYMBOL_SESSION = 162,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 163,                   /* GLOBAL  */
  YYSYMBOL_OFF = 164,                      /* OFF  */
  YYSYMBOL_EXPORT = 165,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 166,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 167,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 168,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 169,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 170,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 171,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 172,                    /* DELTA  */
  YYSYMBOL_LOGS = 173,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 174,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 175,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 176,                   /* SEARCH  */
  YYSYMBOL_MATCH = 177,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 178,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 179,                    /* QUERY  */
  YYSYMBOL_QUERIES = 180,                  /* QUERIES  */
  YYSYMBOL_FUSION = 181,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 182,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 183,                    /* ADMIN  */
  YYSYMBOL_LEADER = 184,                   /* LEADER  */
  YYSYMBOL_FOLLOWER = 185,                 /* FOLLOWER  */
  YYSYMBOL_LEARNER = 186,                  /* LEARNER  */
  YYSYMBOL_CONNECT = 187,                  /* CONNECT  */
  YYSYMBOL_STANDALONE = 188,               /* STANDALONE  */
  YYSYMBOL_NODES = 189,                    /* NODES  */
  YYSYMBOL_NODE = 190,                     /* NODE  */
  YYSYMBOL_PERSISTENCE = 191,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 192,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 193,                  /* OBJECTS  */
  YYSYMBOL_FILES = 194,                    /* FILES  */
  YYSYMBOL_MEMORY = 195,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 196,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 197,                   /* NUMBER  */
  YYSYMBOL_198_ = 198,                     /* '='  */
  YYSYMBOL_199_ = 199,                     /* '<'  */
  YYSYMBOL_200_ = 200,                     /* '>'  */
  YYSYMBOL_201_ = 201,                     /* '+'  */
  YYSYMBOL_202_ = 202,                     /* '-'  */
  YYSYMBOL_203_ = 203,                     /* '*'  */
  YYSYMBOL_204_ = 204,                     /* '/'  */
  YYSYMBOL_205_ = 205,                     /* '%'  */
  YYSYMBOL_206_ = 206,                     /* '['  */
  YYSYMBOL_207_ = 207,                     /* ']'  */
  YYSYMBOL_208_ = 208,                     /* '('  */
  YYSYMBOL_209_ = 209,                     /* ')'  */
  YYSYMBOL_210_ = 210,                     /* '.'  */
  YYSYMBOL_211_ = 211,                     /* ';'  */
  YYSYMBOL_212_ = 212,                     /* ','  */
  YYSYMBOL_213_ = 213,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 214,                 /* $accept  */
  YYSYMBOL_input_pattern = 215,            /* input_pattern  */
  YYSYMBOL_statement_list = 216,           /* statement_list  */
  YYSYMBOL_statement = 217,                /* statement  */
  YYSYMBOL_explainable_statement = 218,    /* explainable_statement  */
  YYSYMBOL_create_statement = 219,         /* create_statement  */
  YYSYMBOL_table_element_array = 220,      /* table_element_array  */
  YYSYMBOL_column_def_array = 221,         /* column_def_array  */
  YYSYMBOL_table_element = 222,            /* table_element  */
  YYSYMBOL_table_column = 223,             /* table_column  */
  YYSYMBOL_column_type = 224,              /* column_type  */
  YYSYMBOL_column_constraints = 225,       /* column_constraints  */
  YYSYMBOL_column_constraint = 226,        /* column_constraint  */
  YYSYMBOL_default_expr = 227,             /* default_expr  */
  YYSYMBOL_table_constraint = 228,         /* table_constraint  */
  YYSYMBOL_identifier_array = 229,         /* identifier_array  */
  YYSYMBOL_delete_statement = 230,         /* delete_statement  */
  YYSYMBOL_insert_statement = 231,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 232, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 233,        /* explain_statement  */
  YYSYMBOL_update_statement = 234,         /* update_statement  */
  YYSYMBOL_update_expr_array = 235,        /* update_expr_array  */
  YYSYMBOL_update_expr = 236,              /* update_expr  */
  YYSYMBOL_drop_statement = 237,           /* drop_statement  */
  YYSYMBOL_copy_statement = 238,           /* copy_statement  */
  YYSYMBOL_select_statement = 239,         /* select_statement  */
  YYSYMBOL_select_with_paren = 240,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 241,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 242, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 243, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 244, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 245,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 246,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 247,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 248,            /* order_by_type  */
  YYSYMBOL_limit_expr = 249,               /* limit_expr  */
  YYSYMBOL_offset_expr = 250,              /* offset_expr  */
  YYSYMBOL_distinct = 251,                 /* distinct  */
  YYSYMBOL_from_clause = 252,              /* from_clause  */
  YYSYMBOL_search_clause = 253,            /* search_clause  */
  YYSYMBOL_optional_search_filter_expr = 254, /* optional_search_filter_expr  */
  YYSYMBOL_where_clause = 255,             /* where_clause  */
  YYSYMBOL_having_clause = 256,            /* having_clause  */
  YYSYMBOL_group_by_clause = 257,          /* group_by_clause  */
  YYSYMBOL_set_operator = 258,             /* set_operator  */
  YYSYMBOL_table_reference = 259,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 260,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 261,     /* table_reference_name  */
  YYSYMBOL_table_name = 262,               /* table_name  */
  YYSYMBOL_table_alias = 263,              /* table_alias  */
  YYSYMBOL_with_clause = 264,              /* with_clause  */
  YYSYMBOL_with_expr_list = 265,           /* with_expr_list  */
  YYSYMBOL_with_expr = 266,                /* with_expr  */
  YYSYMBOL_join_clause = 267,              /* join_clause  */
  YYSYMBOL_join_type = 268,                /* join_type  */
  YYSYMBOL_show_statement = 269,           /* show_statement  */
  YYSYMBOL_flush_statement = 270,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 271,       /* optimize_statement  */
  YYSYMBOL_command_statement = 272,        /* command_statement  */
  YYSYMBOL_compact_statement = 273,        /* compact_statement  */
  YYSYMBOL_admin_statement = 274,          /* admin_statement  */
  YYSYMBOL_alter_statement = 275,          /* alter_statement  */
  YYSYMBOL_expr_array = 276,               /* expr_array  */
  YYSYMBOL_expr_array_list = 277,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 278,               /* expr_alias  */
  YYSYMBOL_expr = 279,                     /* expr  */
  YYSYMBOL_operand = 280,                  /* operand  */
  YYSYMBOL_match_tensor_expr = 281,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 282,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 283,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 284,          /* match_text_expr  */
  YYSYMBOL_query_expr = 285,               /* query_expr  */
  YYSYMBOL_fusion_expr = 286,              /* fusion_expr  */
  YYSYMBOL_sub_search = 287,               /* sub_search  */
  YYSYMBOL_sub_search_array = 288,         /* sub_search_array  */
  YYSYMBOL_function_expr = 289,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 290,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 291,             /* between_expr  */
  YYSYMBOL_in_expr = 292,                  /* in_expr  */
  YYSYMBOL_case_expr = 293,                /* case_expr  */
  YYSYMBOL_case_check_array = 294,         /* case_check_array  */
  YYSYMBOL_cast_expr = 295,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 296,            /* subquery_expr  */
  YYSYMBOL_column_expr = 297,              /* column_expr  */
  YYSYMBOL_constant_expr = 298,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 299,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 300, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 301,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 302, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 303,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 304,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 305, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 306, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 307, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 308,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 309,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 310,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 311,               /* array_expr  */
  YYSYMBOL_long_array_expr = 312,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 313, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 314,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 315, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 316,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 317,         /* copy_option_list  */
  YYSYMBOL_copy_option = 318,              /* copy_option  */
  YYSYMBOL_file_path = 319,                /* file_path  */
  YYSYMBOL_if_exists = 320,                /* if_exists  */
  YYSYMBOL_if_not_exists = 321,            /* if_not_exists  */
  YYSYMBOL_semicolon = 322,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 323,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 324,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 325, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 326,         /* index_param_list  */
  YYSYMBOL_index_param = 327,              /* index_param  */
  YYSYMBOL_index_info = 328                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 457 "parser.cpp"

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
#define YYFINAL  114
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1378

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  214
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  504
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1132

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   452


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
       2,     2,     2,     2,     2,     2,     2,   205,     2,     2,
     208,   209,   203,   201,   212,   202,   210,   204,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   213,   211,
     199,   198,   200,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   206,     2,   207,     2,     2,     2,     2,     2,     2,
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
     195,   196,   197
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   506,   506,   510,   516,   523,   524,   525,   526,   527,
     528,   529,   530,   531,   532,   533,   534,   535,   536,   537,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     549,   550,   557,   574,   590,   619,   635,   653,   682,   686,
     691,   695,   701,   704,   711,   762,   801,   802,   803,   804,
     805,   806,   807,   808,   809,   810,   811,   812,   813,   814,
     815,   816,   817,   818,   819,   820,   821,   824,   826,   827,
     828,   829,   832,   833,   834,   835,   836,   837,   838,   839,
     840,   841,   842,   843,   844,   845,   846,   847,   848,   849,
     850,   851,   852,   853,   854,   855,   856,   857,   858,   859,
     860,   861,   862,   863,   864,   865,   866,   867,   868,   869,
     870,   871,   872,   873,   874,   875,   876,   877,   878,   879,
     880,   881,   882,   883,   884,   885,   886,   887,   888,   889,
     890,   891,   892,   893,   894,   895,   896,   897,   916,   920,
     930,   933,   936,   939,   943,   946,   951,   956,   963,   969,
     979,   995,  1029,  1042,  1045,  1052,  1064,  1073,  1086,  1090,
    1095,  1108,  1121,  1136,  1151,  1166,  1189,  1242,  1297,  1348,
    1351,  1354,  1363,  1373,  1376,  1380,  1385,  1412,  1415,  1420,
    1436,  1439,  1443,  1447,  1452,  1458,  1461,  1464,  1468,  1472,
    1474,  1478,  1480,  1483,  1487,  1490,  1494,  1499,  1503,  1506,
    1510,  1513,  1517,  1520,  1524,  1527,  1531,  1534,  1537,  1540,
    1548,  1551,  1566,  1566,  1568,  1582,  1591,  1596,  1605,  1610,
    1615,  1621,  1628,  1631,  1635,  1638,  1643,  1655,  1662,  1676,
    1679,  1682,  1685,  1688,  1691,  1694,  1700,  1704,  1708,  1712,
    1716,  1723,  1727,  1731,  1735,  1739,  1744,  1748,  1753,  1757,
    1761,  1767,  1773,  1779,  1790,  1801,  1812,  1824,  1836,  1849,
    1863,  1874,  1888,  1904,  1921,  1925,  1929,  1933,  1937,  1941,
    1947,  1951,  1955,  1963,  1967,  1971,  1979,  1990,  2013,  2019,
    2024,  2030,  2036,  2044,  2050,  2056,  2062,  2068,  2076,  2082,
    2088,  2094,  2100,  2108,  2114,  2120,  2129,  2139,  2152,  2156,
    2161,  2167,  2174,  2182,  2191,  2201,  2211,  2222,  2233,  2245,
    2257,  2267,  2278,  2290,  2303,  2307,  2312,  2317,  2323,  2327,
    2331,  2337,  2341,  2347,  2351,  2356,  2361,  2368,  2377,  2387,
    2396,  2408,  2424,  2428,  2433,  2437,  2470,  2476,  2480,  2481,
    2482,  2483,  2484,  2486,  2489,  2495,  2498,  2499,  2500,  2501,
    2502,  2503,  2504,  2505,  2506,  2507,  2511,  2529,  2575,  2614,
    2657,  2704,  2728,  2751,  2772,  2793,  2802,  2814,  2821,  2831,
    2837,  2849,  2852,  2855,  2858,  2861,  2864,  2868,  2872,  2877,
    2885,  2893,  2902,  2909,  2916,  2923,  2930,  2937,  2945,  2953,
    2961,  2969,  2977,  2985,  2993,  3001,  3009,  3017,  3025,  3033,
    3063,  3071,  3080,  3088,  3097,  3105,  3111,  3118,  3124,  3131,
    3136,  3143,  3150,  3158,  3185,  3191,  3197,  3204,  3212,  3219,
    3226,  3231,  3241,  3246,  3251,  3256,  3261,  3266,  3271,  3276,
    3281,  3286,  3289,  3292,  3296,  3299,  3302,  3305,  3309,  3312,
    3315,  3319,  3323,  3328,  3333,  3336,  3340,  3344,  3351,  3358,
    3362,  3369,  3376,  3380,  3384,  3388,  3391,  3395,  3399,  3404,
    3409,  3413,  3418,  3423,  3429,  3435,  3441,  3447,  3453,  3459,
    3465,  3471,  3477,  3483,  3489,  3500,  3504,  3509,  3540,  3550,
    3555,  3560,  3565,  3571,  3575,  3576,  3578,  3579,  3581,  3582,
    3594,  3602,  3606,  3609,  3613,  3616,  3620,  3624,  3629,  3635,
    3645,  3655,  3663,  3674,  3705
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
  "offset_expr", "distinct", "from_clause", "search_clause",
  "optional_search_filter_expr", "where_clause", "having_clause",
  "group_by_clause", "set_operator", "table_reference",
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

#define YYPACT_NINF (-689)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-492)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     754,   382,    42,   423,   133,    78,   133,     3,    71,   704,
      10,    56,   130,   167,   430,   182,   241,   290,   111,    15,
     -47,   311,    88,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,   292,  -689,  -689,   295,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,   244,   244,   244,   244,   107,   133,   253,   253,
     253,   253,   253,   138,   397,   133,   -29,   357,   441,   446,
     779,  -689,  -689,  -689,  -689,  -689,  -689,  -689,   292,  -689,
    -689,  -689,  -689,  -689,   462,   133,  -689,  -689,  -689,  -689,
    -689,   465,  -689,    41,    87,  -689,   473,  -689,   348,  -689,
    -689,   383,  -689,   297,    96,   133,   133,   133,   133,  -689,
    -689,  -689,  -689,   -34,  -689,   431,   339,  -689,   561,  -111,
     199,   567,   398,   399,  -689,   218,  -689,   588,  -689,  -689,
       5,   549,  -689,   548,   545,   612,   133,   133,   133,   615,
     558,   412,   550,   618,   133,   133,   133,   621,   622,   628,
     568,   630,   630,   511,    36,   102,   116,  -689,  -689,   313,
    -689,  -689,   629,  -689,   633,  -689,  -689,   632,   635,  -689,
    -689,  -689,  -689,    66,  -689,  -689,  -689,   133,   432,   290,
     630,  -689,   480,  -689,   645,  -689,  -689,   649,  -689,  -689,
     650,  -689,   653,   604,  -689,  -689,  -689,  -689,     5,  -689,
    -689,  -689,   511,   608,   594,   589,  -689,   -28,  -689,   412,
    -689,   133,   660,   248,  -689,  -689,  -689,  -689,  -689,   609,
    -689,   477,   -31,  -689,   511,  -689,  -689,   600,   605,   463,
    -689,  -689,   996,   554,   475,   476,   387,   685,   689,   690,
     692,  -689,  -689,   691,   483,   260,   493,   494,   643,   643,
    -689,    13,   497,   155,  -689,   -14,   768,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
     498,  -689,  -689,  -689,   110,  -689,  -689,   114,  -689,   147,
    -689,  -689,  -689,   151,  -689,   164,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,   701,   703,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,   663,   664,   640,   641,   295,  -689,  -689,   716,    26,
    -689,   715,  -689,   219,   513,   514,   -30,   511,   511,   657,
    -689,   -47,    30,   673,   520,  -689,   197,   521,  -689,   133,
     511,   628,  -689,   272,   522,   524,   346,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,   643,
     526,   802,   659,   511,   511,   121,   312,  -689,  -689,  -689,
    -689,   996,  -689,   735,   533,   539,   541,   542,   747,   748,
     396,   396,  -689,   540,  -689,  -689,  -689,  -689,   563,   -53,
     678,   511,   755,   511,   511,   -25,   565,    34,   643,   643,
     643,   643,   643,   643,   643,   643,   643,   643,   643,   643,
     643,   643,    11,  -689,   574,  -689,   775,  -689,   776,  -689,
     781,  -689,   789,   756,   242,   590,   592,   794,   593,  -689,
     596,  -689,   800,  -689,   271,   646,   651,  -689,  -689,     8,
     637,   599,  -689,    54,   272,   511,  -689,   292,   921,   688,
     607,   216,  -689,  -689,  -689,   -47,   814,  -689,  -689,   815,
     511,   611,  -689,   272,  -689,   183,   183,   511,  -689,   227,
     659,   665,   616,     7,   -42,   366,  -689,   511,   511,   746,
     511,   822,    12,   511,   617,   231,   448,  -689,  -689,   630,
    -689,  -689,  -689,   676,   620,   643,   497,   702,  -689,   792,
     792,   136,   136,   778,   792,   792,   136,   136,   396,   396,
    -689,  -689,  -689,  -689,  -689,  -689,   619,  -689,   624,  -689,
    -689,  -689,   832,   833,  -689,   660,   840,  -689,   844,  -689,
    -689,   842,  -689,   848,   849,   -47,   642,   375,  -689,   221,
    -689,   303,   568,   511,  -689,  -689,  -689,   272,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,   647,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,   652,   661,   662,   667,   671,   672,   204,   674,
     660,   825,    30,   292,   656,  -689,   252,   686,   852,   853,
     864,   878,  -689,   887,   257,  -689,   299,   300,  -689,   693,
    -689,   921,   511,  -689,   511,   -24,   -17,   643,   -97,   694,
    -689,   351,   -92,    65,   695,  -689,   894,  -689,  -689,   823,
     497,   792,   696,   301,  -689,   643,   895,   898,   858,   862,
     317,   319,  -689,   712,   321,  -689,   905,  -689,  -689,    28,
       8,   856,  -689,  -689,  -689,  -689,  -689,  -689,   857,  -689,
     917,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,   711,
     877,  -689,   924,   577,   957,   974,   991,  1008,  1025,   805,
     808,  -689,  -689,   180,  -689,   806,   660,   323,   727,  -689,
    -689,   782,  -689,   511,  -689,  -689,  -689,  -689,  -689,  -689,
     183,  -689,  -689,  -689,   733,   272,   -20,  -689,   511,   687,
     739,   943,   574,   749,   740,   511,  -689,   744,   750,   745,
     325,  -689,  -689,   802,   951,   953,  -689,  -689,   840,   381,
    -689,   844,   543,   221,   375,     8,     8,   752,   303,   909,
     910,   330,   753,   762,   763,   772,   773,   774,   777,   786,
     787,   874,   813,   820,   821,   824,   827,   828,   838,   839,
     843,   845,   945,   846,   855,   859,   860,   861,   872,   873,
     876,   879,   889,   964,   890,   893,   896,   906,   907,   911,
     912,   913,   930,   932,   966,   933,   934,   935,   936,   937,
     938,   939,   940,   941,   942,   978,   944,   946,   947,   948,
     949,   950,   952,   954,   955,   956,   984,   958,  -689,  -689,
     169,  -689,  -689,  -689,   332,  -689,   844,   985,   334,  -689,
    -689,  -689,   272,  -689,   492,   959,   960,   961,    19,   962,
    -689,  -689,  -689,  1024,   881,   272,  -689,   183,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  1100,  -689,
    -689,  -689,  1046,   660,  -689,   511,   511,  -689,  -689,  1116,
    1149,  1151,  1157,  1159,  1163,  1169,  1170,  1171,  1172,   967,
    1174,  1175,  1176,  1177,  1178,  1179,  1180,  1181,  1182,  1183,
     979,  1184,  1186,  1187,  1188,  1189,  1190,  1191,  1192,  1193,
    1194,   989,  1196,  1197,  1198,  1199,  1200,  1201,  1202,  1203,
    1204,  1205,  1000,  1207,  1208,  1209,  1210,  1211,  1212,  1213,
    1214,  1215,  1216,  1011,  1218,  1219,  1220,  1221,  1222,  1223,
    1224,  1225,  1226,  1227,  1022,  1229,  -689,  -689,   344,   641,
    -689,  -689,  1232,    84,  1028,  1234,  1235,  -689,   363,  1236,
     511,   364,  1029,   272,  1031,  1034,  1035,  1036,  1037,  1038,
    1039,  1040,  1041,  1042,  1246,  1044,  1045,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1249,  1055,  1056,  1057,  1058,
    1059,  1060,  1061,  1062,  1063,  1064,  1268,  1066,  1067,  1068,
    1069,  1070,  1071,  1072,  1073,  1074,  1075,  1279,  1077,  1078,
    1079,  1080,  1081,  1082,  1083,  1084,  1085,  1086,  1290,  1088,
    1089,  1090,  1091,  1092,  1093,  1094,  1095,  1096,  1097,  1301,
    1099,  -689,  -689,  1098,   740,  -689,  1101,  1102,  -689,   359,
     272,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,  1103,  -689,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  1106,  -689,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  1107,  -689,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  1108,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  1109,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  -689,  -689,  1110,  -689,  1305,
    1111,  1307,    61,  1112,  1316,  1317,  -689,  -689,  -689,  -689,
    -689,  -689,  -689,  -689,  -689,  1113,  -689,  1114,   740,   641,
    1318,   536,    80,  1118,  1324,  1120,  -689,   560,  1325,  -689,
     740,   641,   740,    -7,  1326,  -689,  1282,  1124,  -689,  1125,
    1295,  1296,  -689,  -689,  -689,    48,  -689,  -689,  1129,  1298,
    1299,  -689,  1337,  -689,  1133,  1134,  1339,   641,  1135,  -689,
     641,  -689
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     223,     0,     0,     0,     0,     0,     0,     0,   223,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     223,     0,   489,     3,     5,    10,    12,    13,    11,     6,
       7,     9,   170,   169,     0,     8,    14,    15,    16,    17,
      18,    19,   487,   487,   487,   487,   487,     0,   485,   485,
     485,   485,   485,   216,     0,     0,     0,     0,     0,     0,
     223,   156,    20,    25,    27,    26,    21,    22,    24,    23,
      28,    29,    30,    31,     0,     0,   237,   238,   236,   242,
     246,     0,   243,     0,     0,   239,     0,   241,     0,   264,
     266,     0,   244,     0,   270,     0,     0,     0,     0,   273,
     274,   275,   278,   216,   276,     0,   222,   224,     0,     0,
       0,     0,     0,     0,     1,   223,     2,   206,   208,   209,
       0,   193,   175,   181,     0,     0,     0,     0,     0,     0,
       0,   154,     0,     0,     0,     0,     0,     0,     0,     0,
     201,     0,     0,     0,     0,     0,     0,   155,   252,   253,
     247,   248,     0,   249,     0,   240,   265,     0,     0,   268,
     267,   271,   272,     0,   297,   295,   296,     0,     0,     0,
       0,   324,     0,   325,     0,   318,   319,     0,   314,   298,
       0,   321,   323,     0,   174,   173,     4,   207,     0,   171,
     172,   192,     0,     0,   189,     0,    32,     0,    33,   154,
     490,     0,     0,   223,   484,   161,   163,   162,   164,     0,
     217,     0,   201,   158,     0,   150,   483,     0,     0,   418,
     422,   425,   426,     0,     0,     0,     0,     0,     0,     0,
       0,   423,   424,     0,     0,     0,     0,     0,     0,     0,
     420,     0,   223,     0,   332,   337,   338,   352,   350,   353,
     351,   354,   355,   347,   342,   341,   340,   348,   349,   339,
     346,   345,   433,   435,     0,   436,   444,     0,   445,     0,
     437,   434,   455,     0,   456,     0,   432,   282,   284,   283,
     280,   281,   287,   289,   288,   285,   286,   292,   294,   293,
     290,   291,     0,     0,   255,   254,   260,   250,   251,   245,
     269,     0,     0,     0,   493,     0,   225,   279,     0,   315,
     320,   299,   322,     0,     0,     0,   195,     0,     0,   191,
     486,   223,     0,     0,     0,   148,     0,     0,   152,     0,
       0,     0,   157,   200,     0,     0,     0,   464,   463,   466,
     465,   468,   467,   470,   469,   472,   471,   474,   473,     0,
       0,   384,   223,     0,     0,     0,     0,   427,   428,   429,
     430,     0,   431,     0,     0,     0,     0,     0,     0,     0,
     386,   385,   461,   458,   452,   442,   447,   450,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   441,     0,   446,     0,   449,     0,   457,
       0,   460,     0,   261,   256,     0,     0,     0,     0,   277,
       0,   326,     0,   316,     0,     0,     0,   178,   177,     0,
     197,   180,   182,   187,   188,     0,   176,    35,     0,     0,
       0,     0,    38,    42,    43,   223,     0,    37,   153,     0,
       0,   151,   165,   160,   159,     0,     0,     0,   379,     0,
     223,     0,     0,     0,     0,     0,   409,     0,     0,     0,
       0,     0,     0,     0,   199,     0,     0,   344,   343,     0,
     333,   336,   402,   403,     0,     0,   223,     0,   383,   393,
     394,   397,   398,     0,   400,   392,   395,   396,   388,   387,
     389,   390,   391,   419,   421,   443,     0,   448,     0,   451,
     459,   462,     0,     0,   257,     0,     0,   329,     0,   226,
     317,     0,   300,     0,     0,   223,   194,   210,   212,   221,
     213,     0,   201,     0,   185,   186,   184,   190,    46,    49,
      50,    47,    48,    51,    52,    68,    53,    55,    54,    71,
      58,    59,    60,    56,    57,    61,    62,    63,    64,    65,
      66,    67,     0,     0,     0,     0,     0,     0,   493,     0,
       0,   495,     0,    36,     0,   149,     0,     0,     0,     0,
       0,     0,   479,     0,     0,   475,     0,     0,   380,     0,
     414,     0,     0,   407,     0,     0,     0,     0,     0,     0,
     418,     0,     0,     0,     0,   369,     0,   454,   453,     0,
     223,   401,     0,     0,   382,     0,     0,     0,   262,   258,
       0,     0,    40,   498,     0,   496,   301,   327,   328,     0,
       0,     0,   230,   231,   232,   233,   229,   234,     0,   219,
       0,   214,   373,   371,   374,   372,   375,   376,   377,   196,
     205,   183,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,   142,   145,   138,   145,     0,     0,     0,    34,
      39,   504,   334,     0,   481,   480,   478,   477,   482,   168,
       0,   166,   381,   415,     0,   411,     0,   410,     0,     0,
       0,     0,     0,     0,   199,     0,   367,     0,     0,     0,
       0,   416,   405,   404,     0,     0,   331,   330,     0,     0,
     492,     0,     0,   221,   211,     0,     0,   218,     0,     0,
     203,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   143,   140,
       0,   139,    45,    44,     0,   147,     0,     0,     0,   476,
     413,   408,   412,   399,     0,     0,   199,     0,     0,     0,
     438,   440,   439,     0,     0,   198,   370,     0,   417,   406,
     263,   259,    41,   499,   500,   502,   501,   497,     0,   302,
     215,   227,     0,     0,   378,     0,     0,   179,    70,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   144,   146,     0,   493,
     335,   458,     0,     0,     0,     0,     0,   368,     0,   303,
       0,     0,   204,   202,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   494,   503,     0,   199,   365,     0,   199,   167,     0,
     228,   220,    69,    75,    76,    73,    74,    77,    78,    79,
      80,    81,     0,    72,   119,   120,   117,   118,   121,   122,
     123,   124,   125,     0,   116,    86,    87,    84,    85,    88,
      89,    90,    91,    92,     0,    83,    97,    98,    95,    96,
      99,   100,   101,   102,   103,     0,    94,   130,   131,   128,
     129,   132,   133,   134,   135,   136,     0,   127,   108,   109,
     106,   107,   110,   111,   112,   113,   114,     0,   105,     0,
       0,     0,     0,     0,     0,     0,   305,   304,   310,    82,
     126,    93,   104,   137,   115,   199,   366,     0,   199,   493,
     311,   306,     0,     0,     0,     0,   364,     0,     0,   307,
     199,   493,   199,   493,     0,   312,   308,     0,   360,     0,
       0,     0,   363,   313,   309,   493,   356,   362,     0,     0,
       0,   359,     0,   358,     0,     0,     0,   493,     0,   361,
     493,   357
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -689,  -689,  -689,  1230,  1286,    81,  -689,  -689,   780,  -500,
     757,  -689,   684,   697,  -689,  -506,   195,   237,  1150,  -689,
     238,  -689,  1019,   256,   263,    -3,  1331,   -19,  1065,  1165,
     -55,  -689,  -689,   826,  -689,  -689,  -689,  -689,  -689,  -689,
    -688,  -204,  -689,  -689,  -689,  -689,   724,  -110,    16,   644,
    -689,  -689,  1195,  -689,  -689,   270,   274,   282,   283,   286,
    -689,  -689,  -190,  -689,   975,  -214,  -202,  -514,  -508,  -496,
    -480,  -479,  -478,   648,  -689,  -689,  -689,  -689,  -689,  -689,
    1003,  -689,  -689,   883,   570,  -237,  -689,  -689,  -689,   668,
    -689,  -689,  -689,  -689,   670,   963,   965,  -218,  -689,  -689,
    -689,  -689,  1132,  -449,   698,  -132,   444,   461,  -689,  -689,
    -565,  -689,   571,   666,  -689
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    21,    22,    23,    61,    24,   441,   621,   442,   443,
     568,   663,   664,   792,   444,   326,    25,    26,   203,    27,
      28,   212,   213,    29,    30,    31,    32,    33,   122,   189,
     123,   194,   431,   432,   536,   319,   436,   192,   430,   532,
     604,   215,   837,   720,   120,   526,   527,   528,   529,   641,
      34,   106,   107,   530,   638,    35,    36,    37,    38,    39,
      40,    41,   243,   451,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   648,   649,   253,   254,   255,   256,   257,
     356,   258,   259,   260,   261,   262,   809,   263,   264,   265,
     266,   267,   268,   269,   270,   376,   377,   271,   272,   273,
     274,   275,   276,   584,   585,   217,   133,   125,   116,   130,
     419,   669,   624,   625,   447
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     333,   113,   316,   665,   375,    68,   814,   586,   332,   620,
     218,    53,   355,   121,   503,   600,   622,   642,   372,   373,
      54,   351,    56,   643,   372,   373,   321,   167,   379,   109,
      17,   110,   104,   438,   214,   644,   370,   371,   307,   277,
     382,   278,   279,   592,    95,   429,   141,   142,   117,   484,
     118,   645,   646,   647,   383,   384,   119,    68,   687,   383,
     384,   591,   801,   131,   667,   190,   422,  1088,   688,   694,
     418,   140,   171,   172,    60,   423,   301,   173,     1,    47,
       2,     3,     4,     5,     6,     7,  1100,     9,   994,    62,
      96,   149,   302,   303,    11,    12,    13,   280,   383,   384,
      14,    15,    16,   433,   434,   282,   487,   283,   284,   383,
     384,   163,   164,   165,   166,   690,   453,  1110,   914,   287,
     693,   288,   289,   534,   535,   418,   695,   383,   384,   485,
     695,   383,   384,   315,   383,   384,    53,   383,   384,   463,
     464,    62,   197,   198,   199,   695,   459,   351,    17,   695,
     206,   207,   208,    55,  1111,   439,   478,   440,   383,   384,
     794,    20,   488,   285,    97,    57,    58,   505,  -491,   482,
     483,    59,  1119,   220,   221,   222,   138,   290,   124,   143,
     322,   331,   381,   304,   328,   102,   489,   490,   491,   492,
     493,   494,   495,   496,   497,   498,   499,   500,   501,   502,
     281,    98,   111,    63,   642,   383,   384,   354,   822,  1120,
     643,   151,   152,   188,   504,   240,   525,   324,  -488,   241,
     374,   537,   644,   378,   639,     1,   374,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    18,   713,   645,   646,
     647,    11,    12,    13,   103,    64,    65,    14,    15,    16,
     578,   579,   659,   595,   596,    63,   598,   153,   154,   602,
     576,   580,   581,   582,    66,   387,   286,   587,   227,   228,
     229,    67,   383,   384,   230,   640,   659,   108,    69,    20,
     291,   418,    70,   611,  -492,  -492,   327,   513,   514,   161,
      71,    72,   162,   105,    73,    17,   613,    64,    65,   115,
     231,   232,   233,   121,   521,   660,  1070,   661,   662,  1073,
     790,   114,   117,   522,   118,   124,    66,   403,   437,   433,
     119,   405,   404,    67,   132,    17,   406,   921,   650,   660,
      69,   661,   662,   462,    70,  -492,  -492,   397,   398,   399,
     400,   401,    71,    72,   992,   452,    73,   609,   138,   219,
     220,   221,   222,   292,   407,   174,   293,   294,   409,   408,
     144,   295,   296,   410,   380,   583,   175,   381,   918,   176,
     177,   411,   178,   179,   180,   241,   412,   364,   685,   365,
     686,   366,   367,    18,   823,   824,   825,   826,   181,   182,
     219,   220,   221,   222,   466,   689,   467,  1093,   468,  1074,
    1095,    19,  1075,  1076,   425,   426,   448,  1077,  1078,   449,
     457,   139,  1107,   703,  1109,    42,    43,    44,   223,   224,
     700,    45,    46,   383,   384,   571,    20,   225,   572,   226,
     631,  -235,   632,   633,   634,   635,   588,   636,   637,   381,
     605,   589,   573,   606,   145,   227,   228,   229,   593,   146,
     594,   230,   468,   607,   608,   807,    48,    49,    50,   223,
     224,   672,    51,    52,   381,   148,   679,   612,   225,   680,
     226,   150,   805,   354,   802,   812,   155,   231,   232,   233,
     235,   815,   236,   798,   237,   168,   227,   228,   229,   158,
     159,   160,   230,   134,   135,   136,   137,   372,   911,   234,
     219,   220,   221,   222,   126,   127,   128,   129,   681,   682,
     702,   680,   381,   381,   219,   220,   221,   222,   231,   232,
     233,   156,   629,   235,  1096,   236,   706,   237,   707,   449,
     710,   708,   795,   711,   819,   449,  1108,   381,  1112,   838,
     234,   907,   839,   910,   449,   157,   381,   238,   239,   240,
    1121,   169,   241,   991,   242,   458,   711,   219,   220,   221,
     222,   402,  1129,   692,   235,  1131,   236,   170,   237,   223,
     224,   183,   998,  1001,    17,   680,   449,   828,   225,   829,
     226,  1098,  1099,   223,   224,    99,   100,   101,   238,   239,
     240,   699,   225,   241,   226,   242,   227,   228,   229,   399,
     400,   401,   230,  1104,  1105,   831,   832,   184,   185,   187,
     227,   228,   229,   191,   193,   196,   230,   195,   200,   201,
     202,   205,   923,   204,   209,   210,   349,   350,   231,   232,
     233,   211,   297,   214,   216,   225,   298,   226,   299,   300,
     305,   308,   231,   232,   233,   922,   219,   220,   221,   222,
     234,   309,   310,   227,   228,   229,   311,   312,   313,   230,
     317,   318,   320,   325,   234,   722,   723,   724,   725,   726,
     329,   336,   727,   728,   235,   330,   236,   334,   237,   729,
     730,   731,   335,   352,   353,   231,   232,   233,   235,   357,
     236,   363,   237,   358,   359,   732,   360,   361,   238,   239,
     240,   368,   369,   241,   413,   242,  1000,   234,   402,   414,
     415,   416,   238,   239,   240,   349,   417,   241,   418,   242,
     421,   424,   427,   428,   225,   435,   226,   445,   446,   450,
     455,   235,   456,   236,   460,   237,    17,    74,    75,   469,
      76,   470,   227,   228,   229,    77,    78,   471,   230,   472,
     473,   474,   475,   476,   479,   238,   239,   240,   481,   461,
     241,     1,   242,     2,     3,     4,     5,     6,     7,     8,
       9,    10,   477,   486,   231,   232,   233,    11,    12,    13,
     241,   506,   508,    14,    15,    16,     1,   510,     2,     3,
       4,     5,     6,     7,   511,     9,   234,   517,   515,   512,
     516,   518,    11,    12,    13,   519,   520,   523,    14,    15,
      16,   533,   524,   531,   569,   570,   387,   574,   575,   485,
     235,   597,   236,   577,   237,   590,   599,   383,   610,   603,
     614,    17,   616,   388,   389,   390,   391,   617,   618,   619,
     385,   393,   386,   438,   238,   239,   240,   623,   626,   241,
     461,   242,   627,   628,   630,   652,    17,   668,   674,   675,
     653,    79,    80,    81,    82,   671,    83,    84,   676,   654,
     655,    85,    86,    87,   461,   656,    88,    89,    90,   657,
     658,   677,   666,    91,    92,   394,   395,   396,   397,   398,
     399,   400,   401,   678,   673,    93,   803,   387,   697,    94,
     698,   608,   683,   607,   696,   701,   691,   387,   704,   705,
     709,   712,   715,   716,   388,   389,   390,   391,   392,    18,
     717,   387,   393,   718,   388,   389,   390,   391,   719,   615,
     721,   387,   393,   788,   789,   796,   790,    19,  -492,  -492,
     390,   391,   800,   797,    18,   804,  -492,   806,   388,   389,
     390,   391,   813,   816,   818,   808,   393,   820,   817,   821,
     833,   835,    20,   836,   849,   840,   394,   395,   396,   397,
     398,   399,   400,   401,   841,   842,   394,   395,   396,   397,
     398,   399,   400,   401,   843,   844,   845,    20,   909,   846,
    -492,   395,   396,   397,   398,   399,   400,   401,   847,   848,
     394,   395,   396,   397,   398,   399,   400,   401,   538,   539,
     540,   541,   542,   543,   544,   545,   546,   547,   548,   549,
     550,   551,   552,   553,   554,   850,   555,   556,   557,   558,
     559,   560,   851,   852,   561,   860,   853,   562,   563,   854,
     855,   564,   565,   566,   567,   733,   734,   735,   736,   737,
     856,   857,   738,   739,   871,   858,   882,   859,   861,   740,
     741,   742,   744,   745,   746,   747,   748,   862,   893,   749,
     750,   863,   864,   865,   904,   743,   751,   752,   753,   755,
     756,   757,   758,   759,   866,   867,   760,   761,   868,   695,
     917,   869,   754,   762,   763,   764,   766,   767,   768,   769,
     770,   870,   872,   771,   772,   873,   919,   920,   874,   765,
     773,   774,   775,   777,   778,   779,   780,   781,   875,   876,
     782,   783,   924,   877,   878,   879,   776,   784,   785,   786,
     337,   338,   339,   340,   341,   342,   343,   344,   345,   346,
     347,   348,   880,   787,   881,   883,   884,   885,   886,   887,
     888,   889,   890,   891,   892,   925,   894,   926,   895,   896,
     897,   898,   899,   927,   900,   928,   901,   902,   903,   929,
     905,   912,   913,   915,   916,   930,   931,   932,   933,   934,
     935,   936,   937,   938,   939,   940,   941,   942,   943,   944,
     946,   945,   947,   948,   949,   950,   951,   952,   953,   954,
     955,   956,   957,   958,   959,   960,   961,   962,   963,   964,
     965,   966,   967,   968,   969,   970,   971,   972,   973,   974,
     975,   976,   977,   978,   979,   980,   981,   982,   983,   984,
     985,   986,   987,   988,   989,   990,   993,   995,   996,   997,
    1002,   381,   999,  1003,  1004,  1005,  1006,  1007,  1008,  1009,
    1010,  1011,  1012,  1013,  1014,  1023,  1015,  1016,  1017,  1018,
    1019,  1020,  1021,  1022,  1024,  1025,  1026,  1027,  1028,  1029,
    1030,  1031,  1032,  1033,  1034,  1035,  1036,  1037,  1038,  1039,
    1040,  1041,  1042,  1043,  1044,  1045,  1046,  1047,  1048,  1049,
    1050,  1051,  1052,  1053,  1054,  1055,  1056,  1057,  1058,  1059,
    1060,  1061,  1062,  1063,  1064,  1065,  1066,  1067,  1068,  1085,
    1069,  1087,  1079,  1071,  1072,  1080,  1081,  1082,  1083,  1084,
    1086,  1089,  1090,  1091,  1097,  1092,  1094,  1101,  1102,  1103,
    1114,  1106,  1113,  1115,  1116,  1117,  1118,  1122,  1123,  1124,
    1125,  1126,  1128,  1127,  1130,   186,   147,   791,   684,   323,
     454,   112,   670,   314,   714,   601,   480,   830,   465,   651,
     906,   810,   793,   811,   306,   362,   834,   908,     0,   507,
     420,     0,     0,   509,     0,     0,     0,   827,   799
};

static const yytype_int16 yycheck[] =
{
     214,    20,   192,   568,   241,     8,   694,   456,   212,   515,
     142,     3,   226,     8,     3,     3,   516,   531,     5,     6,
       4,   223,     6,   531,     5,     6,    54,    61,   242,    14,
      77,    16,    16,     3,    65,   531,   238,   239,   170,     3,
      54,     5,     6,    85,    34,    75,    75,    76,    20,    74,
      22,   531,   531,   531,   151,   152,    28,    60,    82,   151,
     152,    54,    82,    47,   570,   120,    40,     6,    85,     4,
      77,    55,   183,   184,     3,    49,    10,   188,     7,    37,
       9,    10,    11,    12,    13,    14,     6,    16,     4,     8,
      34,    75,    26,    27,    23,    24,    25,    61,   151,   152,
      29,    30,    31,   317,   318,     3,    72,     5,     6,   151,
     152,    95,    96,    97,    98,   212,   330,   124,   806,     3,
     212,     5,     6,    69,    70,    77,    65,   151,   152,   154,
      65,   151,   152,   188,   151,   152,     3,   151,   152,   353,
     354,    60,   126,   127,   128,    65,   336,   349,    77,    65,
     134,   135,   136,    75,   161,   125,   209,   127,   151,   152,
     666,   208,   128,    61,    34,   162,   163,   404,    61,   383,
     384,   168,   124,     4,     5,     6,   210,    61,    71,   208,
     208,   212,   212,   167,   203,     3,   388,   389,   390,   391,
     392,   393,   394,   395,   396,   397,   398,   399,   400,   401,
     164,    34,   187,     8,   718,   151,   152,    86,   708,   161,
     718,   170,   171,   208,   203,   203,   208,   201,     0,   206,
     207,   435,   718,   242,     3,     7,   207,     9,    10,    11,
      12,    13,    14,    15,    16,    17,   165,   209,   718,   718,
     718,    23,    24,    25,     3,     8,     8,    29,    30,    31,
      67,    68,    72,   467,   468,    60,   470,   170,   171,   473,
     450,    78,    79,    80,     8,   129,   164,   457,    99,   100,
     101,     8,   151,   152,   105,    54,    72,   166,     8,   208,
     164,    77,     8,   485,   148,   149,    38,    45,    46,   193,
       8,     8,   196,     3,     8,    77,   486,    60,    60,   211,
     131,   132,   133,     8,    33,   125,   994,   127,   128,   997,
     130,     0,    20,    42,    22,    71,    60,   207,   321,   533,
      28,   207,   212,    60,    71,    77,   212,   833,   532,   125,
      60,   127,   128,   352,    60,   199,   200,   201,   202,   203,
     204,   205,    60,    60,   909,   329,    60,   479,   210,     3,
       4,     5,     6,    40,   207,   156,    43,    44,   207,   212,
       3,    48,    49,   212,   209,   182,   167,   212,   817,   170,
     171,   207,   173,   174,   175,   206,   212,   117,   592,   119,
     594,   121,   122,   165,     3,     4,     5,     6,   189,   190,
       3,     4,     5,     6,    82,   597,    84,  1085,    86,    40,
    1088,   183,    43,    44,   185,   186,   209,    48,    49,   212,
      64,    14,  1100,   615,  1102,    33,    34,    35,    72,    73,
     610,    39,    40,   151,   152,   209,   208,    81,   212,    83,
      55,    56,    57,    58,    59,    60,   209,    62,    63,   212,
     209,   460,   445,   212,     3,    99,   100,   101,    82,     3,
      84,   105,    86,     5,     6,   692,    33,    34,    35,    72,
      73,   209,    39,    40,   212,     3,   209,   486,    81,   212,
      83,     6,   690,    86,   688,   693,     3,   131,   132,   133,
     177,   695,   179,   673,   181,    54,    99,   100,   101,   192,
     193,   194,   105,    49,    50,    51,    52,     5,     6,   153,
       3,     4,     5,     6,    43,    44,    45,    46,   209,   209,
     209,   212,   212,   212,     3,     4,     5,     6,   131,   132,
     133,   173,   525,   177,  1089,   179,   209,   181,   209,   212,
     209,   212,   209,   212,   209,   212,  1101,   212,  1103,   209,
     153,   209,   212,   209,   212,   162,   212,   201,   202,   203,
    1115,   212,   206,   209,   208,   209,   212,     3,     4,     5,
       6,   210,  1127,   212,   177,  1130,   179,     6,   181,    72,
      73,     4,   209,   209,    77,   212,   212,    34,    81,    36,
      83,    45,    46,    72,    73,   155,   156,   157,   201,   202,
     203,   610,    81,   206,    83,   208,    99,   100,   101,   203,
     204,   205,   105,    43,    44,   715,   716,   209,   209,    21,
      99,   100,   101,    64,    66,     3,   105,    72,     3,    61,
     208,     3,   836,    73,     3,     3,    72,    73,   131,   132,
     133,     3,     3,    65,     4,    81,     3,    83,     6,     4,
     208,   161,   131,   132,   133,   835,     3,     4,     5,     6,
     153,     6,     3,    99,   100,   101,     6,     4,    54,   105,
      52,    67,    73,     3,   153,    88,    89,    90,    91,    92,
      61,   208,    95,    96,   177,   198,   179,    77,   181,   102,
     103,   104,    77,   208,   208,   131,   132,   133,   177,     4,
     179,   208,   181,     4,     4,   118,     4,     6,   201,   202,
     203,   208,   208,   206,     3,   208,   920,   153,   210,     6,
      47,    47,   201,   202,   203,    72,    76,   206,    77,   208,
       4,     6,   209,   209,    81,    68,    83,    54,   208,   208,
     208,   177,   208,   179,   208,   181,    77,    33,    34,     4,
      36,   208,    99,   100,   101,    41,    42,   208,   105,   208,
     208,     4,     4,   213,    76,   201,   202,   203,     3,    72,
     206,     7,   208,     9,    10,    11,    12,    13,    14,    15,
      16,    17,   209,   208,   131,   132,   133,    23,    24,    25,
     206,     6,     6,    29,    30,    31,     7,     6,     9,    10,
      11,    12,    13,    14,     5,    16,   153,     3,   208,    43,
     208,   208,    23,    24,    25,   209,     6,   161,    29,    30,
      31,   212,   161,   176,   126,   208,   129,     3,     3,   154,
     177,    75,   179,   212,   181,   209,     4,   151,   208,   212,
     128,    77,   213,   146,   147,   148,   149,   213,     6,     6,
      72,   154,    74,     3,   201,   202,   203,     3,     6,   206,
      72,   208,     4,     4,   212,   208,    77,    32,     6,     6,
     208,   157,   158,   159,   160,   209,   162,   163,     4,   208,
     208,   167,   168,   169,    72,   208,   172,   173,   174,   208,
     208,     3,   208,   179,   180,   198,   199,   200,   201,   202,
     203,   204,   205,     6,   208,   191,   209,   129,     4,   195,
      77,     6,   209,     5,   209,   209,   212,   129,    50,    47,
     198,     6,    56,    56,   146,   147,   148,   149,   150,   165,
       3,   129,   154,   212,   146,   147,   148,   149,    51,   151,
       6,   129,   154,   128,   126,   208,   130,   183,   146,   147,
     148,   149,   209,   161,   165,   206,   154,     4,   146,   147,
     148,   149,   212,   209,   209,   206,   154,     6,   208,     6,
     208,    52,   208,    53,    90,   212,   198,   199,   200,   201,
     202,   203,   204,   205,   212,   212,   198,   199,   200,   201,
     202,   203,   204,   205,   212,   212,   212,   208,     3,   212,
     198,   199,   200,   201,   202,   203,   204,   205,   212,   212,
     198,   199,   200,   201,   202,   203,   204,   205,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   212,   105,   106,   107,   108,
     109,   110,   212,   212,   113,    90,   212,   116,   117,   212,
     212,   120,   121,   122,   123,    88,    89,    90,    91,    92,
     212,   212,    95,    96,    90,   212,    90,   212,   212,   102,
     103,   104,    88,    89,    90,    91,    92,   212,    90,    95,
      96,   212,   212,   212,    90,   118,   102,   103,   104,    88,
      89,    90,    91,    92,   212,   212,    95,    96,   212,    65,
     209,   212,   118,   102,   103,   104,    88,    89,    90,    91,
      92,   212,   212,    95,    96,   212,     6,    61,   212,   118,
     102,   103,   104,    88,    89,    90,    91,    92,   212,   212,
      95,    96,     6,   212,   212,   212,   118,   102,   103,   104,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   212,   118,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,     6,   212,     6,   212,   212,
     212,   212,   212,     6,   212,     6,   212,   212,   212,     6,
     212,   212,   212,   212,   212,     6,     6,     6,     6,   212,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   212,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   212,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   212,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   212,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   212,     6,     4,   209,     4,     4,
     209,   212,     6,   209,   209,   209,   209,   209,   209,   209,
     209,   209,     6,   209,   209,     6,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,     6,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,     6,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,     6,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     6,   209,     4,
     212,     4,   209,   212,   212,   209,   209,   209,   209,   209,
     209,   209,     6,     6,     6,   212,   212,   209,     4,   209,
      48,     6,     6,   209,   209,    40,    40,   208,    40,    40,
       3,   208,     3,   209,   209,   115,    60,   663,   591,   199,
     331,    20,   572,   188,   630,   472,   381,   713,   355,   533,
     790,   693,   665,   693,   169,   233,   718,   796,    -1,   406,
     305,    -1,    -1,   408,    -1,    -1,    -1,   711,   680
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    23,    24,    25,    29,    30,    31,    77,   165,   183,
     208,   215,   216,   217,   219,   230,   231,   233,   234,   237,
     238,   239,   240,   241,   264,   269,   270,   271,   272,   273,
     274,   275,    33,    34,    35,    39,    40,    37,    33,    34,
      35,    39,    40,     3,   262,    75,   262,   162,   163,   168,
       3,   218,   219,   230,   231,   234,   237,   238,   239,   269,
     270,   271,   272,   273,    33,    34,    36,    41,    42,   157,
     158,   159,   160,   162,   163,   167,   168,   169,   172,   173,
     174,   179,   180,   191,   195,    34,    34,    34,    34,   155,
     156,   157,     3,     3,   262,     3,   265,   266,   166,    14,
      16,   187,   240,   241,     0,   211,   322,    20,    22,    28,
     258,     8,   242,   244,    71,   321,   321,   321,   321,   321,
     323,   262,    71,   320,   320,   320,   320,   320,   210,    14,
     262,    75,    76,   208,     3,     3,     3,   218,     3,   262,
       6,   170,   171,   170,   171,     3,   173,   162,   192,   193,
     194,   193,   196,   262,   262,   262,   262,    61,    54,   212,
       6,   183,   184,   188,   156,   167,   170,   171,   173,   174,
     175,   189,   190,     4,   209,   209,   217,    21,   208,   243,
     244,    64,   251,    66,   245,    72,     3,   262,   262,   262,
       3,    61,   208,   232,    73,     3,   262,   262,   262,     3,
       3,     3,   235,   236,    65,   255,     4,   319,   319,     3,
       4,     5,     6,    72,    73,    81,    83,    99,   100,   101,
     105,   131,   132,   133,   153,   177,   179,   181,   201,   202,
     203,   206,   208,   276,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   289,   290,   291,   292,   293,   295,   296,
     297,   298,   299,   301,   302,   303,   304,   305,   306,   307,
     308,   311,   312,   313,   314,   315,   316,     3,     5,     6,
      61,   164,     3,     5,     6,    61,   164,     3,     5,     6,
      61,   164,    40,    43,    44,    48,    49,     3,     3,     6,
       4,    10,    26,    27,   262,   208,   266,   319,   161,     6,
       3,     6,     4,    54,   243,   244,   276,    52,    67,   249,
      73,    54,   208,   232,   262,     3,   229,    38,   241,    61,
     198,   212,   255,   279,    77,    77,   208,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,    72,
      73,   280,   208,   208,    86,   279,   294,     4,     4,     4,
       4,     6,   316,   208,   117,   119,   121,   122,   208,   208,
     280,   280,     5,     6,   207,   299,   309,   310,   241,   279,
     209,   212,    54,   151,   152,    72,    74,   129,   146,   147,
     148,   149,   150,   154,   198,   199,   200,   201,   202,   203,
     204,   205,   210,   207,   212,   207,   212,   207,   212,   207,
     212,   207,   212,     3,     6,    47,    47,    76,    77,   324,
     242,     4,    40,    49,     6,   185,   186,   209,   209,    75,
     252,   246,   247,   279,   279,    68,   250,   239,     3,   125,
     127,   220,   222,   223,   228,    54,   208,   328,   209,   212,
     208,   277,   262,   279,   236,   208,   208,    64,   209,   276,
     208,    72,   241,   279,   279,   294,    82,    84,    86,     4,
     208,   208,   208,   208,     4,     4,   213,   209,   209,    76,
     278,     3,   279,   279,    74,   154,   208,    72,   128,   280,
     280,   280,   280,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,     3,   203,   299,     6,   309,     6,   310,
       6,     5,    43,    45,    46,   208,   208,     3,   208,   209,
       6,    33,    42,   161,   161,   208,   259,   260,   261,   262,
     267,   176,   253,   212,    69,    70,   248,   279,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   105,   106,   107,   108,   109,
     110,   113,   116,   117,   120,   121,   122,   123,   224,   126,
     208,   209,   212,   239,     3,     3,   276,   212,    67,    68,
      78,    79,    80,   182,   317,   318,   317,   276,   209,   241,
     209,    54,    85,    82,    84,   279,   279,    75,   279,     4,
       3,   297,   279,   212,   254,   209,   212,     5,     6,   319,
     208,   280,   241,   276,   128,   151,   213,   213,     6,     6,
     229,   221,   223,     3,   326,   327,     6,     4,     4,   239,
     212,    55,    57,    58,    59,    60,    62,    63,   268,     3,
      54,   263,   281,   282,   283,   284,   285,   286,   287,   288,
     255,   247,   208,   208,   208,   208,   208,   208,   208,    72,
     125,   127,   128,   225,   226,   324,   208,   229,    32,   325,
     222,   209,   209,   208,     6,     6,     4,     3,     6,   209,
     212,   209,   209,   209,   224,   279,   279,    82,    85,   280,
     212,   212,   212,   212,     4,    65,   209,     4,    77,   241,
     276,   209,   209,   280,    50,    47,   209,   209,   212,   198,
     209,   212,     6,   209,   260,    56,    56,     3,   212,    51,
     257,     6,    88,    89,    90,    91,    92,    95,    96,   102,
     103,   104,   118,    88,    89,    90,    91,    92,    95,    96,
     102,   103,   104,   118,    88,    89,    90,    91,    92,    95,
      96,   102,   103,   104,   118,    88,    89,    90,    91,    92,
      95,    96,   102,   103,   104,   118,    88,    89,    90,    91,
      92,    95,    96,   102,   103,   104,   118,    88,    89,    90,
      91,    92,    95,    96,   102,   103,   104,   118,   128,   126,
     130,   226,   227,   227,   229,   209,   208,   161,   276,   318,
     209,    82,   279,   209,   206,   311,     4,   299,   206,   300,
     303,   308,   311,   212,   254,   279,   209,   208,   209,   209,
       6,     6,   223,     3,     4,     5,     6,   327,    34,    36,
     263,   261,   261,   208,   287,    52,    53,   256,   209,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,    90,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
      90,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,    90,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,    90,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,    90,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,    90,   212,   298,   209,   326,     3,
     209,     6,   212,   212,   254,   212,   212,   209,   317,     6,
      61,   229,   276,   279,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   212,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   212,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   212,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   212,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   212,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   212,
       6,   209,   324,     4,     4,   209,     4,     4,   209,     6,
     279,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   209,     6,   209,   209,   209,   209,   209,   209,   209,
     209,   209,   209,     6,   209,   209,   209,   209,   209,   209,
     209,   209,   209,   209,     6,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,     6,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,     6,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,     6,   209,   212,
     254,   212,   212,   254,    40,    43,    44,    48,    49,   209,
     209,   209,   209,   209,   209,     4,   209,     4,     6,   209,
       6,     6,   212,   254,   212,   254,   324,     6,    45,    46,
       6,   209,     4,   209,    43,    44,     6,   254,   324,   254,
     124,   161,   324,     6,    48,   209,   209,    40,    40,   124,
     161,   324,   208,    40,    40,     3,   208,   209,     3,   324,
     209,   324
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   214,   215,   216,   216,   217,   217,   217,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     218,   218,   218,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   219,   219,   219,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   223,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   225,   225,
     226,   226,   226,   226,   227,   227,   228,   228,   229,   229,
     230,   231,   231,   232,   232,   233,   233,   234,   235,   235,
     236,   237,   237,   237,   237,   237,   238,   238,   238,   239,
     239,   239,   239,   240,   240,   241,   242,   243,   243,   244,
     245,   245,   246,   246,   247,   248,   248,   248,   249,   249,
     250,   250,   251,   251,   252,   252,   253,   253,   254,   254,
     255,   255,   256,   256,   257,   257,   258,   258,   258,   258,
     259,   259,   260,   260,   261,   261,   262,   262,   263,   263,
     263,   263,   264,   264,   265,   265,   266,   267,   267,   268,
     268,   268,   268,   268,   268,   268,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   270,   270,   270,   271,   271,   272,   272,
     272,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   272,   272,   272,   272,   272,   272,   273,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   275,
     275,   275,   276,   276,   277,   277,   278,   278,   279,   279,
     279,   279,   279,   280,   280,   280,   280,   280,   280,   280,
     280,   280,   280,   280,   280,   280,   281,   282,   282,   282,
     282,   283,   283,   283,   283,   284,   284,   285,   285,   286,
     286,   287,   287,   287,   287,   287,   287,   288,   288,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   289,   289,   289,   289,   289,   289,   289,   289,
     289,   289,   290,   290,   291,   292,   292,   293,   293,   293,
     293,   294,   294,   295,   296,   296,   296,   296,   297,   297,
     297,   297,   298,   298,   298,   298,   298,   298,   298,   298,
     298,   298,   298,   298,   299,   299,   299,   299,   300,   300,
     300,   301,   302,   302,   303,   303,   304,   305,   305,   306,
     307,   307,   308,   309,   310,   311,   311,   312,   313,   313,
     314,   315,   315,   316,   316,   316,   316,   316,   316,   316,
     316,   316,   316,   316,   316,   317,   317,   318,   318,   318,
     318,   318,   318,   319,   320,   320,   321,   321,   322,   322,
     323,   323,   324,   324,   325,   325,   326,   326,   327,   327,
     327,   327,   327,   328,   328
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
       4,     6,     5,     3,     0,     3,     2,     5,     1,     3,
       3,     4,     4,     4,     4,     6,     8,    11,     8,     1,
       1,     3,     3,     3,     3,     2,     4,     3,     3,     8,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     3,     0,
       2,     0,     2,     0,     3,     0,     1,     2,     1,     1,
       1,     3,     1,     1,     2,     4,     1,     3,     2,     1,
       5,     0,     2,     0,     1,     3,     5,     4,     6,     1,
       1,     1,     1,     1,     1,     0,     2,     2,     2,     2,
       3,     2,     2,     2,     2,     4,     2,     3,     3,     3,
       4,     4,     3,     3,     4,     4,     5,     6,     7,     9,
       4,     5,     7,     9,     2,     3,     2,     3,     3,     4,
       2,     3,     3,     2,     2,     2,     2,     5,     2,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     3,     3,     3,     3,     4,
       6,     7,     9,    10,    12,    12,    13,    14,    15,    16,
      12,    13,    15,    16,     3,     4,     5,     6,     3,     3,
       4,     3,     4,     3,     3,     3,     5,     7,     7,     6,
       8,     8,     1,     3,     3,     5,     3,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,    14,    20,    16,    15,
      13,    18,    14,    13,    11,     8,    10,     5,     7,     4,
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
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2371 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2379 "parser.cpp"
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
#line 2393 "parser.cpp"
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
#line 2407 "parser.cpp"
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
#line 2421 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2432 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2449 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2458 "parser.cpp"
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
#line 2472 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2493 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2513 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2533 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 359 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2543 "parser.cpp"
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
#line 2557 "parser.cpp"
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
#line 2571 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 347 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2581 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2589 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2597 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2606 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2614 "parser.cpp"
        break;

    case YYSYMBOL_optional_search_filter_expr: /* optional_search_filter_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2622 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2630 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2638 "parser.cpp"
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
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2661 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2670 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2679 "parser.cpp"
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
#line 2692 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 337 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2701 "parser.cpp"
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
#line 2715 "parser.cpp"
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
#line 2729 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 353 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2739 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2748 "parser.cpp"
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
#line 2762 "parser.cpp"
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
#line 2779 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2787 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2795 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2803 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2811 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2819 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2827 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2835 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2843 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2851 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2859 "parser.cpp"
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
#line 2873 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2881 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2889 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2897 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2905 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2913 "parser.cpp"
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
#line 2926 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2934 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2942 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 329 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2950 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2958 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2966 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2974 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2982 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2990 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2998 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3006 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3014 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3022 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3030 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3038 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 374 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 3046 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 378 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3054 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3062 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3070 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3078 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3086 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3094 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 333 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3102 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 320 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3110 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 207 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3121 "parser.cpp"
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
#line 3135 "parser.cpp"
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
#line 3149 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 183 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3160 "parser.cpp"
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

#line 3268 "parser.cpp"

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
#line 3483 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 510 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3494 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 516 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3505 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 523 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3511 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 524 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3517 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 525 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3523 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 526 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3529 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 527 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3535 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 528 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3541 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 529 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3547 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3553 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 531 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3559 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 532 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3565 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 533 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3571 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 534 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3577 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 535 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3583 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3589 "parser.cpp"
    break;

  case 19: /* statement: alter_statement  */
#line 537 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].alter_stmt); }
#line 3595 "parser.cpp"
    break;

  case 20: /* explainable_statement: create_statement  */
#line 539 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3601 "parser.cpp"
    break;

  case 21: /* explainable_statement: drop_statement  */
#line 540 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3607 "parser.cpp"
    break;

  case 22: /* explainable_statement: copy_statement  */
#line 541 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3613 "parser.cpp"
    break;

  case 23: /* explainable_statement: show_statement  */
#line 542 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3619 "parser.cpp"
    break;

  case 24: /* explainable_statement: select_statement  */
#line 543 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3625 "parser.cpp"
    break;

  case 25: /* explainable_statement: delete_statement  */
#line 544 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3631 "parser.cpp"
    break;

  case 26: /* explainable_statement: update_statement  */
#line 545 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3637 "parser.cpp"
    break;

  case 27: /* explainable_statement: insert_statement  */
#line 546 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3643 "parser.cpp"
    break;

  case 28: /* explainable_statement: flush_statement  */
#line 547 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3649 "parser.cpp"
    break;

  case 29: /* explainable_statement: optimize_statement  */
#line 548 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3655 "parser.cpp"
    break;

  case 30: /* explainable_statement: command_statement  */
#line 549 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3661 "parser.cpp"
    break;

  case 31: /* explainable_statement: compact_statement  */
#line 550 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3667 "parser.cpp"
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
#line 3687 "parser.cpp"
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
#line 3705 "parser.cpp"
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
#line 3738 "parser.cpp"
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
#line 3758 "parser.cpp"
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
#line 3779 "parser.cpp"
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
#line 3812 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element  */
#line 682 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3821 "parser.cpp"
    break;

  case 39: /* table_element_array: table_element_array ',' table_element  */
#line 686 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3830 "parser.cpp"
    break;

  case 40: /* column_def_array: table_column  */
#line 691 "parser.y"
                                {
    (yyval.column_def_array_t) = new std::vector<infinity::ColumnDef*>();
    (yyval.column_def_array_t)->push_back((yyvsp[0].table_column_t));
}
#line 3839 "parser.cpp"
    break;

  case 41: /* column_def_array: column_def_array ',' table_column  */
#line 695 "parser.y"
                                    {
    (yyvsp[-2].column_def_array_t)->push_back((yyvsp[0].table_column_t));
    (yyval.column_def_array_t) = (yyvsp[-2].column_def_array_t);
}
#line 3848 "parser.cpp"
    break;

  case 42: /* table_element: table_column  */
#line 701 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3856 "parser.cpp"
    break;

  case 43: /* table_element: table_constraint  */
#line 704 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3864 "parser.cpp"
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
#line 3920 "parser.cpp"
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
#line 3962 "parser.cpp"
    break;

  case 46: /* column_type: BOOLEAN  */
#line 801 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3968 "parser.cpp"
    break;

  case 47: /* column_type: TINYINT  */
#line 802 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3974 "parser.cpp"
    break;

  case 48: /* column_type: SMALLINT  */
#line 803 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3980 "parser.cpp"
    break;

  case 49: /* column_type: INTEGER  */
#line 804 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3986 "parser.cpp"
    break;

  case 50: /* column_type: INT  */
#line 805 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3992 "parser.cpp"
    break;

  case 51: /* column_type: BIGINT  */
#line 806 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3998 "parser.cpp"
    break;

  case 52: /* column_type: HUGEINT  */
#line 807 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4004 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT  */
#line 808 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4010 "parser.cpp"
    break;

  case 54: /* column_type: REAL  */
#line 809 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4016 "parser.cpp"
    break;

  case 55: /* column_type: DOUBLE  */
#line 810 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4022 "parser.cpp"
    break;

  case 56: /* column_type: FLOAT16  */
#line 811 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4028 "parser.cpp"
    break;

  case 57: /* column_type: BFLOAT16  */
#line 812 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4034 "parser.cpp"
    break;

  case 58: /* column_type: DATE  */
#line 813 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4040 "parser.cpp"
    break;

  case 59: /* column_type: TIME  */
#line 814 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4046 "parser.cpp"
    break;

  case 60: /* column_type: DATETIME  */
#line 815 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4052 "parser.cpp"
    break;

  case 61: /* column_type: TIMESTAMP  */
#line 816 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4058 "parser.cpp"
    break;

  case 62: /* column_type: UUID  */
#line 817 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4064 "parser.cpp"
    break;

  case 63: /* column_type: POINT  */
#line 818 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4070 "parser.cpp"
    break;

  case 64: /* column_type: LINE  */
#line 819 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4076 "parser.cpp"
    break;

  case 65: /* column_type: LSEG  */
#line 820 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4082 "parser.cpp"
    break;

  case 66: /* column_type: BOX  */
#line 821 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4088 "parser.cpp"
    break;

  case 67: /* column_type: CIRCLE  */
#line 824 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4094 "parser.cpp"
    break;

  case 68: /* column_type: VARCHAR  */
#line 826 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4100 "parser.cpp"
    break;

  case 69: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4106 "parser.cpp"
    break;

  case 70: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 828 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4112 "parser.cpp"
    break;

  case 71: /* column_type: DECIMAL  */
#line 829 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4118 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4124 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4130 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4136 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4142 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4148 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4154 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4160 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4166 "parser.cpp"
    break;

  case 80: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4172 "parser.cpp"
    break;

  case 81: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4178 "parser.cpp"
    break;

  case 82: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4184 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4190 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4196 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4202 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4208 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4214 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4220 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4226 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4232 "parser.cpp"
    break;

  case 91: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4238 "parser.cpp"
    break;

  case 92: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4244 "parser.cpp"
    break;

  case 93: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4250 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4256 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4262 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4268 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4274 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4280 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4286 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4292 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4298 "parser.cpp"
    break;

  case 102: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4304 "parser.cpp"
    break;

  case 103: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4310 "parser.cpp"
    break;

  case 104: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4316 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4322 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4328 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4334 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4340 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4346 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4352 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4358 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4364 "parser.cpp"
    break;

  case 113: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4370 "parser.cpp"
    break;

  case 114: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4376 "parser.cpp"
    break;

  case 115: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4382 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 876 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4388 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 877 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4394 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 878 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4400 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 879 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4406 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 880 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4412 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 881 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4418 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 882 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4424 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 883 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4430 "parser.cpp"
    break;

  case 124: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 884 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4436 "parser.cpp"
    break;

  case 125: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 885 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4442 "parser.cpp"
    break;

  case 126: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 886 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4448 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 887 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4454 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 888 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4460 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 889 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4466 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 890 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4472 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 891 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4478 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 892 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4484 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 893 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4490 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 894 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4496 "parser.cpp"
    break;

  case 135: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 895 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4502 "parser.cpp"
    break;

  case 136: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 896 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4508 "parser.cpp"
    break;

  case 137: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 897 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4514 "parser.cpp"
    break;

  case 138: /* column_constraints: column_constraint  */
#line 916 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4523 "parser.cpp"
    break;

  case 139: /* column_constraints: column_constraints column_constraint  */
#line 920 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4537 "parser.cpp"
    break;

  case 140: /* column_constraint: PRIMARY KEY  */
#line 930 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4545 "parser.cpp"
    break;

  case 141: /* column_constraint: UNIQUE  */
#line 933 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4553 "parser.cpp"
    break;

  case 142: /* column_constraint: NULLABLE  */
#line 936 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4561 "parser.cpp"
    break;

  case 143: /* column_constraint: NOT NULLABLE  */
#line 939 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4569 "parser.cpp"
    break;

  case 144: /* default_expr: DEFAULT constant_expr  */
#line 943 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4577 "parser.cpp"
    break;

  case 145: /* default_expr: %empty  */
#line 946 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4585 "parser.cpp"
    break;

  case 146: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 951 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4595 "parser.cpp"
    break;

  case 147: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 956 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4605 "parser.cpp"
    break;

  case 148: /* identifier_array: IDENTIFIER  */
#line 963 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4616 "parser.cpp"
    break;

  case 149: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 969 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4627 "parser.cpp"
    break;

  case 150: /* delete_statement: DELETE FROM table_name where_clause  */
#line 979 "parser.y"
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
#line 4644 "parser.cpp"
    break;

  case 151: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 995 "parser.y"
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
#line 4683 "parser.cpp"
    break;

  case 152: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 1029 "parser.y"
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
#line 4700 "parser.cpp"
    break;

  case 153: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1042 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4708 "parser.cpp"
    break;

  case 154: /* optional_identifier_array: %empty  */
#line 1045 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4716 "parser.cpp"
    break;

  case 155: /* explain_statement: EXPLAIN IDENTIFIER explainable_statement  */
#line 1052 "parser.y"
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
#line 4734 "parser.cpp"
    break;

  case 156: /* explain_statement: EXPLAIN explainable_statement  */
#line 1064 "parser.y"
                                  {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ =infinity::ExplainType::kPhysical;
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4744 "parser.cpp"
    break;

  case 157: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1073 "parser.y"
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
#line 4761 "parser.cpp"
    break;

  case 158: /* update_expr_array: update_expr  */
#line 1086 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4770 "parser.cpp"
    break;

  case 159: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1090 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4779 "parser.cpp"
    break;

  case 160: /* update_expr: IDENTIFIER '=' expr  */
#line 1095 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4791 "parser.cpp"
    break;

  case 161: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1108 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4807 "parser.cpp"
    break;

  case 162: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1121 "parser.y"
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
#line 4825 "parser.cpp"
    break;

  case 163: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1136 "parser.y"
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
#line 4843 "parser.cpp"
    break;

  case 164: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1151 "parser.y"
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
#line 4861 "parser.cpp"
    break;

  case 165: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1166 "parser.y"
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
#line 4884 "parser.cpp"
    break;

  case 166: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1189 "parser.y"
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
#line 4942 "parser.cpp"
    break;

  case 167: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1242 "parser.y"
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
#line 5002 "parser.cpp"
    break;

  case 168: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1297 "parser.y"
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
#line 5054 "parser.cpp"
    break;

  case 169: /* select_statement: select_without_paren  */
#line 1348 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5062 "parser.cpp"
    break;

  case 170: /* select_statement: select_with_paren  */
#line 1351 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5070 "parser.cpp"
    break;

  case 171: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
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
#line 5084 "parser.cpp"
    break;

  case 172: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1363 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 5098 "parser.cpp"
    break;

  case 173: /* select_with_paren: '(' select_without_paren ')'  */
#line 1373 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5106 "parser.cpp"
    break;

  case 174: /* select_with_paren: '(' select_with_paren ')'  */
#line 1376 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5114 "parser.cpp"
    break;

  case 175: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1380 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5123 "parser.cpp"
    break;

  case 176: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1385 "parser.y"
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
#line 5154 "parser.cpp"
    break;

  case 177: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1412 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5162 "parser.cpp"
    break;

  case 178: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1415 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5170 "parser.cpp"
    break;

  case 179: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1420 "parser.y"
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
#line 5190 "parser.cpp"
    break;

  case 180: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1436 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5198 "parser.cpp"
    break;

  case 181: /* order_by_clause: %empty  */
#line 1439 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5206 "parser.cpp"
    break;

  case 182: /* order_by_expr_list: order_by_expr  */
#line 1443 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5215 "parser.cpp"
    break;

  case 183: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1447 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5224 "parser.cpp"
    break;

  case 184: /* order_by_expr: expr order_by_type  */
#line 1452 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5234 "parser.cpp"
    break;

  case 185: /* order_by_type: ASC  */
#line 1458 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5242 "parser.cpp"
    break;

  case 186: /* order_by_type: DESC  */
#line 1461 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5250 "parser.cpp"
    break;

  case 187: /* order_by_type: %empty  */
#line 1464 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5258 "parser.cpp"
    break;

  case 188: /* limit_expr: LIMIT expr  */
#line 1468 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5266 "parser.cpp"
    break;

  case 189: /* limit_expr: %empty  */
#line 1472 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5272 "parser.cpp"
    break;

  case 190: /* offset_expr: OFFSET expr  */
#line 1474 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5280 "parser.cpp"
    break;

  case 191: /* offset_expr: %empty  */
#line 1478 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5286 "parser.cpp"
    break;

  case 192: /* distinct: DISTINCT  */
#line 1480 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5294 "parser.cpp"
    break;

  case 193: /* distinct: %empty  */
#line 1483 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5302 "parser.cpp"
    break;

  case 194: /* from_clause: FROM table_reference  */
#line 1487 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5310 "parser.cpp"
    break;

  case 195: /* from_clause: %empty  */
#line 1490 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5318 "parser.cpp"
    break;

  case 196: /* search_clause: SEARCH sub_search_array  */
#line 1494 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5328 "parser.cpp"
    break;

  case 197: /* search_clause: %empty  */
#line 1499 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5336 "parser.cpp"
    break;

  case 198: /* optional_search_filter_expr: ',' WHERE expr  */
#line 1503 "parser.y"
                                            {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5344 "parser.cpp"
    break;

  case 199: /* optional_search_filter_expr: %empty  */
#line 1506 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5352 "parser.cpp"
    break;

  case 200: /* where_clause: WHERE expr  */
#line 1510 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5360 "parser.cpp"
    break;

  case 201: /* where_clause: %empty  */
#line 1513 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5368 "parser.cpp"
    break;

  case 202: /* having_clause: HAVING expr  */
#line 1517 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5376 "parser.cpp"
    break;

  case 203: /* having_clause: %empty  */
#line 1520 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5384 "parser.cpp"
    break;

  case 204: /* group_by_clause: GROUP BY expr_array  */
#line 1524 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5392 "parser.cpp"
    break;

  case 205: /* group_by_clause: %empty  */
#line 1527 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5400 "parser.cpp"
    break;

  case 206: /* set_operator: UNION  */
#line 1531 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5408 "parser.cpp"
    break;

  case 207: /* set_operator: UNION ALL  */
#line 1534 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5416 "parser.cpp"
    break;

  case 208: /* set_operator: INTERSECT  */
#line 1537 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5424 "parser.cpp"
    break;

  case 209: /* set_operator: EXCEPT  */
#line 1540 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5432 "parser.cpp"
    break;

  case 210: /* table_reference: table_reference_unit  */
#line 1548 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5440 "parser.cpp"
    break;

  case 211: /* table_reference: table_reference ',' table_reference_unit  */
#line 1551 "parser.y"
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
#line 5458 "parser.cpp"
    break;

  case 214: /* table_reference_name: table_name table_alias  */
#line 1568 "parser.y"
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
#line 5476 "parser.cpp"
    break;

  case 215: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1582 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5487 "parser.cpp"
    break;

  case 216: /* table_name: IDENTIFIER  */
#line 1591 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5497 "parser.cpp"
    break;

  case 217: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1596 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5509 "parser.cpp"
    break;

  case 218: /* table_alias: AS IDENTIFIER  */
#line 1605 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5519 "parser.cpp"
    break;

  case 219: /* table_alias: IDENTIFIER  */
#line 1610 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5529 "parser.cpp"
    break;

  case 220: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1615 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5540 "parser.cpp"
    break;

  case 221: /* table_alias: %empty  */
#line 1621 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5548 "parser.cpp"
    break;

  case 222: /* with_clause: WITH with_expr_list  */
#line 1628 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5556 "parser.cpp"
    break;

  case 223: /* with_clause: %empty  */
#line 1631 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5564 "parser.cpp"
    break;

  case 224: /* with_expr_list: with_expr  */
#line 1635 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5573 "parser.cpp"
    break;

  case 225: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1638 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5582 "parser.cpp"
    break;

  case 226: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1643 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5594 "parser.cpp"
    break;

  case 227: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1655 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5606 "parser.cpp"
    break;

  case 228: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1662 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5619 "parser.cpp"
    break;

  case 229: /* join_type: INNER  */
#line 1676 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5627 "parser.cpp"
    break;

  case 230: /* join_type: LEFT  */
#line 1679 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5635 "parser.cpp"
    break;

  case 231: /* join_type: RIGHT  */
#line 1682 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5643 "parser.cpp"
    break;

  case 232: /* join_type: OUTER  */
#line 1685 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5651 "parser.cpp"
    break;

  case 233: /* join_type: FULL  */
#line 1688 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5659 "parser.cpp"
    break;

  case 234: /* join_type: CROSS  */
#line 1691 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5667 "parser.cpp"
    break;

  case 235: /* join_type: %empty  */
#line 1694 "parser.y"
                {
}
#line 5674 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW DATABASES  */
#line 1700 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5683 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW TABLES  */
#line 1704 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5692 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW VIEWS  */
#line 1708 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5701 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW CONFIGS  */
#line 1712 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5710 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1716 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5722 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW PROFILES  */
#line 1723 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5731 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW BUFFER  */
#line 1727 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5740 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW MEMINDEX  */
#line 1731 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5749 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW QUERIES  */
#line 1735 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5758 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1739 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5768 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW TRANSACTIONS  */
#line 1744 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5777 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1748 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5787 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW SESSION VARIABLES  */
#line 1753 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5796 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1757 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5805 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1761 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5816 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1767 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5827 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1773 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5838 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW TABLE table_name  */
#line 1779 "parser.y"
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
#line 5854 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1790 "parser.y"
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
#line 5870 "parser.cpp"
    break;

  case 255: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1801 "parser.y"
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
#line 5886 "parser.cpp"
    break;

  case 256: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1812 "parser.y"
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
#line 5903 "parser.cpp"
    break;

  case 257: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1824 "parser.y"
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
#line 5920 "parser.cpp"
    break;

  case 258: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1836 "parser.y"
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
#line 5938 "parser.cpp"
    break;

  case 259: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1849 "parser.y"
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
#line 5957 "parser.cpp"
    break;

  case 260: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1863 "parser.y"
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
#line 5973 "parser.cpp"
    break;

  case 261: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1874 "parser.y"
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
#line 5992 "parser.cpp"
    break;

  case 262: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1888 "parser.y"
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
#line 6013 "parser.cpp"
    break;

  case 263: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1904 "parser.y"
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
#line 6035 "parser.cpp"
    break;

  case 264: /* show_statement: SHOW LOGS  */
#line 1921 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 6044 "parser.cpp"
    break;

  case 265: /* show_statement: SHOW DELTA LOGS  */
#line 1925 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6053 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW CATALOGS  */
#line 1929 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6062 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1933 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6071 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1937 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6080 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1941 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6091 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW MEMORY  */
#line 1947 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6100 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1951 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6109 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1955 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6118 "parser.cpp"
    break;

  case 273: /* flush_statement: FLUSH DATA  */
#line 1963 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6127 "parser.cpp"
    break;

  case 274: /* flush_statement: FLUSH LOG  */
#line 1967 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6136 "parser.cpp"
    break;

  case 275: /* flush_statement: FLUSH BUFFER  */
#line 1971 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6145 "parser.cpp"
    break;

  case 276: /* optimize_statement: OPTIMIZE table_name  */
#line 1979 "parser.y"
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
#line 6160 "parser.cpp"
    break;

  case 277: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1990 "parser.y"
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
#line 6184 "parser.cpp"
    break;

  case 278: /* command_statement: USE IDENTIFIER  */
#line 2013 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6195 "parser.cpp"
    break;

  case 279: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 2019 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6205 "parser.cpp"
    break;

  case 280: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2024 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6216 "parser.cpp"
    break;

  case 281: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2030 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6227 "parser.cpp"
    break;

  case 282: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 2036 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6240 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2044 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6251 "parser.cpp"
    break;

  case 284: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2050 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6262 "parser.cpp"
    break;

  case 285: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2056 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6273 "parser.cpp"
    break;

  case 286: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2062 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6284 "parser.cpp"
    break;

  case 287: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2068 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6297 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2076 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6308 "parser.cpp"
    break;

  case 289: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2082 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6319 "parser.cpp"
    break;

  case 290: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2088 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6330 "parser.cpp"
    break;

  case 291: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2094 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6341 "parser.cpp"
    break;

  case 292: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2100 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6354 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2108 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6365 "parser.cpp"
    break;

  case 294: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2114 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6376 "parser.cpp"
    break;

  case 295: /* command_statement: LOCK TABLE table_name  */
#line 2120 "parser.y"
                        {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::LockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6390 "parser.cpp"
    break;

  case 296: /* command_statement: UNLOCK TABLE table_name  */
#line 2129 "parser.y"
                          {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].table_name_t)->schema_name_ptr_);
    ParserHelper::ToLower((yyvsp[0].table_name_t)->table_name_ptr_);
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UnlockCmd>((yyvsp[0].table_name_t)->schema_name_ptr_, (yyvsp[0].table_name_t)->table_name_ptr_);
    free((yyvsp[0].table_name_t)->schema_name_ptr_);
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 6404 "parser.cpp"
    break;

  case 297: /* compact_statement: COMPACT TABLE table_name  */
#line 2139 "parser.y"
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
#line 6421 "parser.cpp"
    break;

  case 298: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2152 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6430 "parser.cpp"
    break;

  case 299: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2156 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6440 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2161 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6451 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2167 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6463 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2174 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6476 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2182 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6490 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2191 "parser.y"
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
#line 6505 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2201 "parser.y"
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
#line 6520 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2211 "parser.y"
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
#line 6536 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2222 "parser.y"
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
#line 6552 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2233 "parser.y"
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
#line 6569 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2245 "parser.y"
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
#line 6586 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2257 "parser.y"
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
#line 6601 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2267 "parser.y"
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
#line 6617 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2278 "parser.y"
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
#line 6634 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2290 "parser.y"
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
#line 6652 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW LOGS  */
#line 2303 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6661 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2307 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6671 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2312 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6681 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2317 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6692 "parser.cpp"
    break;

  case 318: /* admin_statement: ADMIN SHOW CONFIGS  */
#line 2323 "parser.y"
                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListConfigs;
}
#line 6701 "parser.cpp"
    break;

  case 319: /* admin_statement: ADMIN SHOW VARIABLES  */
#line 2327 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListVariables;
}
#line 6710 "parser.cpp"
    break;

  case 320: /* admin_statement: ADMIN SHOW VARIABLE IDENTIFIER  */
#line 2331 "parser.y"
                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowVariable;
     (yyval.admin_stmt)->variable_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6721 "parser.cpp"
    break;

  case 321: /* admin_statement: ADMIN SHOW NODES  */
#line 2337 "parser.y"
                   {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListNodes;
}
#line 6730 "parser.cpp"
    break;

  case 322: /* admin_statement: ADMIN SHOW NODE STRING  */
#line 2341 "parser.y"
                         {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowNode;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6741 "parser.cpp"
    break;

  case 323: /* admin_statement: ADMIN SHOW NODE  */
#line 2347 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCurrentNode;
}
#line 6750 "parser.cpp"
    break;

  case 324: /* admin_statement: ADMIN SET ADMIN  */
#line 2351 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kAdmin;
}
#line 6760 "parser.cpp"
    break;

  case 325: /* admin_statement: ADMIN SET STANDALONE  */
#line 2356 "parser.y"
                       {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kStandalone;
}
#line 6770 "parser.cpp"
    break;

  case 326: /* admin_statement: ADMIN SET LEADER USING STRING  */
#line 2361 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLeader;
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[0].str_value));
}
#line 6782 "parser.cpp"
    break;

  case 327: /* admin_statement: ADMIN CONNECT STRING AS FOLLOWER USING STRING  */
#line 2368 "parser.y"
                                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kFollower;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6796 "parser.cpp"
    break;

  case 328: /* admin_statement: ADMIN CONNECT STRING AS LEARNER USING STRING  */
#line 2377 "parser.y"
                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kSetRole;
     (yyval.admin_stmt)->admin_node_role_ = infinity::AdminNodeRole::kLearner;
     (yyval.admin_stmt)->leader_address_ = (yyvsp[-4].str_value);
     (yyval.admin_stmt)->node_name_ = (yyvsp[0].str_value);
     free((yyvsp[-4].str_value));
     free((yyvsp[0].str_value));
}
#line 6810 "parser.cpp"
    break;

  case 329: /* alter_statement: ALTER TABLE table_name RENAME TO IDENTIFIER  */
#line 2387 "parser.y"
                                                              {
    auto *ret = new infinity::RenameTableStatement((yyvsp[-3].table_name_t)->schema_name_ptr_, (yyvsp[-3].table_name_t)->table_name_ptr_);
    (yyval.alter_stmt) = ret;
    ret->new_table_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
    free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);
}
#line 6824 "parser.cpp"
    break;

  case 330: /* alter_statement: ALTER TABLE table_name ADD COLUMN '(' column_def_array ')'  */
#line 2396 "parser.y"
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
#line 6841 "parser.cpp"
    break;

  case 331: /* alter_statement: ALTER TABLE table_name DROP COLUMN '(' identifier_array ')'  */
#line 2408 "parser.y"
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
#line 6857 "parser.cpp"
    break;

  case 332: /* expr_array: expr_alias  */
#line 2424 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6866 "parser.cpp"
    break;

  case 333: /* expr_array: expr_array ',' expr_alias  */
#line 2428 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6875 "parser.cpp"
    break;

  case 334: /* expr_array_list: '(' expr_array ')'  */
#line 2433 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6884 "parser.cpp"
    break;

  case 335: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2437 "parser.y"
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
#line 6910 "parser.cpp"
    break;

  case 336: /* expr_alias: expr AS IDENTIFIER  */
#line 2470 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6921 "parser.cpp"
    break;

  case 337: /* expr_alias: expr  */
#line 2476 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6929 "parser.cpp"
    break;

  case 343: /* operand: '(' expr ')'  */
#line 2486 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6937 "parser.cpp"
    break;

  case 344: /* operand: '(' select_without_paren ')'  */
#line 2489 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6948 "parser.cpp"
    break;

  case 345: /* operand: constant_expr  */
#line 2495 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6956 "parser.cpp"
    break;

  case 356: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2511 "parser.y"
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
#line 6976 "parser.cpp"
    break;

  case 357: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2529 "parser.y"
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
#line 7026 "parser.cpp"
    break;

  case 358: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2575 "parser.y"
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
#line 7069 "parser.cpp"
    break;

  case 359: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2614 "parser.y"
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
#line 7116 "parser.cpp"
    break;

  case 360: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2657 "parser.y"
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
#line 7164 "parser.cpp"
    break;

  case 361: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2704 "parser.y"
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
#line 7192 "parser.cpp"
    break;

  case 362: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' IGNORE INDEX  */
#line 2728 "parser.y"
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
#line 7219 "parser.cpp"
    break;

  case 363: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE optional_search_filter_expr ')' with_index_param_list  */
#line 2751 "parser.y"
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
#line 7244 "parser.cpp"
    break;

  case 364: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING optional_search_filter_expr ')' with_index_param_list  */
#line 2772 "parser.y"
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
#line 7269 "parser.cpp"
    break;

  case 365: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2793 "parser.y"
                                                                                   {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-4].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7283 "parser.cpp"
    break;

  case 366: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2802 "parser.y"
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
#line 7299 "parser.cpp"
    break;

  case 367: /* query_expr: QUERY '(' STRING optional_search_filter_expr ')'  */
#line 2814 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7311 "parser.cpp"
    break;

  case 368: /* query_expr: QUERY '(' STRING ',' STRING optional_search_filter_expr ')'  */
#line 2821 "parser.y"
                                                              {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-4].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-2].str_value));
    match_text_expr->filter_expr_.reset((yyvsp[-1].expr_t));
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7325 "parser.cpp"
    break;

  case 369: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2831 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7336 "parser.cpp"
    break;

  case 370: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2837 "parser.y"
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
#line 7352 "parser.cpp"
    break;

  case 371: /* sub_search: match_vector_expr  */
#line 2849 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7360 "parser.cpp"
    break;

  case 372: /* sub_search: match_text_expr  */
#line 2852 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7368 "parser.cpp"
    break;

  case 373: /* sub_search: match_tensor_expr  */
#line 2855 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7376 "parser.cpp"
    break;

  case 374: /* sub_search: match_sparse_expr  */
#line 2858 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7384 "parser.cpp"
    break;

  case 375: /* sub_search: query_expr  */
#line 2861 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7392 "parser.cpp"
    break;

  case 376: /* sub_search: fusion_expr  */
#line 2864 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7400 "parser.cpp"
    break;

  case 377: /* sub_search_array: sub_search  */
#line 2868 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7409 "parser.cpp"
    break;

  case 378: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2872 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7418 "parser.cpp"
    break;

  case 379: /* function_expr: IDENTIFIER '(' ')'  */
#line 2877 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7431 "parser.cpp"
    break;

  case 380: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2885 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7444 "parser.cpp"
    break;

  case 381: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2893 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7458 "parser.cpp"
    break;

  case 382: /* function_expr: operand IS NOT NULLABLE  */
#line 2902 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7470 "parser.cpp"
    break;

  case 383: /* function_expr: operand IS NULLABLE  */
#line 2909 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7482 "parser.cpp"
    break;

  case 384: /* function_expr: NOT operand  */
#line 2916 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7494 "parser.cpp"
    break;

  case 385: /* function_expr: '-' operand  */
#line 2923 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7506 "parser.cpp"
    break;

  case 386: /* function_expr: '+' operand  */
#line 2930 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7518 "parser.cpp"
    break;

  case 387: /* function_expr: operand '-' operand  */
#line 2937 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7531 "parser.cpp"
    break;

  case 388: /* function_expr: operand '+' operand  */
#line 2945 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7544 "parser.cpp"
    break;

  case 389: /* function_expr: operand '*' operand  */
#line 2953 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7557 "parser.cpp"
    break;

  case 390: /* function_expr: operand '/' operand  */
#line 2961 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7570 "parser.cpp"
    break;

  case 391: /* function_expr: operand '%' operand  */
#line 2969 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7583 "parser.cpp"
    break;

  case 392: /* function_expr: operand '=' operand  */
#line 2977 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7596 "parser.cpp"
    break;

  case 393: /* function_expr: operand EQUAL operand  */
#line 2985 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7609 "parser.cpp"
    break;

  case 394: /* function_expr: operand NOT_EQ operand  */
#line 2993 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7622 "parser.cpp"
    break;

  case 395: /* function_expr: operand '<' operand  */
#line 3001 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7635 "parser.cpp"
    break;

  case 396: /* function_expr: operand '>' operand  */
#line 3009 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7648 "parser.cpp"
    break;

  case 397: /* function_expr: operand LESS_EQ operand  */
#line 3017 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7661 "parser.cpp"
    break;

  case 398: /* function_expr: operand GREATER_EQ operand  */
#line 3025 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7674 "parser.cpp"
    break;

  case 399: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 3033 "parser.y"
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
#line 7709 "parser.cpp"
    break;

  case 400: /* function_expr: operand LIKE operand  */
#line 3063 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7722 "parser.cpp"
    break;

  case 401: /* function_expr: operand NOT LIKE operand  */
#line 3071 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7735 "parser.cpp"
    break;

  case 402: /* conjunction_expr: expr AND expr  */
#line 3080 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7748 "parser.cpp"
    break;

  case 403: /* conjunction_expr: expr OR expr  */
#line 3088 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7761 "parser.cpp"
    break;

  case 404: /* between_expr: operand BETWEEN operand AND operand  */
#line 3097 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7773 "parser.cpp"
    break;

  case 405: /* in_expr: operand IN '(' expr_array ')'  */
#line 3105 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7784 "parser.cpp"
    break;

  case 406: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 3111 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7795 "parser.cpp"
    break;

  case 407: /* case_expr: CASE expr case_check_array END  */
#line 3118 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7806 "parser.cpp"
    break;

  case 408: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 3124 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7818 "parser.cpp"
    break;

  case 409: /* case_expr: CASE case_check_array END  */
#line 3131 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7828 "parser.cpp"
    break;

  case 410: /* case_expr: CASE case_check_array ELSE expr END  */
#line 3136 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7839 "parser.cpp"
    break;

  case 411: /* case_check_array: WHEN expr THEN expr  */
#line 3143 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7851 "parser.cpp"
    break;

  case 412: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 3150 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7863 "parser.cpp"
    break;

  case 413: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3158 "parser.y"
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
#line 7894 "parser.cpp"
    break;

  case 414: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3185 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7905 "parser.cpp"
    break;

  case 415: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3191 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7916 "parser.cpp"
    break;

  case 416: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3197 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7928 "parser.cpp"
    break;

  case 417: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3204 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7940 "parser.cpp"
    break;

  case 418: /* column_expr: IDENTIFIER  */
#line 3212 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7952 "parser.cpp"
    break;

  case 419: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3219 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7964 "parser.cpp"
    break;

  case 420: /* column_expr: '*'  */
#line 3226 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7974 "parser.cpp"
    break;

  case 421: /* column_expr: column_expr '.' '*'  */
#line 3231 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7988 "parser.cpp"
    break;

  case 422: /* constant_expr: STRING  */
#line 3241 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7998 "parser.cpp"
    break;

  case 423: /* constant_expr: TRUE  */
#line 3246 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 8008 "parser.cpp"
    break;

  case 424: /* constant_expr: FALSE  */
#line 3251 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 8018 "parser.cpp"
    break;

  case 425: /* constant_expr: DOUBLE_VALUE  */
#line 3256 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8028 "parser.cpp"
    break;

  case 426: /* constant_expr: LONG_VALUE  */
#line 3261 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8038 "parser.cpp"
    break;

  case 427: /* constant_expr: DATE STRING  */
#line 3266 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8048 "parser.cpp"
    break;

  case 428: /* constant_expr: TIME STRING  */
#line 3271 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8058 "parser.cpp"
    break;

  case 429: /* constant_expr: DATETIME STRING  */
#line 3276 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8068 "parser.cpp"
    break;

  case 430: /* constant_expr: TIMESTAMP STRING  */
#line 3281 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8078 "parser.cpp"
    break;

  case 431: /* constant_expr: INTERVAL interval_expr  */
#line 3286 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8086 "parser.cpp"
    break;

  case 432: /* constant_expr: interval_expr  */
#line 3289 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8094 "parser.cpp"
    break;

  case 433: /* constant_expr: common_array_expr  */
#line 3292 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8102 "parser.cpp"
    break;

  case 434: /* common_array_expr: array_expr  */
#line 3296 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8110 "parser.cpp"
    break;

  case 435: /* common_array_expr: subarray_array_expr  */
#line 3299 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8118 "parser.cpp"
    break;

  case 436: /* common_array_expr: sparse_array_expr  */
#line 3302 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8126 "parser.cpp"
    break;

  case 437: /* common_array_expr: empty_array_expr  */
#line 3305 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8134 "parser.cpp"
    break;

  case 438: /* common_sparse_array_expr: sparse_array_expr  */
#line 3309 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8142 "parser.cpp"
    break;

  case 439: /* common_sparse_array_expr: array_expr  */
#line 3312 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8150 "parser.cpp"
    break;

  case 440: /* common_sparse_array_expr: empty_array_expr  */
#line 3315 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8158 "parser.cpp"
    break;

  case 441: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3319 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8166 "parser.cpp"
    break;

  case 442: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3323 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 8176 "parser.cpp"
    break;

  case 443: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3328 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8185 "parser.cpp"
    break;

  case 444: /* sparse_array_expr: long_sparse_array_expr  */
#line 3333 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8193 "parser.cpp"
    break;

  case 445: /* sparse_array_expr: double_sparse_array_expr  */
#line 3336 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8201 "parser.cpp"
    break;

  case 446: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3340 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8209 "parser.cpp"
    break;

  case 447: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3344 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8221 "parser.cpp"
    break;

  case 448: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3351 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8232 "parser.cpp"
    break;

  case 449: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3358 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8240 "parser.cpp"
    break;

  case 450: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3362 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 8252 "parser.cpp"
    break;

  case 451: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3369 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8263 "parser.cpp"
    break;

  case 452: /* empty_array_expr: '[' ']'  */
#line 3376 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8271 "parser.cpp"
    break;

  case 453: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3380 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8279 "parser.cpp"
    break;

  case 454: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3384 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8287 "parser.cpp"
    break;

  case 455: /* array_expr: long_array_expr  */
#line 3388 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8295 "parser.cpp"
    break;

  case 456: /* array_expr: double_array_expr  */
#line 3391 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8303 "parser.cpp"
    break;

  case 457: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3395 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8311 "parser.cpp"
    break;

  case 458: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3399 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8321 "parser.cpp"
    break;

  case 459: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3404 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8330 "parser.cpp"
    break;

  case 460: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3409 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8338 "parser.cpp"
    break;

  case 461: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3413 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8348 "parser.cpp"
    break;

  case 462: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3418 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8357 "parser.cpp"
    break;

  case 463: /* interval_expr: LONG_VALUE SECONDS  */
#line 3423 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8368 "parser.cpp"
    break;

  case 464: /* interval_expr: LONG_VALUE SECOND  */
#line 3429 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8379 "parser.cpp"
    break;

  case 465: /* interval_expr: LONG_VALUE MINUTES  */
#line 3435 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8390 "parser.cpp"
    break;

  case 466: /* interval_expr: LONG_VALUE MINUTE  */
#line 3441 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8401 "parser.cpp"
    break;

  case 467: /* interval_expr: LONG_VALUE HOURS  */
#line 3447 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8412 "parser.cpp"
    break;

  case 468: /* interval_expr: LONG_VALUE HOUR  */
#line 3453 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8423 "parser.cpp"
    break;

  case 469: /* interval_expr: LONG_VALUE DAYS  */
#line 3459 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8434 "parser.cpp"
    break;

  case 470: /* interval_expr: LONG_VALUE DAY  */
#line 3465 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8445 "parser.cpp"
    break;

  case 471: /* interval_expr: LONG_VALUE MONTHS  */
#line 3471 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8456 "parser.cpp"
    break;

  case 472: /* interval_expr: LONG_VALUE MONTH  */
#line 3477 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8467 "parser.cpp"
    break;

  case 473: /* interval_expr: LONG_VALUE YEARS  */
#line 3483 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8478 "parser.cpp"
    break;

  case 474: /* interval_expr: LONG_VALUE YEAR  */
#line 3489 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8489 "parser.cpp"
    break;

  case 475: /* copy_option_list: copy_option  */
#line 3500 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8498 "parser.cpp"
    break;

  case 476: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3504 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8507 "parser.cpp"
    break;

  case 477: /* copy_option: FORMAT IDENTIFIER  */
#line 3509 "parser.y"
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
#line 8543 "parser.cpp"
    break;

  case 478: /* copy_option: DELIMITER STRING  */
#line 3540 "parser.y"
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
#line 8558 "parser.cpp"
    break;

  case 479: /* copy_option: HEADER  */
#line 3550 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8568 "parser.cpp"
    break;

  case 480: /* copy_option: OFFSET LONG_VALUE  */
#line 3555 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8578 "parser.cpp"
    break;

  case 481: /* copy_option: LIMIT LONG_VALUE  */
#line 3560 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8588 "parser.cpp"
    break;

  case 482: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3565 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8598 "parser.cpp"
    break;

  case 483: /* file_path: STRING  */
#line 3571 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8606 "parser.cpp"
    break;

  case 484: /* if_exists: IF EXISTS  */
#line 3575 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8612 "parser.cpp"
    break;

  case 485: /* if_exists: %empty  */
#line 3576 "parser.y"
  { (yyval.bool_value) = false; }
#line 8618 "parser.cpp"
    break;

  case 486: /* if_not_exists: IF NOT EXISTS  */
#line 3578 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8624 "parser.cpp"
    break;

  case 487: /* if_not_exists: %empty  */
#line 3579 "parser.y"
  { (yyval.bool_value) = false; }
#line 8630 "parser.cpp"
    break;

  case 490: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3594 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8643 "parser.cpp"
    break;

  case 491: /* if_not_exists_info: %empty  */
#line 3602 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8651 "parser.cpp"
    break;

  case 492: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3606 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8659 "parser.cpp"
    break;

  case 493: /* with_index_param_list: %empty  */
#line 3609 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8667 "parser.cpp"
    break;

  case 494: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3613 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8675 "parser.cpp"
    break;

  case 495: /* optional_table_properties_list: %empty  */
#line 3616 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8683 "parser.cpp"
    break;

  case 496: /* index_param_list: index_param  */
#line 3620 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8692 "parser.cpp"
    break;

  case 497: /* index_param_list: index_param_list ',' index_param  */
#line 3624 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8701 "parser.cpp"
    break;

  case 498: /* index_param: IDENTIFIER  */
#line 3629 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8712 "parser.cpp"
    break;

  case 499: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3635 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8727 "parser.cpp"
    break;

  case 500: /* index_param: IDENTIFIER '=' STRING  */
#line 3645 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8742 "parser.cpp"
    break;

  case 501: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3655 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8755 "parser.cpp"
    break;

  case 502: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3663 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8768 "parser.cpp"
    break;

  case 503: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3674 "parser.y"
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
#line 8804 "parser.cpp"
    break;

  case 504: /* index_info: '(' IDENTIFIER ')'  */
#line 3705 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8815 "parser.cpp"
    break;


#line 8819 "parser.cpp"

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

#line 3712 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

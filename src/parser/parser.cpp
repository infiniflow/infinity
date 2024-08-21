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
  YYSYMBOL_EXCEPT = 24,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 25,                     /* FLUSH  */
  YYSYMBOL_USE = 26,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 27,                  /* OPTIMIZE  */
  YYSYMBOL_PROPERTIES = 28,                /* PROPERTIES  */
  YYSYMBOL_DATABASE = 29,                  /* DATABASE  */
  YYSYMBOL_TABLE = 30,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 31,                /* COLLECTION  */
  YYSYMBOL_TABLES = 32,                    /* TABLES  */
  YYSYMBOL_INTO = 33,                      /* INTO  */
  YYSYMBOL_VALUES = 34,                    /* VALUES  */
  YYSYMBOL_AST = 35,                       /* AST  */
  YYSYMBOL_PIPELINE = 36,                  /* PIPELINE  */
  YYSYMBOL_RAW = 37,                       /* RAW  */
  YYSYMBOL_LOGICAL = 38,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 39,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 40,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 41,                      /* VIEW  */
  YYSYMBOL_INDEX = 42,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 43,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 44,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 45,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 46,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 47,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 48,                     /* BLOCK  */
  YYSYMBOL_BLOCKS = 49,                    /* BLOCKS  */
  YYSYMBOL_COLUMN = 50,                    /* COLUMN  */
  YYSYMBOL_COLUMNS = 51,                   /* COLUMNS  */
  YYSYMBOL_INDEXES = 52,                   /* INDEXES  */
  YYSYMBOL_CHUNK = 53,                     /* CHUNK  */
  YYSYMBOL_GROUP = 54,                     /* GROUP  */
  YYSYMBOL_BY = 55,                        /* BY  */
  YYSYMBOL_HAVING = 56,                    /* HAVING  */
  YYSYMBOL_AS = 57,                        /* AS  */
  YYSYMBOL_NATURAL = 58,                   /* NATURAL  */
  YYSYMBOL_JOIN = 59,                      /* JOIN  */
  YYSYMBOL_LEFT = 60,                      /* LEFT  */
  YYSYMBOL_RIGHT = 61,                     /* RIGHT  */
  YYSYMBOL_OUTER = 62,                     /* OUTER  */
  YYSYMBOL_FULL = 63,                      /* FULL  */
  YYSYMBOL_ON = 64,                        /* ON  */
  YYSYMBOL_INNER = 65,                     /* INNER  */
  YYSYMBOL_CROSS = 66,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 67,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 68,                     /* WHERE  */
  YYSYMBOL_ORDER = 69,                     /* ORDER  */
  YYSYMBOL_LIMIT = 70,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 71,                    /* OFFSET  */
  YYSYMBOL_ASC = 72,                       /* ASC  */
  YYSYMBOL_DESC = 73,                      /* DESC  */
  YYSYMBOL_IF = 74,                        /* IF  */
  YYSYMBOL_NOT = 75,                       /* NOT  */
  YYSYMBOL_EXISTS = 76,                    /* EXISTS  */
  YYSYMBOL_IN = 77,                        /* IN  */
  YYSYMBOL_FROM = 78,                      /* FROM  */
  YYSYMBOL_TO = 79,                        /* TO  */
  YYSYMBOL_WITH = 80,                      /* WITH  */
  YYSYMBOL_DELIMITER = 81,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 82,                    /* FORMAT  */
  YYSYMBOL_HEADER = 83,                    /* HEADER  */
  YYSYMBOL_CAST = 84,                      /* CAST  */
  YYSYMBOL_END = 85,                       /* END  */
  YYSYMBOL_CASE = 86,                      /* CASE  */
  YYSYMBOL_ELSE = 87,                      /* ELSE  */
  YYSYMBOL_THEN = 88,                      /* THEN  */
  YYSYMBOL_WHEN = 89,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 90,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 91,                   /* INTEGER  */
  YYSYMBOL_INT = 92,                       /* INT  */
  YYSYMBOL_TINYINT = 93,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 94,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 95,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 96,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 97,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 98,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 99,                    /* DOUBLE  */
  YYSYMBOL_REAL = 100,                     /* REAL  */
  YYSYMBOL_DECIMAL = 101,                  /* DECIMAL  */
  YYSYMBOL_DATE = 102,                     /* DATE  */
  YYSYMBOL_TIME = 103,                     /* TIME  */
  YYSYMBOL_DATETIME = 104,                 /* DATETIME  */
  YYSYMBOL_FLOAT16 = 105,                  /* FLOAT16  */
  YYSYMBOL_BFLOAT16 = 106,                 /* BFLOAT16  */
  YYSYMBOL_UNSIGNED = 107,                 /* UNSIGNED  */
  YYSYMBOL_TIMESTAMP = 108,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 109,                     /* UUID  */
  YYSYMBOL_POINT = 110,                    /* POINT  */
  YYSYMBOL_LINE = 111,                     /* LINE  */
  YYSYMBOL_LSEG = 112,                     /* LSEG  */
  YYSYMBOL_BOX = 113,                      /* BOX  */
  YYSYMBOL_PATH = 114,                     /* PATH  */
  YYSYMBOL_POLYGON = 115,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 116,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 117,                     /* BLOB  */
  YYSYMBOL_BITMAP = 118,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 119,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 120,                   /* VECTOR  */
  YYSYMBOL_BIT = 121,                      /* BIT  */
  YYSYMBOL_TEXT = 122,                     /* TEXT  */
  YYSYMBOL_MULTIVECTOR = 123,              /* MULTIVECTOR  */
  YYSYMBOL_TENSOR = 124,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 125,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 126,              /* TENSORARRAY  */
  YYSYMBOL_IGNORE = 127,                   /* IGNORE  */
  YYSYMBOL_PRIMARY = 128,                  /* PRIMARY  */
  YYSYMBOL_KEY = 129,                      /* KEY  */
  YYSYMBOL_UNIQUE = 130,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 131,                 /* NULLABLE  */
  YYSYMBOL_IS = 132,                       /* IS  */
  YYSYMBOL_DEFAULT = 133,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 134,                     /* TRUE  */
  YYSYMBOL_FALSE = 135,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 136,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 137,                   /* SECOND  */
  YYSYMBOL_SECONDS = 138,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 139,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 140,                  /* MINUTES  */
  YYSYMBOL_HOUR = 141,                     /* HOUR  */
  YYSYMBOL_HOURS = 142,                    /* HOURS  */
  YYSYMBOL_DAY = 143,                      /* DAY  */
  YYSYMBOL_DAYS = 144,                     /* DAYS  */
  YYSYMBOL_MONTH = 145,                    /* MONTH  */
  YYSYMBOL_MONTHS = 146,                   /* MONTHS  */
  YYSYMBOL_YEAR = 147,                     /* YEAR  */
  YYSYMBOL_YEARS = 148,                    /* YEARS  */
  YYSYMBOL_EQUAL = 149,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 150,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 151,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 152,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 153,                  /* BETWEEN  */
  YYSYMBOL_AND = 154,                      /* AND  */
  YYSYMBOL_OR = 155,                       /* OR  */
  YYSYMBOL_EXTRACT = 156,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 157,                     /* LIKE  */
  YYSYMBOL_DATA = 158,                     /* DATA  */
  YYSYMBOL_LOG = 159,                      /* LOG  */
  YYSYMBOL_BUFFER = 160,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 161,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 162,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 163,                 /* MEMINDEX  */
  YYSYMBOL_USING = 164,                    /* USING  */
  YYSYMBOL_SESSION = 165,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 166,                   /* GLOBAL  */
  YYSYMBOL_OFF = 167,                      /* OFF  */
  YYSYMBOL_EXPORT = 168,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 169,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 170,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 171,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 172,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 173,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 174,                 /* VARIABLE  */
  YYSYMBOL_DELTA = 175,                    /* DELTA  */
  YYSYMBOL_LOGS = 176,                     /* LOGS  */
  YYSYMBOL_CATALOGS = 177,                 /* CATALOGS  */
  YYSYMBOL_CATALOG = 178,                  /* CATALOG  */
  YYSYMBOL_SEARCH = 179,                   /* SEARCH  */
  YYSYMBOL_MATCH = 180,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 181,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 182,                    /* QUERY  */
  YYSYMBOL_QUERIES = 183,                  /* QUERIES  */
  YYSYMBOL_FUSION = 184,                   /* FUSION  */
  YYSYMBOL_ROWLIMIT = 185,                 /* ROWLIMIT  */
  YYSYMBOL_ADMIN = 186,                    /* ADMIN  */
  YYSYMBOL_PERSISTENCE = 187,              /* PERSISTENCE  */
  YYSYMBOL_OBJECT = 188,                   /* OBJECT  */
  YYSYMBOL_OBJECTS = 189,                  /* OBJECTS  */
  YYSYMBOL_FILES = 190,                    /* FILES  */
  YYSYMBOL_MEMORY = 191,                   /* MEMORY  */
  YYSYMBOL_ALLOCATION = 192,               /* ALLOCATION  */
  YYSYMBOL_NUMBER = 193,                   /* NUMBER  */
  YYSYMBOL_194_ = 194,                     /* '='  */
  YYSYMBOL_195_ = 195,                     /* '<'  */
  YYSYMBOL_196_ = 196,                     /* '>'  */
  YYSYMBOL_197_ = 197,                     /* '+'  */
  YYSYMBOL_198_ = 198,                     /* '-'  */
  YYSYMBOL_199_ = 199,                     /* '*'  */
  YYSYMBOL_200_ = 200,                     /* '/'  */
  YYSYMBOL_201_ = 201,                     /* '%'  */
  YYSYMBOL_202_ = 202,                     /* '['  */
  YYSYMBOL_203_ = 203,                     /* ']'  */
  YYSYMBOL_204_ = 204,                     /* '('  */
  YYSYMBOL_205_ = 205,                     /* ')'  */
  YYSYMBOL_206_ = 206,                     /* '.'  */
  YYSYMBOL_207_ = 207,                     /* ';'  */
  YYSYMBOL_208_ = 208,                     /* ','  */
  YYSYMBOL_209_ = 209,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 210,                 /* $accept  */
  YYSYMBOL_input_pattern = 211,            /* input_pattern  */
  YYSYMBOL_statement_list = 212,           /* statement_list  */
  YYSYMBOL_statement = 213,                /* statement  */
  YYSYMBOL_explainable_statement = 214,    /* explainable_statement  */
  YYSYMBOL_create_statement = 215,         /* create_statement  */
  YYSYMBOL_table_element_array = 216,      /* table_element_array  */
  YYSYMBOL_table_element = 217,            /* table_element  */
  YYSYMBOL_table_column = 218,             /* table_column  */
  YYSYMBOL_column_type = 219,              /* column_type  */
  YYSYMBOL_column_constraints = 220,       /* column_constraints  */
  YYSYMBOL_column_constraint = 221,        /* column_constraint  */
  YYSYMBOL_default_expr = 222,             /* default_expr  */
  YYSYMBOL_table_constraint = 223,         /* table_constraint  */
  YYSYMBOL_identifier_array = 224,         /* identifier_array  */
  YYSYMBOL_delete_statement = 225,         /* delete_statement  */
  YYSYMBOL_insert_statement = 226,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 227, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 228,        /* explain_statement  */
  YYSYMBOL_explain_type = 229,             /* explain_type  */
  YYSYMBOL_update_statement = 230,         /* update_statement  */
  YYSYMBOL_update_expr_array = 231,        /* update_expr_array  */
  YYSYMBOL_update_expr = 232,              /* update_expr  */
  YYSYMBOL_drop_statement = 233,           /* drop_statement  */
  YYSYMBOL_copy_statement = 234,           /* copy_statement  */
  YYSYMBOL_select_statement = 235,         /* select_statement  */
  YYSYMBOL_select_with_paren = 236,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 237,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 238, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 239, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 240, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 241,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 242,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 243,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 244,            /* order_by_type  */
  YYSYMBOL_limit_expr = 245,               /* limit_expr  */
  YYSYMBOL_offset_expr = 246,              /* offset_expr  */
  YYSYMBOL_distinct = 247,                 /* distinct  */
  YYSYMBOL_from_clause = 248,              /* from_clause  */
  YYSYMBOL_search_clause = 249,            /* search_clause  */
  YYSYMBOL_where_clause = 250,             /* where_clause  */
  YYSYMBOL_having_clause = 251,            /* having_clause  */
  YYSYMBOL_group_by_clause = 252,          /* group_by_clause  */
  YYSYMBOL_set_operator = 253,             /* set_operator  */
  YYSYMBOL_table_reference = 254,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 255,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 256,     /* table_reference_name  */
  YYSYMBOL_table_name = 257,               /* table_name  */
  YYSYMBOL_table_alias = 258,              /* table_alias  */
  YYSYMBOL_with_clause = 259,              /* with_clause  */
  YYSYMBOL_with_expr_list = 260,           /* with_expr_list  */
  YYSYMBOL_with_expr = 261,                /* with_expr  */
  YYSYMBOL_join_clause = 262,              /* join_clause  */
  YYSYMBOL_join_type = 263,                /* join_type  */
  YYSYMBOL_show_statement = 264,           /* show_statement  */
  YYSYMBOL_flush_statement = 265,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 266,       /* optimize_statement  */
  YYSYMBOL_command_statement = 267,        /* command_statement  */
  YYSYMBOL_compact_statement = 268,        /* compact_statement  */
  YYSYMBOL_admin_statement = 269,          /* admin_statement  */
  YYSYMBOL_expr_array = 270,               /* expr_array  */
  YYSYMBOL_expr_array_list = 271,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 272,               /* expr_alias  */
  YYSYMBOL_expr = 273,                     /* expr  */
  YYSYMBOL_operand = 274,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 275, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 276,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 277,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 278,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 279,          /* match_text_expr  */
  YYSYMBOL_query_expr = 280,               /* query_expr  */
  YYSYMBOL_fusion_expr = 281,              /* fusion_expr  */
  YYSYMBOL_sub_search = 282,               /* sub_search  */
  YYSYMBOL_sub_search_array = 283,         /* sub_search_array  */
  YYSYMBOL_function_expr = 284,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 285,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 286,             /* between_expr  */
  YYSYMBOL_in_expr = 287,                  /* in_expr  */
  YYSYMBOL_case_expr = 288,                /* case_expr  */
  YYSYMBOL_case_check_array = 289,         /* case_check_array  */
  YYSYMBOL_cast_expr = 290,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 291,            /* subquery_expr  */
  YYSYMBOL_column_expr = 292,              /* column_expr  */
  YYSYMBOL_constant_expr = 293,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 294,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 295, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 296,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 297, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 298,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 299,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 300, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 301, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 302, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 303,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 304,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 305,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 306,               /* array_expr  */
  YYSYMBOL_long_array_expr = 307,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 308, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 309,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 310, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 311,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 312,         /* copy_option_list  */
  YYSYMBOL_copy_option = 313,              /* copy_option  */
  YYSYMBOL_file_path = 314,                /* file_path  */
  YYSYMBOL_if_exists = 315,                /* if_exists  */
  YYSYMBOL_if_not_exists = 316,            /* if_not_exists  */
  YYSYMBOL_semicolon = 317,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 318,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 319,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 320, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 321,         /* index_param_list  */
  YYSYMBOL_index_param = 322,              /* index_param  */
  YYSYMBOL_index_info = 323                /* index_info  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 452 "parser.cpp"

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
#define YYFINAL  99
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1318

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  210
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  491
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1078

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   448


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
       2,     2,     2,     2,     2,     2,     2,   201,     2,     2,
     204,   205,   199,   197,   208,   198,   206,   200,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   209,   207,
     195,   194,   196,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   202,     2,   203,     2,     2,     2,     2,     2,     2,
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
     185,   186,   187,   188,   189,   190,   191,   192,   193
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
    2055,  2061,  2067,  2073,  2079,  2087,  2093,  2100,  2113,  2117,
    2122,  2128,  2135,  2143,  2152,  2162,  2172,  2183,  2194,  2206,
    2218,  2228,  2239,  2251,  2264,  2268,  2273,  2278,  2289,  2293,
    2298,  2302,  2329,  2335,  2339,  2340,  2341,  2342,  2343,  2345,
    2348,  2354,  2357,  2358,  2359,  2360,  2361,  2362,  2363,  2364,
    2365,  2366,  2368,  2371,  2377,  2396,  2441,  2479,  2521,  2567,
    2588,  2608,  2626,  2644,  2652,  2663,  2669,  2678,  2684,  2696,
    2699,  2702,  2705,  2708,  2711,  2715,  2719,  2724,  2732,  2740,
    2749,  2756,  2763,  2770,  2777,  2784,  2792,  2800,  2808,  2816,
    2824,  2832,  2840,  2848,  2856,  2864,  2872,  2880,  2910,  2918,
    2927,  2935,  2944,  2952,  2958,  2965,  2971,  2978,  2983,  2990,
    2997,  3005,  3032,  3038,  3044,  3051,  3059,  3066,  3073,  3078,
    3088,  3093,  3098,  3103,  3108,  3113,  3118,  3123,  3128,  3133,
    3136,  3139,  3143,  3146,  3149,  3152,  3156,  3159,  3162,  3166,
    3170,  3175,  3180,  3183,  3187,  3191,  3198,  3205,  3209,  3216,
    3223,  3227,  3231,  3235,  3238,  3242,  3246,  3251,  3256,  3260,
    3265,  3270,  3276,  3282,  3288,  3294,  3300,  3306,  3312,  3318,
    3324,  3330,  3336,  3347,  3351,  3356,  3386,  3396,  3401,  3406,
    3411,  3417,  3421,  3422,  3424,  3425,  3427,  3428,  3440,  3448,
    3452,  3455,  3459,  3462,  3466,  3470,  3475,  3481,  3491,  3499,
    3510,  3541
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
  "PREPARE", "UNION", "ALL", "INTERSECT", "COMPACT", "EXCEPT", "FLUSH",
  "USE", "OPTIMIZE", "PROPERTIES", "DATABASE", "TABLE", "COLLECTION",
  "TABLES", "INTO", "VALUES", "AST", "PIPELINE", "RAW", "LOGICAL",
  "PHYSICAL", "FRAGMENT", "VIEW", "INDEX", "ANALYZE", "VIEWS", "DATABASES",
  "SEGMENT", "SEGMENTS", "BLOCK", "BLOCKS", "COLUMN", "COLUMNS", "INDEXES",
  "CHUNK", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT",
  "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE",
  "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN",
  "FROM", "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END",
  "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "FLOAT16", "BFLOAT16", "UNSIGNED",
  "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON",
  "CIRCLE", "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT", "TEXT",
  "MULTIVECTOR", "TENSOR", "SPARSE", "TENSORARRAY", "IGNORE", "PRIMARY",
  "KEY", "UNIQUE", "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE",
  "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS",
  "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ",
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE",
  "DATA", "LOG", "BUFFER", "TRANSACTIONS", "TRANSACTION", "MEMINDEX",
  "USING", "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS",
  "CONFIG", "PROFILES", "VARIABLES", "VARIABLE", "DELTA", "LOGS",
  "CATALOGS", "CATALOG", "SEARCH", "MATCH", "MAXSIM", "QUERY", "QUERIES",
  "FUSION", "ROWLIMIT", "ADMIN", "PERSISTENCE", "OBJECT", "OBJECTS",
  "FILES", "MEMORY", "ALLOCATION", "NUMBER", "'='", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'", "';'",
  "','", "':'", "$accept", "input_pattern", "statement_list", "statement",
  "explainable_statement", "create_statement", "table_element_array",
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
  "search_clause", "where_clause", "having_clause", "group_by_clause",
  "set_operator", "table_reference", "table_reference_unit",
  "table_reference_name", "table_name", "table_alias", "with_clause",
  "with_expr_list", "with_expr", "join_clause", "join_type",
  "show_statement", "flush_statement", "optimize_statement",
  "command_statement", "compact_statement", "admin_statement",
  "expr_array", "expr_array_list", "expr_alias", "expr", "operand",
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

#define YYPACT_NINF (-533)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-480)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      49,   251,    95,   366,   132,     1,   132,   177,   447,   604,
     114,   306,   150,   166,   210,    36,   173,    -2,   238,    79,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,   312,  -533,
    -533,   282,  -533,  -533,  -533,  -533,  -533,  -533,   222,   222,
     222,   222,    62,   132,   239,   239,   239,   239,   239,    78,
     301,   132,    29,   350,   367,   373,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,   703,   388,   132,  -533,  -533,  -533,  -533,
    -533,   387,  -533,    76,    98,  -533,   400,  -533,   233,  -533,
    -533,   291,  -533,   323,   -88,   132,  -533,  -533,  -533,  -533,
     -18,  -533,   382,   280,  -533,   472,   126,   300,   314,  -533,
      26,  -533,   502,  -533,  -533,     6,   458,  -533,   469,   470,
     557,   132,   132,   132,   569,   486,   362,   506,   583,   132,
     132,   132,   600,   607,   611,   540,   617,   617,   418,    77,
      81,    85,  -533,  -533,  -533,  -533,  -533,  -533,  -533,   312,
    -533,  -533,  -533,  -533,  -533,  -533,   274,  -533,  -533,   623,
    -533,   625,  -533,  -533,   626,   631,  -533,  -533,  -533,  -533,
    -533,   132,   438,   210,   617,   637,  -533,  -533,   638,  -533,
    -533,  -533,  -533,     6,  -533,  -533,  -533,   418,   595,   582,
     577,  -533,    -9,  -533,   362,  -533,   132,   657,    58,  -533,
    -533,  -533,  -533,  -533,   599,  -533,   471,    -1,  -533,   418,
    -533,  -533,   586,   587,   460,  -533,  -533,   758,   543,   467,
     468,   270,   670,   671,   672,   677,  -533,  -533,   676,   482,
     255,   484,   485,   503,   503,  -533,    13,   395,   -86,  -533,
      -7,   683,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,   487,  -533,  -533,  -533,  -103,
    -533,  -533,    28,  -533,    91,  -533,  -533,  -533,   102,  -533,
     121,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,   687,   685,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,   612,   282,  -533,  -533,
      88,   688,   491,   492,   -62,   418,   418,   624,  -533,    -2,
      40,   641,   499,  -533,    60,   500,  -533,   132,   418,   611,
    -533,   152,   504,   505,   153,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,   503,   507,   717,
     640,   418,   418,    72,   305,  -533,  -533,  -533,  -533,   758,
    -533,   702,   514,   517,   518,   520,   727,   728,   424,   424,
    -533,   524,  -533,  -533,  -533,  -533,   529,   -34,   656,   418,
     733,   418,   418,   -20,   534,    -4,   503,   503,   503,   503,
     503,   503,   503,   503,   503,   503,   503,   503,   503,   503,
      20,  -533,   537,  -533,   737,  -533,   738,  -533,   740,  -533,
     743,   704,   371,   545,  -533,   546,   746,  -533,    69,  -533,
    -533,    10,   574,   551,  -533,   186,   152,   418,  -533,   312,
     829,   632,   559,   139,  -533,  -533,  -533,    -2,   768,  -533,
    -533,   769,   418,   565,  -533,   152,  -533,   196,   196,   418,
    -533,   156,   640,   620,   573,    -3,   108,   340,  -533,   418,
     418,   706,   418,   778,    22,   418,   161,   253,   490,  -533,
    -533,   617,  -533,  -533,  -533,   634,   581,   503,   395,   658,
    -533,   662,   662,   187,   187,   693,   662,   662,   187,   187,
     424,   424,  -533,  -533,  -533,  -533,  -533,  -533,   588,  -533,
     589,  -533,  -533,  -533,   784,   787,  -533,   793,  -533,  -533,
     801,  -533,    -2,   601,   352,  -533,    74,  -533,   273,   540,
     418,  -533,  -533,  -533,   152,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,   613,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,   614,
     616,   618,   619,   622,   627,   237,   633,   657,   780,    40,
     312,   636,  -533,   254,   635,   810,   815,   820,   824,  -533,
     822,   268,  -533,   272,   284,  -533,   643,  -533,   829,   418,
    -533,   418,   -12,   115,   503,   -44,   621,  -533,    -8,   -42,
    -533,   826,  -533,   834,  -533,  -533,   766,   395,   662,   646,
     319,  -533,   503,   846,   848,   802,   804,   678,   327,  -533,
     858,    46,    10,   806,  -533,  -533,  -533,  -533,  -533,  -533,
     811,  -533,   870,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,   667,   831,  -533,   880,   342,   464,   563,   865,   882,
     899,   745,   779,  -533,  -533,   167,  -533,   776,   657,   328,
     732,  -533,  -533,   782,  -533,   418,  -533,  -533,  -533,  -533,
    -533,  -533,   196,  -533,  -533,  -533,   705,   152,     0,  -533,
     418,   605,   741,   940,   537,   748,   742,   756,   747,   757,
     329,  -533,  -533,   717,   959,   960,   425,  -533,   793,   202,
      74,   352,    10,    10,   763,   273,   913,   922,   334,   761,
     771,   774,   775,   777,   788,   791,   792,   794,   891,   799,
     800,   803,   805,   807,   808,   809,   813,   814,   816,   902,
     817,   818,   819,   821,   823,   825,   827,   828,   830,   832,
     908,   833,   835,   836,   837,   838,   839,   840,   841,   842,
     843,   916,   844,   845,   847,   849,   850,   851,   852,   853,
     854,   855,   917,   856,   857,   859,   860,   861,   862,   863,
     864,   866,   867,   919,   868,  -533,  -533,    89,  -533,  -533,
    -533,   335,  -533,   793,  1011,   359,  -533,  -533,  -533,   152,
    -533,   495,   869,   360,   871,    15,   872,  -533,  -533,  -533,
    -533,  -533,   196,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  1012,  -533,  -533,  -533,   955,   657,  -533,   418,   418,
    -533,  -533,  1017,  1022,  1024,  1026,  1028,  1031,  1033,  1036,
    1048,  1050,   873,  1060,  1067,  1072,  1076,  1077,  1078,  1079,
    1080,  1081,  1082,   881,  1084,  1085,  1086,  1087,  1088,  1089,
    1090,  1091,  1092,  1093,   892,  1095,  1096,  1097,  1098,  1099,
    1100,  1101,  1102,  1103,  1104,   903,  1106,  1107,  1108,  1109,
    1110,  1111,  1112,  1113,  1114,  1115,   914,  1117,  1118,  1119,
    1120,  1121,  1122,  1123,  1124,  1125,  1126,   925,  1128,  -533,
    -533,   368,   612,  -533,  -533,  1131,  -533,  1132,  1133,  1134,
     375,  1135,   418,   376,   931,   152,   935,   937,   938,   939,
     941,   942,   943,   944,   945,   946,  1139,   947,   948,   949,
     950,   951,   952,   953,   954,   956,   957,  1154,   958,   961,
     962,   963,   964,   965,   966,   967,   968,   969,  1158,   970,
     971,   972,   973,   974,   975,   976,   977,   978,   979,  1159,
     980,   981,   982,   983,   984,   985,   986,   987,   988,   989,
    1189,   991,   992,   993,   994,   995,   996,   997,   998,   999,
    1000,  1200,  1002,  -533,  -533,  1001,  1003,  1004,   383,  -533,
     276,   152,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  1005,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  1006,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  1008,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  1009,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  1010,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  1013,  -533,
    1212,  -533,  1213,   612,  1214,  1215,  1216,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,   396,  1015,  -533,  1014,
    1218,   547,   612,  1219,  1222,  1023,   -46,   584,  1221,  -533,
    -533,  1025,  -533,  -533,  1187,  1190,  -533,  1225,  -533,  1182,
      23,  -533,  1030,  -533,  -533,  1193,  1194,  -533,  1234,  -533,
    1034,  1035,  1236,   612,  1037,  -533,   612,  -533
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     225,     0,     0,     0,     0,     0,     0,     0,   160,     0,
       0,     0,     0,     0,     0,     0,     0,   225,     0,   477,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   174,
     173,     0,     8,    14,    15,    16,    17,    18,   475,   475,
     475,   475,   475,     0,   473,   473,   473,   473,   473,   218,
       0,     0,     0,     0,     0,     0,   154,   158,   155,   156,
     157,   159,   153,   225,     0,     0,   239,   240,   238,   244,
     248,     0,   245,     0,     0,   241,     0,   243,     0,   266,
     268,     0,   246,     0,   272,     0,   275,   276,   277,   280,
     218,   278,     0,   224,   226,     0,     0,     0,     0,     1,
     225,     2,   208,   210,   211,     0,   197,   179,   185,     0,
       0,     0,     0,     0,     0,     0,   151,     0,     0,     0,
       0,     0,     0,     0,     0,   203,     0,     0,     0,     0,
       0,     0,   152,    19,    24,    26,    25,    20,    21,    23,
      22,    27,    28,    29,    30,   254,   255,   249,   250,     0,
     251,     0,   242,   267,     0,     0,   270,   269,   273,   274,
     297,     0,     0,     0,     0,     0,   314,   298,     0,   178,
     177,     4,   209,     0,   175,   176,   196,     0,     0,   193,
       0,    31,     0,    32,   151,   478,     0,     0,   225,   472,
     165,   167,   166,   168,     0,   219,     0,   203,   162,     0,
     147,   471,     0,     0,   406,   410,   413,   414,     0,     0,
       0,     0,     0,     0,     0,     0,   411,   412,     0,     0,
       0,     0,     0,     0,     0,   408,     0,   225,     0,   318,
     323,   324,   338,   336,   339,   337,   340,   341,   333,   328,
     327,   326,   334,   335,   325,   332,   331,   421,   423,     0,
     424,   432,     0,   433,     0,   425,   422,   443,     0,   444,
       0,   420,   284,   286,   285,   282,   283,   289,   291,   290,
     287,   288,   294,   296,   295,   292,   293,     0,     0,   257,
     256,   262,   252,   253,   247,   271,   481,     0,   227,   281,
     315,   299,     0,     0,   199,     0,     0,   195,   474,   225,
       0,     0,     0,   145,     0,     0,   149,     0,     0,     0,
     161,   202,     0,     0,     0,   452,   451,   454,   453,   456,
     455,   458,   457,   460,   459,   462,   461,     0,     0,   372,
     225,     0,     0,     0,     0,   415,   416,   417,   418,     0,
     419,     0,     0,     0,     0,     0,     0,     0,   374,   373,
     449,   446,   440,   430,   435,   438,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   429,     0,   434,     0,   437,     0,   445,     0,   448,
       0,   263,   258,     0,   279,     0,     0,   316,     0,   182,
     181,     0,   201,   184,   186,   191,   192,     0,   180,    34,
       0,     0,     0,     0,    37,    39,    40,   225,     0,    36,
     150,     0,     0,   148,   169,   164,   163,     0,     0,     0,
     367,     0,   225,     0,     0,     0,     0,     0,   397,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   330,
     329,     0,   319,   322,   390,   391,     0,     0,   225,     0,
     371,   381,   382,   385,   386,     0,   388,   380,   383,   384,
     376,   375,   377,   378,   379,   407,   409,   431,     0,   436,
       0,   439,   447,   450,     0,     0,   259,     0,   228,   317,
       0,   300,   225,   198,   212,   214,   223,   215,     0,   203,
       0,   189,   190,   188,   194,    43,    46,    47,    44,    45,
      48,    49,    65,    50,    52,    51,    68,    55,    56,    57,
      53,    54,    58,    59,    60,    61,    62,    63,    64,     0,
       0,     0,     0,     0,     0,   481,     0,     0,   483,     0,
      35,     0,   146,     0,     0,     0,     0,     0,     0,   467,
       0,     0,   463,     0,     0,   368,     0,   402,     0,     0,
     395,     0,     0,     0,     0,     0,     0,   406,     0,     0,
     355,     0,   357,     0,   442,   441,     0,   225,   389,     0,
       0,   370,     0,     0,     0,   264,   260,   486,     0,   484,
     301,     0,     0,     0,   232,   233,   234,   235,   231,   236,
       0,   221,     0,   216,   361,   359,   362,   360,   363,   364,
     365,   200,   207,   187,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   138,   139,   142,   135,   142,     0,     0,
       0,    33,    38,   491,   320,     0,   469,   468,   466,   465,
     470,   172,     0,   170,   369,   403,     0,   399,     0,   398,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   404,   393,   392,     0,     0,     0,   480,     0,     0,
     223,   213,     0,     0,   220,     0,     0,   205,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   140,   137,     0,   136,    42,
      41,     0,   144,     0,     0,     0,   464,   401,   396,   400,
     387,     0,     0,     0,     0,     0,     0,   426,   428,   427,
     356,   358,     0,   405,   394,   265,   261,   487,   489,   488,
     485,     0,   302,   217,   229,     0,     0,   366,     0,     0,
     183,    67,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   141,
     143,     0,   481,   321,   446,     0,   353,     0,     0,     0,
       0,   303,     0,     0,   206,   204,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   482,   490,     0,     0,     0,     0,   171,
       0,   230,   222,    66,    72,    73,    70,    71,    74,    75,
      76,    77,    78,     0,    69,   116,   117,   114,   115,   118,
     119,   120,   121,   122,     0,   113,    83,    84,    81,    82,
      85,    86,    87,    88,    89,     0,    80,    94,    95,    92,
      93,    96,    97,    98,    99,   100,     0,    91,   127,   128,
     125,   126,   129,   130,   131,   132,   133,     0,   124,   105,
     106,   103,   104,   107,   108,   109,   110,   111,     0,   102,
       0,   354,     0,   481,     0,     0,     0,   305,   304,   310,
      79,   123,    90,   101,   134,   112,     0,   343,   352,     0,
     311,   306,   481,     0,     0,     0,   481,     0,     0,   307,
     348,     0,   342,   344,     0,     0,   351,     0,   312,   308,
     481,   350,     0,   313,   309,     0,     0,   347,     0,   346,
       0,     0,     0,   481,     0,   349,   481,   345
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -533,  -533,  -533,  1141,  -533,  1180,  -533,   707,  -533,   686,
    -533,   628,   629,  -533,  -526,  1184,  1185,  1061,  -533,  -533,
    1186,  -533,   990,  1188,  1191,   -54,  1233,   -16,  1007,  1094,
      -6,  -533,  -533,   752,  -533,  -533,  -533,  -533,  -533,  -533,
    -191,  -533,  -533,  -533,  -533,   663,   -32,     4,   590,  -533,
    -533,  1105,  -533,  -533,  1195,  1196,  1198,  1199,  1201,  -533,
    -175,  -533,   898,  -199,  -193,  -533,  -476,  -474,  -471,  -469,
    -466,  -453,   591,  -533,  -533,  -533,  -533,  -533,  -533,   930,
    -533,  -533,   874,   522,  -222,  -533,  -533,  -533,   610,  -533,
    -533,  -533,  -533,   615,   887,   886,   -43,  -533,  -533,  -533,
    -533,  1055,  -423,   639,  -120,   398,   476,  -533,  -533,  -532,
    -533,   521,   608,  -533
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   132,    21,   413,   414,   415,   535,
     625,   626,   749,   416,   304,    22,    23,   188,    24,    63,
      25,   197,   198,    26,    27,    28,    29,    30,   107,   174,
     108,   179,   403,   404,   503,   297,   408,   177,   402,   499,
     200,   790,   677,   105,   493,   494,   495,   496,   603,    31,
      93,    94,   497,   600,    32,    33,    34,    35,    36,    37,
     228,   423,   229,   230,   231,  1045,   232,   233,   234,   235,
     236,   237,   610,   611,   238,   239,   240,   241,   242,   334,
     243,   244,   245,   246,   247,   766,   248,   249,   250,   251,
     252,   253,   254,   255,   354,   355,   256,   257,   258,   259,
     260,   261,   551,   552,   202,   118,   110,   101,   115,   394,
     631,   588,   589,   419
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     311,    98,   294,   627,   353,   553,   310,   203,    50,   139,
      52,   629,   333,    49,   106,   329,   401,    91,   350,   351,
     350,   351,   604,   475,   605,   567,  -476,   606,   357,   607,
     348,   349,   608,     1,   393,     2,     3,     4,     5,     6,
       7,     8,     9,   410,   289,   609,   161,   116,   299,    10,
     360,    11,    12,    13,   558,   125,     1,   456,     2,     3,
       4,     5,     6,     7,     8,     9,   102,   199,   103,   146,
     104,   459,    10,   649,    11,    12,    13,   601,    14,    51,
     262,  1054,   263,   264,   267,   758,   268,   269,   272,   160,
     273,   274,   305,   205,   206,   207,   405,   406,   490,   175,
     381,   158,   751,   393,   159,   382,    14,   126,   127,   425,
     361,   362,   361,   362,   491,   182,   183,   184,  1055,   358,
     361,   362,   359,   191,   192,   193,  -479,   460,    43,    14,
     396,   602,   435,   436,   329,    49,   109,   457,    14,   431,
     397,   265,   361,   362,    85,   270,   359,   361,   362,   275,
    1065,   361,   362,    89,   361,   362,   204,   205,   206,   207,
     477,   332,   454,   455,   652,   286,   655,   293,   411,    90,
     412,   450,   306,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   473,   474,  1066,   123,    96,
     302,   212,   213,   214,    15,   300,   559,   215,   380,   604,
     654,   605,    17,   650,   606,    95,   607,   309,   504,   608,
     173,   356,    16,    92,   492,   226,   352,    15,   352,   476,
     429,   225,   609,   216,   217,   218,   361,   362,   208,   209,
      17,   383,   781,   128,   782,    16,   384,   210,    99,   211,
     562,   563,   621,   565,   266,   409,   569,   543,   271,   148,
     149,   670,   276,    17,   554,   212,   213,   214,   501,   502,
     873,   215,   361,   362,   578,   420,   545,   546,   421,   361,
     362,   150,   151,   204,   205,   206,   207,   547,   548,   549,
      38,    39,    40,   580,   123,   165,   100,   216,   217,   218,
     106,   226,    41,    42,   385,   622,   109,   623,   624,   386,
     747,   405,   166,   167,   168,   387,   361,   362,   612,   219,
     388,   424,   621,   117,   434,   124,   277,   393,  1025,   365,
     278,   279,  1026,  1027,   389,   280,   281,  1028,  1029,   390,
     944,   576,   102,   220,   103,   221,   104,   222,  -480,  -480,
     361,   362,    53,    54,   538,   208,   209,   539,    55,   870,
     223,   224,   225,   129,   210,   226,   211,   227,   430,   332,
     647,   555,   648,   540,   359,   622,   570,   623,   624,   571,
     130,   651,   212,   213,   214,   342,   131,   343,   215,   344,
     345,   550,  -480,  -480,   375,   376,   377,   378,   379,   663,
     438,   145,   439,   147,   440,    44,    45,    46,   204,   205,
     206,   207,   660,   152,   216,   217,   218,    47,    48,   153,
     593,  -237,   594,   595,   596,   597,   556,   598,   599,   485,
     486,   204,   205,   206,   207,   560,   219,   561,   777,   440,
     778,   779,   764,   679,   680,   681,   682,   683,   591,   162,
     684,   685,   579,   119,   120,   121,   122,   686,   687,   688,
     220,   759,   221,   220,   222,   221,   154,   222,   572,   634,
     755,   573,   359,   689,    86,    87,    88,   223,   224,   225,
     208,   209,   226,   641,   227,    14,   642,   643,   164,   210,
     642,   211,    56,    57,    58,    59,    60,    61,   163,   644,
      62,  1038,   359,   208,   209,   574,   575,   212,   213,   214,
     350,   864,   210,   215,   211,   169,   204,   205,   206,   207,
    1050,   155,   156,   157,  1056,   111,   112,   113,   114,   170,
     212,   213,   214,   172,   662,   176,   215,   359,  1067,   216,
     217,   218,   667,   752,   774,   668,   421,   359,   178,   791,
     860,  1075,   792,   421,  1077,   180,   204,   205,   206,   207,
     186,   219,   216,   217,   218,   690,   691,   692,   693,   694,
     181,   659,   695,   696,   863,   866,   187,   359,   867,   697,
     698,   699,   185,   943,   219,   220,   668,   221,   327,   222,
     949,   952,   189,   642,   421,   700,   190,   210,  1023,   211,
     875,  1024,   223,   224,   225,  1048,  1049,   226,   220,   227,
     221,  1042,   222,   194,  1043,   212,   213,   214,   199,   762,
     195,   215,   769,   874,   196,   223,   224,   225,   327,   328,
     226,   201,   227,   377,   378,   379,   282,   210,   283,   211,
    1057,  1058,   284,    64,    65,   285,    66,   216,   217,   218,
     784,   785,   287,   290,   291,   212,   213,   214,    67,    68,
     295,   215,   296,   298,   701,   702,   703,   704,   705,   219,
     303,   706,   707,   307,   314,   308,   312,   313,   708,   709,
     710,   330,   331,   951,   335,   336,   337,   216,   217,   218,
     433,   338,   339,   220,   711,   221,   341,   222,   346,   347,
     391,   392,   393,   380,   398,   407,   399,   400,   417,   219,
     223,   224,   225,   418,   422,   226,   441,   227,   427,   428,
       1,   432,     2,     3,     4,     5,     6,     7,   442,     9,
      14,   443,   444,   220,   445,   221,    10,   222,    11,    12,
      13,   446,   447,   448,   449,   451,   453,   365,   458,   226,
     223,   224,   225,   478,   480,   226,   482,   227,   483,   487,
     484,   488,   489,   498,   366,   367,   368,   369,   363,   500,
     364,   536,   371,   537,    69,    70,    71,    72,   433,    73,
      74,   541,   542,   544,    75,    76,    77,   457,   557,    78,
      79,    80,   566,    14,   564,   577,    81,    82,   361,   581,
     585,    83,   433,   586,   365,    84,   587,   583,   584,   372,
     373,   374,   375,   376,   377,   378,   379,   590,   630,   592,
     760,  -480,  -480,   368,   369,   365,   636,   614,   615,  -480,
     616,   637,   617,   618,   638,   365,   619,   639,   640,   653,
     656,   620,   366,   367,   368,   369,   370,   628,   657,   635,
     371,   633,   366,   367,   368,   369,   658,   582,   645,   365,
     371,   661,   575,   574,   665,   664,  -480,   373,   374,   375,
     376,   377,   378,   379,   669,   672,   366,   367,   368,   369,
     673,    15,   666,   674,   371,   675,   745,   372,   373,   374,
     375,   376,   377,   378,   379,   676,   678,   372,   373,   374,
     375,   376,   377,   378,   379,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,    17,   746,   747,
     757,   372,   373,   374,   375,   376,   377,   378,   379,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   753,   522,   523,   524,
     525,   526,   527,   761,   763,   528,   754,   770,   529,   530,
     765,   772,   531,   532,   533,   534,   712,   713,   714,   715,
     716,   771,   773,   717,   718,   775,   776,   786,   788,   793,
     719,   720,   721,   723,   724,   725,   726,   727,   789,   794,
     728,   729,   795,   796,   802,   797,   722,   730,   731,   732,
     734,   735,   736,   737,   738,   813,   798,   739,   740,   799,
     800,   824,   801,   733,   741,   742,   743,   803,   804,   835,
     846,   805,   857,   806,   862,   807,   808,   809,   871,   872,
     744,   810,   811,   876,   812,   814,   815,   816,   877,   817,
     878,   818,   879,   819,   880,   820,   821,   881,   822,   882,
     823,   825,   883,   826,   827,   828,   829,   830,   831,   832,
     833,   834,   836,   837,   884,   838,   885,   839,   840,   841,
     842,   843,   844,   845,   847,   848,   887,   849,   850,   851,
     852,   853,   854,   888,   855,   856,   858,   865,   889,   868,
     869,   886,   890,   891,   892,   893,   894,   895,   896,   897,
     898,   899,   900,   901,   902,   903,   904,   905,   906,   907,
     908,   909,   910,   911,   912,   913,   914,   915,   916,   917,
     918,   919,   920,   921,   922,   923,   924,   925,   926,   927,
     928,   929,   930,   931,   932,   933,   934,   935,   936,   937,
     938,   939,   940,   941,   942,   945,   946,   947,   948,   359,
     953,   950,   954,   955,   956,   963,   957,   958,   959,   960,
     961,   962,   964,   965,   966,   967,   968,   969,   970,   971,
     974,   972,   973,   975,   985,   996,   976,   977,   978,   979,
     980,   981,   982,   983,   984,   986,   987,   988,   989,   990,
     991,   992,   993,   994,   995,   997,   998,   999,  1000,  1001,
    1002,  1003,  1004,  1005,  1006,  1007,  1008,  1009,  1010,  1011,
    1012,  1013,  1014,  1015,  1016,  1017,  1018,  1019,  1021,  1020,
    1030,  1031,  1022,  1032,  1033,  1034,  1036,  1037,  1035,  1046,
    1039,  1040,  1041,  1044,  1047,  1051,  1052,  1059,  1053,  1061,
    1060,  1063,  1062,  1064,  1068,  1069,  1070,  1071,  1072,  1074,
    1073,   171,  1076,   133,   646,   301,   632,   134,   135,   136,
      97,   137,   613,   748,   138,   671,   750,   452,   140,   141,
     783,   142,   143,   437,   144,   767,   787,   292,   288,   859,
     768,   479,   481,   340,   861,     0,   780,     0,     0,     0,
       0,   756,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   395,     0,     0,     0,     0,   426,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   568
};

static const yytype_int16 yycheck[] =
{
     199,    17,   177,   535,   226,   428,   197,   127,     4,    63,
       6,   537,   211,     3,     8,   208,    78,    13,     5,     6,
       5,     6,   498,     3,   498,     3,     0,   498,   227,   498,
     223,   224,   498,     7,    80,     9,    10,    11,    12,    13,
      14,    15,    16,     3,   164,   498,    64,    43,    57,    23,
      57,    25,    26,    27,    57,    51,     7,    77,     9,    10,
      11,    12,    13,    14,    15,    16,    20,    68,    22,    65,
      24,    75,    23,    85,    25,    26,    27,     3,    80,    78,
       3,   127,     5,     6,     3,    85,     5,     6,     3,    85,
       5,     6,    34,     4,     5,     6,   295,   296,    29,   105,
     203,   189,   628,    80,   192,   208,    80,    78,    79,   308,
     154,   155,   154,   155,    45,   111,   112,   113,   164,   205,
     154,   155,   208,   119,   120,   121,    64,   131,    33,    80,
      42,    57,   331,   332,   327,     3,    74,   157,    80,   314,
      52,    64,   154,   155,    30,    64,   208,   154,   155,    64,
     127,   154,   155,     3,   154,   155,     3,     4,     5,     6,
     382,    89,   361,   362,   208,   161,   208,   173,   128,     3,
     130,   205,   188,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   164,   206,    16,
     186,   102,   103,   104,   168,   204,    88,   108,   206,   675,
     208,   675,   204,    88,   675,   169,   675,   208,   407,   675,
     204,   227,   186,     3,   204,   202,   203,   168,   203,   199,
      67,   199,   675,   134,   135,   136,   154,   155,    75,    76,
     204,   203,    30,   204,    32,   186,   208,    84,     0,    86,
     439,   440,    75,   442,   167,   299,   445,   422,   167,   173,
     174,   205,   167,   204,   429,   102,   103,   104,    72,    73,
     786,   108,   154,   155,   457,   205,    70,    71,   208,   154,
     155,   173,   174,     3,     4,     5,     6,    81,    82,    83,
      29,    30,    31,   458,   206,   159,   207,   134,   135,   136,
       8,   202,    41,    42,   203,   128,    74,   130,   131,   208,
     133,   500,   176,   177,   178,   203,   154,   155,   499,   156,
     208,   307,    75,    74,   330,    14,    42,    80,    42,   132,
      46,    47,    46,    47,   203,    51,    52,    51,    52,   208,
     862,   451,    20,   180,    22,   182,    24,   184,   151,   152,
     154,   155,   165,   166,   205,    75,    76,   208,   171,   772,
     197,   198,   199,     3,    84,   202,    86,   204,   205,    89,
     559,   205,   561,   417,   208,   128,   205,   130,   131,   208,
       3,   564,   102,   103,   104,   120,     3,   122,   108,   124,
     125,   185,   195,   196,   197,   198,   199,   200,   201,   582,
      85,     3,    87,     6,    89,    29,    30,    31,     3,     4,
       5,     6,   577,     3,   134,   135,   136,    41,    42,   176,
      58,    59,    60,    61,    62,    63,   432,    65,    66,    48,
      49,     3,     4,     5,     6,    85,   156,    87,     3,    89,
       5,     6,   654,    91,    92,    93,    94,    95,   492,    57,
      98,    99,   458,    45,    46,    47,    48,   105,   106,   107,
     180,   650,   182,   180,   184,   182,   165,   184,   205,   205,
     635,   208,   208,   121,   158,   159,   160,   197,   198,   199,
      75,    76,   202,   205,   204,    80,   208,   205,     6,    84,
     208,    86,    35,    36,    37,    38,    39,    40,   208,   205,
      43,  1023,   208,    75,    76,     5,     6,   102,   103,   104,
       5,     6,    84,   108,    86,   205,     3,     4,     5,     6,
    1042,   188,   189,   190,  1046,    39,    40,    41,    42,   205,
     102,   103,   104,    21,   205,    67,   108,   208,  1060,   134,
     135,   136,   205,   205,   205,   208,   208,   208,    69,   205,
     205,  1073,   208,   208,  1076,    75,     3,     4,     5,     6,
      64,   156,   134,   135,   136,    91,    92,    93,    94,    95,
       3,   577,    98,    99,   205,   205,   204,   208,   208,   105,
     106,   107,     3,   205,   156,   180,   208,   182,    75,   184,
     205,   205,    76,   208,   208,   121,     3,    84,   205,    86,
     789,   208,   197,   198,   199,    48,    49,   202,   180,   204,
     182,   205,   184,     3,   208,   102,   103,   104,    68,   652,
       3,   108,   655,   788,     3,   197,   198,   199,    75,    76,
     202,     4,   204,   199,   200,   201,     3,    84,     3,    86,
      46,    47,     6,    29,    30,     4,    32,   134,   135,   136,
     672,   673,   204,     6,     6,   102,   103,   104,    44,    45,
      55,   108,    70,    76,    91,    92,    93,    94,    95,   156,
       3,    98,    99,    64,   204,   194,    80,    80,   105,   106,
     107,   204,   204,   872,     4,     4,     4,   134,   135,   136,
      75,     4,     6,   180,   121,   182,   204,   184,   204,   204,
       3,     6,    80,   206,     6,    71,   205,   205,    57,   156,
     197,   198,   199,   204,   204,   202,     4,   204,   204,   204,
       7,   204,     9,    10,    11,    12,    13,    14,   204,    16,
      80,   204,   204,   180,   204,   182,    23,   184,    25,    26,
      27,     4,     4,   209,   205,    79,     3,   132,   204,   202,
     197,   198,   199,     6,     6,   202,     6,   204,     5,   204,
      46,   205,     6,   179,   149,   150,   151,   152,    75,   208,
      77,   129,   157,   204,   160,   161,   162,   163,    75,   165,
     166,     3,     3,   208,   170,   171,   172,   157,   205,   175,
     176,   177,     4,    80,    78,   204,   182,   183,   154,   131,
       6,   187,    75,     6,   132,   191,     3,   209,   209,   194,
     195,   196,   197,   198,   199,   200,   201,     6,    28,   208,
     205,   149,   150,   151,   152,   132,     6,   204,   204,   157,
     204,     6,   204,   204,     4,   132,   204,     3,     6,   208,
       4,   204,   149,   150,   151,   152,   153,   204,     4,   204,
     157,   205,   149,   150,   151,   152,    80,   154,   205,   132,
     157,   205,     6,     5,    50,    53,   194,   195,   196,   197,
     198,   199,   200,   201,     6,    59,   149,   150,   151,   152,
      59,   168,   194,     3,   157,   208,   131,   194,   195,   196,
     197,   198,   199,   200,   201,    54,     6,   194,   195,   196,
     197,   198,   199,   200,   201,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   204,   129,   133,
     205,   194,   195,   196,   197,   198,   199,   200,   201,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   204,   108,   109,   110,
     111,   112,   113,   202,     4,   116,   164,   205,   119,   120,
     202,   204,   123,   124,   125,   126,    91,    92,    93,    94,
      95,   205,   205,    98,    99,     6,     6,   204,    55,   208,
     105,   106,   107,    91,    92,    93,    94,    95,    56,   208,
      98,    99,   208,   208,    93,   208,   121,   105,   106,   107,
      91,    92,    93,    94,    95,    93,   208,    98,    99,   208,
     208,    93,   208,   121,   105,   106,   107,   208,   208,    93,
      93,   208,    93,   208,     3,   208,   208,   208,     6,    64,
     121,   208,   208,     6,   208,   208,   208,   208,     6,   208,
       6,   208,     6,   208,     6,   208,   208,     6,   208,     6,
     208,   208,     6,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,     6,   208,     6,   208,   208,   208,
     208,   208,   208,   208,   208,   208,     6,   208,   208,   208,
     208,   208,   208,     6,   208,   208,   208,   208,     6,   208,
     208,   208,     6,     6,     6,     6,     6,     6,     6,   208,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     208,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   208,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   208,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,   208,     6,     4,     4,     4,     4,   208,
     205,     6,   205,   205,   205,     6,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
       6,   205,   205,   205,     6,     6,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,     6,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,     6,   205,   205,   208,
     205,   205,   208,   205,   205,   205,     4,     4,   205,   205,
       6,     6,     6,   208,     6,     6,     4,     6,   205,    42,
     205,     6,    42,    51,   204,    42,    42,     3,   204,     3,
     205,   100,   205,    63,   558,   184,   539,    63,    63,    63,
      17,    63,   500,   625,    63,   592,   627,   359,    63,    63,
     670,    63,    63,   333,    63,   655,   675,   173,   163,   747,
     655,   384,   386,   218,   753,    -1,   668,    -1,    -1,    -1,
      -1,   642,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   287,    -1,    -1,    -1,    -1,   309,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   444
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   168,   186,   204,   211,   212,
     213,   215,   225,   226,   228,   230,   233,   234,   235,   236,
     237,   259,   264,   265,   266,   267,   268,   269,    29,    30,
      31,    41,    42,    33,    29,    30,    31,    41,    42,     3,
     257,    78,   257,   165,   166,   171,    35,    36,    37,    38,
      39,    40,    43,   229,    29,    30,    32,    44,    45,   160,
     161,   162,   163,   165,   166,   170,   171,   172,   175,   176,
     177,   182,   183,   187,   191,    30,   158,   159,   160,     3,
       3,   257,     3,   260,   261,   169,    16,   236,   237,     0,
     207,   317,    20,    22,    24,   253,     8,   238,   240,    74,
     316,   316,   316,   316,   316,   318,   257,    74,   315,   315,
     315,   315,   315,   206,    14,   257,    78,    79,   204,     3,
       3,     3,   214,   215,   225,   226,   230,   233,   234,   235,
     264,   265,   266,   267,   268,     3,   257,     6,   173,   174,
     173,   174,     3,   176,   165,   188,   189,   190,   189,   192,
     257,    64,    57,   208,     6,   159,   176,   177,   178,   205,
     205,   213,    21,   204,   239,   240,    67,   247,    69,   241,
      75,     3,   257,   257,   257,     3,    64,   204,   227,    76,
       3,   257,   257,   257,     3,     3,     3,   231,   232,    68,
     250,     4,   314,   314,     3,     4,     5,     6,    75,    76,
      84,    86,   102,   103,   104,   108,   134,   135,   136,   156,
     180,   182,   184,   197,   198,   199,   202,   204,   270,   272,
     273,   274,   276,   277,   278,   279,   280,   281,   284,   285,
     286,   287,   288,   290,   291,   292,   293,   294,   296,   297,
     298,   299,   300,   301,   302,   303,   306,   307,   308,   309,
     310,   311,     3,     5,     6,    64,   167,     3,     5,     6,
      64,   167,     3,     5,     6,    64,   167,    42,    46,    47,
      51,    52,     3,     3,     6,     4,   257,   204,   261,   314,
       6,     6,   239,   240,   270,    55,    70,   245,    76,    57,
     204,   227,   257,     3,   224,    34,   237,    64,   194,   208,
     250,   273,    80,    80,   204,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,    75,    76,   274,
     204,   204,    89,   273,   289,     4,     4,     4,     4,     6,
     311,   204,   120,   122,   124,   125,   204,   204,   274,   274,
       5,     6,   203,   294,   304,   305,   237,   273,   205,   208,
      57,   154,   155,    75,    77,   132,   149,   150,   151,   152,
     153,   157,   194,   195,   196,   197,   198,   199,   200,   201,
     206,   203,   208,   203,   208,   203,   208,   203,   208,   203,
     208,     3,     6,    80,   319,   238,    42,    52,     6,   205,
     205,    78,   248,   242,   243,   273,   273,    71,   246,   235,
       3,   128,   130,   216,   217,   218,   223,    57,   204,   323,
     205,   208,   204,   271,   257,   273,   232,   204,   204,    67,
     205,   270,   204,    75,   237,   273,   273,   289,    85,    87,
      89,     4,   204,   204,   204,   204,     4,     4,   209,   205,
     205,    79,   272,     3,   273,   273,    77,   157,   204,    75,
     131,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   274,   274,   274,     3,   199,   294,     6,   304,
       6,   305,     6,     5,    46,    48,    49,   204,   205,     6,
      29,    45,   204,   254,   255,   256,   257,   262,   179,   249,
     208,    72,    73,   244,   273,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   108,   109,   110,   111,   112,   113,   116,   119,
     120,   123,   124,   125,   126,   219,   129,   204,   205,   208,
     235,     3,     3,   270,   208,    70,    71,    81,    82,    83,
     185,   312,   313,   312,   270,   205,   237,   205,    57,    88,
      85,    87,   273,   273,    78,   273,     4,     3,   292,   273,
     205,   208,   205,   208,     5,     6,   314,   204,   274,   237,
     270,   131,   154,   209,   209,     6,     6,     3,   321,   322,
       6,   235,   208,    58,    60,    61,    62,    63,    65,    66,
     263,     3,    57,   258,   276,   277,   278,   279,   280,   281,
     282,   283,   250,   243,   204,   204,   204,   204,   204,   204,
     204,    75,   128,   130,   131,   220,   221,   319,   204,   224,
      28,   320,   217,   205,   205,   204,     6,     6,     4,     3,
       6,   205,   208,   205,   205,   205,   219,   273,   273,    85,
      88,   274,   208,   208,   208,   208,     4,     4,    80,   237,
     270,   205,   205,   274,    53,    50,   194,   205,   208,     6,
     205,   255,    59,    59,     3,   208,    54,   252,     6,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   121,
      91,    92,    93,    94,    95,    98,    99,   105,   106,   107,
     121,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   121,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   121,    91,    92,    93,    94,    95,    98,    99,
     105,   106,   107,   121,    91,    92,    93,    94,    95,    98,
      99,   105,   106,   107,   121,   131,   129,   133,   221,   222,
     222,   224,   205,   204,   164,   270,   313,   205,    85,   273,
     205,   202,   306,     4,   294,   202,   295,   298,   303,   306,
     205,   205,   204,   205,   205,     6,     6,     3,     5,     6,
     322,    30,    32,   258,   256,   256,   204,   282,    55,    56,
     251,   205,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,    93,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,    93,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,    93,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,    93,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,    93,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,    93,   208,   293,
     205,   321,     3,   205,     6,   208,   205,   208,   208,   208,
     312,     6,    64,   224,   270,   273,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   208,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   208,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   208,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   208,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     208,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   208,     6,   205,   319,     4,     4,     4,     4,   205,
       6,   273,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,     6,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,     6,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,     6,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,     6,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,     6,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,     6,   205,
     208,   205,   208,   205,   208,    42,    46,    47,    51,    52,
     205,   205,   205,   205,   205,   205,     4,     4,   319,     6,
       6,     6,   205,   208,   208,   275,   205,     6,    48,    49,
     319,     6,     4,   205,   127,   164,   319,    46,    47,     6,
     205,    42,    42,     6,    51,   127,   164,   319,   204,    42,
      42,     3,   204,   205,     3,   319,   205,   319
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   210,   211,   212,   212,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   215,   215,   215,   215,   215,   215,   216,   216,   217,
     217,   218,   218,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   220,   220,   221,   221,   221,
     221,   222,   222,   223,   223,   224,   224,   225,   226,   226,
     227,   227,   228,   229,   229,   229,   229,   229,   229,   229,
     229,   230,   231,   231,   232,   233,   233,   233,   233,   233,
     234,   234,   234,   235,   235,   235,   235,   236,   236,   237,
     238,   239,   239,   240,   241,   241,   242,   242,   243,   244,
     244,   244,   245,   245,   246,   246,   247,   247,   248,   248,
     249,   249,   250,   250,   251,   251,   252,   252,   253,   253,
     253,   253,   254,   254,   255,   255,   256,   256,   257,   257,
     258,   258,   258,   258,   259,   259,   260,   260,   261,   262,
     262,   263,   263,   263,   263,   263,   263,   263,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   265,   265,   265,   266,   266,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   268,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   270,   270,
     271,   271,   272,   272,   273,   273,   273,   273,   273,   274,
     274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
     274,   274,   275,   275,   276,   277,   277,   277,   277,   278,
     278,   278,   278,   279,   279,   280,   280,   281,   281,   282,
     282,   282,   282,   282,   282,   283,   283,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     284,   284,   284,   284,   284,   284,   284,   284,   284,   284,
     285,   285,   286,   287,   287,   288,   288,   288,   288,   289,
     289,   290,   291,   291,   291,   291,   292,   292,   292,   292,
     293,   293,   293,   293,   293,   293,   293,   293,   293,   293,
     293,   293,   294,   294,   294,   294,   295,   295,   295,   296,
     297,   297,   298,   298,   299,   300,   300,   301,   302,   302,
     303,   304,   305,   306,   306,   307,   308,   308,   309,   310,
     310,   311,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   311,   311,   312,   312,   313,   313,   313,   313,   313,
     313,   314,   315,   315,   316,   316,   317,   317,   318,   318,
     319,   319,   320,   320,   321,   321,   322,   322,   322,   322,
     323,   323
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
       4,     4,     4,     4,     4,     4,     4,     3,     3,     4,
       6,     7,     9,    10,    12,    12,    13,    14,    15,    16,
      12,    13,    15,    16,     3,     4,     5,     6,     1,     3,
       3,     5,     3,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     0,    12,    19,    15,    14,    12,    17,
      13,    12,    10,     7,     9,     4,     6,     4,     6,     1,
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
#line 2330 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2338 "parser.cpp"
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
#line 2352 "parser.cpp"
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
#line 2366 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2377 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2385 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2394 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2403 "parser.cpp"
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
#line 2417 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2428 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2448 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2458 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2468 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2478 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2488 "parser.cpp"
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
#line 2502 "parser.cpp"
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
#line 2516 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2526 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2534 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2542 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2551 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2559 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2567 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2575 "parser.cpp"
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
#line 2589 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2598 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2607 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2616 "parser.cpp"
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
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2638 "parser.cpp"
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
#line 2652 "parser.cpp"
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
#line 2666 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2685 "parser.cpp"
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
#line 2699 "parser.cpp"
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
#line 2716 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2724 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2732 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2748 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2756 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2764 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2772 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2780 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2788 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2796 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2804 "parser.cpp"
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
#line 2818 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2826 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2834 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2842 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2850 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2858 "parser.cpp"
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
#line 2871 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2887 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2895 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2911 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2919 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2927 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2935 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2943 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2951 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2959 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2999 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3007 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3015 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3023 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3031 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3039 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3047 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3055 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3066 "parser.cpp"
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
#line 3080 "parser.cpp"
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
#line 3094 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3105 "parser.cpp"
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

#line 3213 "parser.cpp"

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
#line 3428 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 498 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3439 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 504 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3450 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 511 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3456 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3462 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3468 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3474 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3480 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3486 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3492 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3498 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3504 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 520 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3510 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 521 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3516 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 522 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3522 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3528 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 524 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3534 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 526 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3540 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3546 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 528 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3552 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3558 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3564 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3570 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3576 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3582 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3588 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 535 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3594 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3600 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 537 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3606 "parser.cpp"
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
#line 3626 "parser.cpp"
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
#line 3644 "parser.cpp"
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
#line 3677 "parser.cpp"
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
#line 3697 "parser.cpp"
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
#line 3718 "parser.cpp"
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
#line 3751 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 669 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3760 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 673 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3769 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 679 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3777 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 682 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3785 "parser.cpp"
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
#line 3841 "parser.cpp"
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
#line 3883 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3889 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3895 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 781 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3901 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3907 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 783 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3913 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 784 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3919 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 785 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3925 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 786 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3931 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 787 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3937 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 788 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3943 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 789 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3949 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 790 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3955 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3961 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 792 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3967 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 793 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3973 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 794 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3979 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3985 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 796 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3991 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3997 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 798 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4003 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 799 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4009 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 802 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4015 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 804 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4021 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4027 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 806 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4033 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 807 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4039 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4045 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4051 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4057 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4063 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4069 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4075 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4081 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4087 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4093 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4099 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4105 "parser.cpp"
    break;

  case 80: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4111 "parser.cpp"
    break;

  case 81: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4117 "parser.cpp"
    break;

  case 82: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4123 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4129 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4135 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4141 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4147 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4153 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4159 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4165 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4171 "parser.cpp"
    break;

  case 91: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4177 "parser.cpp"
    break;

  case 92: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4183 "parser.cpp"
    break;

  case 93: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4189 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4195 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4201 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4207 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4213 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4219 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4225 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4231 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4237 "parser.cpp"
    break;

  case 102: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4243 "parser.cpp"
    break;

  case 103: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4249 "parser.cpp"
    break;

  case 104: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4255 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4261 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4267 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4273 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4279 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4285 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4291 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4297 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4303 "parser.cpp"
    break;

  case 113: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4309 "parser.cpp"
    break;

  case 114: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4315 "parser.cpp"
    break;

  case 115: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4321 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4327 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4333 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4339 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4345 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4351 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4357 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4363 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4369 "parser.cpp"
    break;

  case 124: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4375 "parser.cpp"
    break;

  case 125: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4381 "parser.cpp"
    break;

  case 126: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4387 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4393 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4399 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4405 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4411 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4417 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4423 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4429 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4435 "parser.cpp"
    break;

  case 135: /* column_constraints: column_constraint  */
#line 894 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4444 "parser.cpp"
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
#line 4458 "parser.cpp"
    break;

  case 137: /* column_constraint: PRIMARY KEY  */
#line 908 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4466 "parser.cpp"
    break;

  case 138: /* column_constraint: UNIQUE  */
#line 911 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4474 "parser.cpp"
    break;

  case 139: /* column_constraint: NULLABLE  */
#line 914 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4482 "parser.cpp"
    break;

  case 140: /* column_constraint: NOT NULLABLE  */
#line 917 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4490 "parser.cpp"
    break;

  case 141: /* default_expr: DEFAULT constant_expr  */
#line 921 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4498 "parser.cpp"
    break;

  case 142: /* default_expr: %empty  */
#line 924 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4506 "parser.cpp"
    break;

  case 143: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 929 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4516 "parser.cpp"
    break;

  case 144: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 934 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4526 "parser.cpp"
    break;

  case 145: /* identifier_array: IDENTIFIER  */
#line 941 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4537 "parser.cpp"
    break;

  case 146: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 947 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4548 "parser.cpp"
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
#line 4565 "parser.cpp"
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
#line 4604 "parser.cpp"
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
#line 4621 "parser.cpp"
    break;

  case 150: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1020 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4629 "parser.cpp"
    break;

  case 151: /* optional_identifier_array: %empty  */
#line 1023 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4637 "parser.cpp"
    break;

  case 152: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1030 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4647 "parser.cpp"
    break;

  case 153: /* explain_type: ANALYZE  */
#line 1036 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4655 "parser.cpp"
    break;

  case 154: /* explain_type: AST  */
#line 1039 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4663 "parser.cpp"
    break;

  case 155: /* explain_type: RAW  */
#line 1042 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4671 "parser.cpp"
    break;

  case 156: /* explain_type: LOGICAL  */
#line 1045 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4679 "parser.cpp"
    break;

  case 157: /* explain_type: PHYSICAL  */
#line 1048 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4687 "parser.cpp"
    break;

  case 158: /* explain_type: PIPELINE  */
#line 1051 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4695 "parser.cpp"
    break;

  case 159: /* explain_type: FRAGMENT  */
#line 1054 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4703 "parser.cpp"
    break;

  case 160: /* explain_type: %empty  */
#line 1057 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4711 "parser.cpp"
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
#line 4728 "parser.cpp"
    break;

  case 162: /* update_expr_array: update_expr  */
#line 1077 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4737 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1081 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4746 "parser.cpp"
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
#line 4758 "parser.cpp"
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
#line 4774 "parser.cpp"
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
#line 4792 "parser.cpp"
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
#line 4810 "parser.cpp"
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
#line 4828 "parser.cpp"
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
#line 4851 "parser.cpp"
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
#line 4909 "parser.cpp"
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
#line 4969 "parser.cpp"
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
#line 5021 "parser.cpp"
    break;

  case 173: /* select_statement: select_without_paren  */
#line 1339 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5029 "parser.cpp"
    break;

  case 174: /* select_statement: select_with_paren  */
#line 1342 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5037 "parser.cpp"
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
#line 5051 "parser.cpp"
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
#line 5065 "parser.cpp"
    break;

  case 177: /* select_with_paren: '(' select_without_paren ')'  */
#line 1364 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5073 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_with_paren ')'  */
#line 1367 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5081 "parser.cpp"
    break;

  case 179: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1371 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5090 "parser.cpp"
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
#line 5116 "parser.cpp"
    break;

  case 181: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1398 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5124 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1401 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5132 "parser.cpp"
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
#line 5152 "parser.cpp"
    break;

  case 184: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1422 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5160 "parser.cpp"
    break;

  case 185: /* order_by_clause: %empty  */
#line 1425 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5168 "parser.cpp"
    break;

  case 186: /* order_by_expr_list: order_by_expr  */
#line 1429 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5177 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1433 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5186 "parser.cpp"
    break;

  case 188: /* order_by_expr: expr order_by_type  */
#line 1438 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5196 "parser.cpp"
    break;

  case 189: /* order_by_type: ASC  */
#line 1444 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5204 "parser.cpp"
    break;

  case 190: /* order_by_type: DESC  */
#line 1447 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5212 "parser.cpp"
    break;

  case 191: /* order_by_type: %empty  */
#line 1450 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5220 "parser.cpp"
    break;

  case 192: /* limit_expr: LIMIT expr  */
#line 1454 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5228 "parser.cpp"
    break;

  case 193: /* limit_expr: %empty  */
#line 1458 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5234 "parser.cpp"
    break;

  case 194: /* offset_expr: OFFSET expr  */
#line 1460 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5242 "parser.cpp"
    break;

  case 195: /* offset_expr: %empty  */
#line 1464 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5248 "parser.cpp"
    break;

  case 196: /* distinct: DISTINCT  */
#line 1466 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5256 "parser.cpp"
    break;

  case 197: /* distinct: %empty  */
#line 1469 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5264 "parser.cpp"
    break;

  case 198: /* from_clause: FROM table_reference  */
#line 1473 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5272 "parser.cpp"
    break;

  case 199: /* from_clause: %empty  */
#line 1476 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5280 "parser.cpp"
    break;

  case 200: /* search_clause: SEARCH sub_search_array  */
#line 1480 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5290 "parser.cpp"
    break;

  case 201: /* search_clause: %empty  */
#line 1485 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5298 "parser.cpp"
    break;

  case 202: /* where_clause: WHERE expr  */
#line 1489 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5306 "parser.cpp"
    break;

  case 203: /* where_clause: %empty  */
#line 1492 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5314 "parser.cpp"
    break;

  case 204: /* having_clause: HAVING expr  */
#line 1496 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5322 "parser.cpp"
    break;

  case 205: /* having_clause: %empty  */
#line 1499 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5330 "parser.cpp"
    break;

  case 206: /* group_by_clause: GROUP BY expr_array  */
#line 1503 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5338 "parser.cpp"
    break;

  case 207: /* group_by_clause: %empty  */
#line 1506 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5346 "parser.cpp"
    break;

  case 208: /* set_operator: UNION  */
#line 1510 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5354 "parser.cpp"
    break;

  case 209: /* set_operator: UNION ALL  */
#line 1513 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5362 "parser.cpp"
    break;

  case 210: /* set_operator: INTERSECT  */
#line 1516 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5370 "parser.cpp"
    break;

  case 211: /* set_operator: EXCEPT  */
#line 1519 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5378 "parser.cpp"
    break;

  case 212: /* table_reference: table_reference_unit  */
#line 1527 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5386 "parser.cpp"
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
#line 5404 "parser.cpp"
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
#line 5422 "parser.cpp"
    break;

  case 217: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1561 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5433 "parser.cpp"
    break;

  case 218: /* table_name: IDENTIFIER  */
#line 1570 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5443 "parser.cpp"
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
#line 5455 "parser.cpp"
    break;

  case 220: /* table_alias: AS IDENTIFIER  */
#line 1584 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5465 "parser.cpp"
    break;

  case 221: /* table_alias: IDENTIFIER  */
#line 1589 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5475 "parser.cpp"
    break;

  case 222: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1594 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5486 "parser.cpp"
    break;

  case 223: /* table_alias: %empty  */
#line 1600 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5494 "parser.cpp"
    break;

  case 224: /* with_clause: WITH with_expr_list  */
#line 1607 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5502 "parser.cpp"
    break;

  case 225: /* with_clause: %empty  */
#line 1610 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5510 "parser.cpp"
    break;

  case 226: /* with_expr_list: with_expr  */
#line 1614 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5519 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1617 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5528 "parser.cpp"
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
#line 5540 "parser.cpp"
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
#line 5552 "parser.cpp"
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
#line 5565 "parser.cpp"
    break;

  case 231: /* join_type: INNER  */
#line 1655 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5573 "parser.cpp"
    break;

  case 232: /* join_type: LEFT  */
#line 1658 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5581 "parser.cpp"
    break;

  case 233: /* join_type: RIGHT  */
#line 1661 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5589 "parser.cpp"
    break;

  case 234: /* join_type: OUTER  */
#line 1664 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5597 "parser.cpp"
    break;

  case 235: /* join_type: FULL  */
#line 1667 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5605 "parser.cpp"
    break;

  case 236: /* join_type: CROSS  */
#line 1670 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5613 "parser.cpp"
    break;

  case 237: /* join_type: %empty  */
#line 1673 "parser.y"
                {
}
#line 5620 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW DATABASES  */
#line 1679 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5629 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLES  */
#line 1683 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5638 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW VIEWS  */
#line 1687 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5647 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW CONFIGS  */
#line 1691 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5656 "parser.cpp"
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
#line 5668 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW PROFILES  */
#line 1702 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5677 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW BUFFER  */
#line 1706 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5686 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW MEMINDEX  */
#line 1710 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5695 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW QUERIES  */
#line 1714 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5704 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1718 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5714 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TRANSACTIONS  */
#line 1723 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5723 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1727 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5733 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW SESSION VARIABLES  */
#line 1732 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5742 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1736 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5751 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1740 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5762 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1746 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5773 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1752 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5784 "parser.cpp"
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
#line 5800 "parser.cpp"
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
#line 5816 "parser.cpp"
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
#line 5832 "parser.cpp"
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
#line 5849 "parser.cpp"
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
#line 5866 "parser.cpp"
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
#line 5884 "parser.cpp"
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
#line 5903 "parser.cpp"
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
#line 5919 "parser.cpp"
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
#line 5938 "parser.cpp"
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
#line 5959 "parser.cpp"
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
#line 5981 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW LOGS  */
#line 1900 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5990 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DELTA LOGS  */
#line 1904 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5999 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW CATALOGS  */
#line 1908 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6008 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1912 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6017 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1916 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6026 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1920 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6037 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW MEMORY  */
#line 1926 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6046 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1930 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6055 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1934 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6064 "parser.cpp"
    break;

  case 275: /* flush_statement: FLUSH DATA  */
#line 1942 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6073 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH LOG  */
#line 1946 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6082 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH BUFFER  */
#line 1950 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6091 "parser.cpp"
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
#line 6106 "parser.cpp"
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
#line 6130 "parser.cpp"
    break;

  case 280: /* command_statement: USE IDENTIFIER  */
#line 1992 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6141 "parser.cpp"
    break;

  case 281: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1998 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6151 "parser.cpp"
    break;

  case 282: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2003 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6162 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2009 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6173 "parser.cpp"
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
#line 6186 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2023 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6197 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2029 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6208 "parser.cpp"
    break;

  case 287: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2035 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6219 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2041 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6230 "parser.cpp"
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
#line 6243 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2055 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6254 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2061 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6265 "parser.cpp"
    break;

  case 292: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2067 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6276 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2073 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6287 "parser.cpp"
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
#line 6300 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2087 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6311 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2093 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6322 "parser.cpp"
    break;

  case 297: /* compact_statement: COMPACT TABLE table_name  */
#line 2100 "parser.y"
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
#line 6339 "parser.cpp"
    break;

  case 298: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2113 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6348 "parser.cpp"
    break;

  case 299: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2117 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6358 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2122 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6369 "parser.cpp"
    break;

  case 301: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2128 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6381 "parser.cpp"
    break;

  case 302: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2135 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6394 "parser.cpp"
    break;

  case 303: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2143 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6408 "parser.cpp"
    break;

  case 304: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2152 "parser.y"
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
#line 6423 "parser.cpp"
    break;

  case 305: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2162 "parser.y"
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
#line 6438 "parser.cpp"
    break;

  case 306: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2172 "parser.y"
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
#line 6454 "parser.cpp"
    break;

  case 307: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2183 "parser.y"
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
#line 6470 "parser.cpp"
    break;

  case 308: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2194 "parser.y"
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
#line 6487 "parser.cpp"
    break;

  case 309: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2206 "parser.y"
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
#line 6504 "parser.cpp"
    break;

  case 310: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2218 "parser.y"
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
#line 6519 "parser.cpp"
    break;

  case 311: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2228 "parser.y"
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
#line 6535 "parser.cpp"
    break;

  case 312: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2239 "parser.y"
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
#line 6552 "parser.cpp"
    break;

  case 313: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2251 "parser.y"
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
#line 6570 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW LOGS  */
#line 2264 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6579 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2268 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6589 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2273 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6599 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2278 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6610 "parser.cpp"
    break;

  case 318: /* expr_array: expr_alias  */
#line 2289 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6619 "parser.cpp"
    break;

  case 319: /* expr_array: expr_array ',' expr_alias  */
#line 2293 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6628 "parser.cpp"
    break;

  case 320: /* expr_array_list: '(' expr_array ')'  */
#line 2298 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6637 "parser.cpp"
    break;

  case 321: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2302 "parser.y"
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
        YYERROR;
    }
    (yyvsp[-4].expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
    (yyval.expr_array_list_t) = (yyvsp[-4].expr_array_list_t);
}
#line 6657 "parser.cpp"
    break;

  case 322: /* expr_alias: expr AS IDENTIFIER  */
#line 2329 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6668 "parser.cpp"
    break;

  case 323: /* expr_alias: expr  */
#line 2335 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6676 "parser.cpp"
    break;

  case 329: /* operand: '(' expr ')'  */
#line 2345 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6684 "parser.cpp"
    break;

  case 330: /* operand: '(' select_without_paren ')'  */
#line 2348 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6695 "parser.cpp"
    break;

  case 331: /* operand: constant_expr  */
#line 2354 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6703 "parser.cpp"
    break;

  case 342: /* extra_match_tensor_option: ',' STRING  */
#line 2368 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6711 "parser.cpp"
    break;

  case 343: /* extra_match_tensor_option: %empty  */
#line 2371 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6719 "parser.cpp"
    break;

  case 344: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2377 "parser.y"
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
#line 6740 "parser.cpp"
    break;

  case 345: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2396 "parser.y"
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
#line 6789 "parser.cpp"
    break;

  case 346: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2441 "parser.y"
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
#line 6831 "parser.cpp"
    break;

  case 347: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2479 "parser.y"
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
#line 6877 "parser.cpp"
    break;

  case 348: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2521 "parser.y"
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
#line 6924 "parser.cpp"
    break;

  case 349: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' USING INDEX '(' IDENTIFIER ')' with_index_param_list  */
#line 2567 "parser.y"
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
#line 6949 "parser.cpp"
    break;

  case 350: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' IGNORE INDEX  */
#line 2588 "parser.y"
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
#line 6973 "parser.cpp"
    break;

  case 351: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2608 "parser.y"
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
#line 6995 "parser.cpp"
    break;

  case 352: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2626 "parser.y"
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
#line 7017 "parser.cpp"
    break;

  case 353: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2644 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7030 "parser.cpp"
    break;

  case 354: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2652 "parser.y"
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
#line 7045 "parser.cpp"
    break;

  case 355: /* query_expr: QUERY '(' STRING ')'  */
#line 2663 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7056 "parser.cpp"
    break;

  case 356: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2669 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7069 "parser.cpp"
    break;

  case 357: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2678 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7080 "parser.cpp"
    break;

  case 358: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2684 "parser.y"
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
#line 7096 "parser.cpp"
    break;

  case 359: /* sub_search: match_vector_expr  */
#line 2696 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7104 "parser.cpp"
    break;

  case 360: /* sub_search: match_text_expr  */
#line 2699 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7112 "parser.cpp"
    break;

  case 361: /* sub_search: match_tensor_expr  */
#line 2702 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7120 "parser.cpp"
    break;

  case 362: /* sub_search: match_sparse_expr  */
#line 2705 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7128 "parser.cpp"
    break;

  case 363: /* sub_search: query_expr  */
#line 2708 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7136 "parser.cpp"
    break;

  case 364: /* sub_search: fusion_expr  */
#line 2711 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7144 "parser.cpp"
    break;

  case 365: /* sub_search_array: sub_search  */
#line 2715 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7153 "parser.cpp"
    break;

  case 366: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2719 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7162 "parser.cpp"
    break;

  case 367: /* function_expr: IDENTIFIER '(' ')'  */
#line 2724 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 7175 "parser.cpp"
    break;

  case 368: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2732 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 7188 "parser.cpp"
    break;

  case 369: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2740 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 7202 "parser.cpp"
    break;

  case 370: /* function_expr: operand IS NOT NULLABLE  */
#line 2749 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7214 "parser.cpp"
    break;

  case 371: /* function_expr: operand IS NULLABLE  */
#line 2756 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7226 "parser.cpp"
    break;

  case 372: /* function_expr: NOT operand  */
#line 2763 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7238 "parser.cpp"
    break;

  case 373: /* function_expr: '-' operand  */
#line 2770 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7250 "parser.cpp"
    break;

  case 374: /* function_expr: '+' operand  */
#line 2777 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7262 "parser.cpp"
    break;

  case 375: /* function_expr: operand '-' operand  */
#line 2784 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7275 "parser.cpp"
    break;

  case 376: /* function_expr: operand '+' operand  */
#line 2792 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7288 "parser.cpp"
    break;

  case 377: /* function_expr: operand '*' operand  */
#line 2800 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7301 "parser.cpp"
    break;

  case 378: /* function_expr: operand '/' operand  */
#line 2808 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7314 "parser.cpp"
    break;

  case 379: /* function_expr: operand '%' operand  */
#line 2816 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7327 "parser.cpp"
    break;

  case 380: /* function_expr: operand '=' operand  */
#line 2824 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7340 "parser.cpp"
    break;

  case 381: /* function_expr: operand EQUAL operand  */
#line 2832 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7353 "parser.cpp"
    break;

  case 382: /* function_expr: operand NOT_EQ operand  */
#line 2840 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7366 "parser.cpp"
    break;

  case 383: /* function_expr: operand '<' operand  */
#line 2848 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7379 "parser.cpp"
    break;

  case 384: /* function_expr: operand '>' operand  */
#line 2856 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7392 "parser.cpp"
    break;

  case 385: /* function_expr: operand LESS_EQ operand  */
#line 2864 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7405 "parser.cpp"
    break;

  case 386: /* function_expr: operand GREATER_EQ operand  */
#line 2872 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7418 "parser.cpp"
    break;

  case 387: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2880 "parser.y"
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
#line 7453 "parser.cpp"
    break;

  case 388: /* function_expr: operand LIKE operand  */
#line 2910 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7466 "parser.cpp"
    break;

  case 389: /* function_expr: operand NOT LIKE operand  */
#line 2918 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7479 "parser.cpp"
    break;

  case 390: /* conjunction_expr: expr AND expr  */
#line 2927 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7492 "parser.cpp"
    break;

  case 391: /* conjunction_expr: expr OR expr  */
#line 2935 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7505 "parser.cpp"
    break;

  case 392: /* between_expr: operand BETWEEN operand AND operand  */
#line 2944 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7517 "parser.cpp"
    break;

  case 393: /* in_expr: operand IN '(' expr_array ')'  */
#line 2952 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7528 "parser.cpp"
    break;

  case 394: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2958 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7539 "parser.cpp"
    break;

  case 395: /* case_expr: CASE expr case_check_array END  */
#line 2965 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7550 "parser.cpp"
    break;

  case 396: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2971 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7562 "parser.cpp"
    break;

  case 397: /* case_expr: CASE case_check_array END  */
#line 2978 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7572 "parser.cpp"
    break;

  case 398: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2983 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7583 "parser.cpp"
    break;

  case 399: /* case_check_array: WHEN expr THEN expr  */
#line 2990 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7595 "parser.cpp"
    break;

  case 400: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2997 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7607 "parser.cpp"
    break;

  case 401: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 3005 "parser.y"
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
#line 7638 "parser.cpp"
    break;

  case 402: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3032 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7649 "parser.cpp"
    break;

  case 403: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3038 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7660 "parser.cpp"
    break;

  case 404: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 3044 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7672 "parser.cpp"
    break;

  case 405: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 3051 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7684 "parser.cpp"
    break;

  case 406: /* column_expr: IDENTIFIER  */
#line 3059 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7696 "parser.cpp"
    break;

  case 407: /* column_expr: column_expr '.' IDENTIFIER  */
#line 3066 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7708 "parser.cpp"
    break;

  case 408: /* column_expr: '*'  */
#line 3073 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7718 "parser.cpp"
    break;

  case 409: /* column_expr: column_expr '.' '*'  */
#line 3078 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7732 "parser.cpp"
    break;

  case 410: /* constant_expr: STRING  */
#line 3088 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7742 "parser.cpp"
    break;

  case 411: /* constant_expr: TRUE  */
#line 3093 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7752 "parser.cpp"
    break;

  case 412: /* constant_expr: FALSE  */
#line 3098 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7762 "parser.cpp"
    break;

  case 413: /* constant_expr: DOUBLE_VALUE  */
#line 3103 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7772 "parser.cpp"
    break;

  case 414: /* constant_expr: LONG_VALUE  */
#line 3108 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7782 "parser.cpp"
    break;

  case 415: /* constant_expr: DATE STRING  */
#line 3113 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7792 "parser.cpp"
    break;

  case 416: /* constant_expr: TIME STRING  */
#line 3118 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7802 "parser.cpp"
    break;

  case 417: /* constant_expr: DATETIME STRING  */
#line 3123 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7812 "parser.cpp"
    break;

  case 418: /* constant_expr: TIMESTAMP STRING  */
#line 3128 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7822 "parser.cpp"
    break;

  case 419: /* constant_expr: INTERVAL interval_expr  */
#line 3133 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7830 "parser.cpp"
    break;

  case 420: /* constant_expr: interval_expr  */
#line 3136 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7838 "parser.cpp"
    break;

  case 421: /* constant_expr: common_array_expr  */
#line 3139 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7846 "parser.cpp"
    break;

  case 422: /* common_array_expr: array_expr  */
#line 3143 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7854 "parser.cpp"
    break;

  case 423: /* common_array_expr: subarray_array_expr  */
#line 3146 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7862 "parser.cpp"
    break;

  case 424: /* common_array_expr: sparse_array_expr  */
#line 3149 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7870 "parser.cpp"
    break;

  case 425: /* common_array_expr: empty_array_expr  */
#line 3152 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7878 "parser.cpp"
    break;

  case 426: /* common_sparse_array_expr: sparse_array_expr  */
#line 3156 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7886 "parser.cpp"
    break;

  case 427: /* common_sparse_array_expr: array_expr  */
#line 3159 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7894 "parser.cpp"
    break;

  case 428: /* common_sparse_array_expr: empty_array_expr  */
#line 3162 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7902 "parser.cpp"
    break;

  case 429: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3166 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7910 "parser.cpp"
    break;

  case 430: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3170 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7920 "parser.cpp"
    break;

  case 431: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3175 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7929 "parser.cpp"
    break;

  case 432: /* sparse_array_expr: long_sparse_array_expr  */
#line 3180 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7937 "parser.cpp"
    break;

  case 433: /* sparse_array_expr: double_sparse_array_expr  */
#line 3183 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7945 "parser.cpp"
    break;

  case 434: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3187 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7953 "parser.cpp"
    break;

  case 435: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3191 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7965 "parser.cpp"
    break;

  case 436: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3198 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7976 "parser.cpp"
    break;

  case 437: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3205 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7984 "parser.cpp"
    break;

  case 438: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3209 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7996 "parser.cpp"
    break;

  case 439: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3216 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8007 "parser.cpp"
    break;

  case 440: /* empty_array_expr: '[' ']'  */
#line 3223 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8015 "parser.cpp"
    break;

  case 441: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3227 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8023 "parser.cpp"
    break;

  case 442: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3231 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8031 "parser.cpp"
    break;

  case 443: /* array_expr: long_array_expr  */
#line 3235 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8039 "parser.cpp"
    break;

  case 444: /* array_expr: double_array_expr  */
#line 3238 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8047 "parser.cpp"
    break;

  case 445: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3242 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8055 "parser.cpp"
    break;

  case 446: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3246 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8065 "parser.cpp"
    break;

  case 447: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3251 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8074 "parser.cpp"
    break;

  case 448: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3256 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8082 "parser.cpp"
    break;

  case 449: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3260 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8092 "parser.cpp"
    break;

  case 450: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3265 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8101 "parser.cpp"
    break;

  case 451: /* interval_expr: LONG_VALUE SECONDS  */
#line 3270 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8112 "parser.cpp"
    break;

  case 452: /* interval_expr: LONG_VALUE SECOND  */
#line 3276 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8123 "parser.cpp"
    break;

  case 453: /* interval_expr: LONG_VALUE MINUTES  */
#line 3282 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8134 "parser.cpp"
    break;

  case 454: /* interval_expr: LONG_VALUE MINUTE  */
#line 3288 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8145 "parser.cpp"
    break;

  case 455: /* interval_expr: LONG_VALUE HOURS  */
#line 3294 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8156 "parser.cpp"
    break;

  case 456: /* interval_expr: LONG_VALUE HOUR  */
#line 3300 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8167 "parser.cpp"
    break;

  case 457: /* interval_expr: LONG_VALUE DAYS  */
#line 3306 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8178 "parser.cpp"
    break;

  case 458: /* interval_expr: LONG_VALUE DAY  */
#line 3312 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8189 "parser.cpp"
    break;

  case 459: /* interval_expr: LONG_VALUE MONTHS  */
#line 3318 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8200 "parser.cpp"
    break;

  case 460: /* interval_expr: LONG_VALUE MONTH  */
#line 3324 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8211 "parser.cpp"
    break;

  case 461: /* interval_expr: LONG_VALUE YEARS  */
#line 3330 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8222 "parser.cpp"
    break;

  case 462: /* interval_expr: LONG_VALUE YEAR  */
#line 3336 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8233 "parser.cpp"
    break;

  case 463: /* copy_option_list: copy_option  */
#line 3347 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8242 "parser.cpp"
    break;

  case 464: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3351 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8251 "parser.cpp"
    break;

  case 465: /* copy_option: FORMAT IDENTIFIER  */
#line 3356 "parser.y"
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
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 8286 "parser.cpp"
    break;

  case 466: /* copy_option: DELIMITER STRING  */
#line 3386 "parser.y"
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
#line 8301 "parser.cpp"
    break;

  case 467: /* copy_option: HEADER  */
#line 3396 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8311 "parser.cpp"
    break;

  case 468: /* copy_option: OFFSET LONG_VALUE  */
#line 3401 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8321 "parser.cpp"
    break;

  case 469: /* copy_option: LIMIT LONG_VALUE  */
#line 3406 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8331 "parser.cpp"
    break;

  case 470: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3411 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8341 "parser.cpp"
    break;

  case 471: /* file_path: STRING  */
#line 3417 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8349 "parser.cpp"
    break;

  case 472: /* if_exists: IF EXISTS  */
#line 3421 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8355 "parser.cpp"
    break;

  case 473: /* if_exists: %empty  */
#line 3422 "parser.y"
  { (yyval.bool_value) = false; }
#line 8361 "parser.cpp"
    break;

  case 474: /* if_not_exists: IF NOT EXISTS  */
#line 3424 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8367 "parser.cpp"
    break;

  case 475: /* if_not_exists: %empty  */
#line 3425 "parser.y"
  { (yyval.bool_value) = false; }
#line 8373 "parser.cpp"
    break;

  case 478: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3440 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8386 "parser.cpp"
    break;

  case 479: /* if_not_exists_info: %empty  */
#line 3448 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8394 "parser.cpp"
    break;

  case 480: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3452 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8402 "parser.cpp"
    break;

  case 481: /* with_index_param_list: %empty  */
#line 3455 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8410 "parser.cpp"
    break;

  case 482: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3459 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8418 "parser.cpp"
    break;

  case 483: /* optional_table_properties_list: %empty  */
#line 3462 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8426 "parser.cpp"
    break;

  case 484: /* index_param_list: index_param  */
#line 3466 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8435 "parser.cpp"
    break;

  case 485: /* index_param_list: index_param_list ',' index_param  */
#line 3470 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8444 "parser.cpp"
    break;

  case 486: /* index_param: IDENTIFIER  */
#line 3475 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8455 "parser.cpp"
    break;

  case 487: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3481 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8470 "parser.cpp"
    break;

  case 488: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3491 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8483 "parser.cpp"
    break;

  case 489: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3499 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8496 "parser.cpp"
    break;

  case 490: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3510 "parser.y"
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
#line 8532 "parser.cpp"
    break;

  case 491: /* index_info: '(' IDENTIFIER ')'  */
#line 3541 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8543 "parser.cpp"
    break;


#line 8547 "parser.cpp"

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

#line 3548 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

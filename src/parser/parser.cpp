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
#define YYLAST   1321

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  210
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  492
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1079

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
    3324,  3330,  3336,  3347,  3351,  3356,  3387,  3397,  3402,  3407,
    3412,  3418,  3422,  3423,  3425,  3426,  3428,  3429,  3441,  3449,
    3453,  3456,  3460,  3463,  3467,  3471,  3476,  3482,  3492,  3502,
    3510,  3521,  3552
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
      65,   339,   109,   433,    82,    21,    82,   129,   519,   590,
      88,   240,   210,   214,   225,    75,   236,    -9,   267,    97,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,    83,  -533,
    -533,   282,  -533,  -533,  -533,  -533,  -533,  -533,   228,   228,
     228,   228,    87,    82,   244,   244,   244,   244,   244,   116,
     318,    82,   -12,   341,   356,   361,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,   266,   380,    82,  -533,  -533,  -533,  -533,
    -533,   372,  -533,   163,   353,  -533,   387,  -533,   230,  -533,
    -533,   239,  -533,   375,    67,    82,  -533,  -533,  -533,  -533,
     -18,  -533,   355,   219,  -533,   416,   -64,   226,   241,  -533,
      26,  -533,   415,  -533,  -533,     6,   390,  -533,   392,   384,
     466,    82,    82,    82,   473,   423,   293,   426,   506,    82,
      82,    82,   513,   515,   528,   475,   546,   546,   444,    95,
     105,   132,  -533,  -533,  -533,  -533,  -533,  -533,  -533,    83,
    -533,  -533,  -533,  -533,  -533,  -533,   447,  -533,  -533,   567,
    -533,   578,  -533,  -533,   577,   585,  -533,  -533,  -533,  -533,
    -533,    82,   403,   225,   546,   615,  -533,  -533,   617,  -533,
    -533,  -533,  -533,     6,  -533,  -533,  -533,   444,   570,   559,
     562,  -533,   -13,  -533,   293,  -533,    82,   650,    39,  -533,
    -533,  -533,  -533,  -533,   576,  -533,   451,   -11,  -533,   444,
    -533,  -533,   582,   588,   474,  -533,  -533,   834,   529,   493,
     495,   369,   696,   697,   698,   699,  -533,  -533,   700,   500,
     188,   501,   503,   231,   231,  -533,    13,   404,   -62,  -533,
      -7,   541,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,   502,  -533,  -533,  -533,  -140,
    -533,  -533,    50,  -533,    52,  -533,  -533,  -533,    60,  -533,
     113,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,   707,   717,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,   644,   282,  -533,  -533,
     177,   723,   525,   527,   -44,   444,   444,   663,  -533,    -9,
      59,   686,   540,  -533,    66,   542,  -533,    82,   444,   528,
    -533,   172,   543,   544,   221,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,   231,   545,   647,
     665,   444,   444,   111,   297,  -533,  -533,  -533,  -533,   834,
    -533,   750,   553,   554,   555,   560,   764,   766,   450,   450,
    -533,   565,  -533,  -533,  -533,  -533,   558,   -61,   692,   444,
     772,   444,   444,   -29,   572,     9,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   231,   231,
      22,  -533,   580,  -533,   774,  -533,   777,  -533,   778,  -533,
     773,   739,   488,   586,  -533,   587,   787,  -533,    41,  -533,
    -533,    10,   616,   592,  -533,    77,   172,   444,  -533,    83,
     759,   672,   598,    76,  -533,  -533,  -533,    -9,   800,  -533,
    -533,   802,   444,   599,  -533,   172,  -533,    85,    85,   444,
    -533,   133,   665,   649,   603,     3,   -24,   352,  -533,   444,
     444,   736,   444,   811,    24,   444,   153,   171,   593,  -533,
    -533,   546,  -533,  -533,  -533,   664,   618,   231,   404,   688,
    -533,   460,   460,   196,   196,   637,   460,   460,   196,   196,
     450,   450,  -533,  -533,  -533,  -533,  -533,  -533,   611,  -533,
     612,  -533,  -533,  -533,   820,   821,  -533,   825,  -533,  -533,
     823,  -533,    -9,   622,   621,  -533,    84,  -533,   272,   475,
     444,  -533,  -533,  -533,   172,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,   636,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,   662,
     669,   670,   673,   676,   677,   223,   687,   650,   848,    59,
      83,   690,  -533,   180,   693,   886,   890,   894,   896,  -533,
     902,   209,  -533,   216,   235,  -533,   704,  -533,   759,   444,
    -533,   444,    44,    16,   231,   -72,   705,  -533,    -1,    49,
    -533,   908,  -533,   910,  -533,  -533,   835,   404,   460,   720,
     273,  -533,   231,   920,   924,   877,   881,   738,   277,  -533,
     936,    34,    10,   884,  -533,  -533,  -533,  -533,  -533,  -533,
     887,  -533,   944,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,   740,   895,  -533,   953,   718,   795,   812,   829,   846,
     863,   832,   831,  -533,  -533,   212,  -533,   833,   650,   278,
     760,  -533,  -533,   801,  -533,   444,  -533,  -533,  -533,  -533,
    -533,  -533,    85,  -533,  -533,  -533,   780,   172,   165,  -533,
     444,   520,   781,   982,   580,   785,   783,   784,   786,   788,
     305,  -533,  -533,   647,   985,   986,   462,  -533,   825,   320,
      84,   621,    10,    10,   790,   272,   940,   941,   309,   791,
     792,   793,   794,   796,   797,   798,   799,   803,   903,   804,
     805,   806,   807,   808,   809,   810,   813,   814,   815,   905,
     816,   817,   818,   819,   822,   824,   826,   827,   828,   830,
     915,   836,   837,   838,   839,   840,   841,   842,   843,   844,
     845,   916,   847,   849,   850,   851,   852,   853,   854,   855,
     856,   857,   917,   858,   859,   860,   861,   862,   864,   865,
     866,   867,   868,   926,   869,  -533,  -533,    18,  -533,  -533,
    -533,   336,  -533,   825,  1000,   364,  -533,  -533,  -533,   172,
    -533,   661,   870,   377,   871,    15,   872,  -533,  -533,  -533,
    -533,  -533,    85,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  1014,  -533,  -533,  -533,   964,   650,  -533,   444,
     444,  -533,  -533,  1023,  1025,  1027,  1031,  1033,  1034,  1035,
    1036,  1037,  1048,   873,  1050,  1065,  1076,  1077,  1078,  1079,
    1080,  1081,  1082,  1083,   882,  1085,  1086,  1087,  1088,  1089,
    1090,  1091,  1092,  1093,  1094,   893,  1096,  1097,  1098,  1099,
    1100,  1101,  1102,  1103,  1104,  1105,   904,  1107,  1108,  1109,
    1110,  1111,  1112,  1113,  1114,  1115,  1116,   918,  1117,  1118,
    1119,  1121,  1122,  1123,  1124,  1125,  1126,  1127,   927,  1128,
    -533,  -533,   382,   644,  -533,  -533,  1132,  -533,  1133,  1134,
    1135,   388,  1136,   444,   389,   932,   172,   938,   939,   942,
     943,   945,   946,   947,   948,   949,   950,  1139,   951,   952,
     954,   955,   956,   957,   958,   959,   960,   961,  1140,   962,
     963,   965,   966,   967,   968,   969,   970,   971,   972,  1143,
     973,   974,   975,   976,   977,   978,   979,   980,   981,   983,
    1152,   984,   987,   988,   989,   990,   991,   992,   993,   994,
     995,  1163,   996,   997,   998,   999,  1001,  1002,  1003,  1004,
    1005,  1006,  1181,  1007,  -533,  -533,   933,  1008,  1009,   422,
    -533,   449,   172,  -533,  -533,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  1010,  -533,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  1011,  -533,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  1013,  -533,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  1015,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  1016,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  -533,  1017,
    -533,  1186,  -533,  1187,   644,  1199,  1208,  1213,  -533,  -533,
    -533,  -533,  -533,  -533,  -533,  -533,  -533,   431,  1018,  -533,
    1019,  1217,   627,   644,  1219,  1223,  1024,   121,   642,  1222,
    -533,  -533,  1026,  -533,  -533,  1188,  1190,  -533,  1227,  -533,
    1183,   122,  -533,  1032,  -533,  -533,  1193,  1195,  -533,  1235,
    -533,  1038,  1039,  1236,   644,  1040,  -533,   644,  -533
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
       0,    33,    38,   492,   320,     0,   469,   468,   466,   465,
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
     356,   358,     0,   405,   394,   265,   261,   487,   488,   490,
     489,   485,     0,   302,   217,   229,     0,     0,   366,     0,
       0,   183,    67,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     141,   143,     0,   481,   321,   446,     0,   353,     0,     0,
       0,     0,   303,     0,     0,   206,   204,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   482,   491,     0,     0,     0,     0,
     171,     0,   230,   222,    66,    72,    73,    70,    71,    74,
      75,    76,    77,    78,     0,    69,   116,   117,   114,   115,
     118,   119,   120,   121,   122,     0,   113,    83,    84,    81,
      82,    85,    86,    87,    88,    89,     0,    80,    94,    95,
      92,    93,    96,    97,    98,    99,   100,     0,    91,   127,
     128,   125,   126,   129,   130,   131,   132,   133,     0,   124,
     105,   106,   103,   104,   107,   108,   109,   110,   111,     0,
     102,     0,   354,     0,   481,     0,     0,     0,   305,   304,
     310,    79,   123,    90,   101,   134,   112,     0,   343,   352,
       0,   311,   306,   481,     0,     0,     0,   481,     0,     0,
     307,   348,     0,   342,   344,     0,     0,   351,     0,   312,
     308,   481,   350,     0,   313,   309,     0,     0,   347,     0,
     346,     0,     0,     0,   481,     0,   349,   481,   345
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -533,  -533,  -533,  1141,  -533,  1177,  -533,   708,  -533,   685,
    -533,   623,   619,  -533,  -526,  1189,  1191,  1066,  -533,  -533,
    1192,  -533,  1012,  1194,  1196,   -54,  1232,   -16,  1020,  1095,
     -46,  -533,  -533,   751,  -533,  -533,  -533,  -533,  -533,  -533,
    -190,  -533,  -533,  -533,  -533,   666,    23,     4,   583,  -533,
    -533,  1106,  -533,  -533,  1197,  1198,  1200,  1201,  1202,  -533,
    -175,  -533,   897,  -199,  -193,  -533,  -482,  -469,  -466,  -453,
    -437,  -433,   591,  -533,  -533,  -533,  -533,  -533,  -533,   929,
    -533,  -533,   874,   523,  -222,  -533,  -533,  -533,   620,  -533,
    -533,  -533,  -533,   624,   883,   885,    -8,  -533,  -533,  -533,
    -533,  1054,  -423,   631,  -121,   476,   535,  -533,  -533,  -532,
    -533,   521,   608,  -533
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   132,    21,   413,   414,   415,   535,
     625,   626,   749,   416,   304,    22,    23,   188,    24,    63,
      25,   197,   198,    26,    27,    28,    29,    30,   107,   174,
     108,   179,   403,   404,   503,   297,   408,   177,   402,   499,
     200,   791,   677,   105,   493,   494,   495,   496,   603,    31,
      93,    94,   497,   600,    32,    33,    34,    35,    36,    37,
     228,   423,   229,   230,   231,  1046,   232,   233,   234,   235,
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
     311,    98,   294,   627,   353,   553,   203,   310,    50,   139,
      52,   629,   333,    49,   106,   329,   604,    91,   350,   351,
     350,   351,   205,   206,   207,   475,  -476,   567,   357,   605,
     348,   349,   606,     1,   401,     2,     3,     4,     5,     6,
       7,     8,     9,   289,   299,   607,   161,   116,   456,    10,
     360,    11,    12,    13,   102,   125,   103,   199,   104,   175,
     558,   608,   410,   381,   559,   609,   126,   127,   382,   146,
     490,    14,     1,   305,     2,     3,     4,     5,     6,     7,
       8,     9,   361,   362,   459,    49,   491,   601,    10,   160,
      11,    12,    13,   361,   362,   165,   405,   406,   262,    51,
     263,   264,   751,   102,   650,   103,    14,   104,   267,   425,
     268,   269,   166,   167,   168,   182,   183,   184,    85,    14,
     212,   213,   214,   191,   192,   193,   215,   293,   457,   649,
     361,   362,   435,   436,   329,   272,   652,   273,   274,   431,
     460,   602,    43,   358,   450,    14,   359,   361,   362,   501,
     502,  -479,   216,   217,   218,   545,   546,   361,   362,   265,
     477,   109,   454,   455,   359,   286,   547,   548,   549,   270,
     361,   362,   306,   461,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   472,   473,   474,   411,   123,   412,
     302,   300,   128,   604,    15,    17,   275,   309,   361,   362,
     332,   393,   393,   361,   362,   380,   605,   654,   504,   606,
     173,   356,    16,    89,   492,   226,   352,    90,   352,   396,
     226,   476,   607,   225,   204,   205,   206,   207,    92,   397,
      17,   361,   362,    15,   204,   205,   206,   207,   608,   670,
     562,   563,   609,   565,    95,   409,   569,   543,  1055,  1066,
     758,    16,    96,   383,   554,   385,   158,   655,   384,   159,
     386,   874,   266,   387,   578,   361,   362,    99,   388,    17,
     550,   420,   271,     1,   421,     2,     3,     4,     5,     6,
       7,   538,     9,   580,   539,  1056,  1067,   621,   429,    10,
     106,    11,    12,    13,    53,    54,   208,   209,   621,   276,
      55,   405,   109,   393,   100,   210,   327,   211,   342,   612,
     343,   424,   344,   345,   434,   210,   389,   211,   117,   361,
     362,   390,   123,   212,   213,   214,   361,   362,   365,   215,
     576,   945,   124,   212,   213,   214,   148,   149,   555,   215,
     622,   359,   623,   624,   129,   747,    14,  -480,  -480,   871,
     782,   622,   783,   623,   624,   216,   217,   218,   570,   130,
     647,   571,   648,   540,   131,   216,   217,   218,    38,    39,
      40,   651,   204,   205,   206,   207,   572,   219,   147,   573,
      41,    42,   438,   145,   439,   634,   440,   219,   359,   663,
     152,  -480,  -480,   375,   376,   377,   378,   379,    86,    87,
      88,   220,   660,   221,   154,   222,   153,   204,   205,   206,
     207,   220,   162,   221,   641,   222,   556,   642,   223,   224,
     225,   643,   164,   226,   642,   227,   430,   163,   223,   224,
     225,   169,   764,   226,    15,   227,   172,   560,   591,   561,
     644,   440,   579,   359,   208,   209,   170,   204,   205,   206,
     207,   759,   220,   210,   221,   211,   222,   176,   332,   180,
     755,   178,    44,    45,    46,   777,   778,   779,   780,   181,
      17,   212,   213,   214,    47,    48,   185,   215,   662,   208,
     209,   359,   667,   752,    14,   668,   421,   186,   210,   277,
     211,  1026,  1039,   278,   279,  1027,  1028,   187,   280,   281,
    1029,  1030,   189,   216,   217,   218,   212,   213,   214,   190,
     774,  1051,   215,   359,   792,  1057,   194,   793,   195,   208,
     209,   119,   120,   121,   122,   219,   150,   151,   210,  1068,
     211,   196,   204,   205,   206,   207,   485,   486,   216,   217,
     218,   861,  1076,   199,   421,  1078,   212,   213,   214,   220,
     201,   221,   215,   222,    56,    57,    58,    59,    60,    61,
     219,   659,    62,   155,   156,   157,   223,   224,   225,   864,
     282,   226,   359,   227,   111,   112,   113,   114,   216,   217,
     218,   283,   867,   284,   220,   868,   221,   944,   222,   285,
     668,   876,   365,   950,   953,   433,   642,   421,   574,   575,
     219,   223,   224,   225,   327,   328,   226,   287,   227,  -480,
    -480,   368,   369,   210,   875,   211,   363,  -480,   364,    64,
      65,   290,    66,   291,   220,   295,   221,  1024,   222,   296,
    1025,   212,   213,   214,    67,    68,  1043,   215,   298,  1044,
     307,   223,   224,   225,   762,   308,   226,   769,   227,   377,
     378,   379,   365,   303,  -480,   373,   374,   375,   376,   377,
     378,   379,   312,   216,   217,   218,   350,   865,   313,   366,
     367,   368,   369,   365,   952,  1049,  1050,   371,   314,   593,
    -237,   594,   595,   596,   597,   219,   598,   599,  1058,  1059,
     366,   367,   368,   369,   370,   785,   786,   330,   371,   331,
     335,   336,   337,   338,   341,   346,   339,   347,   380,   220,
     391,   221,   433,   222,   372,   373,   374,   375,   376,   377,
     378,   379,   433,   392,   393,   760,   223,   224,   225,   398,
     399,   226,   400,   227,   407,   372,   373,   374,   375,   376,
     377,   378,   379,   417,   418,    14,   422,   427,   428,   432,
      69,    70,    71,    72,   441,    73,    74,   442,   443,   444,
      75,    76,    77,   449,   445,    78,    79,    80,   446,   365,
     447,   451,    81,    82,   448,   453,   458,    83,   483,   365,
     478,    84,   226,   480,   482,   484,   366,   367,   368,   369,
     487,   582,   488,   489,   371,   498,   366,   367,   368,   369,
     500,   536,   537,   541,   371,   542,   457,   544,   557,   679,
     680,   681,   682,   683,   564,   566,   684,   685,   361,   581,
     583,   584,   577,   686,   687,   688,   585,   586,   587,   590,
     592,   372,   373,   374,   375,   376,   377,   378,   379,   689,
     614,   372,   373,   374,   375,   376,   377,   378,   379,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   615,   522,   523,   524,
     525,   526,   527,   616,   617,   528,   630,   618,   529,   530,
     619,   620,   531,   532,   533,   534,   690,   691,   692,   693,
     694,   628,   636,   695,   696,   633,   637,   635,   638,   639,
     697,   698,   699,   701,   702,   703,   704,   705,   640,   645,
     706,   707,   656,   653,   657,   658,   700,   708,   709,   710,
     712,   713,   714,   715,   716,   661,   575,   717,   718,   574,
     664,   665,   666,   711,   719,   720,   721,   723,   724,   725,
     726,   727,   669,   672,   728,   729,   673,   674,   675,   676,
     722,   730,   731,   732,   734,   735,   736,   737,   738,   678,
     746,   739,   740,   745,   753,   754,   747,   733,   741,   742,
     743,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   761,   744,   757,   763,   765,   770,   771,
     772,   775,   776,   773,   787,   789,   803,   790,   814,   794,
     795,   796,   797,   863,   798,   799,   800,   801,   825,   836,
     847,   802,   804,   805,   806,   807,   808,   809,   810,   858,
     872,   811,   812,   813,   815,   816,   817,   818,   873,   877,
     819,   878,   820,   879,   821,   822,   823,   880,   824,   881,
     882,   883,   884,   885,   826,   827,   828,   829,   830,   831,
     832,   833,   834,   835,   886,   837,   888,   838,   839,   840,
     841,   842,   843,   844,   845,   846,   848,   849,   850,   851,
     852,   889,   853,   854,   855,   856,   857,   859,   866,   869,
     870,   887,   890,   891,   892,   893,   894,   895,   896,   897,
     898,   899,   900,   901,   902,   903,   904,   905,   906,   907,
     908,   909,   910,   911,   912,   913,   914,   915,   916,   917,
     918,   919,   920,   921,   922,   923,   924,   925,   926,   927,
     928,   929,   930,   932,   933,   934,   931,   935,   936,   937,
     938,   939,   940,   941,   943,   942,   946,   947,   948,   949,
     359,  1021,   951,   954,   955,   964,   975,   956,   957,   986,
     958,   959,   960,   961,   962,   963,   965,   966,   997,   967,
     968,   969,   970,   971,   972,   973,   974,   976,   977,  1008,
     978,   979,   980,   981,   982,   983,   984,   985,   987,   988,
     989,   990,   991,   992,   993,   994,   995,  1019,   996,   998,
    1037,  1038,   999,  1000,  1001,  1002,  1003,  1004,  1005,  1006,
    1007,  1009,  1010,  1011,  1012,  1040,  1013,  1014,  1015,  1016,
    1017,  1018,  1020,  1022,  1041,  1031,  1032,  1023,  1033,  1042,
    1034,  1035,  1036,  1048,  1047,  1052,  1045,  1053,  1060,  1054,
    1062,  1061,  1063,  1064,  1065,  1070,  1069,  1071,  1072,  1075,
     133,   171,  1073,   646,  1074,  1077,   750,   632,   748,    97,
     301,   613,   134,   784,   135,   136,   452,   137,   671,   138,
     140,   141,   437,   142,   143,   144,   788,   479,   292,   288,
     860,   481,   340,   756,   862,   767,   781,     0,     0,   768,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   395,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   568,     0,
       0,   426
};

static const yytype_int16 yycheck[] =
{
     199,    17,   177,   535,   226,   428,   127,   197,     4,    63,
       6,   537,   211,     3,     8,   208,   498,    13,     5,     6,
       5,     6,     4,     5,     6,     3,     0,     3,   227,   498,
     223,   224,   498,     7,    78,     9,    10,    11,    12,    13,
      14,    15,    16,   164,    57,   498,    64,    43,    77,    23,
      57,    25,    26,    27,    20,    51,    22,    68,    24,   105,
      57,   498,     3,   203,    88,   498,    78,    79,   208,    65,
      29,    80,     7,    34,     9,    10,    11,    12,    13,    14,
      15,    16,   154,   155,    75,     3,    45,     3,    23,    85,
      25,    26,    27,   154,   155,   159,   295,   296,     3,    78,
       5,     6,   628,    20,    88,    22,    80,    24,     3,   308,
       5,     6,   176,   177,   178,   111,   112,   113,    30,    80,
     102,   103,   104,   119,   120,   121,   108,   173,   157,    85,
     154,   155,   331,   332,   327,     3,   208,     5,     6,   314,
     131,    57,    33,   205,   205,    80,   208,   154,   155,    72,
      73,    64,   134,   135,   136,    70,    71,   154,   155,    64,
     382,    74,   361,   362,   208,   161,    81,    82,    83,    64,
     154,   155,   188,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   128,   206,   130,
     186,   204,   204,   675,   168,   204,    64,   208,   154,   155,
      89,    80,    80,   154,   155,   206,   675,   208,   407,   675,
     204,   227,   186,     3,   204,   202,   203,     3,   203,    42,
     202,   199,   675,   199,     3,     4,     5,     6,     3,    52,
     204,   154,   155,   168,     3,     4,     5,     6,   675,   205,
     439,   440,   675,   442,   169,   299,   445,   422,   127,   127,
      85,   186,    16,   203,   429,   203,   189,   208,   208,   192,
     208,   787,   167,   203,   457,   154,   155,     0,   208,   204,
     185,   205,   167,     7,   208,     9,    10,    11,    12,    13,
      14,   205,    16,   458,   208,   164,   164,    75,    67,    23,
       8,    25,    26,    27,   165,   166,    75,    76,    75,   167,
     171,   500,    74,    80,   207,    84,    75,    86,   120,   499,
     122,   307,   124,   125,   330,    84,   203,    86,    74,   154,
     155,   208,   206,   102,   103,   104,   154,   155,   132,   108,
     451,   863,    14,   102,   103,   104,   173,   174,   205,   108,
     128,   208,   130,   131,     3,   133,    80,   151,   152,   772,
      30,   128,    32,   130,   131,   134,   135,   136,   205,     3,
     559,   208,   561,   417,     3,   134,   135,   136,    29,    30,
      31,   564,     3,     4,     5,     6,   205,   156,     6,   208,
      41,    42,    85,     3,    87,   205,    89,   156,   208,   582,
       3,   195,   196,   197,   198,   199,   200,   201,   158,   159,
     160,   180,   577,   182,   165,   184,   176,     3,     4,     5,
       6,   180,    57,   182,   205,   184,   432,   208,   197,   198,
     199,   205,     6,   202,   208,   204,   205,   208,   197,   198,
     199,   205,   654,   202,   168,   204,    21,    85,   492,    87,
     205,    89,   458,   208,    75,    76,   205,     3,     4,     5,
       6,   650,   180,    84,   182,    86,   184,    67,    89,    75,
     635,    69,    29,    30,    31,     3,     4,     5,     6,     3,
     204,   102,   103,   104,    41,    42,     3,   108,   205,    75,
      76,   208,   205,   205,    80,   208,   208,    64,    84,    42,
      86,    42,  1024,    46,    47,    46,    47,   204,    51,    52,
      51,    52,    76,   134,   135,   136,   102,   103,   104,     3,
     205,  1043,   108,   208,   205,  1047,     3,   208,     3,    75,
      76,    45,    46,    47,    48,   156,   173,   174,    84,  1061,
      86,     3,     3,     4,     5,     6,    48,    49,   134,   135,
     136,   205,  1074,    68,   208,  1077,   102,   103,   104,   180,
       4,   182,   108,   184,    35,    36,    37,    38,    39,    40,
     156,   577,    43,   188,   189,   190,   197,   198,   199,   205,
       3,   202,   208,   204,    39,    40,    41,    42,   134,   135,
     136,     3,   205,     6,   180,   208,   182,   205,   184,     4,
     208,   790,   132,   205,   205,    75,   208,   208,     5,     6,
     156,   197,   198,   199,    75,    76,   202,   204,   204,   149,
     150,   151,   152,    84,   789,    86,    75,   157,    77,    29,
      30,     6,    32,     6,   180,    55,   182,   205,   184,    70,
     208,   102,   103,   104,    44,    45,   205,   108,    76,   208,
      64,   197,   198,   199,   652,   194,   202,   655,   204,   199,
     200,   201,   132,     3,   194,   195,   196,   197,   198,   199,
     200,   201,    80,   134,   135,   136,     5,     6,    80,   149,
     150,   151,   152,   132,   873,    48,    49,   157,   204,    58,
      59,    60,    61,    62,    63,   156,    65,    66,    46,    47,
     149,   150,   151,   152,   153,   672,   673,   204,   157,   204,
       4,     4,     4,     4,   204,   204,     6,   204,   206,   180,
       3,   182,    75,   184,   194,   195,   196,   197,   198,   199,
     200,   201,    75,     6,    80,   205,   197,   198,   199,     6,
     205,   202,   205,   204,    71,   194,   195,   196,   197,   198,
     199,   200,   201,    57,   204,    80,   204,   204,   204,   204,
     160,   161,   162,   163,     4,   165,   166,   204,   204,   204,
     170,   171,   172,   205,   204,   175,   176,   177,     4,   132,
       4,    79,   182,   183,   209,     3,   204,   187,     5,   132,
       6,   191,   202,     6,     6,    46,   149,   150,   151,   152,
     204,   154,   205,     6,   157,   179,   149,   150,   151,   152,
     208,   129,   204,     3,   157,     3,   157,   208,   205,    91,
      92,    93,    94,    95,    78,     4,    98,    99,   154,   131,
     209,   209,   204,   105,   106,   107,     6,     6,     3,     6,
     208,   194,   195,   196,   197,   198,   199,   200,   201,   121,
     204,   194,   195,   196,   197,   198,   199,   200,   201,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   204,   108,   109,   110,
     111,   112,   113,   204,   204,   116,    28,   204,   119,   120,
     204,   204,   123,   124,   125,   126,    91,    92,    93,    94,
      95,   204,     6,    98,    99,   205,     6,   204,     4,     3,
     105,   106,   107,    91,    92,    93,    94,    95,     6,   205,
      98,    99,     4,   208,     4,    80,   121,   105,   106,   107,
      91,    92,    93,    94,    95,   205,     6,    98,    99,     5,
      53,    50,   194,   121,   105,   106,   107,    91,    92,    93,
      94,    95,     6,    59,    98,    99,    59,     3,   208,    54,
     121,   105,   106,   107,    91,    92,    93,    94,    95,     6,
     129,    98,    99,   131,   204,   164,   133,   121,   105,   106,
     107,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   202,   121,   205,     4,   202,   205,   205,
     204,     6,     6,   205,   204,    55,    93,    56,    93,   208,
     208,   208,   208,     3,   208,   208,   208,   208,    93,    93,
      93,   208,   208,   208,   208,   208,   208,   208,   208,    93,
       6,   208,   208,   208,   208,   208,   208,   208,    64,     6,
     208,     6,   208,     6,   208,   208,   208,     6,   208,     6,
       6,     6,     6,     6,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,     6,   208,     6,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,     6,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,     6,     6,     6,     6,     6,     6,     6,     6,
     208,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   208,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   208,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,   208,     6,     6,     6,
       6,     6,     6,     6,     6,   208,     4,     4,     4,     4,
     208,   208,     6,   205,   205,     6,     6,   205,   205,     6,
     205,   205,   205,   205,   205,   205,   205,   205,     6,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,     6,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,     6,   205,   205,
       4,     4,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,     6,   205,   205,   205,   205,
     205,   205,   205,   205,     6,   205,   205,   208,   205,     6,
     205,   205,   205,     6,   205,     6,   208,     4,     6,   205,
      42,   205,    42,     6,    51,    42,   204,    42,     3,     3,
      63,   100,   204,   558,   205,   205,   627,   539,   625,    17,
     184,   500,    63,   670,    63,    63,   359,    63,   592,    63,
      63,    63,   333,    63,    63,    63,   675,   384,   173,   163,
     747,   386,   218,   642,   753,   655,   668,    -1,    -1,   655,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   287,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   444,    -1,
      -1,   309
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
     205,   205,   204,   205,   205,     6,     6,     3,     4,     5,
       6,   322,    30,    32,   258,   256,   256,   204,   282,    55,
      56,   251,   205,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   208,    93,   208,   208,   208,   208,   208,   208,
     208,   208,   208,   208,    93,   208,   208,   208,   208,   208,
     208,   208,   208,   208,   208,    93,   208,   208,   208,   208,
     208,   208,   208,   208,   208,   208,    93,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,    93,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,    93,   208,
     293,   205,   321,     3,   205,     6,   208,   205,   208,   208,
     208,   312,     6,    64,   224,   270,   273,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   208,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   208,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   208,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     208,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,   208,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   208,     6,   205,   319,     4,     4,     4,     4,
     205,     6,   273,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,     6,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,     6,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,     6,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,     6,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,     6,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,     6,
     205,   208,   205,   208,   205,   208,    42,    46,    47,    51,
      52,   205,   205,   205,   205,   205,   205,     4,     4,   319,
       6,     6,     6,   205,   208,   208,   275,   205,     6,    48,
      49,   319,     6,     4,   205,   127,   164,   319,    46,    47,
       6,   205,    42,    42,     6,    51,   127,   164,   319,   204,
      42,    42,     3,   204,   205,     3,   319,   205,   319
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
     322,   323,   323
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
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2332 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2340 "parser.cpp"
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
#line 2354 "parser.cpp"
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
#line 2368 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2379 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2387 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2396 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2405 "parser.cpp"
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
#line 2419 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 266 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2430 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2450 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2460 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2480 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 348 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2490 "parser.cpp"
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
#line 2504 "parser.cpp"
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
#line 2518 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 336 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2528 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2536 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2544 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2553 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2561 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2569 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2577 "parser.cpp"
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
#line 2591 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2600 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2609 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2618 "parser.cpp"
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
#line 2631 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 326 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2640 "parser.cpp"
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
#line 2654 "parser.cpp"
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
#line 2668 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 342 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2678 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2687 "parser.cpp"
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
#line 2701 "parser.cpp"
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
#line 2718 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2726 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2734 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2742 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2750 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2758 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2766 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2774 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2782 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2790 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2798 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2806 "parser.cpp"
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
#line 2820 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2828 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2836 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2844 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2852 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2860 "parser.cpp"
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
#line 2873 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2881 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2889 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 318 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2897 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2905 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2913 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2921 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2929 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2937 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2945 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2953 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2961 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2969 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2977 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2985 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 363 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2993 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 367 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 3001 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3009 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3017 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3025 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3033 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3041 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 322 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 3049 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 309 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 3057 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 206 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3068 "parser.cpp"
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
#line 3082 "parser.cpp"
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
#line 3096 "parser.cpp"
        break;

    case YYSYMBOL_index_info: /* index_info  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info\n");
    if ((((*yyvaluep).index_info_t)) != nullptr) {
        delete (((*yyvaluep).index_info_t));
    }
}
#line 3107 "parser.cpp"
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

#line 3215 "parser.cpp"

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
#line 3430 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 498 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3441 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 504 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3452 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 511 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3458 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 512 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3464 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 513 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3470 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3476 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 515 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3482 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 516 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3488 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3494 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3500 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 519 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3506 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 520 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3512 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 521 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3518 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 522 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3524 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 523 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3530 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 524 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3536 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 526 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3542 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 527 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3548 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 528 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3554 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3560 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 530 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3566 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 531 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3572 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3578 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3584 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 534 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3590 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 535 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3596 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 536 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3602 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 537 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3608 "parser.cpp"
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
#line 3628 "parser.cpp"
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
#line 3646 "parser.cpp"
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
#line 3679 "parser.cpp"
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
#line 3699 "parser.cpp"
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
#line 3720 "parser.cpp"
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
#line 3753 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 669 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3762 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 673 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3771 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 679 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3779 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 682 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3787 "parser.cpp"
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
#line 3843 "parser.cpp"
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
#line 3885 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 779 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3891 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3897 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 781 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3903 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 782 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3909 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 783 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3915 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 784 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3921 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 785 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3927 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 786 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3933 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 787 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3939 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 788 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3945 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 789 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3951 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 790 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3957 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 791 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3963 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 792 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3969 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 793 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3975 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 794 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3981 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3987 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 796 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3993 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 797 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3999 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 798 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4005 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 799 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4011 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 802 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4017 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 804 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4023 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 805 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 4029 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 806 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4035 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 807 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 4041 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4047 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4053 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4059 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4065 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4071 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4077 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4083 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4089 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4095 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4101 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4107 "parser.cpp"
    break;

  case 80: /* column_type: MULTIVECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4113 "parser.cpp"
    break;

  case 81: /* column_type: MULTIVECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4119 "parser.cpp"
    break;

  case 82: /* column_type: MULTIVECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4125 "parser.cpp"
    break;

  case 83: /* column_type: MULTIVECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4131 "parser.cpp"
    break;

  case 84: /* column_type: MULTIVECTOR '(' INT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4137 "parser.cpp"
    break;

  case 85: /* column_type: MULTIVECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4143 "parser.cpp"
    break;

  case 86: /* column_type: MULTIVECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4149 "parser.cpp"
    break;

  case 87: /* column_type: MULTIVECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4155 "parser.cpp"
    break;

  case 88: /* column_type: MULTIVECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4161 "parser.cpp"
    break;

  case 89: /* column_type: MULTIVECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4167 "parser.cpp"
    break;

  case 90: /* column_type: MULTIVECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kMultiVector, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4173 "parser.cpp"
    break;

  case 91: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4179 "parser.cpp"
    break;

  case 92: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4185 "parser.cpp"
    break;

  case 93: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4191 "parser.cpp"
    break;

  case 94: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4197 "parser.cpp"
    break;

  case 95: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4203 "parser.cpp"
    break;

  case 96: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4209 "parser.cpp"
    break;

  case 97: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4215 "parser.cpp"
    break;

  case 98: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4221 "parser.cpp"
    break;

  case 99: /* column_type: TENSOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4227 "parser.cpp"
    break;

  case 100: /* column_type: TENSOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4233 "parser.cpp"
    break;

  case 101: /* column_type: TENSOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4239 "parser.cpp"
    break;

  case 102: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4245 "parser.cpp"
    break;

  case 103: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4251 "parser.cpp"
    break;

  case 104: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4257 "parser.cpp"
    break;

  case 105: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4263 "parser.cpp"
    break;

  case 106: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4269 "parser.cpp"
    break;

  case 107: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4275 "parser.cpp"
    break;

  case 108: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4281 "parser.cpp"
    break;

  case 109: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4287 "parser.cpp"
    break;

  case 110: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4293 "parser.cpp"
    break;

  case 111: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4299 "parser.cpp"
    break;

  case 112: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4305 "parser.cpp"
    break;

  case 113: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4311 "parser.cpp"
    break;

  case 114: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4317 "parser.cpp"
    break;

  case 115: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4323 "parser.cpp"
    break;

  case 116: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 857 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4329 "parser.cpp"
    break;

  case 117: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 858 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4335 "parser.cpp"
    break;

  case 118: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 859 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4341 "parser.cpp"
    break;

  case 119: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 860 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4347 "parser.cpp"
    break;

  case 120: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 861 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4353 "parser.cpp"
    break;

  case 121: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 862 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4359 "parser.cpp"
    break;

  case 122: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 863 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4365 "parser.cpp"
    break;

  case 123: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 864 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4371 "parser.cpp"
    break;

  case 124: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 865 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBit}; }
#line 4377 "parser.cpp"
    break;

  case 125: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 866 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt8}; }
#line 4383 "parser.cpp"
    break;

  case 126: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 867 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt16}; }
#line 4389 "parser.cpp"
    break;

  case 127: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 868 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4395 "parser.cpp"
    break;

  case 128: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 869 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt32}; }
#line 4401 "parser.cpp"
    break;

  case 129: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 870 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemInt64}; }
#line 4407 "parser.cpp"
    break;

  case 130: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 871 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat}; }
#line 4413 "parser.cpp"
    break;

  case 131: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 872 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemDouble}; }
#line 4419 "parser.cpp"
    break;

  case 132: /* column_type: SPARSE '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 873 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemFloat16}; }
#line 4425 "parser.cpp"
    break;

  case 133: /* column_type: SPARSE '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 874 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemBFloat16}; }
#line 4431 "parser.cpp"
    break;

  case 134: /* column_type: SPARSE '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 875 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::EmbeddingDataType::kElemUInt8}; }
#line 4437 "parser.cpp"
    break;

  case 135: /* column_constraints: column_constraint  */
#line 894 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4446 "parser.cpp"
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
#line 4460 "parser.cpp"
    break;

  case 137: /* column_constraint: PRIMARY KEY  */
#line 908 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4468 "parser.cpp"
    break;

  case 138: /* column_constraint: UNIQUE  */
#line 911 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4476 "parser.cpp"
    break;

  case 139: /* column_constraint: NULLABLE  */
#line 914 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4484 "parser.cpp"
    break;

  case 140: /* column_constraint: NOT NULLABLE  */
#line 917 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4492 "parser.cpp"
    break;

  case 141: /* default_expr: DEFAULT constant_expr  */
#line 921 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4500 "parser.cpp"
    break;

  case 142: /* default_expr: %empty  */
#line 924 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4508 "parser.cpp"
    break;

  case 143: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 929 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4518 "parser.cpp"
    break;

  case 144: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 934 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4528 "parser.cpp"
    break;

  case 145: /* identifier_array: IDENTIFIER  */
#line 941 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4539 "parser.cpp"
    break;

  case 146: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 947 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4550 "parser.cpp"
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
#line 4567 "parser.cpp"
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
#line 4606 "parser.cpp"
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
#line 4623 "parser.cpp"
    break;

  case 150: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1020 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4631 "parser.cpp"
    break;

  case 151: /* optional_identifier_array: %empty  */
#line 1023 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4639 "parser.cpp"
    break;

  case 152: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1030 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4649 "parser.cpp"
    break;

  case 153: /* explain_type: ANALYZE  */
#line 1036 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4657 "parser.cpp"
    break;

  case 154: /* explain_type: AST  */
#line 1039 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4665 "parser.cpp"
    break;

  case 155: /* explain_type: RAW  */
#line 1042 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4673 "parser.cpp"
    break;

  case 156: /* explain_type: LOGICAL  */
#line 1045 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4681 "parser.cpp"
    break;

  case 157: /* explain_type: PHYSICAL  */
#line 1048 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4689 "parser.cpp"
    break;

  case 158: /* explain_type: PIPELINE  */
#line 1051 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4697 "parser.cpp"
    break;

  case 159: /* explain_type: FRAGMENT  */
#line 1054 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4705 "parser.cpp"
    break;

  case 160: /* explain_type: %empty  */
#line 1057 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4713 "parser.cpp"
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
#line 4730 "parser.cpp"
    break;

  case 162: /* update_expr_array: update_expr  */
#line 1077 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4739 "parser.cpp"
    break;

  case 163: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1081 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4748 "parser.cpp"
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
#line 4760 "parser.cpp"
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
#line 4776 "parser.cpp"
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
#line 4794 "parser.cpp"
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
#line 4812 "parser.cpp"
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
#line 4830 "parser.cpp"
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
#line 4853 "parser.cpp"
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
#line 4911 "parser.cpp"
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
#line 4971 "parser.cpp"
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
#line 5023 "parser.cpp"
    break;

  case 173: /* select_statement: select_without_paren  */
#line 1339 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5031 "parser.cpp"
    break;

  case 174: /* select_statement: select_with_paren  */
#line 1342 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5039 "parser.cpp"
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
#line 5053 "parser.cpp"
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
#line 5067 "parser.cpp"
    break;

  case 177: /* select_with_paren: '(' select_without_paren ')'  */
#line 1364 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5075 "parser.cpp"
    break;

  case 178: /* select_with_paren: '(' select_with_paren ')'  */
#line 1367 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5083 "parser.cpp"
    break;

  case 179: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1371 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 5092 "parser.cpp"
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
#line 5118 "parser.cpp"
    break;

  case 181: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1398 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5126 "parser.cpp"
    break;

  case 182: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1401 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 5134 "parser.cpp"
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
#line 5154 "parser.cpp"
    break;

  case 184: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1422 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5162 "parser.cpp"
    break;

  case 185: /* order_by_clause: %empty  */
#line 1425 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5170 "parser.cpp"
    break;

  case 186: /* order_by_expr_list: order_by_expr  */
#line 1429 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5179 "parser.cpp"
    break;

  case 187: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1433 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5188 "parser.cpp"
    break;

  case 188: /* order_by_expr: expr order_by_type  */
#line 1438 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5198 "parser.cpp"
    break;

  case 189: /* order_by_type: ASC  */
#line 1444 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5206 "parser.cpp"
    break;

  case 190: /* order_by_type: DESC  */
#line 1447 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5214 "parser.cpp"
    break;

  case 191: /* order_by_type: %empty  */
#line 1450 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5222 "parser.cpp"
    break;

  case 192: /* limit_expr: LIMIT expr  */
#line 1454 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5230 "parser.cpp"
    break;

  case 193: /* limit_expr: %empty  */
#line 1458 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5236 "parser.cpp"
    break;

  case 194: /* offset_expr: OFFSET expr  */
#line 1460 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5244 "parser.cpp"
    break;

  case 195: /* offset_expr: %empty  */
#line 1464 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5250 "parser.cpp"
    break;

  case 196: /* distinct: DISTINCT  */
#line 1466 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5258 "parser.cpp"
    break;

  case 197: /* distinct: %empty  */
#line 1469 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5266 "parser.cpp"
    break;

  case 198: /* from_clause: FROM table_reference  */
#line 1473 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5274 "parser.cpp"
    break;

  case 199: /* from_clause: %empty  */
#line 1476 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5282 "parser.cpp"
    break;

  case 200: /* search_clause: SEARCH sub_search_array  */
#line 1480 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5292 "parser.cpp"
    break;

  case 201: /* search_clause: %empty  */
#line 1485 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5300 "parser.cpp"
    break;

  case 202: /* where_clause: WHERE expr  */
#line 1489 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5308 "parser.cpp"
    break;

  case 203: /* where_clause: %empty  */
#line 1492 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5316 "parser.cpp"
    break;

  case 204: /* having_clause: HAVING expr  */
#line 1496 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5324 "parser.cpp"
    break;

  case 205: /* having_clause: %empty  */
#line 1499 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5332 "parser.cpp"
    break;

  case 206: /* group_by_clause: GROUP BY expr_array  */
#line 1503 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5340 "parser.cpp"
    break;

  case 207: /* group_by_clause: %empty  */
#line 1506 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5348 "parser.cpp"
    break;

  case 208: /* set_operator: UNION  */
#line 1510 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5356 "parser.cpp"
    break;

  case 209: /* set_operator: UNION ALL  */
#line 1513 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5364 "parser.cpp"
    break;

  case 210: /* set_operator: INTERSECT  */
#line 1516 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5372 "parser.cpp"
    break;

  case 211: /* set_operator: EXCEPT  */
#line 1519 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5380 "parser.cpp"
    break;

  case 212: /* table_reference: table_reference_unit  */
#line 1527 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5388 "parser.cpp"
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
#line 5406 "parser.cpp"
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
#line 5424 "parser.cpp"
    break;

  case 217: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1561 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5435 "parser.cpp"
    break;

  case 218: /* table_name: IDENTIFIER  */
#line 1570 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5445 "parser.cpp"
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
#line 5457 "parser.cpp"
    break;

  case 220: /* table_alias: AS IDENTIFIER  */
#line 1584 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5467 "parser.cpp"
    break;

  case 221: /* table_alias: IDENTIFIER  */
#line 1589 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5477 "parser.cpp"
    break;

  case 222: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1594 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5488 "parser.cpp"
    break;

  case 223: /* table_alias: %empty  */
#line 1600 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5496 "parser.cpp"
    break;

  case 224: /* with_clause: WITH with_expr_list  */
#line 1607 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5504 "parser.cpp"
    break;

  case 225: /* with_clause: %empty  */
#line 1610 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5512 "parser.cpp"
    break;

  case 226: /* with_expr_list: with_expr  */
#line 1614 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5521 "parser.cpp"
    break;

  case 227: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1617 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5530 "parser.cpp"
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
#line 5542 "parser.cpp"
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
#line 5554 "parser.cpp"
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
#line 5567 "parser.cpp"
    break;

  case 231: /* join_type: INNER  */
#line 1655 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5575 "parser.cpp"
    break;

  case 232: /* join_type: LEFT  */
#line 1658 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5583 "parser.cpp"
    break;

  case 233: /* join_type: RIGHT  */
#line 1661 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5591 "parser.cpp"
    break;

  case 234: /* join_type: OUTER  */
#line 1664 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5599 "parser.cpp"
    break;

  case 235: /* join_type: FULL  */
#line 1667 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5607 "parser.cpp"
    break;

  case 236: /* join_type: CROSS  */
#line 1670 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5615 "parser.cpp"
    break;

  case 237: /* join_type: %empty  */
#line 1673 "parser.y"
                {
}
#line 5622 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW DATABASES  */
#line 1679 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5631 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLES  */
#line 1683 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5640 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW VIEWS  */
#line 1687 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5649 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW CONFIGS  */
#line 1691 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5658 "parser.cpp"
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
#line 5670 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW PROFILES  */
#line 1702 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5679 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW BUFFER  */
#line 1706 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5688 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW MEMINDEX  */
#line 1710 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5697 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW QUERIES  */
#line 1714 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5706 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1718 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5716 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TRANSACTIONS  */
#line 1723 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5725 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1727 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5735 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW SESSION VARIABLES  */
#line 1732 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5744 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1736 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5753 "parser.cpp"
    break;

  case 252: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1740 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5764 "parser.cpp"
    break;

  case 253: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1746 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5775 "parser.cpp"
    break;

  case 254: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1752 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5786 "parser.cpp"
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
#line 5802 "parser.cpp"
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
#line 5818 "parser.cpp"
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
#line 5834 "parser.cpp"
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
#line 5851 "parser.cpp"
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
#line 5868 "parser.cpp"
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
#line 5886 "parser.cpp"
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
#line 5905 "parser.cpp"
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
#line 5921 "parser.cpp"
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
#line 5940 "parser.cpp"
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
#line 5961 "parser.cpp"
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
#line 5983 "parser.cpp"
    break;

  case 266: /* show_statement: SHOW LOGS  */
#line 1900 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5992 "parser.cpp"
    break;

  case 267: /* show_statement: SHOW DELTA LOGS  */
#line 1904 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 6001 "parser.cpp"
    break;

  case 268: /* show_statement: SHOW CATALOGS  */
#line 1908 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 6010 "parser.cpp"
    break;

  case 269: /* show_statement: SHOW PERSISTENCE FILES  */
#line 1912 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceFiles;
}
#line 6019 "parser.cpp"
    break;

  case 270: /* show_statement: SHOW PERSISTENCE OBJECTS  */
#line 1916 "parser.y"
                           {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObjects;
}
#line 6028 "parser.cpp"
    break;

  case 271: /* show_statement: SHOW PERSISTENCE OBJECT STRING  */
#line 1920 "parser.y"
                                 {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kPersistenceObject;
      (yyval.show_stmt)->file_name_ = (yyvsp[0].str_value);
      free((yyvsp[0].str_value));
}
#line 6039 "parser.cpp"
    break;

  case 272: /* show_statement: SHOW MEMORY  */
#line 1926 "parser.y"
              {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemory;
}
#line 6048 "parser.cpp"
    break;

  case 273: /* show_statement: SHOW MEMORY OBJECTS  */
#line 1930 "parser.y"
                      {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryObjects;
}
#line 6057 "parser.cpp"
    break;

  case 274: /* show_statement: SHOW MEMORY ALLOCATION  */
#line 1934 "parser.y"
                         {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemoryAllocation;
}
#line 6066 "parser.cpp"
    break;

  case 275: /* flush_statement: FLUSH DATA  */
#line 1942 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 6075 "parser.cpp"
    break;

  case 276: /* flush_statement: FLUSH LOG  */
#line 1946 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 6084 "parser.cpp"
    break;

  case 277: /* flush_statement: FLUSH BUFFER  */
#line 1950 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 6093 "parser.cpp"
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
#line 6108 "parser.cpp"
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
#line 6132 "parser.cpp"
    break;

  case 280: /* command_statement: USE IDENTIFIER  */
#line 1992 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 6143 "parser.cpp"
    break;

  case 281: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1998 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 6153 "parser.cpp"
    break;

  case 282: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 2003 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6164 "parser.cpp"
    break;

  case 283: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 2009 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6175 "parser.cpp"
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
#line 6188 "parser.cpp"
    break;

  case 285: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 2023 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 286: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 2029 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6210 "parser.cpp"
    break;

  case 287: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 2035 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6221 "parser.cpp"
    break;

  case 288: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 2041 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6232 "parser.cpp"
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
#line 6245 "parser.cpp"
    break;

  case 290: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2055 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6256 "parser.cpp"
    break;

  case 291: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2061 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6267 "parser.cpp"
    break;

  case 292: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2067 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6278 "parser.cpp"
    break;

  case 293: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2073 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6289 "parser.cpp"
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
#line 6302 "parser.cpp"
    break;

  case 295: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2087 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6313 "parser.cpp"
    break;

  case 296: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2093 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6324 "parser.cpp"
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
#line 6341 "parser.cpp"
    break;

  case 298: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2113 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6350 "parser.cpp"
    break;

  case 299: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2117 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6360 "parser.cpp"
    break;

  case 300: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2122 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6371 "parser.cpp"
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
#line 6383 "parser.cpp"
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
#line 6396 "parser.cpp"
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
#line 6410 "parser.cpp"
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
#line 6425 "parser.cpp"
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
#line 6440 "parser.cpp"
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
#line 6456 "parser.cpp"
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
#line 6472 "parser.cpp"
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
#line 6489 "parser.cpp"
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
#line 6506 "parser.cpp"
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
#line 6521 "parser.cpp"
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
#line 6537 "parser.cpp"
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
#line 6554 "parser.cpp"
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
#line 6572 "parser.cpp"
    break;

  case 314: /* admin_statement: ADMIN SHOW LOGS  */
#line 2264 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6581 "parser.cpp"
    break;

  case 315: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2268 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6591 "parser.cpp"
    break;

  case 316: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2273 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6601 "parser.cpp"
    break;

  case 317: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2278 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6612 "parser.cpp"
    break;

  case 318: /* expr_array: expr_alias  */
#line 2289 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6621 "parser.cpp"
    break;

  case 319: /* expr_array: expr_array ',' expr_alias  */
#line 2293 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6630 "parser.cpp"
    break;

  case 320: /* expr_array_list: '(' expr_array ')'  */
#line 2298 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6639 "parser.cpp"
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
#line 6659 "parser.cpp"
    break;

  case 322: /* expr_alias: expr AS IDENTIFIER  */
#line 2329 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6670 "parser.cpp"
    break;

  case 323: /* expr_alias: expr  */
#line 2335 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6678 "parser.cpp"
    break;

  case 329: /* operand: '(' expr ')'  */
#line 2345 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6686 "parser.cpp"
    break;

  case 330: /* operand: '(' select_without_paren ')'  */
#line 2348 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6697 "parser.cpp"
    break;

  case 331: /* operand: constant_expr  */
#line 2354 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6705 "parser.cpp"
    break;

  case 342: /* extra_match_tensor_option: ',' STRING  */
#line 2368 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6713 "parser.cpp"
    break;

  case 343: /* extra_match_tensor_option: %empty  */
#line 2371 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6721 "parser.cpp"
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
#line 6742 "parser.cpp"
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
#line 6791 "parser.cpp"
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
#line 6833 "parser.cpp"
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
#line 6879 "parser.cpp"
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
#line 6926 "parser.cpp"
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
#line 6951 "parser.cpp"
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
#line 6975 "parser.cpp"
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
#line 6997 "parser.cpp"
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
#line 7019 "parser.cpp"
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
#line 7032 "parser.cpp"
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
#line 7047 "parser.cpp"
    break;

  case 355: /* query_expr: QUERY '(' STRING ')'  */
#line 2663 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 7058 "parser.cpp"
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
#line 7071 "parser.cpp"
    break;

  case 357: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2678 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 7082 "parser.cpp"
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
#line 7098 "parser.cpp"
    break;

  case 359: /* sub_search: match_vector_expr  */
#line 2696 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7106 "parser.cpp"
    break;

  case 360: /* sub_search: match_text_expr  */
#line 2699 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7114 "parser.cpp"
    break;

  case 361: /* sub_search: match_tensor_expr  */
#line 2702 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7122 "parser.cpp"
    break;

  case 362: /* sub_search: match_sparse_expr  */
#line 2705 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7130 "parser.cpp"
    break;

  case 363: /* sub_search: query_expr  */
#line 2708 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7138 "parser.cpp"
    break;

  case 364: /* sub_search: fusion_expr  */
#line 2711 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 7146 "parser.cpp"
    break;

  case 365: /* sub_search_array: sub_search  */
#line 2715 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 7155 "parser.cpp"
    break;

  case 366: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2719 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 7164 "parser.cpp"
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
#line 7177 "parser.cpp"
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
#line 7190 "parser.cpp"
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
#line 7204 "parser.cpp"
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
#line 7216 "parser.cpp"
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
#line 7228 "parser.cpp"
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
#line 7240 "parser.cpp"
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
#line 7252 "parser.cpp"
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
#line 7264 "parser.cpp"
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
#line 7277 "parser.cpp"
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
#line 7290 "parser.cpp"
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
#line 7303 "parser.cpp"
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
#line 7316 "parser.cpp"
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
#line 7329 "parser.cpp"
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
#line 7342 "parser.cpp"
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
#line 7355 "parser.cpp"
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
#line 7368 "parser.cpp"
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
#line 7381 "parser.cpp"
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
#line 7394 "parser.cpp"
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
#line 7407 "parser.cpp"
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
#line 7420 "parser.cpp"
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
#line 7455 "parser.cpp"
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
#line 7468 "parser.cpp"
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
#line 7481 "parser.cpp"
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
#line 7494 "parser.cpp"
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
#line 7507 "parser.cpp"
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
#line 7519 "parser.cpp"
    break;

  case 393: /* in_expr: operand IN '(' expr_array ')'  */
#line 2952 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7530 "parser.cpp"
    break;

  case 394: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2958 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7541 "parser.cpp"
    break;

  case 395: /* case_expr: CASE expr case_check_array END  */
#line 2965 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7552 "parser.cpp"
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
#line 7564 "parser.cpp"
    break;

  case 397: /* case_expr: CASE case_check_array END  */
#line 2978 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7574 "parser.cpp"
    break;

  case 398: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2983 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7585 "parser.cpp"
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
#line 7597 "parser.cpp"
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
#line 7609 "parser.cpp"
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
#line 7640 "parser.cpp"
    break;

  case 402: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 3032 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7651 "parser.cpp"
    break;

  case 403: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 3038 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7662 "parser.cpp"
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
#line 7674 "parser.cpp"
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
#line 7686 "parser.cpp"
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
#line 7698 "parser.cpp"
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
#line 7710 "parser.cpp"
    break;

  case 408: /* column_expr: '*'  */
#line 3073 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7720 "parser.cpp"
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
#line 7734 "parser.cpp"
    break;

  case 410: /* constant_expr: STRING  */
#line 3088 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7744 "parser.cpp"
    break;

  case 411: /* constant_expr: TRUE  */
#line 3093 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7754 "parser.cpp"
    break;

  case 412: /* constant_expr: FALSE  */
#line 3098 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7764 "parser.cpp"
    break;

  case 413: /* constant_expr: DOUBLE_VALUE  */
#line 3103 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7774 "parser.cpp"
    break;

  case 414: /* constant_expr: LONG_VALUE  */
#line 3108 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7784 "parser.cpp"
    break;

  case 415: /* constant_expr: DATE STRING  */
#line 3113 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7794 "parser.cpp"
    break;

  case 416: /* constant_expr: TIME STRING  */
#line 3118 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7804 "parser.cpp"
    break;

  case 417: /* constant_expr: DATETIME STRING  */
#line 3123 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7814 "parser.cpp"
    break;

  case 418: /* constant_expr: TIMESTAMP STRING  */
#line 3128 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7824 "parser.cpp"
    break;

  case 419: /* constant_expr: INTERVAL interval_expr  */
#line 3133 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7832 "parser.cpp"
    break;

  case 420: /* constant_expr: interval_expr  */
#line 3136 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7840 "parser.cpp"
    break;

  case 421: /* constant_expr: common_array_expr  */
#line 3139 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7848 "parser.cpp"
    break;

  case 422: /* common_array_expr: array_expr  */
#line 3143 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7856 "parser.cpp"
    break;

  case 423: /* common_array_expr: subarray_array_expr  */
#line 3146 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7864 "parser.cpp"
    break;

  case 424: /* common_array_expr: sparse_array_expr  */
#line 3149 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7872 "parser.cpp"
    break;

  case 425: /* common_array_expr: empty_array_expr  */
#line 3152 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7880 "parser.cpp"
    break;

  case 426: /* common_sparse_array_expr: sparse_array_expr  */
#line 3156 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7888 "parser.cpp"
    break;

  case 427: /* common_sparse_array_expr: array_expr  */
#line 3159 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7896 "parser.cpp"
    break;

  case 428: /* common_sparse_array_expr: empty_array_expr  */
#line 3162 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7904 "parser.cpp"
    break;

  case 429: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 3166 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7912 "parser.cpp"
    break;

  case 430: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 3170 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7922 "parser.cpp"
    break;

  case 431: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3175 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7931 "parser.cpp"
    break;

  case 432: /* sparse_array_expr: long_sparse_array_expr  */
#line 3180 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7939 "parser.cpp"
    break;

  case 433: /* sparse_array_expr: double_sparse_array_expr  */
#line 3183 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7947 "parser.cpp"
    break;

  case 434: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3187 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7955 "parser.cpp"
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
#line 7967 "parser.cpp"
    break;

  case 436: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3198 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7978 "parser.cpp"
    break;

  case 437: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3205 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7986 "parser.cpp"
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
#line 7998 "parser.cpp"
    break;

  case 439: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3216 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8009 "parser.cpp"
    break;

  case 440: /* empty_array_expr: '[' ']'  */
#line 3223 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 8017 "parser.cpp"
    break;

  case 441: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3227 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 8025 "parser.cpp"
    break;

  case 442: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3231 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 8033 "parser.cpp"
    break;

  case 443: /* array_expr: long_array_expr  */
#line 3235 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8041 "parser.cpp"
    break;

  case 444: /* array_expr: double_array_expr  */
#line 3238 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 8049 "parser.cpp"
    break;

  case 445: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3242 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8057 "parser.cpp"
    break;

  case 446: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3246 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8067 "parser.cpp"
    break;

  case 447: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3251 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8076 "parser.cpp"
    break;

  case 448: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3256 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 8084 "parser.cpp"
    break;

  case 449: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3260 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 8094 "parser.cpp"
    break;

  case 450: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3265 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 8103 "parser.cpp"
    break;

  case 451: /* interval_expr: LONG_VALUE SECONDS  */
#line 3270 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8114 "parser.cpp"
    break;

  case 452: /* interval_expr: LONG_VALUE SECOND  */
#line 3276 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8125 "parser.cpp"
    break;

  case 453: /* interval_expr: LONG_VALUE MINUTES  */
#line 3282 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8136 "parser.cpp"
    break;

  case 454: /* interval_expr: LONG_VALUE MINUTE  */
#line 3288 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8147 "parser.cpp"
    break;

  case 455: /* interval_expr: LONG_VALUE HOURS  */
#line 3294 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8158 "parser.cpp"
    break;

  case 456: /* interval_expr: LONG_VALUE HOUR  */
#line 3300 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8169 "parser.cpp"
    break;

  case 457: /* interval_expr: LONG_VALUE DAYS  */
#line 3306 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8180 "parser.cpp"
    break;

  case 458: /* interval_expr: LONG_VALUE DAY  */
#line 3312 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8191 "parser.cpp"
    break;

  case 459: /* interval_expr: LONG_VALUE MONTHS  */
#line 3318 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8202 "parser.cpp"
    break;

  case 460: /* interval_expr: LONG_VALUE MONTH  */
#line 3324 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8213 "parser.cpp"
    break;

  case 461: /* interval_expr: LONG_VALUE YEARS  */
#line 3330 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8224 "parser.cpp"
    break;

  case 462: /* interval_expr: LONG_VALUE YEAR  */
#line 3336 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 8235 "parser.cpp"
    break;

  case 463: /* copy_option_list: copy_option  */
#line 3347 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 8244 "parser.cpp"
    break;

  case 464: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3351 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 8253 "parser.cpp"
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
        free((yyvsp[0].str_value));
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 8289 "parser.cpp"
    break;

  case 466: /* copy_option: DELIMITER STRING  */
#line 3387 "parser.y"
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
#line 8304 "parser.cpp"
    break;

  case 467: /* copy_option: HEADER  */
#line 3397 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 8314 "parser.cpp"
    break;

  case 468: /* copy_option: OFFSET LONG_VALUE  */
#line 3402 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 8324 "parser.cpp"
    break;

  case 469: /* copy_option: LIMIT LONG_VALUE  */
#line 3407 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 8334 "parser.cpp"
    break;

  case 470: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3412 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 8344 "parser.cpp"
    break;

  case 471: /* file_path: STRING  */
#line 3418 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 8352 "parser.cpp"
    break;

  case 472: /* if_exists: IF EXISTS  */
#line 3422 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8358 "parser.cpp"
    break;

  case 473: /* if_exists: %empty  */
#line 3423 "parser.y"
  { (yyval.bool_value) = false; }
#line 8364 "parser.cpp"
    break;

  case 474: /* if_not_exists: IF NOT EXISTS  */
#line 3425 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8370 "parser.cpp"
    break;

  case 475: /* if_not_exists: %empty  */
#line 3426 "parser.y"
  { (yyval.bool_value) = false; }
#line 8376 "parser.cpp"
    break;

  case 478: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3441 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8389 "parser.cpp"
    break;

  case 479: /* if_not_exists_info: %empty  */
#line 3449 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8397 "parser.cpp"
    break;

  case 480: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3453 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8405 "parser.cpp"
    break;

  case 481: /* with_index_param_list: %empty  */
#line 3456 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8413 "parser.cpp"
    break;

  case 482: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3460 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8421 "parser.cpp"
    break;

  case 483: /* optional_table_properties_list: %empty  */
#line 3463 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8429 "parser.cpp"
    break;

  case 484: /* index_param_list: index_param  */
#line 3467 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8438 "parser.cpp"
    break;

  case 485: /* index_param_list: index_param_list ',' index_param  */
#line 3471 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8447 "parser.cpp"
    break;

  case 486: /* index_param: IDENTIFIER  */
#line 3476 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8458 "parser.cpp"
    break;

  case 487: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3482 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8473 "parser.cpp"
    break;

  case 488: /* index_param: IDENTIFIER '=' STRING  */
#line 3492 "parser.y"
                        {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8488 "parser.cpp"
    break;

  case 489: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3502 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8501 "parser.cpp"
    break;

  case 490: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3510 "parser.y"
                              {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8514 "parser.cpp"
    break;

  case 491: /* index_info: '(' IDENTIFIER ')' USING IDENTIFIER with_index_param_list  */
#line 3521 "parser.y"
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
#line 8550 "parser.cpp"
    break;

  case 492: /* index_info: '(' IDENTIFIER ')'  */
#line 3552 "parser.y"
                     {
    (yyval.index_info_t) = new infinity::IndexInfo();
    (yyval.index_info_t)->index_type_ = infinity::IndexType::kSecondary;
    (yyval.index_info_t)->column_name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
}
#line 8561 "parser.cpp"
    break;


#line 8565 "parser.cpp"

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

#line 3559 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

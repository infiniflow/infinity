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
  YYSYMBOL_TENSOR = 123,                   /* TENSOR  */
  YYSYMBOL_SPARSE = 124,                   /* SPARSE  */
  YYSYMBOL_TENSORARRAY = 125,              /* TENSORARRAY  */
  YYSYMBOL_PRIMARY = 126,                  /* PRIMARY  */
  YYSYMBOL_KEY = 127,                      /* KEY  */
  YYSYMBOL_UNIQUE = 128,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 129,                 /* NULLABLE  */
  YYSYMBOL_IS = 130,                       /* IS  */
  YYSYMBOL_DEFAULT = 131,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 132,                     /* TRUE  */
  YYSYMBOL_FALSE = 133,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 134,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 135,                   /* SECOND  */
  YYSYMBOL_SECONDS = 136,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 137,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 138,                  /* MINUTES  */
  YYSYMBOL_HOUR = 139,                     /* HOUR  */
  YYSYMBOL_HOURS = 140,                    /* HOURS  */
  YYSYMBOL_DAY = 141,                      /* DAY  */
  YYSYMBOL_DAYS = 142,                     /* DAYS  */
  YYSYMBOL_MONTH = 143,                    /* MONTH  */
  YYSYMBOL_MONTHS = 144,                   /* MONTHS  */
  YYSYMBOL_YEAR = 145,                     /* YEAR  */
  YYSYMBOL_YEARS = 146,                    /* YEARS  */
  YYSYMBOL_EQUAL = 147,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 148,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 149,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 150,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 151,                  /* BETWEEN  */
  YYSYMBOL_AND = 152,                      /* AND  */
  YYSYMBOL_OR = 153,                       /* OR  */
  YYSYMBOL_EXTRACT = 154,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 155,                     /* LIKE  */
  YYSYMBOL_DATA = 156,                     /* DATA  */
  YYSYMBOL_LOG = 157,                      /* LOG  */
  YYSYMBOL_BUFFER = 158,                   /* BUFFER  */
  YYSYMBOL_TRANSACTIONS = 159,             /* TRANSACTIONS  */
  YYSYMBOL_TRANSACTION = 160,              /* TRANSACTION  */
  YYSYMBOL_MEMINDEX = 161,                 /* MEMINDEX  */
  YYSYMBOL_USING = 162,                    /* USING  */
  YYSYMBOL_SESSION = 163,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 164,                   /* GLOBAL  */
  YYSYMBOL_OFF = 165,                      /* OFF  */
  YYSYMBOL_EXPORT = 166,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 167,                  /* PROFILE  */
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
  YYSYMBOL_NUMBER = 185,                   /* NUMBER  */
  YYSYMBOL_186_ = 186,                     /* '='  */
  YYSYMBOL_187_ = 187,                     /* '<'  */
  YYSYMBOL_188_ = 188,                     /* '>'  */
  YYSYMBOL_189_ = 189,                     /* '+'  */
  YYSYMBOL_190_ = 190,                     /* '-'  */
  YYSYMBOL_191_ = 191,                     /* '*'  */
  YYSYMBOL_192_ = 192,                     /* '/'  */
  YYSYMBOL_193_ = 193,                     /* '%'  */
  YYSYMBOL_194_ = 194,                     /* '['  */
  YYSYMBOL_195_ = 195,                     /* ']'  */
  YYSYMBOL_196_ = 196,                     /* '('  */
  YYSYMBOL_197_ = 197,                     /* ')'  */
  YYSYMBOL_198_ = 198,                     /* '.'  */
  YYSYMBOL_199_ = 199,                     /* ';'  */
  YYSYMBOL_200_ = 200,                     /* ','  */
  YYSYMBOL_201_ = 201,                     /* ':'  */
  YYSYMBOL_YYACCEPT = 202,                 /* $accept  */
  YYSYMBOL_input_pattern = 203,            /* input_pattern  */
  YYSYMBOL_statement_list = 204,           /* statement_list  */
  YYSYMBOL_statement = 205,                /* statement  */
  YYSYMBOL_explainable_statement = 206,    /* explainable_statement  */
  YYSYMBOL_create_statement = 207,         /* create_statement  */
  YYSYMBOL_table_element_array = 208,      /* table_element_array  */
  YYSYMBOL_table_element = 209,            /* table_element  */
  YYSYMBOL_table_column = 210,             /* table_column  */
  YYSYMBOL_column_type = 211,              /* column_type  */
  YYSYMBOL_column_constraints = 212,       /* column_constraints  */
  YYSYMBOL_column_constraint = 213,        /* column_constraint  */
  YYSYMBOL_default_expr = 214,             /* default_expr  */
  YYSYMBOL_table_constraint = 215,         /* table_constraint  */
  YYSYMBOL_identifier_array = 216,         /* identifier_array  */
  YYSYMBOL_delete_statement = 217,         /* delete_statement  */
  YYSYMBOL_insert_statement = 218,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 219, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 220,        /* explain_statement  */
  YYSYMBOL_explain_type = 221,             /* explain_type  */
  YYSYMBOL_update_statement = 222,         /* update_statement  */
  YYSYMBOL_update_expr_array = 223,        /* update_expr_array  */
  YYSYMBOL_update_expr = 224,              /* update_expr  */
  YYSYMBOL_drop_statement = 225,           /* drop_statement  */
  YYSYMBOL_copy_statement = 226,           /* copy_statement  */
  YYSYMBOL_select_statement = 227,         /* select_statement  */
  YYSYMBOL_select_with_paren = 228,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 229,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 230, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 231, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 232, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 233,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 234,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 235,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 236,            /* order_by_type  */
  YYSYMBOL_limit_expr = 237,               /* limit_expr  */
  YYSYMBOL_offset_expr = 238,              /* offset_expr  */
  YYSYMBOL_distinct = 239,                 /* distinct  */
  YYSYMBOL_from_clause = 240,              /* from_clause  */
  YYSYMBOL_search_clause = 241,            /* search_clause  */
  YYSYMBOL_where_clause = 242,             /* where_clause  */
  YYSYMBOL_having_clause = 243,            /* having_clause  */
  YYSYMBOL_group_by_clause = 244,          /* group_by_clause  */
  YYSYMBOL_set_operator = 245,             /* set_operator  */
  YYSYMBOL_table_reference = 246,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 247,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 248,     /* table_reference_name  */
  YYSYMBOL_table_name = 249,               /* table_name  */
  YYSYMBOL_table_alias = 250,              /* table_alias  */
  YYSYMBOL_with_clause = 251,              /* with_clause  */
  YYSYMBOL_with_expr_list = 252,           /* with_expr_list  */
  YYSYMBOL_with_expr = 253,                /* with_expr  */
  YYSYMBOL_join_clause = 254,              /* join_clause  */
  YYSYMBOL_join_type = 255,                /* join_type  */
  YYSYMBOL_show_statement = 256,           /* show_statement  */
  YYSYMBOL_flush_statement = 257,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 258,       /* optimize_statement  */
  YYSYMBOL_command_statement = 259,        /* command_statement  */
  YYSYMBOL_compact_statement = 260,        /* compact_statement  */
  YYSYMBOL_admin_statement = 261,          /* admin_statement  */
  YYSYMBOL_expr_array = 262,               /* expr_array  */
  YYSYMBOL_expr_array_list = 263,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 264,               /* expr_alias  */
  YYSYMBOL_expr = 265,                     /* expr  */
  YYSYMBOL_operand = 266,                  /* operand  */
  YYSYMBOL_extra_match_tensor_option = 267, /* extra_match_tensor_option  */
  YYSYMBOL_match_tensor_expr = 268,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 269,        /* match_vector_expr  */
  YYSYMBOL_match_sparse_expr = 270,        /* match_sparse_expr  */
  YYSYMBOL_match_text_expr = 271,          /* match_text_expr  */
  YYSYMBOL_query_expr = 272,               /* query_expr  */
  YYSYMBOL_fusion_expr = 273,              /* fusion_expr  */
  YYSYMBOL_sub_search = 274,               /* sub_search  */
  YYSYMBOL_sub_search_array = 275,         /* sub_search_array  */
  YYSYMBOL_function_expr = 276,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 277,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 278,             /* between_expr  */
  YYSYMBOL_in_expr = 279,                  /* in_expr  */
  YYSYMBOL_case_expr = 280,                /* case_expr  */
  YYSYMBOL_case_check_array = 281,         /* case_check_array  */
  YYSYMBOL_cast_expr = 282,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 283,            /* subquery_expr  */
  YYSYMBOL_column_expr = 284,              /* column_expr  */
  YYSYMBOL_constant_expr = 285,            /* constant_expr  */
  YYSYMBOL_common_array_expr = 286,        /* common_array_expr  */
  YYSYMBOL_common_sparse_array_expr = 287, /* common_sparse_array_expr  */
  YYSYMBOL_subarray_array_expr = 288,      /* subarray_array_expr  */
  YYSYMBOL_unclosed_subarray_array_expr = 289, /* unclosed_subarray_array_expr  */
  YYSYMBOL_sparse_array_expr = 290,        /* sparse_array_expr  */
  YYSYMBOL_long_sparse_array_expr = 291,   /* long_sparse_array_expr  */
  YYSYMBOL_unclosed_long_sparse_array_expr = 292, /* unclosed_long_sparse_array_expr  */
  YYSYMBOL_double_sparse_array_expr = 293, /* double_sparse_array_expr  */
  YYSYMBOL_unclosed_double_sparse_array_expr = 294, /* unclosed_double_sparse_array_expr  */
  YYSYMBOL_empty_array_expr = 295,         /* empty_array_expr  */
  YYSYMBOL_int_sparse_ele = 296,           /* int_sparse_ele  */
  YYSYMBOL_float_sparse_ele = 297,         /* float_sparse_ele  */
  YYSYMBOL_array_expr = 298,               /* array_expr  */
  YYSYMBOL_long_array_expr = 299,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 300, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 301,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 302, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 303,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 304,         /* copy_option_list  */
  YYSYMBOL_copy_option = 305,              /* copy_option  */
  YYSYMBOL_file_path = 306,                /* file_path  */
  YYSYMBOL_if_exists = 307,                /* if_exists  */
  YYSYMBOL_if_not_exists = 308,            /* if_not_exists  */
  YYSYMBOL_semicolon = 309,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 310,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 311,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 312, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 313,         /* index_param_list  */
  YYSYMBOL_index_param = 314,              /* index_param  */
  YYSYMBOL_index_info_list = 315,          /* index_info_list  */
  YYSYMBOL_index_info_list_one_pack = 316  /* index_info_list_one_pack  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 97 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 445 "parser.cpp"

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
#define YYFINAL  97
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1211

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  202
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  115
/* YYNRULES -- Number of rules.  */
#define YYNRULES  466
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  983

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   440


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
       2,     2,     2,     2,     2,     2,     2,   193,     2,     2,
     196,   197,   191,   189,   200,   190,   198,   192,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   201,   199,
     187,   186,   188,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   194,     2,   195,     2,     2,     2,     2,     2,     2,
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
     185
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   496,   496,   500,   506,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   522,   523,   524,   525,   526,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
     539,   546,   563,   579,   608,   624,   642,   671,   675,   681,
     684,   691,   741,   777,   778,   779,   780,   781,   782,   783,
     784,   785,   786,   787,   788,   789,   790,   791,   792,   793,
     794,   795,   796,   797,   800,   802,   803,   804,   805,   808,
     809,   810,   811,   812,   813,   814,   815,   816,   817,   818,
     819,   820,   821,   822,   823,   824,   825,   826,   827,   828,
     829,   830,   831,   832,   833,   834,   835,   836,   837,   838,
     839,   840,   841,   842,   843,   844,   845,   846,   847,   848,
     849,   850,   851,   852,   853,   854,   855,   856,   875,   879,
     889,   892,   895,   898,   902,   905,   910,   915,   922,   928,
     938,   954,   988,  1001,  1004,  1011,  1017,  1020,  1023,  1026,
    1029,  1032,  1035,  1038,  1045,  1058,  1062,  1067,  1080,  1093,
    1108,  1123,  1138,  1161,  1214,  1269,  1320,  1323,  1326,  1335,
    1345,  1348,  1352,  1357,  1379,  1382,  1387,  1403,  1406,  1410,
    1414,  1419,  1425,  1428,  1431,  1435,  1439,  1441,  1445,  1447,
    1450,  1454,  1457,  1461,  1466,  1470,  1473,  1477,  1480,  1484,
    1487,  1491,  1494,  1497,  1500,  1508,  1511,  1526,  1526,  1528,
    1542,  1551,  1556,  1565,  1570,  1575,  1581,  1588,  1591,  1595,
    1598,  1603,  1615,  1622,  1636,  1639,  1642,  1645,  1648,  1651,
    1654,  1660,  1664,  1668,  1672,  1676,  1683,  1687,  1691,  1695,
    1699,  1704,  1708,  1713,  1717,  1721,  1727,  1733,  1739,  1750,
    1761,  1772,  1784,  1796,  1809,  1823,  1834,  1848,  1864,  1881,
    1885,  1889,  1897,  1901,  1905,  1913,  1924,  1947,  1953,  1958,
    1964,  1970,  1978,  1984,  1990,  1996,  2002,  2010,  2016,  2022,
    2028,  2034,  2042,  2048,  2055,  2068,  2072,  2077,  2083,  2090,
    2098,  2107,  2117,  2127,  2138,  2149,  2161,  2173,  2183,  2194,
    2206,  2219,  2223,  2228,  2233,  2244,  2248,  2253,  2257,  2284,
    2290,  2294,  2295,  2296,  2297,  2298,  2300,  2303,  2309,  2312,
    2313,  2314,  2315,  2316,  2317,  2318,  2319,  2320,  2321,  2323,
    2326,  2332,  2351,  2393,  2439,  2457,  2475,  2483,  2494,  2500,
    2509,  2515,  2527,  2530,  2533,  2536,  2539,  2542,  2546,  2550,
    2555,  2563,  2571,  2580,  2587,  2594,  2601,  2608,  2615,  2623,
    2631,  2639,  2647,  2655,  2663,  2671,  2679,  2687,  2695,  2703,
    2711,  2741,  2749,  2758,  2766,  2775,  2783,  2789,  2796,  2802,
    2809,  2814,  2821,  2828,  2836,  2860,  2866,  2872,  2879,  2887,
    2894,  2901,  2906,  2916,  2921,  2926,  2931,  2936,  2941,  2946,
    2951,  2956,  2961,  2964,  2967,  2971,  2974,  2977,  2980,  2984,
    2987,  2990,  2994,  2998,  3003,  3008,  3011,  3015,  3019,  3026,
    3033,  3037,  3044,  3051,  3055,  3059,  3063,  3066,  3070,  3074,
    3079,  3084,  3088,  3093,  3098,  3104,  3110,  3116,  3122,  3128,
    3134,  3140,  3146,  3152,  3158,  3164,  3175,  3179,  3184,  3214,
    3224,  3229,  3234,  3239,  3245,  3249,  3250,  3252,  3253,  3255,
    3256,  3268,  3276,  3280,  3283,  3287,  3290,  3294,  3298,  3303,
    3309,  3319,  3326,  3337,  3341,  3349,  3403
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
  "TENSOR", "SPARSE", "TENSORARRAY", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL", "SECOND",
  "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH",
  "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ",
  "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER",
  "TRANSACTIONS", "TRANSACTION", "MEMINDEX", "USING", "SESSION", "GLOBAL",
  "OFF", "EXPORT", "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES",
  "VARIABLE", "DELTA", "LOGS", "CATALOGS", "CATALOG", "SEARCH", "MATCH",
  "MAXSIM", "QUERY", "QUERIES", "FUSION", "ROWLIMIT", "ADMIN", "NUMBER",
  "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'",
  "'('", "')'", "'.'", "';'", "','", "':'", "$accept", "input_pattern",
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
  "index_info_list", "index_info_list_one_pack", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-525)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-453)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      78,   268,    49,   273,    69,    36,    69,   115,   587,   713,
      31,   310,    99,   127,   219,   106,   193,   -36,   282,    93,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,   318,  -525,
    -525,   304,  -525,  -525,  -525,  -525,  -525,  -525,   254,   254,
     254,   254,    -1,    69,   290,   290,   290,   290,   290,   171,
     359,    69,     0,   383,   402,   404,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,   682,   426,    69,  -525,  -525,  -525,  -525,
    -525,   432,  -525,    43,   136,  -525,   431,  -525,   271,  -525,
    -525,   293,  -525,    69,  -525,  -525,  -525,  -525,   -17,  -525,
     401,   262,  -525,   466,    81,   276,   302,  -525,    39,  -525,
     479,  -525,  -525,     3,   447,  -525,   452,   456,   522,    69,
      69,    69,   529,   472,   341,   465,   549,    69,    69,    69,
     550,   566,   575,   517,   582,   582,   480,    65,    71,   121,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,   318,  -525,  -525,
    -525,  -525,  -525,  -525,   390,  -525,  -525,   584,  -525,   591,
    -525,  -525,   592,  -525,    69,   403,   219,   582,   594,  -525,
    -525,   605,  -525,  -525,  -525,  -525,     3,  -525,  -525,  -525,
     480,   561,   547,   552,  -525,   -30,  -525,   341,  -525,    69,
     634,    33,  -525,  -525,  -525,  -525,  -525,   576,  -525,   453,
     -37,  -525,   480,  -525,  -525,   562,   563,   445,  -525,  -525,
     843,   545,   448,   454,   344,   641,   642,   647,   648,  -525,
    -525,   650,   458,   248,   459,   461,   598,   598,  -525,    18,
     385,    37,  -525,     1,   635,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,   463,  -525,
    -525,  -525,  -114,  -525,  -525,   -89,  -525,    58,  -525,  -525,
    -525,    85,  -525,   105,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
     656,   657,  -525,  -525,  -525,  -525,  -525,  -525,   585,   304,
    -525,  -525,   187,   658,   469,   470,   -44,   480,   480,   597,
    -525,   -36,    29,   615,   484,  -525,    45,   485,  -525,    69,
     480,   575,  -525,   168,   487,   489,   185,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,   598,
     490,   663,   595,   480,   480,    98,   298,  -525,  -525,  -525,
    -525,   843,  -525,   683,   492,   494,   501,   507,   700,   707,
     299,   299,  -525,   512,  -525,  -525,  -525,  -525,   518,    30,
     637,   480,   711,   480,   480,   -34,   521,    -6,   598,   598,
     598,   598,   598,   598,   598,   598,   598,   598,   598,   598,
     598,   598,    11,  -525,   524,  -525,   715,  -525,   716,  -525,
     720,  -525,   723,   673,   297,   533,  -525,   523,   727,  -525,
      55,  -525,  -525,    10,   560,   540,  -525,   -13,   168,   480,
    -525,   318,   818,   617,   551,    66,  -525,  -525,  -525,   -36,
     634,   484,  -525,  -525,   743,   480,   548,  -525,   168,  -525,
     194,   194,   480,  -525,    70,   595,   596,   553,    42,    -8,
     364,  -525,   480,   480,   671,   480,   749,    12,   480,    94,
     116,   353,  -525,  -525,   582,  -525,  -525,  -525,   602,   559,
     598,   385,   627,  -525,   714,   714,   314,   314,   649,   714,
     714,   314,   314,   299,   299,  -525,  -525,  -525,  -525,  -525,
    -525,   558,  -525,   565,  -525,  -525,  -525,   754,   755,  -525,
     760,  -525,  -525,   758,  -525,   -36,   567,   450,  -525,    95,
    -525,   226,   517,   480,  -525,  -525,  -525,   168,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,   572,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,   573,   574,   577,   578,   579,   215,   581,   634,
     744,    29,   318,   135,  -525,  -525,   137,   599,   765,   775,
     787,   797,  -525,   796,   257,  -525,   323,   345,  -525,   606,
    -525,   818,   480,  -525,   480,    48,    96,   598,    68,   607,
    -525,    28,    72,  -525,   801,  -525,   802,  -525,  -525,   728,
     385,   714,   612,   346,  -525,   598,   808,   810,   763,   767,
     633,   347,  -525,   814,    13,    10,   770,  -525,  -525,  -525,
    -525,  -525,  -525,   771,  -525,   828,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,   632,   779,  -525,   837,   319,   853,
     231,   498,   870,   705,   718,  -525,  -525,   -19,  -525,   726,
     634,   360,   651,  -525,  -525,   684,  -525,   480,  -525,  -525,
    -525,  -525,  -525,  -525,   194,  -525,  -525,  -525,   661,   168,
      79,  -525,   480,   206,   665,   856,   524,   672,   668,   670,
     674,   678,   361,  -525,  -525,   663,   862,   873,   131,  -525,
     760,   348,    95,   450,    10,    10,   688,   226,   825,   829,
     362,   689,   690,   691,   692,   695,   696,   697,   698,   699,
     832,   732,   733,   735,   736,   739,   740,   750,   753,   756,
     757,   861,   766,   772,   773,   790,   792,   793,   794,   795,
     798,   799,   800,   803,   804,   805,   806,   807,   809,   811,
     812,   813,   815,   816,   817,   819,   820,   821,   874,   822,
    -525,  -525,    14,  -525,  -525,  -525,   371,  -525,   760,   946,
     373,  -525,  -525,  -525,   168,  -525,   395,   823,   380,   824,
      24,   826,  -525,  -525,  -525,  -525,  -525,   194,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,   949,  -525,  -525,  -525,
     906,   634,  -525,   480,   480,  -525,  -525,   965,   990,   991,
     995,   996,  1002,  1004,  1008,  1012,  1019,   827,  1022,  1023,
    1024,  1025,  1026,  1027,  1028,  1029,  1030,  1031,   838,  1033,
    1034,  1035,  1036,  1037,  1038,  1039,  1040,  1041,  1042,  1043,
    1044,  1045,  1046,  1047,  1048,  1049,  1050,  1051,  1052,  1053,
    1054,  1055,  1056,  1057,  1058,   865,  1060,  -525,  -525,   408,
     585,  -525,  -525,  1063,  -525,  1064,  1065,  1066,   409,  1067,
     480,   410,   871,   168,   875,   877,   878,   879,   880,   881,
     882,   883,   884,   885,  1077,   887,   888,   889,   890,   891,
     892,   893,   894,   895,   896,  1088,   898,   899,   900,   901,
     902,   903,   904,   905,   907,   908,   909,   910,   911,   912,
     913,   914,   915,   916,   917,   918,   919,   920,   921,   922,
     923,   924,  1097,   925,  -525,  -525,   926,   927,   928,   418,
    -525,   428,   168,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,   930,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,   932,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,  -525,
    -525,  -525,   933,  -525,  1119,  -525,  1121,   585,  1125,  1126,
    1127,  -525,  -525,  -525,  -525,  -525,  -525,   435,   934,  -525,
     938,  1130,   367,   585,  1131,  1134,   942,   585,   375,  1135,
    -525,  -525,   943,  -525,  -525,  -525,  1136,  -525,  1092,   585,
    -525,  -525,  -525
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     208,     0,     0,     0,     0,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,     0,     0,   208,     0,   450,
       3,     5,    10,    12,    13,    11,     6,     7,     9,   157,
     156,     0,     8,    14,    15,    16,    17,    18,   448,   448,
     448,   448,   448,     0,   446,   446,   446,   446,   446,   201,
       0,     0,     0,     0,     0,     0,   137,   141,   138,   139,
     140,   142,   136,   208,     0,     0,   222,   223,   221,   227,
     231,     0,   228,     0,     0,   224,     0,   226,     0,   249,
     251,     0,   229,     0,   252,   253,   254,   257,   201,   255,
       0,   207,   209,     0,     0,     0,     0,     1,   208,     2,
     191,   193,   194,     0,   180,   162,   168,     0,     0,     0,
       0,     0,     0,     0,   134,     0,     0,     0,     0,     0,
       0,     0,     0,   186,     0,     0,     0,     0,     0,     0,
     135,    19,    24,    26,    25,    20,    21,    23,    22,    27,
      28,    29,    30,   237,   238,   232,   233,     0,   234,     0,
     225,   250,     0,   274,     0,     0,     0,     0,     0,   291,
     275,     0,   161,   160,     4,   192,     0,   158,   159,   179,
       0,     0,   176,     0,    31,     0,    32,   134,   451,     0,
       0,   208,   445,   148,   150,   149,   151,     0,   202,     0,
     186,   145,     0,   130,   444,     0,     0,   379,   383,   386,
     387,     0,     0,     0,     0,     0,     0,     0,     0,   384,
     385,     0,     0,     0,     0,     0,     0,     0,   381,     0,
     208,     0,   295,   300,   301,   315,   313,   316,   314,   317,
     318,   310,   305,   304,   303,   311,   312,   302,   309,   308,
     394,   396,     0,   397,   405,     0,   406,     0,   398,   395,
     416,     0,   417,     0,   393,   261,   263,   262,   259,   260,
     266,   268,   267,   264,   265,   271,   273,   272,   269,   270,
       0,     0,   240,   239,   245,   235,   236,   230,   454,     0,
     210,   258,   292,   276,     0,     0,   182,     0,     0,   178,
     447,   208,     0,     0,     0,   128,     0,     0,   132,     0,
       0,     0,   144,   185,     0,     0,     0,   425,   424,   427,
     426,   429,   428,   431,   430,   433,   432,   435,   434,     0,
       0,   345,   208,     0,     0,     0,     0,   388,   389,   390,
     391,     0,   392,     0,     0,     0,     0,     0,     0,     0,
     347,   346,   422,   419,   413,   403,   408,   411,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   402,     0,   407,     0,   410,     0,   418,
       0,   421,     0,   246,   241,     0,   256,     0,     0,   293,
       0,   165,   164,     0,   184,   167,   169,   174,   175,     0,
     163,    34,     0,     0,     0,     0,    37,    39,    40,   208,
       0,    36,   463,   133,     0,     0,   131,   152,   147,   146,
       0,     0,     0,   340,     0,   208,     0,     0,     0,     0,
       0,   370,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   307,   306,     0,   296,   299,   363,   364,     0,
       0,   208,     0,   344,   354,   355,   358,   359,     0,   361,
     353,   356,   357,   349,   348,   350,   351,   352,   380,   382,
     404,     0,   409,     0,   412,   420,   423,     0,     0,   242,
       0,   211,   294,     0,   277,   208,   181,   195,   197,   206,
     198,     0,   186,     0,   172,   173,   171,   177,    43,    46,
      47,    44,    45,    48,    49,    65,    50,    52,    51,    68,
      55,    56,    57,    53,    54,    58,    59,    60,    61,    62,
      63,    64,     0,     0,     0,     0,     0,   454,     0,     0,
     456,     0,    35,     0,   464,   129,     0,     0,     0,     0,
       0,     0,   440,     0,     0,   436,     0,     0,   341,     0,
     375,     0,     0,   368,     0,     0,     0,     0,     0,     0,
     379,     0,     0,   328,     0,   330,     0,   415,   414,     0,
     208,   362,     0,     0,   343,     0,     0,     0,   247,   243,
     459,     0,   457,   278,     0,     0,     0,   215,   216,   217,
     218,   214,   219,     0,   204,     0,   199,   334,   332,   335,
     333,   336,   337,   338,   183,   190,   170,     0,     0,     0,
       0,     0,     0,     0,     0,   121,   122,   125,   118,   125,
       0,     0,     0,    33,    38,   466,   297,     0,   442,   441,
     439,   438,   443,   155,     0,   153,   342,   376,     0,   372,
       0,   371,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   377,   366,   365,     0,     0,     0,   453,
       0,     0,   206,   196,     0,     0,   203,     0,     0,   188,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     123,   120,     0,   119,    42,    41,     0,   127,     0,     0,
       0,   437,   374,   369,   373,   360,     0,     0,     0,     0,
       0,     0,   399,   401,   400,   329,   331,     0,   378,   367,
     248,   244,   460,   462,   461,   458,     0,   279,   200,   212,
       0,     0,   339,     0,     0,   166,    67,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   124,   126,     0,
     454,   298,   419,     0,   326,     0,     0,     0,     0,   280,
       0,     0,   189,   187,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   455,   465,     0,     0,     0,     0,
     154,     0,   213,   205,    66,    72,    73,    70,    71,    74,
      75,    76,    77,    78,     0,    69,   102,   103,   100,   101,
     104,   105,   106,   107,   108,     0,    99,    83,    84,    81,
      82,    85,    86,    87,    80,   113,   114,   111,   112,   115,
     116,   117,   110,    91,    92,    89,    90,    93,    94,    95,
      96,    97,     0,    88,     0,   327,     0,   454,     0,     0,
       0,   282,   281,   287,    79,   109,    98,     0,   320,   325,
       0,   288,   283,   454,     0,     0,     0,   454,     0,     0,
     284,   323,     0,   319,   321,   324,     0,   289,   285,   454,
     290,   286,   322
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -525,  -525,  -525,  1059,  -525,  1081,  -525,   614,  -525,   600,
    -525,   530,   527,  -525,  -404,  1085,  1086,   973,  -525,  -525,
    1089,  -525,   852,  1091,  1093,   -58,  1138,   -16,   886,   992,
     -46,  -525,  -525,   666,  -525,  -525,  -525,  -525,  -525,  -525,
    -181,  -525,  -525,  -525,  -525,   583,  -183,    32,   499,  -525,
    -525,  1006,  -525,  -525,  1100,  1101,  1103,  1104,  1106,  -525,
    -168,  -525,   830,  -192,  -191,  -525,  -475,  -474,  -470,  -469,
    -450,  -449,   493,  -525,  -525,  -525,  -525,  -525,  -525,   845,
    -525,  -525,   734,   451,  -215,  -525,  -525,  -525,   525,  -525,
    -525,  -525,  -525,   528,   831,   833,   -11,  -525,  -525,  -525,
    -525,   963,  -414,   542,  -117,   449,   488,  -525,  -525,  -524,
    -525,   455,   519,  -525,   769
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    18,    19,    20,   130,    21,   405,   406,   407,   527,
     617,   618,   724,   408,   296,    22,    23,   181,    24,    63,
      25,   190,   191,    26,    27,    28,    29,    30,   105,   167,
     106,   172,   395,   396,   496,   289,   400,   170,   394,   492,
     193,   765,   669,   103,   486,   487,   488,   489,   596,    31,
      91,    92,   490,   593,    32,    33,    34,    35,    36,    37,
     221,   416,   222,   223,   224,   966,   225,   226,   227,   228,
     229,   230,   603,   604,   231,   232,   233,   234,   235,   326,
     236,   237,   238,   239,   240,   741,   241,   242,   243,   244,
     245,   246,   247,   248,   346,   347,   249,   250,   251,   252,
     253,   254,   544,   545,   195,   116,   108,    99,   113,   386,
     623,   581,   582,   411,   412
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     303,    96,   286,   619,   345,   137,   533,   546,   196,   302,
     321,   104,   325,    49,   468,   560,   597,   598,   198,   199,
     200,   599,   600,   342,   343,   340,   341,   291,   349,   342,
     343,   192,   402,   100,   393,   101,    50,   102,    52,  -449,
     281,   601,   602,   449,    14,    89,     1,   154,     2,     3,
       4,     5,     6,     7,     8,     9,   613,   168,   352,   494,
     495,    83,    10,  -452,    11,    12,    13,   297,   255,   452,
     256,   257,    49,   107,   260,   114,   261,   262,   124,   125,
     552,   373,    43,   123,   483,     1,   374,     2,     3,     4,
       5,     6,     7,     8,     9,   397,   398,   144,   594,   551,
     484,    10,    87,    11,    12,    13,   375,   614,   418,   615,
     616,   376,   722,    14,    51,   153,   205,   206,   207,    14,
     285,   450,   208,   453,   265,   621,   266,   267,   321,   258,
      88,   428,   429,   641,   752,   263,   753,   754,   424,   353,
     354,   175,   176,   177,   353,   354,   209,   210,   211,   184,
     185,   186,   595,   353,   354,   403,   351,   404,    14,   470,
      17,   447,   448,   301,   733,   298,   292,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,   466,
     467,   121,   353,   354,   642,   268,   278,   324,   197,   198,
     199,   200,   597,   598,   353,   354,   126,   599,   600,   166,
     353,   354,   469,   218,   348,    15,   485,   497,   219,    94,
     662,   294,   219,   344,   146,   147,   726,   601,   602,   344,
     353,   354,    90,    16,   353,   354,   372,   443,   646,   388,
     259,   353,   354,   401,   350,    17,   264,   351,   158,   389,
     555,   556,   413,   558,    15,   414,   562,   536,   353,   354,
     353,   354,   422,   377,   547,   159,   160,   161,   378,   571,
     201,   202,    16,   530,   538,   539,   531,   548,   644,   203,
     351,   204,   647,    93,    17,   540,   541,   542,    53,    54,
     379,   426,    97,   573,    55,   380,   269,   205,   206,   207,
     613,   563,    98,   208,   564,   385,   885,    38,    39,    40,
     381,   397,    44,    45,    46,   382,   427,   148,   149,    41,
      42,   605,   104,   565,    47,    48,   566,   209,   210,   211,
     353,   354,   693,   694,   695,   696,   697,   569,   107,   698,
     699,   417,   625,   828,   626,   414,   357,   351,   100,   212,
     101,   614,   102,   615,   616,   478,   479,   197,   198,   199,
     200,   532,   700,   358,   359,   360,   361,   831,   567,   568,
     639,   363,   640,   213,   115,   214,   643,   215,   334,   121,
     335,   336,   337,   122,   216,   217,   218,   543,   756,   219,
     757,   220,   423,   431,   655,   432,   127,   433,   197,   198,
     199,   200,   364,   365,   366,   367,   368,   369,   370,   371,
     342,   822,   652,   735,   213,   128,   214,   129,   215,   549,
     671,   672,   673,   674,   675,   969,   970,   676,   677,   201,
     202,   976,   977,   959,   678,   679,   680,   584,   203,   143,
     204,   739,   270,   324,   150,   572,   271,   272,   145,   971,
     681,   273,   274,   975,   357,   151,   205,   206,   207,   553,
     734,   554,   208,   433,   633,   982,   152,   634,   155,   730,
     201,   202,   156,  -453,  -453,    14,    84,    85,    86,   203,
     949,   204,   157,   162,   950,   951,   209,   210,   211,   952,
     953,   759,   760,   197,   198,   199,   200,   205,   206,   207,
     369,   370,   371,   208,   117,   118,   119,   120,   212,   163,
     165,  -453,  -453,   367,   368,   369,   370,   371,   586,  -220,
     587,   588,   589,   590,   169,   591,   592,   209,   210,   211,
     635,   171,   213,   634,   214,   174,   215,   109,   110,   111,
     112,   173,   178,   216,   217,   218,   179,   180,   219,   212,
     220,   182,   636,   654,   659,   351,   351,   660,   197,   198,
     199,   200,   183,   187,   651,   201,   202,   727,   749,   766,
     414,   351,   767,   213,   203,   214,   204,   215,   818,   188,
     821,   414,   833,   351,   216,   217,   218,   824,   189,   219,
     825,   220,   205,   206,   207,   192,   194,   275,   208,   701,
     702,   703,   704,   705,   276,   832,   706,   707,   277,   279,
     282,   197,   198,   199,   200,   884,   890,   893,   660,   634,
     414,   283,   209,   210,   211,   947,   287,   288,   948,   708,
     319,   320,    56,    57,    58,    59,    60,    61,   290,   203,
      62,   204,   963,   737,   212,   964,   744,   295,   892,   300,
     299,   306,   304,   305,   322,   327,   328,   205,   206,   207,
     323,   329,   330,   208,   333,   338,   331,   339,   213,   383,
     214,   372,   215,   384,   390,   385,   391,   392,   399,   216,
     217,   218,   409,   319,   219,    14,   220,   209,   210,   211,
     410,   415,   203,   420,   204,   421,   425,   434,   435,     1,
     436,     2,     3,     4,     5,     6,     7,   437,     9,   212,
     205,   206,   207,   438,   439,    10,   208,    11,    12,    13,
     355,   440,   356,   441,   446,   442,   444,   451,   219,   477,
     481,   471,   473,   213,   426,   214,   475,   215,   476,   480,
     209,   210,   211,   482,   216,   217,   218,   491,   426,   219,
     493,   220,    64,    65,   528,    66,   535,   529,   537,   557,
     550,   450,   212,   559,   353,   570,   574,    67,    68,   576,
     578,   579,    14,   580,   583,   357,   577,   585,   607,   608,
     609,   628,   622,   610,   611,   612,   213,   620,   214,   357,
     215,   629,   358,   359,   360,   361,   362,   216,   217,   218,
     363,   630,   219,   357,   220,   627,   358,   359,   360,   361,
     631,   575,   632,   637,   363,   648,   649,   645,   650,   653,
     358,   359,   360,   361,   568,   567,   656,   657,   363,   658,
     661,   364,   365,   366,   367,   368,   369,   370,   371,   664,
     665,   666,   667,   668,   720,   364,   365,   366,   367,   368,
     369,   370,   371,   670,   357,   721,   729,   728,    15,   364,
     365,   366,   367,   368,   369,   370,   371,   722,   732,   736,
     738,  -453,  -453,   360,   361,   745,   740,   746,   750,  -453,
     747,    69,    70,    71,    72,   748,    73,    74,    17,   751,
     763,    75,    76,    77,   761,   764,    78,    79,    80,   768,
     769,   770,   771,    81,    82,   772,   773,   774,   775,   776,
    -453,   365,   366,   367,   368,   369,   370,   371,   498,   499,
     500,   501,   502,   503,   504,   505,   506,   507,   508,   509,
     510,   511,   512,   513,   514,   777,   515,   516,   517,   518,
     519,   520,   778,   779,   521,   780,   781,   522,   523,   782,
     783,   524,   525,   526,   682,   683,   684,   685,   686,   820,
     784,   687,   688,   785,   788,   829,   786,   787,   689,   690,
     691,   709,   710,   711,   712,   713,   789,   815,   714,   715,
     830,   834,   790,   791,   692,   716,   717,   718,   307,   308,
     309,   310,   311,   312,   313,   314,   315,   316,   317,   318,
     792,   719,   793,   794,   795,   796,   835,   836,   797,   798,
     799,   837,   838,   800,   801,   802,   803,   804,   839,   805,
     840,   806,   807,   808,   841,   809,   810,   811,   842,   812,
     813,   814,   816,   823,   826,   843,   827,   844,   845,   846,
     847,   848,   849,   850,   851,   852,   853,   854,   855,   856,
     857,   858,   859,   860,   861,   862,   863,   864,   865,   866,
     867,   868,   869,   870,   871,   872,   873,   874,   875,   876,
     877,   878,   879,   880,   881,   882,   883,   886,   887,   888,
     889,   351,   894,   891,   895,   896,   897,   898,   899,   900,
     901,   902,   903,   904,   905,   906,   907,   908,   909,   910,
     911,   912,   913,   914,   915,   916,   917,   918,   919,   920,
     921,   922,   923,   942,   924,   925,   926,   927,   928,   929,
     930,   931,   932,   933,   934,   935,   936,   937,   938,   939,
     940,   941,   943,   957,   945,   958,   944,   954,   946,   955,
     956,   960,   961,   962,   965,   967,   968,   972,   973,   974,
     979,   978,   980,   981,   131,   624,   725,   723,   132,   133,
     293,   638,   134,   419,   135,    95,   136,   164,   284,   606,
     762,   758,   280,   138,   139,   387,   140,   141,   663,   142,
     430,   561,   742,   817,   332,   743,   731,     0,     0,   755,
     534,   445,     0,   819,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   472,     0,     0,
       0,   474
};

static const yytype_int16 yycheck[] =
{
     192,    17,   170,   527,   219,    63,   410,   421,   125,   190,
     201,     8,   204,     3,     3,     3,   491,   491,     4,     5,
       6,   491,   491,     5,     6,   216,   217,    57,   220,     5,
       6,    68,     3,    20,    78,    22,     4,    24,     6,     0,
     157,   491,   491,    77,    80,    13,     7,    64,     9,    10,
      11,    12,    13,    14,    15,    16,    75,   103,    57,    72,
      73,    30,    23,    64,    25,    26,    27,    34,     3,    75,
       5,     6,     3,    74,     3,    43,     5,     6,    78,    79,
      88,   195,    33,    51,    29,     7,   200,     9,    10,    11,
      12,    13,    14,    15,    16,   287,   288,    65,     3,    57,
      45,    23,     3,    25,    26,    27,   195,   126,   300,   128,
     129,   200,   131,    80,    78,    83,   102,   103,   104,    80,
     166,   155,   108,   129,     3,   529,     5,     6,   319,    64,
       3,   323,   324,    85,     3,    64,     5,     6,   306,   152,
     153,   109,   110,   111,   152,   153,   132,   133,   134,   117,
     118,   119,    57,   152,   153,   126,   200,   128,    80,   374,
     196,   353,   354,   200,    85,   181,   196,   358,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,   369,   370,
     371,   198,   152,   153,    88,    64,   154,    89,     3,     4,
       5,     6,   667,   667,   152,   153,   196,   667,   667,   196,
     152,   153,   191,   191,   220,   166,   196,   399,   194,    16,
     197,   179,   194,   195,   171,   172,   620,   667,   667,   195,
     152,   153,     3,   184,   152,   153,   198,   197,   200,    42,
     165,   152,   153,   291,   197,   196,   165,   200,   157,    52,
     432,   433,   197,   435,   166,   200,   438,   415,   152,   153,
     152,   153,    67,   195,   422,   174,   175,   176,   200,   450,
      75,    76,   184,   197,    70,    71,   200,   197,   200,    84,
     200,    86,   200,   167,   196,    81,    82,    83,   163,   164,
     195,    75,     0,   451,   169,   200,   165,   102,   103,   104,
      75,   197,   199,   108,   200,    80,   820,    29,    30,    31,
     195,   493,    29,    30,    31,   200,   322,   171,   172,    41,
      42,   492,     8,   197,    41,    42,   200,   132,   133,   134,
     152,   153,    91,    92,    93,    94,    95,   444,    74,    98,
      99,   299,   197,   747,   197,   200,   130,   200,    20,   154,
      22,   126,    24,   128,   129,    48,    49,     3,     4,     5,
       6,   409,   121,   147,   148,   149,   150,   761,     5,     6,
     552,   155,   554,   178,    74,   180,   557,   182,   120,   198,
     122,   123,   124,    14,   189,   190,   191,   183,    30,   194,
      32,   196,   197,    85,   575,    87,     3,    89,     3,     4,
       5,     6,   186,   187,   188,   189,   190,   191,   192,   193,
       5,     6,   570,   197,   178,     3,   180,     3,   182,   425,
      91,    92,    93,    94,    95,    48,    49,    98,    99,    75,
      76,    46,    47,   947,   105,   106,   107,   485,    84,     3,
      86,   646,    42,    89,     3,   451,    46,    47,     6,   963,
     121,    51,    52,   967,   130,   174,   102,   103,   104,    85,
     642,    87,   108,    89,   197,   979,   163,   200,    57,   627,
      75,    76,   200,   149,   150,    80,   156,   157,   158,    84,
      42,    86,     6,   197,    46,    47,   132,   133,   134,    51,
      52,   664,   665,     3,     4,     5,     6,   102,   103,   104,
     191,   192,   193,   108,    45,    46,    47,    48,   154,   197,
      21,   187,   188,   189,   190,   191,   192,   193,    58,    59,
      60,    61,    62,    63,    67,    65,    66,   132,   133,   134,
     197,    69,   178,   200,   180,     3,   182,    39,    40,    41,
      42,    75,     3,   189,   190,   191,    64,   196,   194,   154,
     196,    76,   197,   197,   197,   200,   200,   200,     3,     4,
       5,     6,     3,     3,   570,    75,    76,   197,   197,   197,
     200,   200,   200,   178,    84,   180,    86,   182,   197,     3,
     197,   200,   764,   200,   189,   190,   191,   197,     3,   194,
     200,   196,   102,   103,   104,    68,     4,     3,   108,    91,
      92,    93,    94,    95,     3,   763,    98,    99,     6,   196,
       6,     3,     4,     5,     6,   197,   197,   197,   200,   200,
     200,     6,   132,   133,   134,   197,    55,    70,   200,   121,
      75,    76,    35,    36,    37,    38,    39,    40,    76,    84,
      43,    86,   197,   644,   154,   200,   647,     3,   830,   186,
      64,   196,    80,    80,   196,     4,     4,   102,   103,   104,
     196,     4,     4,   108,   196,   196,     6,   196,   178,     3,
     180,   198,   182,     6,     6,    80,   197,   197,    71,   189,
     190,   191,    57,    75,   194,    80,   196,   132,   133,   134,
     196,   196,    84,   196,    86,   196,   196,     4,   196,     7,
     196,     9,    10,    11,    12,    13,    14,   196,    16,   154,
     102,   103,   104,   196,     4,    23,   108,    25,    26,    27,
      75,     4,    77,   201,     3,   197,    79,   196,   194,    46,
     197,     6,     6,   178,    75,   180,     6,   182,     5,   196,
     132,   133,   134,     6,   189,   190,   191,   177,    75,   194,
     200,   196,    29,    30,   127,    32,     3,   196,   200,    78,
     197,   155,   154,     4,   152,   196,   129,    44,    45,   201,
       6,     6,    80,     3,     6,   130,   201,   200,   196,   196,
     196,     6,    28,   196,   196,   196,   178,   196,   180,   130,
     182,     6,   147,   148,   149,   150,   151,   189,   190,   191,
     155,     4,   194,   130,   196,   196,   147,   148,   149,   150,
       3,   152,     6,   197,   155,     4,     4,   200,    80,   197,
     147,   148,   149,   150,     6,     5,    53,    50,   155,   186,
       6,   186,   187,   188,   189,   190,   191,   192,   193,    59,
      59,     3,   200,    54,   129,   186,   187,   188,   189,   190,
     191,   192,   193,     6,   130,   127,   162,   196,   166,   186,
     187,   188,   189,   190,   191,   192,   193,   131,   197,   194,
       4,   147,   148,   149,   150,   197,   194,   197,     6,   155,
     196,   158,   159,   160,   161,   197,   163,   164,   196,     6,
      55,   168,   169,   170,   196,    56,   173,   174,   175,   200,
     200,   200,   200,   180,   181,   200,   200,   200,   200,   200,
     186,   187,   188,   189,   190,   191,   192,   193,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,    93,   108,   109,   110,   111,
     112,   113,   200,   200,   116,   200,   200,   119,   120,   200,
     200,   123,   124,   125,    91,    92,    93,    94,    95,     3,
     200,    98,    99,   200,    93,     6,   200,   200,   105,   106,
     107,    91,    92,    93,    94,    95,   200,    93,    98,    99,
      64,     6,   200,   200,   121,   105,   106,   107,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     200,   121,   200,   200,   200,   200,     6,     6,   200,   200,
     200,     6,     6,   200,   200,   200,   200,   200,     6,   200,
       6,   200,   200,   200,     6,   200,   200,   200,     6,   200,
     200,   200,   200,   200,   200,     6,   200,   200,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   200,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   200,     6,     4,     4,     4,
       4,   200,   197,     6,   197,   197,   197,   197,   197,   197,
     197,   197,   197,     6,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,     6,   197,   197,   197,   197,   197,
     197,   197,   197,     6,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,     4,   197,     4,   200,   197,   200,   197,
     197,     6,     6,     6,   200,   197,     6,     6,     4,   197,
     197,     6,     6,    51,    63,   531,   619,   617,    63,    63,
     177,   551,    63,   301,    63,    17,    63,    98,   166,   493,
     667,   662,   156,    63,    63,   279,    63,    63,   585,    63,
     325,   437,   647,   722,   211,   647,   634,    -1,    -1,   660,
     411,   351,    -1,   728,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   376,    -1,    -1,
      -1,   378
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    80,   166,   184,   196,   203,   204,
     205,   207,   217,   218,   220,   222,   225,   226,   227,   228,
     229,   251,   256,   257,   258,   259,   260,   261,    29,    30,
      31,    41,    42,    33,    29,    30,    31,    41,    42,     3,
     249,    78,   249,   163,   164,   169,    35,    36,    37,    38,
      39,    40,    43,   221,    29,    30,    32,    44,    45,   158,
     159,   160,   161,   163,   164,   168,   169,   170,   173,   174,
     175,   180,   181,    30,   156,   157,   158,     3,     3,   249,
       3,   252,   253,   167,    16,   228,   229,     0,   199,   309,
      20,    22,    24,   245,     8,   230,   232,    74,   308,   308,
     308,   308,   308,   310,   249,    74,   307,   307,   307,   307,
     307,   198,    14,   249,    78,    79,   196,     3,     3,     3,
     206,   207,   217,   218,   222,   225,   226,   227,   256,   257,
     258,   259,   260,     3,   249,     6,   171,   172,   171,   172,
       3,   174,   163,   249,    64,    57,   200,     6,   157,   174,
     175,   176,   197,   197,   205,    21,   196,   231,   232,    67,
     239,    69,   233,    75,     3,   249,   249,   249,     3,    64,
     196,   219,    76,     3,   249,   249,   249,     3,     3,     3,
     223,   224,    68,   242,     4,   306,   306,     3,     4,     5,
       6,    75,    76,    84,    86,   102,   103,   104,   108,   132,
     133,   134,   154,   178,   180,   182,   189,   190,   191,   194,
     196,   262,   264,   265,   266,   268,   269,   270,   271,   272,
     273,   276,   277,   278,   279,   280,   282,   283,   284,   285,
     286,   288,   289,   290,   291,   292,   293,   294,   295,   298,
     299,   300,   301,   302,   303,     3,     5,     6,    64,   165,
       3,     5,     6,    64,   165,     3,     5,     6,    64,   165,
      42,    46,    47,    51,    52,     3,     3,     6,   249,   196,
     253,   306,     6,     6,   231,   232,   262,    55,    70,   237,
      76,    57,   196,   219,   249,     3,   216,    34,   229,    64,
     186,   200,   242,   265,    80,    80,   196,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    75,
      76,   266,   196,   196,    89,   265,   281,     4,     4,     4,
       4,     6,   303,   196,   120,   122,   123,   124,   196,   196,
     266,   266,     5,     6,   195,   286,   296,   297,   229,   265,
     197,   200,    57,   152,   153,    75,    77,   130,   147,   148,
     149,   150,   151,   155,   186,   187,   188,   189,   190,   191,
     192,   193,   198,   195,   200,   195,   200,   195,   200,   195,
     200,   195,   200,     3,     6,    80,   311,   230,    42,    52,
       6,   197,   197,    78,   240,   234,   235,   265,   265,    71,
     238,   227,     3,   126,   128,   208,   209,   210,   215,    57,
     196,   315,   316,   197,   200,   196,   263,   249,   265,   224,
     196,   196,    67,   197,   262,   196,    75,   229,   265,   265,
     281,    85,    87,    89,     4,   196,   196,   196,   196,     4,
       4,   201,   197,   197,    79,   264,     3,   265,   265,    77,
     155,   196,    75,   129,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,     3,   191,
     286,     6,   296,     6,   297,     6,     5,    46,    48,    49,
     196,   197,     6,    29,    45,   196,   246,   247,   248,   249,
     254,   177,   241,   200,    72,    73,   236,   265,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   108,   109,   110,   111,   112,
     113,   116,   119,   120,   123,   124,   125,   211,   127,   196,
     197,   200,   227,   216,   316,     3,   262,   200,    70,    71,
      81,    82,    83,   183,   304,   305,   304,   262,   197,   229,
     197,    57,    88,    85,    87,   265,   265,    78,   265,     4,
       3,   284,   265,   197,   200,   197,   200,     5,     6,   306,
     196,   266,   229,   262,   129,   152,   201,   201,     6,     6,
       3,   313,   314,     6,   227,   200,    58,    60,    61,    62,
      63,    65,    66,   255,     3,    57,   250,   268,   269,   270,
     271,   272,   273,   274,   275,   242,   235,   196,   196,   196,
     196,   196,   196,    75,   126,   128,   129,   212,   213,   311,
     196,   216,    28,   312,   209,   197,   197,   196,     6,     6,
       4,     3,     6,   197,   200,   197,   197,   197,   211,   265,
     265,    85,    88,   266,   200,   200,   200,   200,     4,     4,
      80,   229,   262,   197,   197,   266,    53,    50,   186,   197,
     200,     6,   197,   247,    59,    59,     3,   200,    54,   244,
       6,    91,    92,    93,    94,    95,    98,    99,   105,   106,
     107,   121,    91,    92,    93,    94,    95,    98,    99,   105,
     106,   107,   121,    91,    92,    93,    94,    95,    98,    99,
     121,    91,    92,    93,    94,    95,    98,    99,   121,    91,
      92,    93,    94,    95,    98,    99,   105,   106,   107,   121,
     129,   127,   131,   213,   214,   214,   216,   197,   196,   162,
     262,   305,   197,    85,   265,   197,   194,   298,     4,   286,
     194,   287,   290,   295,   298,   197,   197,   196,   197,   197,
       6,     6,     3,     5,     6,   314,    30,    32,   250,   248,
     248,   196,   274,    55,    56,   243,   197,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,    93,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,    93,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,    93,   200,   285,   197,   313,
       3,   197,     6,   200,   197,   200,   200,   200,   304,     6,
      64,   216,   262,   265,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,   200,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,   200,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,   200,     6,   197,   311,     4,     4,     4,     4,
     197,     6,   265,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,     6,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,     6,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,     6,   197,   200,   197,   200,   197,   200,    42,
      46,    47,    51,    52,   197,   197,   197,     4,     4,   311,
       6,     6,     6,   197,   200,   200,   267,   197,     6,    48,
      49,   311,     6,     4,   197,   311,    46,    47,     6,   197,
       6,    51,   311
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   202,   203,   204,   204,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   207,   207,   207,   207,   207,   207,   208,   208,   209,
     209,   210,   210,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   211,   211,   212,   212,
     213,   213,   213,   213,   214,   214,   215,   215,   216,   216,
     217,   218,   218,   219,   219,   220,   221,   221,   221,   221,
     221,   221,   221,   221,   222,   223,   223,   224,   225,   225,
     225,   225,   225,   226,   226,   226,   227,   227,   227,   227,
     228,   228,   229,   230,   231,   231,   232,   233,   233,   234,
     234,   235,   236,   236,   236,   237,   237,   238,   238,   239,
     239,   240,   240,   241,   241,   242,   242,   243,   243,   244,
     244,   245,   245,   245,   245,   246,   246,   247,   247,   248,
     248,   249,   249,   250,   250,   250,   250,   251,   251,   252,
     252,   253,   254,   254,   255,   255,   255,   255,   255,   255,
     255,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   257,   257,   257,   258,   258,   259,   259,   259,
     259,   259,   259,   259,   259,   259,   259,   259,   259,   259,
     259,   259,   259,   259,   260,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   261,   261,   261,   261,   261,
     261,   261,   261,   261,   261,   262,   262,   263,   263,   264,
     264,   265,   265,   265,   265,   265,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   267,
     267,   268,   269,   269,   270,   270,   271,   271,   272,   272,
     273,   273,   274,   274,   274,   274,   274,   274,   275,   275,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
     276,   276,   276,   277,   277,   278,   279,   279,   280,   280,
     280,   280,   281,   281,   282,   283,   283,   283,   283,   284,
     284,   284,   284,   285,   285,   285,   285,   285,   285,   285,
     285,   285,   285,   285,   285,   286,   286,   286,   286,   287,
     287,   287,   288,   289,   289,   290,   290,   291,   292,   292,
     293,   294,   294,   295,   296,   297,   298,   298,   299,   300,
     300,   301,   302,   302,   303,   303,   303,   303,   303,   303,
     303,   303,   303,   303,   303,   303,   304,   304,   305,   305,
     305,   305,   305,   305,   306,   307,   307,   308,   308,   309,
     309,   310,   310,   311,   311,   312,   312,   313,   313,   314,
     314,   314,   314,   315,   315,   316,   316
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
       6,     6,     6,     6,     6,     6,     6,     6,     7,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     7,
       6,     6,     6,     6,     6,     6,     6,     6,     1,     2,
       2,     1,     1,     2,     2,     0,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     1,     3,     3,     4,     4,
       4,     4,     6,     8,    11,     8,     1,     1,     3,     3,
       3,     3,     2,     4,     3,     3,     8,     3,     0,     1,
       3,     2,     1,     1,     0,     2,     0,     2,     0,     1,
       0,     2,     0,     2,     0,     2,     0,     2,     0,     3,
       0,     1,     2,     1,     1,     1,     3,     1,     1,     2,
       4,     1,     3,     2,     1,     5,     0,     2,     0,     1,
       3,     5,     4,     6,     1,     1,     1,     1,     1,     1,
       0,     2,     2,     2,     2,     3,     2,     2,     2,     2,
       4,     2,     3,     3,     3,     4,     4,     3,     3,     4,
       4,     5,     6,     7,     9,     4,     5,     7,     9,     2,
       3,     2,     2,     2,     2,     2,     5,     2,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     3,     3,     4,     6,     7,     9,
      10,    12,    12,    13,    14,    15,    16,    12,    13,    15,
      16,     3,     4,     5,     6,     1,     3,     3,     5,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       0,    12,    14,    12,    12,    10,     7,     9,     4,     6,
       4,     6,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     4,     5,     4,     3,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       6,     3,     4,     3,     3,     5,     5,     6,     4,     6,
       3,     5,     4,     5,     6,     4,     5,     5,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     2,     3,     1,     1,     2,     2,     3,
       2,     2,     3,     2,     3,     3,     1,     1,     2,     2,
       3,     2,     2,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     3,     2,     2,
       1,     2,     2,     2,     1,     2,     0,     3,     0,     1,
       0,     2,     0,     4,     0,     4,     0,     1,     3,     1,
       3,     3,     3,     1,     2,     6,     3
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
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2266 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2274 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 226 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2288 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 216 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2302 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 305 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2313 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2321 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2330 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 316 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2339 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 276 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2353 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 269 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2364 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2374 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2384 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2394 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2404 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2414 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 351 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2424 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 259 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 259 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2452 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 339 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2462 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2478 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2487 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2495 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2511 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2534 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2543 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2552 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 296 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2565 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2574 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2588 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 286 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2602 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 345 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2612 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2621 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2635 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 246 "parser.y"
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
#line 2652 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2660 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2668 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2676 "parser.cpp"
        break;

    case YYSYMBOL_extra_match_tensor_option: /* extra_match_tensor_option  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2684 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2692 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2700 "parser.cpp"
        break;

    case YYSYMBOL_match_sparse_expr: /* match_sparse_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2708 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2716 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2724 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2732 "parser.cpp"
        break;

    case YYSYMBOL_sub_search: /* sub_search  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2740 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 236 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2754 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2762 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2770 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2778 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2786 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2794 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 357 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2807 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2815 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2823 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 321 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2831 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2839 "parser.cpp"
        break;

    case YYSYMBOL_common_array_expr: /* common_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2847 "parser.cpp"
        break;

    case YYSYMBOL_common_sparse_array_expr: /* common_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2855 "parser.cpp"
        break;

    case YYSYMBOL_subarray_array_expr: /* subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2863 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_subarray_array_expr: /* unclosed_subarray_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2871 "parser.cpp"
        break;

    case YYSYMBOL_sparse_array_expr: /* sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2879 "parser.cpp"
        break;

    case YYSYMBOL_long_sparse_array_expr: /* long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2887 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_sparse_array_expr: /* unclosed_long_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2895 "parser.cpp"
        break;

    case YYSYMBOL_double_sparse_array_expr: /* double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2903 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_sparse_array_expr: /* unclosed_double_sparse_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2911 "parser.cpp"
        break;

    case YYSYMBOL_empty_array_expr: /* empty_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2919 "parser.cpp"
        break;

    case YYSYMBOL_int_sparse_ele: /* int_sparse_ele  */
#line 366 "parser.y"
            {
    delete (((*yyvaluep).int_sparse_ele_t));
}
#line 2927 "parser.cpp"
        break;

    case YYSYMBOL_float_sparse_ele: /* float_sparse_ele  */
#line 370 "parser.y"
            {
    delete (((*yyvaluep).float_sparse_ele_t));
}
#line 2935 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2943 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2951 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2959 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2967 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2975 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 325 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2983 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 312 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2991 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 209 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 3002 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3016 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 192 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 3030 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 3044 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list_one_pack: /* index_info_list_one_pack  */
#line 182 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 3058 "parser.cpp"
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

#line 3166 "parser.cpp"

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
#line 496 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3381 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 500 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3392 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 506 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3403 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 513 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3409 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 514 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3415 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 515 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3421 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 516 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3427 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 517 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3433 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 518 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3439 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 519 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3445 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 520 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3451 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 521 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3457 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 522 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3463 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 523 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3469 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 524 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3475 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 525 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3481 "parser.cpp"
    break;

  case 18: /* statement: admin_statement  */
#line 526 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].admin_stmt); }
#line 3487 "parser.cpp"
    break;

  case 19: /* explainable_statement: create_statement  */
#line 528 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3493 "parser.cpp"
    break;

  case 20: /* explainable_statement: drop_statement  */
#line 529 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3499 "parser.cpp"
    break;

  case 21: /* explainable_statement: copy_statement  */
#line 530 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3505 "parser.cpp"
    break;

  case 22: /* explainable_statement: show_statement  */
#line 531 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3511 "parser.cpp"
    break;

  case 23: /* explainable_statement: select_statement  */
#line 532 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3517 "parser.cpp"
    break;

  case 24: /* explainable_statement: delete_statement  */
#line 533 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3523 "parser.cpp"
    break;

  case 25: /* explainable_statement: update_statement  */
#line 534 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3529 "parser.cpp"
    break;

  case 26: /* explainable_statement: insert_statement  */
#line 535 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3535 "parser.cpp"
    break;

  case 27: /* explainable_statement: flush_statement  */
#line 536 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3541 "parser.cpp"
    break;

  case 28: /* explainable_statement: optimize_statement  */
#line 537 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3547 "parser.cpp"
    break;

  case 29: /* explainable_statement: command_statement  */
#line 538 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3553 "parser.cpp"
    break;

  case 30: /* explainable_statement: compact_statement  */
#line 539 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3559 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
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
#line 3579 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
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
#line 3597 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
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
#line 3630 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
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
#line 3650 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
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
#line 3671 "parser.cpp"
    break;

  case 36: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
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

    create_index_info->index_info_list_ = (yyvsp[0].index_info_list_t);

    if(create_index_info->index_name_.empty()) {
        yyerror(&yyloc, scanner, result, "No index name");
        YYERROR;
    }

    (yyval.create_stmt) = new infinity::CreateStatement();
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3704 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element  */
#line 671 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3713 "parser.cpp"
    break;

  case 38: /* table_element_array: table_element_array ',' table_element  */
#line 675 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3722 "parser.cpp"
    break;

  case 39: /* table_element: table_column  */
#line 681 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3730 "parser.cpp"
    break;

  case 40: /* table_element: table_constraint  */
#line 684 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3738 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type with_index_param_list default_expr  */
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
        case infinity::LogicalType::kTensor:
        case infinity::LogicalType::kTensorArray:
        case infinity::LogicalType::kEmbedding: {
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
#line 3793 "parser.cpp"
    break;

  case 42: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 741 "parser.y"
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
        case infinity::LogicalType::kEmbedding: {
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
#line 3832 "parser.cpp"
    break;

  case 43: /* column_type: BOOLEAN  */
#line 777 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3838 "parser.cpp"
    break;

  case 44: /* column_type: TINYINT  */
#line 778 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3844 "parser.cpp"
    break;

  case 45: /* column_type: SMALLINT  */
#line 779 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3850 "parser.cpp"
    break;

  case 46: /* column_type: INTEGER  */
#line 780 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3856 "parser.cpp"
    break;

  case 47: /* column_type: INT  */
#line 781 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3862 "parser.cpp"
    break;

  case 48: /* column_type: BIGINT  */
#line 782 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3868 "parser.cpp"
    break;

  case 49: /* column_type: HUGEINT  */
#line 783 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3874 "parser.cpp"
    break;

  case 50: /* column_type: FLOAT  */
#line 784 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3880 "parser.cpp"
    break;

  case 51: /* column_type: REAL  */
#line 785 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3886 "parser.cpp"
    break;

  case 52: /* column_type: DOUBLE  */
#line 786 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3892 "parser.cpp"
    break;

  case 53: /* column_type: FLOAT16  */
#line 787 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3898 "parser.cpp"
    break;

  case 54: /* column_type: BFLOAT16  */
#line 788 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBFloat16, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3904 "parser.cpp"
    break;

  case 55: /* column_type: DATE  */
#line 789 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3910 "parser.cpp"
    break;

  case 56: /* column_type: TIME  */
#line 790 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3916 "parser.cpp"
    break;

  case 57: /* column_type: DATETIME  */
#line 791 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3922 "parser.cpp"
    break;

  case 58: /* column_type: TIMESTAMP  */
#line 792 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3928 "parser.cpp"
    break;

  case 59: /* column_type: UUID  */
#line 793 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3934 "parser.cpp"
    break;

  case 60: /* column_type: POINT  */
#line 794 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3940 "parser.cpp"
    break;

  case 61: /* column_type: LINE  */
#line 795 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3946 "parser.cpp"
    break;

  case 62: /* column_type: LSEG  */
#line 796 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3952 "parser.cpp"
    break;

  case 63: /* column_type: BOX  */
#line 797 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3958 "parser.cpp"
    break;

  case 64: /* column_type: CIRCLE  */
#line 800 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3964 "parser.cpp"
    break;

  case 65: /* column_type: VARCHAR  */
#line 802 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3970 "parser.cpp"
    break;

  case 66: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 803 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3976 "parser.cpp"
    break;

  case 67: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 804 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3982 "parser.cpp"
    break;

  case 68: /* column_type: DECIMAL  */
#line 805 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3988 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 808 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3994 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 809 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4000 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 810 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4006 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 811 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4012 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 812 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4018 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 813 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4024 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 814 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4030 "parser.cpp"
    break;

  case 76: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 815 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4036 "parser.cpp"
    break;

  case 77: /* column_type: EMBEDDING '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 816 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4042 "parser.cpp"
    break;

  case 78: /* column_type: EMBEDDING '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 817 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4048 "parser.cpp"
    break;

  case 79: /* column_type: EMBEDDING '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 818 "parser.y"
                                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4054 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 819 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4060 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 820 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4066 "parser.cpp"
    break;

  case 82: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 821 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4072 "parser.cpp"
    break;

  case 83: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 822 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4078 "parser.cpp"
    break;

  case 84: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 823 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4084 "parser.cpp"
    break;

  case 85: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 824 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4090 "parser.cpp"
    break;

  case 86: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 825 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4096 "parser.cpp"
    break;

  case 87: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 826 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4102 "parser.cpp"
    break;

  case 88: /* column_type: TENSORARRAY '(' BIT ',' LONG_VALUE ')'  */
#line 827 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4108 "parser.cpp"
    break;

  case 89: /* column_type: TENSORARRAY '(' TINYINT ',' LONG_VALUE ')'  */
#line 828 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4114 "parser.cpp"
    break;

  case 90: /* column_type: TENSORARRAY '(' SMALLINT ',' LONG_VALUE ')'  */
#line 829 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4120 "parser.cpp"
    break;

  case 91: /* column_type: TENSORARRAY '(' INTEGER ',' LONG_VALUE ')'  */
#line 830 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4126 "parser.cpp"
    break;

  case 92: /* column_type: TENSORARRAY '(' INT ',' LONG_VALUE ')'  */
#line 831 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4132 "parser.cpp"
    break;

  case 93: /* column_type: TENSORARRAY '(' BIGINT ',' LONG_VALUE ')'  */
#line 832 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4138 "parser.cpp"
    break;

  case 94: /* column_type: TENSORARRAY '(' FLOAT ',' LONG_VALUE ')'  */
#line 833 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4144 "parser.cpp"
    break;

  case 95: /* column_type: TENSORARRAY '(' DOUBLE ',' LONG_VALUE ')'  */
#line 834 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4150 "parser.cpp"
    break;

  case 96: /* column_type: TENSORARRAY '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 835 "parser.y"
                                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4156 "parser.cpp"
    break;

  case 97: /* column_type: TENSORARRAY '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 836 "parser.y"
                                              { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4162 "parser.cpp"
    break;

  case 98: /* column_type: TENSORARRAY '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 837 "parser.y"
                                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensorArray, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4168 "parser.cpp"
    break;

  case 99: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 838 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4174 "parser.cpp"
    break;

  case 100: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 839 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4180 "parser.cpp"
    break;

  case 101: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 840 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4186 "parser.cpp"
    break;

  case 102: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 841 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4192 "parser.cpp"
    break;

  case 103: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 842 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4198 "parser.cpp"
    break;

  case 104: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 843 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4204 "parser.cpp"
    break;

  case 105: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 844 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4210 "parser.cpp"
    break;

  case 106: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 845 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4216 "parser.cpp"
    break;

  case 107: /* column_type: VECTOR '(' FLOAT16 ',' LONG_VALUE ')'  */
#line 846 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat16}; }
#line 4222 "parser.cpp"
    break;

  case 108: /* column_type: VECTOR '(' BFLOAT16 ',' LONG_VALUE ')'  */
#line 847 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBFloat16}; }
#line 4228 "parser.cpp"
    break;

  case 109: /* column_type: VECTOR '(' UNSIGNED TINYINT ',' LONG_VALUE ')'  */
#line 848 "parser.y"
                                                 { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemUInt8}; }
#line 4234 "parser.cpp"
    break;

  case 110: /* column_type: SPARSE '(' BIT ',' LONG_VALUE ')'  */
#line 849 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 4240 "parser.cpp"
    break;

  case 111: /* column_type: SPARSE '(' TINYINT ',' LONG_VALUE ')'  */
#line 850 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 4246 "parser.cpp"
    break;

  case 112: /* column_type: SPARSE '(' SMALLINT ',' LONG_VALUE ')'  */
#line 851 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 4252 "parser.cpp"
    break;

  case 113: /* column_type: SPARSE '(' INTEGER ',' LONG_VALUE ')'  */
#line 852 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4258 "parser.cpp"
    break;

  case 114: /* column_type: SPARSE '(' INT ',' LONG_VALUE ')'  */
#line 853 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 4264 "parser.cpp"
    break;

  case 115: /* column_type: SPARSE '(' BIGINT ',' LONG_VALUE ')'  */
#line 854 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 4270 "parser.cpp"
    break;

  case 116: /* column_type: SPARSE '(' FLOAT ',' LONG_VALUE ')'  */
#line 855 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 4276 "parser.cpp"
    break;

  case 117: /* column_type: SPARSE '(' DOUBLE ',' LONG_VALUE ')'  */
#line 856 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSparse, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 4282 "parser.cpp"
    break;

  case 118: /* column_constraints: column_constraint  */
#line 875 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 4291 "parser.cpp"
    break;

  case 119: /* column_constraints: column_constraints column_constraint  */
#line 879 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 4305 "parser.cpp"
    break;

  case 120: /* column_constraint: PRIMARY KEY  */
#line 889 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 4313 "parser.cpp"
    break;

  case 121: /* column_constraint: UNIQUE  */
#line 892 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 4321 "parser.cpp"
    break;

  case 122: /* column_constraint: NULLABLE  */
#line 895 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 4329 "parser.cpp"
    break;

  case 123: /* column_constraint: NOT NULLABLE  */
#line 898 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 4337 "parser.cpp"
    break;

  case 124: /* default_expr: DEFAULT constant_expr  */
#line 902 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 4345 "parser.cpp"
    break;

  case 125: /* default_expr: %empty  */
#line 905 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 4353 "parser.cpp"
    break;

  case 126: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 910 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 4363 "parser.cpp"
    break;

  case 127: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 915 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 4373 "parser.cpp"
    break;

  case 128: /* identifier_array: IDENTIFIER  */
#line 922 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4384 "parser.cpp"
    break;

  case 129: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 928 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 4395 "parser.cpp"
    break;

  case 130: /* delete_statement: DELETE FROM table_name where_clause  */
#line 938 "parser.y"
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
#line 4412 "parser.cpp"
    break;

  case 131: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 954 "parser.y"
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
#line 4451 "parser.cpp"
    break;

  case 132: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 988 "parser.y"
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
#line 4468 "parser.cpp"
    break;

  case 133: /* optional_identifier_array: '(' identifier_array ')'  */
#line 1001 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 4476 "parser.cpp"
    break;

  case 134: /* optional_identifier_array: %empty  */
#line 1004 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 4484 "parser.cpp"
    break;

  case 135: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 1011 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 4494 "parser.cpp"
    break;

  case 136: /* explain_type: ANALYZE  */
#line 1017 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4502 "parser.cpp"
    break;

  case 137: /* explain_type: AST  */
#line 1020 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4510 "parser.cpp"
    break;

  case 138: /* explain_type: RAW  */
#line 1023 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4518 "parser.cpp"
    break;

  case 139: /* explain_type: LOGICAL  */
#line 1026 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4526 "parser.cpp"
    break;

  case 140: /* explain_type: PHYSICAL  */
#line 1029 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4534 "parser.cpp"
    break;

  case 141: /* explain_type: PIPELINE  */
#line 1032 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4542 "parser.cpp"
    break;

  case 142: /* explain_type: FRAGMENT  */
#line 1035 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4550 "parser.cpp"
    break;

  case 143: /* explain_type: %empty  */
#line 1038 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4558 "parser.cpp"
    break;

  case 144: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 1045 "parser.y"
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
#line 4575 "parser.cpp"
    break;

  case 145: /* update_expr_array: update_expr  */
#line 1058 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4584 "parser.cpp"
    break;

  case 146: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1062 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4593 "parser.cpp"
    break;

  case 147: /* update_expr: IDENTIFIER '=' expr  */
#line 1067 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4605 "parser.cpp"
    break;

  case 148: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1080 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4621 "parser.cpp"
    break;

  case 149: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1093 "parser.y"
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
#line 4639 "parser.cpp"
    break;

  case 150: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1108 "parser.y"
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
#line 4657 "parser.cpp"
    break;

  case 151: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1123 "parser.y"
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
#line 4675 "parser.cpp"
    break;

  case 152: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1138 "parser.y"
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
#line 4698 "parser.cpp"
    break;

  case 153: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1161 "parser.y"
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
#line 4756 "parser.cpp"
    break;

  case 154: /* copy_statement: COPY table_name '(' expr_array ')' TO file_path WITH '(' copy_option_list ')'  */
#line 1214 "parser.y"
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
#line 4816 "parser.cpp"
    break;

  case 155: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1269 "parser.y"
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
#line 4868 "parser.cpp"
    break;

  case 156: /* select_statement: select_without_paren  */
#line 1320 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4876 "parser.cpp"
    break;

  case 157: /* select_statement: select_with_paren  */
#line 1323 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4884 "parser.cpp"
    break;

  case 158: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1326 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4898 "parser.cpp"
    break;

  case 159: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1335 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4912 "parser.cpp"
    break;

  case 160: /* select_with_paren: '(' select_without_paren ')'  */
#line 1345 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4920 "parser.cpp"
    break;

  case 161: /* select_with_paren: '(' select_with_paren ')'  */
#line 1348 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4928 "parser.cpp"
    break;

  case 162: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1352 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4937 "parser.cpp"
    break;

  case 163: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1357 "parser.y"
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
#line 4963 "parser.cpp"
    break;

  case 164: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1379 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4971 "parser.cpp"
    break;

  case 165: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1382 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4979 "parser.cpp"
    break;

  case 166: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1387 "parser.y"
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
#line 4999 "parser.cpp"
    break;

  case 167: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1403 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 5007 "parser.cpp"
    break;

  case 168: /* order_by_clause: %empty  */
#line 1406 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 5015 "parser.cpp"
    break;

  case 169: /* order_by_expr_list: order_by_expr  */
#line 1410 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 5024 "parser.cpp"
    break;

  case 170: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1414 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 5033 "parser.cpp"
    break;

  case 171: /* order_by_expr: expr order_by_type  */
#line 1419 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 5043 "parser.cpp"
    break;

  case 172: /* order_by_type: ASC  */
#line 1425 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5051 "parser.cpp"
    break;

  case 173: /* order_by_type: DESC  */
#line 1428 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 5059 "parser.cpp"
    break;

  case 174: /* order_by_type: %empty  */
#line 1431 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 5067 "parser.cpp"
    break;

  case 175: /* limit_expr: LIMIT expr  */
#line 1435 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5075 "parser.cpp"
    break;

  case 176: /* limit_expr: %empty  */
#line 1439 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5081 "parser.cpp"
    break;

  case 177: /* offset_expr: OFFSET expr  */
#line 1441 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5089 "parser.cpp"
    break;

  case 178: /* offset_expr: %empty  */
#line 1445 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 5095 "parser.cpp"
    break;

  case 179: /* distinct: DISTINCT  */
#line 1447 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 5103 "parser.cpp"
    break;

  case 180: /* distinct: %empty  */
#line 1450 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 5111 "parser.cpp"
    break;

  case 181: /* from_clause: FROM table_reference  */
#line 1454 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5119 "parser.cpp"
    break;

  case 182: /* from_clause: %empty  */
#line 1457 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 5127 "parser.cpp"
    break;

  case 183: /* search_clause: SEARCH sub_search_array  */
#line 1461 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 5137 "parser.cpp"
    break;

  case 184: /* search_clause: %empty  */
#line 1466 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 5145 "parser.cpp"
    break;

  case 185: /* where_clause: WHERE expr  */
#line 1470 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5153 "parser.cpp"
    break;

  case 186: /* where_clause: %empty  */
#line 1473 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5161 "parser.cpp"
    break;

  case 187: /* having_clause: HAVING expr  */
#line 1477 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5169 "parser.cpp"
    break;

  case 188: /* having_clause: %empty  */
#line 1480 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 5177 "parser.cpp"
    break;

  case 189: /* group_by_clause: GROUP BY expr_array  */
#line 1484 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 5185 "parser.cpp"
    break;

  case 190: /* group_by_clause: %empty  */
#line 1487 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 5193 "parser.cpp"
    break;

  case 191: /* set_operator: UNION  */
#line 1491 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 5201 "parser.cpp"
    break;

  case 192: /* set_operator: UNION ALL  */
#line 1494 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 5209 "parser.cpp"
    break;

  case 193: /* set_operator: INTERSECT  */
#line 1497 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 5217 "parser.cpp"
    break;

  case 194: /* set_operator: EXCEPT  */
#line 1500 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 5225 "parser.cpp"
    break;

  case 195: /* table_reference: table_reference_unit  */
#line 1508 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 5233 "parser.cpp"
    break;

  case 196: /* table_reference: table_reference ',' table_reference_unit  */
#line 1511 "parser.y"
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
#line 5251 "parser.cpp"
    break;

  case 199: /* table_reference_name: table_name table_alias  */
#line 1528 "parser.y"
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
#line 5269 "parser.cpp"
    break;

  case 200: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1542 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 5280 "parser.cpp"
    break;

  case 201: /* table_name: IDENTIFIER  */
#line 1551 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5290 "parser.cpp"
    break;

  case 202: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1556 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 5302 "parser.cpp"
    break;

  case 203: /* table_alias: AS IDENTIFIER  */
#line 1565 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5312 "parser.cpp"
    break;

  case 204: /* table_alias: IDENTIFIER  */
#line 1570 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 5322 "parser.cpp"
    break;

  case 205: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1575 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 5333 "parser.cpp"
    break;

  case 206: /* table_alias: %empty  */
#line 1581 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 5341 "parser.cpp"
    break;

  case 207: /* with_clause: WITH with_expr_list  */
#line 1588 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 5349 "parser.cpp"
    break;

  case 208: /* with_clause: %empty  */
#line 1591 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 5357 "parser.cpp"
    break;

  case 209: /* with_expr_list: with_expr  */
#line 1595 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 5366 "parser.cpp"
    break;

  case 210: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1598 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 5375 "parser.cpp"
    break;

  case 211: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1603 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 5387 "parser.cpp"
    break;

  case 212: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1615 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 5399 "parser.cpp"
    break;

  case 213: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1622 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 5412 "parser.cpp"
    break;

  case 214: /* join_type: INNER  */
#line 1636 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 5420 "parser.cpp"
    break;

  case 215: /* join_type: LEFT  */
#line 1639 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 5428 "parser.cpp"
    break;

  case 216: /* join_type: RIGHT  */
#line 1642 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 5436 "parser.cpp"
    break;

  case 217: /* join_type: OUTER  */
#line 1645 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5444 "parser.cpp"
    break;

  case 218: /* join_type: FULL  */
#line 1648 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 5452 "parser.cpp"
    break;

  case 219: /* join_type: CROSS  */
#line 1651 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 5460 "parser.cpp"
    break;

  case 220: /* join_type: %empty  */
#line 1654 "parser.y"
                {
}
#line 5467 "parser.cpp"
    break;

  case 221: /* show_statement: SHOW DATABASES  */
#line 1660 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 5476 "parser.cpp"
    break;

  case 222: /* show_statement: SHOW TABLES  */
#line 1664 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 5485 "parser.cpp"
    break;

  case 223: /* show_statement: SHOW VIEWS  */
#line 1668 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 5494 "parser.cpp"
    break;

  case 224: /* show_statement: SHOW CONFIGS  */
#line 1672 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 5503 "parser.cpp"
    break;

  case 225: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1676 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5515 "parser.cpp"
    break;

  case 226: /* show_statement: SHOW PROFILES  */
#line 1683 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 5524 "parser.cpp"
    break;

  case 227: /* show_statement: SHOW BUFFER  */
#line 1687 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kBuffer;
}
#line 5533 "parser.cpp"
    break;

  case 228: /* show_statement: SHOW MEMINDEX  */
#line 1691 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kMemIndex;
}
#line 5542 "parser.cpp"
    break;

  case 229: /* show_statement: SHOW QUERIES  */
#line 1695 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQueries;
}
#line 5551 "parser.cpp"
    break;

  case 230: /* show_statement: SHOW QUERY SESSION LONG_VALUE  */
#line 1699 "parser.y"
                                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kQuery;
    (yyval.show_stmt)->session_id_ = (yyvsp[0].long_value);
}
#line 5561 "parser.cpp"
    break;

  case 231: /* show_statement: SHOW TRANSACTIONS  */
#line 1704 "parser.y"
                    {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransactions;
}
#line 5570 "parser.cpp"
    break;

  case 232: /* show_statement: SHOW TRANSACTION LONG_VALUE  */
#line 1708 "parser.y"
                              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTransaction;
    (yyval.show_stmt)->txn_id_ = (yyvsp[0].long_value);
}
#line 5580 "parser.cpp"
    break;

  case 233: /* show_statement: SHOW SESSION VARIABLES  */
#line 1713 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 5589 "parser.cpp"
    break;

  case 234: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1717 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 5598 "parser.cpp"
    break;

  case 235: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1721 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5609 "parser.cpp"
    break;

  case 236: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1727 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5620 "parser.cpp"
    break;

  case 237: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1733 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5631 "parser.cpp"
    break;

  case 238: /* show_statement: SHOW TABLE table_name  */
#line 1739 "parser.y"
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
#line 5647 "parser.cpp"
    break;

  case 239: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1750 "parser.y"
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
#line 5663 "parser.cpp"
    break;

  case 240: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1761 "parser.y"
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
#line 5679 "parser.cpp"
    break;

  case 241: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1772 "parser.y"
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
#line 5696 "parser.cpp"
    break;

  case 242: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1784 "parser.y"
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
#line 5713 "parser.cpp"
    break;

  case 243: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1796 "parser.y"
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
#line 5731 "parser.cpp"
    break;

  case 244: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1809 "parser.y"
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
#line 5750 "parser.cpp"
    break;

  case 245: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1823 "parser.y"
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
#line 5766 "parser.cpp"
    break;

  case 246: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1834 "parser.y"
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
#line 5785 "parser.cpp"
    break;

  case 247: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE  */
#line 1848 "parser.y"
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
#line 5806 "parser.cpp"
    break;

  case 248: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER SEGMENT LONG_VALUE CHUNK LONG_VALUE  */
#line 1864 "parser.y"
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
#line 5828 "parser.cpp"
    break;

  case 249: /* show_statement: SHOW LOGS  */
#line 1881 "parser.y"
            {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kLogs;
}
#line 5837 "parser.cpp"
    break;

  case 250: /* show_statement: SHOW DELTA LOGS  */
#line 1885 "parser.y"
                  {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDeltaLogs;
}
#line 5846 "parser.cpp"
    break;

  case 251: /* show_statement: SHOW CATALOGS  */
#line 1889 "parser.y"
                {
      (yyval.show_stmt) = new infinity::ShowStatement();
      (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kCatalogs;
}
#line 5855 "parser.cpp"
    break;

  case 252: /* flush_statement: FLUSH DATA  */
#line 1897 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5864 "parser.cpp"
    break;

  case 253: /* flush_statement: FLUSH LOG  */
#line 1901 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5873 "parser.cpp"
    break;

  case 254: /* flush_statement: FLUSH BUFFER  */
#line 1905 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5882 "parser.cpp"
    break;

  case 255: /* optimize_statement: OPTIMIZE table_name  */
#line 1913 "parser.y"
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
#line 5897 "parser.cpp"
    break;

  case 256: /* optimize_statement: OPTIMIZE IDENTIFIER ON table_name with_index_param_list  */
#line 1924 "parser.y"
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
#line 5921 "parser.cpp"
    break;

  case 257: /* command_statement: USE IDENTIFIER  */
#line 1947 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5932 "parser.cpp"
    break;

  case 258: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1953 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5942 "parser.cpp"
    break;

  case 259: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1958 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5953 "parser.cpp"
    break;

  case 260: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1964 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5964 "parser.cpp"
    break;

  case 261: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1970 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5977 "parser.cpp"
    break;

  case 262: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1978 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5988 "parser.cpp"
    break;

  case 263: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1984 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5999 "parser.cpp"
    break;

  case 264: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1990 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6010 "parser.cpp"
    break;

  case 265: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1996 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6021 "parser.cpp"
    break;

  case 266: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 2002 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6034 "parser.cpp"
    break;

  case 267: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 2010 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6045 "parser.cpp"
    break;

  case 268: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 2016 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6056 "parser.cpp"
    break;

  case 269: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 2022 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 6067 "parser.cpp"
    break;

  case 270: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 2028 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 6078 "parser.cpp"
    break;

  case 271: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 2034 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 6091 "parser.cpp"
    break;

  case 272: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 2042 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 6102 "parser.cpp"
    break;

  case 273: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 2048 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 6113 "parser.cpp"
    break;

  case 274: /* compact_statement: COMPACT TABLE table_name  */
#line 2055 "parser.y"
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
#line 6130 "parser.cpp"
    break;

  case 275: /* admin_statement: ADMIN SHOW CATALOGS  */
#line 2068 "parser.y"
                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListCatalogs;
}
#line 6139 "parser.cpp"
    break;

  case 276: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE  */
#line 2072 "parser.y"
                                {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowCatalog;
     (yyval.admin_stmt)->catalog_file_index_ = (yyvsp[0].long_value);
}
#line 6149 "parser.cpp"
    break;

  case 277: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASES  */
#line 2077 "parser.y"
                                                     {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListDatabases;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-1].long_value);
}
#line 6160 "parser.cpp"
    break;

  case 278: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE  */
#line 2083 "parser.y"
                                                               {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowDatabase;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[0].long_value);
}
#line 6172 "parser.cpp"
    break;

  case 279: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLES  */
#line 2090 "parser.y"
                                                                                 {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListTables;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-4].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-1].long_value);
}
#line 6185 "parser.cpp"
    break;

  case 280: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE  */
#line 2098 "parser.y"
                                                                                           {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowTable;
     (yyval.admin_stmt)->catalog_file_start_index_ = (yyvsp[-6].long_value);
     (yyval.admin_stmt)->catalog_file_end_index_ = (yyvsp[-5].long_value);
     (yyval.admin_stmt)->database_meta_index_ = (yyvsp[-3].long_value);
     (yyval.admin_stmt)->database_entry_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->table_meta_index_ = (yyvsp[0].long_value);
}
#line 6199 "parser.cpp"
    break;

  case 281: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE COLUMNS  */
#line 2107 "parser.y"
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
#line 6214 "parser.cpp"
    break;

  case 282: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2117 "parser.y"
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
#line 6229 "parser.cpp"
    break;

  case 283: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2127 "parser.y"
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
#line 6245 "parser.cpp"
    break;

  case 284: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCKS  */
#line 2138 "parser.y"
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
#line 6261 "parser.cpp"
    break;

  case 285: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 2149 "parser.y"
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
#line 6278 "parser.cpp"
    break;

  case 286: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMNS  */
#line 2161 "parser.y"
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
#line 6295 "parser.cpp"
    break;

  case 287: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEXES  */
#line 2173 "parser.y"
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
#line 6310 "parser.cpp"
    break;

  case 288: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE  */
#line 2183 "parser.y"
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
#line 6326 "parser.cpp"
    break;

  case 289: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENTS  */
#line 2194 "parser.y"
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
#line 6343 "parser.cpp"
    break;

  case 290: /* admin_statement: ADMIN SHOW CATALOG LONG_VALUE LONG_VALUE DATABASE LONG_VALUE LONG_VALUE TABLE LONG_VALUE LONG_VALUE INDEX LONG_VALUE LONG_VALUE SEGMENT LONG_VALUE  */
#line 2206 "parser.y"
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
#line 6361 "parser.cpp"
    break;

  case 291: /* admin_statement: ADMIN SHOW LOGS  */
#line 2219 "parser.y"
                  {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogFiles;
}
#line 6370 "parser.cpp"
    break;

  case 292: /* admin_statement: ADMIN SHOW LOG LONG_VALUE  */
#line 2223 "parser.y"
                            {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogFile;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[0].long_value);
}
#line 6380 "parser.cpp"
    break;

  case 293: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEXES  */
#line 2228 "parser.y"
                                    {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kListLogIndexes;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-1].long_value);
}
#line 6390 "parser.cpp"
    break;

  case 294: /* admin_statement: ADMIN SHOW LOG LONG_VALUE INDEX LONG_VALUE  */
#line 2233 "parser.y"
                                             {
     (yyval.admin_stmt) = new infinity::AdminStatement();
     (yyval.admin_stmt)->admin_type_ = infinity::AdminStmtType::kShowLogIndex;
     (yyval.admin_stmt)->log_file_index_ = (yyvsp[-2].long_value);
     (yyval.admin_stmt)->log_index_in_file_ = (yyvsp[0].long_value);
}
#line 6401 "parser.cpp"
    break;

  case 295: /* expr_array: expr_alias  */
#line 2244 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6410 "parser.cpp"
    break;

  case 296: /* expr_array: expr_array ',' expr_alias  */
#line 2248 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6419 "parser.cpp"
    break;

  case 297: /* expr_array_list: '(' expr_array ')'  */
#line 2253 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 6428 "parser.cpp"
    break;

  case 298: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 2257 "parser.y"
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
#line 6448 "parser.cpp"
    break;

  case 299: /* expr_alias: expr AS IDENTIFIER  */
#line 2284 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6459 "parser.cpp"
    break;

  case 300: /* expr_alias: expr  */
#line 2290 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6467 "parser.cpp"
    break;

  case 306: /* operand: '(' expr ')'  */
#line 2300 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 6475 "parser.cpp"
    break;

  case 307: /* operand: '(' select_without_paren ')'  */
#line 2303 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6486 "parser.cpp"
    break;

  case 308: /* operand: constant_expr  */
#line 2309 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 6494 "parser.cpp"
    break;

  case 319: /* extra_match_tensor_option: ',' STRING  */
#line 2323 "parser.y"
                                       {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6502 "parser.cpp"
    break;

  case 320: /* extra_match_tensor_option: %empty  */
#line 2326 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 6510 "parser.cpp"
    break;

  case 321: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' common_array_expr ',' STRING ',' STRING extra_match_tensor_option ')'  */
#line 2332 "parser.y"
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
#line 6531 "parser.cpp"
    break;

  case 322: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2351 "parser.y"
                                                                                                                        {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // vector distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-4].str_value));
    if (!check) {
        goto Error1;
    }

    // vector data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-6].str_value), (yyvsp[-8].const_expr_t));
    if (!check) {
        goto Error1;
    }
    free((yyvsp[-6].str_value));
    free((yyvsp[-4].str_value));
    delete (yyvsp[-8].const_expr_t);

    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return1;
Error1:
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
Return1:
    ;
}
#line 6577 "parser.cpp"
    break;

  case 323: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ')' with_index_param_list  */
#line 2393 "parser.y"
                                                                                     {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // vector search search column
    match_vector_expr->column_expr_ = (yyvsp[-8].expr_t);

    // vector search distance type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    bool check = match_vector_expr->InitDistanceType((yyvsp[-2].str_value));
    if (!check) {
        goto Error2;
    }

    // vector search data type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    check = match_vector_expr->InitEmbedding((yyvsp[-4].str_value), (yyvsp[-6].const_expr_t));
    if (!check) {
        goto Error2;
    }
    free((yyvsp[-4].str_value));
    free((yyvsp[-2].str_value));
    delete (yyvsp[-6].const_expr_t);

    match_vector_expr->topn_ = infinity::DEFAULT_MATCH_VECTOR_TOP_N;
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
    goto Return2;

Error2:
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
Return2:
    ;
}
#line 6624 "parser.cpp"
    break;

  case 324: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2439 "parser.y"
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
#line 6646 "parser.cpp"
    break;

  case 325: /* match_sparse_expr: MATCH SPARSE '(' expr ',' common_sparse_array_expr ',' STRING ')' with_index_param_list  */
#line 2457 "parser.y"
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
#line 6668 "parser.cpp"
    break;

  case 326: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2475 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6681 "parser.cpp"
    break;

  case 327: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2483 "parser.y"
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
#line 6696 "parser.cpp"
    break;

  case 328: /* query_expr: QUERY '(' STRING ')'  */
#line 2494 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6707 "parser.cpp"
    break;

  case 329: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2500 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 6720 "parser.cpp"
    break;

  case 330: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2509 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 6731 "parser.cpp"
    break;

  case 331: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2515 "parser.y"
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
#line 6747 "parser.cpp"
    break;

  case 332: /* sub_search: match_vector_expr  */
#line 2527 "parser.y"
                               {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6755 "parser.cpp"
    break;

  case 333: /* sub_search: match_text_expr  */
#line 2530 "parser.y"
                  {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6763 "parser.cpp"
    break;

  case 334: /* sub_search: match_tensor_expr  */
#line 2533 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6771 "parser.cpp"
    break;

  case 335: /* sub_search: match_sparse_expr  */
#line 2536 "parser.y"
                    {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6779 "parser.cpp"
    break;

  case 336: /* sub_search: query_expr  */
#line 2539 "parser.y"
             {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6787 "parser.cpp"
    break;

  case 337: /* sub_search: fusion_expr  */
#line 2542 "parser.y"
              {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 6795 "parser.cpp"
    break;

  case 338: /* sub_search_array: sub_search  */
#line 2546 "parser.y"
                              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 6804 "parser.cpp"
    break;

  case 339: /* sub_search_array: sub_search_array ',' sub_search  */
#line 2550 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 6813 "parser.cpp"
    break;

  case 340: /* function_expr: IDENTIFIER '(' ')'  */
#line 2555 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 6826 "parser.cpp"
    break;

  case 341: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2563 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6839 "parser.cpp"
    break;

  case 342: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2571 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6853 "parser.cpp"
    break;

  case 343: /* function_expr: operand IS NOT NULLABLE  */
#line 2580 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6865 "parser.cpp"
    break;

  case 344: /* function_expr: operand IS NULLABLE  */
#line 2587 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6877 "parser.cpp"
    break;

  case 345: /* function_expr: NOT operand  */
#line 2594 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6889 "parser.cpp"
    break;

  case 346: /* function_expr: '-' operand  */
#line 2601 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6901 "parser.cpp"
    break;

  case 347: /* function_expr: '+' operand  */
#line 2608 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6913 "parser.cpp"
    break;

  case 348: /* function_expr: operand '-' operand  */
#line 2615 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6926 "parser.cpp"
    break;

  case 349: /* function_expr: operand '+' operand  */
#line 2623 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6939 "parser.cpp"
    break;

  case 350: /* function_expr: operand '*' operand  */
#line 2631 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6952 "parser.cpp"
    break;

  case 351: /* function_expr: operand '/' operand  */
#line 2639 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6965 "parser.cpp"
    break;

  case 352: /* function_expr: operand '%' operand  */
#line 2647 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6978 "parser.cpp"
    break;

  case 353: /* function_expr: operand '=' operand  */
#line 2655 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6991 "parser.cpp"
    break;

  case 354: /* function_expr: operand EQUAL operand  */
#line 2663 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7004 "parser.cpp"
    break;

  case 355: /* function_expr: operand NOT_EQ operand  */
#line 2671 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7017 "parser.cpp"
    break;

  case 356: /* function_expr: operand '<' operand  */
#line 2679 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7030 "parser.cpp"
    break;

  case 357: /* function_expr: operand '>' operand  */
#line 2687 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7043 "parser.cpp"
    break;

  case 358: /* function_expr: operand LESS_EQ operand  */
#line 2695 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7056 "parser.cpp"
    break;

  case 359: /* function_expr: operand GREATER_EQ operand  */
#line 2703 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7069 "parser.cpp"
    break;

  case 360: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2711 "parser.y"
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
#line 7104 "parser.cpp"
    break;

  case 361: /* function_expr: operand LIKE operand  */
#line 2741 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7117 "parser.cpp"
    break;

  case 362: /* function_expr: operand NOT LIKE operand  */
#line 2749 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7130 "parser.cpp"
    break;

  case 363: /* conjunction_expr: expr AND expr  */
#line 2758 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7143 "parser.cpp"
    break;

  case 364: /* conjunction_expr: expr OR expr  */
#line 2766 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 7156 "parser.cpp"
    break;

  case 365: /* between_expr: operand BETWEEN operand AND operand  */
#line 2775 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 7168 "parser.cpp"
    break;

  case 366: /* in_expr: operand IN '(' expr_array ')'  */
#line 2783 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7179 "parser.cpp"
    break;

  case 367: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2789 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 7190 "parser.cpp"
    break;

  case 368: /* case_expr: CASE expr case_check_array END  */
#line 2796 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7201 "parser.cpp"
    break;

  case 369: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2802 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7213 "parser.cpp"
    break;

  case 370: /* case_expr: CASE case_check_array END  */
#line 2809 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 7223 "parser.cpp"
    break;

  case 371: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2814 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 7234 "parser.cpp"
    break;

  case 372: /* case_check_array: WHEN expr THEN expr  */
#line 2821 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 7246 "parser.cpp"
    break;

  case 373: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2828 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 7258 "parser.cpp"
    break;

  case 374: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2836 "parser.y"
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
        case infinity::LogicalType::kEmbedding: {
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
#line 7286 "parser.cpp"
    break;

  case 375: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2860 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7297 "parser.cpp"
    break;

  case 376: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2866 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7308 "parser.cpp"
    break;

  case 377: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2872 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7320 "parser.cpp"
    break;

  case 378: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2879 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 7332 "parser.cpp"
    break;

  case 379: /* column_expr: IDENTIFIER  */
#line 2887 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7344 "parser.cpp"
    break;

  case 380: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2894 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 7356 "parser.cpp"
    break;

  case 381: /* column_expr: '*'  */
#line 2901 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7366 "parser.cpp"
    break;

  case 382: /* column_expr: column_expr '.' '*'  */
#line 2906 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 7380 "parser.cpp"
    break;

  case 383: /* constant_expr: STRING  */
#line 2916 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7390 "parser.cpp"
    break;

  case 384: /* constant_expr: TRUE  */
#line 2921 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 7400 "parser.cpp"
    break;

  case 385: /* constant_expr: FALSE  */
#line 2926 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 7410 "parser.cpp"
    break;

  case 386: /* constant_expr: DOUBLE_VALUE  */
#line 2931 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7420 "parser.cpp"
    break;

  case 387: /* constant_expr: LONG_VALUE  */
#line 2936 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7430 "parser.cpp"
    break;

  case 388: /* constant_expr: DATE STRING  */
#line 2941 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7440 "parser.cpp"
    break;

  case 389: /* constant_expr: TIME STRING  */
#line 2946 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7450 "parser.cpp"
    break;

  case 390: /* constant_expr: DATETIME STRING  */
#line 2951 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7460 "parser.cpp"
    break;

  case 391: /* constant_expr: TIMESTAMP STRING  */
#line 2956 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7470 "parser.cpp"
    break;

  case 392: /* constant_expr: INTERVAL interval_expr  */
#line 2961 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7478 "parser.cpp"
    break;

  case 393: /* constant_expr: interval_expr  */
#line 2964 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7486 "parser.cpp"
    break;

  case 394: /* constant_expr: common_array_expr  */
#line 2967 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7494 "parser.cpp"
    break;

  case 395: /* common_array_expr: array_expr  */
#line 2971 "parser.y"
                              {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7502 "parser.cpp"
    break;

  case 396: /* common_array_expr: subarray_array_expr  */
#line 2974 "parser.y"
                      {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7510 "parser.cpp"
    break;

  case 397: /* common_array_expr: sparse_array_expr  */
#line 2977 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7518 "parser.cpp"
    break;

  case 398: /* common_array_expr: empty_array_expr  */
#line 2980 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7526 "parser.cpp"
    break;

  case 399: /* common_sparse_array_expr: sparse_array_expr  */
#line 2984 "parser.y"
                                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7534 "parser.cpp"
    break;

  case 400: /* common_sparse_array_expr: array_expr  */
#line 2987 "parser.y"
             {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7542 "parser.cpp"
    break;

  case 401: /* common_sparse_array_expr: empty_array_expr  */
#line 2990 "parser.y"
                   {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7550 "parser.cpp"
    break;

  case 402: /* subarray_array_expr: unclosed_subarray_array_expr ']'  */
#line 2994 "parser.y"
                                                      {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7558 "parser.cpp"
    break;

  case 403: /* unclosed_subarray_array_expr: '[' common_array_expr  */
#line 2998 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kSubArrayArray);
    const_expr->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = const_expr;
}
#line 7568 "parser.cpp"
    break;

  case 404: /* unclosed_subarray_array_expr: unclosed_subarray_array_expr ',' common_array_expr  */
#line 3003 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->sub_array_array_.emplace_back((yyvsp[0].const_expr_t));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7577 "parser.cpp"
    break;

  case 405: /* sparse_array_expr: long_sparse_array_expr  */
#line 3008 "parser.y"
                                          {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7585 "parser.cpp"
    break;

  case 406: /* sparse_array_expr: double_sparse_array_expr  */
#line 3011 "parser.y"
                           {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7593 "parser.cpp"
    break;

  case 407: /* long_sparse_array_expr: unclosed_long_sparse_array_expr ']'  */
#line 3015 "parser.y"
                                                            {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7601 "parser.cpp"
    break;

  case 408: /* unclosed_long_sparse_array_expr: '[' int_sparse_ele  */
#line 3019 "parser.y"
                                                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kLongSparseArray);
    const_expr->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    const_expr->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7613 "parser.cpp"
    break;

  case 409: /* unclosed_long_sparse_array_expr: unclosed_long_sparse_array_expr ',' int_sparse_ele  */
#line 3026 "parser.y"
                                                     {
    (yyvsp[-2].const_expr_t)->long_sparse_array_.first.emplace_back((yyvsp[0].int_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->long_sparse_array_.second.emplace_back((yyvsp[0].int_sparse_ele_t)->second);
    delete (yyvsp[0].int_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7624 "parser.cpp"
    break;

  case 410: /* double_sparse_array_expr: unclosed_double_sparse_array_expr ']'  */
#line 3033 "parser.y"
                                                                {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7632 "parser.cpp"
    break;

  case 411: /* unclosed_double_sparse_array_expr: '[' float_sparse_ele  */
#line 3037 "parser.y"
                                                        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleSparseArray);
    const_expr->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    const_expr->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = const_expr;
}
#line 7644 "parser.cpp"
    break;

  case 412: /* unclosed_double_sparse_array_expr: unclosed_double_sparse_array_expr ',' float_sparse_ele  */
#line 3044 "parser.y"
                                                         {
    (yyvsp[-2].const_expr_t)->double_sparse_array_.first.emplace_back((yyvsp[0].float_sparse_ele_t)->first);
    (yyvsp[-2].const_expr_t)->double_sparse_array_.second.emplace_back((yyvsp[0].float_sparse_ele_t)->second);
    delete (yyvsp[0].float_sparse_ele_t);
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7655 "parser.cpp"
    break;

  case 413: /* empty_array_expr: '[' ']'  */
#line 3051 "parser.y"
                          {
    (yyval.const_expr_t) = new infinity::ConstantExpr(infinity::LiteralType::kEmptyArray);
}
#line 7663 "parser.cpp"
    break;

  case 414: /* int_sparse_ele: LONG_VALUE ':' LONG_VALUE  */
#line 3055 "parser.y"
                                          {
    (yyval.int_sparse_ele_t) = new std::pair<int64_t, int64_t>{(yyvsp[-2].long_value), (yyvsp[0].long_value)};
}
#line 7671 "parser.cpp"
    break;

  case 415: /* float_sparse_ele: LONG_VALUE ':' DOUBLE_VALUE  */
#line 3059 "parser.y"
                                              {
    (yyval.float_sparse_ele_t) = new std::pair<int64_t, double>{(yyvsp[-2].long_value), (yyvsp[0].double_value)};
}
#line 7679 "parser.cpp"
    break;

  case 416: /* array_expr: long_array_expr  */
#line 3063 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7687 "parser.cpp"
    break;

  case 417: /* array_expr: double_array_expr  */
#line 3066 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 7695 "parser.cpp"
    break;

  case 418: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 3070 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7703 "parser.cpp"
    break;

  case 419: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 3074 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7713 "parser.cpp"
    break;

  case 420: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 3079 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7722 "parser.cpp"
    break;

  case 421: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 3084 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 7730 "parser.cpp"
    break;

  case 422: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 3088 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 7740 "parser.cpp"
    break;

  case 423: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 3093 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 7749 "parser.cpp"
    break;

  case 424: /* interval_expr: LONG_VALUE SECONDS  */
#line 3098 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7760 "parser.cpp"
    break;

  case 425: /* interval_expr: LONG_VALUE SECOND  */
#line 3104 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7771 "parser.cpp"
    break;

  case 426: /* interval_expr: LONG_VALUE MINUTES  */
#line 3110 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7782 "parser.cpp"
    break;

  case 427: /* interval_expr: LONG_VALUE MINUTE  */
#line 3116 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7793 "parser.cpp"
    break;

  case 428: /* interval_expr: LONG_VALUE HOURS  */
#line 3122 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7804 "parser.cpp"
    break;

  case 429: /* interval_expr: LONG_VALUE HOUR  */
#line 3128 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7815 "parser.cpp"
    break;

  case 430: /* interval_expr: LONG_VALUE DAYS  */
#line 3134 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7826 "parser.cpp"
    break;

  case 431: /* interval_expr: LONG_VALUE DAY  */
#line 3140 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7837 "parser.cpp"
    break;

  case 432: /* interval_expr: LONG_VALUE MONTHS  */
#line 3146 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7848 "parser.cpp"
    break;

  case 433: /* interval_expr: LONG_VALUE MONTH  */
#line 3152 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7859 "parser.cpp"
    break;

  case 434: /* interval_expr: LONG_VALUE YEARS  */
#line 3158 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7870 "parser.cpp"
    break;

  case 435: /* interval_expr: LONG_VALUE YEAR  */
#line 3164 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 7881 "parser.cpp"
    break;

  case 436: /* copy_option_list: copy_option  */
#line 3175 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 7890 "parser.cpp"
    break;

  case 437: /* copy_option_list: copy_option_list ',' copy_option  */
#line 3179 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 7899 "parser.cpp"
    break;

  case 438: /* copy_option: FORMAT IDENTIFIER  */
#line 3184 "parser.y"
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
#line 7934 "parser.cpp"
    break;

  case 439: /* copy_option: DELIMITER STRING  */
#line 3214 "parser.y"
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
#line 7949 "parser.cpp"
    break;

  case 440: /* copy_option: HEADER  */
#line 3224 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 7959 "parser.cpp"
    break;

  case 441: /* copy_option: OFFSET LONG_VALUE  */
#line 3229 "parser.y"
                    {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kOffset;
    (yyval.copy_option_t)->offset_ = (yyvsp[0].long_value);
}
#line 7969 "parser.cpp"
    break;

  case 442: /* copy_option: LIMIT LONG_VALUE  */
#line 3234 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kLimit;
    (yyval.copy_option_t)->limit_ = (yyvsp[0].long_value);
}
#line 7979 "parser.cpp"
    break;

  case 443: /* copy_option: ROWLIMIT LONG_VALUE  */
#line 3239 "parser.y"
                      {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kRowLimit;
    (yyval.copy_option_t)->row_limit_ = (yyvsp[0].long_value);
}
#line 7989 "parser.cpp"
    break;

  case 444: /* file_path: STRING  */
#line 3245 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 7997 "parser.cpp"
    break;

  case 445: /* if_exists: IF EXISTS  */
#line 3249 "parser.y"
                     { (yyval.bool_value) = true; }
#line 8003 "parser.cpp"
    break;

  case 446: /* if_exists: %empty  */
#line 3250 "parser.y"
  { (yyval.bool_value) = false; }
#line 8009 "parser.cpp"
    break;

  case 447: /* if_not_exists: IF NOT EXISTS  */
#line 3252 "parser.y"
                              { (yyval.bool_value) = true; }
#line 8015 "parser.cpp"
    break;

  case 448: /* if_not_exists: %empty  */
#line 3253 "parser.y"
  { (yyval.bool_value) = false; }
#line 8021 "parser.cpp"
    break;

  case 451: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 3268 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8034 "parser.cpp"
    break;

  case 452: /* if_not_exists_info: %empty  */
#line 3276 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 8042 "parser.cpp"
    break;

  case 453: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 3280 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8050 "parser.cpp"
    break;

  case 454: /* with_index_param_list: %empty  */
#line 3283 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 8058 "parser.cpp"
    break;

  case 455: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 3287 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 8066 "parser.cpp"
    break;

  case 456: /* optional_table_properties_list: %empty  */
#line 3290 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 8074 "parser.cpp"
    break;

  case 457: /* index_param_list: index_param  */
#line 3294 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 8083 "parser.cpp"
    break;

  case 458: /* index_param_list: index_param_list ',' index_param  */
#line 3298 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 8092 "parser.cpp"
    break;

  case 459: /* index_param: IDENTIFIER  */
#line 3303 "parser.y"
                         {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8103 "parser.cpp"
    break;

  case 460: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 3309 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 8118 "parser.cpp"
    break;

  case 461: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 3319 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 8130 "parser.cpp"
    break;

  case 462: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 3326 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 8142 "parser.cpp"
    break;

  case 463: /* index_info_list: index_info_list_one_pack  */
#line 3337 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8151 "parser.cpp"
    break;

  case 464: /* index_info_list: index_info_list index_info_list_one_pack  */
#line 3341 "parser.y"
                                           {
    (yyval.index_info_list_t) = (yyvsp[-1].index_info_list_t);
    (yyvsp[-1].index_info_list_t) = nullptr;
    (yyval.index_info_list_t)->insert((yyval.index_info_list_t)->end(), (yyvsp[0].index_info_list_t)->begin(), (yyvsp[0].index_info_list_t)->end());
    delete (yyvsp[0].index_info_list_t);
    (yyvsp[0].index_info_list_t) = nullptr;
}
#line 8163 "parser.cpp"
    break;

  case 465: /* index_info_list_one_pack: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3349 "parser.y"
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
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
        yyerror(&yyloc, scanner, result, "Unknown index type");
        YYERROR;
    }
    free((yyvsp[-1].str_value));

    size_t index_count = (yyvsp[-4].identifier_array_t)->size();
    if(index_count == 0) {
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
    }
    (yyval.index_info_list_t) = new std::vector<infinity::IndexInfo*>();
    (yyval.index_info_list_t)->reserve(index_count);

    infinity::IndexInfo* index_info = new infinity::IndexInfo();
    index_info->index_type_ = index_type;
    index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[0];
    index_info->index_param_list_ = (yyvsp[0].with_index_param_list_t);
    (yyval.index_info_list_t)->emplace_back(index_info);

    for(size_t idx = 1; idx < index_count; ++ idx) {
        infinity::IndexInfo* index_info = new infinity::IndexInfo();
        index_info->index_type_ = index_type;
        index_info->column_name_ = (*(yyvsp[-4].identifier_array_t))[idx];

        size_t param_count = (yyvsp[0].with_index_param_list_t)->size();
        index_info->index_param_list_ = new std::vector<infinity::InitParameter*>();
        index_info->index_param_list_->resize(param_count);
        for(size_t param_idx = 0; param_idx < param_count; ++ param_idx) {
            (*(index_info->index_param_list_))[param_idx] = new infinity::InitParameter();
            *(*(index_info->index_param_list_))[param_idx] = *(*(yyvsp[0].with_index_param_list_t))[param_idx];
        }
        (yyval.index_info_list_t)->emplace_back(index_info);
    }
    delete (yyvsp[-4].identifier_array_t);
}
#line 8222 "parser.cpp"
    break;

  case 466: /* index_info_list_one_pack: '(' identifier_array ')'  */
#line 3403 "parser.y"
                           {
    infinity::IndexType index_type = infinity::IndexType::kSecondary;
    size_t index_count = (yyvsp[-1].identifier_array_t)->size();
    (yyval.index_info_list_t) = new std::vector<infinity::IndexInfo*>();
    (yyval.index_info_list_t)->reserve(index_count);
    for(size_t idx = 0; idx < index_count; ++ idx) {
        infinity::IndexInfo* index_info = new infinity::IndexInfo();
        index_info->index_type_ = index_type;
        index_info->column_name_ = (*(yyvsp[-1].identifier_array_t))[idx];
        (yyval.index_info_list_t)->emplace_back(index_info);
    }
    delete (yyvsp[-1].identifier_array_t);
}
#line 8240 "parser.cpp"
    break;


#line 8244 "parser.cpp"

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

#line 3417 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

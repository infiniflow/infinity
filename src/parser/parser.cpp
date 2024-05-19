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
  YYSYMBOL_GROUP = 53,                     /* GROUP  */
  YYSYMBOL_BY = 54,                        /* BY  */
  YYSYMBOL_HAVING = 55,                    /* HAVING  */
  YYSYMBOL_AS = 56,                        /* AS  */
  YYSYMBOL_NATURAL = 57,                   /* NATURAL  */
  YYSYMBOL_JOIN = 58,                      /* JOIN  */
  YYSYMBOL_LEFT = 59,                      /* LEFT  */
  YYSYMBOL_RIGHT = 60,                     /* RIGHT  */
  YYSYMBOL_OUTER = 61,                     /* OUTER  */
  YYSYMBOL_FULL = 62,                      /* FULL  */
  YYSYMBOL_ON = 63,                        /* ON  */
  YYSYMBOL_INNER = 64,                     /* INNER  */
  YYSYMBOL_CROSS = 65,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 66,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 67,                     /* WHERE  */
  YYSYMBOL_ORDER = 68,                     /* ORDER  */
  YYSYMBOL_LIMIT = 69,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 70,                    /* OFFSET  */
  YYSYMBOL_ASC = 71,                       /* ASC  */
  YYSYMBOL_DESC = 72,                      /* DESC  */
  YYSYMBOL_IF = 73,                        /* IF  */
  YYSYMBOL_NOT = 74,                       /* NOT  */
  YYSYMBOL_EXISTS = 75,                    /* EXISTS  */
  YYSYMBOL_IN = 76,                        /* IN  */
  YYSYMBOL_FROM = 77,                      /* FROM  */
  YYSYMBOL_TO = 78,                        /* TO  */
  YYSYMBOL_WITH = 79,                      /* WITH  */
  YYSYMBOL_DELIMITER = 80,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 81,                    /* FORMAT  */
  YYSYMBOL_HEADER = 82,                    /* HEADER  */
  YYSYMBOL_CAST = 83,                      /* CAST  */
  YYSYMBOL_END = 84,                       /* END  */
  YYSYMBOL_CASE = 85,                      /* CASE  */
  YYSYMBOL_ELSE = 86,                      /* ELSE  */
  YYSYMBOL_THEN = 87,                      /* THEN  */
  YYSYMBOL_WHEN = 88,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 89,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 90,                   /* INTEGER  */
  YYSYMBOL_INT = 91,                       /* INT  */
  YYSYMBOL_TINYINT = 92,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 93,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 94,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 95,                   /* HUGEINT  */
  YYSYMBOL_VARCHAR = 96,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 97,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 98,                    /* DOUBLE  */
  YYSYMBOL_REAL = 99,                      /* REAL  */
  YYSYMBOL_DECIMAL = 100,                  /* DECIMAL  */
  YYSYMBOL_DATE = 101,                     /* DATE  */
  YYSYMBOL_TIME = 102,                     /* TIME  */
  YYSYMBOL_DATETIME = 103,                 /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 104,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 105,                     /* UUID  */
  YYSYMBOL_POINT = 106,                    /* POINT  */
  YYSYMBOL_LINE = 107,                     /* LINE  */
  YYSYMBOL_LSEG = 108,                     /* LSEG  */
  YYSYMBOL_BOX = 109,                      /* BOX  */
  YYSYMBOL_PATH = 110,                     /* PATH  */
  YYSYMBOL_POLYGON = 111,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 112,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 113,                     /* BLOB  */
  YYSYMBOL_BITMAP = 114,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 115,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 116,                   /* VECTOR  */
  YYSYMBOL_BIT = 117,                      /* BIT  */
  YYSYMBOL_TENSOR = 118,                   /* TENSOR  */
  YYSYMBOL_TEXT = 119,                     /* TEXT  */
  YYSYMBOL_PRIMARY = 120,                  /* PRIMARY  */
  YYSYMBOL_KEY = 121,                      /* KEY  */
  YYSYMBOL_UNIQUE = 122,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 123,                 /* NULLABLE  */
  YYSYMBOL_IS = 124,                       /* IS  */
  YYSYMBOL_DEFAULT = 125,                  /* DEFAULT  */
  YYSYMBOL_TRUE = 126,                     /* TRUE  */
  YYSYMBOL_FALSE = 127,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 128,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 129,                   /* SECOND  */
  YYSYMBOL_SECONDS = 130,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 131,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 132,                  /* MINUTES  */
  YYSYMBOL_HOUR = 133,                     /* HOUR  */
  YYSYMBOL_HOURS = 134,                    /* HOURS  */
  YYSYMBOL_DAY = 135,                      /* DAY  */
  YYSYMBOL_DAYS = 136,                     /* DAYS  */
  YYSYMBOL_MONTH = 137,                    /* MONTH  */
  YYSYMBOL_MONTHS = 138,                   /* MONTHS  */
  YYSYMBOL_YEAR = 139,                     /* YEAR  */
  YYSYMBOL_YEARS = 140,                    /* YEARS  */
  YYSYMBOL_EQUAL = 141,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 142,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 143,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 144,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 145,                  /* BETWEEN  */
  YYSYMBOL_AND = 146,                      /* AND  */
  YYSYMBOL_OR = 147,                       /* OR  */
  YYSYMBOL_EXTRACT = 148,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 149,                     /* LIKE  */
  YYSYMBOL_DATA = 150,                     /* DATA  */
  YYSYMBOL_LOG = 151,                      /* LOG  */
  YYSYMBOL_BUFFER = 152,                   /* BUFFER  */
  YYSYMBOL_KNN = 153,                      /* KNN  */
  YYSYMBOL_USING = 154,                    /* USING  */
  YYSYMBOL_SESSION = 155,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 156,                   /* GLOBAL  */
  YYSYMBOL_OFF = 157,                      /* OFF  */
  YYSYMBOL_EXPORT = 158,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 159,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 160,                  /* CONFIGS  */
  YYSYMBOL_CONFIG = 161,                   /* CONFIG  */
  YYSYMBOL_PROFILES = 162,                 /* PROFILES  */
  YYSYMBOL_VARIABLES = 163,                /* VARIABLES  */
  YYSYMBOL_VARIABLE = 164,                 /* VARIABLE  */
  YYSYMBOL_SEARCH = 165,                   /* SEARCH  */
  YYSYMBOL_MATCH = 166,                    /* MATCH  */
  YYSYMBOL_MAXSIM = 167,                   /* MAXSIM  */
  YYSYMBOL_QUERY = 168,                    /* QUERY  */
  YYSYMBOL_FUSION = 169,                   /* FUSION  */
  YYSYMBOL_NUMBER = 170,                   /* NUMBER  */
  YYSYMBOL_171_ = 171,                     /* '='  */
  YYSYMBOL_172_ = 172,                     /* '<'  */
  YYSYMBOL_173_ = 173,                     /* '>'  */
  YYSYMBOL_174_ = 174,                     /* '+'  */
  YYSYMBOL_175_ = 175,                     /* '-'  */
  YYSYMBOL_176_ = 176,                     /* '*'  */
  YYSYMBOL_177_ = 177,                     /* '/'  */
  YYSYMBOL_178_ = 178,                     /* '%'  */
  YYSYMBOL_179_ = 179,                     /* '['  */
  YYSYMBOL_180_ = 180,                     /* ']'  */
  YYSYMBOL_181_ = 181,                     /* '('  */
  YYSYMBOL_182_ = 182,                     /* ')'  */
  YYSYMBOL_183_ = 183,                     /* '.'  */
  YYSYMBOL_184_ = 184,                     /* ';'  */
  YYSYMBOL_185_ = 185,                     /* ','  */
  YYSYMBOL_YYACCEPT = 186,                 /* $accept  */
  YYSYMBOL_input_pattern = 187,            /* input_pattern  */
  YYSYMBOL_statement_list = 188,           /* statement_list  */
  YYSYMBOL_statement = 189,                /* statement  */
  YYSYMBOL_explainable_statement = 190,    /* explainable_statement  */
  YYSYMBOL_create_statement = 191,         /* create_statement  */
  YYSYMBOL_table_element_array = 192,      /* table_element_array  */
  YYSYMBOL_table_element = 193,            /* table_element  */
  YYSYMBOL_table_column = 194,             /* table_column  */
  YYSYMBOL_column_type = 195,              /* column_type  */
  YYSYMBOL_column_constraints = 196,       /* column_constraints  */
  YYSYMBOL_column_constraint = 197,        /* column_constraint  */
  YYSYMBOL_default_expr = 198,             /* default_expr  */
  YYSYMBOL_table_constraint = 199,         /* table_constraint  */
  YYSYMBOL_identifier_array = 200,         /* identifier_array  */
  YYSYMBOL_delete_statement = 201,         /* delete_statement  */
  YYSYMBOL_insert_statement = 202,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 203, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 204,        /* explain_statement  */
  YYSYMBOL_explain_type = 205,             /* explain_type  */
  YYSYMBOL_update_statement = 206,         /* update_statement  */
  YYSYMBOL_update_expr_array = 207,        /* update_expr_array  */
  YYSYMBOL_update_expr = 208,              /* update_expr  */
  YYSYMBOL_drop_statement = 209,           /* drop_statement  */
  YYSYMBOL_copy_statement = 210,           /* copy_statement  */
  YYSYMBOL_select_statement = 211,         /* select_statement  */
  YYSYMBOL_select_with_paren = 212,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 213,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 214, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 215, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 216, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 217,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 218,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 219,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 220,            /* order_by_type  */
  YYSYMBOL_limit_expr = 221,               /* limit_expr  */
  YYSYMBOL_offset_expr = 222,              /* offset_expr  */
  YYSYMBOL_distinct = 223,                 /* distinct  */
  YYSYMBOL_from_clause = 224,              /* from_clause  */
  YYSYMBOL_search_clause = 225,            /* search_clause  */
  YYSYMBOL_where_clause = 226,             /* where_clause  */
  YYSYMBOL_having_clause = 227,            /* having_clause  */
  YYSYMBOL_group_by_clause = 228,          /* group_by_clause  */
  YYSYMBOL_set_operator = 229,             /* set_operator  */
  YYSYMBOL_table_reference = 230,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 231,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 232,     /* table_reference_name  */
  YYSYMBOL_table_name = 233,               /* table_name  */
  YYSYMBOL_table_alias = 234,              /* table_alias  */
  YYSYMBOL_with_clause = 235,              /* with_clause  */
  YYSYMBOL_with_expr_list = 236,           /* with_expr_list  */
  YYSYMBOL_with_expr = 237,                /* with_expr  */
  YYSYMBOL_join_clause = 238,              /* join_clause  */
  YYSYMBOL_join_type = 239,                /* join_type  */
  YYSYMBOL_show_statement = 240,           /* show_statement  */
  YYSYMBOL_flush_statement = 241,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 242,       /* optimize_statement  */
  YYSYMBOL_command_statement = 243,        /* command_statement  */
  YYSYMBOL_compact_statement = 244,        /* compact_statement  */
  YYSYMBOL_expr_array = 245,               /* expr_array  */
  YYSYMBOL_expr_array_list = 246,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 247,               /* expr_alias  */
  YYSYMBOL_expr = 248,                     /* expr  */
  YYSYMBOL_operand = 249,                  /* operand  */
  YYSYMBOL_optional_maxsim_option = 250,   /* optional_maxsim_option  */
  YYSYMBOL_match_tensor_expr = 251,        /* match_tensor_expr  */
  YYSYMBOL_match_vector_expr = 252,        /* match_vector_expr  */
  YYSYMBOL_match_text_expr = 253,          /* match_text_expr  */
  YYSYMBOL_query_expr = 254,               /* query_expr  */
  YYSYMBOL_fusion_expr = 255,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 256,         /* sub_search_array  */
  YYSYMBOL_function_expr = 257,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 258,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 259,             /* between_expr  */
  YYSYMBOL_in_expr = 260,                  /* in_expr  */
  YYSYMBOL_case_expr = 261,                /* case_expr  */
  YYSYMBOL_case_check_array = 262,         /* case_check_array  */
  YYSYMBOL_cast_expr = 263,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 264,            /* subquery_expr  */
  YYSYMBOL_column_expr = 265,              /* column_expr  */
  YYSYMBOL_constant_expr = 266,            /* constant_expr  */
  YYSYMBOL_array_expr = 267,               /* array_expr  */
  YYSYMBOL_long_array_expr = 268,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 269, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 270,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 271, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 272,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 273,         /* copy_option_list  */
  YYSYMBOL_copy_option = 274,              /* copy_option  */
  YYSYMBOL_file_path = 275,                /* file_path  */
  YYSYMBOL_if_exists = 276,                /* if_exists  */
  YYSYMBOL_if_not_exists = 277,            /* if_not_exists  */
  YYSYMBOL_semicolon = 278,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 279,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 280,    /* with_index_param_list  */
  YYSYMBOL_optional_table_properties_list = 281, /* optional_table_properties_list  */
  YYSYMBOL_index_param_list = 282,         /* index_param_list  */
  YYSYMBOL_index_param = 283,              /* index_param  */
  YYSYMBOL_index_info_list = 284           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 96 "parser.y"

// Avoid warnings with the error counter.
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#line 413 "parser.cpp"

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
#define YYFINAL  84
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   955

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  186
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  99
/* YYNRULES -- Number of rules.  */
#define YYNRULES  379
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  757

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   425


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
       2,     2,     2,     2,     2,     2,     2,   178,     2,     2,
     181,   182,   176,   174,   185,   175,   183,   177,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   184,
     172,   171,   173,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   179,     2,   180,     2,     2,     2,     2,     2,     2,
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
     165,   166,   167,   168,   169,   170
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   476,   476,   480,   486,   493,   494,   495,   496,   497,
     498,   499,   500,   501,   502,   503,   504,   505,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     525,   542,   558,   587,   603,   621,   650,   654,   660,   663,
     669,   707,   743,   744,   745,   746,   747,   748,   749,   750,
     751,   752,   753,   754,   755,   756,   757,   758,   759,   760,
     761,   764,   766,   767,   768,   769,   772,   773,   774,   775,
     776,   777,   778,   779,   780,   781,   782,   783,   784,   785,
     786,   787,   788,   789,   790,   791,   792,   793,   794,   795,
     814,   818,   828,   831,   834,   837,   841,   844,   849,   854,
     861,   867,   877,   893,   927,   940,   943,   950,   956,   959,
     962,   965,   968,   971,   974,   977,   984,   997,  1001,  1006,
    1019,  1032,  1047,  1062,  1077,  1100,  1141,  1186,  1189,  1192,
    1201,  1211,  1214,  1218,  1223,  1245,  1248,  1253,  1269,  1272,
    1276,  1280,  1285,  1291,  1294,  1297,  1301,  1305,  1307,  1311,
    1313,  1316,  1320,  1323,  1327,  1332,  1336,  1339,  1343,  1346,
    1350,  1353,  1357,  1360,  1363,  1366,  1374,  1377,  1392,  1392,
    1394,  1408,  1417,  1422,  1431,  1436,  1441,  1447,  1454,  1457,
    1461,  1464,  1469,  1481,  1488,  1502,  1505,  1508,  1511,  1514,
    1517,  1520,  1526,  1530,  1534,  1538,  1542,  1549,  1553,  1557,
    1561,  1567,  1573,  1579,  1590,  1601,  1612,  1624,  1636,  1649,
    1663,  1674,  1692,  1696,  1700,  1708,  1722,  1728,  1733,  1739,
    1745,  1753,  1759,  1765,  1771,  1777,  1785,  1791,  1797,  1803,
    1809,  1817,  1823,  1830,  1847,  1851,  1856,  1860,  1887,  1893,
    1897,  1898,  1899,  1900,  1901,  1903,  1906,  1912,  1915,  1916,
    1917,  1918,  1919,  1920,  1921,  1922,  1923,  1925,  1928,  1934,
    2060,  2226,  2234,  2245,  2251,  2260,  2266,  2276,  2280,  2284,
    2288,  2292,  2296,  2300,  2304,  2308,  2312,  2317,  2325,  2333,
    2342,  2349,  2356,  2363,  2370,  2377,  2385,  2393,  2401,  2409,
    2417,  2425,  2433,  2441,  2449,  2457,  2465,  2473,  2503,  2511,
    2520,  2528,  2537,  2545,  2551,  2558,  2564,  2571,  2576,  2583,
    2590,  2598,  2622,  2628,  2634,  2641,  2649,  2656,  2663,  2668,
    2678,  2683,  2688,  2693,  2698,  2703,  2708,  2713,  2718,  2723,
    2726,  2729,  2732,  2736,  2739,  2743,  2747,  2752,  2757,  2761,
    2766,  2771,  2777,  2783,  2789,  2795,  2801,  2807,  2813,  2819,
    2825,  2831,  2837,  2848,  2852,  2857,  2879,  2889,  2895,  2899,
    2900,  2902,  2903,  2905,  2906,  2918,  2926,  2930,  2933,  2937,
    2940,  2944,  2948,  2953,  2958,  2966,  2973,  2984,  3032,  3081
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
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM",
  "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID", "POINT",
  "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "TENSOR", "TEXT", "PRIMARY", "KEY",
  "UNIQUE", "NULLABLE", "IS", "DEFAULT", "TRUE", "FALSE", "INTERVAL",
  "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS",
  "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ",
  "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG",
  "BUFFER", "KNN", "USING", "SESSION", "GLOBAL", "OFF", "EXPORT",
  "PROFILE", "CONFIGS", "CONFIG", "PROFILES", "VARIABLES", "VARIABLE",
  "SEARCH", "MATCH", "MAXSIM", "QUERY", "FUSION", "NUMBER", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'",
  "'.'", "';'", "','", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "table_element", "table_column", "column_type",
  "column_constraints", "column_constraint", "default_expr",
  "table_constraint", "identifier_array", "delete_statement",
  "insert_statement", "optional_identifier_array", "explain_statement",
  "explain_type", "update_statement", "update_expr_array", "update_expr",
  "drop_statement", "copy_statement", "select_statement",
  "select_with_paren", "select_without_paren",
  "select_clause_with_modifier", "select_clause_without_modifier_paren",
  "select_clause_without_modifier", "order_by_clause",
  "order_by_expr_list", "order_by_expr", "order_by_type", "limit_expr",
  "offset_expr", "distinct", "from_clause", "search_clause",
  "where_clause", "having_clause", "group_by_clause", "set_operator",
  "table_reference", "table_reference_unit", "table_reference_name",
  "table_name", "table_alias", "with_clause", "with_expr_list",
  "with_expr", "join_clause", "join_type", "show_statement",
  "flush_statement", "optimize_statement", "command_statement",
  "compact_statement", "expr_array", "expr_array_list", "expr_alias",
  "expr", "operand", "optional_maxsim_option", "match_tensor_expr",
  "match_vector_expr", "match_text_expr", "query_expr", "fusion_expr",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "array_expr", "long_array_expr",
  "unclosed_long_array_expr", "double_array_expr",
  "unclosed_double_array_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon",
  "if_not_exists_info", "with_index_param_list",
  "optional_table_properties_list", "index_param_list", "index_param",
  "index_info_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-675)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-367)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     192,   195,    34,   232,    41,    -2,    41,   122,   792,   385,
      55,    -8,   104,    41,   119,   -32,   -53,   136,   -36,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,    39,  -675,  -675,
     158,  -675,  -675,  -675,  -675,  -675,    99,    99,    99,    99,
      -7,    41,   141,   141,   141,   141,   141,    62,   224,    41,
      16,   219,   266,   282,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,   309,   292,    41,  -675,  -675,  -675,   101,   199,  -675,
     294,  -675,    41,  -675,  -675,  -675,  -675,  -675,   252,    91,
    -675,   300,   133,   142,  -675,   171,  -675,   305,  -675,  -675,
      12,   264,  -675,   269,   316,   376,    41,    41,    41,   413,
     355,   162,   351,   434,    41,    41,    41,   443,   454,   463,
     401,   469,   469,    10,    66,    71,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,    39,  -675,  -675,  -675,  -675,  -675,  -675,
     340,  -675,   476,  -675,   478,  -675,  -675,   306,   119,   469,
    -675,  -675,  -675,  -675,    12,  -675,  -675,  -675,   435,   437,
     420,   423,  -675,   -44,  -675,   162,  -675,    41,   505,   -15,
    -675,  -675,  -675,  -675,  -675,   450,  -675,   343,   -50,  -675,
     435,  -675,  -675,   436,   438,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
     513,   515,  -675,  -675,  -675,  -675,  -675,   158,  -675,  -675,
     341,   342,   352,  -675,  -675,   686,   522,   353,   354,   295,
     518,   538,   539,   540,  -675,  -675,   526,   367,    14,   368,
     369,   554,   554,  -675,   350,   403,   -59,  -675,   -35,   568,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,   370,  -675,  -675,  -115,  -675,   -72,  -675,   435,
     435,   482,  -675,   -53,    48,   498,   374,  -675,  -145,   375,
    -675,    41,   435,   463,  -675,   238,   383,   386,  -675,   363,
     384,  -675,  -675,   185,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,   554,   387,   625,   494,
     435,   435,   -43,   143,  -675,  -675,  -675,  -675,   686,  -675,
     566,   393,   394,   395,   576,   577,   217,   217,  -675,  -675,
     409,   -94,    11,   435,   429,   592,   435,   435,   -54,   417,
     -45,   554,   554,   554,   554,   554,   554,   554,   554,   554,
     554,   554,   554,   554,   554,    24,  -675,   593,  -675,   595,
     421,  -675,   181,   238,   435,  -675,    39,   755,   481,   427,
     -76,  -675,  -675,  -675,   -53,   505,   431,  -675,   614,   435,
     428,  -675,   238,  -675,   267,   267,   612,  -675,  -675,   435,
    -675,    50,   494,   470,   439,   -28,   -63,   206,  -675,   435,
     435,   543,   435,    44,   618,    64,   132,  -675,  -675,   -53,
     442,   528,  -675,    30,  -675,  -675,   208,   401,  -675,  -675,
     483,   449,   554,   403,   508,  -675,   665,   665,   276,   276,
     610,   665,   665,   276,   276,   217,   217,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,   435,  -675,  -675,  -675,   238,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
     455,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,   457,   460,   462,   159,   464,   505,   619,    48,    39,
     186,   505,  -675,   190,   465,   647,   649,  -675,   220,  -675,
     228,   603,   242,  -675,   472,  -675,   755,   435,  -675,   435,
     -48,   -57,   554,   -89,  -675,   108,   474,  -675,   656,  -675,
     657,   -13,    11,   613,  -675,  -675,  -675,  -675,  -675,  -675,
     616,  -675,   669,  -675,  -675,  -675,  -675,  -675,  -675,   490,
     623,   403,   665,   495,   243,  -675,   554,  -675,   672,   150,
     248,   402,   556,   562,  -675,  -675,    37,   159,  -675,  -675,
     505,   249,   504,  -675,  -675,   532,   250,  -675,   435,  -675,
    -675,  -675,   267,  -675,   681,  -675,  -675,   507,   238,   -46,
    -675,   435,   491,   514,   514,   690,   523,   524,    30,   528,
      11,    11,   519,   208,   641,   652,   533,   251,  -675,  -675,
     625,   260,   529,   531,   534,   536,   541,   542,   546,   547,
     551,   552,   553,   563,   565,   570,   572,   573,   575,   578,
     579,   580,   585,   586,   587,   588,  -675,  -675,  -675,  -675,
    -675,   262,  -675,   701,   705,   564,   273,  -675,  -675,  -675,
    -675,   238,  -675,   590,  -675,  -675,   591,   274,  -675,  -675,
    -675,  -675,   661,   505,  -675,  -675,  -675,  -675,  -675,   435,
     435,  -675,  -675,  -675,  -675,   719,   741,   756,   771,   772,
     773,   774,   784,   785,   786,   787,   788,   789,   798,   799,
     804,   805,   806,   807,   827,   828,   859,   860,   862,   863,
    -675,   703,   280,  -675,   682,   869,  -675,   871,   872,  -675,
     873,   435,   290,   693,   238,   697,   698,   699,   700,   702,
     704,   706,   707,   708,   709,   710,   711,   712,   713,   714,
     715,   716,   717,   718,   720,   721,   722,   723,   724,   725,
     398,  -675,   701,   727,  -675,   682,   726,   728,   730,   238,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
    -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,  -675,
     701,  -675,   879,   881,  -675,   298,   729,   731,  -675,   895,
     883,   733,   735,  -675,  -675,   682,  -675
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     179,     0,     0,     0,     0,     0,     0,     0,   115,     0,
       0,     0,     0,     0,     0,     0,   179,     0,   364,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   128,   127,
       0,     8,    14,    15,    16,    17,   362,   362,   362,   362,
     362,     0,   360,   360,   360,   360,   360,   172,     0,     0,
       0,     0,     0,     0,   109,   113,   110,   111,   112,   114,
     108,   179,     0,     0,   193,   194,   192,     0,     0,   195,
       0,   197,     0,   212,   213,   214,   216,   215,     0,   178,
     180,     0,     0,     0,     1,   179,     2,   162,   164,   165,
       0,   151,   133,   139,     0,     0,     0,     0,     0,     0,
       0,   106,     0,     0,     0,     0,     0,     0,     0,     0,
     157,     0,     0,     0,     0,     0,   107,    18,    23,    25,
      24,    19,    20,    22,    21,    26,    27,    28,    29,   202,
     203,   198,     0,   199,     0,   196,   233,     0,     0,     0,
     132,   131,     4,   163,     0,   129,   130,   150,     0,     0,
     147,     0,    30,     0,    31,   106,   365,     0,     0,   179,
     359,   120,   122,   121,   123,     0,   173,     0,   157,   117,
       0,   102,   358,     0,     0,   220,   222,   221,   218,   219,
     225,   227,   226,   223,   224,   230,   232,   231,   228,   229,
       0,     0,   205,   204,   210,   200,   201,     0,   181,   217,
       0,     0,   316,   320,   323,   324,     0,     0,     0,     0,
       0,     0,     0,     0,   321,   322,     0,     0,     0,     0,
       0,     0,     0,   318,     0,   179,   153,   234,   239,   240,
     254,   252,   253,   255,   256,   249,   244,   243,   242,   250,
     251,   241,   248,   247,   331,     0,   332,     0,   330,     0,
       0,   149,   361,   179,     0,     0,     0,   100,     0,     0,
     104,     0,     0,     0,   116,   156,     0,     0,   211,   206,
       0,   136,   135,     0,   342,   341,   344,   343,   346,   345,
     348,   347,   350,   349,   352,   351,     0,     0,   282,   179,
       0,     0,     0,     0,   325,   326,   327,   328,     0,   329,
       0,     0,     0,     0,     0,     0,   284,   283,   339,   336,
       0,     0,     0,     0,   155,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   335,     0,   338,     0,
     138,   140,   145,   146,     0,   134,    33,     0,     0,     0,
       0,    36,    38,    39,   179,     0,    35,   105,     0,     0,
     103,   124,   119,   118,     0,     0,     0,   207,   182,     0,
     277,     0,   179,     0,     0,     0,     0,     0,   307,     0,
       0,     0,     0,     0,     0,     0,     0,   246,   245,   179,
     152,   166,   168,   177,   169,   235,     0,   157,   238,   300,
     301,     0,     0,   179,     0,   281,   291,   292,   295,   296,
       0,   298,   290,   293,   294,   286,   285,   287,   288,   289,
     317,   319,   337,   340,     0,   143,   144,   142,   148,    42,
      45,    46,    43,    44,    47,    48,    62,    49,    51,    50,
      65,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,     0,     0,     0,    97,     0,     0,   370,     0,    34,
       0,     0,   101,     0,     0,     0,     0,   357,     0,   353,
       0,   208,     0,   278,     0,   312,     0,     0,   305,     0,
       0,     0,     0,     0,   316,     0,     0,   263,     0,   265,
       0,     0,     0,     0,   186,   187,   188,   189,   185,   190,
       0,   175,     0,   170,   269,   267,   268,   270,   271,   154,
     161,   179,   299,     0,     0,   280,     0,   141,     0,     0,
       0,     0,     0,     0,    93,    94,     0,    97,    90,    40,
       0,     0,     0,    32,    37,   379,     0,   236,     0,   356,
     355,   126,     0,   125,     0,   279,   313,     0,   309,     0,
     308,     0,     0,     0,     0,     0,     0,     0,   177,   167,
       0,     0,   174,     0,     0,   159,     0,     0,   314,   303,
     302,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    95,    92,    96,    91,
      41,     0,    99,     0,     0,     0,     0,   354,   209,   311,
     306,   310,   297,     0,   333,   334,     0,     0,   264,   266,
     171,   183,     0,     0,   274,   272,   273,   275,   276,     0,
       0,   137,   315,   304,    64,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      98,   373,     0,   371,   368,     0,   237,     0,     0,   261,
       0,     0,     0,   160,   158,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   369,     0,     0,   377,   368,     0,     0,     0,   184,
     176,    63,    69,    70,    67,    68,    71,    72,    73,    66,
      85,    86,    83,    84,    87,    88,    89,    82,    77,    78,
      75,    76,    79,    80,    81,    74,   374,   376,   375,   372,
       0,   378,     0,     0,   262,     0,     0,   258,   367,     0,
       0,     0,     0,   257,   259,   368,   260
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -675,  -675,  -675,   824,  -675,   849,  -675,   461,  -675,   444,
    -675,   391,   396,  -675,  -351,   861,   864,   766,  -675,  -675,
     865,  -675,   664,   867,   868,   -58,   908,   -14,   734,   790,
     -42,  -675,  -675,   506,  -675,  -675,  -675,  -675,  -675,  -675,
    -163,  -675,  -675,  -675,  -675,   440,   -49,    19,   377,  -675,
    -675,   795,  -675,  -675,   875,   876,   877,   878,   880,  -272,
    -675,   627,  -170,  -172,  -675,  -390,  -388,  -386,  -350,  -342,
    -675,  -675,  -675,  -675,  -675,  -675,   650,  -675,  -675,   560,
     418,   392,   -69,  -675,   -51,  -675,   732,   582,   407,   -77,
     212,   265,  -675,  -675,  -674,  -675,   205,   253,  -675
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   116,    20,   350,   351,   352,   454,
     527,   528,   529,   353,   258,    21,    22,   159,    23,    61,
      24,   168,   169,    25,    26,    27,    28,    29,    92,   145,
      93,   150,   340,   341,   427,   251,   345,   148,   314,   397,
     171,   631,   565,    90,   390,   391,   392,   393,   503,    30,
      79,    80,   394,   500,    31,    32,    33,    34,    35,   226,
     360,   227,   228,   229,   751,   230,   231,   232,   233,   234,
     509,   235,   236,   237,   238,   239,   293,   240,   241,   242,
     243,   613,   244,   245,   246,   247,   248,   468,   469,   173,
     103,    95,    86,   100,   704,   533,   662,   663,   356
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     265,   371,    83,   123,   460,   264,   504,    87,   505,    88,
     506,    89,   253,   175,    47,   176,   177,   170,   312,   259,
      91,   315,   401,    48,   477,    50,    14,   420,   476,   404,
     551,   741,    77,   501,   288,   174,   550,   357,   610,   292,
     358,   203,   204,   205,    47,   291,   507,   484,   146,   306,
     307,   347,   316,   317,   508,   311,  -366,   316,   317,    87,
     101,    88,   199,    89,    14,   336,    94,    41,   110,   180,
     337,   181,   182,   178,   185,    49,   186,   187,   405,   342,
     343,   756,   130,   316,   317,    72,   502,   463,   388,   316,
     317,   136,   362,   111,   112,   402,   553,   472,   316,   317,
     316,   317,   201,   316,   317,   531,   457,    76,   338,   458,
     536,   316,   317,   339,   288,   153,   154,   155,   316,   317,
     375,   376,    78,   162,   163,   164,   313,    81,    16,   183,
     301,   514,   302,   303,   188,   263,    84,   254,   210,   211,
     212,   213,    73,    74,    75,   260,   399,   400,    85,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   214,   215,   216,    91,   179,   348,   558,
     349,  -363,    94,   624,   428,   625,   256,   626,     1,   601,
       2,     3,     4,     5,     6,     7,     8,     9,   202,   203,
     204,   205,   389,   144,    10,   346,    11,    12,    13,     1,
     421,     2,     3,     4,     5,     6,     7,     8,     9,   480,
     481,   310,   483,   627,   102,    10,   224,    11,    12,    13,
     223,   628,   113,   184,    36,    37,    38,   378,   189,   379,
     512,   380,   473,   522,   510,   313,    39,    40,   109,   567,
     572,   573,   574,   575,   576,   108,   487,   577,   578,   488,
      14,   369,   425,   426,   342,   104,   105,   106,   107,   206,
     207,    42,    43,    44,   131,   132,   606,   579,   208,   114,
     209,    14,   672,    45,    46,   374,   138,    51,    52,   523,
     361,   524,   525,    53,   526,   115,   210,   211,   212,   213,
     478,   335,   479,   554,   380,   129,   459,   135,   202,   203,
     204,   205,    96,    97,    98,    99,   139,   548,   137,   549,
     552,   214,   215,   216,   489,   140,     1,   490,     2,     3,
       4,     5,     6,     7,   141,     9,   143,   316,   317,    15,
     147,   491,    10,   217,    11,    12,    13,   149,   580,   581,
     582,   583,   584,   158,   570,   585,   586,   465,   466,   467,
      15,   218,    16,   219,   220,   308,   309,   673,   474,   221,
     222,   223,   133,   134,   224,   587,   225,   370,   535,   206,
     207,   358,   537,    16,   218,   313,   219,   220,   208,   152,
     209,   611,   190,   291,   316,   317,   191,   192,    14,   513,
     151,   193,   194,   332,   333,   334,   210,   211,   212,   213,
     320,   736,   541,   737,   738,   542,   202,   203,   204,   205,
     543,   366,   367,   542,    62,    63,   156,    64,   157,  -367,
    -367,   214,   215,   216,   545,   569,   160,   313,   313,    65,
      66,   602,   605,   633,   358,   358,   313,   161,   202,   203,
     204,   205,   634,   217,   660,   635,   165,   358,  -367,  -367,
     330,   331,   332,   333,   334,   666,   669,   166,   313,   670,
     674,   218,   701,   219,   220,   702,   167,    15,   170,   221,
     222,   223,   710,   172,   224,   358,   225,   206,   207,   195,
     748,   196,    14,   702,   614,   614,   208,   197,   209,   250,
      16,   249,   588,   589,   590,   591,   592,   566,   252,   593,
     594,   709,   615,   615,   210,   211,   212,   213,   257,   206,
     207,   621,   622,   261,   262,   266,   268,   267,   208,   595,
     209,   269,   294,   271,   272,   202,   203,   204,   205,   214,
     215,   216,   298,   273,   289,   290,   210,   211,   212,   213,
      67,    68,   295,   296,   297,    69,    70,    71,   300,   304,
     305,   217,   344,   335,   354,   355,   359,   202,   203,   204,
     205,   214,   215,   216,   364,   373,   368,   365,   372,   218,
     381,   219,   220,    14,   382,   383,   384,   221,   222,   223,
     385,   386,   224,   217,   225,   493,  -191,   494,   495,   496,
     497,   387,   498,   499,   396,   398,   286,   287,   403,   422,
     423,   218,   455,   219,   220,   208,   424,   209,   456,   221,
     222,   223,   461,   464,   224,   320,   225,   462,   471,   402,
     482,   475,   486,   210,   211,   212,   213,   492,   286,   316,
     511,   515,   321,   322,   323,   324,   518,   208,   519,   209,
     326,   520,   318,   521,   319,   530,   538,   532,   214,   215,
     216,   539,   540,   544,   546,   210,   211,   212,   213,   555,
     556,   557,   327,   328,   329,   330,   331,   332,   333,   334,
     217,   560,   562,   612,   561,   563,   564,   568,   571,   596,
     214,   215,   216,   597,   373,   603,   604,   608,   218,   609,
     219,   220,   320,   224,   617,   629,   221,   222,   223,   373,
     623,   224,   217,   225,   661,   618,   619,   630,   664,   321,
     322,   323,   324,   325,   636,   632,   637,   326,   665,   638,
     218,   639,   219,   220,   671,   675,   640,   641,   221,   222,
     223,   642,   643,   224,   320,   225,   644,   645,   646,   327,
     328,   329,   330,   331,   332,   333,   334,   676,   647,   320,
     648,   321,   322,   323,   324,   649,   516,   650,   651,   326,
     652,   703,   677,   653,   654,   655,   321,   322,   323,   324,
     656,   657,   658,   659,   326,   667,   668,   678,   679,   680,
     681,   327,   328,   329,   330,   331,   332,   333,   334,   320,
     682,   683,   684,   685,   686,   687,   327,   328,   329,   330,
     331,   332,   333,   334,   688,   689,  -367,  -367,   323,   324,
     690,   691,   692,   693,  -367,   274,   275,   276,   277,   278,
     279,   280,   281,   282,   283,   284,   285,    54,    55,    56,
      57,    58,    59,   694,   695,    60,  -367,   328,   329,   330,
     331,   332,   333,   334,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448,   449,   696,   697,   450,   698,   699,
     451,   452,   705,   453,   700,   706,   707,   708,   313,   711,
     712,   713,   714,   746,   715,   747,   716,   753,   717,   718,
     719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
     729,   752,   730,   731,   732,   733,   734,   735,   740,   142,
     117,   742,   744,   743,   749,   754,   750,   755,   599,   534,
     547,   255,   118,   600,    82,   119,   120,   363,   121,   122,
     517,   270,   559,   198,   200,   620,   124,   125,   126,   127,
     395,   128,   377,   485,   598,   745,   616,   470,   299,   607,
       0,     0,     0,     0,     0,   739
};

static const yytype_int16 yycheck[] =
{
     170,   273,    16,    61,   355,   168,   396,    20,   396,    22,
     396,    24,    56,     3,     3,     5,     6,    67,    77,    34,
       8,    56,    76,     4,    87,     6,    79,     3,    56,    74,
      87,   705,    13,     3,   206,   112,    84,   182,    84,   209,
     185,     4,     5,     6,     3,    88,   396,     3,    90,   221,
     222,     3,   146,   147,   396,   225,    63,   146,   147,    20,
      41,    22,   139,    24,    79,   180,    73,    33,    49,     3,
     185,     5,     6,    63,     3,    77,     5,     6,   123,   249,
     250,   755,    63,   146,   147,    30,    56,   359,   182,   146,
     147,    72,   262,    77,    78,   149,   185,   369,   146,   147,
     146,   147,   144,   146,   147,   456,   182,     3,   180,   185,
     461,   146,   147,   185,   286,    96,    97,    98,   146,   147,
     290,   291,     3,   104,   105,   106,   185,   159,   181,    63,
     116,   403,   118,   119,    63,   185,     0,   181,   101,   102,
     103,   104,   150,   151,   152,   159,   316,   317,   184,   321,
     322,   323,   324,   325,   326,   327,   328,   329,   330,   331,
     332,   333,   334,   126,   127,   128,     8,   157,   120,   182,
     122,     0,    73,   563,   344,   563,   157,   563,     7,   530,
       9,    10,    11,    12,    13,    14,    15,    16,     3,     4,
       5,     6,   181,   181,    23,   253,    25,    26,    27,     7,
     176,     9,    10,    11,    12,    13,    14,    15,    16,   379,
     380,   225,   382,   563,    73,    23,   179,    25,    26,    27,
     176,   563,     3,   157,    29,    30,    31,    84,   157,    86,
     402,    88,   182,    74,   397,   185,    41,    42,    14,   511,
      90,    91,    92,    93,    94,   183,   182,    97,    98,   185,
      79,    66,    71,    72,   424,    43,    44,    45,    46,    74,
      75,    29,    30,    31,   163,   164,   538,   117,    83,     3,
      85,    79,   623,    41,    42,   289,   185,   155,   156,   120,
     261,   122,   123,   161,   125,     3,   101,   102,   103,   104,
      84,   183,    86,   185,    88,     3,   354,     3,     3,     4,
       5,     6,    37,    38,    39,    40,     6,   477,    56,   479,
     482,   126,   127,   128,   182,   182,     7,   185,     9,    10,
      11,    12,    13,    14,   182,    16,    21,   146,   147,   158,
      66,   389,    23,   148,    25,    26,    27,    68,    90,    91,
      92,    93,    94,   181,   516,    97,    98,    80,    81,    82,
     158,   166,   181,   168,   169,     5,     6,   629,   372,   174,
     175,   176,   163,   164,   179,   117,   181,   182,   182,    74,
      75,   185,   182,   181,   166,   185,   168,   169,    83,     3,
      85,   551,    42,    88,   146,   147,    46,    47,    79,   403,
      74,    51,    52,   176,   177,   178,   101,   102,   103,   104,
     124,     3,   182,     5,     6,   185,     3,     4,     5,     6,
     182,    48,    49,   185,    29,    30,     3,    32,    63,   143,
     144,   126,   127,   128,   182,   182,    75,   185,   185,    44,
      45,   182,   182,   182,   185,   185,   185,     3,     3,     4,
       5,     6,   182,   148,   182,   185,     3,   185,   172,   173,
     174,   175,   176,   177,   178,   182,   182,     3,   185,   185,
     630,   166,   182,   168,   169,   185,     3,   158,    67,   174,
     175,   176,   182,     4,   179,   185,   181,    74,    75,     3,
     182,     3,    79,   185,   553,   554,    83,   181,    85,    69,
     181,    54,    90,    91,    92,    93,    94,   511,    75,    97,
      98,   671,   553,   554,   101,   102,   103,   104,     3,    74,
      75,   560,   561,    63,   171,    79,     3,    79,    83,   117,
      85,     6,     4,   182,   182,     3,     4,     5,     6,   126,
     127,   128,     6,   181,   181,   181,   101,   102,   103,   104,
     155,   156,     4,     4,     4,   160,   161,   162,   181,   181,
     181,   148,    70,   183,    56,   181,   181,     3,     4,     5,
       6,   126,   127,   128,   181,    74,   182,   181,   181,   166,
       4,   168,   169,    79,   181,   181,   181,   174,   175,   176,
       4,     4,   179,   148,   181,    57,    58,    59,    60,    61,
      62,   182,    64,    65,   165,     3,    74,    75,   181,     6,
       5,   166,   121,   168,   169,    83,   185,    85,   181,   174,
     175,   176,   181,   185,   179,   124,   181,     3,     6,   149,
      77,   182,     4,   101,   102,   103,   104,   185,    74,   146,
     181,   123,   141,   142,   143,   144,   181,    83,   181,    85,
     149,   181,    74,   181,    76,   181,   181,    28,   126,   127,
     128,     4,     3,    50,   182,   101,   102,   103,   104,   185,
       4,     4,   171,   172,   173,   174,   175,   176,   177,   178,
     148,    58,     3,   182,    58,   185,    53,   182,     6,   123,
     126,   127,   128,   121,    74,   181,   154,     6,   166,   182,
     168,   169,   124,   179,     4,    54,   174,   175,   176,    74,
     181,   179,   148,   181,     3,   182,   182,    55,     3,   141,
     142,   143,   144,   145,   185,   182,   185,   149,   154,   185,
     166,   185,   168,   169,    63,     6,   185,   185,   174,   175,
     176,   185,   185,   179,   124,   181,   185,   185,   185,   171,
     172,   173,   174,   175,   176,   177,   178,     6,   185,   124,
     185,   141,   142,   143,   144,   185,   146,   185,   185,   149,
     185,    79,     6,   185,   185,   185,   141,   142,   143,   144,
     185,   185,   185,   185,   149,   185,   185,     6,     6,     6,
       6,   171,   172,   173,   174,   175,   176,   177,   178,   124,
       6,     6,     6,     6,     6,     6,   171,   172,   173,   174,
     175,   176,   177,   178,     6,     6,   141,   142,   143,   144,
       6,     6,     6,     6,   149,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,    35,    36,    37,
      38,    39,    40,     6,     6,    43,   171,   172,   173,   174,
     175,   176,   177,   178,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,     6,     6,   112,     6,     6,
     115,   116,     3,   118,   171,     4,     4,     4,   185,   182,
     182,   182,   182,     4,   182,     4,   182,     4,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,     6,   182,   182,   182,   182,   182,   182,   181,    85,
      61,   185,   182,   185,   185,   182,   185,   182,   527,   458,
     476,   155,    61,   527,    16,    61,    61,   263,    61,    61,
     424,   197,   492,   138,   144,   558,    61,    61,    61,    61,
     313,    61,   292,   383,   526,   740,   554,   365,   216,   542,
      -1,    -1,    -1,    -1,    -1,   702
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      23,    25,    26,    27,    79,   158,   181,   187,   188,   189,
     191,   201,   202,   204,   206,   209,   210,   211,   212,   213,
     235,   240,   241,   242,   243,   244,    29,    30,    31,    41,
      42,    33,    29,    30,    31,    41,    42,     3,   233,    77,
     233,   155,   156,   161,    35,    36,    37,    38,    39,    40,
      43,   205,    29,    30,    32,    44,    45,   155,   156,   160,
     161,   162,    30,   150,   151,   152,     3,   233,     3,   236,
     237,   159,   212,   213,     0,   184,   278,    20,    22,    24,
     229,     8,   214,   216,    73,   277,   277,   277,   277,   277,
     279,   233,    73,   276,   276,   276,   276,   276,   183,    14,
     233,    77,    78,     3,     3,     3,   190,   191,   201,   202,
     206,   209,   210,   211,   240,   241,   242,   243,   244,     3,
     233,   163,   164,   163,   164,     3,   233,    56,   185,     6,
     182,   182,   189,    21,   181,   215,   216,    66,   223,    68,
     217,    74,     3,   233,   233,   233,     3,    63,   181,   203,
      75,     3,   233,   233,   233,     3,     3,     3,   207,   208,
      67,   226,     4,   275,   275,     3,     5,     6,    63,   157,
       3,     5,     6,    63,   157,     3,     5,     6,    63,   157,
      42,    46,    47,    51,    52,     3,     3,   181,   237,   275,
     215,   216,     3,     4,     5,     6,    74,    75,    83,    85,
     101,   102,   103,   104,   126,   127,   128,   148,   166,   168,
     169,   174,   175,   176,   179,   181,   245,   247,   248,   249,
     251,   252,   253,   254,   255,   257,   258,   259,   260,   261,
     263,   264,   265,   266,   268,   269,   270,   271,   272,    54,
      69,   221,    75,    56,   181,   203,   233,     3,   200,    34,
     213,    63,   171,   185,   226,   248,    79,    79,     3,     6,
     214,   182,   182,   181,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,    74,    75,   249,   181,
     181,    88,   248,   262,     4,     4,     4,     4,     6,   272,
     181,   116,   118,   119,   181,   181,   249,   249,     5,     6,
     213,   248,    77,   185,   224,    56,   146,   147,    74,    76,
     124,   141,   142,   143,   144,   145,   149,   171,   172,   173,
     174,   175,   176,   177,   178,   183,   180,   185,   180,   185,
     218,   219,   248,   248,    70,   222,   211,     3,   120,   122,
     192,   193,   194,   199,    56,   181,   284,   182,   185,   181,
     246,   233,   248,   208,   181,   181,    48,    49,   182,    66,
     182,   245,   181,    74,   213,   248,   248,   262,    84,    86,
      88,     4,   181,   181,   181,     4,     4,   182,   182,   181,
     230,   231,   232,   233,   238,   247,   165,   225,     3,   248,
     248,    76,   149,   181,    74,   123,   249,   249,   249,   249,
     249,   249,   249,   249,   249,   249,   249,   249,   249,   249,
       3,   176,     6,     5,   185,    71,    72,   220,   248,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     112,   115,   116,   118,   195,   121,   181,   182,   185,   211,
     200,   181,     3,   245,   185,    80,    81,    82,   273,   274,
     273,     6,   245,   182,   213,   182,    56,    87,    84,    86,
     248,   248,    77,   248,     3,   265,     4,   182,   185,   182,
     185,   211,   185,    57,    59,    60,    61,    62,    64,    65,
     239,     3,    56,   234,   251,   252,   253,   254,   255,   256,
     226,   181,   249,   213,   245,   123,   146,   219,   181,   181,
     181,   181,    74,   120,   122,   123,   125,   196,   197,   198,
     181,   200,    28,   281,   193,   182,   200,   182,   181,     4,
       3,   182,   185,   182,    50,   182,   182,   195,   248,   248,
      84,    87,   249,   185,   185,   185,     4,     4,   182,   231,
      58,    58,     3,   185,    53,   228,   213,   245,   182,   182,
     249,     6,    90,    91,    92,    93,    94,    97,    98,   117,
      90,    91,    92,    93,    94,    97,    98,   117,    90,    91,
      92,    93,    94,    97,    98,   117,   123,   121,   266,   197,
     198,   200,   182,   181,   154,   182,   245,   274,     6,   182,
      84,   248,   182,   267,   268,   270,   267,     4,   182,   182,
     234,   232,   232,   181,   251,   252,   253,   254,   255,    54,
      55,   227,   182,   182,   182,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     182,     3,   282,   283,     3,   154,   182,   185,   185,   182,
     185,    63,   200,   245,   248,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     171,   182,   185,    79,   280,     3,     4,     4,     4,   248,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,     3,     5,     6,   283,
     181,   280,   185,   185,   182,   282,     4,     4,   182,   185,
     185,   250,     6,     4,   182,   182,   280
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   186,   187,   188,   188,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   190,   190,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     191,   191,   191,   191,   191,   191,   192,   192,   193,   193,
     194,   194,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     196,   196,   197,   197,   197,   197,   198,   198,   199,   199,
     200,   200,   201,   202,   202,   203,   203,   204,   205,   205,
     205,   205,   205,   205,   205,   205,   206,   207,   207,   208,
     209,   209,   209,   209,   209,   210,   210,   211,   211,   211,
     211,   212,   212,   213,   214,   215,   215,   216,   217,   217,
     218,   218,   219,   220,   220,   220,   221,   221,   222,   222,
     223,   223,   224,   224,   225,   225,   226,   226,   227,   227,
     228,   228,   229,   229,   229,   229,   230,   230,   231,   231,
     232,   232,   233,   233,   234,   234,   234,   234,   235,   235,
     236,   236,   237,   238,   238,   239,   239,   239,   239,   239,
     239,   239,   240,   240,   240,   240,   240,   240,   240,   240,
     240,   240,   240,   240,   240,   240,   240,   240,   240,   240,
     240,   240,   241,   241,   241,   242,   243,   243,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   244,   245,   245,   246,   246,   247,   247,
     248,   248,   248,   248,   248,   249,   249,   249,   249,   249,
     249,   249,   249,   249,   249,   249,   249,   250,   250,   251,
     252,   253,   253,   254,   254,   255,   255,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   256,   257,   257,   257,
     257,   257,   257,   257,   257,   257,   257,   257,   257,   257,
     257,   257,   257,   257,   257,   257,   257,   257,   257,   257,
     258,   258,   259,   260,   260,   261,   261,   261,   261,   262,
     262,   263,   264,   264,   264,   264,   265,   265,   265,   265,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   267,   267,   268,   269,   269,   270,   271,
     271,   272,   272,   272,   272,   272,   272,   272,   272,   272,
     272,   272,   272,   273,   273,   274,   274,   274,   275,   276,
     276,   277,   277,   278,   278,   279,   279,   280,   280,   281,
     281,   282,   282,   283,   283,   283,   283,   284,   284,   284
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       4,     4,     8,     6,     7,     6,     1,     3,     1,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     4,     1,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       1,     2,     2,     1,     1,     2,     2,     0,     5,     4,
       1,     3,     4,     6,     5,     3,     0,     3,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     1,     3,     3,
       4,     4,     4,     4,     6,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     8,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     1,     2,     1,     1,     1,     3,     1,     1,
       2,     4,     1,     3,     2,     1,     5,     0,     2,     0,
       1,     3,     5,     4,     6,     1,     1,     1,     1,     1,
       1,     0,     2,     2,     2,     2,     3,     2,     3,     3,
       4,     4,     3,     3,     4,     4,     5,     6,     7,     9,
       4,     5,     2,     2,     2,     2,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     3,     1,     3,     3,     5,     3,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     0,    12,
      14,     7,     9,     4,     6,     4,     6,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     2,     2,     1,     1,     2,
       0,     3,     0,     1,     0,     2,     0,     4,     0,     4,
       0,     1,     3,     1,     3,     3,     3,     6,     7,     3
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
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2072 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2080 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 221 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2094 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 211 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2108 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2119 "parser.cpp"
        break;

    case YYSYMBOL_default_expr: /* default_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2127 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2136 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2145 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 271 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2159 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 264 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2170 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2180 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2190 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2200 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2210 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2220 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 346 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2230 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 254 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2244 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 254 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2258 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 334 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2268 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2276 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2284 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2293 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2301 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2309 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2317 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2331 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2340 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2349 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2358 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2371 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 324 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2380 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 281 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2394 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 281 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2408 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 340 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2418 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2427 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2441 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 241 "parser.y"
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
#line 2458 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2466 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2474 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2482 "parser.cpp"
        break;

    case YYSYMBOL_optional_maxsim_option: /* optional_maxsim_option  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2490 "parser.cpp"
        break;

    case YYSYMBOL_match_tensor_expr: /* match_tensor_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2498 "parser.cpp"
        break;

    case YYSYMBOL_match_vector_expr: /* match_vector_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2506 "parser.cpp"
        break;

    case YYSYMBOL_match_text_expr: /* match_text_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2514 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2522 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2530 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2544 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2552 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2560 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2568 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2576 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2584 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 352 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2597 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2605 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2613 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 316 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2621 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2629 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2637 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2645 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2653 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2661 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2669 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 320 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2677 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 307 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2685 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 204 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2696 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2710 "parser.cpp"
        break;

    case YYSYMBOL_optional_table_properties_list: /* optional_table_properties_list  */
#line 187 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2724 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 177 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 2738 "parser.cpp"
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
#line 86 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2846 "parser.cpp"

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
#line 476 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 3061 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 480 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 3072 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 486 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 3083 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 493 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3089 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 494 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3095 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3101 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 496 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3107 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3113 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3119 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3125 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3131 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3137 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 502 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3143 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 503 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3149 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 504 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3155 "parser.cpp"
    break;

  case 17: /* statement: compact_statement  */
#line 505 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3161 "parser.cpp"
    break;

  case 18: /* explainable_statement: create_statement  */
#line 507 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3167 "parser.cpp"
    break;

  case 19: /* explainable_statement: drop_statement  */
#line 508 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3173 "parser.cpp"
    break;

  case 20: /* explainable_statement: copy_statement  */
#line 509 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3179 "parser.cpp"
    break;

  case 21: /* explainable_statement: show_statement  */
#line 510 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3185 "parser.cpp"
    break;

  case 22: /* explainable_statement: select_statement  */
#line 511 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3191 "parser.cpp"
    break;

  case 23: /* explainable_statement: delete_statement  */
#line 512 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3197 "parser.cpp"
    break;

  case 24: /* explainable_statement: update_statement  */
#line 513 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3203 "parser.cpp"
    break;

  case 25: /* explainable_statement: insert_statement  */
#line 514 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3209 "parser.cpp"
    break;

  case 26: /* explainable_statement: flush_statement  */
#line 515 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3215 "parser.cpp"
    break;

  case 27: /* explainable_statement: optimize_statement  */
#line 516 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3221 "parser.cpp"
    break;

  case 28: /* explainable_statement: command_statement  */
#line 517 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3227 "parser.cpp"
    break;

  case 29: /* explainable_statement: compact_statement  */
#line 518 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].compact_stmt); }
#line 3233 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 525 "parser.y"
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
#line 3253 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 542 "parser.y"
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
#line 3271 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')' optional_table_properties_list  */
#line 558 "parser.y"
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
#line 3304 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 587 "parser.y"
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
#line 3324 "parser.cpp"
    break;

  case 34: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 603 "parser.y"
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
#line 3345 "parser.cpp"
    break;

  case 35: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 621 "parser.y"
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
#line 3378 "parser.cpp"
    break;

  case 36: /* table_element_array: table_element  */
#line 650 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3387 "parser.cpp"
    break;

  case 37: /* table_element_array: table_element_array ',' table_element  */
#line 654 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3396 "parser.cpp"
    break;

  case 38: /* table_element: table_column  */
#line 660 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3404 "parser.cpp"
    break;

  case 39: /* table_element: table_constraint  */
#line 663 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3412 "parser.cpp"
    break;

  case 40: /* table_column: IDENTIFIER column_type default_expr  */
#line 669 "parser.y"
                                    {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                delete (yyvsp[-2].str_value);
                YYERROR;
            }
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
//            break;
//        }
        case infinity::LogicalType::kTensor:
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[-1].column_type_t).embedding_type_, (yyvsp[-1].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }

    std::shared_ptr<infinity::ParsedExpr> default_expr((yyvsp[0].const_expr_t));
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-1].column_type_t).logical_type_, type_info_ptr, std::move(default_expr));

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3455 "parser.cpp"
    break;

  case 41: /* table_column: IDENTIFIER column_type column_constraints default_expr  */
#line 707 "parser.y"
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
#line 3494 "parser.cpp"
    break;

  case 42: /* column_type: BOOLEAN  */
#line 743 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3500 "parser.cpp"
    break;

  case 43: /* column_type: TINYINT  */
#line 744 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3506 "parser.cpp"
    break;

  case 44: /* column_type: SMALLINT  */
#line 745 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3512 "parser.cpp"
    break;

  case 45: /* column_type: INTEGER  */
#line 746 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3518 "parser.cpp"
    break;

  case 46: /* column_type: INT  */
#line 747 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3524 "parser.cpp"
    break;

  case 47: /* column_type: BIGINT  */
#line 748 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3530 "parser.cpp"
    break;

  case 48: /* column_type: HUGEINT  */
#line 749 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3536 "parser.cpp"
    break;

  case 49: /* column_type: FLOAT  */
#line 750 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3542 "parser.cpp"
    break;

  case 50: /* column_type: REAL  */
#line 751 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3548 "parser.cpp"
    break;

  case 51: /* column_type: DOUBLE  */
#line 752 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3554 "parser.cpp"
    break;

  case 52: /* column_type: DATE  */
#line 753 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3560 "parser.cpp"
    break;

  case 53: /* column_type: TIME  */
#line 754 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3566 "parser.cpp"
    break;

  case 54: /* column_type: DATETIME  */
#line 755 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3572 "parser.cpp"
    break;

  case 55: /* column_type: TIMESTAMP  */
#line 756 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3578 "parser.cpp"
    break;

  case 56: /* column_type: UUID  */
#line 757 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3584 "parser.cpp"
    break;

  case 57: /* column_type: POINT  */
#line 758 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3590 "parser.cpp"
    break;

  case 58: /* column_type: LINE  */
#line 759 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3596 "parser.cpp"
    break;

  case 59: /* column_type: LSEG  */
#line 760 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3602 "parser.cpp"
    break;

  case 60: /* column_type: BOX  */
#line 761 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3608 "parser.cpp"
    break;

  case 61: /* column_type: CIRCLE  */
#line 764 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3614 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 766 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3620 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 767 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3626 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 768 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3632 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 769 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3638 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 772 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3644 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 773 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3650 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 774 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3656 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 775 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3662 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 776 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3668 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 777 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3674 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 778 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3680 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 779 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3686 "parser.cpp"
    break;

  case 74: /* column_type: TENSOR '(' BIT ',' LONG_VALUE ')'  */
#line 780 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3692 "parser.cpp"
    break;

  case 75: /* column_type: TENSOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 781 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3698 "parser.cpp"
    break;

  case 76: /* column_type: TENSOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 782 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3704 "parser.cpp"
    break;

  case 77: /* column_type: TENSOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 783 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3710 "parser.cpp"
    break;

  case 78: /* column_type: TENSOR '(' INT ',' LONG_VALUE ')'  */
#line 784 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3716 "parser.cpp"
    break;

  case 79: /* column_type: TENSOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 785 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3722 "parser.cpp"
    break;

  case 80: /* column_type: TENSOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 786 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3728 "parser.cpp"
    break;

  case 81: /* column_type: TENSOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 787 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTensor, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3734 "parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 788 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3740 "parser.cpp"
    break;

  case 83: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 789 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3746 "parser.cpp"
    break;

  case 84: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 790 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3752 "parser.cpp"
    break;

  case 85: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 791 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3758 "parser.cpp"
    break;

  case 86: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 792 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3764 "parser.cpp"
    break;

  case 87: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 793 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3770 "parser.cpp"
    break;

  case 88: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 794 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3776 "parser.cpp"
    break;

  case 89: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 795 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3782 "parser.cpp"
    break;

  case 90: /* column_constraints: column_constraint  */
#line 814 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3791 "parser.cpp"
    break;

  case 91: /* column_constraints: column_constraints column_constraint  */
#line 818 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3805 "parser.cpp"
    break;

  case 92: /* column_constraint: PRIMARY KEY  */
#line 828 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3813 "parser.cpp"
    break;

  case 93: /* column_constraint: UNIQUE  */
#line 831 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3821 "parser.cpp"
    break;

  case 94: /* column_constraint: NULLABLE  */
#line 834 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3829 "parser.cpp"
    break;

  case 95: /* column_constraint: NOT NULLABLE  */
#line 837 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3837 "parser.cpp"
    break;

  case 96: /* default_expr: DEFAULT constant_expr  */
#line 841 "parser.y"
                                     {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 3845 "parser.cpp"
    break;

  case 97: /* default_expr: %empty  */
#line 844 "parser.y"
                            {
    (yyval.const_expr_t) = nullptr;
}
#line 3853 "parser.cpp"
    break;

  case 98: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 849 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3863 "parser.cpp"
    break;

  case 99: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 854 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3873 "parser.cpp"
    break;

  case 100: /* identifier_array: IDENTIFIER  */
#line 861 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3884 "parser.cpp"
    break;

  case 101: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 867 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3895 "parser.cpp"
    break;

  case 102: /* delete_statement: DELETE FROM table_name where_clause  */
#line 877 "parser.y"
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
#line 3912 "parser.cpp"
    break;

  case 103: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 893 "parser.y"
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
#line 3951 "parser.cpp"
    break;

  case 104: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 927 "parser.y"
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
#line 3968 "parser.cpp"
    break;

  case 105: /* optional_identifier_array: '(' identifier_array ')'  */
#line 940 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3976 "parser.cpp"
    break;

  case 106: /* optional_identifier_array: %empty  */
#line 943 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3984 "parser.cpp"
    break;

  case 107: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 950 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3994 "parser.cpp"
    break;

  case 108: /* explain_type: ANALYZE  */
#line 956 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 4002 "parser.cpp"
    break;

  case 109: /* explain_type: AST  */
#line 959 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 4010 "parser.cpp"
    break;

  case 110: /* explain_type: RAW  */
#line 962 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 4018 "parser.cpp"
    break;

  case 111: /* explain_type: LOGICAL  */
#line 965 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 4026 "parser.cpp"
    break;

  case 112: /* explain_type: PHYSICAL  */
#line 968 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4034 "parser.cpp"
    break;

  case 113: /* explain_type: PIPELINE  */
#line 971 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 4042 "parser.cpp"
    break;

  case 114: /* explain_type: FRAGMENT  */
#line 974 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 4050 "parser.cpp"
    break;

  case 115: /* explain_type: %empty  */
#line 977 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 4058 "parser.cpp"
    break;

  case 116: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 984 "parser.y"
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
#line 4075 "parser.cpp"
    break;

  case 117: /* update_expr_array: update_expr  */
#line 997 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 4084 "parser.cpp"
    break;

  case 118: /* update_expr_array: update_expr_array ',' update_expr  */
#line 1001 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 4093 "parser.cpp"
    break;

  case 119: /* update_expr: IDENTIFIER '=' expr  */
#line 1006 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 4105 "parser.cpp"
    break;

  case 120: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 1019 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 4121 "parser.cpp"
    break;

  case 121: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 1032 "parser.y"
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
#line 4139 "parser.cpp"
    break;

  case 122: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1047 "parser.y"
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
#line 4157 "parser.cpp"
    break;

  case 123: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1062 "parser.y"
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
#line 4175 "parser.cpp"
    break;

  case 124: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1077 "parser.y"
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
#line 4198 "parser.cpp"
    break;

  case 125: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1100 "parser.y"
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
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 4244 "parser.cpp"
    break;

  case 126: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1141 "parser.y"
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
        }
        delete option_ptr;
    }
    delete (yyvsp[-1].copy_option_array);
}
#line 4290 "parser.cpp"
    break;

  case 127: /* select_statement: select_without_paren  */
#line 1186 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4298 "parser.cpp"
    break;

  case 128: /* select_statement: select_with_paren  */
#line 1189 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4306 "parser.cpp"
    break;

  case 129: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1192 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4320 "parser.cpp"
    break;

  case 130: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1201 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4334 "parser.cpp"
    break;

  case 131: /* select_with_paren: '(' select_without_paren ')'  */
#line 1211 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4342 "parser.cpp"
    break;

  case 132: /* select_with_paren: '(' select_with_paren ')'  */
#line 1214 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4350 "parser.cpp"
    break;

  case 133: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1218 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4359 "parser.cpp"
    break;

  case 134: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1223 "parser.y"
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
#line 4385 "parser.cpp"
    break;

  case 135: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1245 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4393 "parser.cpp"
    break;

  case 136: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1248 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4401 "parser.cpp"
    break;

  case 137: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1253 "parser.y"
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
#line 4421 "parser.cpp"
    break;

  case 138: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1269 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4429 "parser.cpp"
    break;

  case 139: /* order_by_clause: %empty  */
#line 1272 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4437 "parser.cpp"
    break;

  case 140: /* order_by_expr_list: order_by_expr  */
#line 1276 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4446 "parser.cpp"
    break;

  case 141: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1280 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4455 "parser.cpp"
    break;

  case 142: /* order_by_expr: expr order_by_type  */
#line 1285 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4465 "parser.cpp"
    break;

  case 143: /* order_by_type: ASC  */
#line 1291 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4473 "parser.cpp"
    break;

  case 144: /* order_by_type: DESC  */
#line 1294 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4481 "parser.cpp"
    break;

  case 145: /* order_by_type: %empty  */
#line 1297 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4489 "parser.cpp"
    break;

  case 146: /* limit_expr: LIMIT expr  */
#line 1301 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4497 "parser.cpp"
    break;

  case 147: /* limit_expr: %empty  */
#line 1305 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4503 "parser.cpp"
    break;

  case 148: /* offset_expr: OFFSET expr  */
#line 1307 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4511 "parser.cpp"
    break;

  case 149: /* offset_expr: %empty  */
#line 1311 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4517 "parser.cpp"
    break;

  case 150: /* distinct: DISTINCT  */
#line 1313 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4525 "parser.cpp"
    break;

  case 151: /* distinct: %empty  */
#line 1316 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4533 "parser.cpp"
    break;

  case 152: /* from_clause: FROM table_reference  */
#line 1320 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4541 "parser.cpp"
    break;

  case 153: /* from_clause: %empty  */
#line 1323 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4549 "parser.cpp"
    break;

  case 154: /* search_clause: SEARCH sub_search_array  */
#line 1327 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4559 "parser.cpp"
    break;

  case 155: /* search_clause: %empty  */
#line 1332 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4567 "parser.cpp"
    break;

  case 156: /* where_clause: WHERE expr  */
#line 1336 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4575 "parser.cpp"
    break;

  case 157: /* where_clause: %empty  */
#line 1339 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4583 "parser.cpp"
    break;

  case 158: /* having_clause: HAVING expr  */
#line 1343 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4591 "parser.cpp"
    break;

  case 159: /* having_clause: %empty  */
#line 1346 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4599 "parser.cpp"
    break;

  case 160: /* group_by_clause: GROUP BY expr_array  */
#line 1350 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4607 "parser.cpp"
    break;

  case 161: /* group_by_clause: %empty  */
#line 1353 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4615 "parser.cpp"
    break;

  case 162: /* set_operator: UNION  */
#line 1357 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4623 "parser.cpp"
    break;

  case 163: /* set_operator: UNION ALL  */
#line 1360 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4631 "parser.cpp"
    break;

  case 164: /* set_operator: INTERSECT  */
#line 1363 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4639 "parser.cpp"
    break;

  case 165: /* set_operator: EXCEPT  */
#line 1366 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4647 "parser.cpp"
    break;

  case 166: /* table_reference: table_reference_unit  */
#line 1374 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4655 "parser.cpp"
    break;

  case 167: /* table_reference: table_reference ',' table_reference_unit  */
#line 1377 "parser.y"
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
#line 4673 "parser.cpp"
    break;

  case 170: /* table_reference_name: table_name table_alias  */
#line 1394 "parser.y"
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
#line 4691 "parser.cpp"
    break;

  case 171: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1408 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4702 "parser.cpp"
    break;

  case 172: /* table_name: IDENTIFIER  */
#line 1417 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4712 "parser.cpp"
    break;

  case 173: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1422 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4724 "parser.cpp"
    break;

  case 174: /* table_alias: AS IDENTIFIER  */
#line 1431 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4734 "parser.cpp"
    break;

  case 175: /* table_alias: IDENTIFIER  */
#line 1436 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4744 "parser.cpp"
    break;

  case 176: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1441 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4755 "parser.cpp"
    break;

  case 177: /* table_alias: %empty  */
#line 1447 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4763 "parser.cpp"
    break;

  case 178: /* with_clause: WITH with_expr_list  */
#line 1454 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4771 "parser.cpp"
    break;

  case 179: /* with_clause: %empty  */
#line 1457 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4779 "parser.cpp"
    break;

  case 180: /* with_expr_list: with_expr  */
#line 1461 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4788 "parser.cpp"
    break;

  case 181: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1464 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4797 "parser.cpp"
    break;

  case 182: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1469 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4809 "parser.cpp"
    break;

  case 183: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1481 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4821 "parser.cpp"
    break;

  case 184: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1488 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4834 "parser.cpp"
    break;

  case 185: /* join_type: INNER  */
#line 1502 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4842 "parser.cpp"
    break;

  case 186: /* join_type: LEFT  */
#line 1505 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4850 "parser.cpp"
    break;

  case 187: /* join_type: RIGHT  */
#line 1508 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4858 "parser.cpp"
    break;

  case 188: /* join_type: OUTER  */
#line 1511 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4866 "parser.cpp"
    break;

  case 189: /* join_type: FULL  */
#line 1514 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4874 "parser.cpp"
    break;

  case 190: /* join_type: CROSS  */
#line 1517 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4882 "parser.cpp"
    break;

  case 191: /* join_type: %empty  */
#line 1520 "parser.y"
                {
}
#line 4889 "parser.cpp"
    break;

  case 192: /* show_statement: SHOW DATABASES  */
#line 1526 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4898 "parser.cpp"
    break;

  case 193: /* show_statement: SHOW TABLES  */
#line 1530 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4907 "parser.cpp"
    break;

  case 194: /* show_statement: SHOW VIEWS  */
#line 1534 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4916 "parser.cpp"
    break;

  case 195: /* show_statement: SHOW CONFIGS  */
#line 1538 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4925 "parser.cpp"
    break;

  case 196: /* show_statement: SHOW CONFIG IDENTIFIER  */
#line 1542 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfig;
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4937 "parser.cpp"
    break;

  case 197: /* show_statement: SHOW PROFILES  */
#line 1549 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4946 "parser.cpp"
    break;

  case 198: /* show_statement: SHOW SESSION VARIABLES  */
#line 1553 "parser.y"
                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariables;
}
#line 4955 "parser.cpp"
    break;

  case 199: /* show_statement: SHOW GLOBAL VARIABLES  */
#line 1557 "parser.y"
                        {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariables;
}
#line 4964 "parser.cpp"
    break;

  case 200: /* show_statement: SHOW SESSION VARIABLE IDENTIFIER  */
#line 1561 "parser.y"
                                   {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4975 "parser.cpp"
    break;

  case 201: /* show_statement: SHOW GLOBAL VARIABLE IDENTIFIER  */
#line 1567 "parser.y"
                                  {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalVariable;
    (yyval.show_stmt)->var_name_ = std::string((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4986 "parser.cpp"
    break;

  case 202: /* show_statement: SHOW DATABASE IDENTIFIER  */
#line 1573 "parser.y"
                           {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabase;
    (yyval.show_stmt)->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4997 "parser.cpp"
    break;

  case 203: /* show_statement: SHOW TABLE table_name  */
#line 1579 "parser.y"
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
#line 5013 "parser.cpp"
    break;

  case 204: /* show_statement: SHOW TABLE table_name COLUMNS  */
#line 1590 "parser.y"
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
#line 5029 "parser.cpp"
    break;

  case 205: /* show_statement: SHOW TABLE table_name SEGMENTS  */
#line 1601 "parser.y"
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
#line 5045 "parser.cpp"
    break;

  case 206: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE  */
#line 1612 "parser.y"
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
#line 5062 "parser.cpp"
    break;

  case 207: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCKS  */
#line 1624 "parser.y"
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
#line 5079 "parser.cpp"
    break;

  case 208: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1636 "parser.y"
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
#line 5097 "parser.cpp"
    break;

  case 209: /* show_statement: SHOW TABLE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE COLUMN LONG_VALUE  */
#line 1649 "parser.y"
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
#line 5116 "parser.cpp"
    break;

  case 210: /* show_statement: SHOW TABLE table_name INDEXES  */
#line 1663 "parser.y"
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
#line 5132 "parser.cpp"
    break;

  case 211: /* show_statement: SHOW TABLE table_name INDEX IDENTIFIER  */
#line 1674 "parser.y"
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
#line 5151 "parser.cpp"
    break;

  case 212: /* flush_statement: FLUSH DATA  */
#line 1692 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 5160 "parser.cpp"
    break;

  case 213: /* flush_statement: FLUSH LOG  */
#line 1696 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 5169 "parser.cpp"
    break;

  case 214: /* flush_statement: FLUSH BUFFER  */
#line 1700 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 5178 "parser.cpp"
    break;

  case 215: /* optimize_statement: OPTIMIZE table_name  */
#line 1708 "parser.y"
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
#line 5193 "parser.cpp"
    break;

  case 216: /* command_statement: USE IDENTIFIER  */
#line 1722 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 5204 "parser.cpp"
    break;

  case 217: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1728 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 5214 "parser.cpp"
    break;

  case 218: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1733 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5225 "parser.cpp"
    break;

  case 219: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1739 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5236 "parser.cpp"
    break;

  case 220: /* command_statement: SET SESSION IDENTIFIER IDENTIFIER  */
#line 1745 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5249 "parser.cpp"
    break;

  case 221: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1753 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5260 "parser.cpp"
    break;

  case 222: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1759 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5271 "parser.cpp"
    break;

  case 223: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1765 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5282 "parser.cpp"
    break;

  case 224: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1771 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5293 "parser.cpp"
    break;

  case 225: /* command_statement: SET GLOBAL IDENTIFIER IDENTIFIER  */
#line 1777 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5306 "parser.cpp"
    break;

  case 226: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1785 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5317 "parser.cpp"
    break;

  case 227: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1791 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5328 "parser.cpp"
    break;

  case 228: /* command_statement: SET CONFIG IDENTIFIER ON  */
#line 1797 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 5339 "parser.cpp"
    break;

  case 229: /* command_statement: SET CONFIG IDENTIFIER OFF  */
#line 1803 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 5350 "parser.cpp"
    break;

  case 230: /* command_statement: SET CONFIG IDENTIFIER IDENTIFIER  */
#line 1809 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 5363 "parser.cpp"
    break;

  case 231: /* command_statement: SET CONFIG IDENTIFIER LONG_VALUE  */
#line 1817 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 5374 "parser.cpp"
    break;

  case 232: /* command_statement: SET CONFIG IDENTIFIER DOUBLE_VALUE  */
#line 1823 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_unique<infinity::SetCmd>(infinity::SetScope::kConfig, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5385 "parser.cpp"
    break;

  case 233: /* compact_statement: COMPACT TABLE table_name  */
#line 1830 "parser.y"
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
#line 5402 "parser.cpp"
    break;

  case 234: /* expr_array: expr_alias  */
#line 1847 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5411 "parser.cpp"
    break;

  case 235: /* expr_array: expr_array ',' expr_alias  */
#line 1851 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5420 "parser.cpp"
    break;

  case 236: /* expr_array_list: '(' expr_array ')'  */
#line 1856 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5429 "parser.cpp"
    break;

  case 237: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1860 "parser.y"
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
#line 5449 "parser.cpp"
    break;

  case 238: /* expr_alias: expr AS IDENTIFIER  */
#line 1887 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5460 "parser.cpp"
    break;

  case 239: /* expr_alias: expr  */
#line 1893 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5468 "parser.cpp"
    break;

  case 245: /* operand: '(' expr ')'  */
#line 1903 "parser.y"
                      {
    (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5476 "parser.cpp"
    break;

  case 246: /* operand: '(' select_without_paren ')'  */
#line 1906 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5487 "parser.cpp"
    break;

  case 247: /* operand: constant_expr  */
#line 1912 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5495 "parser.cpp"
    break;

  case 257: /* optional_maxsim_option: ',' STRING  */
#line 1925 "parser.y"
                                    {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 5503 "parser.cpp"
    break;

  case 258: /* optional_maxsim_option: %empty  */
#line 1928 "parser.y"
  {
    (yyval.str_value) = nullptr;
}
#line 5511 "parser.cpp"
    break;

  case 259: /* match_tensor_expr: MATCH TENSOR '(' column_expr ',' array_expr ',' STRING ',' STRING optional_maxsim_option ')'  */
#line 1934 "parser.y"
                                                                                                                 {
    infinity::MatchTensorExpr* match_tensor_expr = new infinity::MatchTensorExpr();
    (yyval.expr_t) = match_tensor_expr;
    // TENSOR search column
    match_tensor_expr->column_expr_ = (yyvsp[-8].expr_t);
    // TENSOR search options
    if ((yyvsp[-1].str_value)) {
        match_tensor_expr->options_text_ = (yyvsp[-1].str_value);
        free((yyvsp[-1].str_value));
    }
    // TENSOR search type
    ParserHelper::ToLower((yyvsp[-2].str_value));
    if(strcmp((yyvsp[-2].str_value), "maxsim") == 0) {
        match_tensor_expr->search_method_ = infinity::MatchTensorMethod::kMaxSim;
        free((yyvsp[-2].str_value));
    } else {
        free((yyvsp[-2].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid MATCH TENSOR method type");
        YYERROR;
    }
    // TENSOR query tensor
    ParserHelper::ToLower((yyvsp[-4].str_value));
    if(strcmp((yyvsp[-4].str_value), "float") == 0) {
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-6].const_expr_t)->double_array_.empty())) {
            match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->double_array_.size();
            match_tensor_expr->embedding_data_ptr_ = new float[match_tensor_expr->dimension_];
            for(long i = 0; i < match_tensor_expr->dimension_; ++ i) {
                ((float*)(match_tensor_expr->embedding_data_ptr_))[i] = (yyvsp[-6].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-6].const_expr_t)->long_array_.empty())) {
            match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
            match_tensor_expr->embedding_data_ptr_ = new float[match_tensor_expr->dimension_];
            for(long i = 0; i < match_tensor_expr->dimension_; ++ i) {
                ((float*)(match_tensor_expr->embedding_data_ptr_))[i] = (yyvsp[-6].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else if(strcmp((yyvsp[-4].str_value), "tinyint") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        match_tensor_expr->embedding_data_ptr_ = new char[match_tensor_expr->dimension_];
        for(long i = 0; i < match_tensor_expr->dimension_; ++ i) {
            ((char*)match_tensor_expr->embedding_data_ptr_)[i] = (yyvsp[-6].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else if(strcmp((yyvsp[-4].str_value), "smallint") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        match_tensor_expr->embedding_data_ptr_ = new short int[match_tensor_expr->dimension_];
        for(long i = 0; i < match_tensor_expr->dimension_; ++ i) {
            ((short int*)match_tensor_expr->embedding_data_ptr_)[i] = (yyvsp[-6].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else if(strcmp((yyvsp[-4].str_value), "integer") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        match_tensor_expr->embedding_data_ptr_ = new int[match_tensor_expr->dimension_];
        for(long i = 0; i < match_tensor_expr->dimension_; ++ i) {
            ((int*)match_tensor_expr->embedding_data_ptr_)[i] = (yyvsp[-6].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else if(strcmp((yyvsp[-4].str_value), "bigint") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        match_tensor_expr->embedding_data_ptr_ = new long[match_tensor_expr->dimension_];
        memcpy(match_tensor_expr->embedding_data_ptr_, (void*)(yyvsp[-6].const_expr_t)->long_array_.data(), match_tensor_expr->dimension_ * sizeof(long));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else if(strcmp((yyvsp[-4].str_value), "bit") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->long_array_.size();
        if(match_tensor_expr->dimension_ % 8 == 0) {
            match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = match_tensor_expr->dimension_ / 8;
            uint8_t *data_ptr = new uint8_t[embedding_size];
            match_tensor_expr->embedding_data_ptr_ = data_ptr;
            for(long i = 0; i < embedding_size; ++ i) {
                uint8_t embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    double val = (yyvsp[-6].const_expr_t)->long_array_[i * 8 + bit_idx];
                    if (val != 0 && val != 1) {
                        free((yyvsp[-4].str_value));
                        delete (yyvsp[-6].const_expr_t);
                        delete (yyval.expr_t);
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                    if(val == 1) {
                        embedding_unit |= (uint8_t(1) << bit_idx);
                    }
                }
                data_ptr[i] = embedding_unit;
            }
            free((yyvsp[-4].str_value));
            delete (yyvsp[-6].const_expr_t);
        } else {
            free((yyvsp[-4].str_value));
            delete (yyvsp[-6].const_expr_t);
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "tensor data type is bit which length should be aligned with 8");
            YYERROR;
        }
    } else if(strcmp((yyvsp[-4].str_value), "double") == 0) {
        match_tensor_expr->dimension_ = (yyvsp[-6].const_expr_t)->double_array_.size();
        match_tensor_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        match_tensor_expr->embedding_data_ptr_ = new double[match_tensor_expr->dimension_];
        memcpy(match_tensor_expr->embedding_data_ptr_, (void*)(yyvsp[-6].const_expr_t)->double_array_.data(), match_tensor_expr->dimension_ * sizeof(double));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
    } else {
        free((yyvsp[-4].str_value));
        delete (yyvsp[-6].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid tensor data type");
        YYERROR;
    }
}
#line 5641 "parser.cpp"
    break;

  case 260: /* match_vector_expr: MATCH VECTOR '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 2060 "parser.y"
                                                                                                                        {
    infinity::KnnExpr* match_vector_expr = new infinity::KnnExpr();
    (yyval.expr_t) = match_vector_expr;

    // KNN search column
    match_vector_expr->column_expr_ = (yyvsp[-10].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    if(strcmp((yyvsp[-4].str_value), "l2") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-4].str_value), "ip") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-4].str_value), "cosine") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-4].str_value), "hamming") == 0) {
        match_vector_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
    } else {
        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
            delete param_ptr;
        }
        delete (yyvsp[0].with_index_param_list_t);
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn distance type");
        YYERROR;
    }

    // KNN data type
    ParserHelper::ToLower((yyvsp[-6].str_value));
    if(strcmp((yyvsp[-6].str_value), "float") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-8].const_expr_t)->double_array_.empty())) {
            match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
            match_vector_expr->embedding_data_ptr_ = new float[match_vector_expr->dimension_];
            for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
                ((float*)(match_vector_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-8].const_expr_t)->long_array_.empty())) {
            match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
            match_vector_expr->embedding_data_ptr_ = new float[match_vector_expr->dimension_];
            for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
                ((float*)(match_vector_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "tinyint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        match_vector_expr->embedding_data_ptr_ = new char[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((char*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "smallint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        match_vector_expr->embedding_data_ptr_ = new short int[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((short int*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "integer") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        match_vector_expr->embedding_data_ptr_ = new int[match_vector_expr->dimension_];

        for(long i = 0; i < match_vector_expr->dimension_; ++ i) {
            ((int*)match_vector_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bigint") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        match_vector_expr->embedding_data_ptr_ = new long[match_vector_expr->dimension_];

        memcpy(match_vector_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->long_array_.data(), match_vector_expr->dimension_ * sizeof(long));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bit") == 0 and match_vector_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        if(match_vector_expr->dimension_ % 8 == 0) {
            match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = match_vector_expr->dimension_ / 8;
            char *char_ptr = new char[embedding_size];
            uint8_t *data_ptr = reinterpret_cast<uint8_t *>(char_ptr);
            match_vector_expr->embedding_data_ptr_ = char_ptr;
            for(long i = 0; i < embedding_size; ++ i) {
                uint8_t embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        embedding_unit |= (uint8_t(1) << bit_idx);
                    } else if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        // no-op
                    } else {
                        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
                            delete param_ptr;
                        }
                        delete (yyvsp[0].with_index_param_list_t);
                        free((yyvsp[-6].str_value));
                        free((yyvsp[-4].str_value));
                        delete (yyvsp[-8].const_expr_t);
                        delete (yyval.expr_t);
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                }
                data_ptr[i] = embedding_unit;
            }
            free((yyvsp[-6].str_value));
            free((yyvsp[-4].str_value));
            delete (yyvsp[-8].const_expr_t);
        } else {
            for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
                delete param_ptr;
            }
            delete (yyvsp[0].with_index_param_list_t);
            free((yyvsp[-6].str_value));
            free((yyvsp[-4].str_value));
            delete (yyvsp[-8].const_expr_t);
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "KNN data type is bit which length should be aligned with 8");
            YYERROR;
        }

    } else if(strcmp((yyvsp[-6].str_value), "double") == 0 and match_vector_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        match_vector_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
        match_vector_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        match_vector_expr->embedding_data_ptr_ = new double[match_vector_expr->dimension_];

        memcpy(match_vector_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->double_array_.data(), match_vector_expr->dimension_ * sizeof(double));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else {
        for (auto* param_ptr: *(yyvsp[0].with_index_param_list_t)) {
            delete param_ptr;
        }
        delete (yyvsp[0].with_index_param_list_t);
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn data type");
        YYERROR;
    }
    match_vector_expr->topn_ = (yyvsp[-2].long_value);
    match_vector_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
}
#line 5811 "parser.cpp"
    break;

  case 261: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ')'  */
#line 2226 "parser.y"
                                                       {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5824 "parser.cpp"
    break;

  case 262: /* match_text_expr: MATCH TEXT '(' STRING ',' STRING ',' STRING ')'  */
#line 2234 "parser.y"
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
#line 5839 "parser.cpp"
    break;

  case 263: /* query_expr: QUERY '(' STRING ')'  */
#line 2245 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5850 "parser.cpp"
    break;

  case 264: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 2251 "parser.y"
                                  {
    infinity::MatchExpr* match_text_expr = new infinity::MatchExpr();
    match_text_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_text_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_text_expr;
}
#line 5863 "parser.cpp"
    break;

  case 265: /* fusion_expr: FUSION '(' STRING ')'  */
#line 2260 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5874 "parser.cpp"
    break;

  case 266: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 2266 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5887 "parser.cpp"
    break;

  case 267: /* sub_search_array: match_vector_expr  */
#line 2276 "parser.y"
                                     {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5896 "parser.cpp"
    break;

  case 268: /* sub_search_array: match_text_expr  */
#line 2280 "parser.y"
                  {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5905 "parser.cpp"
    break;

  case 269: /* sub_search_array: match_tensor_expr  */
#line 2284 "parser.y"
                    {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5914 "parser.cpp"
    break;

  case 270: /* sub_search_array: query_expr  */
#line 2288 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5923 "parser.cpp"
    break;

  case 271: /* sub_search_array: fusion_expr  */
#line 2292 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5932 "parser.cpp"
    break;

  case 272: /* sub_search_array: sub_search_array ',' match_vector_expr  */
#line 2296 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5941 "parser.cpp"
    break;

  case 273: /* sub_search_array: sub_search_array ',' match_text_expr  */
#line 2300 "parser.y"
                                       {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5950 "parser.cpp"
    break;

  case 274: /* sub_search_array: sub_search_array ',' match_tensor_expr  */
#line 2304 "parser.y"
                                         {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5959 "parser.cpp"
    break;

  case 275: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2308 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5968 "parser.cpp"
    break;

  case 276: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2312 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5977 "parser.cpp"
    break;

  case 277: /* function_expr: IDENTIFIER '(' ')'  */
#line 2317 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5990 "parser.cpp"
    break;

  case 278: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2325 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 6003 "parser.cpp"
    break;

  case 279: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2333 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 6017 "parser.cpp"
    break;

  case 280: /* function_expr: operand IS NOT NULLABLE  */
#line 2342 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6029 "parser.cpp"
    break;

  case 281: /* function_expr: operand IS NULLABLE  */
#line 2349 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6041 "parser.cpp"
    break;

  case 282: /* function_expr: NOT operand  */
#line 2356 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6053 "parser.cpp"
    break;

  case 283: /* function_expr: '-' operand  */
#line 2363 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6065 "parser.cpp"
    break;

  case 284: /* function_expr: '+' operand  */
#line 2370 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6077 "parser.cpp"
    break;

  case 285: /* function_expr: operand '-' operand  */
#line 2377 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6090 "parser.cpp"
    break;

  case 286: /* function_expr: operand '+' operand  */
#line 2385 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6103 "parser.cpp"
    break;

  case 287: /* function_expr: operand '*' operand  */
#line 2393 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6116 "parser.cpp"
    break;

  case 288: /* function_expr: operand '/' operand  */
#line 2401 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6129 "parser.cpp"
    break;

  case 289: /* function_expr: operand '%' operand  */
#line 2409 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6142 "parser.cpp"
    break;

  case 290: /* function_expr: operand '=' operand  */
#line 2417 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6155 "parser.cpp"
    break;

  case 291: /* function_expr: operand EQUAL operand  */
#line 2425 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6168 "parser.cpp"
    break;

  case 292: /* function_expr: operand NOT_EQ operand  */
#line 2433 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6181 "parser.cpp"
    break;

  case 293: /* function_expr: operand '<' operand  */
#line 2441 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6194 "parser.cpp"
    break;

  case 294: /* function_expr: operand '>' operand  */
#line 2449 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6207 "parser.cpp"
    break;

  case 295: /* function_expr: operand LESS_EQ operand  */
#line 2457 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6220 "parser.cpp"
    break;

  case 296: /* function_expr: operand GREATER_EQ operand  */
#line 2465 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6233 "parser.cpp"
    break;

  case 297: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2473 "parser.y"
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
#line 6268 "parser.cpp"
    break;

  case 298: /* function_expr: operand LIKE operand  */
#line 2503 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6281 "parser.cpp"
    break;

  case 299: /* function_expr: operand NOT LIKE operand  */
#line 2511 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6294 "parser.cpp"
    break;

  case 300: /* conjunction_expr: expr AND expr  */
#line 2520 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6307 "parser.cpp"
    break;

  case 301: /* conjunction_expr: expr OR expr  */
#line 2528 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 6320 "parser.cpp"
    break;

  case 302: /* between_expr: operand BETWEEN operand AND operand  */
#line 2537 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 6332 "parser.cpp"
    break;

  case 303: /* in_expr: operand IN '(' expr_array ')'  */
#line 2545 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6343 "parser.cpp"
    break;

  case 304: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2551 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 6354 "parser.cpp"
    break;

  case 305: /* case_expr: CASE expr case_check_array END  */
#line 2558 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6365 "parser.cpp"
    break;

  case 306: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2564 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6377 "parser.cpp"
    break;

  case 307: /* case_expr: CASE case_check_array END  */
#line 2571 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 6387 "parser.cpp"
    break;

  case 308: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2576 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 6398 "parser.cpp"
    break;

  case 309: /* case_check_array: WHEN expr THEN expr  */
#line 2583 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 6410 "parser.cpp"
    break;

  case 310: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2590 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 6422 "parser.cpp"
    break;

  case 311: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2598 "parser.y"
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
#line 6450 "parser.cpp"
    break;

  case 312: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2622 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6461 "parser.cpp"
    break;

  case 313: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2628 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6472 "parser.cpp"
    break;

  case 314: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2634 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6484 "parser.cpp"
    break;

  case 315: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2641 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 6496 "parser.cpp"
    break;

  case 316: /* column_expr: IDENTIFIER  */
#line 2649 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6508 "parser.cpp"
    break;

  case 317: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2656 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 6520 "parser.cpp"
    break;

  case 318: /* column_expr: '*'  */
#line 2663 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6530 "parser.cpp"
    break;

  case 319: /* column_expr: column_expr '.' '*'  */
#line 2668 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 6544 "parser.cpp"
    break;

  case 320: /* constant_expr: STRING  */
#line 2678 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6554 "parser.cpp"
    break;

  case 321: /* constant_expr: TRUE  */
#line 2683 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6564 "parser.cpp"
    break;

  case 322: /* constant_expr: FALSE  */
#line 2688 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6574 "parser.cpp"
    break;

  case 323: /* constant_expr: DOUBLE_VALUE  */
#line 2693 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6584 "parser.cpp"
    break;

  case 324: /* constant_expr: LONG_VALUE  */
#line 2698 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6594 "parser.cpp"
    break;

  case 325: /* constant_expr: DATE STRING  */
#line 2703 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6604 "parser.cpp"
    break;

  case 326: /* constant_expr: TIME STRING  */
#line 2708 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6614 "parser.cpp"
    break;

  case 327: /* constant_expr: DATETIME STRING  */
#line 2713 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6624 "parser.cpp"
    break;

  case 328: /* constant_expr: TIMESTAMP STRING  */
#line 2718 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6634 "parser.cpp"
    break;

  case 329: /* constant_expr: INTERVAL interval_expr  */
#line 2723 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6642 "parser.cpp"
    break;

  case 330: /* constant_expr: interval_expr  */
#line 2726 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6650 "parser.cpp"
    break;

  case 331: /* constant_expr: long_array_expr  */
#line 2729 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6658 "parser.cpp"
    break;

  case 332: /* constant_expr: double_array_expr  */
#line 2732 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6666 "parser.cpp"
    break;

  case 333: /* array_expr: long_array_expr  */
#line 2736 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6674 "parser.cpp"
    break;

  case 334: /* array_expr: double_array_expr  */
#line 2739 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6682 "parser.cpp"
    break;

  case 335: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2743 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6690 "parser.cpp"
    break;

  case 336: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2747 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6700 "parser.cpp"
    break;

  case 337: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2752 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6709 "parser.cpp"
    break;

  case 338: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2757 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6717 "parser.cpp"
    break;

  case 339: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2761 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6727 "parser.cpp"
    break;

  case 340: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2766 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6736 "parser.cpp"
    break;

  case 341: /* interval_expr: LONG_VALUE SECONDS  */
#line 2771 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6747 "parser.cpp"
    break;

  case 342: /* interval_expr: LONG_VALUE SECOND  */
#line 2777 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6758 "parser.cpp"
    break;

  case 343: /* interval_expr: LONG_VALUE MINUTES  */
#line 2783 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6769 "parser.cpp"
    break;

  case 344: /* interval_expr: LONG_VALUE MINUTE  */
#line 2789 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6780 "parser.cpp"
    break;

  case 345: /* interval_expr: LONG_VALUE HOURS  */
#line 2795 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6791 "parser.cpp"
    break;

  case 346: /* interval_expr: LONG_VALUE HOUR  */
#line 2801 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6802 "parser.cpp"
    break;

  case 347: /* interval_expr: LONG_VALUE DAYS  */
#line 2807 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6813 "parser.cpp"
    break;

  case 348: /* interval_expr: LONG_VALUE DAY  */
#line 2813 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6824 "parser.cpp"
    break;

  case 349: /* interval_expr: LONG_VALUE MONTHS  */
#line 2819 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6835 "parser.cpp"
    break;

  case 350: /* interval_expr: LONG_VALUE MONTH  */
#line 2825 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6846 "parser.cpp"
    break;

  case 351: /* interval_expr: LONG_VALUE YEARS  */
#line 2831 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6857 "parser.cpp"
    break;

  case 352: /* interval_expr: LONG_VALUE YEAR  */
#line 2837 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6868 "parser.cpp"
    break;

  case 353: /* copy_option_list: copy_option  */
#line 2848 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6877 "parser.cpp"
    break;

  case 354: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2852 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6886 "parser.cpp"
    break;

  case 355: /* copy_option: FORMAT IDENTIFIER  */
#line 2857 "parser.y"
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
    } else {
        free((yyvsp[0].str_value));
        delete (yyval.copy_option_t);
        yyerror(&yyloc, scanner, result, "Unknown file format");
        YYERROR;
    }
}
#line 6913 "parser.cpp"
    break;

  case 356: /* copy_option: DELIMITER STRING  */
#line 2879 "parser.y"
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
#line 6928 "parser.cpp"
    break;

  case 357: /* copy_option: HEADER  */
#line 2889 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6938 "parser.cpp"
    break;

  case 358: /* file_path: STRING  */
#line 2895 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6946 "parser.cpp"
    break;

  case 359: /* if_exists: IF EXISTS  */
#line 2899 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6952 "parser.cpp"
    break;

  case 360: /* if_exists: %empty  */
#line 2900 "parser.y"
  { (yyval.bool_value) = false; }
#line 6958 "parser.cpp"
    break;

  case 361: /* if_not_exists: IF NOT EXISTS  */
#line 2902 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6964 "parser.cpp"
    break;

  case 362: /* if_not_exists: %empty  */
#line 2903 "parser.y"
  { (yyval.bool_value) = false; }
#line 6970 "parser.cpp"
    break;

  case 365: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2918 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6983 "parser.cpp"
    break;

  case 366: /* if_not_exists_info: %empty  */
#line 2926 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6991 "parser.cpp"
    break;

  case 367: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2930 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6999 "parser.cpp"
    break;

  case 368: /* with_index_param_list: %empty  */
#line 2933 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 7007 "parser.cpp"
    break;

  case 369: /* optional_table_properties_list: PROPERTIES '(' index_param_list ')'  */
#line 2937 "parser.y"
                                                                     {
    (yyval.with_index_param_list_t) = (yyvsp[-1].index_param_list_t);
}
#line 7015 "parser.cpp"
    break;

  case 370: /* optional_table_properties_list: %empty  */
#line 2940 "parser.y"
  {
    (yyval.with_index_param_list_t) = nullptr;
}
#line 7023 "parser.cpp"
    break;

  case 371: /* index_param_list: index_param  */
#line 2944 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 7032 "parser.cpp"
    break;

  case 372: /* index_param_list: index_param_list ',' index_param  */
#line 2948 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 7041 "parser.cpp"
    break;

  case 373: /* index_param: IDENTIFIER  */
#line 2953 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7051 "parser.cpp"
    break;

  case 374: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2958 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 7064 "parser.cpp"
    break;

  case 375: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2966 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 7076 "parser.cpp"
    break;

  case 376: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2973 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 7088 "parser.cpp"
    break;

  case 377: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2984 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
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
#line 7141 "parser.cpp"
    break;

  case 378: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 3032 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hnsw") == 0) {
        index_type = infinity::IndexType::kHnsw;
    } else if (strcmp((yyvsp[-1].str_value), "ivfflat") == 0) {
        index_type = infinity::IndexType::kIVFFlat;
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
        delete (yyvsp[-6].index_info_list_t);
        delete (yyvsp[-4].identifier_array_t);
        delete (yyvsp[0].with_index_param_list_t);
    }
    (yyval.index_info_list_t) = (yyvsp[-6].index_info_list_t);
    (yyval.index_info_list_t)->reserve((yyval.index_info_list_t)->size() + index_count);

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
#line 7195 "parser.cpp"
    break;

  case 379: /* index_info_list: '(' identifier_array ')'  */
#line 3081 "parser.y"
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
#line 7213 "parser.cpp"
    break;


#line 7217 "parser.cpp"

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

#line 3095 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

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
  YYSYMBOL_DESCRIBE = 20,                  /* DESCRIBE  */
  YYSYMBOL_UNION = 21,                     /* UNION  */
  YYSYMBOL_ALL = 22,                       /* ALL  */
  YYSYMBOL_INTERSECT = 23,                 /* INTERSECT  */
  YYSYMBOL_EXCEPT = 24,                    /* EXCEPT  */
  YYSYMBOL_FLUSH = 25,                     /* FLUSH  */
  YYSYMBOL_USE = 26,                       /* USE  */
  YYSYMBOL_OPTIMIZE = 27,                  /* OPTIMIZE  */
  YYSYMBOL_DATABASE = 28,                  /* DATABASE  */
  YYSYMBOL_TABLE = 29,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 30,                /* COLLECTION  */
  YYSYMBOL_TABLES = 31,                    /* TABLES  */
  YYSYMBOL_INTO = 32,                      /* INTO  */
  YYSYMBOL_VALUES = 33,                    /* VALUES  */
  YYSYMBOL_AST = 34,                       /* AST  */
  YYSYMBOL_PIPELINE = 35,                  /* PIPELINE  */
  YYSYMBOL_RAW = 36,                       /* RAW  */
  YYSYMBOL_LOGICAL = 37,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 38,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 39,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 40,                      /* VIEW  */
  YYSYMBOL_INDEX = 41,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 42,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 43,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 44,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 45,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 46,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 47,                     /* BLOCK  */
  YYSYMBOL_GROUP = 48,                     /* GROUP  */
  YYSYMBOL_BY = 49,                        /* BY  */
  YYSYMBOL_HAVING = 50,                    /* HAVING  */
  YYSYMBOL_AS = 51,                        /* AS  */
  YYSYMBOL_NATURAL = 52,                   /* NATURAL  */
  YYSYMBOL_JOIN = 53,                      /* JOIN  */
  YYSYMBOL_LEFT = 54,                      /* LEFT  */
  YYSYMBOL_RIGHT = 55,                     /* RIGHT  */
  YYSYMBOL_OUTER = 56,                     /* OUTER  */
  YYSYMBOL_FULL = 57,                      /* FULL  */
  YYSYMBOL_ON = 58,                        /* ON  */
  YYSYMBOL_INNER = 59,                     /* INNER  */
  YYSYMBOL_CROSS = 60,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 61,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 62,                     /* WHERE  */
  YYSYMBOL_ORDER = 63,                     /* ORDER  */
  YYSYMBOL_LIMIT = 64,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 65,                    /* OFFSET  */
  YYSYMBOL_ASC = 66,                       /* ASC  */
  YYSYMBOL_DESC = 67,                      /* DESC  */
  YYSYMBOL_IF = 68,                        /* IF  */
  YYSYMBOL_NOT = 69,                       /* NOT  */
  YYSYMBOL_EXISTS = 70,                    /* EXISTS  */
  YYSYMBOL_IN = 71,                        /* IN  */
  YYSYMBOL_FROM = 72,                      /* FROM  */
  YYSYMBOL_TO = 73,                        /* TO  */
  YYSYMBOL_WITH = 74,                      /* WITH  */
  YYSYMBOL_DELIMITER = 75,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 76,                    /* FORMAT  */
  YYSYMBOL_HEADER = 77,                    /* HEADER  */
  YYSYMBOL_CAST = 78,                      /* CAST  */
  YYSYMBOL_END = 79,                       /* END  */
  YYSYMBOL_CASE = 80,                      /* CASE  */
  YYSYMBOL_ELSE = 81,                      /* ELSE  */
  YYSYMBOL_THEN = 82,                      /* THEN  */
  YYSYMBOL_WHEN = 83,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 84,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 85,                   /* INTEGER  */
  YYSYMBOL_INT = 86,                       /* INT  */
  YYSYMBOL_TINYINT = 87,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 88,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 89,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 90,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 91,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 92,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 93,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 94,                    /* DOUBLE  */
  YYSYMBOL_REAL = 95,                      /* REAL  */
  YYSYMBOL_DECIMAL = 96,                   /* DECIMAL  */
  YYSYMBOL_DATE = 97,                      /* DATE  */
  YYSYMBOL_TIME = 98,                      /* TIME  */
  YYSYMBOL_DATETIME = 99,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 100,                /* TIMESTAMP  */
  YYSYMBOL_UUID = 101,                     /* UUID  */
  YYSYMBOL_POINT = 102,                    /* POINT  */
  YYSYMBOL_LINE = 103,                     /* LINE  */
  YYSYMBOL_LSEG = 104,                     /* LSEG  */
  YYSYMBOL_BOX = 105,                      /* BOX  */
  YYSYMBOL_PATH = 106,                     /* PATH  */
  YYSYMBOL_POLYGON = 107,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 108,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 109,                     /* BLOB  */
  YYSYMBOL_BITMAP = 110,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 111,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 112,                   /* VECTOR  */
  YYSYMBOL_BIT = 113,                      /* BIT  */
  YYSYMBOL_PRIMARY = 114,                  /* PRIMARY  */
  YYSYMBOL_KEY = 115,                      /* KEY  */
  YYSYMBOL_UNIQUE = 116,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 117,                 /* NULLABLE  */
  YYSYMBOL_IS = 118,                       /* IS  */
  YYSYMBOL_TRUE = 119,                     /* TRUE  */
  YYSYMBOL_FALSE = 120,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 121,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 122,                   /* SECOND  */
  YYSYMBOL_SECONDS = 123,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 124,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 125,                  /* MINUTES  */
  YYSYMBOL_HOUR = 126,                     /* HOUR  */
  YYSYMBOL_HOURS = 127,                    /* HOURS  */
  YYSYMBOL_DAY = 128,                      /* DAY  */
  YYSYMBOL_DAYS = 129,                     /* DAYS  */
  YYSYMBOL_MONTH = 130,                    /* MONTH  */
  YYSYMBOL_MONTHS = 131,                   /* MONTHS  */
  YYSYMBOL_YEAR = 132,                     /* YEAR  */
  YYSYMBOL_YEARS = 133,                    /* YEARS  */
  YYSYMBOL_EQUAL = 134,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 135,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 136,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 137,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 138,                  /* BETWEEN  */
  YYSYMBOL_AND = 139,                      /* AND  */
  YYSYMBOL_OR = 140,                       /* OR  */
  YYSYMBOL_EXTRACT = 141,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 142,                     /* LIKE  */
  YYSYMBOL_DATA = 143,                     /* DATA  */
  YYSYMBOL_LOG = 144,                      /* LOG  */
  YYSYMBOL_BUFFER = 145,                   /* BUFFER  */
  YYSYMBOL_KNN = 146,                      /* KNN  */
  YYSYMBOL_USING = 147,                    /* USING  */
  YYSYMBOL_SESSION = 148,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 149,                   /* GLOBAL  */
  YYSYMBOL_OFF = 150,                      /* OFF  */
  YYSYMBOL_EXPORT = 151,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 152,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 153,                  /* CONFIGS  */
  YYSYMBOL_PROFILES = 154,                 /* PROFILES  */
  YYSYMBOL_STATUS = 155,                   /* STATUS  */
  YYSYMBOL_SEARCH = 156,                   /* SEARCH  */
  YYSYMBOL_MATCH = 157,                    /* MATCH  */
  YYSYMBOL_QUERY = 158,                    /* QUERY  */
  YYSYMBOL_FUSION = 159,                   /* FUSION  */
  YYSYMBOL_NUMBER = 160,                   /* NUMBER  */
  YYSYMBOL_161_ = 161,                     /* '='  */
  YYSYMBOL_162_ = 162,                     /* '<'  */
  YYSYMBOL_163_ = 163,                     /* '>'  */
  YYSYMBOL_164_ = 164,                     /* '+'  */
  YYSYMBOL_165_ = 165,                     /* '-'  */
  YYSYMBOL_166_ = 166,                     /* '*'  */
  YYSYMBOL_167_ = 167,                     /* '/'  */
  YYSYMBOL_168_ = 168,                     /* '%'  */
  YYSYMBOL_169_ = 169,                     /* '['  */
  YYSYMBOL_170_ = 170,                     /* ']'  */
  YYSYMBOL_171_ = 171,                     /* '('  */
  YYSYMBOL_172_ = 172,                     /* ')'  */
  YYSYMBOL_173_ = 173,                     /* '.'  */
  YYSYMBOL_174_ = 174,                     /* ';'  */
  YYSYMBOL_175_ = 175,                     /* ','  */
  YYSYMBOL_YYACCEPT = 176,                 /* $accept  */
  YYSYMBOL_input_pattern = 177,            /* input_pattern  */
  YYSYMBOL_statement_list = 178,           /* statement_list  */
  YYSYMBOL_statement = 179,                /* statement  */
  YYSYMBOL_explainable_statement = 180,    /* explainable_statement  */
  YYSYMBOL_create_statement = 181,         /* create_statement  */
  YYSYMBOL_table_element_array = 182,      /* table_element_array  */
  YYSYMBOL_table_element = 183,            /* table_element  */
  YYSYMBOL_table_column = 184,             /* table_column  */
  YYSYMBOL_column_type = 185,              /* column_type  */
  YYSYMBOL_column_constraints = 186,       /* column_constraints  */
  YYSYMBOL_column_constraint = 187,        /* column_constraint  */
  YYSYMBOL_table_constraint = 188,         /* table_constraint  */
  YYSYMBOL_identifier_array = 189,         /* identifier_array  */
  YYSYMBOL_delete_statement = 190,         /* delete_statement  */
  YYSYMBOL_insert_statement = 191,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 192, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 193,        /* explain_statement  */
  YYSYMBOL_explain_type = 194,             /* explain_type  */
  YYSYMBOL_update_statement = 195,         /* update_statement  */
  YYSYMBOL_update_expr_array = 196,        /* update_expr_array  */
  YYSYMBOL_update_expr = 197,              /* update_expr  */
  YYSYMBOL_drop_statement = 198,           /* drop_statement  */
  YYSYMBOL_copy_statement = 199,           /* copy_statement  */
  YYSYMBOL_select_statement = 200,         /* select_statement  */
  YYSYMBOL_select_with_paren = 201,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 202,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 203, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 204, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 205, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 206,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 207,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 208,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 209,            /* order_by_type  */
  YYSYMBOL_limit_expr = 210,               /* limit_expr  */
  YYSYMBOL_offset_expr = 211,              /* offset_expr  */
  YYSYMBOL_distinct = 212,                 /* distinct  */
  YYSYMBOL_from_clause = 213,              /* from_clause  */
  YYSYMBOL_search_clause = 214,            /* search_clause  */
  YYSYMBOL_where_clause = 215,             /* where_clause  */
  YYSYMBOL_having_clause = 216,            /* having_clause  */
  YYSYMBOL_group_by_clause = 217,          /* group_by_clause  */
  YYSYMBOL_set_operator = 218,             /* set_operator  */
  YYSYMBOL_table_reference = 219,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 220,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 221,     /* table_reference_name  */
  YYSYMBOL_table_name = 222,               /* table_name  */
  YYSYMBOL_table_alias = 223,              /* table_alias  */
  YYSYMBOL_with_clause = 224,              /* with_clause  */
  YYSYMBOL_with_expr_list = 225,           /* with_expr_list  */
  YYSYMBOL_with_expr = 226,                /* with_expr  */
  YYSYMBOL_join_clause = 227,              /* join_clause  */
  YYSYMBOL_join_type = 228,                /* join_type  */
  YYSYMBOL_show_statement = 229,           /* show_statement  */
  YYSYMBOL_flush_statement = 230,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 231,       /* optimize_statement  */
  YYSYMBOL_command_statement = 232,        /* command_statement  */
  YYSYMBOL_expr_array = 233,               /* expr_array  */
  YYSYMBOL_expr_array_list = 234,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 235,               /* expr_alias  */
  YYSYMBOL_expr = 236,                     /* expr  */
  YYSYMBOL_operand = 237,                  /* operand  */
  YYSYMBOL_knn_expr = 238,                 /* knn_expr  */
  YYSYMBOL_match_expr = 239,               /* match_expr  */
  YYSYMBOL_query_expr = 240,               /* query_expr  */
  YYSYMBOL_fusion_expr = 241,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 242,         /* sub_search_array  */
  YYSYMBOL_function_expr = 243,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 244,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 245,             /* between_expr  */
  YYSYMBOL_in_expr = 246,                  /* in_expr  */
  YYSYMBOL_case_expr = 247,                /* case_expr  */
  YYSYMBOL_case_check_array = 248,         /* case_check_array  */
  YYSYMBOL_cast_expr = 249,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 250,            /* subquery_expr  */
  YYSYMBOL_column_expr = 251,              /* column_expr  */
  YYSYMBOL_constant_expr = 252,            /* constant_expr  */
  YYSYMBOL_array_expr = 253,               /* array_expr  */
  YYSYMBOL_long_array_expr = 254,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 255, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 256,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 257, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 258,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 259,         /* copy_option_list  */
  YYSYMBOL_copy_option = 260,              /* copy_option  */
  YYSYMBOL_file_path = 261,                /* file_path  */
  YYSYMBOL_if_exists = 262,                /* if_exists  */
  YYSYMBOL_if_not_exists = 263,            /* if_not_exists  */
  YYSYMBOL_semicolon = 264,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 265,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 266,    /* with_index_param_list  */
  YYSYMBOL_index_param_list = 267,         /* index_param_list  */
  YYSYMBOL_index_param = 268,              /* index_param  */
  YYSYMBOL_index_info_list = 269           /* index_info_list  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




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
#define YYFINAL  80
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   844

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  176
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  343
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  668

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   415


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
       2,     2,     2,     2,     2,     2,     2,   168,     2,     2,
     171,   172,   166,   164,   175,   165,   173,   167,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   174,
     162,   161,   163,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   169,     2,   170,     2,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158,   159,   160
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   466,   466,   470,   476,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   496,   497,   498,
     499,   500,   501,   502,   503,   504,   505,   506,   513,   530,
     545,   569,   585,   603,   632,   636,   642,   645,   651,   686,
     720,   721,   722,   723,   724,   725,   726,   727,   728,   729,
     730,   731,   732,   733,   734,   735,   736,   737,   738,   741,
     743,   744,   745,   746,   747,   748,   751,   752,   753,   754,
     755,   756,   757,   758,   759,   760,   761,   762,   763,   764,
     765,   766,   785,   789,   799,   802,   805,   808,   812,   817,
     824,   830,   840,   856,   868,   881,   884,   891,   897,   900,
     903,   906,   909,   912,   915,   918,   925,   938,   942,   947,
     960,   973,   988,  1003,  1018,  1041,  1082,  1127,  1130,  1133,
    1142,  1152,  1155,  1159,  1164,  1186,  1189,  1194,  1210,  1213,
    1217,  1221,  1226,  1232,  1235,  1238,  1242,  1246,  1248,  1252,
    1254,  1257,  1261,  1264,  1268,  1273,  1277,  1280,  1284,  1287,
    1291,  1294,  1298,  1301,  1304,  1307,  1315,  1318,  1333,  1333,
    1335,  1349,  1358,  1363,  1372,  1377,  1382,  1388,  1395,  1398,
    1402,  1405,  1410,  1422,  1429,  1443,  1446,  1449,  1452,  1455,
    1458,  1461,  1467,  1471,  1475,  1479,  1483,  1487,  1491,  1495,
    1506,  1517,  1529,  1542,  1557,  1561,  1565,  1573,  1588,  1594,
    1599,  1605,  1611,  1619,  1625,  1631,  1637,  1643,  1651,  1657,
    1668,  1672,  1677,  1681,  1708,  1714,  1718,  1719,  1720,  1721,
    1722,  1724,  1727,  1733,  1736,  1737,  1738,  1739,  1740,  1741,
    1742,  1743,  1745,  1895,  1903,  1914,  1920,  1929,  1935,  1945,
    1949,  1953,  1957,  1961,  1965,  1969,  1973,  1978,  1986,  1994,
    2003,  2010,  2017,  2024,  2031,  2038,  2046,  2054,  2062,  2070,
    2078,  2086,  2094,  2102,  2110,  2118,  2126,  2134,  2164,  2172,
    2181,  2189,  2198,  2206,  2212,  2219,  2225,  2232,  2237,  2244,
    2251,  2259,  2283,  2289,  2295,  2302,  2310,  2317,  2324,  2329,
    2339,  2344,  2349,  2354,  2359,  2364,  2369,  2372,  2375,  2378,
    2382,  2385,  2389,  2393,  2398,  2403,  2407,  2412,  2417,  2423,
    2429,  2435,  2441,  2447,  2453,  2459,  2465,  2471,  2477,  2483,
    2494,  2498,  2503,  2522,  2532,  2538,  2542,  2543,  2545,  2546,
    2548,  2549,  2561,  2569,  2574,  2577,  2581,  2585,  2590,  2595,
    2603,  2610,  2621,  2669
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
  "PREPARE", "DESCRIBE", "UNION", "ALL", "INTERSECT", "EXCEPT", "FLUSH",
  "USE", "OPTIMIZE", "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO",
  "VALUES", "AST", "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "FRAGMENT",
  "VIEW", "INDEX", "ANALYZE", "VIEWS", "DATABASES", "SEGMENT", "SEGMENTS",
  "BLOCK", "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT",
  "RIGHT", "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE",
  "ORDER", "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN",
  "FROM", "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END",
  "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
  "SMALLINT", "BIGINT", "HUGEINT", "CHAR", "VARCHAR", "FLOAT", "DOUBLE",
  "REAL", "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID",
  "POINT", "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB",
  "BITMAP", "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE",
  "NULLABLE", "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS",
  "MINUTE", "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS",
  "YEAR", "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN",
  "AND", "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "KNN", "USING",
  "SESSION", "GLOBAL", "OFF", "EXPORT", "PROFILE", "CONFIGS", "PROFILES",
  "STATUS", "SEARCH", "MATCH", "QUERY", "FUSION", "NUMBER", "'='", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'",
  "'.'", "';'", "','", "$accept", "input_pattern", "statement_list",
  "statement", "explainable_statement", "create_statement",
  "table_element_array", "table_element", "table_column", "column_type",
  "column_constraints", "column_constraint", "table_constraint",
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
  "command_statement", "expr_array", "expr_array_list", "expr_alias",
  "expr", "operand", "knn_expr", "match_expr", "query_expr", "fusion_expr",
  "sub_search_array", "function_expr", "conjunction_expr", "between_expr",
  "in_expr", "case_expr", "case_check_array", "cast_expr", "subquery_expr",
  "column_expr", "constant_expr", "array_expr", "long_array_expr",
  "unclosed_long_array_expr", "double_array_expr",
  "unclosed_double_array_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon",
  "if_not_exists_info", "with_index_param_list", "index_param_list",
  "index_param", "index_info_list", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-357)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-334)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      37,   231,    49,   348,    99,    47,    99,   125,   445,   136,
      77,    92,   122,    99,   130,   -21,   -58,   161,    -8,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,    51,  -357,  -357,
     173,  -357,  -357,  -357,  -357,   102,   102,   102,   102,    59,
      99,   115,   115,   115,   115,   115,    23,   205,    99,   196,
     207,   218,  -357,  -357,  -357,  -357,  -357,  -357,  -357,   213,
    -357,  -357,  -357,    73,    95,  -357,  -357,    99,   335,  -357,
    -357,  -357,  -357,  -357,   211,   116,  -357,   290,   147,   156,
    -357,    11,  -357,   280,  -357,  -357,    -3,   271,  -357,   282,
     284,   362,    99,    99,    99,   364,   315,   180,   312,   383,
      99,    99,    99,   396,   404,   416,   359,   419,   419,    54,
      65,  -357,  -357,  -357,  -357,  -357,  -357,  -357,    51,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,   438,  -357,   283,   130,
     419,  -357,  -357,  -357,  -357,    -3,  -357,  -357,  -357,   408,
     410,   401,   390,  -357,   -19,  -357,   180,  -357,    99,   463,
      35,  -357,  -357,  -357,  -357,  -357,   409,  -357,   307,   -45,
    -357,   408,  -357,  -357,   398,   399,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,   428,   173,  -357,  -357,
     313,   317,   319,  -357,  -357,   491,   437,   320,   326,   251,
     494,  -357,  -357,   493,   329,   331,   332,   333,   341,   490,
     490,  -357,   400,   305,   -18,  -357,    10,   528,  -357,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,   328,
    -357,  -357,    24,  -357,    43,  -357,   408,   408,   448,  -357,
     -58,     6,   473,   343,  -357,   -74,   360,  -357,    99,   408,
     416,  -357,   293,   361,   372,   524,   373,  -357,  -357,   197,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,   490,   375,   603,   459,   408,   408,    25,   114,
    -357,   491,  -357,   540,   408,   543,   544,   546,   192,   192,
    -357,  -357,   379,   -57,     4,   408,   397,   549,   408,   408,
     -26,   384,   -14,   490,   490,   490,   490,   490,   490,   490,
     490,   490,   490,   490,   490,   490,   490,     5,  -357,   554,
    -357,   556,   387,  -357,    20,   293,   408,  -357,    51,   688,
     449,   392,   -17,  -357,  -357,  -357,   -58,   463,   405,  -357,
     566,   408,   406,  -357,   293,  -357,   381,   381,  -357,  -357,
     408,  -357,   120,   459,   429,   403,    14,   -63,   128,  -357,
     408,   408,   508,    66,   407,   132,   151,  -357,  -357,   -58,
     411,   466,  -357,    70,  -357,  -357,   106,   359,  -357,  -357,
     450,   413,   490,   305,   471,  -357,   374,   374,    80,    80,
     568,   374,   374,    80,    80,   192,   192,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,   408,  -357,  -357,  -357,   293,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,   420,  -357,  -357,  -357,
    -357,   421,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,   422,   427,   162,   433,   463,  -357,     6,    51,
     165,   463,  -357,   167,   434,   586,   597,  -357,   169,  -357,
     174,   175,  -357,   435,  -357,   688,   408,  -357,   408,   -44,
     -48,   490,   443,   621,  -357,   622,  -357,   623,    -9,     4,
     575,  -357,  -357,  -357,  -357,  -357,  -357,   576,  -357,   627,
    -357,  -357,  -357,  -357,  -357,   457,   585,   305,   374,   467,
     215,  -357,   490,  -357,   629,   632,   212,   342,   523,   526,
    -357,  -357,   162,  -357,   463,   219,  -357,   495,   221,  -357,
     408,  -357,  -357,  -357,   381,  -357,  -357,  -357,   472,   293,
     -40,  -357,   408,   516,   468,  -357,  -357,   223,   485,   488,
      70,   466,     4,     4,   474,   106,   618,   619,   496,   228,
    -357,  -357,   603,   499,   229,   498,   500,   501,   510,   512,
     522,   525,   531,   533,   534,   536,   537,   538,   539,   541,
     542,  -357,  -357,  -357,   262,  -357,   671,   551,   273,  -357,
    -357,  -357,   293,  -357,   695,  -357,   697,  -357,  -357,  -357,
    -357,   657,   463,  -357,  -357,  -357,  -357,   408,   408,  -357,
    -357,  -357,  -357,  -357,   712,   713,   714,   716,   717,   718,
     719,   720,   721,   722,   735,   736,   737,   738,   740,   741,
     742,  -357,   675,   747,  -357,   577,   579,   408,   277,   578,
     293,   582,   583,   584,   587,   588,   589,   590,   591,   625,
     626,   630,   631,   633,   634,   635,   636,   637,   624,  -357,
     675,   753,  -357,   293,  -357,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,  -357,
    -357,  -357,   755,  -357,   638,   640,   278,  -357,   788,   330,
    -357,   755,   639,  -357,  -357,  -357,  -357,  -357
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     169,     0,     0,     0,     0,     0,     0,     0,   105,     0,
       0,     0,     0,     0,     0,     0,   169,     0,   331,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   118,   117,
       0,     8,    14,    15,    16,   329,   329,   329,   329,   329,
       0,   327,   327,   327,   327,   327,   162,     0,     0,     0,
       0,     0,    99,   103,   100,   101,   102,   104,    98,   169,
     183,   184,   182,     0,     0,   185,   186,     0,   189,   194,
     195,   196,   198,   197,     0,   168,   170,     0,     0,     0,
       1,   169,     2,   152,   154,   155,     0,   141,   123,   129,
       0,     0,     0,     0,     0,     0,     0,    96,     0,     0,
       0,     0,     0,     0,     0,     0,   147,     0,     0,     0,
       0,    97,    17,    22,    24,    23,    18,    19,    21,    20,
      25,    26,    27,   187,   188,   193,     0,   190,     0,     0,
       0,   122,   121,     4,   153,     0,   119,   120,   140,     0,
       0,   137,     0,    28,     0,    29,    96,   332,     0,     0,
     169,   326,   110,   112,   111,   113,     0,   163,     0,   147,
     107,     0,    92,   325,     0,     0,   202,   204,   203,   200,
     201,   207,   209,   208,   205,   206,   191,     0,   171,   199,
       0,     0,   286,   290,   293,   294,     0,     0,     0,     0,
       0,   291,   292,     0,     0,     0,     0,     0,     0,     0,
       0,   288,     0,   169,   143,   210,   215,   216,   228,   229,
     230,   231,   225,   220,   219,   218,   226,   227,   217,   224,
     223,   298,     0,   299,     0,   297,     0,     0,   139,   328,
     169,     0,     0,     0,    90,     0,     0,    94,     0,     0,
       0,   106,   146,     0,     0,     0,     0,   126,   125,     0,
     309,   308,   311,   310,   313,   312,   315,   314,   317,   316,
     319,   318,     0,     0,   252,   169,     0,     0,     0,     0,
     295,     0,   296,     0,     0,     0,     0,     0,   254,   253,
     306,   303,     0,     0,     0,     0,   145,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   302,     0,
     305,     0,   128,   130,   135,   136,     0,   124,    31,     0,
       0,     0,     0,    34,    36,    37,   169,     0,    33,    95,
       0,     0,    93,   114,   109,   108,     0,     0,   192,   172,
       0,   247,     0,   169,     0,     0,     0,     0,     0,   277,
       0,     0,     0,     0,     0,     0,     0,   222,   221,   169,
     142,   156,   158,   167,   159,   211,     0,   147,   214,   270,
     271,     0,     0,   169,     0,   251,   261,   262,   265,   266,
       0,   268,   260,   263,   264,   256,   255,   257,   258,   259,
     287,   289,   304,   307,     0,   133,   134,   132,   138,    40,
      43,    44,    41,    42,    45,    46,    60,    62,    47,    49,
      48,    65,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     0,     0,    38,     0,     0,    30,     0,    32,
       0,     0,    91,     0,     0,     0,     0,   324,     0,   320,
       0,     0,   248,     0,   282,     0,     0,   275,     0,     0,
       0,     0,     0,     0,   235,     0,   237,     0,     0,     0,
       0,   176,   177,   178,   179,   175,   180,     0,   165,     0,
     160,   239,   240,   241,   242,   144,   151,   169,   269,     0,
       0,   250,     0,   131,     0,     0,     0,     0,     0,     0,
      85,    86,    39,    82,     0,     0,    35,     0,     0,   212,
       0,   323,   322,   116,     0,   115,   249,   283,     0,   279,
       0,   278,     0,     0,     0,   300,   301,     0,     0,     0,
     167,   157,     0,     0,   164,     0,     0,   149,     0,     0,
     284,   273,   272,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    87,    84,    83,     0,    89,     0,     0,     0,   321,
     281,   276,   280,   267,     0,   233,     0,   236,   238,   161,
     173,     0,     0,   243,   244,   245,   246,     0,     0,   127,
     285,   274,    61,    64,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    88,   335,     0,   213,     0,     0,     0,     0,   150,
     148,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   342,
     335,     0,   234,   174,   166,    63,    69,    70,    67,    68,
      71,    72,    73,    66,    77,    78,    75,    76,    79,    80,
      81,    74,     0,   343,     0,   338,     0,   336,     0,     0,
     334,     0,     0,   339,   341,   340,   337,   232
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -357,  -357,  -357,   723,  -357,   751,  -357,   386,  -357,   367,
    -357,   323,  -357,  -321,   757,   758,   672,  -357,  -357,   760,
    -357,   580,   762,   763,   -56,   807,   -16,   647,   690,   -46,
    -357,  -357,   432,  -357,  -357,  -357,  -357,  -357,  -357,  -155,
    -357,  -357,  -357,  -357,   368,   -84,    84,   308,  -357,  -357,
     700,  -357,  -357,   771,   772,   773,   774,  -247,  -357,   550,
    -160,  -158,  -356,  -353,  -336,  -333,  -357,  -357,  -357,  -357,
    -357,  -357,   569,  -357,  -357,  -357,  -357,  -357,   382,  -357,
     388,  -357,   643,   502,   334,   -52,   238,   276,  -357,  -357,
     214,  -357,   181,  -357
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   111,    20,   322,   323,   324,   424,
     492,   493,   325,   235,    21,    22,   150,    23,    59,    24,
     159,   160,    25,    26,    27,    28,    29,    88,   136,    89,
     141,   312,   313,   397,   228,   317,   139,   286,   367,   162,
     579,   527,    86,   360,   361,   362,   363,   470,    30,    75,
      76,   364,   467,    31,    32,    33,    34,   204,   332,   205,
     206,   207,   208,   209,   210,   211,   475,   212,   213,   214,
     215,   216,   269,   217,   218,   219,   220,   514,   221,   222,
     223,   224,   225,   438,   439,   164,    99,    91,    82,    96,
     629,   656,   657,   328
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,   242,   342,   118,   241,    87,   430,    46,   390,   319,
     471,  -330,    83,   472,    84,    85,    14,   161,     1,   446,
       2,     3,     4,     5,     6,     7,     8,     9,   264,   268,
     473,    10,   230,   474,   512,   511,    11,    12,    13,   561,
     137,   278,   279,   283,     1,   371,     2,     3,     4,     5,
       6,     7,     8,     9,   284,   374,   165,    10,   166,   167,
     168,   287,    11,    12,    13,   445,   314,   315,   236,   171,
     172,   173,    83,   468,    84,    85,   288,   289,   179,   334,
      46,    40,   288,   289,   433,    14,   395,   396,    47,   181,
      49,   288,   289,   441,    68,   288,   289,    73,   329,   288,
     289,   330,    46,   375,   264,   495,   346,   347,   267,    14,
     498,    14,   169,    16,   353,   358,   372,  -333,    67,    48,
     320,   469,   321,   174,    97,    72,   480,    90,   369,   370,
     240,    77,   106,    74,   237,   376,   377,   378,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   288,
     289,   125,   231,   288,   289,   427,   398,   285,   428,   288,
     289,    80,    15,   520,   288,   289,    81,    60,   135,   573,
      90,   391,   574,   554,   318,   359,   144,   145,   146,    61,
      62,    87,    16,    98,   153,   154,   155,   282,    15,   575,
     449,   450,   576,   349,   308,   350,   104,   351,   292,   309,
     182,   183,   184,   185,   170,   288,   289,   447,    16,   448,
     109,   351,   476,   310,   478,   175,  -334,  -334,   311,   105,
       1,   110,     2,     3,     4,     5,     6,     7,   123,     9,
     529,   488,   233,    10,   314,    69,    70,    71,    11,    12,
      13,   452,  -334,  -334,   302,   303,   304,   305,   306,   345,
     124,   608,   195,   558,   182,   183,   184,   185,   340,    35,
      36,    37,   128,   196,   197,   198,   186,   187,   107,   108,
     429,    38,    39,    50,    51,   188,   489,   189,   490,   491,
     100,   101,   102,   103,    63,    64,   509,    14,   510,    65,
      66,   129,   442,   513,   190,   285,   130,   535,   536,   537,
     538,   539,   134,   458,   454,   540,   541,   455,   182,   183,
     184,   185,    92,    93,    94,    95,   191,   192,   193,   131,
     186,   187,   333,   456,   532,   542,   457,   443,   132,   188,
     609,   189,   138,   663,   267,   664,   665,   497,   194,   499,
     330,   503,   285,   195,   504,   140,   505,   506,   190,   504,
     285,   149,   562,   142,   196,   197,   198,   479,   304,   305,
     306,   199,   200,   201,    15,   143,   202,   147,   203,   341,
     191,   192,   193,   148,   186,   187,    41,    42,    43,    14,
     126,   127,   151,   188,    16,   189,   152,   531,    44,    45,
     285,   555,   194,   557,   330,   565,   330,   195,   566,   156,
     581,   583,   190,   285,   584,   280,   281,   157,   196,   197,
     198,   182,   183,   184,   185,   199,   200,   201,   610,   158,
     202,   161,   203,   163,   191,   192,   193,   543,   544,   545,
     546,   547,   288,   289,   601,   548,   549,   330,   570,   571,
     182,   183,   184,   185,   176,   604,   194,   633,   285,   634,
     660,   195,   330,   661,   177,   550,   435,   436,   437,   226,
     229,   528,   196,   197,   198,   227,   234,   238,   239,   199,
     200,   201,   243,   244,   202,   245,   203,   186,   187,    52,
      53,    54,    55,    56,    57,   247,   188,    58,   189,   248,
     249,   265,   292,   182,   183,   184,   185,   266,   270,   271,
     273,   307,   274,   275,   276,   190,   262,   263,  -334,  -334,
     295,   296,   277,   316,   327,   188,  -334,   189,   460,  -181,
     461,   462,   463,   464,   326,   465,   466,   191,   192,   193,
     338,   331,   336,    14,   190,  -334,   300,   301,   302,   303,
     304,   305,   306,   337,   352,   339,   343,   354,   355,   194,
     356,   357,   368,   366,   195,   373,   191,   192,   193,   262,
     392,   393,   394,   426,   425,   196,   197,   198,   188,   432,
     189,   372,   199,   200,   201,   444,   431,   202,   194,   203,
     451,   434,   453,   195,   477,   344,   459,   190,   481,   288,
     501,   484,   485,   486,   196,   197,   198,   290,   487,   291,
     502,   199,   200,   201,   494,   500,   202,   507,   203,   191,
     192,   193,   202,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   259,   260,   261,   517,   518,   519,   522,   523,
     524,   194,   525,   526,   292,   533,   195,   344,   534,   530,
     551,   552,   556,   564,   560,   572,   292,   196,   197,   198,
     293,   294,   295,   296,   199,   200,   201,   567,   298,   202,
     568,   203,   293,   294,   295,   296,   297,   577,   580,   578,
     298,   582,   344,   585,   602,   586,   587,   299,   300,   301,
     302,   303,   304,   305,   306,   588,   292,   589,   563,   299,
     300,   301,   302,   303,   304,   305,   306,   590,   603,   605,
     591,   606,   293,   294,   295,   296,   592,   482,   593,   594,
     298,   595,   596,   597,   598,   607,   599,   600,   611,   612,
     613,   292,   614,   615,   616,   617,   618,   619,   620,   299,
     300,   301,   302,   303,   304,   305,   306,   293,   294,   295,
     296,   621,   622,   623,   624,   298,   625,   626,   627,   628,
     630,   632,   631,   285,   635,   636,   637,   654,   655,   638,
     639,   640,   641,   642,   299,   300,   301,   302,   303,   304,
     305,   306,   399,   400,   401,   402,   403,   404,   405,   406,
     407,   408,   409,   410,   411,   412,   413,   414,   415,   416,
     417,   418,   419,   420,   662,   652,   421,   643,   644,   422,
     423,   659,   645,   646,   133,   647,   648,   649,   650,   651,
     112,   667,   508,   658,   496,   553,   113,   114,   232,   115,
     335,   116,   117,    78,   246,   180,   483,   521,   569,   178,
     119,   120,   121,   122,   515,   365,   272,   348,   559,   440,
     516,     0,   666,     0,   653
};

static const yytype_int16 yycheck[] =
{
      16,   161,   249,    59,   159,     8,   327,     3,     3,     3,
     366,     0,    21,   366,    23,    24,    74,    62,     7,    82,
       9,    10,    11,    12,    13,    14,    15,    16,   186,   189,
     366,    20,    51,   366,    82,    79,    25,    26,    27,    79,
      86,   199,   200,   203,     7,    71,     9,    10,    11,    12,
      13,    14,    15,    16,    72,    69,   108,    20,     4,     5,
       6,    51,    25,    26,    27,    51,   226,   227,    33,     4,
       5,     6,    21,     3,    23,    24,   139,   140,   130,   239,
       3,    32,   139,   140,   331,    74,    66,    67,     4,   135,
       6,   139,   140,   340,    10,   139,   140,    13,   172,   139,
     140,   175,     3,   117,   262,   426,   266,   267,    83,    74,
     431,    74,    58,   171,   274,   172,   142,    58,    41,    72,
     114,    51,   116,    58,    40,     3,   373,    68,   288,   289,
     175,   152,    48,     3,   150,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   139,
     140,    67,   171,   139,   140,   172,   316,   175,   175,   139,
     140,     0,   151,   172,   139,   140,   174,    31,   171,   525,
      68,   166,   525,   494,   230,   171,    92,    93,    94,    43,
      44,     8,   171,    68,   100,   101,   102,   203,   151,   525,
     350,   351,   525,    79,   170,    81,   173,    83,   118,   175,
       3,     4,     5,     6,   150,   139,   140,    79,   171,    81,
       3,    83,   367,   170,   372,   150,   136,   137,   175,    14,
       7,     3,     9,    10,    11,    12,    13,    14,   155,    16,
     477,    69,   148,    20,   394,   143,   144,   145,    25,    26,
      27,   175,   162,   163,   164,   165,   166,   167,   168,   265,
     155,   572,   146,   500,     3,     4,     5,     6,    61,    28,
      29,    30,    51,   157,   158,   159,    69,    70,    72,    73,
     326,    40,    41,   148,   149,    78,   114,    80,   116,   117,
      42,    43,    44,    45,   148,   149,   446,    74,   448,   153,
     154,   175,   172,   451,    97,   175,     6,    85,    86,    87,
      88,    89,    22,   359,   172,    93,    94,   175,     3,     4,
       5,     6,    36,    37,    38,    39,   119,   120,   121,   172,
      69,    70,   238,   172,   482,   113,   175,   343,   172,    78,
     577,    80,    61,     3,    83,     5,     6,   172,   141,   172,
     175,   172,   175,   146,   175,    63,   172,   172,    97,   175,
     175,   171,   512,    69,   157,   158,   159,   373,   166,   167,
     168,   164,   165,   166,   151,     3,   169,     3,   171,   172,
     119,   120,   121,    58,    69,    70,    28,    29,    30,    74,
      45,    46,    70,    78,   171,    80,     3,   172,    40,    41,
     175,   172,   141,   172,   175,   172,   175,   146,   175,     3,
     172,   172,    97,   175,   175,     5,     6,     3,   157,   158,
     159,     3,     4,     5,     6,   164,   165,   166,   578,     3,
     169,    62,   171,     4,   119,   120,   121,    85,    86,    87,
      88,    89,   139,   140,   172,    93,    94,   175,   522,   523,
       3,     4,     5,     6,     6,   172,   141,   607,   175,   172,
     172,   146,   175,   175,   171,   113,    75,    76,    77,    49,
      70,   477,   157,   158,   159,    64,     3,    58,   161,   164,
     165,   166,    74,    74,   169,    47,   171,    69,    70,    34,
      35,    36,    37,    38,    39,   172,    78,    42,    80,   172,
     171,   171,   118,     3,     4,     5,     6,   171,     4,     6,
     171,   173,   171,   171,   171,    97,    69,    70,   134,   135,
     136,   137,   171,    65,   171,    78,   142,    80,    52,    53,
      54,    55,    56,    57,    51,    59,    60,   119,   120,   121,
       6,   171,   171,    74,    97,   161,   162,   163,   164,   165,
     166,   167,   168,   171,     4,   172,   171,     4,     4,   141,
       4,   172,     3,   156,   146,   171,   119,   120,   121,    69,
       6,     5,   175,   171,   115,   157,   158,   159,    78,     3,
      80,   142,   164,   165,   166,   172,   171,   169,   141,   171,
      72,   175,   175,   146,   171,    69,   175,    97,   117,   139,
       4,   171,   171,   171,   157,   158,   159,    69,   171,    71,
       3,   164,   165,   166,   171,   171,   169,   172,   171,   119,
     120,   121,   169,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,     4,     4,     4,    53,    53,
       3,   141,   175,    48,   118,     6,   146,    69,     6,   172,
     117,   115,   147,   175,   172,   171,   118,   157,   158,   159,
     134,   135,   136,   137,   164,   165,   166,   172,   142,   169,
     172,   171,   134,   135,   136,   137,   138,    49,   172,    50,
     142,   172,    69,   175,     3,   175,   175,   161,   162,   163,
     164,   165,   166,   167,   168,   175,   118,   175,   172,   161,
     162,   163,   164,   165,   166,   167,   168,   175,   147,     4,
     175,     4,   134,   135,   136,   137,   175,   139,   175,   175,
     142,   175,   175,   175,   175,    58,   175,   175,     6,     6,
       6,   118,     6,     6,     6,     6,     6,     6,     6,   161,
     162,   163,   164,   165,   166,   167,   168,   134,   135,   136,
     137,     6,     6,     6,     6,   142,     6,     6,     6,    74,
       3,   172,   175,   175,   172,   172,   172,     4,     3,   172,
     172,   172,   172,   172,   161,   162,   163,   164,   165,   166,
     167,   168,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,     6,   171,   108,   172,   172,   111,
     112,   161,   172,   172,    81,   172,   172,   172,   172,   172,
      59,   172,   445,   175,   428,   492,    59,    59,   146,    59,
     240,    59,    59,    16,   177,   135,   394,   459,   520,   129,
      59,    59,    59,    59,   452,   285,   193,   268,   504,   337,
     452,    -1,   661,    -1,   630
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      20,    25,    26,    27,    74,   151,   171,   177,   178,   179,
     181,   190,   191,   193,   195,   198,   199,   200,   201,   202,
     224,   229,   230,   231,   232,    28,    29,    30,    40,    41,
      32,    28,    29,    30,    40,    41,     3,   222,    72,   222,
     148,   149,    34,    35,    36,    37,    38,    39,    42,   194,
      31,    43,    44,   148,   149,   153,   154,    41,   222,   143,
     144,   145,     3,   222,     3,   225,   226,   152,   201,   202,
       0,   174,   264,    21,    23,    24,   218,     8,   203,   205,
      68,   263,   263,   263,   263,   263,   265,   222,    68,   262,
     262,   262,   262,   262,   173,    14,   222,    72,    73,     3,
       3,   180,   181,   190,   191,   195,   198,   199,   200,   229,
     230,   231,   232,   155,   155,   222,    45,    46,    51,   175,
       6,   172,   172,   179,    22,   171,   204,   205,    61,   212,
      63,   206,    69,     3,   222,   222,   222,     3,    58,   171,
     192,    70,     3,   222,   222,   222,     3,     3,     3,   196,
     197,    62,   215,     4,   261,   261,     4,     5,     6,    58,
     150,     4,     5,     6,    58,   150,     6,   171,   226,   261,
     204,   205,     3,     4,     5,     6,    69,    70,    78,    80,
      97,   119,   120,   121,   141,   146,   157,   158,   159,   164,
     165,   166,   169,   171,   233,   235,   236,   237,   238,   239,
     240,   241,   243,   244,   245,   246,   247,   249,   250,   251,
     252,   254,   255,   256,   257,   258,    49,    64,   210,    70,
      51,   171,   192,   222,     3,   189,    33,   202,    58,   161,
     175,   215,   236,    74,    74,    47,   203,   172,   172,   171,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,    69,    70,   237,   171,   171,    83,   236,   248,
       4,     6,   258,   171,   171,   171,   171,   171,   237,   237,
       5,     6,   202,   236,    72,   175,   213,    51,   139,   140,
      69,    71,   118,   134,   135,   136,   137,   138,   142,   161,
     162,   163,   164,   165,   166,   167,   168,   173,   170,   175,
     170,   175,   207,   208,   236,   236,    65,   211,   200,     3,
     114,   116,   182,   183,   184,   188,    51,   171,   269,   172,
     175,   171,   234,   222,   236,   197,   171,   171,     6,   172,
      61,   172,   233,   171,    69,   202,   236,   236,   248,    79,
      81,    83,     4,   236,     4,     4,     4,   172,   172,   171,
     219,   220,   221,   222,   227,   235,   156,   214,     3,   236,
     236,    71,   142,   171,    69,   117,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
       3,   166,     6,     5,   175,    66,    67,   209,   236,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   108,   111,   112,   185,   115,   171,   172,   175,   200,
     189,   171,     3,   233,   175,    75,    76,    77,   259,   260,
     259,   233,   172,   202,   172,    51,    82,    79,    81,   236,
     236,    72,   175,   175,   172,   175,   172,   175,   200,   175,
      52,    54,    55,    56,    57,    59,    60,   228,     3,    51,
     223,   238,   239,   240,   241,   242,   215,   171,   237,   202,
     233,   117,   139,   208,   171,   171,   171,   171,    69,   114,
     116,   117,   186,   187,   171,   189,   183,   172,   189,   172,
     171,     4,     3,   172,   175,   172,   172,   172,   185,   236,
     236,    79,    82,   237,   253,   254,   256,     4,     4,     4,
     172,   220,    53,    53,     3,   175,    48,   217,   202,   233,
     172,   172,   237,     6,     6,    85,    86,    87,    88,    89,
      93,    94,   113,    85,    86,    87,    88,    89,    93,    94,
     113,   117,   115,   187,   189,   172,   147,   172,   233,   260,
     172,    79,   236,   172,   175,   172,   175,   172,   172,   223,
     221,   221,   171,   238,   239,   240,   241,    49,    50,   216,
     172,   172,   172,   172,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   172,     3,   147,   172,     4,     4,    58,   189,   233,
     236,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,    74,   266,
       3,   175,   172,   236,   172,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   172,   171,   266,     4,     3,   267,   268,   175,   161,
     172,   175,     6,     3,     5,     6,   268,   172
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   176,   177,   178,   178,   179,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   181,   181,
     181,   181,   181,   181,   182,   182,   183,   183,   184,   184,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   186,   186,   187,   187,   187,   187,   188,   188,
     189,   189,   190,   191,   191,   192,   192,   193,   194,   194,
     194,   194,   194,   194,   194,   194,   195,   196,   196,   197,
     198,   198,   198,   198,   198,   199,   199,   200,   200,   200,
     200,   201,   201,   202,   203,   204,   204,   205,   206,   206,
     207,   207,   208,   209,   209,   209,   210,   210,   211,   211,
     212,   212,   213,   213,   214,   214,   215,   215,   216,   216,
     217,   217,   218,   218,   218,   218,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   223,   223,   223,   224,   224,
     225,   225,   226,   227,   227,   228,   228,   228,   228,   228,
     228,   228,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   230,   230,   230,   231,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     233,   233,   234,   234,   235,   235,   236,   236,   236,   236,
     236,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   238,   239,   239,   240,   240,   241,   241,   242,
     242,   242,   242,   242,   242,   242,   242,   243,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     244,   244,   245,   246,   246,   247,   247,   247,   247,   248,
     248,   249,   250,   250,   250,   250,   251,   251,   251,   251,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     253,   253,   254,   255,   255,   256,   257,   257,   258,   258,
     258,   258,   258,   258,   258,   258,   258,   258,   258,   258,
     259,   259,   260,   260,   260,   261,   262,   262,   263,   263,
     264,   264,   265,   265,   266,   266,   267,   267,   268,   268,
     268,   268,   269,   269
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       7,     6,     7,     6,     1,     3,     1,     1,     2,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     6,     4,     1,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     1,     2,     2,     1,     1,     2,     5,     4,
       1,     3,     4,     6,     5,     3,     0,     3,     1,     1,
       1,     1,     1,     1,     1,     0,     5,     1,     3,     3,
       4,     4,     4,     4,     6,     8,     8,     1,     1,     3,
       3,     3,     3,     2,     4,     3,     3,     8,     3,     0,
       1,     3,     2,     1,     1,     0,     2,     0,     2,     0,
       1,     0,     2,     0,     2,     0,     2,     0,     2,     0,
       3,     0,     1,     2,     1,     1,     1,     3,     1,     1,
       2,     4,     1,     3,     2,     1,     5,     0,     2,     0,
       1,     3,     5,     4,     6,     1,     1,     1,     1,     1,
       1,     0,     2,     2,     2,     2,     2,     3,     3,     2,
       3,     4,     6,     3,     2,     2,     2,     2,     2,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
       1,     3,     3,     5,     3,     1,     1,     1,     1,     1,
       1,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,    12,     6,     8,     4,     6,     4,     6,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     4,     5,
       4,     3,     2,     2,     2,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     6,     3,     4,
       3,     3,     5,     5,     6,     4,     6,     3,     5,     4,
       5,     6,     4,     5,     5,     6,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     3,     2,     2,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     3,     2,     2,     1,     1,     2,     0,     3,     0,
       1,     0,     2,     0,     4,     0,     1,     3,     1,     3,
       3,     3,     6,     7
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
#line 298 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1984 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 298 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1992 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 212 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 2006 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 202 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2020 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2031 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2040 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2049 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 262 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2063 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2074 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2084 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2094 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2104 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2114 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2124 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2134 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 245 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2148 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 245 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2162 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 325 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2172 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2180 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2188 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2197 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2205 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2213 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2221 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 222 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2235 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2244 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2253 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2262 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 282 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2275 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 315 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2284 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 272 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2298 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 272 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2312 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2322 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2331 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 222 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2345 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 232 "parser.y"
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
#line 2362 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2370 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2378 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2386 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2394 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2402 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2410 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2418 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 222 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2432 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2440 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2448 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2456 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2464 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2472 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 343 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2485 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2493 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2501 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2509 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2517 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2525 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2533 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2541 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2549 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2557 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2565 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 298 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2573 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 195 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2584 "parser.cpp"
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 178 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
#line 2598 "parser.cpp"
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 168 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
#line 2612 "parser.cpp"
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
#line 85 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

#line 2720 "parser.cpp"

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
#line 466 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2935 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 470 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2946 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 476 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2957 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 483 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2963 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 484 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2969 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 485 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2975 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 486 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2981 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 487 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2987 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 488 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2993 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 489 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2999 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 490 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3005 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 491 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3011 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 492 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3017 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 493 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3023 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 494 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3029 "parser.cpp"
    break;

  case 17: /* explainable_statement: create_statement  */
#line 496 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3035 "parser.cpp"
    break;

  case 18: /* explainable_statement: drop_statement  */
#line 497 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3041 "parser.cpp"
    break;

  case 19: /* explainable_statement: copy_statement  */
#line 498 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3047 "parser.cpp"
    break;

  case 20: /* explainable_statement: show_statement  */
#line 499 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3053 "parser.cpp"
    break;

  case 21: /* explainable_statement: select_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3059 "parser.cpp"
    break;

  case 22: /* explainable_statement: delete_statement  */
#line 501 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3065 "parser.cpp"
    break;

  case 23: /* explainable_statement: update_statement  */
#line 502 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3071 "parser.cpp"
    break;

  case 24: /* explainable_statement: insert_statement  */
#line 503 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3077 "parser.cpp"
    break;

  case 25: /* explainable_statement: flush_statement  */
#line 504 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3083 "parser.cpp"
    break;

  case 26: /* explainable_statement: optimize_statement  */
#line 505 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3089 "parser.cpp"
    break;

  case 27: /* explainable_statement: command_statement  */
#line 506 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3095 "parser.cpp"
    break;

  case 28: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 513 "parser.y"
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
#line 3115 "parser.cpp"
    break;

  case 29: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 530 "parser.y"
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
#line 3133 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 545 "parser.y"
                                                                    {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateTableInfo> create_table_info = std::make_shared<infinity::CreateTableInfo>();
    if((yyvsp[-3].table_name_t)->schema_name_ptr_ != nullptr) {
        create_table_info->schema_name_ = (yyvsp[-3].table_name_t)->schema_name_ptr_;
        free((yyvsp[-3].table_name_t)->schema_name_ptr_);
    }
    create_table_info->table_name_ = (yyvsp[-3].table_name_t)->table_name_ptr_;
    free((yyvsp[-3].table_name_t)->table_name_ptr_);
    delete (yyvsp[-3].table_name_t);

    for (infinity::TableElement*& element : *(yyvsp[-1].table_element_array_t)) {
        if(element->type_ == infinity::TableElementType::kColumn) {
            create_table_info->column_defs_.emplace_back((infinity::ColumnDef*)element);
        } else {
            create_table_info->constraints_.emplace_back((infinity::TableConstraint*)element);
        }
    }
    delete (yyvsp[-1].table_element_array_t);

    (yyval.create_stmt)->create_info_ = create_table_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-4].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3161 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 569 "parser.y"
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
#line 3181 "parser.cpp"
    break;

  case 32: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 585 "parser.y"
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
#line 3202 "parser.cpp"
    break;

  case 33: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 603 "parser.y"
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
#line 3235 "parser.cpp"
    break;

  case 34: /* table_element_array: table_element  */
#line 632 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3244 "parser.cpp"
    break;

  case 35: /* table_element_array: table_element_array ',' table_element  */
#line 636 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3253 "parser.cpp"
    break;

  case 36: /* table_element: table_column  */
#line 642 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3261 "parser.cpp"
    break;

  case 37: /* table_element: table_constraint  */
#line 645 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3269 "parser.cpp"
    break;

  case 38: /* table_column: IDENTIFIER column_type  */
#line 651 "parser.y"
                       {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[0].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[0].column_type_t).precision, (yyvsp[0].column_type_t).scale);
            if(type_info_ptr == nullptr) {
                yyerror(&yyloc, scanner, result, "Fail to create decimal info.");
                delete (yyvsp[-1].str_value);
                YYERROR;
            }
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
//            break;
//        }
        case infinity::LogicalType::kEmbedding: {
            type_info_ptr = infinity::EmbeddingInfo::Make((yyvsp[0].column_type_t).embedding_type_, (yyvsp[0].column_type_t).width);
            break;
        }
        default: {
            break;
        }
    }
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[0].column_type_t).logical_type_, type_info_ptr);

    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3309 "parser.cpp"
    break;

  case 39: /* table_column: IDENTIFIER column_type column_constraints  */
#line 686 "parser.y"
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
//        case infinity::LogicalType::kBitmap: {
//            type_info_ptr = infinity::BitmapInfo::Make($2.width);
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
    (yyval.table_column_t) = new infinity::ColumnDef((yyvsp[-1].column_type_t).logical_type_, type_info_ptr);

    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.table_column_t)->name_ = (yyvsp[-2].str_value);
    (yyval.table_column_t)->constraints_ = *(yyvsp[0].column_constraints_t);
    delete (yyvsp[0].column_constraints_t);
    free((yyvsp[-2].str_value));
    /*
    if (!$$->trySetNullableExplicit()) {
        yyerror(&yyloc, result, scanner, ("Conflicting nullability constraints for " + std::string{$1}).c_str());
    }
    */
}
#line 3346 "parser.cpp"
    break;

  case 40: /* column_type: BOOLEAN  */
#line 720 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3352 "parser.cpp"
    break;

  case 41: /* column_type: TINYINT  */
#line 721 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3358 "parser.cpp"
    break;

  case 42: /* column_type: SMALLINT  */
#line 722 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3364 "parser.cpp"
    break;

  case 43: /* column_type: INTEGER  */
#line 723 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3370 "parser.cpp"
    break;

  case 44: /* column_type: INT  */
#line 724 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3376 "parser.cpp"
    break;

  case 45: /* column_type: BIGINT  */
#line 725 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3382 "parser.cpp"
    break;

  case 46: /* column_type: HUGEINT  */
#line 726 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3388 "parser.cpp"
    break;

  case 47: /* column_type: FLOAT  */
#line 727 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3394 "parser.cpp"
    break;

  case 48: /* column_type: REAL  */
#line 728 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3400 "parser.cpp"
    break;

  case 49: /* column_type: DOUBLE  */
#line 729 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3406 "parser.cpp"
    break;

  case 50: /* column_type: DATE  */
#line 730 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3412 "parser.cpp"
    break;

  case 51: /* column_type: TIME  */
#line 731 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3418 "parser.cpp"
    break;

  case 52: /* column_type: DATETIME  */
#line 732 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3424 "parser.cpp"
    break;

  case 53: /* column_type: TIMESTAMP  */
#line 733 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3430 "parser.cpp"
    break;

  case 54: /* column_type: UUID  */
#line 734 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3436 "parser.cpp"
    break;

  case 55: /* column_type: POINT  */
#line 735 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3442 "parser.cpp"
    break;

  case 56: /* column_type: LINE  */
#line 736 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3448 "parser.cpp"
    break;

  case 57: /* column_type: LSEG  */
#line 737 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3454 "parser.cpp"
    break;

  case 58: /* column_type: BOX  */
#line 738 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3460 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 741 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3466 "parser.cpp"
    break;

  case 60: /* column_type: CHAR  */
#line 743 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3472 "parser.cpp"
    break;

  case 61: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 744 "parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3478 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 745 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3484 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 746 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3490 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 747 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3496 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 748 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3502 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 751 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3508 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 752 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3514 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 753 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3520 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 754 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3526 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 755 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3532 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 756 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3538 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3544 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 758 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3550 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 759 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3556 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 760 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3562 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3568 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 762 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3574 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 763 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3580 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 764 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3586 "parser.cpp"
    break;

  case 80: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 765 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3592 "parser.cpp"
    break;

  case 81: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 766 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3598 "parser.cpp"
    break;

  case 82: /* column_constraints: column_constraint  */
#line 785 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3607 "parser.cpp"
    break;

  case 83: /* column_constraints: column_constraints column_constraint  */
#line 789 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3621 "parser.cpp"
    break;

  case 84: /* column_constraint: PRIMARY KEY  */
#line 799 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3629 "parser.cpp"
    break;

  case 85: /* column_constraint: UNIQUE  */
#line 802 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3637 "parser.cpp"
    break;

  case 86: /* column_constraint: NULLABLE  */
#line 805 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3645 "parser.cpp"
    break;

  case 87: /* column_constraint: NOT NULLABLE  */
#line 808 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3653 "parser.cpp"
    break;

  case 88: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 812 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3663 "parser.cpp"
    break;

  case 89: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 817 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3673 "parser.cpp"
    break;

  case 90: /* identifier_array: IDENTIFIER  */
#line 824 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3684 "parser.cpp"
    break;

  case 91: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 830 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3695 "parser.cpp"
    break;

  case 92: /* delete_statement: DELETE FROM table_name where_clause  */
#line 840 "parser.y"
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
#line 3712 "parser.cpp"
    break;

  case 93: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 856 "parser.y"
                                                                                          {
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
#line 3729 "parser.cpp"
    break;

  case 94: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 868 "parser.y"
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
#line 3746 "parser.cpp"
    break;

  case 95: /* optional_identifier_array: '(' identifier_array ')'  */
#line 881 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3754 "parser.cpp"
    break;

  case 96: /* optional_identifier_array: %empty  */
#line 884 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3762 "parser.cpp"
    break;

  case 97: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 891 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3772 "parser.cpp"
    break;

  case 98: /* explain_type: ANALYZE  */
#line 897 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3780 "parser.cpp"
    break;

  case 99: /* explain_type: AST  */
#line 900 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3788 "parser.cpp"
    break;

  case 100: /* explain_type: RAW  */
#line 903 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3796 "parser.cpp"
    break;

  case 101: /* explain_type: LOGICAL  */
#line 906 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3804 "parser.cpp"
    break;

  case 102: /* explain_type: PHYSICAL  */
#line 909 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3812 "parser.cpp"
    break;

  case 103: /* explain_type: PIPELINE  */
#line 912 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3820 "parser.cpp"
    break;

  case 104: /* explain_type: FRAGMENT  */
#line 915 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3828 "parser.cpp"
    break;

  case 105: /* explain_type: %empty  */
#line 918 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3836 "parser.cpp"
    break;

  case 106: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 925 "parser.y"
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
#line 3853 "parser.cpp"
    break;

  case 107: /* update_expr_array: update_expr  */
#line 938 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3862 "parser.cpp"
    break;

  case 108: /* update_expr_array: update_expr_array ',' update_expr  */
#line 942 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3871 "parser.cpp"
    break;

  case 109: /* update_expr: IDENTIFIER '=' expr  */
#line 947 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3883 "parser.cpp"
    break;

  case 110: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 960 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3899 "parser.cpp"
    break;

  case 111: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 973 "parser.y"
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
#line 3917 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP TABLE if_exists table_name  */
#line 988 "parser.y"
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
#line 3935 "parser.cpp"
    break;

  case 113: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1003 "parser.y"
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
#line 3953 "parser.cpp"
    break;

  case 114: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1018 "parser.y"
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
#line 3976 "parser.cpp"
    break;

  case 115: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1041 "parser.y"
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
#line 4022 "parser.cpp"
    break;

  case 116: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1082 "parser.y"
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
#line 4068 "parser.cpp"
    break;

  case 117: /* select_statement: select_without_paren  */
#line 1127 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4076 "parser.cpp"
    break;

  case 118: /* select_statement: select_with_paren  */
#line 1130 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4084 "parser.cpp"
    break;

  case 119: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1133 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4098 "parser.cpp"
    break;

  case 120: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1142 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4112 "parser.cpp"
    break;

  case 121: /* select_with_paren: '(' select_without_paren ')'  */
#line 1152 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4120 "parser.cpp"
    break;

  case 122: /* select_with_paren: '(' select_with_paren ')'  */
#line 1155 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4128 "parser.cpp"
    break;

  case 123: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1159 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4137 "parser.cpp"
    break;

  case 124: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1164 "parser.y"
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
#line 4163 "parser.cpp"
    break;

  case 125: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1186 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4171 "parser.cpp"
    break;

  case 126: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1189 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4179 "parser.cpp"
    break;

  case 127: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1194 "parser.y"
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
#line 4199 "parser.cpp"
    break;

  case 128: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1210 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4207 "parser.cpp"
    break;

  case 129: /* order_by_clause: %empty  */
#line 1213 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4215 "parser.cpp"
    break;

  case 130: /* order_by_expr_list: order_by_expr  */
#line 1217 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4224 "parser.cpp"
    break;

  case 131: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1221 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4233 "parser.cpp"
    break;

  case 132: /* order_by_expr: expr order_by_type  */
#line 1226 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4243 "parser.cpp"
    break;

  case 133: /* order_by_type: ASC  */
#line 1232 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4251 "parser.cpp"
    break;

  case 134: /* order_by_type: DESC  */
#line 1235 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4259 "parser.cpp"
    break;

  case 135: /* order_by_type: %empty  */
#line 1238 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4267 "parser.cpp"
    break;

  case 136: /* limit_expr: LIMIT expr  */
#line 1242 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4275 "parser.cpp"
    break;

  case 137: /* limit_expr: %empty  */
#line 1246 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4281 "parser.cpp"
    break;

  case 138: /* offset_expr: OFFSET expr  */
#line 1248 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4289 "parser.cpp"
    break;

  case 139: /* offset_expr: %empty  */
#line 1252 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4295 "parser.cpp"
    break;

  case 140: /* distinct: DISTINCT  */
#line 1254 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4303 "parser.cpp"
    break;

  case 141: /* distinct: %empty  */
#line 1257 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4311 "parser.cpp"
    break;

  case 142: /* from_clause: FROM table_reference  */
#line 1261 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4319 "parser.cpp"
    break;

  case 143: /* from_clause: %empty  */
#line 1264 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4327 "parser.cpp"
    break;

  case 144: /* search_clause: SEARCH sub_search_array  */
#line 1268 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4337 "parser.cpp"
    break;

  case 145: /* search_clause: %empty  */
#line 1273 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4345 "parser.cpp"
    break;

  case 146: /* where_clause: WHERE expr  */
#line 1277 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4353 "parser.cpp"
    break;

  case 147: /* where_clause: %empty  */
#line 1280 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4361 "parser.cpp"
    break;

  case 148: /* having_clause: HAVING expr  */
#line 1284 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4369 "parser.cpp"
    break;

  case 149: /* having_clause: %empty  */
#line 1287 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4377 "parser.cpp"
    break;

  case 150: /* group_by_clause: GROUP BY expr_array  */
#line 1291 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4385 "parser.cpp"
    break;

  case 151: /* group_by_clause: %empty  */
#line 1294 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4393 "parser.cpp"
    break;

  case 152: /* set_operator: UNION  */
#line 1298 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4401 "parser.cpp"
    break;

  case 153: /* set_operator: UNION ALL  */
#line 1301 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4409 "parser.cpp"
    break;

  case 154: /* set_operator: INTERSECT  */
#line 1304 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4417 "parser.cpp"
    break;

  case 155: /* set_operator: EXCEPT  */
#line 1307 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4425 "parser.cpp"
    break;

  case 156: /* table_reference: table_reference_unit  */
#line 1315 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4433 "parser.cpp"
    break;

  case 157: /* table_reference: table_reference ',' table_reference_unit  */
#line 1318 "parser.y"
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
#line 4451 "parser.cpp"
    break;

  case 160: /* table_reference_name: table_name table_alias  */
#line 1335 "parser.y"
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
#line 4469 "parser.cpp"
    break;

  case 161: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1349 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4480 "parser.cpp"
    break;

  case 162: /* table_name: IDENTIFIER  */
#line 1358 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4490 "parser.cpp"
    break;

  case 163: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1363 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4502 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER  */
#line 1372 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4512 "parser.cpp"
    break;

  case 165: /* table_alias: IDENTIFIER  */
#line 1377 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4522 "parser.cpp"
    break;

  case 166: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1382 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4533 "parser.cpp"
    break;

  case 167: /* table_alias: %empty  */
#line 1388 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4541 "parser.cpp"
    break;

  case 168: /* with_clause: WITH with_expr_list  */
#line 1395 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4549 "parser.cpp"
    break;

  case 169: /* with_clause: %empty  */
#line 1398 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4557 "parser.cpp"
    break;

  case 170: /* with_expr_list: with_expr  */
#line 1402 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4566 "parser.cpp"
    break;

  case 171: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1405 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4575 "parser.cpp"
    break;

  case 172: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1410 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4587 "parser.cpp"
    break;

  case 173: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1422 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4599 "parser.cpp"
    break;

  case 174: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1429 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4612 "parser.cpp"
    break;

  case 175: /* join_type: INNER  */
#line 1443 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4620 "parser.cpp"
    break;

  case 176: /* join_type: LEFT  */
#line 1446 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4628 "parser.cpp"
    break;

  case 177: /* join_type: RIGHT  */
#line 1449 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4636 "parser.cpp"
    break;

  case 178: /* join_type: OUTER  */
#line 1452 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4644 "parser.cpp"
    break;

  case 179: /* join_type: FULL  */
#line 1455 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4652 "parser.cpp"
    break;

  case 180: /* join_type: CROSS  */
#line 1458 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4660 "parser.cpp"
    break;

  case 181: /* join_type: %empty  */
#line 1461 "parser.y"
                {
}
#line 4667 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW DATABASES  */
#line 1467 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4676 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW TABLES  */
#line 1471 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4685 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW VIEWS  */
#line 1475 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4694 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW CONFIGS  */
#line 1479 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4703 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW PROFILES  */
#line 1483 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4712 "parser.cpp"
    break;

  case 187: /* show_statement: SHOW SESSION STATUS  */
#line 1487 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4721 "parser.cpp"
    break;

  case 188: /* show_statement: SHOW GLOBAL STATUS  */
#line 1491 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4730 "parser.cpp"
    break;

  case 189: /* show_statement: DESCRIBE table_name  */
#line 1495 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kColumns;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4746 "parser.cpp"
    break;

  case 190: /* show_statement: DESCRIBE table_name SEGMENTS  */
#line 1506 "parser.y"
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
#line 4762 "parser.cpp"
    break;

  case 191: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE  */
#line 1517 "parser.y"
                                         {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
    if((yyvsp[-2].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[-2].table_name_t)->schema_name_ptr_;
        free((yyvsp[-2].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[-2].table_name_t)->table_name_ptr_;
    free((yyvsp[-2].table_name_t)->table_name_ptr_);
    (yyval.show_stmt)->segment_id_ = (yyvsp[0].long_value);
    delete (yyvsp[-2].table_name_t);
}
#line 4779 "parser.cpp"
    break;

  case 192: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1529 "parser.y"
                                                          {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSegments;
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
#line 4797 "parser.cpp"
    break;

  case 193: /* show_statement: DESCRIBE INDEX table_name  */
#line 1542 "parser.y"
                            {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kIndexes;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.show_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.show_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4813 "parser.cpp"
    break;

  case 194: /* flush_statement: FLUSH DATA  */
#line 1557 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4822 "parser.cpp"
    break;

  case 195: /* flush_statement: FLUSH LOG  */
#line 1561 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4831 "parser.cpp"
    break;

  case 196: /* flush_statement: FLUSH BUFFER  */
#line 1565 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4840 "parser.cpp"
    break;

  case 197: /* optimize_statement: OPTIMIZE table_name  */
#line 1573 "parser.y"
                                        {
    (yyval.optimize_stmt) = new infinity::OptimizeStatement();
    (yyval.optimize_stmt)->type_ = infinity::OptimizeType::kIRS;
    if((yyvsp[0].table_name_t)->schema_name_ptr_ != nullptr) {
        (yyval.optimize_stmt)->schema_name_ = (yyvsp[0].table_name_t)->schema_name_ptr_;
        free((yyvsp[0].table_name_t)->schema_name_ptr_);
    }
    (yyval.optimize_stmt)->table_name_ = (yyvsp[0].table_name_t)->table_name_ptr_;
    free((yyvsp[0].table_name_t)->table_name_ptr_);
    delete (yyvsp[0].table_name_t);
}
#line 4856 "parser.cpp"
    break;

  case 198: /* command_statement: USE IDENTIFIER  */
#line 1588 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4867 "parser.cpp"
    break;

  case 199: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1594 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4877 "parser.cpp"
    break;

  case 200: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1599 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4888 "parser.cpp"
    break;

  case 201: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1605 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4899 "parser.cpp"
    break;

  case 202: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1611 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4912 "parser.cpp"
    break;

  case 203: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1619 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4923 "parser.cpp"
    break;

  case 204: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1625 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4934 "parser.cpp"
    break;

  case 205: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1631 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4945 "parser.cpp"
    break;

  case 206: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1637 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4956 "parser.cpp"
    break;

  case 207: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1643 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4969 "parser.cpp"
    break;

  case 208: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1651 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4980 "parser.cpp"
    break;

  case 209: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1657 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4991 "parser.cpp"
    break;

  case 210: /* expr_array: expr_alias  */
#line 1668 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5000 "parser.cpp"
    break;

  case 211: /* expr_array: expr_array ',' expr_alias  */
#line 1672 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5009 "parser.cpp"
    break;

  case 212: /* expr_array_list: '(' expr_array ')'  */
#line 1677 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5018 "parser.cpp"
    break;

  case 213: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1681 "parser.y"
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
#line 5038 "parser.cpp"
    break;

  case 214: /* expr_alias: expr AS IDENTIFIER  */
#line 1708 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5049 "parser.cpp"
    break;

  case 215: /* expr_alias: expr  */
#line 1714 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5057 "parser.cpp"
    break;

  case 221: /* operand: '(' expr ')'  */
#line 1724 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5065 "parser.cpp"
    break;

  case 222: /* operand: '(' select_without_paren ')'  */
#line 1727 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5076 "parser.cpp"
    break;

  case 223: /* operand: constant_expr  */
#line 1733 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5084 "parser.cpp"
    break;

  case 232: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')'  */
#line 1745 "parser.y"
                                                                                {
    infinity::KnnExpr* knn_expr = new infinity::KnnExpr();
    (yyval.expr_t) = knn_expr;

    // KNN search column
    knn_expr->column_expr_ = (yyvsp[-9].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    if(strcmp((yyvsp[-3].str_value), "l2") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-3].str_value), "ip") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-3].str_value), "cosine") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-3].str_value), "hamming") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
    } else {
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn distance type");
        YYERROR;
    }

    // KNN data type
    ParserHelper::ToLower((yyvsp[-5].str_value));
    if(strcmp((yyvsp[-5].str_value), "float") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-7].const_expr_t)->double_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->double_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-7].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-7].const_expr_t)->long_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-7].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
    } else if(strcmp((yyvsp[-5].str_value), "tinyint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_]{};

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((char*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-7].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
    } else if(strcmp((yyvsp[-5].str_value), "smallint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        knn_expr->embedding_data_ptr_ = new short int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((short int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-7].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
    } else if(strcmp((yyvsp[-5].str_value), "integer") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        knn_expr->embedding_data_ptr_ = new int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-7].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);

    } else if(strcmp((yyvsp[-5].str_value), "bigint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        knn_expr->embedding_data_ptr_ = new long[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-7].const_expr_t)->long_array_.data(), knn_expr->dimension_ * sizeof(long));
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);

    } else if(strcmp((yyvsp[-5].str_value), "bit") == 0 and knn_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->long_array_.size();
        if(knn_expr->dimension_ % 8 == 0) {
            knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = knn_expr->dimension_ / 8;
            knn_expr->embedding_data_ptr_ = new char[embedding_size]{};

            for(long i = 0; i < embedding_size; ++ i) {
                char embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-7].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        char temp = embedding_unit << 1;
                        temp &= 1;
                        embedding_unit = temp;
                    } else if((yyvsp[-7].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        embedding_unit <<= 0;
                    } else {
                        free((yyvsp[-5].str_value));
                        free((yyvsp[-3].str_value));
                        delete (yyvsp[-7].const_expr_t);
                        delete (yyval.expr_t);
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                }
                ((char*)knn_expr->embedding_data_ptr_)[i] = embedding_unit;
            }
            free((yyvsp[-5].str_value));
            free((yyvsp[-3].str_value));
            delete (yyvsp[-7].const_expr_t);
        } else {
            free((yyvsp[-5].str_value));
            free((yyvsp[-3].str_value));
            delete (yyvsp[-7].const_expr_t);
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "KNN data type is bit which length should be aligned with 8");
            YYERROR;
        }

    } else if(strcmp((yyvsp[-5].str_value), "double") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-7].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        knn_expr->embedding_data_ptr_ = new double[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-7].const_expr_t)->double_array_.data(), knn_expr->dimension_ * sizeof(double));
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
    } else {
        free((yyvsp[-5].str_value));
        free((yyvsp[-3].str_value));
        delete (yyvsp[-7].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn data type");
        YYERROR;
    }
    knn_expr->topn_ = (yyvsp[-1].long_value);
}
#line 5238 "parser.cpp"
    break;

  case 233: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1895 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5251 "parser.cpp"
    break;

  case 234: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1903 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-5].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5266 "parser.cpp"
    break;

  case 235: /* query_expr: QUERY '(' STRING ')'  */
#line 1914 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5277 "parser.cpp"
    break;

  case 236: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1920 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5290 "parser.cpp"
    break;

  case 237: /* fusion_expr: FUSION '(' STRING ')'  */
#line 1929 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5301 "parser.cpp"
    break;

  case 238: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1935 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5314 "parser.cpp"
    break;

  case 239: /* sub_search_array: knn_expr  */
#line 1945 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5323 "parser.cpp"
    break;

  case 240: /* sub_search_array: match_expr  */
#line 1949 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5332 "parser.cpp"
    break;

  case 241: /* sub_search_array: query_expr  */
#line 1953 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5341 "parser.cpp"
    break;

  case 242: /* sub_search_array: fusion_expr  */
#line 1957 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5350 "parser.cpp"
    break;

  case 243: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 1961 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5359 "parser.cpp"
    break;

  case 244: /* sub_search_array: sub_search_array ',' match_expr  */
#line 1965 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5368 "parser.cpp"
    break;

  case 245: /* sub_search_array: sub_search_array ',' query_expr  */
#line 1969 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5377 "parser.cpp"
    break;

  case 246: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 1973 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5386 "parser.cpp"
    break;

  case 247: /* function_expr: IDENTIFIER '(' ')'  */
#line 1978 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5399 "parser.cpp"
    break;

  case 248: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1986 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5412 "parser.cpp"
    break;

  case 249: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1994 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5426 "parser.cpp"
    break;

  case 250: /* function_expr: operand IS NOT NULLABLE  */
#line 2003 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5438 "parser.cpp"
    break;

  case 251: /* function_expr: operand IS NULLABLE  */
#line 2010 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5450 "parser.cpp"
    break;

  case 252: /* function_expr: NOT operand  */
#line 2017 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5462 "parser.cpp"
    break;

  case 253: /* function_expr: '-' operand  */
#line 2024 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5474 "parser.cpp"
    break;

  case 254: /* function_expr: '+' operand  */
#line 2031 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5486 "parser.cpp"
    break;

  case 255: /* function_expr: operand '-' operand  */
#line 2038 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5499 "parser.cpp"
    break;

  case 256: /* function_expr: operand '+' operand  */
#line 2046 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5512 "parser.cpp"
    break;

  case 257: /* function_expr: operand '*' operand  */
#line 2054 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5525 "parser.cpp"
    break;

  case 258: /* function_expr: operand '/' operand  */
#line 2062 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5538 "parser.cpp"
    break;

  case 259: /* function_expr: operand '%' operand  */
#line 2070 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5551 "parser.cpp"
    break;

  case 260: /* function_expr: operand '=' operand  */
#line 2078 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5564 "parser.cpp"
    break;

  case 261: /* function_expr: operand EQUAL operand  */
#line 2086 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5577 "parser.cpp"
    break;

  case 262: /* function_expr: operand NOT_EQ operand  */
#line 2094 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5590 "parser.cpp"
    break;

  case 263: /* function_expr: operand '<' operand  */
#line 2102 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5603 "parser.cpp"
    break;

  case 264: /* function_expr: operand '>' operand  */
#line 2110 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5616 "parser.cpp"
    break;

  case 265: /* function_expr: operand LESS_EQ operand  */
#line 2118 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5629 "parser.cpp"
    break;

  case 266: /* function_expr: operand GREATER_EQ operand  */
#line 2126 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5642 "parser.cpp"
    break;

  case 267: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2134 "parser.y"
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
#line 5677 "parser.cpp"
    break;

  case 268: /* function_expr: operand LIKE operand  */
#line 2164 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5690 "parser.cpp"
    break;

  case 269: /* function_expr: operand NOT LIKE operand  */
#line 2172 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5703 "parser.cpp"
    break;

  case 270: /* conjunction_expr: expr AND expr  */
#line 2181 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5716 "parser.cpp"
    break;

  case 271: /* conjunction_expr: expr OR expr  */
#line 2189 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5729 "parser.cpp"
    break;

  case 272: /* between_expr: operand BETWEEN operand AND operand  */
#line 2198 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5741 "parser.cpp"
    break;

  case 273: /* in_expr: operand IN '(' expr_array ')'  */
#line 2206 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5752 "parser.cpp"
    break;

  case 274: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2212 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5763 "parser.cpp"
    break;

  case 275: /* case_expr: CASE expr case_check_array END  */
#line 2219 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5774 "parser.cpp"
    break;

  case 276: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2225 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5786 "parser.cpp"
    break;

  case 277: /* case_expr: CASE case_check_array END  */
#line 2232 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5796 "parser.cpp"
    break;

  case 278: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2237 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5807 "parser.cpp"
    break;

  case 279: /* case_check_array: WHEN expr THEN expr  */
#line 2244 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5819 "parser.cpp"
    break;

  case 280: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2251 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5831 "parser.cpp"
    break;

  case 281: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2259 "parser.y"
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
#line 5859 "parser.cpp"
    break;

  case 282: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2283 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5870 "parser.cpp"
    break;

  case 283: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2289 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5881 "parser.cpp"
    break;

  case 284: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2295 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5893 "parser.cpp"
    break;

  case 285: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2302 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5905 "parser.cpp"
    break;

  case 286: /* column_expr: IDENTIFIER  */
#line 2310 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5917 "parser.cpp"
    break;

  case 287: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2317 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5929 "parser.cpp"
    break;

  case 288: /* column_expr: '*'  */
#line 2324 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5939 "parser.cpp"
    break;

  case 289: /* column_expr: column_expr '.' '*'  */
#line 2329 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5953 "parser.cpp"
    break;

  case 290: /* constant_expr: STRING  */
#line 2339 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5963 "parser.cpp"
    break;

  case 291: /* constant_expr: TRUE  */
#line 2344 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 5973 "parser.cpp"
    break;

  case 292: /* constant_expr: FALSE  */
#line 2349 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 5983 "parser.cpp"
    break;

  case 293: /* constant_expr: DOUBLE_VALUE  */
#line 2354 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5993 "parser.cpp"
    break;

  case 294: /* constant_expr: LONG_VALUE  */
#line 2359 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6003 "parser.cpp"
    break;

  case 295: /* constant_expr: DATE STRING  */
#line 2364 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6013 "parser.cpp"
    break;

  case 296: /* constant_expr: INTERVAL interval_expr  */
#line 2369 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6021 "parser.cpp"
    break;

  case 297: /* constant_expr: interval_expr  */
#line 2372 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6029 "parser.cpp"
    break;

  case 298: /* constant_expr: long_array_expr  */
#line 2375 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6037 "parser.cpp"
    break;

  case 299: /* constant_expr: double_array_expr  */
#line 2378 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6045 "parser.cpp"
    break;

  case 300: /* array_expr: long_array_expr  */
#line 2382 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6053 "parser.cpp"
    break;

  case 301: /* array_expr: double_array_expr  */
#line 2385 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6061 "parser.cpp"
    break;

  case 302: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2389 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6069 "parser.cpp"
    break;

  case 303: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2393 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6079 "parser.cpp"
    break;

  case 304: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2398 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6088 "parser.cpp"
    break;

  case 305: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2403 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6096 "parser.cpp"
    break;

  case 306: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2407 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6106 "parser.cpp"
    break;

  case 307: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2412 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6115 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE SECONDS  */
#line 2417 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6126 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE SECOND  */
#line 2423 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6137 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE MINUTES  */
#line 2429 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6148 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE MINUTE  */
#line 2435 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6159 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE HOURS  */
#line 2441 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6170 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE HOUR  */
#line 2447 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6181 "parser.cpp"
    break;

  case 314: /* interval_expr: LONG_VALUE DAYS  */
#line 2453 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6192 "parser.cpp"
    break;

  case 315: /* interval_expr: LONG_VALUE DAY  */
#line 2459 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6203 "parser.cpp"
    break;

  case 316: /* interval_expr: LONG_VALUE MONTHS  */
#line 2465 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6214 "parser.cpp"
    break;

  case 317: /* interval_expr: LONG_VALUE MONTH  */
#line 2471 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6225 "parser.cpp"
    break;

  case 318: /* interval_expr: LONG_VALUE YEARS  */
#line 2477 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6236 "parser.cpp"
    break;

  case 319: /* interval_expr: LONG_VALUE YEAR  */
#line 2483 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6247 "parser.cpp"
    break;

  case 320: /* copy_option_list: copy_option  */
#line 2494 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6256 "parser.cpp"
    break;

  case 321: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2498 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6265 "parser.cpp"
    break;

  case 322: /* copy_option: FORMAT IDENTIFIER  */
#line 2503 "parser.y"
                                {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kFormat;
    if (strcasecmp((yyvsp[0].str_value), "csv") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kCSV;
        free((yyvsp[0].str_value));
    } else if (strcasecmp((yyvsp[0].str_value), "json") == 0) {
        (yyval.copy_option_t)->file_type_ = infinity::CopyFileType::kJSON;
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
#line 6289 "parser.cpp"
    break;

  case 323: /* copy_option: DELIMITER STRING  */
#line 2522 "parser.y"
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
#line 6304 "parser.cpp"
    break;

  case 324: /* copy_option: HEADER  */
#line 2532 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6314 "parser.cpp"
    break;

  case 325: /* file_path: STRING  */
#line 2538 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6322 "parser.cpp"
    break;

  case 326: /* if_exists: IF EXISTS  */
#line 2542 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6328 "parser.cpp"
    break;

  case 327: /* if_exists: %empty  */
#line 2543 "parser.y"
  { (yyval.bool_value) = false; }
#line 6334 "parser.cpp"
    break;

  case 328: /* if_not_exists: IF NOT EXISTS  */
#line 2545 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6340 "parser.cpp"
    break;

  case 329: /* if_not_exists: %empty  */
#line 2546 "parser.y"
  { (yyval.bool_value) = false; }
#line 6346 "parser.cpp"
    break;

  case 332: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2561 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6359 "parser.cpp"
    break;

  case 333: /* if_not_exists_info: %empty  */
#line 2569 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6367 "parser.cpp"
    break;

  case 334: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2574 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6375 "parser.cpp"
    break;

  case 335: /* with_index_param_list: %empty  */
#line 2577 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6383 "parser.cpp"
    break;

  case 336: /* index_param_list: index_param  */
#line 2581 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6392 "parser.cpp"
    break;

  case 337: /* index_param_list: index_param_list ',' index_param  */
#line 2585 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6401 "parser.cpp"
    break;

  case 338: /* index_param: IDENTIFIER  */
#line 2590 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6411 "parser.cpp"
    break;

  case 339: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2595 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6424 "parser.cpp"
    break;

  case 340: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2603 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6436 "parser.cpp"
    break;

  case 341: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2610 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6448 "parser.cpp"
    break;

  case 342: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2621 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
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
#line 6501 "parser.cpp"
    break;

  case 343: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2669 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
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
#line 6555 "parser.cpp"
    break;


#line 6559 "parser.cpp"

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

#line 2719 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

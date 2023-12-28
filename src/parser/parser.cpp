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
  YYSYMBOL_VARCHAR = 91,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 92,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 93,                    /* DOUBLE  */
  YYSYMBOL_REAL = 94,                      /* REAL  */
  YYSYMBOL_DECIMAL = 95,                   /* DECIMAL  */
  YYSYMBOL_DATE = 96,                      /* DATE  */
  YYSYMBOL_TIME = 97,                      /* TIME  */
  YYSYMBOL_DATETIME = 98,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 99,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 100,                     /* UUID  */
  YYSYMBOL_POINT = 101,                    /* POINT  */
  YYSYMBOL_LINE = 102,                     /* LINE  */
  YYSYMBOL_LSEG = 103,                     /* LSEG  */
  YYSYMBOL_BOX = 104,                      /* BOX  */
  YYSYMBOL_PATH = 105,                     /* PATH  */
  YYSYMBOL_POLYGON = 106,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 107,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 108,                     /* BLOB  */
  YYSYMBOL_BITMAP = 109,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 110,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 111,                   /* VECTOR  */
  YYSYMBOL_BIT = 112,                      /* BIT  */
  YYSYMBOL_PRIMARY = 113,                  /* PRIMARY  */
  YYSYMBOL_KEY = 114,                      /* KEY  */
  YYSYMBOL_UNIQUE = 115,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 116,                 /* NULLABLE  */
  YYSYMBOL_IS = 117,                       /* IS  */
  YYSYMBOL_TRUE = 118,                     /* TRUE  */
  YYSYMBOL_FALSE = 119,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 120,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 121,                   /* SECOND  */
  YYSYMBOL_SECONDS = 122,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 123,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 124,                  /* MINUTES  */
  YYSYMBOL_HOUR = 125,                     /* HOUR  */
  YYSYMBOL_HOURS = 126,                    /* HOURS  */
  YYSYMBOL_DAY = 127,                      /* DAY  */
  YYSYMBOL_DAYS = 128,                     /* DAYS  */
  YYSYMBOL_MONTH = 129,                    /* MONTH  */
  YYSYMBOL_MONTHS = 130,                   /* MONTHS  */
  YYSYMBOL_YEAR = 131,                     /* YEAR  */
  YYSYMBOL_YEARS = 132,                    /* YEARS  */
  YYSYMBOL_EQUAL = 133,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 134,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 135,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 136,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 137,                  /* BETWEEN  */
  YYSYMBOL_AND = 138,                      /* AND  */
  YYSYMBOL_OR = 139,                       /* OR  */
  YYSYMBOL_EXTRACT = 140,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 141,                     /* LIKE  */
  YYSYMBOL_DATA = 142,                     /* DATA  */
  YYSYMBOL_LOG = 143,                      /* LOG  */
  YYSYMBOL_BUFFER = 144,                   /* BUFFER  */
  YYSYMBOL_KNN = 145,                      /* KNN  */
  YYSYMBOL_USING = 146,                    /* USING  */
  YYSYMBOL_SESSION = 147,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 148,                   /* GLOBAL  */
  YYSYMBOL_OFF = 149,                      /* OFF  */
  YYSYMBOL_EXPORT = 150,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 151,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 152,                  /* CONFIGS  */
  YYSYMBOL_PROFILES = 153,                 /* PROFILES  */
  YYSYMBOL_STATUS = 154,                   /* STATUS  */
  YYSYMBOL_SEARCH = 155,                   /* SEARCH  */
  YYSYMBOL_MATCH = 156,                    /* MATCH  */
  YYSYMBOL_QUERY = 157,                    /* QUERY  */
  YYSYMBOL_FUSION = 158,                   /* FUSION  */
  YYSYMBOL_NUMBER = 159,                   /* NUMBER  */
  YYSYMBOL_160_ = 160,                     /* '='  */
  YYSYMBOL_161_ = 161,                     /* '<'  */
  YYSYMBOL_162_ = 162,                     /* '>'  */
  YYSYMBOL_163_ = 163,                     /* '+'  */
  YYSYMBOL_164_ = 164,                     /* '-'  */
  YYSYMBOL_165_ = 165,                     /* '*'  */
  YYSYMBOL_166_ = 166,                     /* '/'  */
  YYSYMBOL_167_ = 167,                     /* '%'  */
  YYSYMBOL_168_ = 168,                     /* '['  */
  YYSYMBOL_169_ = 169,                     /* ']'  */
  YYSYMBOL_170_ = 170,                     /* '('  */
  YYSYMBOL_171_ = 171,                     /* ')'  */
  YYSYMBOL_172_ = 172,                     /* '.'  */
  YYSYMBOL_173_ = 173,                     /* ';'  */
  YYSYMBOL_174_ = 174,                     /* ','  */
  YYSYMBOL_YYACCEPT = 175,                 /* $accept  */
  YYSYMBOL_input_pattern = 176,            /* input_pattern  */
  YYSYMBOL_statement_list = 177,           /* statement_list  */
  YYSYMBOL_statement = 178,                /* statement  */
  YYSYMBOL_explainable_statement = 179,    /* explainable_statement  */
  YYSYMBOL_create_statement = 180,         /* create_statement  */
  YYSYMBOL_table_element_array = 181,      /* table_element_array  */
  YYSYMBOL_table_element = 182,            /* table_element  */
  YYSYMBOL_table_column = 183,             /* table_column  */
  YYSYMBOL_column_type = 184,              /* column_type  */
  YYSYMBOL_column_constraints = 185,       /* column_constraints  */
  YYSYMBOL_column_constraint = 186,        /* column_constraint  */
  YYSYMBOL_table_constraint = 187,         /* table_constraint  */
  YYSYMBOL_identifier_array = 188,         /* identifier_array  */
  YYSYMBOL_delete_statement = 189,         /* delete_statement  */
  YYSYMBOL_insert_statement = 190,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 191, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 192,        /* explain_statement  */
  YYSYMBOL_explain_type = 193,             /* explain_type  */
  YYSYMBOL_update_statement = 194,         /* update_statement  */
  YYSYMBOL_update_expr_array = 195,        /* update_expr_array  */
  YYSYMBOL_update_expr = 196,              /* update_expr  */
  YYSYMBOL_drop_statement = 197,           /* drop_statement  */
  YYSYMBOL_copy_statement = 198,           /* copy_statement  */
  YYSYMBOL_select_statement = 199,         /* select_statement  */
  YYSYMBOL_select_with_paren = 200,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 201,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 202, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 203, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 204, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 205,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 206,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 207,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 208,            /* order_by_type  */
  YYSYMBOL_limit_expr = 209,               /* limit_expr  */
  YYSYMBOL_offset_expr = 210,              /* offset_expr  */
  YYSYMBOL_distinct = 211,                 /* distinct  */
  YYSYMBOL_from_clause = 212,              /* from_clause  */
  YYSYMBOL_search_clause = 213,            /* search_clause  */
  YYSYMBOL_where_clause = 214,             /* where_clause  */
  YYSYMBOL_having_clause = 215,            /* having_clause  */
  YYSYMBOL_group_by_clause = 216,          /* group_by_clause  */
  YYSYMBOL_set_operator = 217,             /* set_operator  */
  YYSYMBOL_table_reference = 218,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 219,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 220,     /* table_reference_name  */
  YYSYMBOL_table_name = 221,               /* table_name  */
  YYSYMBOL_table_alias = 222,              /* table_alias  */
  YYSYMBOL_with_clause = 223,              /* with_clause  */
  YYSYMBOL_with_expr_list = 224,           /* with_expr_list  */
  YYSYMBOL_with_expr = 225,                /* with_expr  */
  YYSYMBOL_join_clause = 226,              /* join_clause  */
  YYSYMBOL_join_type = 227,                /* join_type  */
  YYSYMBOL_show_statement = 228,           /* show_statement  */
  YYSYMBOL_flush_statement = 229,          /* flush_statement  */
  YYSYMBOL_optimize_statement = 230,       /* optimize_statement  */
  YYSYMBOL_command_statement = 231,        /* command_statement  */
  YYSYMBOL_expr_array = 232,               /* expr_array  */
  YYSYMBOL_expr_array_list = 233,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 234,               /* expr_alias  */
  YYSYMBOL_expr = 235,                     /* expr  */
  YYSYMBOL_operand = 236,                  /* operand  */
  YYSYMBOL_knn_expr = 237,                 /* knn_expr  */
  YYSYMBOL_match_expr = 238,               /* match_expr  */
  YYSYMBOL_query_expr = 239,               /* query_expr  */
  YYSYMBOL_fusion_expr = 240,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 241,         /* sub_search_array  */
  YYSYMBOL_function_expr = 242,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 243,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 244,             /* between_expr  */
  YYSYMBOL_in_expr = 245,                  /* in_expr  */
  YYSYMBOL_case_expr = 246,                /* case_expr  */
  YYSYMBOL_case_check_array = 247,         /* case_check_array  */
  YYSYMBOL_cast_expr = 248,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 249,            /* subquery_expr  */
  YYSYMBOL_column_expr = 250,              /* column_expr  */
  YYSYMBOL_constant_expr = 251,            /* constant_expr  */
  YYSYMBOL_array_expr = 252,               /* array_expr  */
  YYSYMBOL_long_array_expr = 253,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 254, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 255,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 256, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 257,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 258,         /* copy_option_list  */
  YYSYMBOL_copy_option = 259,              /* copy_option  */
  YYSYMBOL_file_path = 260,                /* file_path  */
  YYSYMBOL_if_exists = 261,                /* if_exists  */
  YYSYMBOL_if_not_exists = 262,            /* if_not_exists  */
  YYSYMBOL_semicolon = 263,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 264,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 265,    /* with_index_param_list  */
  YYSYMBOL_index_param_list = 266,         /* index_param_list  */
  YYSYMBOL_index_param = 267,              /* index_param  */
  YYSYMBOL_index_info_list = 268           /* index_info_list  */
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
#define YYLAST   868

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  175
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  94
/* YYNRULES -- Number of rules.  */
#define YYNRULES  344
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  671

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   414


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
       2,     2,     2,     2,     2,     2,     2,   167,     2,     2,
     170,   171,   165,   163,   174,   164,   172,   166,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   173,
     161,   160,   162,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   168,     2,   169,     2,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158,   159
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
     743,   744,   745,   746,   749,   750,   751,   752,   753,   754,
     755,   756,   757,   758,   759,   760,   761,   762,   763,   764,
     783,   787,   797,   800,   803,   806,   810,   815,   822,   828,
     838,   854,   888,   901,   904,   911,   917,   920,   923,   926,
     929,   932,   935,   938,   945,   958,   962,   967,   980,   993,
    1008,  1023,  1038,  1061,  1102,  1147,  1150,  1153,  1162,  1172,
    1175,  1179,  1184,  1206,  1209,  1214,  1230,  1233,  1237,  1241,
    1246,  1252,  1255,  1258,  1262,  1266,  1268,  1272,  1274,  1277,
    1281,  1284,  1288,  1293,  1297,  1300,  1304,  1307,  1311,  1314,
    1318,  1321,  1324,  1327,  1335,  1338,  1353,  1353,  1355,  1369,
    1378,  1383,  1392,  1397,  1402,  1408,  1415,  1418,  1422,  1425,
    1430,  1442,  1449,  1463,  1466,  1469,  1472,  1475,  1478,  1481,
    1487,  1491,  1495,  1499,  1503,  1507,  1511,  1515,  1526,  1537,
    1549,  1562,  1577,  1581,  1585,  1593,  1608,  1614,  1619,  1625,
    1631,  1639,  1645,  1651,  1657,  1663,  1671,  1677,  1688,  1692,
    1697,  1701,  1728,  1734,  1738,  1739,  1740,  1741,  1742,  1744,
    1747,  1753,  1756,  1757,  1758,  1759,  1760,  1761,  1762,  1763,
    1765,  1932,  1940,  1951,  1957,  1966,  1972,  1982,  1986,  1990,
    1994,  1998,  2002,  2006,  2010,  2015,  2023,  2031,  2040,  2047,
    2054,  2061,  2068,  2075,  2083,  2091,  2099,  2107,  2115,  2123,
    2131,  2139,  2147,  2155,  2163,  2171,  2201,  2209,  2218,  2226,
    2235,  2243,  2249,  2256,  2262,  2269,  2274,  2281,  2288,  2296,
    2320,  2326,  2332,  2339,  2347,  2354,  2361,  2366,  2376,  2381,
    2386,  2391,  2396,  2401,  2406,  2411,  2416,  2421,  2424,  2427,
    2430,  2434,  2437,  2441,  2445,  2450,  2455,  2459,  2464,  2469,
    2475,  2481,  2487,  2493,  2499,  2505,  2511,  2517,  2523,  2529,
    2535,  2546,  2550,  2555,  2577,  2587,  2593,  2597,  2598,  2600,
    2601,  2603,  2604,  2616,  2624,  2628,  2631,  2635,  2639,  2644,
    2649,  2657,  2664,  2675,  2723
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
  "SMALLINT", "BIGINT", "HUGEINT", "VARCHAR", "FLOAT", "DOUBLE", "REAL",
  "DECIMAL", "DATE", "TIME", "DATETIME", "TIMESTAMP", "UUID", "POINT",
  "LINE", "LSEG", "BOX", "PATH", "POLYGON", "CIRCLE", "BLOB", "BITMAP",
  "EMBEDDING", "VECTOR", "BIT", "PRIMARY", "KEY", "UNIQUE", "NULLABLE",
  "IS", "TRUE", "FALSE", "INTERVAL", "SECOND", "SECONDS", "MINUTE",
  "MINUTES", "HOUR", "HOURS", "DAY", "DAYS", "MONTH", "MONTHS", "YEAR",
  "YEARS", "EQUAL", "NOT_EQ", "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND",
  "OR", "EXTRACT", "LIKE", "DATA", "LOG", "BUFFER", "KNN", "USING",
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

#define YYPACT_NINF (-609)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-335)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      42,   306,     8,   330,    45,   -13,    45,    28,   488,    33,
      43,    12,    72,    45,    91,   -55,   -58,   133,    -9,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,  -609,   244,  -609,  -609,
     129,  -609,  -609,  -609,  -609,   119,   119,   119,   119,    30,
      45,   121,   121,   121,   121,   121,    44,   179,    45,   110,
     197,   215,  -609,  -609,  -609,  -609,  -609,  -609,  -609,   223,
    -609,  -609,  -609,    69,   100,  -609,  -609,    45,    63,  -609,
    -609,  -609,  -609,  -609,   194,    82,  -609,   255,    99,   131,
    -609,   195,  -609,   286,  -609,  -609,    -2,   216,  -609,   237,
     247,   334,    45,    45,    45,   335,   284,   173,   274,   346,
      45,    45,    45,   347,   353,   365,   320,   379,   379,    16,
      39,  -609,  -609,  -609,  -609,  -609,  -609,  -609,   244,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,   384,  -609,   224,    91,
     379,  -609,  -609,  -609,  -609,    -2,  -609,  -609,  -609,   451,
     354,   331,   343,  -609,   -40,  -609,   173,  -609,    45,   401,
      17,  -609,  -609,  -609,  -609,  -609,   362,  -609,   266,   -45,
    -609,   451,  -609,  -609,   377,   386,  -609,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,   358,   129,  -609,  -609,
     282,   290,   294,  -609,  -609,   411,   505,   295,   300,   318,
     467,   468,   469,   473,  -609,  -609,   472,   309,   310,   314,
     315,   317,   558,   558,  -609,   275,   349,   -59,  -609,   -33,
     610,  -609,  -609,  -609,  -609,  -609,  -609,  -609,  -609,  -609,
    -609,  -609,   319,  -609,  -609,   -84,  -609,    22,  -609,   451,
     451,   425,  -609,   -58,     9,   441,   323,  -609,  -108,   345,
    -609,    45,   451,   365,  -609,    90,   348,   374,   495,   333,
    -609,  -609,   221,  -609,  -609,  -609,  -609,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,  -609,   558,   375,   661,   442,   451,
     451,   -60,   120,  -609,  -609,  -609,  -609,   411,  -609,   524,
     451,   542,   547,   548,   209,   209,  -609,  -609,   382,    23,
       4,   451,   399,   552,   451,   451,   -52,   387,   -36,   558,
     558,   558,   558,   558,   558,   558,   558,   558,   558,   558,
     558,   558,   558,     5,  -609,   550,  -609,   553,   385,  -609,
     -39,    90,   451,  -609,   244,   757,   446,   395,   -51,  -609,
    -609,  -609,   -58,   401,   396,  -609,   564,   451,   394,  -609,
      90,  -609,   212,   212,  -609,  -609,   451,  -609,    73,   442,
     431,   402,   -25,    19,   159,  -609,   451,   451,   504,    40,
     403,    84,   122,  -609,  -609,   -58,   404,   443,  -609,    36,
    -609,  -609,   106,   320,  -609,  -609,   444,   409,   558,   349,
     464,  -609,   673,   673,   245,   245,   622,   673,   673,   245,
     245,   209,   209,  -609,  -609,  -609,  -609,  -609,  -609,  -609,
     451,  -609,  -609,  -609,    90,  -609,  -609,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,   414,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,  -609,   416,   417,   144,
     418,   401,  -609,     9,   244,   177,   401,  -609,   193,   419,
     577,   587,  -609,   198,  -609,   226,   228,  -609,   421,  -609,
     757,   451,  -609,   451,    -7,    21,   558,   426,   589,  -609,
     591,  -609,   593,    13,     4,   545,  -609,  -609,  -609,  -609,
    -609,  -609,   546,  -609,   597,  -609,  -609,  -609,  -609,  -609,
     432,   557,   349,   673,   439,   250,  -609,   558,  -609,   605,
     186,   218,   496,   499,  -609,  -609,   144,  -609,   401,   254,
    -609,   471,   259,  -609,   451,  -609,  -609,  -609,   212,  -609,
    -609,  -609,   447,    90,    14,  -609,   451,   571,   448,  -609,
    -609,   260,   449,   455,    36,   443,     4,     4,   458,   106,
     580,   581,   459,   261,  -609,  -609,   661,   268,   460,   461,
     463,   465,   470,   474,   475,   477,   478,   479,   484,   485,
     486,   490,   491,   492,  -609,  -609,  -609,   269,  -609,   629,
     487,   270,  -609,  -609,  -609,    90,  -609,   637,  -609,   638,
    -609,  -609,  -609,  -609,   585,   401,  -609,  -609,  -609,  -609,
     451,   451,  -609,  -609,  -609,  -609,   640,   641,   665,   666,
     668,   674,   676,   677,   678,   679,   680,   681,   683,   684,
     686,   687,   688,  -609,   621,   664,  -609,   522,   526,   451,
     278,   525,    90,   529,   530,   531,   537,   538,   539,   540,
     549,   554,   569,   570,   578,   579,   582,   583,   588,   590,
     543,  -609,   621,   713,  -609,    90,  -609,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,  -609,  -609,  -609,  -609,  -609,  -609,
    -609,  -609,  -609,  -609,   715,  -609,   555,   559,   288,  -609,
     718,   322,  -609,   715,   594,  -609,  -609,  -609,  -609,   621,
    -609
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,     0,     0,     0,     0,     0,   167,     0,   332,     3,
       5,    10,    12,    13,    11,     6,     7,     9,   116,   115,
       0,     8,    14,    15,    16,   330,   330,   330,   330,   330,
       0,   328,   328,   328,   328,   328,   160,     0,     0,     0,
       0,     0,    97,   101,    98,    99,   100,   102,    96,   167,
     181,   182,   180,     0,     0,   183,   184,     0,   187,   192,
     193,   194,   196,   195,     0,   166,   168,     0,     0,     0,
       1,   167,     2,   150,   152,   153,     0,   139,   121,   127,
       0,     0,     0,     0,     0,     0,     0,    94,     0,     0,
       0,     0,     0,     0,     0,     0,   145,     0,     0,     0,
       0,    95,    17,    22,    24,    23,    18,    19,    21,    20,
      25,    26,    27,   185,   186,   191,     0,   188,     0,     0,
       0,   120,   119,     4,   151,     0,   117,   118,   138,     0,
       0,   135,     0,    28,     0,    29,    94,   333,     0,     0,
     167,   327,   108,   110,   109,   111,     0,   161,     0,   145,
     105,     0,    90,   326,     0,     0,   200,   202,   201,   198,
     199,   205,   207,   206,   203,   204,   189,     0,   169,   197,
       0,     0,   284,   288,   291,   292,     0,     0,     0,     0,
       0,     0,     0,     0,   289,   290,     0,     0,     0,     0,
       0,     0,     0,     0,   286,     0,   167,   141,   208,   213,
     214,   226,   227,   228,   229,   223,   218,   217,   216,   224,
     225,   215,   222,   221,   299,     0,   300,     0,   298,     0,
       0,   137,   329,   167,     0,     0,     0,    88,     0,     0,
      92,     0,     0,     0,   104,   144,     0,     0,     0,     0,
     124,   123,     0,   310,   309,   312,   311,   314,   313,   316,
     315,   318,   317,   320,   319,     0,     0,   250,   167,     0,
       0,     0,     0,   293,   294,   295,   296,     0,   297,     0,
       0,     0,     0,     0,   252,   251,   307,   304,     0,     0,
       0,     0,   143,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   303,     0,   306,     0,   126,   128,
     133,   134,     0,   122,    31,     0,     0,     0,     0,    34,
      36,    37,   167,     0,    33,    93,     0,     0,    91,   112,
     107,   106,     0,     0,   190,   170,     0,   245,     0,   167,
       0,     0,     0,     0,     0,   275,     0,     0,     0,     0,
       0,     0,     0,   220,   219,   167,   140,   154,   156,   165,
     157,   209,     0,   145,   212,   268,   269,     0,     0,   167,
       0,   249,   259,   260,   263,   264,     0,   266,   258,   261,
     262,   254,   253,   255,   256,   257,   285,   287,   305,   308,
       0,   131,   132,   130,   136,    40,    43,    44,    41,    42,
      45,    46,    60,    47,    49,    48,    63,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     0,     0,    38,
       0,     0,    30,     0,    32,     0,     0,    89,     0,     0,
       0,     0,   325,     0,   321,     0,     0,   246,     0,   280,
       0,     0,   273,     0,     0,     0,     0,     0,     0,   233,
       0,   235,     0,     0,     0,     0,   174,   175,   176,   177,
     173,   178,     0,   163,     0,   158,   237,   238,   239,   240,
     142,   149,   167,   267,     0,     0,   248,     0,   129,     0,
       0,     0,     0,     0,    83,    84,    39,    80,     0,     0,
      35,     0,     0,   210,     0,   324,   323,   114,     0,   113,
     247,   281,     0,   277,     0,   276,     0,     0,     0,   301,
     302,     0,     0,     0,   165,   155,     0,     0,   162,     0,
       0,   147,     0,     0,   282,   271,   270,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    85,    82,    81,     0,    87,     0,
       0,     0,   322,   279,   274,   278,   265,     0,   231,     0,
     234,   236,   159,   171,     0,     0,   241,   242,   243,   244,
       0,     0,   125,   283,   272,    62,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    86,   336,     0,   211,     0,     0,     0,
       0,   148,   146,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   343,   336,     0,   232,   172,   164,    61,    67,    68,
      65,    66,    69,    70,    71,    64,    75,    76,    73,    74,
      77,    78,    79,    72,     0,   344,     0,   339,     0,   337,
       0,     0,   335,     0,     0,   340,   342,   341,   338,   336,
     230
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -609,  -609,  -609,   667,  -609,   693,  -609,   329,  -609,   316,
    -609,   271,  -609,  -329,   705,   709,   623,  -609,  -609,   720,
    -609,   556,   721,   722,   -56,   775,   -16,   615,   658,   -54,
    -609,  -609,   398,  -609,  -609,  -609,  -609,  -609,  -609,  -154,
    -609,  -609,  -609,  -609,   336,  -195,    25,   277,  -609,  -609,
     675,  -609,  -609,   744,   746,   751,   752,  -251,  -609,   521,
    -159,  -161,  -363,  -362,  -358,  -357,  -609,  -609,  -609,  -609,
    -609,  -609,   544,  -609,  -609,  -609,  -609,  -609,   356,  -609,
     359,  -609,   624,   476,   321,   -48,   241,   276,  -609,  -609,
    -608,  -609,   154,  -609
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    17,    18,    19,   111,    20,   328,   329,   330,   429,
     496,   497,   331,   238,    21,    22,   150,    23,    59,    24,
     159,   160,    25,    26,    27,    28,    29,    88,   136,    89,
     141,   318,   319,   403,   231,   323,   139,   292,   373,   162,
     582,   531,    86,   366,   367,   368,   369,   475,    30,    75,
      76,   370,   472,    31,    32,    33,    34,   207,   338,   208,
     209,   210,   211,   212,   213,   214,   480,   215,   216,   217,
     218,   219,   272,   220,   221,   222,   223,   518,   224,   225,
     226,   227,   228,   443,   444,   164,    99,    91,    82,    96,
     631,   658,   659,   334
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,   348,   245,   118,   435,   244,    87,    46,   396,   476,
     477,   233,   325,   290,   478,   479,    14,   161,   293,   377,
     166,   167,   168,   270,   655,   267,   450,   401,   402,    47,
     271,    49,   137,   380,    83,    68,    84,    85,    73,   473,
      40,   284,   285,   171,   172,   173,    46,   289,    46,     1,
     239,     2,     3,     4,     5,     6,     7,     8,     9,    48,
     165,   670,    10,   335,    60,    97,   336,    11,    12,    13,
     320,   321,   515,   106,   169,    72,    61,    62,   294,   295,
     381,   181,   179,   340,    67,   314,   438,   474,  -334,   378,
     315,    14,   125,   564,    74,   446,    77,   174,    90,   294,
     295,   451,   499,   516,   267,   294,   295,   502,   126,   127,
     352,   353,    16,   294,   295,   291,    14,   144,   145,   146,
     432,   359,   326,   433,   327,   153,   154,   155,   485,   243,
     234,   294,   295,    80,   240,   375,   376,    87,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   392,   393,
     394,   395,   294,   295,    69,    70,    71,   294,   295,   294,
     295,   294,   295,   404,    81,   170,   576,   577,   135,   557,
     397,   578,   579,   236,   365,    50,    51,   324,   294,   295,
      63,    64,   107,   108,   524,    65,    66,    90,   175,    98,
     288,   316,    15,   105,   364,  -331,   317,   454,   455,   355,
     109,   356,     1,   357,     2,     3,     4,     5,     6,     7,
       8,     9,    16,   492,   457,    10,   104,   483,   110,   481,
      11,    12,    13,   123,   182,   183,   184,   185,   294,   295,
       1,   533,     2,     3,     4,     5,     6,     7,   452,     9,
     453,   320,   357,    10,   447,   128,   610,   291,    11,    12,
      13,   198,   351,   561,   124,   459,   129,   493,   460,   494,
     495,   130,   199,   200,   201,    83,   339,    84,    85,    14,
     131,   538,   539,   540,   541,   542,   434,   138,   543,   544,
     286,   287,   346,   100,   101,   102,   103,   440,   441,   442,
     186,   187,   513,   461,   514,   517,   462,    14,   545,   188,
     140,   189,   132,   546,   547,   548,   549,   550,   134,   463,
     551,   552,    92,    93,    94,    95,   142,   190,   191,   192,
     193,   182,   183,   184,   185,   665,   536,   666,   667,   611,
     553,   573,   574,   448,    35,    36,    37,   143,   147,   194,
     195,   196,   148,   149,   151,    15,    38,    39,   501,   152,
     156,   336,   182,   183,   184,   185,   157,   565,    41,    42,
      43,   197,   298,   484,   503,    16,   198,   291,   158,   507,
      44,    45,   508,    15,   310,   311,   312,   199,   200,   201,
    -335,  -335,   161,   163,   202,   203,   204,   186,   187,   205,
     176,   206,   347,    16,   177,   230,   188,   509,   189,   510,
     508,   270,   291,   229,   237,   248,  -335,  -335,   308,   309,
     310,   311,   312,   232,   190,   191,   192,   193,   186,   187,
     241,   535,   612,    14,   291,   558,   242,   188,   336,   189,
     560,   568,   584,   336,   569,   291,   194,   195,   196,   585,
     603,   606,   586,   336,   291,   190,   191,   192,   193,   636,
     635,   246,   336,   250,   182,   183,   184,   185,   197,   662,
     247,   251,   663,   198,   252,   268,   532,   194,   195,   196,
     269,   273,   274,   275,   199,   200,   201,   276,   277,   279,
     280,   202,   203,   204,   281,   282,   205,   283,   206,   197,
     322,   313,   332,   333,   198,   465,  -179,   466,   467,   468,
     469,   344,   470,   471,   345,   199,   200,   201,   182,   183,
     184,   185,   202,   203,   204,   337,    14,   205,   342,   206,
     186,   187,    52,    53,    54,    55,    56,    57,   358,   188,
      58,   189,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   343,   349,   360,   190,   191,   192,
     193,   361,   362,   363,   372,   374,   398,   379,   399,   400,
     430,   182,   183,   184,   185,   431,   436,   437,   439,   194,
     195,   196,   378,   449,   265,   266,   456,   458,   464,   482,
     486,   505,   294,   188,   489,   189,   490,   491,   498,   504,
     506,   197,   511,   521,   205,   522,   198,   523,   526,   527,
     528,   190,   191,   192,   193,   530,   529,   199,   200,   201,
     534,   537,   554,   555,   202,   203,   204,   559,   563,   205,
     570,   206,   567,   194,   195,   196,   571,   265,   575,   580,
     583,   581,   604,   605,   587,   588,   188,   589,   189,   590,
     350,   607,   608,   609,   591,   197,   613,   614,   592,   593,
     198,   594,   595,   596,   190,   191,   192,   193,   597,   598,
     599,   199,   200,   201,   600,   601,   602,   632,   202,   203,
     204,   615,   616,   205,   617,   206,   194,   195,   196,   296,
     618,   297,   619,   620,   621,   622,   623,   624,   298,   625,
     626,   350,   627,   628,   629,   630,   633,   634,   197,   291,
     637,   638,   639,   198,   299,   300,   301,   302,   640,   641,
     642,   643,   304,   654,   199,   200,   201,   656,   657,   661,
     644,   202,   203,   204,   664,   645,   205,   298,   206,   660,
     350,   305,   306,   307,   308,   309,   310,   311,   312,   298,
     646,   647,   566,   299,   300,   301,   302,   303,   133,   648,
     649,   304,   112,   650,   651,   299,   300,   301,   302,   652,
     487,   653,   500,   304,   113,   669,   512,   556,   114,   235,
     305,   306,   307,   308,   309,   310,   311,   312,   298,   115,
     116,   117,   305,   306,   307,   308,   309,   310,   311,   312,
     298,    78,   249,   180,   299,   300,   301,   302,   488,   341,
     525,   572,   304,   119,   178,   120,  -335,  -335,   301,   302,
     121,   122,   371,   519,  -335,   354,   520,   668,     0,   445,
     278,   305,   306,   307,   308,   309,   310,   311,   312,   562,
       0,     0,     0,  -335,   306,   307,   308,   309,   310,   311,
     312,   405,   406,   407,   408,   409,   410,   411,   412,   413,
     414,   415,   416,   417,   418,   419,   420,   421,   422,   423,
     424,   425,     0,     0,   426,     0,     0,   427,   428
};

static const yytype_int16 yycheck[] =
{
      16,   252,   161,    59,   333,   159,     8,     3,     3,   372,
     372,    51,     3,    72,   372,   372,    74,    62,    51,    71,
       4,     5,     6,    83,   632,   186,    51,    66,    67,     4,
     189,     6,    86,    69,    21,    10,    23,    24,    13,     3,
      32,   202,   203,     4,     5,     6,     3,   206,     3,     7,
      33,     9,    10,    11,    12,    13,    14,    15,    16,    72,
     108,   669,    20,   171,    31,    40,   174,    25,    26,    27,
     229,   230,    79,    48,    58,     3,    43,    44,   138,   139,
     116,   135,   130,   242,    41,   169,   337,    51,    58,   141,
     174,    74,    67,    79,     3,   346,   151,    58,    68,   138,
     139,    82,   431,    82,   265,   138,   139,   436,    45,    46,
     269,   270,   170,   138,   139,   174,    74,    92,    93,    94,
     171,   280,   113,   174,   115,   100,   101,   102,   379,   174,
     170,   138,   139,     0,   150,   294,   295,     8,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   138,   139,   142,   143,   144,   138,   139,   138,
     139,   138,   139,   322,   173,   149,   529,   529,   170,   498,
     165,   529,   529,   148,   170,   147,   148,   233,   138,   139,
     147,   148,    72,    73,   171,   152,   153,    68,   149,    68,
     206,   169,   150,    14,   171,     0,   174,   356,   357,    79,
       3,    81,     7,    83,     9,    10,    11,    12,    13,    14,
      15,    16,   170,    69,   174,    20,   172,   378,     3,   373,
      25,    26,    27,   154,     3,     4,     5,     6,   138,   139,
       7,   482,     9,    10,    11,    12,    13,    14,    79,    16,
      81,   400,    83,    20,   171,    51,   575,   174,    25,    26,
      27,   145,   268,   504,   154,   171,   174,   113,   174,   115,
     116,     6,   156,   157,   158,    21,   241,    23,    24,    74,
     171,    85,    86,    87,    88,    89,   332,    61,    92,    93,
       5,     6,    61,    42,    43,    44,    45,    75,    76,    77,
      69,    70,   451,   171,   453,   456,   174,    74,   112,    78,
      63,    80,   171,    85,    86,    87,    88,    89,    22,   365,
      92,    93,    36,    37,    38,    39,    69,    96,    97,    98,
      99,     3,     4,     5,     6,     3,   487,     5,     6,   580,
     112,   526,   527,   349,    28,    29,    30,     3,     3,   118,
     119,   120,    58,   170,    70,   150,    40,    41,   171,     3,
       3,   174,     3,     4,     5,     6,     3,   516,    28,    29,
      30,   140,   117,   379,   171,   170,   145,   174,     3,   171,
      40,    41,   174,   150,   165,   166,   167,   156,   157,   158,
     135,   136,    62,     4,   163,   164,   165,    69,    70,   168,
       6,   170,   171,   170,   170,    64,    78,   171,    80,   171,
     174,    83,   174,    49,     3,    47,   161,   162,   163,   164,
     165,   166,   167,    70,    96,    97,    98,    99,    69,    70,
      58,   171,   581,    74,   174,   171,   160,    78,   174,    80,
     171,   171,   171,   174,   174,   174,   118,   119,   120,   171,
     171,   171,   174,   174,   174,    96,    97,    98,    99,   171,
     609,    74,   174,   171,     3,     4,     5,     6,   140,   171,
      74,   171,   174,   145,   170,   170,   482,   118,   119,   120,
     170,     4,     4,     4,   156,   157,   158,     4,     6,   170,
     170,   163,   164,   165,   170,   170,   168,   170,   170,   140,
      65,   172,    51,   170,   145,    52,    53,    54,    55,    56,
      57,     6,    59,    60,   171,   156,   157,   158,     3,     4,
       5,     6,   163,   164,   165,   170,    74,   168,   170,   170,
      69,    70,    34,    35,    36,    37,    38,    39,     4,    78,
      42,    80,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   170,   170,     4,    96,    97,    98,
      99,     4,     4,   171,   155,     3,     6,   170,     5,   174,
     114,     3,     4,     5,     6,   170,   170,     3,   174,   118,
     119,   120,   141,   171,    69,    70,    72,   174,   174,   170,
     116,     4,   138,    78,   170,    80,   170,   170,   170,   170,
       3,   140,   171,     4,   168,     4,   145,     4,    53,    53,
       3,    96,    97,    98,    99,    48,   174,   156,   157,   158,
     171,     6,   116,   114,   163,   164,   165,   146,   171,   168,
     171,   170,   174,   118,   119,   120,   171,    69,   170,    49,
     171,    50,     3,   146,   174,   174,    78,   174,    80,   174,
      69,     4,     4,    58,   174,   140,     6,     6,   174,   174,
     145,   174,   174,   174,    96,    97,    98,    99,   174,   174,
     174,   156,   157,   158,   174,   174,   174,     3,   163,   164,
     165,     6,     6,   168,     6,   170,   118,   119,   120,    69,
       6,    71,     6,     6,     6,     6,     6,     6,   117,     6,
       6,    69,     6,     6,     6,    74,   174,   171,   140,   174,
     171,   171,   171,   145,   133,   134,   135,   136,   171,   171,
     171,   171,   141,   170,   156,   157,   158,     4,     3,   160,
     171,   163,   164,   165,     6,   171,   168,   117,   170,   174,
      69,   160,   161,   162,   163,   164,   165,   166,   167,   117,
     171,   171,   171,   133,   134,   135,   136,   137,    81,   171,
     171,   141,    59,   171,   171,   133,   134,   135,   136,   171,
     138,   171,   433,   141,    59,   171,   450,   496,    59,   146,
     160,   161,   162,   163,   164,   165,   166,   167,   117,    59,
      59,    59,   160,   161,   162,   163,   164,   165,   166,   167,
     117,    16,   177,   135,   133,   134,   135,   136,   400,   243,
     464,   524,   141,    59,   129,    59,   133,   134,   135,   136,
      59,    59,   291,   457,   141,   271,   457,   663,    -1,   343,
     196,   160,   161,   162,   163,   164,   165,   166,   167,   508,
      -1,    -1,    -1,   160,   161,   162,   163,   164,   165,   166,
     167,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,   107,    -1,    -1,   110,   111
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      20,    25,    26,    27,    74,   150,   170,   176,   177,   178,
     180,   189,   190,   192,   194,   197,   198,   199,   200,   201,
     223,   228,   229,   230,   231,    28,    29,    30,    40,    41,
      32,    28,    29,    30,    40,    41,     3,   221,    72,   221,
     147,   148,    34,    35,    36,    37,    38,    39,    42,   193,
      31,    43,    44,   147,   148,   152,   153,    41,   221,   142,
     143,   144,     3,   221,     3,   224,   225,   151,   200,   201,
       0,   173,   263,    21,    23,    24,   217,     8,   202,   204,
      68,   262,   262,   262,   262,   262,   264,   221,    68,   261,
     261,   261,   261,   261,   172,    14,   221,    72,    73,     3,
       3,   179,   180,   189,   190,   194,   197,   198,   199,   228,
     229,   230,   231,   154,   154,   221,    45,    46,    51,   174,
       6,   171,   171,   178,    22,   170,   203,   204,    61,   211,
      63,   205,    69,     3,   221,   221,   221,     3,    58,   170,
     191,    70,     3,   221,   221,   221,     3,     3,     3,   195,
     196,    62,   214,     4,   260,   260,     4,     5,     6,    58,
     149,     4,     5,     6,    58,   149,     6,   170,   225,   260,
     203,   204,     3,     4,     5,     6,    69,    70,    78,    80,
      96,    97,    98,    99,   118,   119,   120,   140,   145,   156,
     157,   158,   163,   164,   165,   168,   170,   232,   234,   235,
     236,   237,   238,   239,   240,   242,   243,   244,   245,   246,
     248,   249,   250,   251,   253,   254,   255,   256,   257,    49,
      64,   209,    70,    51,   170,   191,   221,     3,   188,    33,
     201,    58,   160,   174,   214,   235,    74,    74,    47,   202,
     171,   171,   170,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,    69,    70,   236,   170,   170,
      83,   235,   247,     4,     4,     4,     4,     6,   257,   170,
     170,   170,   170,   170,   236,   236,     5,     6,   201,   235,
      72,   174,   212,    51,   138,   139,    69,    71,   117,   133,
     134,   135,   136,   137,   141,   160,   161,   162,   163,   164,
     165,   166,   167,   172,   169,   174,   169,   174,   206,   207,
     235,   235,    65,   210,   199,     3,   113,   115,   181,   182,
     183,   187,    51,   170,   268,   171,   174,   170,   233,   221,
     235,   196,   170,   170,     6,   171,    61,   171,   232,   170,
      69,   201,   235,   235,   247,    79,    81,    83,     4,   235,
       4,     4,     4,   171,   171,   170,   218,   219,   220,   221,
     226,   234,   155,   213,     3,   235,   235,    71,   141,   170,
      69,   116,   236,   236,   236,   236,   236,   236,   236,   236,
     236,   236,   236,   236,   236,   236,     3,   165,     6,     5,
     174,    66,    67,   208,   235,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   107,   110,   111,   184,
     114,   170,   171,   174,   199,   188,   170,     3,   232,   174,
      75,    76,    77,   258,   259,   258,   232,   171,   201,   171,
      51,    82,    79,    81,   235,   235,    72,   174,   174,   171,
     174,   171,   174,   199,   174,    52,    54,    55,    56,    57,
      59,    60,   227,     3,    51,   222,   237,   238,   239,   240,
     241,   214,   170,   236,   201,   232,   116,   138,   207,   170,
     170,   170,    69,   113,   115,   116,   185,   186,   170,   188,
     182,   171,   188,   171,   170,     4,     3,   171,   174,   171,
     171,   171,   184,   235,   235,    79,    82,   236,   252,   253,
     255,     4,     4,     4,   171,   219,    53,    53,     3,   174,
      48,   216,   201,   232,   171,   171,   236,     6,    85,    86,
      87,    88,    89,    92,    93,   112,    85,    86,    87,    88,
      89,    92,    93,   112,   116,   114,   186,   188,   171,   146,
     171,   232,   259,   171,    79,   235,   171,   174,   171,   174,
     171,   171,   222,   220,   220,   170,   237,   238,   239,   240,
      49,    50,   215,   171,   171,   171,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   171,     3,   146,   171,     4,     4,    58,
     188,   232,   235,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
      74,   265,     3,   174,   171,   235,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   170,   265,     4,     3,   266,   267,
     174,   160,   171,   174,     6,     3,     5,     6,   267,   171,
     265
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   175,   176,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   180,   180,
     180,   180,   180,   180,   181,   181,   182,   182,   183,   183,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     185,   185,   186,   186,   186,   186,   187,   187,   188,   188,
     189,   190,   190,   191,   191,   192,   193,   193,   193,   193,
     193,   193,   193,   193,   194,   195,   195,   196,   197,   197,
     197,   197,   197,   198,   198,   199,   199,   199,   199,   200,
     200,   201,   202,   203,   203,   204,   205,   205,   206,   206,
     207,   208,   208,   208,   209,   209,   210,   210,   211,   211,
     212,   212,   213,   213,   214,   214,   215,   215,   216,   216,
     217,   217,   217,   217,   218,   218,   219,   219,   220,   220,
     221,   221,   222,   222,   222,   222,   223,   223,   224,   224,
     225,   226,   226,   227,   227,   227,   227,   227,   227,   227,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   228,
     228,   228,   229,   229,   229,   230,   231,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   232,   232,
     233,   233,   234,   234,   235,   235,   235,   235,   235,   236,
     236,   236,   236,   236,   236,   236,   236,   236,   236,   236,
     237,   238,   238,   239,   239,   240,   240,   241,   241,   241,
     241,   241,   241,   241,   241,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   243,   243,
     244,   245,   245,   246,   246,   246,   246,   247,   247,   248,
     249,   249,   249,   249,   250,   250,   250,   250,   251,   251,
     251,   251,   251,   251,   251,   251,   251,   251,   251,   251,
     251,   252,   252,   253,   254,   254,   255,   256,   256,   257,
     257,   257,   257,   257,   257,   257,   257,   257,   257,   257,
     257,   258,   258,   259,   259,   259,   260,   261,   261,   262,
     262,   263,   263,   264,   264,   265,   265,   266,   266,   267,
     267,   267,   267,   268,   268
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
       1,     6,     4,     1,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       1,     2,     2,     1,     1,     2,     5,     4,     1,     3,
       4,     6,     5,     3,     0,     3,     1,     1,     1,     1,
       1,     1,     1,     0,     5,     1,     3,     3,     4,     4,
       4,     4,     6,     8,     8,     1,     1,     3,     3,     3,
       3,     2,     4,     3,     3,     8,     3,     0,     1,     3,
       2,     1,     1,     0,     2,     0,     2,     0,     1,     0,
       2,     0,     2,     0,     2,     0,     2,     0,     3,     0,
       1,     2,     1,     1,     1,     3,     1,     1,     2,     4,
       1,     3,     2,     1,     5,     0,     2,     0,     1,     3,
       5,     4,     6,     1,     1,     1,     1,     1,     1,     0,
       2,     2,     2,     2,     2,     3,     3,     2,     3,     4,
       6,     3,     2,     2,     2,     2,     2,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     1,     3,
       3,     5,     3,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
      13,     6,     8,     4,     6,     4,     6,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     4,     5,     4,     3,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     6,     3,     4,     3,     3,
       5,     5,     6,     4,     6,     3,     5,     4,     5,     6,
       4,     5,     5,     6,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     2,     2,     2,     2,     2,     1,     1,
       1,     1,     1,     2,     2,     3,     2,     2,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     1,     3,     2,     2,     1,     1,     2,     0,     3,
       0,     1,     0,     2,     0,     4,     0,     1,     3,     1,
       3,     3,     3,     6,     7
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
#line 1990 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 298 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1998 "parser.cpp"
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
#line 2012 "parser.cpp"
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
#line 2026 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 291 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2037 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2046 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 302 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2055 "parser.cpp"
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
#line 2069 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 255 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2080 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2090 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2100 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2110 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2120 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2130 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 337 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2140 "parser.cpp"
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
#line 2154 "parser.cpp"
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
#line 2168 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 325 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2178 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2186 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2194 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2203 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2211 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2219 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2227 "parser.cpp"
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
#line 2241 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2250 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2259 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2268 "parser.cpp"
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
#line 2281 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 315 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2290 "parser.cpp"
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
#line 2304 "parser.cpp"
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
#line 2318 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 331 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2328 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 320 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2337 "parser.cpp"
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
#line 2351 "parser.cpp"
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
#line 2368 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2376 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2384 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2392 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2400 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2408 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2416 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2424 "parser.cpp"
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
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2446 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2454 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2462 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2470 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2478 "parser.cpp"
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
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2507 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 307 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2515 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2563 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 311 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2571 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 298 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2579 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 195 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2590 "parser.cpp"
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
#line 2604 "parser.cpp"
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
#line 2618 "parser.cpp"
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

#line 2726 "parser.cpp"

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
#line 2941 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 470 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2952 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 476 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2963 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 483 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2969 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 484 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2975 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 485 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2981 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 486 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2987 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 487 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2993 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 488 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2999 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 489 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3005 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 490 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3011 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 491 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 3017 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 492 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3023 "parser.cpp"
    break;

  case 15: /* statement: optimize_statement  */
#line 493 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3029 "parser.cpp"
    break;

  case 16: /* statement: command_statement  */
#line 494 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3035 "parser.cpp"
    break;

  case 17: /* explainable_statement: create_statement  */
#line 496 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3041 "parser.cpp"
    break;

  case 18: /* explainable_statement: drop_statement  */
#line 497 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3047 "parser.cpp"
    break;

  case 19: /* explainable_statement: copy_statement  */
#line 498 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3053 "parser.cpp"
    break;

  case 20: /* explainable_statement: show_statement  */
#line 499 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3059 "parser.cpp"
    break;

  case 21: /* explainable_statement: select_statement  */
#line 500 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3065 "parser.cpp"
    break;

  case 22: /* explainable_statement: delete_statement  */
#line 501 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3071 "parser.cpp"
    break;

  case 23: /* explainable_statement: update_statement  */
#line 502 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3077 "parser.cpp"
    break;

  case 24: /* explainable_statement: insert_statement  */
#line 503 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3083 "parser.cpp"
    break;

  case 25: /* explainable_statement: flush_statement  */
#line 504 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3089 "parser.cpp"
    break;

  case 26: /* explainable_statement: optimize_statement  */
#line 505 "parser.y"
                     { (yyval.base_stmt) = (yyvsp[0].optimize_stmt); }
#line 3095 "parser.cpp"
    break;

  case 27: /* explainable_statement: command_statement  */
#line 506 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3101 "parser.cpp"
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
#line 3121 "parser.cpp"
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
#line 3139 "parser.cpp"
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
#line 3167 "parser.cpp"
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
#line 3187 "parser.cpp"
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
#line 3208 "parser.cpp"
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
#line 3241 "parser.cpp"
    break;

  case 34: /* table_element_array: table_element  */
#line 632 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3250 "parser.cpp"
    break;

  case 35: /* table_element_array: table_element_array ',' table_element  */
#line 636 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3259 "parser.cpp"
    break;

  case 36: /* table_element: table_column  */
#line 642 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3267 "parser.cpp"
    break;

  case 37: /* table_element: table_constraint  */
#line 645 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3275 "parser.cpp"
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
#line 3315 "parser.cpp"
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
#line 3352 "parser.cpp"
    break;

  case 40: /* column_type: BOOLEAN  */
#line 720 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3358 "parser.cpp"
    break;

  case 41: /* column_type: TINYINT  */
#line 721 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3364 "parser.cpp"
    break;

  case 42: /* column_type: SMALLINT  */
#line 722 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3370 "parser.cpp"
    break;

  case 43: /* column_type: INTEGER  */
#line 723 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3376 "parser.cpp"
    break;

  case 44: /* column_type: INT  */
#line 724 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3382 "parser.cpp"
    break;

  case 45: /* column_type: BIGINT  */
#line 725 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3388 "parser.cpp"
    break;

  case 46: /* column_type: HUGEINT  */
#line 726 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3394 "parser.cpp"
    break;

  case 47: /* column_type: FLOAT  */
#line 727 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3400 "parser.cpp"
    break;

  case 48: /* column_type: REAL  */
#line 728 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3406 "parser.cpp"
    break;

  case 49: /* column_type: DOUBLE  */
#line 729 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3412 "parser.cpp"
    break;

  case 50: /* column_type: DATE  */
#line 730 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3418 "parser.cpp"
    break;

  case 51: /* column_type: TIME  */
#line 731 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3424 "parser.cpp"
    break;

  case 52: /* column_type: DATETIME  */
#line 732 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3430 "parser.cpp"
    break;

  case 53: /* column_type: TIMESTAMP  */
#line 733 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3436 "parser.cpp"
    break;

  case 54: /* column_type: UUID  */
#line 734 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3442 "parser.cpp"
    break;

  case 55: /* column_type: POINT  */
#line 735 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3448 "parser.cpp"
    break;

  case 56: /* column_type: LINE  */
#line 736 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3454 "parser.cpp"
    break;

  case 57: /* column_type: LSEG  */
#line 737 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3460 "parser.cpp"
    break;

  case 58: /* column_type: BOX  */
#line 738 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3466 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 741 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3472 "parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 743 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3478 "parser.cpp"
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 744 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value), infinity::EmbeddingDataType::kElemInvalid}; }
#line 3484 "parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 745 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3490 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL  */
#line 746 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0, infinity::EmbeddingDataType::kElemInvalid}; }
#line 3496 "parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 749 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3502 "parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 750 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3508 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 751 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3514 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 752 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3520 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 753 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3526 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 754 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3532 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 755 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3538 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 756 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3544 "parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3550 "parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 758 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3556 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 759 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3562 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 760 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3568 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3574 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 762 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3580 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 763 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3586 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 764 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3592 "parser.cpp"
    break;

  case 80: /* column_constraints: column_constraint  */
#line 783 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3601 "parser.cpp"
    break;

  case 81: /* column_constraints: column_constraints column_constraint  */
#line 787 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
#line 3615 "parser.cpp"
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 797 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3623 "parser.cpp"
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 800 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3631 "parser.cpp"
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 803 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3639 "parser.cpp"
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 806 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3647 "parser.cpp"
    break;

  case 86: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 810 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3657 "parser.cpp"
    break;

  case 87: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 815 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3667 "parser.cpp"
    break;

  case 88: /* identifier_array: IDENTIFIER  */
#line 822 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3678 "parser.cpp"
    break;

  case 89: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 828 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3689 "parser.cpp"
    break;

  case 90: /* delete_statement: DELETE FROM table_name where_clause  */
#line 838 "parser.y"
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
#line 3706 "parser.cpp"
    break;

  case 91: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 854 "parser.y"
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
#line 3745 "parser.cpp"
    break;

  case 92: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 888 "parser.y"
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
#line 3762 "parser.cpp"
    break;

  case 93: /* optional_identifier_array: '(' identifier_array ')'  */
#line 901 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3770 "parser.cpp"
    break;

  case 94: /* optional_identifier_array: %empty  */
#line 904 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3778 "parser.cpp"
    break;

  case 95: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 911 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3788 "parser.cpp"
    break;

  case 96: /* explain_type: ANALYZE  */
#line 917 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3796 "parser.cpp"
    break;

  case 97: /* explain_type: AST  */
#line 920 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3804 "parser.cpp"
    break;

  case 98: /* explain_type: RAW  */
#line 923 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3812 "parser.cpp"
    break;

  case 99: /* explain_type: LOGICAL  */
#line 926 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3820 "parser.cpp"
    break;

  case 100: /* explain_type: PHYSICAL  */
#line 929 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3828 "parser.cpp"
    break;

  case 101: /* explain_type: PIPELINE  */
#line 932 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3836 "parser.cpp"
    break;

  case 102: /* explain_type: FRAGMENT  */
#line 935 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3844 "parser.cpp"
    break;

  case 103: /* explain_type: %empty  */
#line 938 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3852 "parser.cpp"
    break;

  case 104: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 945 "parser.y"
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
#line 3869 "parser.cpp"
    break;

  case 105: /* update_expr_array: update_expr  */
#line 958 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3878 "parser.cpp"
    break;

  case 106: /* update_expr_array: update_expr_array ',' update_expr  */
#line 962 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3887 "parser.cpp"
    break;

  case 107: /* update_expr: IDENTIFIER '=' expr  */
#line 967 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3899 "parser.cpp"
    break;

  case 108: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 980 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3915 "parser.cpp"
    break;

  case 109: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 993 "parser.y"
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
#line 3933 "parser.cpp"
    break;

  case 110: /* drop_statement: DROP TABLE if_exists table_name  */
#line 1008 "parser.y"
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
#line 3951 "parser.cpp"
    break;

  case 111: /* drop_statement: DROP VIEW if_exists table_name  */
#line 1023 "parser.y"
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
#line 3969 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1038 "parser.y"
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
#line 3992 "parser.cpp"
    break;

  case 113: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1061 "parser.y"
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
#line 4038 "parser.cpp"
    break;

  case 114: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1102 "parser.y"
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
#line 4084 "parser.cpp"
    break;

  case 115: /* select_statement: select_without_paren  */
#line 1147 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4092 "parser.cpp"
    break;

  case 116: /* select_statement: select_with_paren  */
#line 1150 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4100 "parser.cpp"
    break;

  case 117: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1153 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4114 "parser.cpp"
    break;

  case 118: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1162 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
#line 4128 "parser.cpp"
    break;

  case 119: /* select_with_paren: '(' select_without_paren ')'  */
#line 1172 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4136 "parser.cpp"
    break;

  case 120: /* select_with_paren: '(' select_with_paren ')'  */
#line 1175 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4144 "parser.cpp"
    break;

  case 121: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1179 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4153 "parser.cpp"
    break;

  case 122: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1184 "parser.y"
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
#line 4179 "parser.cpp"
    break;

  case 123: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1206 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4187 "parser.cpp"
    break;

  case 124: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1209 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4195 "parser.cpp"
    break;

  case 125: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1214 "parser.y"
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
#line 4215 "parser.cpp"
    break;

  case 126: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1230 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4223 "parser.cpp"
    break;

  case 127: /* order_by_clause: %empty  */
#line 1233 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4231 "parser.cpp"
    break;

  case 128: /* order_by_expr_list: order_by_expr  */
#line 1237 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4240 "parser.cpp"
    break;

  case 129: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1241 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4249 "parser.cpp"
    break;

  case 130: /* order_by_expr: expr order_by_type  */
#line 1246 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4259 "parser.cpp"
    break;

  case 131: /* order_by_type: ASC  */
#line 1252 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4267 "parser.cpp"
    break;

  case 132: /* order_by_type: DESC  */
#line 1255 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4275 "parser.cpp"
    break;

  case 133: /* order_by_type: %empty  */
#line 1258 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4283 "parser.cpp"
    break;

  case 134: /* limit_expr: LIMIT expr  */
#line 1262 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4291 "parser.cpp"
    break;

  case 135: /* limit_expr: %empty  */
#line 1266 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4297 "parser.cpp"
    break;

  case 136: /* offset_expr: OFFSET expr  */
#line 1268 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4305 "parser.cpp"
    break;

  case 137: /* offset_expr: %empty  */
#line 1272 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4311 "parser.cpp"
    break;

  case 138: /* distinct: DISTINCT  */
#line 1274 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4319 "parser.cpp"
    break;

  case 139: /* distinct: %empty  */
#line 1277 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4327 "parser.cpp"
    break;

  case 140: /* from_clause: FROM table_reference  */
#line 1281 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4335 "parser.cpp"
    break;

  case 141: /* from_clause: %empty  */
#line 1284 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4343 "parser.cpp"
    break;

  case 142: /* search_clause: SEARCH sub_search_array  */
#line 1288 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4353 "parser.cpp"
    break;

  case 143: /* search_clause: %empty  */
#line 1293 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4361 "parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1297 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4369 "parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1300 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4377 "parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1304 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4385 "parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1307 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4393 "parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1311 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4401 "parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1314 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4409 "parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1318 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4417 "parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1321 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4425 "parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1324 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4433 "parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1327 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4441 "parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1335 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4449 "parser.cpp"
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1338 "parser.y"
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
#line 4467 "parser.cpp"
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1355 "parser.y"
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
#line 4485 "parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1369 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4496 "parser.cpp"
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1378 "parser.y"
                        {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4506 "parser.cpp"
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1383 "parser.y"
                            {
    (yyval.table_name_t) = new infinity::TableName();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
    (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
}
#line 4518 "parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1392 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4528 "parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1397 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4538 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1402 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4549 "parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1408 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4557 "parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1415 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4565 "parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1418 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4573 "parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1422 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4582 "parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1425 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4591 "parser.cpp"
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1430 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4603 "parser.cpp"
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1442 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4615 "parser.cpp"
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1449 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
#line 4628 "parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1463 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4636 "parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1466 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4644 "parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1469 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4652 "parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1472 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4660 "parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1475 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4668 "parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1478 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4676 "parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1481 "parser.y"
                {
}
#line 4683 "parser.cpp"
    break;

  case 180: /* show_statement: SHOW DATABASES  */
#line 1487 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4692 "parser.cpp"
    break;

  case 181: /* show_statement: SHOW TABLES  */
#line 1491 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4701 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW VIEWS  */
#line 1495 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4710 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW CONFIGS  */
#line 1499 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4719 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW PROFILES  */
#line 1503 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4728 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW SESSION STATUS  */
#line 1507 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4737 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW GLOBAL STATUS  */
#line 1511 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4746 "parser.cpp"
    break;

  case 187: /* show_statement: DESCRIBE table_name  */
#line 1515 "parser.y"
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
#line 4762 "parser.cpp"
    break;

  case 188: /* show_statement: DESCRIBE table_name SEGMENTS  */
#line 1526 "parser.y"
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
#line 4778 "parser.cpp"
    break;

  case 189: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE  */
#line 1537 "parser.y"
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
#line 4795 "parser.cpp"
    break;

  case 190: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1549 "parser.y"
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
#line 4813 "parser.cpp"
    break;

  case 191: /* show_statement: DESCRIBE INDEX table_name  */
#line 1562 "parser.y"
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
#line 4829 "parser.cpp"
    break;

  case 192: /* flush_statement: FLUSH DATA  */
#line 1577 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4838 "parser.cpp"
    break;

  case 193: /* flush_statement: FLUSH LOG  */
#line 1581 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4847 "parser.cpp"
    break;

  case 194: /* flush_statement: FLUSH BUFFER  */
#line 1585 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4856 "parser.cpp"
    break;

  case 195: /* optimize_statement: OPTIMIZE table_name  */
#line 1593 "parser.y"
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
#line 4872 "parser.cpp"
    break;

  case 196: /* command_statement: USE IDENTIFIER  */
#line 1608 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4883 "parser.cpp"
    break;

  case 197: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1614 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4893 "parser.cpp"
    break;

  case 198: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1619 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4904 "parser.cpp"
    break;

  case 199: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1625 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4915 "parser.cpp"
    break;

  case 200: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1631 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4928 "parser.cpp"
    break;

  case 201: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1639 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4939 "parser.cpp"
    break;

  case 202: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1645 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4950 "parser.cpp"
    break;

  case 203: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1651 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4961 "parser.cpp"
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1657 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4972 "parser.cpp"
    break;

  case 205: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1663 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4985 "parser.cpp"
    break;

  case 206: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1671 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4996 "parser.cpp"
    break;

  case 207: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1677 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 5007 "parser.cpp"
    break;

  case 208: /* expr_array: expr_alias  */
#line 1688 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5016 "parser.cpp"
    break;

  case 209: /* expr_array: expr_array ',' expr_alias  */
#line 1692 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5025 "parser.cpp"
    break;

  case 210: /* expr_array_list: '(' expr_array ')'  */
#line 1697 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 5034 "parser.cpp"
    break;

  case 211: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1701 "parser.y"
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
#line 5054 "parser.cpp"
    break;

  case 212: /* expr_alias: expr AS IDENTIFIER  */
#line 1728 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 5065 "parser.cpp"
    break;

  case 213: /* expr_alias: expr  */
#line 1734 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 5073 "parser.cpp"
    break;

  case 219: /* operand: '(' expr ')'  */
#line 1744 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 5081 "parser.cpp"
    break;

  case 220: /* operand: '(' select_without_paren ')'  */
#line 1747 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5092 "parser.cpp"
    break;

  case 221: /* operand: constant_expr  */
#line 1753 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5100 "parser.cpp"
    break;

  case 230: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ',' LONG_VALUE ')' with_index_param_list  */
#line 1765 "parser.y"
                                                                                                      {
    infinity::KnnExpr* knn_expr = new infinity::KnnExpr();
    (yyval.expr_t) = knn_expr;

    // KNN search column
    knn_expr->column_expr_ = (yyvsp[-10].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-4].str_value));
    if(strcmp((yyvsp[-4].str_value), "l2") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-4].str_value), "ip") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-4].str_value), "cosine") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-4].str_value), "hamming") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
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
    if(strcmp((yyvsp[-6].str_value), "float") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if(!((yyvsp[-8].const_expr_t)->double_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->double_array_[i];
            }
        }
        if(!((yyvsp[-8].const_expr_t)->long_array_.empty())) {
            knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
            knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];
            for(long i = 0; i < knn_expr->dimension_; ++ i) {
                ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
            }
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "tinyint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_]{};

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((char*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "smallint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        knn_expr->embedding_data_ptr_ = new short int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((short int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);
    } else if(strcmp((yyvsp[-6].str_value), "integer") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        knn_expr->embedding_data_ptr_ = new int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-8].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bigint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        knn_expr->embedding_data_ptr_ = new long[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->long_array_.data(), knn_expr->dimension_ * sizeof(long));
        free((yyvsp[-6].str_value));
        free((yyvsp[-4].str_value));
        delete (yyvsp[-8].const_expr_t);

    } else if(strcmp((yyvsp[-6].str_value), "bit") == 0 and knn_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->long_array_.size();
        if(knn_expr->dimension_ % 8 == 0) {
            knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = knn_expr->dimension_ / 8;
            knn_expr->embedding_data_ptr_ = new char[embedding_size]{};

            for(long i = 0; i < embedding_size; ++ i) {
                char embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        char temp = embedding_unit << 1;
                        temp &= 1;
                        embedding_unit = temp;
                    } else if((yyvsp[-8].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        embedding_unit <<= 0;
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
                ((char*)knn_expr->embedding_data_ptr_)[i] = embedding_unit;
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

    } else if(strcmp((yyvsp[-6].str_value), "double") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-8].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        knn_expr->embedding_data_ptr_ = new double[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-8].const_expr_t)->double_array_.data(), knn_expr->dimension_ * sizeof(double));
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
    knn_expr->topn_ = (yyvsp[-2].long_value);
    knn_expr->opt_params_ = (yyvsp[0].with_index_param_list_t);
}
#line 5271 "parser.cpp"
    break;

  case 231: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1932 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5284 "parser.cpp"
    break;

  case 232: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1940 "parser.y"
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
#line 5299 "parser.cpp"
    break;

  case 233: /* query_expr: QUERY '(' STRING ')'  */
#line 1951 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5310 "parser.cpp"
    break;

  case 234: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1957 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5323 "parser.cpp"
    break;

  case 235: /* fusion_expr: FUSION '(' STRING ')'  */
#line 1966 "parser.y"
                                    {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5334 "parser.cpp"
    break;

  case 236: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1972 "parser.y"
                                   {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5347 "parser.cpp"
    break;

  case 237: /* sub_search_array: knn_expr  */
#line 1982 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5356 "parser.cpp"
    break;

  case 238: /* sub_search_array: match_expr  */
#line 1986 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5365 "parser.cpp"
    break;

  case 239: /* sub_search_array: query_expr  */
#line 1990 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5374 "parser.cpp"
    break;

  case 240: /* sub_search_array: fusion_expr  */
#line 1994 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5383 "parser.cpp"
    break;

  case 241: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 1998 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5392 "parser.cpp"
    break;

  case 242: /* sub_search_array: sub_search_array ',' match_expr  */
#line 2002 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5401 "parser.cpp"
    break;

  case 243: /* sub_search_array: sub_search_array ',' query_expr  */
#line 2006 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5410 "parser.cpp"
    break;

  case 244: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 2010 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5419 "parser.cpp"
    break;

  case 245: /* function_expr: IDENTIFIER '(' ')'  */
#line 2015 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5432 "parser.cpp"
    break;

  case 246: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 2023 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5445 "parser.cpp"
    break;

  case 247: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 2031 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5459 "parser.cpp"
    break;

  case 248: /* function_expr: operand IS NOT NULLABLE  */
#line 2040 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5471 "parser.cpp"
    break;

  case 249: /* function_expr: operand IS NULLABLE  */
#line 2047 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5483 "parser.cpp"
    break;

  case 250: /* function_expr: NOT operand  */
#line 2054 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5495 "parser.cpp"
    break;

  case 251: /* function_expr: '-' operand  */
#line 2061 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5507 "parser.cpp"
    break;

  case 252: /* function_expr: '+' operand  */
#line 2068 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5519 "parser.cpp"
    break;

  case 253: /* function_expr: operand '-' operand  */
#line 2075 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5532 "parser.cpp"
    break;

  case 254: /* function_expr: operand '+' operand  */
#line 2083 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5545 "parser.cpp"
    break;

  case 255: /* function_expr: operand '*' operand  */
#line 2091 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5558 "parser.cpp"
    break;

  case 256: /* function_expr: operand '/' operand  */
#line 2099 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5571 "parser.cpp"
    break;

  case 257: /* function_expr: operand '%' operand  */
#line 2107 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5584 "parser.cpp"
    break;

  case 258: /* function_expr: operand '=' operand  */
#line 2115 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5597 "parser.cpp"
    break;

  case 259: /* function_expr: operand EQUAL operand  */
#line 2123 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5610 "parser.cpp"
    break;

  case 260: /* function_expr: operand NOT_EQ operand  */
#line 2131 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5623 "parser.cpp"
    break;

  case 261: /* function_expr: operand '<' operand  */
#line 2139 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5636 "parser.cpp"
    break;

  case 262: /* function_expr: operand '>' operand  */
#line 2147 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5649 "parser.cpp"
    break;

  case 263: /* function_expr: operand LESS_EQ operand  */
#line 2155 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5662 "parser.cpp"
    break;

  case 264: /* function_expr: operand GREATER_EQ operand  */
#line 2163 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5675 "parser.cpp"
    break;

  case 265: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2171 "parser.y"
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
#line 5710 "parser.cpp"
    break;

  case 266: /* function_expr: operand LIKE operand  */
#line 2201 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5723 "parser.cpp"
    break;

  case 267: /* function_expr: operand NOT LIKE operand  */
#line 2209 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5736 "parser.cpp"
    break;

  case 268: /* conjunction_expr: expr AND expr  */
#line 2218 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5749 "parser.cpp"
    break;

  case 269: /* conjunction_expr: expr OR expr  */
#line 2226 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5762 "parser.cpp"
    break;

  case 270: /* between_expr: operand BETWEEN operand AND operand  */
#line 2235 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5774 "parser.cpp"
    break;

  case 271: /* in_expr: operand IN '(' expr_array ')'  */
#line 2243 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5785 "parser.cpp"
    break;

  case 272: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2249 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5796 "parser.cpp"
    break;

  case 273: /* case_expr: CASE expr case_check_array END  */
#line 2256 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5807 "parser.cpp"
    break;

  case 274: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2262 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5819 "parser.cpp"
    break;

  case 275: /* case_expr: CASE case_check_array END  */
#line 2269 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5829 "parser.cpp"
    break;

  case 276: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2274 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5840 "parser.cpp"
    break;

  case 277: /* case_check_array: WHEN expr THEN expr  */
#line 2281 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5852 "parser.cpp"
    break;

  case 278: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2288 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5864 "parser.cpp"
    break;

  case 279: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2296 "parser.y"
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
#line 5892 "parser.cpp"
    break;

  case 280: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2320 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5903 "parser.cpp"
    break;

  case 281: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2326 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5914 "parser.cpp"
    break;

  case 282: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2332 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5926 "parser.cpp"
    break;

  case 283: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2339 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5938 "parser.cpp"
    break;

  case 284: /* column_expr: IDENTIFIER  */
#line 2347 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5950 "parser.cpp"
    break;

  case 285: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2354 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5962 "parser.cpp"
    break;

  case 286: /* column_expr: '*'  */
#line 2361 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5972 "parser.cpp"
    break;

  case 287: /* column_expr: column_expr '.' '*'  */
#line 2366 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5986 "parser.cpp"
    break;

  case 288: /* constant_expr: STRING  */
#line 2376 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5996 "parser.cpp"
    break;

  case 289: /* constant_expr: TRUE  */
#line 2381 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 6006 "parser.cpp"
    break;

  case 290: /* constant_expr: FALSE  */
#line 2386 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 6016 "parser.cpp"
    break;

  case 291: /* constant_expr: DOUBLE_VALUE  */
#line 2391 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6026 "parser.cpp"
    break;

  case 292: /* constant_expr: LONG_VALUE  */
#line 2396 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6036 "parser.cpp"
    break;

  case 293: /* constant_expr: DATE STRING  */
#line 2401 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6046 "parser.cpp"
    break;

  case 294: /* constant_expr: TIME STRING  */
#line 2406 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6056 "parser.cpp"
    break;

  case 295: /* constant_expr: DATETIME STRING  */
#line 2411 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDateTime);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6066 "parser.cpp"
    break;

  case 296: /* constant_expr: TIMESTAMP STRING  */
#line 2416 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kTimestamp);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6076 "parser.cpp"
    break;

  case 297: /* constant_expr: INTERVAL interval_expr  */
#line 2421 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6084 "parser.cpp"
    break;

  case 298: /* constant_expr: interval_expr  */
#line 2424 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6092 "parser.cpp"
    break;

  case 299: /* constant_expr: long_array_expr  */
#line 2427 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6100 "parser.cpp"
    break;

  case 300: /* constant_expr: double_array_expr  */
#line 2430 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6108 "parser.cpp"
    break;

  case 301: /* array_expr: long_array_expr  */
#line 2434 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6116 "parser.cpp"
    break;

  case 302: /* array_expr: double_array_expr  */
#line 2437 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 6124 "parser.cpp"
    break;

  case 303: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2441 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6132 "parser.cpp"
    break;

  case 304: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2445 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6142 "parser.cpp"
    break;

  case 305: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2450 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6151 "parser.cpp"
    break;

  case 306: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2455 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6159 "parser.cpp"
    break;

  case 307: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2459 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 6169 "parser.cpp"
    break;

  case 308: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2464 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6178 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE SECONDS  */
#line 2469 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6189 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE SECOND  */
#line 2475 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6200 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE MINUTES  */
#line 2481 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6211 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE MINUTE  */
#line 2487 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6222 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE HOURS  */
#line 2493 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6233 "parser.cpp"
    break;

  case 314: /* interval_expr: LONG_VALUE HOUR  */
#line 2499 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6244 "parser.cpp"
    break;

  case 315: /* interval_expr: LONG_VALUE DAYS  */
#line 2505 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6255 "parser.cpp"
    break;

  case 316: /* interval_expr: LONG_VALUE DAY  */
#line 2511 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6266 "parser.cpp"
    break;

  case 317: /* interval_expr: LONG_VALUE MONTHS  */
#line 2517 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6277 "parser.cpp"
    break;

  case 318: /* interval_expr: LONG_VALUE MONTH  */
#line 2523 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6288 "parser.cpp"
    break;

  case 319: /* interval_expr: LONG_VALUE YEARS  */
#line 2529 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6299 "parser.cpp"
    break;

  case 320: /* interval_expr: LONG_VALUE YEAR  */
#line 2535 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6310 "parser.cpp"
    break;

  case 321: /* copy_option_list: copy_option  */
#line 2546 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6319 "parser.cpp"
    break;

  case 322: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2550 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6328 "parser.cpp"
    break;

  case 323: /* copy_option: FORMAT IDENTIFIER  */
#line 2555 "parser.y"
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
#line 6355 "parser.cpp"
    break;

  case 324: /* copy_option: DELIMITER STRING  */
#line 2577 "parser.y"
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
#line 6370 "parser.cpp"
    break;

  case 325: /* copy_option: HEADER  */
#line 2587 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6380 "parser.cpp"
    break;

  case 326: /* file_path: STRING  */
#line 2593 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6388 "parser.cpp"
    break;

  case 327: /* if_exists: IF EXISTS  */
#line 2597 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6394 "parser.cpp"
    break;

  case 328: /* if_exists: %empty  */
#line 2598 "parser.y"
  { (yyval.bool_value) = false; }
#line 6400 "parser.cpp"
    break;

  case 329: /* if_not_exists: IF NOT EXISTS  */
#line 2600 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6406 "parser.cpp"
    break;

  case 330: /* if_not_exists: %empty  */
#line 2601 "parser.y"
  { (yyval.bool_value) = false; }
#line 6412 "parser.cpp"
    break;

  case 333: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2616 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6425 "parser.cpp"
    break;

  case 334: /* if_not_exists_info: %empty  */
#line 2624 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6433 "parser.cpp"
    break;

  case 335: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2628 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6441 "parser.cpp"
    break;

  case 336: /* with_index_param_list: %empty  */
#line 2631 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6449 "parser.cpp"
    break;

  case 337: /* index_param_list: index_param  */
#line 2635 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6458 "parser.cpp"
    break;

  case 338: /* index_param_list: index_param_list ',' index_param  */
#line 2639 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6467 "parser.cpp"
    break;

  case 339: /* index_param: IDENTIFIER  */
#line 2644 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6477 "parser.cpp"
    break;

  case 340: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2649 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6490 "parser.cpp"
    break;

  case 341: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2657 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6502 "parser.cpp"
    break;

  case 342: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2664 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6514 "parser.cpp"
    break;

  case 343: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2675 "parser.y"
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
#line 6567 "parser.cpp"
    break;

  case 344: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2723 "parser.y"
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
#line 6621 "parser.cpp"
    break;


#line 6625 "parser.cpp"

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

#line 2773 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

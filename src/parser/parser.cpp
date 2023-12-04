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
  YYSYMBOL_DATABASE = 27,                  /* DATABASE  */
  YYSYMBOL_TABLE = 28,                     /* TABLE  */
  YYSYMBOL_COLLECTION = 29,                /* COLLECTION  */
  YYSYMBOL_TABLES = 30,                    /* TABLES  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_AST = 33,                       /* AST  */
  YYSYMBOL_PIPELINE = 34,                  /* PIPELINE  */
  YYSYMBOL_RAW = 35,                       /* RAW  */
  YYSYMBOL_LOGICAL = 36,                   /* LOGICAL  */
  YYSYMBOL_PHYSICAL = 37,                  /* PHYSICAL  */
  YYSYMBOL_FRAGMENT = 38,                  /* FRAGMENT  */
  YYSYMBOL_VIEW = 39,                      /* VIEW  */
  YYSYMBOL_INDEX = 40,                     /* INDEX  */
  YYSYMBOL_ANALYZE = 41,                   /* ANALYZE  */
  YYSYMBOL_VIEWS = 42,                     /* VIEWS  */
  YYSYMBOL_DATABASES = 43,                 /* DATABASES  */
  YYSYMBOL_SEGMENT = 44,                   /* SEGMENT  */
  YYSYMBOL_SEGMENTS = 45,                  /* SEGMENTS  */
  YYSYMBOL_BLOCK = 46,                     /* BLOCK  */
  YYSYMBOL_GROUP = 47,                     /* GROUP  */
  YYSYMBOL_BY = 48,                        /* BY  */
  YYSYMBOL_HAVING = 49,                    /* HAVING  */
  YYSYMBOL_AS = 50,                        /* AS  */
  YYSYMBOL_NATURAL = 51,                   /* NATURAL  */
  YYSYMBOL_JOIN = 52,                      /* JOIN  */
  YYSYMBOL_LEFT = 53,                      /* LEFT  */
  YYSYMBOL_RIGHT = 54,                     /* RIGHT  */
  YYSYMBOL_OUTER = 55,                     /* OUTER  */
  YYSYMBOL_FULL = 56,                      /* FULL  */
  YYSYMBOL_ON = 57,                        /* ON  */
  YYSYMBOL_INNER = 58,                     /* INNER  */
  YYSYMBOL_CROSS = 59,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 60,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 61,                     /* WHERE  */
  YYSYMBOL_ORDER = 62,                     /* ORDER  */
  YYSYMBOL_LIMIT = 63,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 64,                    /* OFFSET  */
  YYSYMBOL_ASC = 65,                       /* ASC  */
  YYSYMBOL_DESC = 66,                      /* DESC  */
  YYSYMBOL_IF = 67,                        /* IF  */
  YYSYMBOL_NOT = 68,                       /* NOT  */
  YYSYMBOL_EXISTS = 69,                    /* EXISTS  */
  YYSYMBOL_IN = 70,                        /* IN  */
  YYSYMBOL_FROM = 71,                      /* FROM  */
  YYSYMBOL_TO = 72,                        /* TO  */
  YYSYMBOL_WITH = 73,                      /* WITH  */
  YYSYMBOL_DELIMITER = 74,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 75,                    /* FORMAT  */
  YYSYMBOL_HEADER = 76,                    /* HEADER  */
  YYSYMBOL_CAST = 77,                      /* CAST  */
  YYSYMBOL_END = 78,                       /* END  */
  YYSYMBOL_CASE = 79,                      /* CASE  */
  YYSYMBOL_ELSE = 80,                      /* ELSE  */
  YYSYMBOL_THEN = 81,                      /* THEN  */
  YYSYMBOL_WHEN = 82,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 83,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 84,                   /* INTEGER  */
  YYSYMBOL_INT = 85,                       /* INT  */
  YYSYMBOL_TINYINT = 86,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 87,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 88,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 89,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 90,                      /* CHAR  */
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
  YYSYMBOL_command_statement = 230,        /* command_statement  */
  YYSYMBOL_expr_array = 231,               /* expr_array  */
  YYSYMBOL_expr_array_list = 232,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 233,               /* expr_alias  */
  YYSYMBOL_expr = 234,                     /* expr  */
  YYSYMBOL_operand = 235,                  /* operand  */
  YYSYMBOL_knn_expr = 236,                 /* knn_expr  */
  YYSYMBOL_match_expr = 237,               /* match_expr  */
  YYSYMBOL_query_expr = 238,               /* query_expr  */
  YYSYMBOL_fusion_expr = 239,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 240,         /* sub_search_array  */
  YYSYMBOL_function_expr = 241,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 242,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 243,             /* between_expr  */
  YYSYMBOL_in_expr = 244,                  /* in_expr  */
  YYSYMBOL_case_expr = 245,                /* case_expr  */
  YYSYMBOL_case_check_array = 246,         /* case_check_array  */
  YYSYMBOL_cast_expr = 247,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 248,            /* subquery_expr  */
  YYSYMBOL_column_expr = 249,              /* column_expr  */
  YYSYMBOL_constant_expr = 250,            /* constant_expr  */
  YYSYMBOL_array_expr = 251,               /* array_expr  */
  YYSYMBOL_long_array_expr = 252,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 253, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 254,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 255, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 256,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 257,         /* copy_option_list  */
  YYSYMBOL_copy_option = 258,              /* copy_option  */
  YYSYMBOL_file_path = 259,                /* file_path  */
  YYSYMBOL_if_exists = 260,                /* if_exists  */
  YYSYMBOL_if_not_exists = 261,            /* if_not_exists  */
  YYSYMBOL_semicolon = 262,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 263,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 264,    /* with_index_param_list  */
  YYSYMBOL_index_param_list = 265,         /* index_param_list  */
  YYSYMBOL_index_param = 266,              /* index_param  */
  YYSYMBOL_index_info_list = 267           /* index_info_list  */
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
#define YYFINAL  77
/* YYLAST -- Last index in YYTABLE.  */
<<<<<<< HEAD
#define YYLAST   826
=======
#define YYLAST   854
>>>>>>> 13445115 (Add insert embedding syntax.)

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  175
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  93
/* YYNRULES -- Number of rules.  */
#define YYNRULES  337
/* YYNSTATES -- Number of states.  */
<<<<<<< HEAD
#define YYNSTATES  659
=======
#define YYNSTATES  657
>>>>>>> 13445115 (Add insert embedding syntax.)

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
       0,   463,   463,   467,   473,   480,   481,   482,   483,   484,
     485,   486,   487,   488,   489,   490,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   508,   525,   540,   564,
     580,   598,   627,   631,   637,   640,   646,   681,   715,   716,
     717,   718,   719,   720,   721,   722,   723,   724,   725,   726,
     727,   728,   729,   730,   731,   732,   733,   736,   738,   739,
     740,   741,   742,   743,   746,   747,   748,   749,   750,   751,
     752,   753,   754,   755,   756,   757,   758,   759,   760,   761,
     780,   784,   794,   797,   800,   803,   807,   812,   819,   825,
     835,   851,   863,   876,   879,   886,   892,   895,   898,   901,
     904,   907,   910,   913,   920,   933,   937,   942,   955,   968,
     983,   998,  1013,  1036,  1077,  1122,  1125,  1128,  1137,  1147,
    1150,  1154,  1159,  1166,  1169,  1174,  1190,  1193,  1197,  1201,
    1206,  1212,  1215,  1218,  1222,  1226,  1228,  1232,  1234,  1237,
    1241,  1244,  1248,  1253,  1257,  1260,  1264,  1267,  1271,  1274,
    1278,  1281,  1284,  1287,  1295,  1298,  1313,  1313,  1315,  1329,
    1338,  1345,  1356,  1361,  1366,  1372,  1379,  1382,  1386,  1389,
    1394,  1406,  1413,  1427,  1430,  1433,  1436,  1439,  1442,  1445,
<<<<<<< HEAD
    1451,  1455,  1459,  1463,  1467,  1471,  1475,  1479,  1490,  1501,
    1513,  1526,  1541,  1545,  1549,  1557,  1563,  1568,  1574,  1580,
    1588,  1594,  1600,  1606,  1612,  1620,  1626,  1637,  1641,  1646,
    1650,  1677,  1683,  1687,  1688,  1689,  1690,  1691,  1693,  1696,
    1702,  1705,  1706,  1707,  1708,  1709,  1710,  1711,  1712,  1714,
    1855,  1863,  1874,  1880,  1889,  1899,  1903,  1907,  1911,  1915,
    1919,  1923,  1927,  1932,  1940,  1948,  1957,  1964,  1971,  1978,
    1985,  1992,  2000,  2008,  2016,  2024,  2032,  2040,  2048,  2056,
    2064,  2072,  2080,  2088,  2118,  2126,  2135,  2143,  2152,  2160,
    2166,  2173,  2179,  2186,  2191,  2198,  2205,  2213,  2237,  2243,
    2249,  2256,  2264,  2271,  2278,  2283,  2293,  2298,  2303,  2308,
    2313,  2318,  2323,  2326,  2338,  2341,  2345,  2349,  2354,  2359,
    2363,  2368,  2373,  2379,  2385,  2391,  2397,  2403,  2409,  2415,
    2421,  2427,  2433,  2439,  2450,  2454,  2459,  2478,  2488,  2494,
    2498,  2499,  2501,  2502,  2504,  2505,  2517,  2525,  2530,  2533,
    2537,  2541,  2546,  2551,  2559,  2566,  2577,  2625
=======
    1451,  1455,  1459,  1463,  1467,  1471,  1482,  1493,  1505,  1518,
    1533,  1537,  1541,  1549,  1555,  1560,  1566,  1572,  1580,  1586,
    1592,  1598,  1604,  1612,  1618,  1629,  1633,  1638,  1642,  1669,
    1675,  1679,  1680,  1681,  1682,  1683,  1685,  1688,  1694,  1697,
    1698,  1699,  1700,  1701,  1702,  1703,  1704,  1706,  1847,  1855,
    1866,  1872,  1881,  1891,  1895,  1899,  1903,  1907,  1911,  1915,
    1919,  1924,  1932,  1940,  1949,  1956,  1963,  1970,  1977,  1984,
    1992,  2000,  2008,  2016,  2024,  2032,  2040,  2048,  2056,  2064,
    2072,  2080,  2110,  2118,  2127,  2135,  2144,  2152,  2158,  2165,
    2171,  2178,  2183,  2190,  2197,  2205,  2229,  2235,  2241,  2248,
    2256,  2263,  2270,  2275,  2285,  2290,  2295,  2300,  2305,  2310,
<<<<<<< HEAD
    2315,  2318,  2321,  2324,  2328,  2331,  2335,  2340,  2345,  2350,
    2354,  2359,  2364,  2370,  2376,  2382,  2388,  2394,  2400,  2406,
    2412,  2418,  2424,  2430,  2441,  2445,  2450,  2469,  2479,  2485,
    2489,  2490,  2492,  2493,  2495,  2496,  2508,  2516,  2521,  2524,
    2528,  2532,  2537,  2542,  2550,  2557,  2568,  2616
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
    2315,  2318,  2321,  2324,  2328,  2331,  2335,  2339,  2344,  2349,
    2353,  2358,  2363,  2369,  2375,  2381,  2387,  2393,  2399,  2405,
    2411,  2417,  2423,  2429,  2440,  2444,  2449,  2468,  2478,  2484,
    2488,  2489,  2491,  2492,  2494,  2495,  2507,  2515,  2520,  2523,
    2527,  2531,  2536,  2541,  2549,  2556,  2567,  2615
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
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
  "USE", "DATABASE", "TABLE", "COLLECTION", "TABLES", "INTO", "VALUES",
  "AST", "PIPELINE", "RAW", "LOGICAL", "PHYSICAL", "FRAGMENT", "VIEW",
  "INDEX", "ANALYZE", "VIEWS", "DATABASES", "SEGMENT", "SEGMENTS", "BLOCK",
  "GROUP", "BY", "HAVING", "AS", "NATURAL", "JOIN", "LEFT", "RIGHT",
  "OUTER", "FULL", "ON", "INNER", "CROSS", "DISTINCT", "WHERE", "ORDER",
  "LIMIT", "OFFSET", "ASC", "DESC", "IF", "NOT", "EXISTS", "IN", "FROM",
  "TO", "WITH", "DELIMITER", "FORMAT", "HEADER", "CAST", "END", "CASE",
  "ELSE", "THEN", "WHEN", "BOOLEAN", "INTEGER", "INT", "TINYINT",
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
  "show_statement", "flush_statement", "command_statement", "expr_array",
  "expr_array_list", "expr_alias", "expr", "operand", "knn_expr",
  "match_expr", "query_expr", "fusion_expr", "sub_search_array",
  "function_expr", "conjunction_expr", "between_expr", "in_expr",
  "case_expr", "case_check_array", "cast_expr", "subquery_expr",
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

<<<<<<< HEAD
#define YYPACT_NINF (-340)
=======
#define YYPACT_NINF (-350)
>>>>>>> 13445115 (Add insert embedding syntax.)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-328)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
<<<<<<< HEAD
     567,   170,    14,   187,    31,    27,    31,    -3,   389,    38,
      67,    75,    71,   129,   -26,   -56,   137,   -33,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,   309,  -340,  -340,   174,
    -340,  -340,  -340,   117,   117,   117,   117,    28,    31,   128,
     128,   128,   128,   128,    29,   193,    31,    80,   227,   235,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,   588,  -340,  -340,
    -340,    90,    95,  -340,  -340,    31,   180,  -340,  -340,  -340,
    -340,   236,   103,  -340,   282,   135,   141,  -340,    26,  -340,
     295,  -340,  -340,    -1,   262,  -340,   269,   278,   349,    31,
      31,    31,   355,   334,   199,   328,   399,    31,    31,    31,
     400,   410,   416,   368,   444,   444,    51,    54,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,   309,  -340,  -340,  -340,  -340,
    -340,  -340,   448,  -340,   280,   129,   444,  -340,  -340,  -340,
    -340,    -1,  -340,  -340,  -340,   374,   408,   394,   391,  -340,
     -41,  -340,   199,  -340,    31,   455,    -8,  -340,  -340,  -340,
    -340,  -340,   404,  -340,   302,   -47,  -340,   374,  -340,  -340,
     390,   392,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,   418,   174,  -340,  -340,   296,   297,   299,  -340,
    -340,   507,   405,   301,   310,   242,   468,  -340,  -340,   473,
     311,   315,   316,   317,   321,   370,   370,  -340,   276,   -61,
    -340,   -23,   537,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,   323,  -340,  -340,   374,   374,   432,
    -340,   -56,    15,   447,   329,  -340,   -74,   330,  -340,    31,
     374,   416,  -340,   122,   341,   343,   492,   345,  -340,  -340,
     144,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,   370,   347,   599,   439,   374,   374,   -21,
     205,  -340,   507,  -340,   514,   374,   516,   517,   518,   175,
     175,   358,   -51,     3,   374,   381,   538,   374,   374,   -45,
     372,   -40,   370,   370,   370,   370,   370,   370,   370,   370,
     370,   370,   370,   370,   370,   370,     5,   369,  -340,   -46,
     122,   374,  -340,   309,   684,   433,   376,    18,  -340,  -340,
    -340,   -56,   455,   378,  -340,   546,   374,   377,  -340,   122,
    -340,   361,   361,  -340,  -340,   374,  -340,    37,   439,   411,
     382,    39,   -60,   238,  -340,   374,   374,   484,    55,   383,
      68,   384,  -340,  -340,   -56,   385,   312,  -340,    19,  -340,
    -340,   111,   368,  -340,  -340,   422,   386,   370,   276,   449,
    -340,   342,   342,    70,    70,   547,   342,   342,    70,    70,
     175,   175,  -340,  -340,  -340,  -340,  -340,   374,  -340,  -340,
    -340,   122,  -340,  -340,  -340,  -340,  -340,  -340,  -340,   396,
    -340,  -340,  -340,  -340,   397,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,   401,   402,    23,   403,   455,
    -340,    15,   309,   101,   455,  -340,   176,   414,   560,   582,
    -340,   177,  -340,   183,   185,  -340,   415,  -340,   684,   374,
    -340,   374,   -67,   -34,   370,   420,   585,  -340,   586,   587,
       8,     3,   542,  -340,  -340,  -340,  -340,  -340,  -340,   544,
    -340,   603,  -340,  -340,  -340,  -340,  -340,   435,   563,   276,
     342,   440,   210,  -340,   370,  -340,   606,   610,   166,   211,
     501,   504,  -340,  -340,    23,  -340,   455,   212,  -340,   477,
     214,  -340,   374,  -340,  -340,  -340,   361,  -340,  -340,  -340,
     453,   122,   -55,  -340,   374,   486,   260,   451,  -340,  -125,
    -340,   -48,   218,   470,   471,    19,   312,     3,     3,   456,
     111,   595,   596,   485,   219,  -340,  -340,   599,   487,   230,
     481,   488,   489,   491,   494,   495,   502,   503,   505,   510,
     512,   513,   515,   519,   520,   521,  -340,  -340,  -340,   244,
    -340,   641,   529,   246,  -340,  -340,  -340,   122,  -340,  -340,
    -340,   655,  -340,   654,  -340,   661,  -340,   686,  -340,  -340,
    -340,  -340,   634,   455,  -340,  -340,  -340,  -340,   374,   374,
    -340,  -340,  -340,  -340,  -340,   690,   699,   700,   709,   712,
     713,   714,   715,   716,   717,   718,   719,   720,   721,   722,
     723,   724,  -340,   619,   728,  -340,   562,  -340,  -340,   568,
     374,   257,   569,   122,   570,   571,   573,   574,   575,   576,
     577,   578,   579,   580,   581,   583,   584,   618,   621,   622,
     625,   620,  -340,   619,   749,  -340,   122,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,  -340,  -340,  -340,  -340,   753,  -340,   626,   597,   281,
    -340,  -340,   331,  -340,   753,  -340,  -340,  -340,  -340
=======
      43,   251,    40,   304,    44,    12,    44,    89,   684,    47,
      42,   139,    78,    82,   -42,    -9,   115,   -26,  -350,  -350,
    -350,  -350,  -350,  -350,  -350,  -350,   204,  -350,  -350,   150,
    -350,  -350,  -350,    95,    95,    95,    95,     8,    44,    99,
      99,    99,    99,    99,    33,   227,    44,   205,   184,   215,
    -350,  -350,  -350,  -350,  -350,  -350,  -350,   172,  -350,  -350,
    -350,  -350,  -350,    44,   295,  -350,  -350,  -350,  -350,   196,
      96,  -350,   288,   143,   160,  -350,     7,  -350,   283,  -350,
    -350,     0,   302,  -350,   324,   314,   395,    44,    44,    44,
     397,   345,   252,   366,   436,    44,    44,    44,   440,   442,
     451,   404,   469,   469,    68,    90,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,   204,  -350,  -350,  -350,  -350,   475,  -350,
     306,    82,   469,  -350,  -350,  -350,  -350,     0,  -350,  -350,
    -350,   322,   434,   420,   419,  -350,   -39,  -350,   252,  -350,
      44,   491,    86,  -350,  -350,  -350,  -350,  -350,   438,  -350,
     341,   -51,  -350,   322,  -350,  -350,   433,   439,  -350,  -350,
    -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,   461,   150,
    -350,  -350,   338,   343,   346,  -350,  -350,   584,   391,   347,
     349,   216,   510,  -350,  -350,   513,   361,   365,   368,   370,
     371,   424,   424,  -350,   369,   269,   -56,  -350,    10,   489,
    -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,   350,  -350,  -350,    28,  -350,    76,  -350,   322,   322,
     468,  -350,    -9,    48,   483,   372,  -350,    38,   376,  -350,
      44,   322,   451,  -350,   238,   380,   381,   529,   386,  -350,
    -350,   147,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,  -350,   424,   382,   499,   465,   322,   322,
     -47,   173,  -350,   584,  -350,   557,   322,   558,   559,   561,
     135,   135,  -350,  -350,   398,    39,     3,   322,   418,   570,
     322,   322,   -31,   405,   -28,   424,   424,   424,   424,   424,
     424,   424,   424,   424,   424,   424,   424,   424,   424,     9,
    -350,   569,  -350,   571,   416,  -350,   -40,   238,   322,  -350,
     204,   583,   463,   409,    77,  -350,  -350,  -350,    -9,   491,
     423,  -350,   591,   322,   429,  -350,   238,  -350,   294,   294,
    -350,  -350,   322,  -350,   145,   465,   462,   437,    36,    16,
     267,  -350,   322,   322,   533,    67,   432,   193,   444,  -350,
    -350,    -9,   445,   352,  -350,    73,  -350,  -350,   151,   404,
    -350,  -350,   482,   452,   424,   269,   511,  -350,   363,   363,
     435,   435,    97,   363,   363,   435,   435,   135,   135,  -350,
    -350,  -350,  -350,  -350,  -350,  -350,   322,  -350,  -350,  -350,
     238,  -350,  -350,  -350,  -350,  -350,  -350,  -350,   459,  -350,
    -350,  -350,  -350,   460,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,  -350,   467,   470,   155,   472,   491,  -350,
      48,   204,   194,   491,  -350,   211,   473,   627,   634,  -350,
     242,  -350,   243,   249,  -350,   474,  -350,   583,   322,  -350,
     322,   -37,    64,   424,   471,   639,  -350,   641,   642,    25,
       3,   595,  -350,  -350,  -350,  -350,  -350,  -350,   604,  -350,
     654,  -350,  -350,  -350,  -350,  -350,   515,   644,   269,   363,
     519,   250,  -350,   424,  -350,   686,   694,   364,   611,   585,
     588,  -350,  -350,   155,  -350,   491,   264,  -350,   578,   274,
    -350,   322,  -350,  -350,  -350,   294,  -350,  -350,  -350,   556,
     238,   -34,  -350,   322,   449,   554,  -350,  -350,   285,   560,
     562,    73,   352,     3,     3,   564,   151,   680,   682,   565,
     291,  -350,  -350,   499,   566,   293,   567,   568,   572,   573,
     574,   575,   576,   577,   579,   580,   581,   582,   586,   587,
     589,   590,  -350,  -350,  -350,   299,  -350,   726,   592,   301,
    -350,  -350,  -350,   238,  -350,   730,  -350,   733,  -350,  -350,
    -350,  -350,   685,   491,  -350,  -350,  -350,  -350,   322,   322,
    -350,  -350,  -350,  -350,  -350,   737,   738,   745,   750,   751,
     752,   755,   758,   759,   760,   761,   762,   763,   764,   765,
     766,   767,  -350,   666,   771,  -350,   602,   606,   322,   320,
     605,   238,   607,   609,   610,   612,   613,   614,   615,   616,
     617,   618,   619,   620,   621,   622,   623,   624,   625,   628,
    -350,   666,   777,  -350,   238,  -350,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,  -350,
    -350,  -350,  -350,   793,  -350,   629,   643,   332,  -350,  -350,
     318,  -350,   793,  -350,  -350,  -350,  -350
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,     0,     0,     0,     0,   167,     0,   325,     3,     5,
      10,    12,    13,    11,     6,     7,     9,   116,   115,     0,
       8,    14,    15,   323,   323,   323,   323,   323,     0,   321,
     321,   321,   321,   321,   160,     0,     0,     0,     0,     0,
      97,   101,    98,    99,   100,   102,    96,   167,   181,   182,
<<<<<<< HEAD
     180,     0,     0,   183,   184,     0,   187,   192,   193,   194,
     195,     0,   166,   168,     0,     0,     0,     1,   167,     2,
     150,   152,   153,     0,   139,   121,   127,     0,     0,     0,
       0,     0,     0,     0,    94,     0,     0,     0,     0,     0,
       0,     0,     0,   145,     0,     0,     0,     0,    95,    16,
      21,    23,    22,    17,    18,    20,    19,    24,    25,   185,
     186,   191,     0,   188,     0,     0,     0,   120,   119,     4,
     151,     0,   117,   118,   138,     0,     0,   135,     0,    26,
       0,    27,    94,   326,     0,     0,   167,   320,   108,   110,
     109,   111,     0,   161,     0,   145,   105,     0,    90,   319,
       0,     0,   199,   201,   200,   197,   198,   204,   206,   205,
     202,   203,   189,     0,   169,   196,     0,     0,   282,   286,
     289,   290,     0,     0,     0,     0,     0,   287,   288,     0,
       0,     0,     0,     0,     0,     0,     0,   284,   167,   141,
     207,   212,   213,   225,   226,   227,   228,   222,   217,   216,
     215,   223,   224,   214,   221,   220,   293,     0,     0,   137,
     322,   167,     0,     0,     0,    88,     0,     0,    92,     0,
       0,     0,   104,   144,     0,     0,     0,     0,   124,   123,
       0,   303,   302,   305,   304,   307,   306,   309,   308,   311,
     310,   313,   312,     0,     0,   248,   167,     0,     0,     0,
       0,   291,     0,   292,     0,     0,     0,     0,     0,   250,
     249,     0,     0,     0,     0,   143,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   126,   128,   133,
     134,     0,   122,    29,     0,     0,     0,     0,    32,    34,
      35,   167,     0,    31,    93,     0,     0,    91,   112,   107,
     106,     0,     0,   190,   170,     0,   243,     0,   167,     0,
       0,     0,     0,     0,   273,     0,     0,     0,     0,     0,
       0,     0,   219,   218,   167,   140,   154,   156,   165,   157,
     208,     0,   145,   211,   266,   267,     0,     0,   167,     0,
     247,   257,   258,   261,   262,     0,   264,   256,   259,   260,
     252,   251,   253,   254,   255,   283,   285,     0,   131,   132,
     130,   136,    38,    41,    42,    39,    40,    43,    44,    58,
      60,    45,    47,    46,    63,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,     0,    36,     0,     0,
      28,     0,    30,     0,     0,    89,     0,     0,     0,     0,
     318,     0,   314,     0,     0,   244,     0,   278,     0,     0,
     271,     0,     0,     0,     0,     0,     0,   232,     0,     0,
       0,     0,     0,   174,   175,   176,   177,   173,   178,     0,
     163,     0,   158,   235,   236,   237,   238,   142,   149,   167,
     265,     0,     0,   246,     0,   129,     0,     0,     0,     0,
       0,     0,    83,    84,    37,    80,     0,     0,    33,     0,
       0,   209,     0,   317,   316,   114,     0,   113,   245,   279,
       0,   275,     0,   274,     0,     0,     0,     0,   294,     0,
     295,     0,     0,     0,     0,   165,   155,     0,     0,   162,
       0,     0,   147,     0,     0,   280,   269,   268,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    85,    82,    81,     0,
      87,     0,     0,     0,   315,   277,   272,   276,   263,   300,
     297,     0,   296,     0,   299,     0,   230,     0,   233,   234,
     159,   171,     0,     0,   239,   240,   241,   242,     0,     0,
     125,   281,   270,    59,    62,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    86,   329,     0,   210,     0,   298,   301,     0,
       0,     0,   148,   146,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   336,   329,     0,   231,   172,   164,    61,    67,
      68,    65,    66,    69,    70,    71,    64,    75,    76,    73,
      74,    77,    78,    79,    72,     0,   337,     0,   332,     0,
     330,   229,     0,   328,     0,   333,   335,   334,   331
=======
     180,   183,   184,     0,   185,   190,   191,   192,   193,     0,
     166,   168,     0,     0,     0,     1,   167,     2,   150,   152,
     153,     0,   139,   121,   127,     0,     0,     0,     0,     0,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,   145,     0,     0,     0,     0,    95,    16,    21,    23,
      22,    17,    18,    20,    19,    24,    25,   189,     0,   186,
       0,     0,     0,   120,   119,     4,   151,     0,   117,   118,
     138,     0,     0,   135,     0,    26,     0,    27,    94,   326,
       0,     0,   167,   320,   108,   110,   109,   111,     0,   161,
       0,   145,   105,     0,    90,   319,     0,     0,   197,   199,
     198,   195,   196,   202,   204,   203,   200,   201,   187,     0,
     169,   194,     0,     0,   280,   284,   287,   288,     0,     0,
       0,     0,     0,   285,   286,     0,     0,     0,     0,     0,
       0,     0,     0,   282,     0,   167,   141,   205,   210,   211,
     223,   224,   225,   226,   220,   215,   214,   213,   221,   222,
     212,   219,   218,   292,     0,   293,     0,   291,     0,     0,
     137,   322,   167,     0,     0,     0,    88,     0,     0,    92,
       0,     0,     0,   104,   144,     0,     0,     0,     0,   124,
     123,     0,   303,   302,   305,   304,   307,   306,   309,   308,
     311,   310,   313,   312,     0,     0,   246,   167,     0,     0,
       0,     0,   289,     0,   290,     0,     0,     0,     0,     0,
     248,   247,   300,   297,     0,     0,     0,     0,   143,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     296,     0,   299,     0,   126,   128,   133,   134,     0,   122,
      29,     0,     0,     0,     0,    32,    34,    35,   167,     0,
      31,    93,     0,     0,    91,   112,   107,   106,     0,     0,
     188,   170,     0,   241,     0,   167,     0,     0,     0,     0,
       0,   271,     0,     0,     0,     0,     0,     0,     0,   217,
     216,   167,   140,   154,   156,   165,   157,   206,     0,   145,
     209,   264,   265,     0,     0,   167,     0,   245,   255,   256,
     259,   260,     0,   262,   254,   257,   258,   250,   249,   251,
     252,   253,   281,   283,   298,   301,     0,   131,   132,   130,
     136,    38,    41,    42,    39,    40,    43,    44,    58,    60,
      45,    47,    46,    63,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,     0,     0,    36,     0,     0,    28,
       0,    30,     0,     0,    89,     0,     0,     0,     0,   318,
       0,   314,     0,     0,   242,     0,   276,     0,     0,   269,
       0,     0,     0,     0,     0,     0,   230,     0,     0,     0,
       0,     0,   174,   175,   176,   177,   173,   178,     0,   163,
       0,   158,   233,   234,   235,   236,   142,   149,   167,   263,
       0,     0,   244,     0,   129,     0,     0,     0,     0,     0,
       0,    83,    84,    37,    80,     0,     0,    33,     0,     0,
     207,     0,   317,   316,   114,     0,   113,   243,   277,     0,
     273,     0,   272,     0,     0,     0,   294,   295,     0,     0,
       0,   165,   155,     0,     0,   162,     0,     0,   147,     0,
       0,   278,   267,   266,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    85,    82,    81,     0,    87,     0,     0,     0,
     315,   275,   270,   274,   261,     0,   228,     0,   231,   232,
     159,   171,     0,     0,   237,   238,   239,   240,     0,     0,
     125,   279,   268,    59,    62,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    86,   329,     0,   208,     0,     0,     0,     0,
     148,   146,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     336,   329,     0,   229,   172,   164,    61,    67,    68,    65,
      66,    69,    70,    71,    64,    75,    76,    73,    74,    77,
      78,    79,    72,     0,   337,     0,   332,     0,   330,   227,
       0,   328,     0,   333,   335,   334,   331
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
<<<<<<< HEAD
    -340,  -340,  -340,   725,  -340,   741,  -340,   388,  -340,   373,
    -340,   326,  -340,  -308,   745,   747,   663,  -340,  -340,   750,
    -340,   589,   751,   752,   -54,   791,   -15,   637,   680,   -19,
    -340,  -340,   436,  -340,  -340,  -340,  -340,  -340,  -340,  -150,
    -340,  -340,  -340,  -340,   371,  -237,    44,   313,  -340,  -340,
     689,  -340,  -340,   758,   759,   760,  -239,  -340,   545,  -155,
    -129,  -339,  -338,  -336,  -335,  -340,  -340,  -340,  -340,  -340,
    -340,   564,  -340,  -340,  -340,  -340,  -340,  -340,  -340,  -340,
    -340,   632,   500,   338,   -32,   251,   291,  -340,  -340,   202,
    -340,   172,  -340
=======
    -350,  -350,  -350,   722,  -350,   743,  -350,   383,  -350,   367,
    -350,   325,  -350,  -315,   744,   748,   668,  -350,  -350,   753,
    -350,   593,   754,   756,   -54,   792,   -15,   640,   687,    -3,
    -350,  -350,   426,  -350,  -350,  -350,  -350,  -350,  -350,  -146,
    -350,  -350,  -350,  -350,   373,  -121,    24,   305,  -350,  -350,
     696,  -350,  -350,   769,   770,   772,  -239,  -350,   538,  -152,
    -154,  -349,  -345,  -327,  -322,  -350,  -350,  -350,  -350,  -350,
    -350,   594,  -350,  -350,  -350,  -350,  -350,   374,  -350,   375,
    -350,   635,   492,   327,   -61,   312,   323,  -350,  -350,   203,
    -350,   176,  -350
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
<<<<<<< HEAD
       0,    16,    17,    18,   108,    19,   307,   308,   309,   407,
     474,   475,   310,   226,    20,    21,   146,    22,    57,    23,
     155,   156,    24,    25,    26,    27,    28,    85,   132,    86,
     137,   297,   298,   380,   219,   302,   135,   275,   352,   158,
     570,   512,    83,   345,   346,   347,   348,   452,    29,    72,
      73,   349,   449,    30,    31,    32,   199,   317,   200,   201,
     202,   203,   204,   205,   206,   457,   207,   208,   209,   210,
     211,   260,   212,   213,   214,   215,   497,   498,   499,   500,
     501,   216,   421,   422,   160,    96,    88,    79,    93,   622,
     649,   650,   313
=======
       0,    16,    17,    18,   106,    19,   314,   315,   316,   416,
     483,   484,   317,   227,    20,    21,   142,    22,    57,    23,
     151,   152,    24,    25,    26,    27,    28,    83,   128,    84,
     133,   304,   305,   389,   220,   309,   131,   278,   359,   154,
     570,   518,    81,   352,   353,   354,   355,   461,    29,    70,
      71,   356,   458,    30,    31,    32,   196,   324,   197,   198,
     199,   200,   201,   202,   203,   466,   204,   205,   206,   207,
     208,   261,   209,   210,   211,   212,   505,   213,   214,   215,
     216,   217,   430,   431,   156,    94,    86,    77,    91,   620,
     647,   648,   320
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
<<<<<<< HEAD
      76,   327,   233,   115,   413,   232,    44,    84,   375,   221,
     273,   493,   453,   454,   157,   455,   456,    13,   304,   378,
     379,   429,   450,   546,   227,   356,  -324,   276,   359,    80,
     259,    81,    82,     1,    44,     2,     3,     4,     5,     6,
       7,     8,     9,   272,   552,    38,    10,   494,    45,   553,
      47,    11,    12,   255,    66,   162,   163,   164,   167,   168,
     169,   258,   299,   300,   133,    13,   269,   270,    58,   451,
      44,   277,   278,   161,    70,   319,   360,   416,   277,   278,
      59,    60,    94,   277,   278,  -327,   424,   277,   278,   428,
     103,   470,   277,   278,   175,    87,   357,   314,    46,    13,
     315,   477,   331,   332,   277,   278,   480,    65,   165,   121,
     338,   170,   177,   274,    15,   277,   278,   277,   278,   462,
     343,   554,   354,   355,   255,    74,   555,   231,   305,   222,
     306,   228,    71,   140,   141,   142,   471,    77,   472,   473,
      78,   149,   150,   151,    48,    49,   381,   178,   179,   180,
     181,   104,   105,   361,   362,   363,   364,   365,   366,   367,
     368,   369,   370,   371,   372,   373,   374,   303,   539,   131,
     376,   564,   565,   344,   566,   567,    14,   277,   278,   505,
     432,   433,    84,   271,    87,    61,    62,   281,   224,   410,
      63,    64,   411,   277,   278,    95,    15,    33,    34,    35,
     166,   101,   458,   171,   325,  -328,  -328,   102,   425,    36,
      37,   274,   182,   183,    39,    40,    41,    67,    68,    69,
     514,   184,   299,   185,   122,   123,    42,    43,   460,   435,
     106,  -328,  -328,   291,   292,   293,   294,   295,   107,   437,
     186,   330,   438,   543,   119,   178,   179,   180,   181,   120,
     520,   521,   522,   523,   524,   601,   191,   412,   525,   526,
     277,   278,   187,   188,   189,   549,   550,   192,   193,   194,
     561,   562,   479,   318,   491,   315,   492,   125,   527,   178,
     179,   180,   181,   334,   190,   335,   124,   336,   126,   191,
     440,    97,    98,    99,   100,   528,   529,   530,   531,   532,
     192,   193,   194,   533,   534,   495,   127,   195,   196,   197,
     182,   183,   128,   426,   198,   326,   430,   130,   431,   184,
     336,   185,   134,   535,   258,    89,    90,    91,    92,   602,
      80,   136,    81,    82,   655,   517,   656,   657,   186,   547,
     293,   294,   295,   461,   182,   183,   138,   481,   485,    13,
     274,   486,   139,   184,   487,   185,   488,   486,   143,   274,
     187,   188,   189,   442,  -179,   443,   444,   445,   446,   145,
     447,   448,   186,   178,   179,   180,   181,   178,   179,   180,
     181,   516,   190,   540,   274,   542,   315,   191,   315,   556,
     572,   144,   557,   274,   187,   188,   189,   147,   192,   193,
     194,   574,   148,   152,   575,   195,   196,   197,   178,   179,
     180,   181,   198,   153,   603,   592,   190,   595,   315,   154,
     274,   191,    50,    51,    52,    53,    54,    55,   627,   157,
      56,   315,   192,   193,   194,   418,   419,   420,   253,   195,
     196,   197,   182,   183,   513,   626,   198,   184,   159,   185,
     173,   184,   653,   185,   172,   654,   217,   218,   225,   281,
     220,   229,   230,   234,   236,   235,   186,   238,   239,   240,
     186,   256,   261,   253,   254,  -328,  -328,   284,   285,   262,
     257,   264,   184,  -328,   185,   265,   266,   267,   187,   188,
     189,   268,   187,   188,   189,   296,   301,   311,   323,   312,
     316,   186,  -328,   289,   290,   291,   292,   293,   294,   295,
     190,   321,    13,   322,   190,   191,   324,   328,   337,   191,
     339,   340,   341,   187,   188,   189,   192,   193,   194,   342,
     192,   193,   194,   195,   196,   197,   351,   195,   196,   197,
     198,   353,   358,   377,   198,   190,   409,   408,   414,   415,
     191,   417,   357,   427,   329,   434,   459,   436,   439,   441,
     277,   192,   193,   194,   483,   463,   466,   467,   195,   196,
     197,   468,   469,   476,     1,   198,     2,     3,     4,     5,
       6,     7,     8,     9,   482,   484,   489,    10,   496,   502,
     503,   504,    11,    12,   507,     1,   508,     2,     3,     4,
       5,     6,     7,   281,     9,   279,   509,   280,    10,   510,
     511,   515,   518,    11,    12,   329,   519,   536,   537,   282,
     283,   284,   285,   541,   545,   551,   563,   287,   241,   242,
     243,   244,   245,   246,   247,   248,   249,   250,   251,   252,
      13,   558,   559,   568,   593,   569,   288,   289,   290,   291,
     292,   293,   294,   295,   281,   576,   571,   548,   573,   596,
     597,    13,   577,   578,   281,   579,   598,   329,   580,   581,
     282,   283,   284,   285,   286,   594,   582,   583,   287,   584,
     282,   283,   284,   285,   585,   464,   586,   587,   287,   588,
     599,   600,   621,   589,   590,   591,   604,   288,   289,   290,
     291,   292,   293,   294,   295,   605,   606,   288,   289,   290,
     291,   292,   293,   294,   295,   607,   281,    14,   608,   609,
     610,   611,   612,   613,   614,   615,   616,   617,   618,   619,
     620,   623,   282,   283,   284,   285,   624,    15,    14,   625,
     287,   628,   629,   274,   630,   631,   632,   633,   634,   635,
     636,   637,   638,   647,   639,   640,   648,   652,    15,   288,
     289,   290,   291,   292,   293,   294,   295,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   641,
     645,   404,   642,   643,   405,   406,   644,   651,   109,   478,
     538,   490,   110,   129,   111,   223,    75,   112,   113,   114,
     237,   176,   506,   465,   174,   116,   117,   118,   560,   350,
     320,   263,   423,   333,   544,   646,   658
=======
      74,   234,   334,   113,   422,   233,    44,  -324,    82,   462,
     153,   222,   382,   463,     1,   276,     2,     3,     4,     5,
       6,     7,     8,     9,   256,   387,   388,    10,    45,   260,
      47,   464,    11,    12,    64,   259,   465,   270,   271,   363,
     366,   502,   157,   275,   552,    44,    78,    44,    79,    80,
       1,   311,     2,     3,     4,     5,     6,     7,     8,     9,
     279,   171,    92,    10,    13,  -327,   306,   307,    11,    12,
     101,    38,   158,   159,   160,    85,   459,    58,   129,   326,
      13,    68,    63,    46,   425,    69,   437,   117,   367,    59,
      60,   280,   281,   433,   163,   164,   165,   438,   280,   281,
     256,   280,   281,   486,   280,   281,   338,   339,   489,    72,
     364,   136,   137,   138,   345,    75,    13,   277,   228,   145,
     146,   147,   232,   460,   173,   161,   471,   229,   361,   362,
     223,   368,   369,   370,   371,   372,   373,   374,   375,   376,
     377,   378,   379,   380,   381,   503,    76,   166,   280,   281,
     174,   175,   176,   177,   280,   281,   390,    14,    82,    13,
      15,   312,    85,   313,   225,   336,    93,   564,   310,   127,
     545,   565,   351,   383,   280,   281,    15,   280,   281,     1,
     274,     2,     3,     4,     5,     6,     7,   104,     9,   566,
     441,   442,    10,    14,   567,   511,   300,    11,    12,    61,
      62,   301,   280,   281,    99,   280,   281,   332,   321,   350,
     469,   322,    15,   467,   284,   178,   179,   162,   105,   174,
     175,   176,   177,   479,   180,    78,   181,    79,    80,   520,
     285,   286,   287,   288,   306,   473,    48,    49,   290,   167,
     444,   100,   337,   182,   302,    13,   120,   419,   599,   303,
     420,   341,   549,   342,   325,   343,   291,   292,   293,   294,
     295,   296,   297,   298,   421,   183,   184,   185,   480,   121,
     481,   482,   174,   175,   176,   177,   102,   103,    33,    34,
      35,    65,    66,    67,   178,   179,   500,   186,   501,   504,
      36,    37,   187,   180,   122,   181,   187,   449,   259,   296,
     297,   298,   188,   189,   190,   126,   188,   189,   190,   191,
     192,   193,   182,   123,   194,   434,   195,   333,   277,   523,
     435,   653,    14,   654,   655,   174,   175,   176,   177,   600,
     124,    39,    40,    41,   183,   184,   185,   178,   179,   118,
     119,    15,    13,    42,    43,   439,   180,   440,   181,   343,
     470,   553,    95,    96,    97,    98,   186,    87,    88,    89,
      90,   187,   130,   446,   488,   182,   447,   322,   427,   428,
     429,   188,   189,   190,   272,   273,   280,   281,   191,   192,
     193,   490,   134,   194,   277,   195,   132,   183,   184,   185,
     178,   179,   561,   562,   174,   175,   176,   177,   135,   180,
     139,   181,   140,   451,  -179,   452,   453,   454,   455,   186,
     456,   457,   494,   496,   187,   495,   495,   601,   182,   497,
     522,   141,   277,   277,   188,   189,   190,   174,   175,   176,
     177,   191,   192,   193,   546,   143,   194,   322,   195,   144,
     183,   184,   185,   148,   548,   149,   624,   322,   526,   527,
     528,   529,   530,   519,   150,   556,   531,   532,   557,   254,
     255,   572,   186,   574,   277,   153,   575,   187,   180,   592,
     181,   595,   322,   155,   277,   169,   533,   188,   189,   190,
     284,   168,   218,   219,   191,   192,   193,   182,   221,   194,
     625,   195,   254,   322,   226,   230,  -328,  -328,   287,   288,
     231,   180,   651,   181,  -328,   652,   235,   237,   239,   183,
     184,   185,   236,   240,   262,   241,   257,   336,   258,   263,
     182,   299,  -328,   292,   293,   294,   295,   296,   297,   298,
     265,   186,   308,   318,   266,   330,   187,   267,    13,   268,
     269,   319,   183,   184,   185,   323,   188,   189,   190,   328,
     329,   335,   284,   191,   192,   193,   331,   282,   194,   283,
     195,   344,   346,   347,   186,   348,   284,   336,   349,   187,
    -328,  -328,   358,   360,   365,   384,   385,   417,   418,   188,
     189,   190,   285,   286,   287,   288,   191,   192,   193,   386,
     290,   194,   423,   195,   424,  -328,  -328,   294,   295,   296,
     297,   298,   426,   364,   443,   445,   284,   436,   291,   292,
     293,   294,   295,   296,   297,   298,   284,   448,   450,   554,
     280,   468,   285,   286,   287,   288,   289,   472,   475,   476,
     290,   492,   285,   286,   287,   288,   477,   493,   194,   478,
     290,   485,   491,   508,   498,   509,   510,   513,   291,   292,
     293,   294,   295,   296,   297,   298,   514,   515,   291,   292,
     293,   294,   295,   296,   297,   298,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   516,   521,
     413,   517,   524,   414,   415,   534,   535,   536,   537,   538,
     525,   542,   543,   539,   540,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,    50,    51,    52,
      53,    54,    55,   541,   547,    56,   551,   555,   568,   593,
     558,   569,   559,   563,   596,   571,   573,   597,   594,   619,
     576,   577,   598,   602,   603,   578,   579,   580,   581,   582,
     583,   604,   584,   585,   586,   587,   605,   606,   607,   588,
     589,   608,   590,   591,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   618,   621,   622,   623,   626,   277,   627,
     628,   645,   629,   630,   631,   632,   633,   634,   635,   636,
     637,   638,   639,   640,   641,   642,   646,   643,   125,   649,
     107,   108,   650,   487,   499,   109,   224,    73,   544,   238,
     110,   111,   474,   112,   172,   357,   560,   170,   506,   507,
     264,   432,   550,   512,   644,   327,   114,   115,   656,   116,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   340
>>>>>>> 13445115 (Add insert embedding syntax.)
};

static const yytype_int16 yycheck[] =
{
<<<<<<< HEAD
      15,   240,   157,    57,   312,   155,     3,     8,     3,    50,
      71,    78,   351,   351,    61,   351,   351,    73,     3,    65,
      66,    81,     3,    78,    32,    70,     0,    50,    68,    21,
     185,    23,    24,     7,     3,     9,    10,    11,    12,    13,
      14,    15,    16,   198,   169,    31,    20,    81,     4,   174,
       6,    25,    26,   182,    10,     4,     5,     6,     4,     5,
       6,    82,   217,   218,    83,    73,   195,   196,    30,    50,
       3,   138,   139,   105,     3,   230,   116,   316,   138,   139,
      42,    43,    38,   138,   139,    57,   325,   138,   139,    50,
      46,    68,   138,   139,   126,    67,   141,   171,    71,    73,
     174,   409,   257,   258,   138,   139,   414,    40,    57,    65,
     265,    57,   131,   174,   170,   138,   139,   138,   139,   358,
     171,   169,   277,   278,   253,   151,   174,   174,   113,   170,
     115,   146,     3,    89,    90,    91,   113,     0,   115,   116,
     173,    97,    98,    99,   147,   148,   301,     3,     4,     5,
       6,    71,    72,   282,   283,   284,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   295,   221,   476,   170,
     165,   510,   510,   170,   510,   510,   150,   138,   139,   171,
     335,   336,     8,   198,    67,   147,   148,   117,   144,   171,
     152,   153,   174,   138,   139,    67,   170,    27,    28,    29,
     149,   172,   352,   149,    60,   135,   136,    14,   171,    39,
      40,   174,    68,    69,    27,    28,    29,   142,   143,   144,
     459,    77,   377,    79,    44,    45,    39,    40,   357,   174,
       3,   161,   162,   163,   164,   165,   166,   167,     3,   171,
      96,   256,   174,   482,   154,     3,     4,     5,     6,   154,
      84,    85,    86,    87,    88,   563,   145,   311,    92,    93,
     138,   139,   118,   119,   120,     5,     6,   156,   157,   158,
     507,   508,   171,   229,   429,   174,   431,   174,   112,     3,
       4,     5,     6,    78,   140,    80,    50,    82,     6,   145,
     344,    40,    41,    42,    43,    84,    85,    86,    87,    88,
     156,   157,   158,    92,    93,   434,   171,   163,   164,   165,
      68,    69,   171,   328,   170,   171,    78,    22,    80,    77,
      82,    79,    60,   112,    82,    34,    35,    36,    37,   568,
      21,    62,    23,    24,     3,   464,     5,     6,    96,   494,
     165,   166,   167,   358,    68,    69,    68,   171,   171,    73,
     174,   174,     3,    77,   171,    79,   171,   174,     3,   174,
     118,   119,   120,    51,    52,    53,    54,    55,    56,   170,
      58,    59,    96,     3,     4,     5,     6,     3,     4,     5,
       6,   171,   140,   171,   174,   171,   174,   145,   174,   171,
     171,    57,   174,   174,   118,   119,   120,    69,   156,   157,
     158,   171,     3,     3,   174,   163,   164,   165,     3,     4,
       5,     6,   170,     3,   569,   171,   140,   171,   174,     3,
     174,   145,    33,    34,    35,    36,    37,    38,   171,    61,
      41,   174,   156,   157,   158,    74,    75,    76,    68,   163,
     164,   165,    68,    69,   459,   600,   170,    77,     4,    79,
     170,    77,   171,    79,     6,   174,    48,    63,     3,   117,
      69,    57,   160,    73,    46,    73,    96,   171,   171,   170,
      96,   170,     4,    68,    69,   133,   134,   135,   136,     6,
     170,   170,    77,   141,    79,   170,   170,   170,   118,   119,
     120,   170,   118,   119,   120,   172,    64,    50,     6,   170,
     170,    96,   160,   161,   162,   163,   164,   165,   166,   167,
     140,   170,    73,   170,   140,   145,   171,   170,     4,   145,
       4,     4,     4,   118,   119,   120,   156,   157,   158,   171,
     156,   157,   158,   163,   164,   165,   155,   163,   164,   165,
     170,     3,   170,   174,   170,   140,   170,   114,   170,     3,
     145,   174,   141,   171,    68,    71,   170,   174,   174,   174,
     138,   156,   157,   158,     4,   116,   170,   170,   163,   164,
     165,   170,   170,   170,     7,   170,     9,    10,    11,    12,
      13,    14,    15,    16,   170,     3,   171,    20,   168,     4,
       4,     4,    25,    26,    52,     7,    52,     9,    10,    11,
      12,    13,    14,   117,    16,    68,     3,    70,    20,   174,
      47,   171,     6,    25,    26,    68,     6,   116,   114,   133,
     134,   135,   136,   146,   171,   174,   170,   141,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
      73,   171,   171,    48,     3,    49,   160,   161,   162,   163,
     164,   165,   166,   167,   117,   174,   171,   171,   171,     4,
       6,    73,   174,   174,   117,   174,     5,    68,   174,   174,
     133,   134,   135,   136,   137,   146,   174,   174,   141,   174,
     133,   134,   135,   136,   174,   138,   174,   174,   141,   174,
       4,    57,    73,   174,   174,   174,     6,   160,   161,   162,
     163,   164,   165,   166,   167,     6,     6,   160,   161,   162,
     163,   164,   165,   166,   167,     6,   117,   150,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     3,   133,   134,   135,   136,   174,   170,   150,   171,
     141,   171,   171,   174,   171,   171,   171,   171,   171,   171,
     171,   171,   171,     4,   171,   171,     3,   160,   170,   160,
     161,   162,   163,   164,   165,   166,   167,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   171,
     170,   107,   171,   171,   110,   111,   171,   171,    57,   411,
     474,   428,    57,    78,    57,   142,    15,    57,    57,    57,
     173,   131,   441,   377,   125,    57,    57,    57,   505,   274,
     231,   189,   322,   259,   486,   623,   654
=======
      15,   153,   241,    57,   319,   151,     3,     0,     8,   358,
      61,    50,     3,   358,     7,    71,     9,    10,    11,    12,
      13,    14,    15,    16,   178,    65,    66,    20,     4,   181,
       6,   358,    25,    26,    10,    82,   358,   191,   192,    70,
      68,    78,   103,   195,    78,     3,    21,     3,    23,    24,
       7,     3,     9,    10,    11,    12,    13,    14,    15,    16,
      50,   122,    38,    20,    73,    57,   218,   219,    25,    26,
      46,    31,     4,     5,     6,    67,     3,    30,    81,   231,
      73,     3,    40,    71,   323,     3,    50,    63,   116,    42,
      43,   138,   139,   332,     4,     5,     6,    81,   138,   139,
     254,   138,   139,   418,   138,   139,   258,   259,   423,   151,
     141,    87,    88,    89,   266,     0,    73,   173,    32,    95,
      96,    97,   173,    50,   127,    57,   365,   142,   280,   281,
     169,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   296,   297,   298,    81,   172,    57,   138,   139,
       3,     4,     5,     6,   138,   139,   308,   150,     8,    73,
     169,   113,    67,   115,   140,    68,    67,   516,   222,   169,
     485,   516,   169,   164,   138,   139,   169,   138,   139,     7,
     195,     9,    10,    11,    12,    13,    14,     3,    16,   516,
     342,   343,    20,   150,   516,   170,   168,    25,    26,   152,
     153,   173,   138,   139,   171,   138,   139,    60,   170,   170,
     364,   173,   169,   359,   117,    68,    69,   149,     3,     3,
       4,     5,     6,    68,    77,    21,    79,    23,    24,   468,
     133,   134,   135,   136,   386,   138,   147,   148,   141,   149,
     173,    14,   257,    96,   168,    73,    50,   170,   563,   173,
     173,    78,   491,    80,   230,    82,   159,   160,   161,   162,
     163,   164,   165,   166,   318,   118,   119,   120,   113,   173,
     115,   116,     3,     4,     5,     6,    71,    72,    27,    28,
      29,   142,   143,   144,    68,    69,   438,   140,   440,   443,
      39,    40,   145,    77,     6,    79,   145,   351,    82,   164,
     165,   166,   155,   156,   157,    22,   155,   156,   157,   162,
     163,   164,    96,   170,   167,   170,   169,   170,   173,   473,
     335,     3,   150,     5,     6,     3,     4,     5,     6,   568,
     170,    27,    28,    29,   118,   119,   120,    68,    69,    44,
      45,   169,    73,    39,    40,    78,    77,    80,    79,    82,
     365,   503,    40,    41,    42,    43,   140,    34,    35,    36,
      37,   145,    60,   170,   170,    96,   173,   173,    74,    75,
      76,   155,   156,   157,     5,     6,   138,   139,   162,   163,
     164,   170,    68,   167,   173,   169,    62,   118,   119,   120,
      68,    69,   513,   514,     3,     4,     5,     6,     3,    77,
       3,    79,    57,    51,    52,    53,    54,    55,    56,   140,
      58,    59,   170,   170,   145,   173,   173,   569,    96,   170,
     170,   169,   173,   173,   155,   156,   157,     3,     4,     5,
       6,   162,   163,   164,   170,    69,   167,   173,   169,     3,
     118,   119,   120,     3,   170,     3,   598,   173,    84,    85,
      86,    87,    88,   468,     3,   170,    92,    93,   173,    68,
      69,   170,   140,   170,   173,    61,   173,   145,    77,   170,
      79,   170,   173,     4,   173,   169,   112,   155,   156,   157,
     117,     6,    48,    63,   162,   163,   164,    96,    69,   167,
     170,   169,    68,   173,     3,    57,   133,   134,   135,   136,
     159,    77,   170,    79,   141,   173,    73,    46,   170,   118,
     119,   120,    73,   170,     4,   169,   169,    68,   169,     6,
      96,   171,   159,   160,   161,   162,   163,   164,   165,   166,
     169,   140,    64,    50,   169,     6,   145,   169,    73,   169,
     169,   169,   118,   119,   120,   169,   155,   156,   157,   169,
     169,   169,   117,   162,   163,   164,   170,    68,   167,    70,
     169,     4,     4,     4,   140,     4,   117,    68,   170,   145,
     135,   136,   154,     3,   169,     6,     5,   114,   169,   155,
     156,   157,   133,   134,   135,   136,   162,   163,   164,   173,
     141,   167,   169,   169,     3,   160,   161,   162,   163,   164,
     165,   166,   173,   141,    71,   173,   117,   170,   159,   160,
     161,   162,   163,   164,   165,   166,   117,   173,   173,   170,
     138,   169,   133,   134,   135,   136,   137,   116,   169,   169,
     141,     4,   133,   134,   135,   136,   169,     3,   167,   169,
     141,   169,   169,     4,   170,     4,     4,    52,   159,   160,
     161,   162,   163,   164,   165,   166,    52,     3,   159,   160,
     161,   162,   163,   164,   165,   166,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   173,   170,
     107,    47,     6,   110,   111,    84,    85,    86,    87,    88,
       6,   116,   114,    92,    93,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    33,    34,    35,
      36,    37,    38,   112,   146,    41,   170,   173,    48,     3,
     170,    49,   170,   169,     4,   170,   170,     4,   146,    73,
     173,   173,    57,     6,     6,   173,   173,   173,   173,   173,
     173,     6,   173,   173,   173,   173,     6,     6,     6,   173,
     173,     6,   173,   173,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     3,   173,   170,   170,   173,   170,
     170,     4,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,     3,   169,    76,   170,
      57,    57,   159,   420,   437,    57,   138,    15,   483,   169,
      57,    57,   386,    57,   127,   277,   511,   121,   444,   444,
     185,   329,   495,   450,   621,   232,    57,    57,   652,    57,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   260
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
<<<<<<< HEAD
      20,    25,    26,    73,   150,   170,   176,   177,   178,   180,
     189,   190,   192,   194,   197,   198,   199,   200,   201,   223,
     228,   229,   230,    27,    28,    29,    39,    40,    31,    27,
      28,    29,    39,    40,     3,   221,    71,   221,   147,   148,
      33,    34,    35,    36,    37,    38,    41,   193,    30,    42,
      43,   147,   148,   152,   153,    40,   221,   142,   143,   144,
       3,     3,   224,   225,   151,   200,   201,     0,   173,   262,
      21,    23,    24,   217,     8,   202,   204,    67,   261,   261,
     261,   261,   261,   263,   221,    67,   260,   260,   260,   260,
     260,   172,    14,   221,    71,    72,     3,     3,   179,   180,
     189,   190,   194,   197,   198,   199,   228,   229,   230,   154,
     154,   221,    44,    45,    50,   174,     6,   171,   171,   178,
      22,   170,   203,   204,    60,   211,    62,   205,    68,     3,
     221,   221,   221,     3,    57,   170,   191,    69,     3,   221,
     221,   221,     3,     3,     3,   195,   196,    61,   214,     4,
     259,   259,     4,     5,     6,    57,   149,     4,     5,     6,
      57,   149,     6,   170,   225,   259,   203,   204,     3,     4,
       5,     6,    68,    69,    77,    79,    96,   118,   119,   120,
     140,   145,   156,   157,   158,   163,   164,   165,   170,   231,
     233,   234,   235,   236,   237,   238,   239,   241,   242,   243,
     244,   245,   247,   248,   249,   250,   256,    48,    63,   209,
      69,    50,   170,   191,   221,     3,   188,    32,   201,    57,
     160,   174,   214,   234,    73,    73,    46,   202,   171,   171,
     170,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,    68,    69,   235,   170,   170,    82,   234,
     246,     4,     6,   256,   170,   170,   170,   170,   170,   235,
     235,   201,   234,    71,   174,   212,    50,   138,   139,    68,
      70,   117,   133,   134,   135,   136,   137,   141,   160,   161,
     162,   163,   164,   165,   166,   167,   172,   206,   207,   234,
     234,    64,   210,   199,     3,   113,   115,   181,   182,   183,
     187,    50,   170,   267,   171,   174,   170,   232,   221,   234,
     196,   170,   170,     6,   171,    60,   171,   231,   170,    68,
     201,   234,   234,   246,    78,    80,    82,     4,   234,     4,
       4,     4,   171,   171,   170,   218,   219,   220,   221,   226,
     233,   155,   213,     3,   234,   234,    70,   141,   170,    68,
     116,   235,   235,   235,   235,   235,   235,   235,   235,   235,
     235,   235,   235,   235,   235,     3,   165,   174,    65,    66,
     208,   234,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   107,   110,   111,   184,   114,   170,
     171,   174,   199,   188,   170,     3,   231,   174,    74,    75,
      76,   257,   258,   257,   231,   171,   201,   171,    50,    81,
      78,    80,   234,   234,    71,   174,   174,   171,   174,   174,
     199,   174,    51,    53,    54,    55,    56,    58,    59,   227,
       3,    50,   222,   236,   237,   238,   239,   240,   214,   170,
     235,   201,   231,   116,   138,   207,   170,   170,   170,   170,
      68,   113,   115,   116,   185,   186,   170,   188,   182,   171,
     188,   171,   170,     4,     3,   171,   174,   171,   171,   171,
     184,   234,   234,    78,    81,   235,   168,   251,   252,   253,
     254,   255,     4,     4,     4,   171,   219,    52,    52,     3,
     174,    47,   216,   201,   231,   171,   171,   235,     6,     6,
      84,    85,    86,    87,    88,    92,    93,   112,    84,    85,
      86,    87,    88,    92,    93,   112,   116,   114,   186,   188,
     171,   146,   171,   231,   258,   171,    78,   234,   171,     5,
       6,   174,   169,   174,   169,   174,   171,   174,   171,   171,
     222,   220,   220,   170,   236,   237,   238,   239,    48,    49,
     215,   171,   171,   171,   171,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   171,     3,   146,   171,     4,     6,     5,     4,
      57,   188,   231,   234,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,    73,   264,     3,   174,   171,   234,   171,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   171,   171,   170,   264,     4,     3,   265,
     266,   171,   160,   171,   174,     3,     5,     6,   266
=======
      20,    25,    26,    73,   150,   169,   175,   176,   177,   179,
     188,   189,   191,   193,   196,   197,   198,   199,   200,   222,
     227,   228,   229,    27,    28,    29,    39,    40,    31,    27,
      28,    29,    39,    40,     3,   220,    71,   220,   147,   148,
      33,    34,    35,    36,    37,    38,    41,   192,    30,    42,
      43,   152,   153,    40,   220,   142,   143,   144,     3,     3,
     223,   224,   151,   199,   200,     0,   172,   261,    21,    23,
      24,   216,     8,   201,   203,    67,   260,   260,   260,   260,
     260,   262,   220,    67,   259,   259,   259,   259,   259,   171,
      14,   220,    71,    72,     3,     3,   178,   179,   188,   189,
     193,   196,   197,   198,   227,   228,   229,   220,    44,    45,
      50,   173,     6,   170,   170,   177,    22,   169,   202,   203,
      60,   210,    62,   204,    68,     3,   220,   220,   220,     3,
      57,   169,   190,    69,     3,   220,   220,   220,     3,     3,
       3,   194,   195,    61,   213,     4,   258,   258,     4,     5,
       6,    57,   149,     4,     5,     6,    57,   149,     6,   169,
     224,   258,   202,   203,     3,     4,     5,     6,    68,    69,
      77,    79,    96,   118,   119,   120,   140,   145,   155,   156,
     157,   162,   163,   164,   167,   169,   230,   232,   233,   234,
     235,   236,   237,   238,   240,   241,   242,   243,   244,   246,
     247,   248,   249,   251,   252,   253,   254,   255,    48,    63,
     208,    69,    50,   169,   190,   220,     3,   187,    32,   200,
      57,   159,   173,   213,   233,    73,    73,    46,   201,   170,
     170,   169,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,    68,    69,   234,   169,   169,    82,
     233,   245,     4,     6,   255,   169,   169,   169,   169,   169,
     234,   234,     5,     6,   200,   233,    71,   173,   211,    50,
     138,   139,    68,    70,   117,   133,   134,   135,   136,   137,
     141,   159,   160,   161,   162,   163,   164,   165,   166,   171,
     168,   173,   168,   173,   205,   206,   233,   233,    64,   209,
     198,     3,   113,   115,   180,   181,   182,   186,    50,   169,
     266,   170,   173,   169,   231,   220,   233,   195,   169,   169,
       6,   170,    60,   170,   230,   169,    68,   200,   233,   233,
     245,    78,    80,    82,     4,   233,     4,     4,     4,   170,
     170,   169,   217,   218,   219,   220,   225,   232,   154,   212,
       3,   233,   233,    70,   141,   169,    68,   116,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,   234,     3,   164,     6,     5,   173,    65,    66,   207,
     233,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   107,   110,   111,   183,   114,   169,   170,
     173,   198,   187,   169,     3,   230,   173,    74,    75,    76,
     256,   257,   256,   230,   170,   200,   170,    50,    81,    78,
      80,   233,   233,    71,   173,   173,   170,   173,   173,   198,
     173,    51,    53,    54,    55,    56,    58,    59,   226,     3,
      50,   221,   235,   236,   237,   238,   239,   213,   169,   234,
     200,   230,   116,   138,   206,   169,   169,   169,   169,    68,
     113,   115,   116,   184,   185,   169,   187,   181,   170,   187,
     170,   169,     4,     3,   170,   173,   170,   170,   170,   183,
     233,   233,    78,    81,   234,   250,   251,   253,     4,     4,
       4,   170,   218,    52,    52,     3,   173,    47,   215,   200,
     230,   170,   170,   234,     6,     6,    84,    85,    86,    87,
      88,    92,    93,   112,    84,    85,    86,    87,    88,    92,
      93,   112,   116,   114,   185,   187,   170,   146,   170,   230,
     257,   170,    78,   233,   170,   173,   170,   173,   170,   170,
     221,   219,   219,   169,   235,   236,   237,   238,    48,    49,
     214,   170,   170,   170,   170,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   170,     3,   146,   170,     4,     4,    57,   187,
     230,   233,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,    73,
     263,     3,   173,   170,   233,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   169,   263,     4,     3,   264,   265,   170,
     159,   170,   173,     3,     5,     6,   265
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   175,   176,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   179,   179,   179,   179,
<<<<<<< HEAD
     179,   179,   179,   179,   179,   179,   180,   180,   180,   180,
     180,   180,   181,   181,   182,   182,   183,   183,   184,   184,
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
     228,   228,   229,   229,   229,   230,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   231,   231,   232,
     232,   233,   233,   234,   234,   234,   234,   234,   235,   235,
     235,   235,   235,   235,   235,   235,   235,   235,   235,   236,
     237,   237,   238,   238,   239,   240,   240,   240,   240,   240,
     240,   240,   240,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   242,   242,   243,   244,
     244,   245,   245,   245,   245,   246,   246,   247,   248,   248,
     248,   248,   249,   249,   249,   249,   250,   250,   250,   250,
     250,   250,   250,   250,   251,   251,   252,   253,   253,   254,
     255,   255,   256,   256,   256,   256,   256,   256,   256,   256,
     256,   256,   256,   256,   257,   257,   258,   258,   258,   259,
     260,   260,   261,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   266,   266,   266,   266,   267,   267
=======
     179,   179,   180,   180,   181,   181,   182,   182,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     184,   184,   185,   185,   185,   185,   186,   186,   187,   187,
     188,   189,   189,   190,   190,   191,   192,   192,   192,   192,
     192,   192,   192,   192,   193,   194,   194,   195,   196,   196,
     196,   196,   196,   197,   197,   198,   198,   198,   198,   199,
     199,   200,   201,   202,   202,   203,   204,   204,   205,   205,
     206,   207,   207,   207,   208,   208,   209,   209,   210,   210,
     211,   211,   212,   212,   213,   213,   214,   214,   215,   215,
     216,   216,   216,   216,   217,   217,   218,   218,   219,   219,
     220,   220,   221,   221,   221,   221,   222,   222,   223,   223,
     224,   225,   225,   226,   226,   226,   226,   226,   226,   226,
     227,   227,   227,   227,   227,   227,   227,   227,   227,   227,
     228,   228,   228,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   230,   230,   231,   231,   232,
     232,   233,   233,   233,   233,   233,   234,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   235,   236,   236,
     237,   237,   238,   239,   239,   239,   239,   239,   239,   239,
     239,   240,   240,   240,   240,   240,   240,   240,   240,   240,
     240,   240,   240,   240,   240,   240,   240,   240,   240,   240,
     240,   240,   240,   240,   241,   241,   242,   243,   243,   244,
     244,   244,   244,   245,   245,   246,   247,   247,   247,   247,
     248,   248,   248,   248,   249,   249,   249,   249,   249,   249,
     249,   249,   249,   249,   250,   250,   251,   252,   252,   253,
     254,   254,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   256,   256,   257,   257,   257,   258,
     259,   259,   260,   260,   261,   261,   262,   262,   263,   263,
     264,   264,   265,   265,   265,   265,   266,   266
>>>>>>> 13445115 (Add insert embedding syntax.)
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     4,     7,     6,
       7,     6,     1,     3,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
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
<<<<<<< HEAD
       2,     2,     2,     2,     2,     3,     3,     2,     3,     4,
       6,     3,     2,     2,     2,     2,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     1,     3,     3,
       5,     3,     1,     1,     1,     1,     1,     1,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,    10,
       6,     8,     4,     6,     6,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     4,     5,     4,     3,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     6,     3,     4,     3,     3,     5,     5,
       6,     4,     6,     3,     5,     4,     5,     6,     4,     5,
       5,     6,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     2,     1,     1,     1,     2,     2,     3,     2,
=======
       2,     2,     2,     2,     2,     2,     3,     4,     6,     3,
       2,     2,     2,     2,     4,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     1,     3,     3,     5,     3,
       1,     1,     1,     1,     1,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,    10,     6,     8,
       4,     6,     6,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     4,     5,     4,     3,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     6,     3,     4,     3,     3,     5,     5,     6,     4,
       6,     3,     5,     4,     5,     6,     4,     5,     5,     6,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       2,     1,     1,     1,     1,     1,     2,     2,     3,     2,
>>>>>>> 13445115 (Add insert embedding syntax.)
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     3,     2,     2,     1,     1,
       2,     0,     3,     0,     1,     0,     2,     0,     4,     0,
       1,     3,     1,     3,     3,     3,     6,     7
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
#line 296 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 1972 "parser.cpp"
=======
#line 1976 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 296 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 1980 "parser.cpp"
=======
#line 1984 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 210 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
<<<<<<< HEAD
#line 1994 "parser.cpp"
=======
#line 1998 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 200 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
<<<<<<< HEAD
#line 2008 "parser.cpp"
=======
#line 2012 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 289 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
<<<<<<< HEAD
#line 2019 "parser.cpp"
=======
#line 2023 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
<<<<<<< HEAD
#line 2028 "parser.cpp"
=======
#line 2032 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
<<<<<<< HEAD
#line 2037 "parser.cpp"
=======
#line 2041 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 260 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
<<<<<<< HEAD
#line 2051 "parser.cpp"
=======
#line 2055 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 253 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
<<<<<<< HEAD
#line 2062 "parser.cpp"
=======
#line 2066 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2072 "parser.cpp"
=======
#line 2076 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2082 "parser.cpp"
=======
#line 2086 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2092 "parser.cpp"
=======
#line 2096 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2102 "parser.cpp"
=======
#line 2106 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2112 "parser.cpp"
=======
#line 2116 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
<<<<<<< HEAD
#line 2122 "parser.cpp"
=======
#line 2126 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 243 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2136 "parser.cpp"
=======
#line 2140 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 243 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2150 "parser.cpp"
=======
#line 2154 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
<<<<<<< HEAD
#line 2160 "parser.cpp"
=======
#line 2164 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2168 "parser.cpp"
=======
#line 2172 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2176 "parser.cpp"
=======
#line 2180 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2185 "parser.cpp"
=======
#line 2189 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2193 "parser.cpp"
=======
#line 2197 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2201 "parser.cpp"
=======
#line 2205 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2209 "parser.cpp"
=======
#line 2213 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
<<<<<<< HEAD
#line 2223 "parser.cpp"
=======
#line 2227 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2232 "parser.cpp"
=======
#line 2236 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2241 "parser.cpp"
=======
#line 2245 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2250 "parser.cpp"
=======
#line 2254 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 280 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
<<<<<<< HEAD
#line 2263 "parser.cpp"
=======
#line 2267 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
<<<<<<< HEAD
#line 2272 "parser.cpp"
=======
#line 2276 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 270 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2286 "parser.cpp"
=======
#line 2290 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 270 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
<<<<<<< HEAD
#line 2300 "parser.cpp"
=======
#line 2304 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
<<<<<<< HEAD
#line 2310 "parser.cpp"
=======
#line 2314 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
<<<<<<< HEAD
#line 2319 "parser.cpp"
=======
#line 2323 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
<<<<<<< HEAD
#line 2333 "parser.cpp"
=======
#line 2337 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 230 "parser.y"
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
<<<<<<< HEAD
#line 2350 "parser.cpp"
=======
#line 2354 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2358 "parser.cpp"
=======
#line 2362 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_expr: /* expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2366 "parser.cpp"
=======
#line 2370 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_operand: /* operand  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2374 "parser.cpp"
=======
#line 2378 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2382 "parser.cpp"
=======
#line 2386 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2390 "parser.cpp"
=======
#line 2394 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2398 "parser.cpp"
=======
#line 2402 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2406 "parser.cpp"
=======
#line 2410 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 220 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
<<<<<<< HEAD
#line 2420 "parser.cpp"
=======
#line 2424 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2428 "parser.cpp"
=======
#line 2432 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2436 "parser.cpp"
=======
#line 2440 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2444 "parser.cpp"
=======
#line 2448 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2452 "parser.cpp"
=======
#line 2456 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2460 "parser.cpp"
=======
#line 2464 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 341 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
<<<<<<< HEAD
#line 2473 "parser.cpp"
=======
#line 2477 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2481 "parser.cpp"
=======
#line 2485 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2489 "parser.cpp"
=======
#line 2493 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
<<<<<<< HEAD
#line 2497 "parser.cpp"
=======
#line 2501 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2505 "parser.cpp"
=======
#line 2509 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2513 "parser.cpp"
=======
#line 2517 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2521 "parser.cpp"
=======
#line 2525 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2529 "parser.cpp"
=======
#line 2533 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2537 "parser.cpp"
=======
#line 2541 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2545 "parser.cpp"
=======
#line 2549 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
<<<<<<< HEAD
#line 2553 "parser.cpp"
=======
#line 2557 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 296 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
<<<<<<< HEAD
#line 2561 "parser.cpp"
=======
#line 2565 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 193 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
<<<<<<< HEAD
#line 2572 "parser.cpp"
=======
#line 2576 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_with_index_param_list: /* with_index_param_list  */
#line 176 "parser.y"
            {
    fprintf(stderr, "destroy create index param list\n");
    if ((((*yyvaluep).with_index_param_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_param_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_param_list_t));
    }
}
<<<<<<< HEAD
#line 2586 "parser.cpp"
=======
#line 2590 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
        break;

    case YYSYMBOL_index_info_list: /* index_info_list  */
#line 166 "parser.y"
            {
    fprintf(stderr, "destroy index info list\n");
    if ((((*yyvaluep).index_info_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).index_info_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).index_info_list_t));
    }
}
<<<<<<< HEAD
#line 2600 "parser.cpp"
=======
#line 2604 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
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
#line 84 "parser.y"
{
  // Initialize
  yylloc.first_column = 0;
  yylloc.last_column = 0;
  yylloc.first_line = 0;
  yylloc.last_line = 0;
  yylloc.total_column = 0;
  yylloc.string_length = 0;
}

<<<<<<< HEAD
#line 2708 "parser.cpp"
=======
#line 2712 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)

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
#line 463 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
<<<<<<< HEAD
#line 2923 "parser.cpp"
=======
#line 2927 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 3: /* statement_list: statement  */
#line 467 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
<<<<<<< HEAD
#line 2934 "parser.cpp"
=======
#line 2938 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 473 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
<<<<<<< HEAD
#line 2945 "parser.cpp"
=======
#line 2949 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 5: /* statement: create_statement  */
#line 480 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
<<<<<<< HEAD
#line 2951 "parser.cpp"
=======
#line 2955 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 6: /* statement: drop_statement  */
#line 481 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
<<<<<<< HEAD
#line 2957 "parser.cpp"
=======
#line 2961 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 7: /* statement: copy_statement  */
#line 482 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
<<<<<<< HEAD
#line 2963 "parser.cpp"
=======
#line 2967 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 8: /* statement: show_statement  */
#line 483 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
<<<<<<< HEAD
#line 2969 "parser.cpp"
=======
#line 2973 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 9: /* statement: select_statement  */
#line 484 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
<<<<<<< HEAD
#line 2975 "parser.cpp"
=======
#line 2979 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 10: /* statement: delete_statement  */
#line 485 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
<<<<<<< HEAD
#line 2981 "parser.cpp"
=======
#line 2985 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 11: /* statement: update_statement  */
#line 486 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
<<<<<<< HEAD
#line 2987 "parser.cpp"
=======
#line 2991 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 12: /* statement: insert_statement  */
#line 487 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
<<<<<<< HEAD
#line 2993 "parser.cpp"
=======
#line 2997 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 13: /* statement: explain_statement  */
#line 488 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
<<<<<<< HEAD
#line 2999 "parser.cpp"
=======
#line 3003 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 14: /* statement: flush_statement  */
#line 489 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
<<<<<<< HEAD
#line 3005 "parser.cpp"
=======
#line 3009 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 15: /* statement: command_statement  */
#line 490 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
<<<<<<< HEAD
#line 3011 "parser.cpp"
=======
#line 3015 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 16: /* explainable_statement: create_statement  */
#line 492 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
<<<<<<< HEAD
#line 3017 "parser.cpp"
=======
#line 3021 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 17: /* explainable_statement: drop_statement  */
#line 493 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
<<<<<<< HEAD
#line 3023 "parser.cpp"
=======
#line 3027 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 18: /* explainable_statement: copy_statement  */
#line 494 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
<<<<<<< HEAD
#line 3029 "parser.cpp"
=======
#line 3033 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 19: /* explainable_statement: show_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
<<<<<<< HEAD
#line 3035 "parser.cpp"
=======
#line 3039 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 20: /* explainable_statement: select_statement  */
#line 496 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
<<<<<<< HEAD
#line 3041 "parser.cpp"
=======
#line 3045 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 21: /* explainable_statement: delete_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
<<<<<<< HEAD
#line 3047 "parser.cpp"
=======
#line 3051 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 22: /* explainable_statement: update_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
<<<<<<< HEAD
#line 3053 "parser.cpp"
=======
#line 3057 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 23: /* explainable_statement: insert_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
<<<<<<< HEAD
#line 3059 "parser.cpp"
=======
#line 3063 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 24: /* explainable_statement: flush_statement  */
#line 500 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
<<<<<<< HEAD
#line 3065 "parser.cpp"
=======
#line 3069 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 25: /* explainable_statement: command_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
<<<<<<< HEAD
#line 3071 "parser.cpp"
=======
#line 3075 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 26: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 508 "parser.y"
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
<<<<<<< HEAD
#line 3091 "parser.cpp"
=======
#line 3095 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 27: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 525 "parser.y"
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
<<<<<<< HEAD
#line 3109 "parser.cpp"
=======
#line 3113 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 28: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 540 "parser.y"
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
<<<<<<< HEAD
#line 3137 "parser.cpp"
=======
#line 3141 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 29: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 564 "parser.y"
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
<<<<<<< HEAD
#line 3157 "parser.cpp"
=======
#line 3161 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 30: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 580 "parser.y"
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
<<<<<<< HEAD
#line 3178 "parser.cpp"
=======
#line 3182 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 31: /* create_statement: CREATE INDEX if_not_exists_info ON table_name index_info_list  */
#line 598 "parser.y"
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
<<<<<<< HEAD
#line 3211 "parser.cpp"
=======
#line 3215 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 32: /* table_element_array: table_element  */
#line 627 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
<<<<<<< HEAD
#line 3220 "parser.cpp"
=======
#line 3224 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 33: /* table_element_array: table_element_array ',' table_element  */
#line 631 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
<<<<<<< HEAD
#line 3229 "parser.cpp"
=======
#line 3233 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 34: /* table_element: table_column  */
#line 637 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
<<<<<<< HEAD
#line 3237 "parser.cpp"
=======
#line 3241 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 35: /* table_element: table_constraint  */
#line 640 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
<<<<<<< HEAD
#line 3245 "parser.cpp"
=======
#line 3249 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 36: /* table_column: IDENTIFIER column_type  */
#line 646 "parser.y"
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
<<<<<<< HEAD
#line 3285 "parser.cpp"
=======
#line 3289 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 37: /* table_column: IDENTIFIER column_type column_constraints  */
#line 681 "parser.y"
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
<<<<<<< HEAD
#line 3322 "parser.cpp"
=======
#line 3326 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 38: /* column_type: BOOLEAN  */
#line 715 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
<<<<<<< HEAD
#line 3328 "parser.cpp"
=======
#line 3332 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 39: /* column_type: TINYINT  */
#line 716 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
<<<<<<< HEAD
#line 3334 "parser.cpp"
=======
#line 3338 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 40: /* column_type: SMALLINT  */
#line 717 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
<<<<<<< HEAD
#line 3340 "parser.cpp"
=======
#line 3344 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 41: /* column_type: INTEGER  */
#line 718 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
<<<<<<< HEAD
#line 3346 "parser.cpp"
=======
#line 3350 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 42: /* column_type: INT  */
#line 719 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
<<<<<<< HEAD
#line 3352 "parser.cpp"
=======
#line 3356 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 43: /* column_type: BIGINT  */
#line 720 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
<<<<<<< HEAD
#line 3358 "parser.cpp"
=======
#line 3362 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 44: /* column_type: HUGEINT  */
#line 721 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
<<<<<<< HEAD
#line 3364 "parser.cpp"
=======
#line 3368 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 45: /* column_type: FLOAT  */
#line 722 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
<<<<<<< HEAD
#line 3370 "parser.cpp"
=======
#line 3374 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 46: /* column_type: REAL  */
#line 723 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
<<<<<<< HEAD
#line 3376 "parser.cpp"
=======
#line 3380 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 47: /* column_type: DOUBLE  */
#line 724 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
<<<<<<< HEAD
#line 3382 "parser.cpp"
=======
#line 3386 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 48: /* column_type: DATE  */
#line 725 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
<<<<<<< HEAD
#line 3388 "parser.cpp"
=======
#line 3392 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 49: /* column_type: TIME  */
#line 726 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
<<<<<<< HEAD
#line 3394 "parser.cpp"
=======
#line 3398 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 50: /* column_type: DATETIME  */
#line 727 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
<<<<<<< HEAD
#line 3400 "parser.cpp"
=======
#line 3404 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 51: /* column_type: TIMESTAMP  */
#line 728 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
<<<<<<< HEAD
#line 3406 "parser.cpp"
=======
#line 3410 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 52: /* column_type: UUID  */
#line 729 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
<<<<<<< HEAD
#line 3412 "parser.cpp"
=======
#line 3416 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 53: /* column_type: POINT  */
#line 730 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
<<<<<<< HEAD
#line 3418 "parser.cpp"
=======
#line 3422 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 54: /* column_type: LINE  */
#line 731 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
<<<<<<< HEAD
#line 3424 "parser.cpp"
=======
#line 3428 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 55: /* column_type: LSEG  */
#line 732 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
<<<<<<< HEAD
#line 3430 "parser.cpp"
=======
#line 3434 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 56: /* column_type: BOX  */
#line 733 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
<<<<<<< HEAD
#line 3436 "parser.cpp"
=======
#line 3440 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 57: /* column_type: CIRCLE  */
#line 736 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
<<<<<<< HEAD
#line 3442 "parser.cpp"
=======
#line 3446 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 58: /* column_type: CHAR  */
#line 738 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
<<<<<<< HEAD
#line 3448 "parser.cpp"
=======
#line 3452 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 59: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 739 "parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
<<<<<<< HEAD
#line 3454 "parser.cpp"
=======
#line 3458 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 60: /* column_type: VARCHAR  */
#line 740 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
<<<<<<< HEAD
#line 3460 "parser.cpp"
=======
#line 3464 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 741 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
<<<<<<< HEAD
#line 3466 "parser.cpp"
=======
#line 3470 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 742 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
<<<<<<< HEAD
#line 3472 "parser.cpp"
=======
#line 3476 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 63: /* column_type: DECIMAL  */
#line 743 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
<<<<<<< HEAD
#line 3478 "parser.cpp"
=======
#line 3482 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 746 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
<<<<<<< HEAD
#line 3484 "parser.cpp"
=======
#line 3488 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 747 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
<<<<<<< HEAD
#line 3490 "parser.cpp"
=======
#line 3494 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 748 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
<<<<<<< HEAD
#line 3496 "parser.cpp"
=======
#line 3500 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 749 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
<<<<<<< HEAD
#line 3502 "parser.cpp"
=======
#line 3506 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 750 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
<<<<<<< HEAD
#line 3508 "parser.cpp"
=======
#line 3512 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 751 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
<<<<<<< HEAD
#line 3514 "parser.cpp"
=======
#line 3518 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 752 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
<<<<<<< HEAD
#line 3520 "parser.cpp"
=======
#line 3524 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 753 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
<<<<<<< HEAD
#line 3526 "parser.cpp"
=======
#line 3530 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 754 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
<<<<<<< HEAD
#line 3532 "parser.cpp"
=======
#line 3536 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 755 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
<<<<<<< HEAD
#line 3538 "parser.cpp"
=======
#line 3542 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 756 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
<<<<<<< HEAD
#line 3544 "parser.cpp"
=======
#line 3548 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
<<<<<<< HEAD
#line 3550 "parser.cpp"
=======
#line 3554 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 758 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
<<<<<<< HEAD
#line 3556 "parser.cpp"
=======
#line 3560 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 759 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
<<<<<<< HEAD
#line 3562 "parser.cpp"
=======
#line 3566 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 760 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
<<<<<<< HEAD
#line 3568 "parser.cpp"
=======
#line 3572 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
<<<<<<< HEAD
#line 3574 "parser.cpp"
=======
#line 3578 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 80: /* column_constraints: column_constraint  */
#line 780 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
<<<<<<< HEAD
#line 3583 "parser.cpp"
=======
#line 3587 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 81: /* column_constraints: column_constraints column_constraint  */
#line 784 "parser.y"
                                       {
    if((yyvsp[-1].column_constraints_t)->contains((yyvsp[0].column_constraint_t))) {
        yyerror(&yyloc, scanner, result, "Duplicate column constraint.");
        delete (yyvsp[-1].column_constraints_t);
        YYERROR;
    }
    (yyvsp[-1].column_constraints_t)->insert((yyvsp[0].column_constraint_t));
    (yyval.column_constraints_t) = (yyvsp[-1].column_constraints_t);
}
<<<<<<< HEAD
#line 3597 "parser.cpp"
=======
#line 3601 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 794 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
<<<<<<< HEAD
#line 3605 "parser.cpp"
=======
#line 3609 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 797 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
<<<<<<< HEAD
#line 3613 "parser.cpp"
=======
#line 3617 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 800 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
<<<<<<< HEAD
#line 3621 "parser.cpp"
=======
#line 3625 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 803 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
<<<<<<< HEAD
#line 3629 "parser.cpp"
=======
#line 3633 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 86: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 807 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
<<<<<<< HEAD
#line 3639 "parser.cpp"
=======
#line 3643 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 87: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 812 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
<<<<<<< HEAD
#line 3649 "parser.cpp"
=======
#line 3653 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 88: /* identifier_array: IDENTIFIER  */
#line 819 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 3660 "parser.cpp"
=======
#line 3664 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 89: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 825 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
<<<<<<< HEAD
#line 3671 "parser.cpp"
=======
#line 3675 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 90: /* delete_statement: DELETE FROM table_name where_clause  */
#line 835 "parser.y"
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
<<<<<<< HEAD
#line 3688 "parser.cpp"
=======
#line 3692 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 91: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 851 "parser.y"
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
<<<<<<< HEAD
#line 3705 "parser.cpp"
=======
#line 3709 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 92: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 863 "parser.y"
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
<<<<<<< HEAD
#line 3722 "parser.cpp"
=======
#line 3726 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 93: /* optional_identifier_array: '(' identifier_array ')'  */
#line 876 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
<<<<<<< HEAD
#line 3730 "parser.cpp"
=======
#line 3734 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 94: /* optional_identifier_array: %empty  */
#line 879 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
<<<<<<< HEAD
#line 3738 "parser.cpp"
=======
#line 3742 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 95: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 886 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
<<<<<<< HEAD
#line 3748 "parser.cpp"
=======
#line 3752 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 96: /* explain_type: ANALYZE  */
#line 892 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
<<<<<<< HEAD
#line 3756 "parser.cpp"
=======
#line 3760 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 97: /* explain_type: AST  */
#line 895 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
<<<<<<< HEAD
#line 3764 "parser.cpp"
=======
#line 3768 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 98: /* explain_type: RAW  */
#line 898 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
<<<<<<< HEAD
#line 3772 "parser.cpp"
=======
#line 3776 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 99: /* explain_type: LOGICAL  */
#line 901 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
<<<<<<< HEAD
#line 3780 "parser.cpp"
=======
#line 3784 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 100: /* explain_type: PHYSICAL  */
#line 904 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
<<<<<<< HEAD
#line 3788 "parser.cpp"
=======
#line 3792 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 101: /* explain_type: PIPELINE  */
#line 907 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
<<<<<<< HEAD
#line 3796 "parser.cpp"
=======
#line 3800 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 102: /* explain_type: FRAGMENT  */
#line 910 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
<<<<<<< HEAD
#line 3804 "parser.cpp"
=======
#line 3808 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 103: /* explain_type: %empty  */
#line 913 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
<<<<<<< HEAD
#line 3812 "parser.cpp"
=======
#line 3816 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 104: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 920 "parser.y"
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
<<<<<<< HEAD
#line 3829 "parser.cpp"
=======
#line 3833 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 105: /* update_expr_array: update_expr  */
#line 933 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
<<<<<<< HEAD
#line 3838 "parser.cpp"
=======
#line 3842 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 106: /* update_expr_array: update_expr_array ',' update_expr  */
#line 937 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
<<<<<<< HEAD
#line 3847 "parser.cpp"
=======
#line 3851 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 107: /* update_expr: IDENTIFIER '=' expr  */
#line 942 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 3859 "parser.cpp"
=======
#line 3863 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 108: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 955 "parser.y"
                                                   {
    (yyval.drop_stmt) = new infinity::DropStatement();
    std::shared_ptr<infinity::DropSchemaInfo> drop_schema_info = std::make_shared<infinity::DropSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    drop_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.drop_stmt)->drop_info_ = drop_schema_info;
    (yyval.drop_stmt)->drop_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
<<<<<<< HEAD
#line 3875 "parser.cpp"
=======
#line 3879 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 109: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 968 "parser.y"
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
<<<<<<< HEAD
#line 3893 "parser.cpp"
=======
#line 3897 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 110: /* drop_statement: DROP TABLE if_exists table_name  */
#line 983 "parser.y"
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
<<<<<<< HEAD
#line 3911 "parser.cpp"
=======
#line 3915 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 111: /* drop_statement: DROP VIEW if_exists table_name  */
#line 998 "parser.y"
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
<<<<<<< HEAD
#line 3929 "parser.cpp"
=======
#line 3933 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 112: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 1013 "parser.y"
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
<<<<<<< HEAD
#line 3952 "parser.cpp"
=======
#line 3956 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 113: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1036 "parser.y"
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
<<<<<<< HEAD
#line 3998 "parser.cpp"
=======
#line 4002 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 114: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1077 "parser.y"
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
<<<<<<< HEAD
#line 4044 "parser.cpp"
=======
#line 4048 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 115: /* select_statement: select_without_paren  */
#line 1122 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
<<<<<<< HEAD
#line 4052 "parser.cpp"
=======
#line 4056 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 116: /* select_statement: select_with_paren  */
#line 1125 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
<<<<<<< HEAD
#line 4060 "parser.cpp"
=======
#line 4064 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 117: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1128 "parser.y"
                                                                     {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
<<<<<<< HEAD
#line 4074 "parser.cpp"
=======
#line 4078 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 118: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1137 "parser.y"
                                                               {
    infinity::SelectStatement* node = (yyvsp[-2].select_stmt);
    while(node->nested_select_ != nullptr) {
        node = node->nested_select_;
    }
    node->set_op_ = (yyvsp[-1].set_operator_t);
    node->nested_select_ = (yyvsp[0].select_stmt);
    (yyval.select_stmt) = (yyvsp[-2].select_stmt);
}
<<<<<<< HEAD
#line 4088 "parser.cpp"
=======
#line 4092 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 119: /* select_with_paren: '(' select_without_paren ')'  */
#line 1147 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4096 "parser.cpp"
=======
#line 4100 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 120: /* select_with_paren: '(' select_with_paren ')'  */
#line 1150 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4104 "parser.cpp"
=======
#line 4108 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 121: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1154 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
<<<<<<< HEAD
#line 4113 "parser.cpp"
=======
#line 4117 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 122: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1159 "parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
<<<<<<< HEAD
#line 4124 "parser.cpp"
=======
#line 4128 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 123: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1166 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4132 "parser.cpp"
=======
#line 4136 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 124: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1169 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4140 "parser.cpp"
=======
#line 4144 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 125: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1174 "parser.y"
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
<<<<<<< HEAD
#line 4160 "parser.cpp"
=======
#line 4164 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 126: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1190 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
<<<<<<< HEAD
#line 4168 "parser.cpp"
=======
#line 4172 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 127: /* order_by_clause: %empty  */
#line 1193 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
<<<<<<< HEAD
#line 4176 "parser.cpp"
=======
#line 4180 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 128: /* order_by_expr_list: order_by_expr  */
#line 1197 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
<<<<<<< HEAD
#line 4185 "parser.cpp"
=======
#line 4189 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 129: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1201 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
<<<<<<< HEAD
#line 4194 "parser.cpp"
=======
#line 4198 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 130: /* order_by_expr: expr order_by_type  */
#line 1206 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
<<<<<<< HEAD
#line 4204 "parser.cpp"
=======
#line 4208 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 131: /* order_by_type: ASC  */
#line 1212 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
<<<<<<< HEAD
#line 4212 "parser.cpp"
=======
#line 4216 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 132: /* order_by_type: DESC  */
#line 1215 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
<<<<<<< HEAD
#line 4220 "parser.cpp"
=======
#line 4224 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 133: /* order_by_type: %empty  */
#line 1218 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
<<<<<<< HEAD
#line 4228 "parser.cpp"
=======
#line 4232 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 134: /* limit_expr: LIMIT expr  */
#line 1222 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 4236 "parser.cpp"
=======
#line 4240 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 135: /* limit_expr: %empty  */
#line 1226 "parser.y"
{   (yyval.expr_t) = nullptr; }
<<<<<<< HEAD
#line 4242 "parser.cpp"
=======
#line 4246 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 136: /* offset_expr: OFFSET expr  */
#line 1228 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 4250 "parser.cpp"
=======
#line 4254 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 137: /* offset_expr: %empty  */
#line 1232 "parser.y"
{   (yyval.expr_t) = nullptr; }
<<<<<<< HEAD
#line 4256 "parser.cpp"
=======
#line 4260 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 138: /* distinct: DISTINCT  */
#line 1234 "parser.y"
                    {
    (yyval.bool_value) = true;
}
<<<<<<< HEAD
#line 4264 "parser.cpp"
=======
#line 4268 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 139: /* distinct: %empty  */
#line 1237 "parser.y"
  {
    (yyval.bool_value) = false;
}
<<<<<<< HEAD
#line 4272 "parser.cpp"
=======
#line 4276 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 140: /* from_clause: FROM table_reference  */
#line 1241 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
<<<<<<< HEAD
#line 4280 "parser.cpp"
=======
#line 4284 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 141: /* from_clause: %empty  */
#line 1244 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
<<<<<<< HEAD
#line 4288 "parser.cpp"
=======
#line 4292 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 142: /* search_clause: SEARCH sub_search_array  */
#line 1248 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
<<<<<<< HEAD
#line 4298 "parser.cpp"
=======
#line 4302 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 143: /* search_clause: %empty  */
#line 1253 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
<<<<<<< HEAD
#line 4306 "parser.cpp"
=======
#line 4310 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1257 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 4314 "parser.cpp"
=======
#line 4318 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 145: /* where_clause: %empty  */
#line 1260 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
<<<<<<< HEAD
#line 4322 "parser.cpp"
=======
#line 4326 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1264 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 4330 "parser.cpp"
=======
#line 4334 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 147: /* having_clause: %empty  */
#line 1267 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
<<<<<<< HEAD
#line 4338 "parser.cpp"
=======
#line 4342 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1271 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
<<<<<<< HEAD
#line 4346 "parser.cpp"
=======
#line 4350 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 149: /* group_by_clause: %empty  */
#line 1274 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
<<<<<<< HEAD
#line 4354 "parser.cpp"
=======
#line 4358 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 150: /* set_operator: UNION  */
#line 1278 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
<<<<<<< HEAD
#line 4362 "parser.cpp"
=======
#line 4366 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1281 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
<<<<<<< HEAD
#line 4370 "parser.cpp"
=======
#line 4374 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1284 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
<<<<<<< HEAD
#line 4378 "parser.cpp"
=======
#line 4382 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1287 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
<<<<<<< HEAD
#line 4386 "parser.cpp"
=======
#line 4390 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1295 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
<<<<<<< HEAD
#line 4394 "parser.cpp"
=======
#line 4398 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 155: /* table_reference: table_reference ',' table_reference_unit  */
#line 1298 "parser.y"
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
<<<<<<< HEAD
#line 4412 "parser.cpp"
=======
#line 4416 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 158: /* table_reference_name: table_name table_alias  */
#line 1315 "parser.y"
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
<<<<<<< HEAD
#line 4430 "parser.cpp"
=======
#line 4434 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1329 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
<<<<<<< HEAD
#line 4441 "parser.cpp"
=======
#line 4445 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 160: /* table_name: IDENTIFIER  */
#line 1338 "parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
<<<<<<< HEAD
#line 4453 "parser.cpp"
=======
#line 4457 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 161: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1345 "parser.y"
                            {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[-2].str_value));
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->schema_name_ptr_ = (yyvsp[-2].str_value);
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
<<<<<<< HEAD
#line 4467 "parser.cpp"
=======
#line 4471 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1356 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
<<<<<<< HEAD
#line 4477 "parser.cpp"
=======
#line 4481 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1361 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
<<<<<<< HEAD
#line 4487 "parser.cpp"
=======
#line 4491 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1366 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
<<<<<<< HEAD
#line 4498 "parser.cpp"
=======
#line 4502 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 165: /* table_alias: %empty  */
#line 1372 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
<<<<<<< HEAD
#line 4506 "parser.cpp"
=======
#line 4510 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1379 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
<<<<<<< HEAD
#line 4514 "parser.cpp"
=======
#line 4518 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 167: /* with_clause: %empty  */
#line 1382 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
<<<<<<< HEAD
#line 4522 "parser.cpp"
=======
#line 4526 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1386 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
<<<<<<< HEAD
#line 4531 "parser.cpp"
=======
#line 4535 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1389 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
<<<<<<< HEAD
#line 4540 "parser.cpp"
=======
#line 4544 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 170: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1394 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
<<<<<<< HEAD
#line 4552 "parser.cpp"
=======
#line 4556 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 171: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1406 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
<<<<<<< HEAD
#line 4564 "parser.cpp"
=======
#line 4568 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 172: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1413 "parser.y"
                                                                   {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-5].table_reference_t);
    join_reference->right_ = (yyvsp[-2].table_reference_t);
    join_reference->join_type_ = (yyvsp[-4].join_type_t);
    join_reference->condition_ = (yyvsp[0].expr_t);
    (yyval.table_reference_t) = join_reference;
}
<<<<<<< HEAD
#line 4577 "parser.cpp"
=======
#line 4581 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 173: /* join_type: INNER  */
#line 1427 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
<<<<<<< HEAD
#line 4585 "parser.cpp"
=======
#line 4589 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 174: /* join_type: LEFT  */
#line 1430 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
<<<<<<< HEAD
#line 4593 "parser.cpp"
=======
#line 4597 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 175: /* join_type: RIGHT  */
#line 1433 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
<<<<<<< HEAD
#line 4601 "parser.cpp"
=======
#line 4605 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 176: /* join_type: OUTER  */
#line 1436 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
<<<<<<< HEAD
#line 4609 "parser.cpp"
=======
#line 4613 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 177: /* join_type: FULL  */
#line 1439 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
<<<<<<< HEAD
#line 4617 "parser.cpp"
=======
#line 4621 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 178: /* join_type: CROSS  */
#line 1442 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
<<<<<<< HEAD
#line 4625 "parser.cpp"
=======
#line 4629 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 179: /* join_type: %empty  */
#line 1445 "parser.y"
                {
}
<<<<<<< HEAD
#line 4632 "parser.cpp"
=======
#line 4636 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 180: /* show_statement: SHOW DATABASES  */
#line 1451 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
<<<<<<< HEAD
#line 4641 "parser.cpp"
=======
#line 4645 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 181: /* show_statement: SHOW TABLES  */
#line 1455 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
<<<<<<< HEAD
#line 4650 "parser.cpp"
=======
#line 4654 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 182: /* show_statement: SHOW VIEWS  */
#line 1459 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
<<<<<<< HEAD
#line 4659 "parser.cpp"
=======
#line 4663 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 183: /* show_statement: SHOW CONFIGS  */
#line 1463 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
<<<<<<< HEAD
#line 4668 "parser.cpp"
=======
#line 4672 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 184: /* show_statement: SHOW PROFILES  */
#line 1467 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
<<<<<<< HEAD
#line 4677 "parser.cpp"
=======
#line 4681 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 185: /* show_statement: SHOW SESSION STATUS  */
#line 1471 "parser.y"
                      {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kSessionStatus;
}
#line 4686 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW GLOBAL STATUS  */
#line 1475 "parser.y"
                     {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kGlobalStatus;
}
#line 4695 "parser.cpp"
    break;

  case 187: /* show_statement: DESCRIBE table_name  */
#line 1479 "parser.y"
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
<<<<<<< HEAD
#line 4711 "parser.cpp"
=======
#line 4697 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 188: /* show_statement: DESCRIBE table_name SEGMENTS  */
#line 1490 "parser.y"
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
<<<<<<< HEAD
#line 4727 "parser.cpp"
=======
#line 4713 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 189: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE  */
#line 1501 "parser.y"
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
<<<<<<< HEAD
#line 4744 "parser.cpp"
=======
#line 4730 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 190: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1513 "parser.y"
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
<<<<<<< HEAD
#line 4762 "parser.cpp"
=======
#line 4748 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 191: /* show_statement: DESCRIBE INDEX table_name  */
#line 1526 "parser.y"
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
<<<<<<< HEAD
#line 4778 "parser.cpp"
=======
#line 4764 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 192: /* flush_statement: FLUSH DATA  */
#line 1541 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
<<<<<<< HEAD
#line 4787 "parser.cpp"
=======
#line 4773 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 193: /* flush_statement: FLUSH LOG  */
#line 1545 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
<<<<<<< HEAD
#line 4796 "parser.cpp"
=======
#line 4782 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 194: /* flush_statement: FLUSH BUFFER  */
#line 1549 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
<<<<<<< HEAD
#line 4805 "parser.cpp"
=======
#line 4791 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 195: /* command_statement: USE IDENTIFIER  */
#line 1557 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4816 "parser.cpp"
=======
#line 4802 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 196: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1563 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4826 "parser.cpp"
=======
#line 4812 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 197: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1568 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4837 "parser.cpp"
=======
#line 4823 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 198: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1574 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4848 "parser.cpp"
=======
#line 4834 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 199: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1580 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4861 "parser.cpp"
=======
#line 4847 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 200: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1588 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4872 "parser.cpp"
=======
#line 4858 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 201: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1594 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4883 "parser.cpp"
=======
#line 4869 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 202: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1600 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4894 "parser.cpp"
=======
#line 4880 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 203: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1606 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4905 "parser.cpp"
=======
#line 4891 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1612 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4918 "parser.cpp"
=======
#line 4904 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 205: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1620 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
<<<<<<< HEAD
#line 4929 "parser.cpp"
=======
#line 4915 "parser.cpp"
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1618 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4926 "parser.cpp"
    break;

  case 205: /* expr_array: expr_alias  */
#line 1629 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4935 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 206: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1626 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4940 "parser.cpp"
    break;

  case 207: /* expr_array: expr_alias  */
#line 1637 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4949 "parser.cpp"
    break;

  case 208: /* expr_array: expr_array ',' expr_alias  */
#line 1641 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
<<<<<<< HEAD
#line 4958 "parser.cpp"
=======
#line 4944 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 209: /* expr_array_list: '(' expr_array ')'  */
#line 1646 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
<<<<<<< HEAD
#line 4967 "parser.cpp"
=======
#line 4953 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 210: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1650 "parser.y"
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
<<<<<<< HEAD
#line 4987 "parser.cpp"
=======
#line 4973 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 211: /* expr_alias: expr AS IDENTIFIER  */
#line 1677 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
#line 4998 "parser.cpp"
=======
#line 4984 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 212: /* expr_alias: expr  */
#line 1683 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
<<<<<<< HEAD
#line 5006 "parser.cpp"
=======
#line 4992 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 218: /* operand: '(' expr ')'  */
#line 1693 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
<<<<<<< HEAD
#line 5014 "parser.cpp"
=======
#line 5000 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 219: /* operand: '(' select_without_paren ')'  */
#line 1696 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 5025 "parser.cpp"
=======
#line 5011 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 220: /* operand: constant_expr  */
#line 1702 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
<<<<<<< HEAD
#line 5033 "parser.cpp"
=======
#line 5019 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 229: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ')'  */
#line 1714 "parser.y"
                                                                 {
    infinity::KnnExpr* knn_expr = new infinity::KnnExpr();
    (yyval.expr_t) = knn_expr;

    // KNN search column
    knn_expr->column_expr_ = (yyvsp[-7].expr_t);

    // KNN distance type
    ParserHelper::ToLower((yyvsp[-1].str_value));
    if(strcmp((yyvsp[-1].str_value), "l2") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kL2;
    } else if(strcmp((yyvsp[-1].str_value), "ip") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if(strcmp((yyvsp[-1].str_value), "cosine") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if(strcmp((yyvsp[-1].str_value), "hamming") == 0) {
        knn_expr->distance_type_ = infinity::KnnDistanceType::kHamming;
    } else {
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn distance type");
        YYERROR;
    }

    // KNN data type
    ParserHelper::ToLower((yyvsp[-3].str_value));
    if(strcmp((yyvsp[-3].str_value), "float") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        knn_expr->embedding_data_ptr_ = new float[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((float*)(knn_expr->embedding_data_ptr_))[i] = (yyvsp[-5].const_expr_t)->double_array_[i];
        }
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
    } else if(strcmp((yyvsp[-3].str_value), "tinyint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        knn_expr->embedding_data_ptr_ = new char[knn_expr->dimension_]{};

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((char*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
    } else if(strcmp((yyvsp[-3].str_value), "smallint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        knn_expr->embedding_data_ptr_ = new short int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((short int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
    } else if(strcmp((yyvsp[-3].str_value), "integer") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        knn_expr->embedding_data_ptr_ = new int[knn_expr->dimension_];

        for(long i = 0; i < knn_expr->dimension_; ++ i) {
            ((int*)knn_expr->embedding_data_ptr_)[i] = (yyvsp[-5].const_expr_t)->long_array_[i];
        }
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);

    } else if(strcmp((yyvsp[-3].str_value), "bigint") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        knn_expr->embedding_data_ptr_ = new long[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-5].const_expr_t)->long_array_.data(), knn_expr->dimension_ * sizeof(long));
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);

    } else if(strcmp((yyvsp[-3].str_value), "bit") == 0 and knn_expr->distance_type_ == infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->long_array_.size();
        if(knn_expr->dimension_ % 8 == 0) {
            knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = knn_expr->dimension_ / 8;
            knn_expr->embedding_data_ptr_ = new char[embedding_size]{};

            for(long i = 0; i < embedding_size; ++ i) {
                char embedding_unit = 0;
                for(long bit_idx = 0; bit_idx < 8; ++ bit_idx) {
                    if((yyvsp[-5].const_expr_t)->long_array_[i * 8 + bit_idx] == 1) {
                        char temp = embedding_unit << 1;
                        temp &= 1;
                        embedding_unit = temp;
                    } else if((yyvsp[-5].const_expr_t)->long_array_[i * 8 + bit_idx] == 0) {
                        embedding_unit <<= 0;
                    } else {
                        free((yyvsp[-3].str_value));
                        free((yyvsp[-1].str_value));
                        delete (yyvsp[-5].const_expr_t);
                        delete (yyval.expr_t);
                        yyerror(&yyloc, scanner, result, "Invalid bit embedding type data");
                        YYERROR;
                    }
                }
                ((char*)knn_expr->embedding_data_ptr_)[i] = embedding_unit;
            }
            free((yyvsp[-3].str_value));
            free((yyvsp[-1].str_value));
            delete (yyvsp[-5].const_expr_t);
        } else {
            free((yyvsp[-3].str_value));
            free((yyvsp[-1].str_value));
            delete (yyvsp[-5].const_expr_t);
            delete (yyval.expr_t);
            yyerror(&yyloc, scanner, result, "KNN data type is bit which length should be aligned with 8");
            YYERROR;
        }

    } else if(strcmp((yyvsp[-3].str_value), "double") == 0 and knn_expr->distance_type_ != infinity::KnnDistanceType::kHamming) {
        knn_expr->dimension_ = (yyvsp[-5].const_expr_t)->double_array_.size();
        knn_expr->embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        knn_expr->embedding_data_ptr_ = new double[knn_expr->dimension_];

        memcpy(knn_expr->embedding_data_ptr_, (void*)(yyvsp[-5].const_expr_t)->double_array_.data(), knn_expr->dimension_ * sizeof(double));
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
    } else {
        free((yyvsp[-3].str_value));
        free((yyvsp[-1].str_value));
        delete (yyvsp[-5].const_expr_t);
        delete (yyval.expr_t);
        yyerror(&yyloc, scanner, result, "Invalid knn data type");
        YYERROR;
    }
}
<<<<<<< HEAD
#line 5178 "parser.cpp"
=======
#line 5164 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 230: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1855 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = std::string((yyvsp[-3].str_value));
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
<<<<<<< HEAD
#line 5191 "parser.cpp"
=======
#line 5177 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 231: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1863 "parser.y"
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
<<<<<<< HEAD
#line 5206 "parser.cpp"
=======
#line 5192 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 232: /* query_expr: QUERY '(' STRING ')'  */
#line 1874 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
<<<<<<< HEAD
#line 5217 "parser.cpp"
=======
#line 5203 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 233: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1880 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = std::string((yyvsp[-3].str_value));
    match_expr->options_text_ = std::string((yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
<<<<<<< HEAD
#line 5230 "parser.cpp"
=======
#line 5216 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 234: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1889 "parser.y"
                                               {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
<<<<<<< HEAD
#line 5243 "parser.cpp"
=======
#line 5229 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 235: /* sub_search_array: knn_expr  */
#line 1899 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
<<<<<<< HEAD
#line 5252 "parser.cpp"
    break;

  case 236: /* sub_search_array: match_expr  */
=======
#line 5238 "parser.cpp"
    break;

  case 234: /* sub_search_array: match_expr  */
#line 1895 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5247 "parser.cpp"
    break;

  case 235: /* sub_search_array: query_expr  */
#line 1899 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5256 "parser.cpp"
    break;

  case 236: /* sub_search_array: fusion_expr  */
>>>>>>> 13445115 (Add insert embedding syntax.)
#line 1903 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5261 "parser.cpp"
    break;

  case 237: /* sub_search_array: query_expr  */
#line 1907 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5270 "parser.cpp"
    break;

  case 238: /* sub_search_array: fusion_expr  */
#line 1911 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
<<<<<<< HEAD
#line 5279 "parser.cpp"
=======
#line 5265 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 239: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 1915 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
<<<<<<< HEAD
#line 5288 "parser.cpp"
    break;

  case 240: /* sub_search_array: sub_search_array ',' match_expr  */
=======
#line 5274 "parser.cpp"
    break;

  case 238: /* sub_search_array: sub_search_array ',' match_expr  */
#line 1911 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5283 "parser.cpp"
    break;

  case 239: /* sub_search_array: sub_search_array ',' query_expr  */
#line 1915 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5292 "parser.cpp"
    break;

  case 240: /* sub_search_array: sub_search_array ',' fusion_expr  */
>>>>>>> 13445115 (Add insert embedding syntax.)
#line 1919 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5297 "parser.cpp"
    break;

  case 241: /* sub_search_array: sub_search_array ',' query_expr  */
#line 1923 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5306 "parser.cpp"
    break;

  case 242: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 1927 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
<<<<<<< HEAD
#line 5315 "parser.cpp"
=======
#line 5301 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 243: /* function_expr: IDENTIFIER '(' ')'  */
#line 1932 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5328 "parser.cpp"
=======
#line 5314 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 244: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1940 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5341 "parser.cpp"
=======
#line 5327 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 245: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1948 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5355 "parser.cpp"
=======
#line 5341 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 246: /* function_expr: operand IS NOT NULLABLE  */
#line 1957 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5367 "parser.cpp"
=======
#line 5353 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 247: /* function_expr: operand IS NULLABLE  */
#line 1964 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5379 "parser.cpp"
=======
#line 5365 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 248: /* function_expr: NOT operand  */
#line 1971 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5391 "parser.cpp"
=======
#line 5377 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 249: /* function_expr: '-' operand  */
#line 1978 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5403 "parser.cpp"
=======
#line 5389 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 250: /* function_expr: '+' operand  */
#line 1985 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5415 "parser.cpp"
    break;

  case 251: /* function_expr: operand '-' operand  */
=======
#line 5401 "parser.cpp"
    break;

  case 249: /* function_expr: operand '-' operand  */
#line 1984 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5414 "parser.cpp"
    break;

  case 250: /* function_expr: operand '+' operand  */
>>>>>>> 13445115 (Add insert embedding syntax.)
#line 1992 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5428 "parser.cpp"
    break;

  case 252: /* function_expr: operand '+' operand  */
#line 2000 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5441 "parser.cpp"
=======
#line 5427 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 253: /* function_expr: operand '*' operand  */
#line 2008 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5454 "parser.cpp"
=======
#line 5440 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 254: /* function_expr: operand '/' operand  */
#line 2016 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5467 "parser.cpp"
=======
#line 5453 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 255: /* function_expr: operand '%' operand  */
#line 2024 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5480 "parser.cpp"
=======
#line 5466 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 256: /* function_expr: operand '=' operand  */
#line 2032 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5493 "parser.cpp"
=======
#line 5479 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 257: /* function_expr: operand EQUAL operand  */
#line 2040 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5506 "parser.cpp"
=======
#line 5492 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 258: /* function_expr: operand NOT_EQ operand  */
#line 2048 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5519 "parser.cpp"
=======
#line 5505 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 259: /* function_expr: operand '<' operand  */
#line 2056 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5532 "parser.cpp"
=======
#line 5518 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 260: /* function_expr: operand '>' operand  */
#line 2064 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5545 "parser.cpp"
=======
#line 5531 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 261: /* function_expr: operand LESS_EQ operand  */
#line 2072 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5558 "parser.cpp"
=======
#line 5544 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 262: /* function_expr: operand GREATER_EQ operand  */
#line 2080 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5571 "parser.cpp"
=======
#line 5557 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 263: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2088 "parser.y"
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
<<<<<<< HEAD
#line 5606 "parser.cpp"
=======
#line 5592 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 264: /* function_expr: operand LIKE operand  */
#line 2118 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5619 "parser.cpp"
=======
#line 5605 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 265: /* function_expr: operand NOT LIKE operand  */
#line 2126 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5632 "parser.cpp"
=======
#line 5618 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 266: /* conjunction_expr: expr AND expr  */
#line 2135 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5645 "parser.cpp"
=======
#line 5631 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 267: /* conjunction_expr: expr OR expr  */
#line 2143 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
<<<<<<< HEAD
#line 5658 "parser.cpp"
=======
#line 5644 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 268: /* between_expr: operand BETWEEN operand AND operand  */
#line 2152 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
<<<<<<< HEAD
#line 5670 "parser.cpp"
=======
#line 5656 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 269: /* in_expr: operand IN '(' expr_array ')'  */
#line 2160 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
<<<<<<< HEAD
#line 5681 "parser.cpp"
=======
#line 5667 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 270: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2166 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
<<<<<<< HEAD
#line 5692 "parser.cpp"
=======
#line 5678 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 271: /* case_expr: CASE expr case_check_array END  */
#line 2173 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 5703 "parser.cpp"
=======
#line 5689 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 272: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2179 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 5715 "parser.cpp"
=======
#line 5701 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 273: /* case_expr: CASE case_check_array END  */
#line 2186 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 5725 "parser.cpp"
=======
#line 5711 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 274: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2191 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
<<<<<<< HEAD
#line 5736 "parser.cpp"
=======
#line 5722 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 275: /* case_check_array: WHEN expr THEN expr  */
#line 2198 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
<<<<<<< HEAD
#line 5748 "parser.cpp"
=======
#line 5734 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 276: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2205 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
<<<<<<< HEAD
#line 5760 "parser.cpp"
=======
#line 5746 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 277: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2213 "parser.y"
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
<<<<<<< HEAD
#line 5788 "parser.cpp"
=======
#line 5774 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 278: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2237 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 5799 "parser.cpp"
=======
#line 5785 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 279: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2243 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 5810 "parser.cpp"
=======
#line 5796 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 280: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2249 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 5822 "parser.cpp"
=======
#line 5808 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 281: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2256 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
<<<<<<< HEAD
#line 5834 "parser.cpp"
=======
#line 5820 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 282: /* column_expr: IDENTIFIER  */
#line 2264 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 5846 "parser.cpp"
=======
#line 5832 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 283: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2271 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 5858 "parser.cpp"
=======
#line 5844 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 284: /* column_expr: '*'  */
#line 2278 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 5868 "parser.cpp"
=======
#line 5854 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 285: /* column_expr: column_expr '.' '*'  */
#line 2283 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
<<<<<<< HEAD
#line 5882 "parser.cpp"
=======
#line 5868 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 286: /* constant_expr: STRING  */
#line 2293 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5892 "parser.cpp"
=======
#line 5878 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 287: /* constant_expr: TRUE  */
#line 2298 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5902 "parser.cpp"
=======
#line 5888 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 288: /* constant_expr: FALSE  */
#line 2303 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5912 "parser.cpp"
=======
#line 5898 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 289: /* constant_expr: DOUBLE_VALUE  */
#line 2308 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5922 "parser.cpp"
=======
#line 5908 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 290: /* constant_expr: LONG_VALUE  */
#line 2313 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5932 "parser.cpp"
=======
#line 5918 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 291: /* constant_expr: DATE STRING  */
#line 2318 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
#line 5942 "parser.cpp"
=======
#line 5928 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 292: /* constant_expr: INTERVAL interval_expr  */
#line 2323 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
<<<<<<< HEAD
#line 5950 "parser.cpp"
=======
#line 5936 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
    break;

  case 293: /* constant_expr: interval_expr  */
#line 2326 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
<<<<<<< HEAD
#line 5958 "parser.cpp"
    break;

  case 294: /* array_expr: long_array_expr  */
#line 2338 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5966 "parser.cpp"
    break;

  case 295: /* array_expr: double_array_expr  */
#line 2341 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5974 "parser.cpp"
    break;

  case 296: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2345 "parser.y"
=======
#line 5944 "parser.cpp"
    break;

  case 292: /* constant_expr: long_array_expr  */
#line 2321 "parser.y"
                  {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5952 "parser.cpp"
    break;

  case 293: /* constant_expr: double_array_expr  */
#line 2324 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5960 "parser.cpp"
    break;

  case 294: /* array_expr: long_array_expr  */
#line 2328 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5968 "parser.cpp"
    break;

  case 295: /* array_expr: double_array_expr  */
#line 2331 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5976 "parser.cpp"
    break;

  case 296: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2335 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 5982 "parser.cpp"
    break;

  case 297: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2349 "parser.y"
=======
#line 5985 "parser.cpp"
    break;

  case 297: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2340 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 5984 "parser.cpp"
    break;

  case 297: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2339 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 5992 "parser.cpp"
    break;

  case 298: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2354 "parser.y"
=======
#line 5995 "parser.cpp"
    break;

  case 298: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2345 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 5994 "parser.cpp"
    break;

  case 298: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2344 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6001 "parser.cpp"
    break;

  case 299: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2359 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6009 "parser.cpp"
    break;

  case 300: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2363 "parser.y"
=======
#line 6004 "parser.cpp"
=======
#line 6003 "parser.cpp"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
    break;

  case 299: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2349 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 6011 "parser.cpp"
    break;

  case 300: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
<<<<<<< HEAD
#line 2354 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 2353 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6019 "parser.cpp"
    break;

  case 301: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2368 "parser.y"
=======
#line 6022 "parser.cpp"
    break;

  case 301: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2359 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6021 "parser.cpp"
    break;

  case 301: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2358 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6028 "parser.cpp"
    break;

  case 302: /* interval_expr: LONG_VALUE SECONDS  */
#line 2373 "parser.y"
=======
#line 6031 "parser.cpp"
    break;

  case 302: /* interval_expr: LONG_VALUE SECONDS  */
#line 2364 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6030 "parser.cpp"
    break;

  case 302: /* interval_expr: LONG_VALUE SECONDS  */
#line 2363 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6039 "parser.cpp"
    break;

  case 303: /* interval_expr: LONG_VALUE SECOND  */
#line 2379 "parser.y"
=======
#line 6042 "parser.cpp"
    break;

  case 303: /* interval_expr: LONG_VALUE SECOND  */
#line 2370 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6041 "parser.cpp"
    break;

  case 303: /* interval_expr: LONG_VALUE SECOND  */
#line 2369 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6050 "parser.cpp"
    break;

  case 304: /* interval_expr: LONG_VALUE MINUTES  */
#line 2385 "parser.y"
=======
#line 6053 "parser.cpp"
    break;

  case 304: /* interval_expr: LONG_VALUE MINUTES  */
#line 2376 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6052 "parser.cpp"
    break;

  case 304: /* interval_expr: LONG_VALUE MINUTES  */
#line 2375 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6061 "parser.cpp"
    break;

  case 305: /* interval_expr: LONG_VALUE MINUTE  */
#line 2391 "parser.y"
=======
#line 6064 "parser.cpp"
    break;

  case 305: /* interval_expr: LONG_VALUE MINUTE  */
#line 2382 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6063 "parser.cpp"
    break;

  case 305: /* interval_expr: LONG_VALUE MINUTE  */
#line 2381 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6072 "parser.cpp"
    break;

  case 306: /* interval_expr: LONG_VALUE HOURS  */
#line 2397 "parser.y"
=======
#line 6075 "parser.cpp"
    break;

  case 306: /* interval_expr: LONG_VALUE HOURS  */
#line 2388 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6074 "parser.cpp"
    break;

  case 306: /* interval_expr: LONG_VALUE HOURS  */
#line 2387 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6083 "parser.cpp"
    break;

  case 307: /* interval_expr: LONG_VALUE HOUR  */
#line 2403 "parser.y"
=======
#line 6086 "parser.cpp"
    break;

  case 307: /* interval_expr: LONG_VALUE HOUR  */
#line 2394 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6085 "parser.cpp"
    break;

  case 307: /* interval_expr: LONG_VALUE HOUR  */
#line 2393 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6094 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE DAYS  */
#line 2409 "parser.y"
=======
#line 6097 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE DAYS  */
#line 2400 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6096 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE DAYS  */
#line 2399 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6105 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE DAY  */
#line 2415 "parser.y"
=======
#line 6108 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE DAY  */
#line 2406 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6107 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE DAY  */
#line 2405 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6116 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE MONTHS  */
#line 2421 "parser.y"
=======
#line 6119 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE MONTHS  */
#line 2412 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6118 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE MONTHS  */
#line 2411 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6127 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE MONTH  */
#line 2427 "parser.y"
=======
#line 6130 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE MONTH  */
#line 2418 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6129 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE MONTH  */
#line 2417 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6138 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE YEARS  */
#line 2433 "parser.y"
=======
#line 6141 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE YEARS  */
#line 2424 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6140 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE YEARS  */
#line 2423 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6149 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE YEAR  */
#line 2439 "parser.y"
=======
#line 6152 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE YEAR  */
#line 2430 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6151 "parser.cpp"
    break;

  case 313: /* interval_expr: LONG_VALUE YEAR  */
#line 2429 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6160 "parser.cpp"
    break;

  case 314: /* copy_option_list: copy_option  */
#line 2450 "parser.y"
=======
#line 6163 "parser.cpp"
    break;

  case 314: /* copy_option_list: copy_option  */
#line 2441 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6162 "parser.cpp"
    break;

  case 314: /* copy_option_list: copy_option  */
#line 2440 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6169 "parser.cpp"
    break;

  case 315: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2454 "parser.y"
=======
#line 6172 "parser.cpp"
    break;

  case 315: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2445 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6171 "parser.cpp"
    break;

  case 315: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2444 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6178 "parser.cpp"
    break;

  case 316: /* copy_option: FORMAT IDENTIFIER  */
#line 2459 "parser.y"
=======
#line 6181 "parser.cpp"
    break;

  case 316: /* copy_option: FORMAT IDENTIFIER  */
#line 2450 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6180 "parser.cpp"
    break;

  case 316: /* copy_option: FORMAT IDENTIFIER  */
#line 2449 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
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
<<<<<<< HEAD
<<<<<<< HEAD
#line 6202 "parser.cpp"
    break;

  case 317: /* copy_option: DELIMITER STRING  */
#line 2478 "parser.y"
=======
#line 6205 "parser.cpp"
    break;

  case 317: /* copy_option: DELIMITER STRING  */
#line 2469 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6204 "parser.cpp"
    break;

  case 317: /* copy_option: DELIMITER STRING  */
#line 2468 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
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
<<<<<<< HEAD
<<<<<<< HEAD
#line 6217 "parser.cpp"
    break;

  case 318: /* copy_option: HEADER  */
#line 2488 "parser.y"
=======
#line 6220 "parser.cpp"
    break;

  case 318: /* copy_option: HEADER  */
#line 2479 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6219 "parser.cpp"
    break;

  case 318: /* copy_option: HEADER  */
#line 2478 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6227 "parser.cpp"
    break;

  case 319: /* file_path: STRING  */
#line 2494 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6235 "parser.cpp"
    break;

  case 320: /* if_exists: IF EXISTS  */
#line 2498 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6241 "parser.cpp"
    break;

  case 321: /* if_exists: %empty  */
#line 2499 "parser.y"
  { (yyval.bool_value) = false; }
#line 6247 "parser.cpp"
    break;

  case 322: /* if_not_exists: IF NOT EXISTS  */
#line 2501 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6253 "parser.cpp"
    break;

  case 323: /* if_not_exists: %empty  */
#line 2502 "parser.y"
  { (yyval.bool_value) = false; }
#line 6259 "parser.cpp"
    break;

  case 326: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2517 "parser.y"
=======
#line 6230 "parser.cpp"
=======
#line 6229 "parser.cpp"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
    break;

  case 319: /* file_path: STRING  */
#line 2484 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6237 "parser.cpp"
    break;

  case 320: /* if_exists: IF EXISTS  */
#line 2488 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6243 "parser.cpp"
    break;

  case 321: /* if_exists: %empty  */
#line 2489 "parser.y"
  { (yyval.bool_value) = false; }
#line 6249 "parser.cpp"
    break;

  case 322: /* if_not_exists: IF NOT EXISTS  */
#line 2491 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6255 "parser.cpp"
    break;

  case 323: /* if_not_exists: %empty  */
#line 2492 "parser.y"
  { (yyval.bool_value) = false; }
#line 6261 "parser.cpp"
    break;

  case 326: /* if_not_exists_info: if_not_exists IDENTIFIER  */
<<<<<<< HEAD
#line 2508 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 2507 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6272 "parser.cpp"
    break;

  case 327: /* if_not_exists_info: %empty  */
#line 2525 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6280 "parser.cpp"
    break;

  case 328: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2530 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6288 "parser.cpp"
    break;

  case 329: /* with_index_param_list: %empty  */
#line 2533 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6296 "parser.cpp"
    break;

  case 330: /* index_param_list: index_param  */
#line 2537 "parser.y"
=======
#line 6275 "parser.cpp"
=======
#line 6274 "parser.cpp"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
    break;

  case 327: /* if_not_exists_info: %empty  */
#line 2515 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6282 "parser.cpp"
    break;

  case 328: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2520 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6290 "parser.cpp"
    break;

  case 329: /* with_index_param_list: %empty  */
#line 2523 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6298 "parser.cpp"
    break;

  case 330: /* index_param_list: index_param  */
<<<<<<< HEAD
#line 2528 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 2527 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6305 "parser.cpp"
    break;

  case 331: /* index_param_list: index_param_list ',' index_param  */
#line 2541 "parser.y"
=======
#line 6308 "parser.cpp"
    break;

  case 331: /* index_param_list: index_param_list ',' index_param  */
#line 2532 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6307 "parser.cpp"
    break;

  case 331: /* index_param_list: index_param_list ',' index_param  */
#line 2531 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6314 "parser.cpp"
    break;

  case 332: /* index_param: IDENTIFIER  */
#line 2546 "parser.y"
=======
#line 6317 "parser.cpp"
    break;

  case 332: /* index_param: IDENTIFIER  */
#line 2537 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6316 "parser.cpp"
    break;

  case 332: /* index_param: IDENTIFIER  */
#line 2536 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6324 "parser.cpp"
    break;

  case 333: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2551 "parser.y"
=======
#line 6327 "parser.cpp"
    break;

  case 333: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2542 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6326 "parser.cpp"
    break;

  case 333: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2541 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6337 "parser.cpp"
    break;

  case 334: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2559 "parser.y"
=======
#line 6340 "parser.cpp"
    break;

  case 334: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2550 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6339 "parser.cpp"
    break;

  case 334: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2549 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6349 "parser.cpp"
    break;

  case 335: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2566 "parser.y"
=======
#line 6352 "parser.cpp"
    break;

  case 335: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2557 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6351 "parser.cpp"
    break;

  case 335: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2556 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
<<<<<<< HEAD
<<<<<<< HEAD
#line 6361 "parser.cpp"
    break;

  case 336: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2577 "parser.y"
=======
#line 6364 "parser.cpp"
    break;

  case 336: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2568 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6363 "parser.cpp"
    break;

  case 336: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2567 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
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
<<<<<<< HEAD
<<<<<<< HEAD
#line 6414 "parser.cpp"
    break;

  case 337: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2625 "parser.y"
=======
#line 6417 "parser.cpp"
    break;

  case 337: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2616 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6416 "parser.cpp"
    break;

  case 337: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2615 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)
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
<<<<<<< HEAD
<<<<<<< HEAD
#line 6468 "parser.cpp"
    break;


#line 6472 "parser.cpp"
=======
#line 6471 "parser.cpp"
    break;


#line 6475 "parser.cpp"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 6470 "parser.cpp"
    break;


#line 6474 "parser.cpp"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)

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

<<<<<<< HEAD
<<<<<<< HEAD
#line 2675 "parser.y"
=======
#line 2666 "parser.y"
>>>>>>> 13445115 (Add insert embedding syntax.)
=======
#line 2665 "parser.y"
>>>>>>> 3b7827ea (Fix parser.y debug info before pr.)


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

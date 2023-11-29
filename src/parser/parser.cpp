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
  YYSYMBOL_SEARCH = 154,                   /* SEARCH  */
  YYSYMBOL_MATCH = 155,                    /* MATCH  */
  YYSYMBOL_QUERY = 156,                    /* QUERY  */
  YYSYMBOL_FUSION = 157,                   /* FUSION  */
  YYSYMBOL_NUMBER = 158,                   /* NUMBER  */
  YYSYMBOL_159_ = 159,                     /* '='  */
  YYSYMBOL_160_ = 160,                     /* '<'  */
  YYSYMBOL_161_ = 161,                     /* '>'  */
  YYSYMBOL_162_ = 162,                     /* '+'  */
  YYSYMBOL_163_ = 163,                     /* '-'  */
  YYSYMBOL_164_ = 164,                     /* '*'  */
  YYSYMBOL_165_ = 165,                     /* '/'  */
  YYSYMBOL_166_ = 166,                     /* '%'  */
  YYSYMBOL_167_ = 167,                     /* '['  */
  YYSYMBOL_168_ = 168,                     /* ']'  */
  YYSYMBOL_169_ = 169,                     /* '('  */
  YYSYMBOL_170_ = 170,                     /* ')'  */
  YYSYMBOL_171_ = 171,                     /* '.'  */
  YYSYMBOL_172_ = 172,                     /* ';'  */
  YYSYMBOL_173_ = 173,                     /* ','  */
  YYSYMBOL_YYACCEPT = 174,                 /* $accept  */
  YYSYMBOL_input_pattern = 175,            /* input_pattern  */
  YYSYMBOL_statement_list = 176,           /* statement_list  */
  YYSYMBOL_statement = 177,                /* statement  */
  YYSYMBOL_explainable_statement = 178,    /* explainable_statement  */
  YYSYMBOL_create_statement = 179,         /* create_statement  */
  YYSYMBOL_table_element_array = 180,      /* table_element_array  */
  YYSYMBOL_table_element = 181,            /* table_element  */
  YYSYMBOL_table_column = 182,             /* table_column  */
  YYSYMBOL_column_type = 183,              /* column_type  */
  YYSYMBOL_column_constraints = 184,       /* column_constraints  */
  YYSYMBOL_column_constraint = 185,        /* column_constraint  */
  YYSYMBOL_table_constraint = 186,         /* table_constraint  */
  YYSYMBOL_identifier_array = 187,         /* identifier_array  */
  YYSYMBOL_delete_statement = 188,         /* delete_statement  */
  YYSYMBOL_insert_statement = 189,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 190, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 191,        /* explain_statement  */
  YYSYMBOL_explain_type = 192,             /* explain_type  */
  YYSYMBOL_update_statement = 193,         /* update_statement  */
  YYSYMBOL_update_expr_array = 194,        /* update_expr_array  */
  YYSYMBOL_update_expr = 195,              /* update_expr  */
  YYSYMBOL_drop_statement = 196,           /* drop_statement  */
  YYSYMBOL_copy_statement = 197,           /* copy_statement  */
  YYSYMBOL_select_statement = 198,         /* select_statement  */
  YYSYMBOL_select_with_paren = 199,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 200,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 201, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 202, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 203, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 204,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 205,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 206,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 207,            /* order_by_type  */
  YYSYMBOL_limit_expr = 208,               /* limit_expr  */
  YYSYMBOL_offset_expr = 209,              /* offset_expr  */
  YYSYMBOL_distinct = 210,                 /* distinct  */
  YYSYMBOL_from_clause = 211,              /* from_clause  */
  YYSYMBOL_search_clause = 212,            /* search_clause  */
  YYSYMBOL_where_clause = 213,             /* where_clause  */
  YYSYMBOL_having_clause = 214,            /* having_clause  */
  YYSYMBOL_group_by_clause = 215,          /* group_by_clause  */
  YYSYMBOL_set_operator = 216,             /* set_operator  */
  YYSYMBOL_table_reference = 217,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 218,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 219,     /* table_reference_name  */
  YYSYMBOL_table_name = 220,               /* table_name  */
  YYSYMBOL_table_alias = 221,              /* table_alias  */
  YYSYMBOL_with_clause = 222,              /* with_clause  */
  YYSYMBOL_with_expr_list = 223,           /* with_expr_list  */
  YYSYMBOL_with_expr = 224,                /* with_expr  */
  YYSYMBOL_join_clause = 225,              /* join_clause  */
  YYSYMBOL_join_type = 226,                /* join_type  */
  YYSYMBOL_show_statement = 227,           /* show_statement  */
  YYSYMBOL_flush_statement = 228,          /* flush_statement  */
  YYSYMBOL_command_statement = 229,        /* command_statement  */
  YYSYMBOL_expr_array = 230,               /* expr_array  */
  YYSYMBOL_expr_array_list = 231,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 232,               /* expr_alias  */
  YYSYMBOL_expr = 233,                     /* expr  */
  YYSYMBOL_operand = 234,                  /* operand  */
  YYSYMBOL_knn_expr = 235,                 /* knn_expr  */
  YYSYMBOL_match_expr = 236,               /* match_expr  */
  YYSYMBOL_query_expr = 237,               /* query_expr  */
  YYSYMBOL_fusion_expr = 238,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 239,         /* sub_search_array  */
  YYSYMBOL_function_expr = 240,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 241,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 242,             /* between_expr  */
  YYSYMBOL_in_expr = 243,                  /* in_expr  */
  YYSYMBOL_case_expr = 244,                /* case_expr  */
  YYSYMBOL_case_check_array = 245,         /* case_check_array  */
  YYSYMBOL_cast_expr = 246,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 247,            /* subquery_expr  */
  YYSYMBOL_column_expr = 248,              /* column_expr  */
  YYSYMBOL_constant_expr = 249,            /* constant_expr  */
  YYSYMBOL_array_expr = 250,               /* array_expr  */
  YYSYMBOL_long_array_expr = 251,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 252, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 253,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 254, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 255,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 256,         /* copy_option_list  */
  YYSYMBOL_copy_option = 257,              /* copy_option  */
  YYSYMBOL_file_path = 258,                /* file_path  */
  YYSYMBOL_if_exists = 259,                /* if_exists  */
  YYSYMBOL_if_not_exists = 260,            /* if_not_exists  */
  YYSYMBOL_semicolon = 261,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 262,       /* if_not_exists_info  */
  YYSYMBOL_with_index_param_list = 263,    /* with_index_param_list  */
  YYSYMBOL_index_param_list = 264,         /* index_param_list  */
  YYSYMBOL_index_param = 265,              /* index_param  */
  YYSYMBOL_index_info_list = 266           /* index_info_list  */
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
#define YYFINAL  75
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   823

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  174
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  93
/* YYNRULES -- Number of rules.  */
#define YYNRULES  335
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  655

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   413


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
       2,     2,     2,     2,     2,     2,     2,   166,     2,     2,
     169,   170,   164,   162,   173,   163,   171,   165,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   172,
     160,   159,   161,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   167,     2,   168,     2,     2,     2,     2,     2,     2,
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
     155,   156,   157,   158
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
    2315,  2318,  2330,  2333,  2337,  2341,  2346,  2351,  2355,  2360,
    2365,  2371,  2377,  2383,  2389,  2395,  2401,  2407,  2413,  2419,
    2425,  2431,  2442,  2446,  2451,  2470,  2480,  2486,  2490,  2491,
    2493,  2494,  2496,  2497,  2509,  2517,  2522,  2525,  2529,  2533,
    2538,  2543,  2551,  2558,  2569,  2619
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
  "SEARCH", "MATCH", "QUERY", "FUSION", "NUMBER", "'='", "'<'", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'%'", "'['", "']'", "'('", "')'", "'.'",
  "';'", "','", "$accept", "input_pattern", "statement_list", "statement",
  "explainable_statement", "create_statement", "table_element_array",
  "table_element", "table_column", "column_type", "column_constraints",
  "column_constraint", "table_constraint", "identifier_array",
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
  "flush_statement", "command_statement", "expr_array", "expr_array_list",
  "expr_alias", "expr", "operand", "knn_expr", "match_expr", "query_expr",
  "fusion_expr", "sub_search_array", "function_expr", "conjunction_expr",
  "between_expr", "in_expr", "case_expr", "case_check_array", "cast_expr",
  "subquery_expr", "column_expr", "constant_expr", "array_expr",
  "long_array_expr", "unclosed_long_array_expr", "double_array_expr",
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

#define YYPACT_NINF (-322)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-326)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     534,   171,    18,   201,    64,     4,    64,    54,   450,   -23,
      69,    52,   135,   142,   -68,   -37,   153,    -9,  -322,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,   252,  -322,  -322,   151,
    -322,  -322,  -322,   120,   120,   120,   120,    80,    64,   146,
     146,   146,   146,   146,    51,   229,    64,   160,   242,   255,
    -322,  -322,  -322,  -322,  -322,  -322,  -322,   576,  -322,  -322,
    -322,  -322,  -322,    64,    81,  -322,  -322,  -322,  -322,   209,
     111,  -322,   302,   168,   177,  -322,    43,  -322,   301,  -322,
    -322,    14,   291,  -322,   295,   300,   369,    64,    64,    64,
     370,   325,   228,   318,   400,    64,    64,    64,   401,   402,
     403,   337,   407,   407,     9,    42,  -322,  -322,  -322,  -322,
    -322,  -322,  -322,   252,  -322,  -322,  -322,  -322,   415,  -322,
     258,   142,   407,  -322,  -322,  -322,  -322,    14,  -322,  -322,
    -322,   356,   374,   365,   360,  -322,   -38,  -322,   228,  -322,
      64,   431,    12,  -322,  -322,  -322,  -322,  -322,   379,  -322,
     281,   -40,  -322,   356,  -322,  -322,   384,   387,  -322,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,   416,   151,
    -322,  -322,   297,   298,   303,  -322,  -322,   627,   413,   304,
     308,   245,   457,  -322,  -322,   463,   309,   310,   311,   320,
     324,   460,   460,  -322,   275,   -46,  -322,   -32,   536,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,
     323,  -322,  -322,   356,   356,   406,  -322,   -37,    21,   421,
     326,  -322,   -91,   328,  -322,    64,   356,   403,  -322,   115,
     329,   330,   494,   332,  -322,  -322,   137,  -322,  -322,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,   460,
     334,   606,   432,   356,   356,    10,    72,  -322,   627,  -322,
     500,   356,   502,   503,   504,   105,   105,   340,    22,    13,
     356,   361,   511,   356,   356,   -31,   347,    -3,   460,   460,
     460,   460,   460,   460,   460,   460,   460,   460,   460,   460,
     460,   460,    20,   344,  -322,   -34,   115,   356,  -322,   252,
     690,   408,   352,    34,  -322,  -322,  -322,   -37,   431,   354,
    -322,   521,   356,   353,  -322,   115,  -322,   254,   254,  -322,
    -322,   356,  -322,    74,   432,   386,   359,   -15,   -48,   139,
    -322,   356,   356,   459,    17,   362,   113,   363,  -322,  -322,
     -37,   367,   395,  -322,    31,  -322,  -322,    70,   337,  -322,
    -322,   396,   373,   460,   275,   422,  -322,   101,   101,   214,
     214,   571,   101,   101,   214,   214,   105,   105,  -322,  -322,
    -322,  -322,  -322,   356,  -322,  -322,  -322,   115,  -322,  -322,
    -322,  -322,  -322,  -322,  -322,   382,  -322,  -322,  -322,  -322,
     383,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,
    -322,   388,   392,     2,   393,   431,  -322,    21,   252,   169,
     431,  -322,   183,   394,   551,   561,  -322,   185,  -322,   196,
     197,  -322,   397,  -322,   690,   356,  -322,   356,   -36,   -41,
     460,   399,   567,  -322,   568,   569,   -13,    13,   522,  -322,
    -322,  -322,  -322,  -322,  -322,   529,  -322,   581,  -322,  -322,
    -322,  -322,  -322,   418,   546,   275,   101,   424,   211,  -322,
     460,  -322,   589,   591,   203,   233,   482,   485,  -322,  -322,
       2,  -322,   431,   213,  -322,   462,   218,  -322,   356,  -322,
    -322,  -322,   254,  -322,  -322,  -322,   433,   115,   -27,  -322,
     356,   497,   292,   436,  -322,   -97,  -322,   -22,   219,   440,
     441,    31,   395,    13,    13,   443,    70,   565,   570,   448,
     226,  -322,  -322,   606,   451,   240,   447,   452,   453,   454,
     455,   461,   464,   467,   468,   469,   470,   471,   472,   473,
     474,   475,  -322,  -322,  -322,   253,  -322,   632,   490,   272,
    -322,  -322,  -322,   115,  -322,  -322,  -322,   646,  -322,   645,
    -322,   647,  -322,   650,  -322,  -322,  -322,  -322,   598,   431,
    -322,  -322,  -322,  -322,   356,   356,  -322,  -322,  -322,  -322,
    -322,   658,   659,   660,   662,   669,   670,   672,   673,   674,
     675,   676,   677,   679,   680,   681,   683,   684,  -322,   618,
     689,  -322,   520,  -322,  -322,   524,   356,   285,   535,   115,
     540,   541,   543,   544,   545,   547,   548,   549,   550,   552,
     554,   555,   557,   558,   559,   573,   574,   577,  -322,   618,
     712,  -322,   115,  -322,  -322,  -322,  -322,  -322,  -322,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,
    -322,   718,  -322,   590,   579,   286,  -322,  -322,   306,  -322,
     718,  -322,  -322,  -322,  -322
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     167,     0,     0,     0,     0,     0,     0,     0,   103,     0,
       0,     0,     0,     0,     0,   167,     0,   323,     3,     5,
      10,    12,    13,    11,     6,     7,     9,   116,   115,     0,
       8,    14,    15,   321,   321,   321,   321,   321,     0,   319,
     319,   319,   319,   319,   160,     0,     0,     0,     0,     0,
      97,   101,    98,    99,   100,   102,    96,   167,   181,   182,
     180,   183,   184,     0,   185,   190,   191,   192,   193,     0,
     166,   168,     0,     0,     0,     1,   167,     2,   150,   152,
     153,     0,   139,   121,   127,     0,     0,     0,     0,     0,
       0,     0,    94,     0,     0,     0,     0,     0,     0,     0,
       0,   145,     0,     0,     0,     0,    95,    16,    21,    23,
      22,    17,    18,    20,    19,    24,    25,   189,     0,   186,
       0,     0,     0,   120,   119,     4,   151,     0,   117,   118,
     138,     0,     0,   135,     0,    26,     0,    27,    94,   324,
       0,     0,   167,   318,   108,   110,   109,   111,     0,   161,
       0,   145,   105,     0,    90,   317,     0,     0,   197,   199,
     198,   195,   196,   202,   204,   203,   200,   201,   187,     0,
     169,   194,     0,     0,   280,   284,   287,   288,     0,     0,
       0,     0,     0,   285,   286,     0,     0,     0,     0,     0,
       0,     0,     0,   282,   167,   141,   205,   210,   211,   223,
     224,   225,   226,   220,   215,   214,   213,   221,   222,   212,
     219,   218,   291,     0,     0,   137,   320,   167,     0,     0,
       0,    88,     0,     0,    92,     0,     0,     0,   104,   144,
       0,     0,     0,     0,   124,   123,     0,   301,   300,   303,
     302,   305,   304,   307,   306,   309,   308,   311,   310,     0,
       0,   246,   167,     0,     0,     0,     0,   289,     0,   290,
       0,     0,     0,     0,     0,   248,   247,     0,     0,     0,
       0,   143,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   126,   128,   133,   134,     0,   122,    29,
       0,     0,     0,     0,    32,    34,    35,   167,     0,    31,
      93,     0,     0,    91,   112,   107,   106,     0,     0,   188,
     170,     0,   241,     0,   167,     0,     0,     0,     0,     0,
     271,     0,     0,     0,     0,     0,     0,     0,   217,   216,
     167,   140,   154,   156,   165,   157,   206,     0,   145,   209,
     264,   265,     0,     0,   167,     0,   245,   255,   256,   259,
     260,     0,   262,   254,   257,   258,   250,   249,   251,   252,
     253,   281,   283,     0,   131,   132,   130,   136,    38,    41,
      42,    39,    40,    43,    44,    58,    60,    45,    47,    46,
      63,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,     0,     0,    36,     0,     0,    28,     0,    30,     0,
       0,    89,     0,     0,     0,     0,   316,     0,   312,     0,
       0,   242,     0,   276,     0,     0,   269,     0,     0,     0,
       0,     0,     0,   230,     0,     0,     0,     0,     0,   174,
     175,   176,   177,   173,   178,     0,   163,     0,   158,   233,
     234,   235,   236,   142,   149,   167,   263,     0,     0,   244,
       0,   129,     0,     0,     0,     0,     0,     0,    83,    84,
      37,    80,     0,     0,    33,     0,     0,   207,     0,   315,
     314,   114,     0,   113,   243,   277,     0,   273,     0,   272,
       0,     0,     0,     0,   292,     0,   293,     0,     0,     0,
       0,   165,   155,     0,     0,   162,     0,     0,   147,     0,
       0,   278,   267,   266,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    85,    82,    81,     0,    87,     0,     0,     0,
     313,   275,   270,   274,   261,   298,   295,     0,   294,     0,
     297,     0,   228,     0,   231,   232,   159,   171,     0,     0,
     237,   238,   239,   240,     0,     0,   125,   279,   268,    59,
      62,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    86,   327,
       0,   208,     0,   296,   299,     0,     0,     0,   148,   146,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   334,   327,
       0,   229,   172,   164,    61,    67,    68,    65,    66,    69,
      70,    71,    64,    75,    76,    73,    74,    77,    78,    79,
      72,     0,   335,     0,   330,     0,   328,   227,     0,   326,
       0,   331,   333,   332,   329
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -322,  -322,  -322,   685,  -322,   705,  -322,   357,  -322,   339,
    -322,   333,  -322,  -291,   738,   739,   661,  -322,  -322,   741,
    -322,   575,   747,   748,   -55,   791,   -14,   638,   682,   -43,
    -322,  -322,   435,  -322,  -322,  -322,  -322,  -322,  -322,  -145,
    -322,  -322,  -322,  -322,   375,  -295,    -1,   312,  -322,  -322,
     693,  -322,  -322,   753,   754,   758,  -232,  -322,   553,  -153,
    -114,  -321,  -320,  -318,  -317,  -322,  -322,  -322,  -322,  -322,
    -322,   562,  -322,  -322,  -322,  -322,  -322,  -322,  -322,  -322,
    -322,   631,   501,   336,   -29,   262,   299,  -322,  -322,   202,
    -322,   170,  -322
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    16,    17,    18,   106,    19,   303,   304,   305,   403,
     470,   471,   306,   222,    20,    21,   142,    22,    57,    23,
     151,   152,    24,    25,    26,    27,    28,    83,   128,    84,
     133,   293,   294,   376,   215,   298,   131,   271,   348,   154,
     566,   508,    81,   341,   342,   343,   344,   448,    29,    70,
      71,   345,   445,    30,    31,    32,   195,   313,   196,   197,
     198,   199,   200,   201,   202,   453,   203,   204,   205,   206,
     207,   256,   208,   209,   210,   211,   493,   494,   495,   496,
     497,   212,   417,   418,   156,    94,    86,    77,    91,   618,
     645,   646,   309
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     229,    74,   113,    45,   323,    47,   228,    58,    78,    64,
      79,    80,   217,   158,   159,   160,    44,   409,   272,    59,
      60,   153,    82,   371,   300,   269,   449,   450,   255,   451,
     452,   374,   375,   425,   446,   424,    13,    92,   129,   352,
     490,   268,   489,  -322,   223,   101,   163,   164,   165,    38,
       1,   542,     2,     3,     4,     5,     6,     7,     8,     9,
     295,   296,   117,    10,   251,   355,   161,    44,    11,    12,
     466,   548,    44,   315,   157,    46,   549,   265,   266,   310,
     412,   447,   311,    72,   173,    13,   136,   137,   138,   420,
     273,   274,   254,   171,   145,   146,   147,   273,   274,   166,
     327,   328,   273,   274,   273,   274,   273,   274,   334,    63,
     353,   273,   274,   356,   473,   467,    13,   468,   469,   476,
     350,   351,   458,   273,   274,   118,   119,   270,   224,    61,
      62,   218,    15,   227,   301,   251,   302,  -325,    68,   220,
     174,   175,   176,   177,   377,    69,   550,    85,   273,   274,
     330,   551,   331,    75,   332,   273,   274,   501,   162,    82,
     273,   274,   299,    76,   357,   358,   359,   360,   361,   362,
     363,   364,   365,   366,   367,   368,   369,   370,   428,   429,
     267,   535,   340,   127,   372,   560,   561,    85,   562,   563,
     431,   167,   339,    14,    65,    66,    67,   321,    33,    34,
      35,    48,    49,   454,   406,   178,   179,   407,   557,   558,
      36,    37,    15,    93,   180,   187,   181,   426,   277,   427,
     295,   332,    99,   510,   314,   188,   189,   190,    39,    40,
      41,   102,   103,   182,  -326,  -326,   280,   281,   326,   456,
      42,    43,  -326,   100,   421,   104,   539,   270,   174,   175,
     176,   177,   408,   273,   274,   183,   184,   185,   105,   120,
    -326,   285,   286,   287,   288,   289,   290,   291,   597,   289,
     290,   291,   487,    78,   488,    79,    80,   186,   174,   175,
     176,   177,   187,   433,   121,   436,   434,   516,   517,   518,
     519,   520,   188,   189,   190,   521,   522,   545,   546,   191,
     192,   193,    95,    96,    97,    98,   194,   322,   122,   651,
     422,   652,   653,   178,   179,   523,   491,   524,   525,   526,
     527,   528,   180,   126,   181,   529,   530,   254,   414,   415,
     416,   277,   598,    87,    88,    89,    90,   543,   123,   475,
     457,   182,   311,   178,   179,   531,   513,   124,    13,  -326,
    -326,   130,   180,   477,   181,   481,   270,   132,   482,   174,
     175,   176,   177,   183,   184,   185,   483,   484,   134,   482,
     270,   182,   135,   139,  -326,  -326,   287,   288,   289,   290,
     291,   512,   140,   536,   270,   186,   311,   143,   538,   552,
     187,   311,   553,   183,   184,   185,   568,   141,   153,   270,
     188,   189,   190,   144,   148,   149,   150,   191,   192,   193,
     570,   155,   599,   571,   194,   186,   174,   175,   176,   177,
     187,   168,   213,   588,   178,   179,   311,   169,   214,   216,
     188,   189,   190,   180,   221,   181,   225,   191,   192,   193,
     226,   509,   591,   622,   194,   270,   438,  -179,   439,   440,
     441,   442,   182,   443,   444,   623,   649,   230,   311,   650,
     231,   257,   232,   174,   175,   176,   177,   234,   235,   258,
     297,   307,   236,   252,   183,   184,   185,   253,   260,   261,
     262,   249,   250,    50,    51,    52,    53,    54,    55,   263,
     180,    56,   181,   264,   292,   308,   186,   312,   317,   318,
     319,   187,   320,   324,   333,    13,   335,   336,   337,   182,
     338,   188,   189,   190,   349,   347,   354,   373,   191,   192,
     193,   405,   404,   410,   411,   194,   413,   353,   249,   423,
     430,   183,   184,   185,   273,   432,   435,   180,   459,   181,
     437,     1,   455,     2,     3,     4,     5,     6,     7,     8,
       9,   462,   463,   186,    10,   479,   182,   464,   187,    11,
      12,   465,   472,   478,   480,   325,   492,   485,   188,   189,
     190,   498,   499,   500,   503,   191,   192,   193,   183,   184,
     185,   504,   194,     1,   505,     2,     3,     4,     5,     6,
       7,   506,     9,   507,   511,   514,    10,   515,   532,   533,
     186,    11,    12,   541,   275,   187,   276,    13,   537,   547,
     554,   555,   559,   564,   277,   188,   189,   190,   567,   565,
     572,   569,   191,   192,   193,   573,   574,   575,   576,   194,
     278,   279,   280,   281,   577,   589,   590,   578,   283,   325,
     579,   580,   581,   582,   583,   584,   585,   586,   587,    13,
     592,   593,   594,   277,   595,   596,   284,   285,   286,   287,
     288,   289,   290,   291,   600,   601,   602,   544,   603,   278,
     279,   280,   281,   282,   325,   604,   605,   283,   606,   607,
     608,   609,   610,   611,    14,   612,   613,   614,   277,   615,
     616,   617,   619,   620,   621,   284,   285,   286,   287,   288,
     289,   290,   291,    15,   278,   279,   280,   281,   270,   460,
     624,   625,   283,   626,   627,   628,   643,   629,   630,   631,
     632,   644,   633,   277,   634,   635,    14,   636,   637,   638,
     284,   285,   286,   287,   288,   289,   290,   291,   648,   278,
     279,   280,   281,   639,   640,    15,   641,   283,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     647,   125,   107,   486,   474,   284,   285,   286,   287,   288,
     289,   290,   291,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   108,   109,   400,   110,   219,
     401,   402,   316,   534,   111,   112,    73,   233,   461,   172,
     114,   115,   502,   556,   170,   116,   259,   329,   540,   419,
     654,   642,     0,   346
};

static const yytype_int16 yycheck[] =
{
     153,    15,    57,     4,   236,     6,   151,    30,    21,    10,
      23,    24,    50,     4,     5,     6,     3,   308,    50,    42,
      43,    61,     8,     3,     3,    71,   347,   347,   181,   347,
     347,    65,    66,    81,     3,    50,    73,    38,    81,    70,
      81,   194,    78,     0,    32,    46,     4,     5,     6,    31,
       7,    78,     9,    10,    11,    12,    13,    14,    15,    16,
     213,   214,    63,    20,   178,    68,    57,     3,    25,    26,
      68,   168,     3,   226,   103,    71,   173,   191,   192,   170,
     312,    50,   173,   151,   127,    73,    87,    88,    89,   321,
     138,   139,    82,   122,    95,    96,    97,   138,   139,    57,
     253,   254,   138,   139,   138,   139,   138,   139,   261,    40,
     141,   138,   139,   116,   405,   113,    73,   115,   116,   410,
     273,   274,   354,   138,   139,    44,    45,   173,   142,   152,
     153,   169,   169,   173,   113,   249,   115,    57,     3,   140,
       3,     4,     5,     6,   297,     3,   168,    67,   138,   139,
      78,   173,    80,     0,    82,   138,   139,   170,   149,     8,
     138,   139,   217,   172,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   331,   332,
     194,   472,   169,   169,   164,   506,   506,    67,   506,   506,
     173,   149,   170,   150,   142,   143,   144,    60,    27,    28,
      29,   147,   148,   348,   170,    68,    69,   173,   503,   504,
      39,    40,   169,    67,    77,   145,    79,    78,   117,    80,
     373,    82,   171,   455,   225,   155,   156,   157,    27,    28,
      29,    71,    72,    96,   133,   134,   135,   136,   252,   353,
      39,    40,   141,    14,   170,     3,   478,   173,     3,     4,
       5,     6,   307,   138,   139,   118,   119,   120,     3,    50,
     159,   160,   161,   162,   163,   164,   165,   166,   559,   164,
     165,   166,   425,    21,   427,    23,    24,   140,     3,     4,
       5,     6,   145,   170,   173,   340,   173,    84,    85,    86,
      87,    88,   155,   156,   157,    92,    93,     5,     6,   162,
     163,   164,    40,    41,    42,    43,   169,   170,     6,     3,
     324,     5,     6,    68,    69,   112,   430,    84,    85,    86,
      87,    88,    77,    22,    79,    92,    93,    82,    74,    75,
      76,   117,   564,    34,    35,    36,    37,   490,   170,   170,
     354,    96,   173,    68,    69,   112,   460,   170,    73,   135,
     136,    60,    77,   170,    79,   170,   173,    62,   173,     3,
       4,     5,     6,   118,   119,   120,   170,   170,    68,   173,
     173,    96,     3,     3,   160,   161,   162,   163,   164,   165,
     166,   170,    57,   170,   173,   140,   173,    69,   170,   170,
     145,   173,   173,   118,   119,   120,   170,   169,    61,   173,
     155,   156,   157,     3,     3,     3,     3,   162,   163,   164,
     170,     4,   565,   173,   169,   140,     3,     4,     5,     6,
     145,     6,    48,   170,    68,    69,   173,   169,    63,    69,
     155,   156,   157,    77,     3,    79,    57,   162,   163,   164,
     159,   455,   170,   596,   169,   173,    51,    52,    53,    54,
      55,    56,    96,    58,    59,   170,   170,    73,   173,   173,
      73,     4,    46,     3,     4,     5,     6,   170,   170,     6,
      64,    50,   169,   169,   118,   119,   120,   169,   169,   169,
     169,    68,    69,    33,    34,    35,    36,    37,    38,   169,
      77,    41,    79,   169,   171,   169,   140,   169,   169,   169,
       6,   145,   170,   169,     4,    73,     4,     4,     4,    96,
     170,   155,   156,   157,     3,   154,   169,   173,   162,   163,
     164,   169,   114,   169,     3,   169,   173,   141,    68,   170,
      71,   118,   119,   120,   138,   173,   173,    77,   116,    79,
     173,     7,   169,     9,    10,    11,    12,    13,    14,    15,
      16,   169,   169,   140,    20,     4,    96,   169,   145,    25,
      26,   169,   169,   169,     3,    68,   167,   170,   155,   156,
     157,     4,     4,     4,    52,   162,   163,   164,   118,   119,
     120,    52,   169,     7,     3,     9,    10,    11,    12,    13,
      14,   173,    16,    47,   170,     6,    20,     6,   116,   114,
     140,    25,    26,   170,    68,   145,    70,    73,   146,   173,
     170,   170,   169,    48,   117,   155,   156,   157,   170,    49,
     173,   170,   162,   163,   164,   173,   173,   173,   173,   169,
     133,   134,   135,   136,   173,     3,   146,   173,   141,    68,
     173,   173,   173,   173,   173,   173,   173,   173,   173,    73,
       4,     6,     5,   117,     4,    57,   159,   160,   161,   162,
     163,   164,   165,   166,     6,     6,     6,   170,     6,   133,
     134,   135,   136,   137,    68,     6,     6,   141,     6,     6,
       6,     6,     6,     6,   150,     6,     6,     6,   117,     6,
       6,    73,     3,   173,   170,   159,   160,   161,   162,   163,
     164,   165,   166,   169,   133,   134,   135,   136,   173,   138,
     170,   170,   141,   170,   170,   170,     4,   170,   170,   170,
     170,     3,   170,   117,   170,   170,   150,   170,   170,   170,
     159,   160,   161,   162,   163,   164,   165,   166,   159,   133,
     134,   135,   136,   170,   170,   169,   169,   141,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     170,    76,    57,   424,   407,   159,   160,   161,   162,   163,
     164,   165,   166,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,    57,    57,   107,    57,   138,
     110,   111,   227,   470,    57,    57,    15,   169,   373,   127,
      57,    57,   437,   501,   121,    57,   185,   255,   482,   318,
     650,   619,    -1,   270
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
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
     157,   162,   163,   164,   169,   230,   232,   233,   234,   235,
     236,   237,   238,   240,   241,   242,   243,   244,   246,   247,
     248,   249,   255,    48,    63,   208,    69,    50,   169,   190,
     220,     3,   187,    32,   200,    57,   159,   173,   213,   233,
      73,    73,    46,   201,   170,   170,   169,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,    68,
      69,   234,   169,   169,    82,   233,   245,     4,     6,   255,
     169,   169,   169,   169,   169,   234,   234,   200,   233,    71,
     173,   211,    50,   138,   139,    68,    70,   117,   133,   134,
     135,   136,   137,   141,   159,   160,   161,   162,   163,   164,
     165,   166,   171,   205,   206,   233,   233,    64,   209,   198,
       3,   113,   115,   180,   181,   182,   186,    50,   169,   266,
     170,   173,   169,   231,   220,   233,   195,   169,   169,     6,
     170,    60,   170,   230,   169,    68,   200,   233,   233,   245,
      78,    80,    82,     4,   233,     4,     4,     4,   170,   170,
     169,   217,   218,   219,   220,   225,   232,   154,   212,     3,
     233,   233,    70,   141,   169,    68,   116,   234,   234,   234,
     234,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     234,     3,   164,   173,    65,    66,   207,   233,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     107,   110,   111,   183,   114,   169,   170,   173,   198,   187,
     169,     3,   230,   173,    74,    75,    76,   256,   257,   256,
     230,   170,   200,   170,    50,    81,    78,    80,   233,   233,
      71,   173,   173,   170,   173,   173,   198,   173,    51,    53,
      54,    55,    56,    58,    59,   226,     3,    50,   221,   235,
     236,   237,   238,   239,   213,   169,   234,   200,   230,   116,
     138,   206,   169,   169,   169,   169,    68,   113,   115,   116,
     184,   185,   169,   187,   181,   170,   187,   170,   169,     4,
       3,   170,   173,   170,   170,   170,   183,   233,   233,    78,
      81,   234,   167,   250,   251,   252,   253,   254,     4,     4,
       4,   170,   218,    52,    52,     3,   173,    47,   215,   200,
     230,   170,   170,   234,     6,     6,    84,    85,    86,    87,
      88,    92,    93,   112,    84,    85,    86,    87,    88,    92,
      93,   112,   116,   114,   185,   187,   170,   146,   170,   230,
     257,   170,    78,   233,   170,     5,     6,   173,   168,   173,
     168,   173,   170,   173,   170,   170,   221,   219,   219,   169,
     235,   236,   237,   238,    48,    49,   214,   170,   170,   170,
     170,   173,   173,   173,   173,   173,   173,   173,   173,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   170,     3,
     146,   170,     4,     6,     5,     4,    57,   187,   230,   233,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,    73,   263,     3,
     173,   170,   233,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   169,   263,     4,     3,   264,   265,   170,   159,   170,
     173,     3,     5,     6,   265
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   174,   175,   176,   176,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   179,   179,   179,   179,
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
     249,   249,   250,   250,   251,   252,   252,   253,   254,   254,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   255,   256,   256,   257,   257,   257,   258,   259,   259,
     260,   260,   261,   261,   262,   262,   263,   263,   264,   264,
     265,   265,   265,   265,   266,   266
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
       2,     1,     1,     1,     2,     2,     3,     2,     2,     3,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     3,     2,     2,     1,     1,     2,     0,
       3,     0,     1,     0,     2,     0,     4,     0,     1,     3,
       1,     3,     3,     3,     6,     7
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
#line 1970 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 296 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1978 "parser.cpp"
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
#line 1992 "parser.cpp"
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
#line 2006 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 289 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2017 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2026 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 300 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2035 "parser.cpp"
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
#line 2049 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 253 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2060 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2070 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2080 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2090 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2100 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2110 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 335 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2120 "parser.cpp"
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
#line 2134 "parser.cpp"
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
#line 2148 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 323 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2158 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2166 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2174 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2183 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2191 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2199 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2207 "parser.cpp"
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
#line 2221 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2230 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2239 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2248 "parser.cpp"
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
#line 2261 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 313 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2270 "parser.cpp"
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
#line 2284 "parser.cpp"
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
#line 2298 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2308 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 318 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2317 "parser.cpp"
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
#line 2331 "parser.cpp"
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
#line 2348 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2356 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2364 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2372 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2380 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2388 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2396 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2404 "parser.cpp"
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
#line 2418 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2426 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2434 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2442 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2450 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2458 "parser.cpp"
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
#line 2471 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2479 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2487 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 305 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2495 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2503 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2511 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2519 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2527 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2535 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2543 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 309 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2551 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 296 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2559 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 193 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2570 "parser.cpp"
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
#line 2584 "parser.cpp"
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
#line 2598 "parser.cpp"
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

#line 2706 "parser.cpp"

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
#line 2921 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 467 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2932 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 473 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2943 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 480 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2949 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 481 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2955 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 482 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2961 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 483 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2967 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 484 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2973 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 485 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2979 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 486 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2985 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 487 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2991 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 488 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2997 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 489 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3003 "parser.cpp"
    break;

  case 15: /* statement: command_statement  */
#line 490 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3009 "parser.cpp"
    break;

  case 16: /* explainable_statement: create_statement  */
#line 492 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 3015 "parser.cpp"
    break;

  case 17: /* explainable_statement: drop_statement  */
#line 493 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3021 "parser.cpp"
    break;

  case 18: /* explainable_statement: copy_statement  */
#line 494 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3027 "parser.cpp"
    break;

  case 19: /* explainable_statement: show_statement  */
#line 495 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3033 "parser.cpp"
    break;

  case 20: /* explainable_statement: select_statement  */
#line 496 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3039 "parser.cpp"
    break;

  case 21: /* explainable_statement: delete_statement  */
#line 497 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3045 "parser.cpp"
    break;

  case 22: /* explainable_statement: update_statement  */
#line 498 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3051 "parser.cpp"
    break;

  case 23: /* explainable_statement: insert_statement  */
#line 499 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3057 "parser.cpp"
    break;

  case 24: /* explainable_statement: flush_statement  */
#line 500 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3063 "parser.cpp"
    break;

  case 25: /* explainable_statement: command_statement  */
#line 501 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3069 "parser.cpp"
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
#line 3089 "parser.cpp"
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
#line 3107 "parser.cpp"
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
#line 3135 "parser.cpp"
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
#line 3155 "parser.cpp"
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
#line 3176 "parser.cpp"
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
#line 3209 "parser.cpp"
    break;

  case 32: /* table_element_array: table_element  */
#line 627 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3218 "parser.cpp"
    break;

  case 33: /* table_element_array: table_element_array ',' table_element  */
#line 631 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3227 "parser.cpp"
    break;

  case 34: /* table_element: table_column  */
#line 637 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3235 "parser.cpp"
    break;

  case 35: /* table_element: table_constraint  */
#line 640 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3243 "parser.cpp"
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
#line 3283 "parser.cpp"
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
#line 3320 "parser.cpp"
    break;

  case 38: /* column_type: BOOLEAN  */
#line 715 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3326 "parser.cpp"
    break;

  case 39: /* column_type: TINYINT  */
#line 716 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3332 "parser.cpp"
    break;

  case 40: /* column_type: SMALLINT  */
#line 717 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3338 "parser.cpp"
    break;

  case 41: /* column_type: INTEGER  */
#line 718 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3344 "parser.cpp"
    break;

  case 42: /* column_type: INT  */
#line 719 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3350 "parser.cpp"
    break;

  case 43: /* column_type: BIGINT  */
#line 720 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3356 "parser.cpp"
    break;

  case 44: /* column_type: HUGEINT  */
#line 721 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3362 "parser.cpp"
    break;

  case 45: /* column_type: FLOAT  */
#line 722 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3368 "parser.cpp"
    break;

  case 46: /* column_type: REAL  */
#line 723 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3374 "parser.cpp"
    break;

  case 47: /* column_type: DOUBLE  */
#line 724 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3380 "parser.cpp"
    break;

  case 48: /* column_type: DATE  */
#line 725 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3386 "parser.cpp"
    break;

  case 49: /* column_type: TIME  */
#line 726 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3392 "parser.cpp"
    break;

  case 50: /* column_type: DATETIME  */
#line 727 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3398 "parser.cpp"
    break;

  case 51: /* column_type: TIMESTAMP  */
#line 728 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3404 "parser.cpp"
    break;

  case 52: /* column_type: UUID  */
#line 729 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3410 "parser.cpp"
    break;

  case 53: /* column_type: POINT  */
#line 730 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3416 "parser.cpp"
    break;

  case 54: /* column_type: LINE  */
#line 731 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3422 "parser.cpp"
    break;

  case 55: /* column_type: LSEG  */
#line 732 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3428 "parser.cpp"
    break;

  case 56: /* column_type: BOX  */
#line 733 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3434 "parser.cpp"
    break;

  case 57: /* column_type: CIRCLE  */
#line 736 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3440 "parser.cpp"
    break;

  case 58: /* column_type: CHAR  */
#line 738 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3446 "parser.cpp"
    break;

  case 59: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 739 "parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3452 "parser.cpp"
    break;

  case 60: /* column_type: VARCHAR  */
#line 740 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3458 "parser.cpp"
    break;

  case 61: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 741 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3464 "parser.cpp"
    break;

  case 62: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 742 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3470 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL  */
#line 743 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3476 "parser.cpp"
    break;

  case 64: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 746 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3482 "parser.cpp"
    break;

  case 65: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 747 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3488 "parser.cpp"
    break;

  case 66: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 748 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3494 "parser.cpp"
    break;

  case 67: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 749 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3500 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 750 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3506 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 751 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3512 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 752 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3518 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 753 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3524 "parser.cpp"
    break;

  case 72: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 754 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3530 "parser.cpp"
    break;

  case 73: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 755 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3536 "parser.cpp"
    break;

  case 74: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 756 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3542 "parser.cpp"
    break;

  case 75: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 757 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3548 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 758 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3554 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 759 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3560 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 760 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3566 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 761 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3572 "parser.cpp"
    break;

  case 80: /* column_constraints: column_constraint  */
#line 780 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3581 "parser.cpp"
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
#line 3595 "parser.cpp"
    break;

  case 82: /* column_constraint: PRIMARY KEY  */
#line 794 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3603 "parser.cpp"
    break;

  case 83: /* column_constraint: UNIQUE  */
#line 797 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3611 "parser.cpp"
    break;

  case 84: /* column_constraint: NULLABLE  */
#line 800 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3619 "parser.cpp"
    break;

  case 85: /* column_constraint: NOT NULLABLE  */
#line 803 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3627 "parser.cpp"
    break;

  case 86: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 807 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3637 "parser.cpp"
    break;

  case 87: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 812 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3647 "parser.cpp"
    break;

  case 88: /* identifier_array: IDENTIFIER  */
#line 819 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3658 "parser.cpp"
    break;

  case 89: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 825 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3669 "parser.cpp"
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
#line 3686 "parser.cpp"
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
#line 3703 "parser.cpp"
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
#line 3720 "parser.cpp"
    break;

  case 93: /* optional_identifier_array: '(' identifier_array ')'  */
#line 876 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3728 "parser.cpp"
    break;

  case 94: /* optional_identifier_array: %empty  */
#line 879 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3736 "parser.cpp"
    break;

  case 95: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 886 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3746 "parser.cpp"
    break;

  case 96: /* explain_type: ANALYZE  */
#line 892 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3754 "parser.cpp"
    break;

  case 97: /* explain_type: AST  */
#line 895 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3762 "parser.cpp"
    break;

  case 98: /* explain_type: RAW  */
#line 898 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3770 "parser.cpp"
    break;

  case 99: /* explain_type: LOGICAL  */
#line 901 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3778 "parser.cpp"
    break;

  case 100: /* explain_type: PHYSICAL  */
#line 904 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3786 "parser.cpp"
    break;

  case 101: /* explain_type: PIPELINE  */
#line 907 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3794 "parser.cpp"
    break;

  case 102: /* explain_type: FRAGMENT  */
#line 910 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3802 "parser.cpp"
    break;

  case 103: /* explain_type: %empty  */
#line 913 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3810 "parser.cpp"
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
#line 3827 "parser.cpp"
    break;

  case 105: /* update_expr_array: update_expr  */
#line 933 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3836 "parser.cpp"
    break;

  case 106: /* update_expr_array: update_expr_array ',' update_expr  */
#line 937 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3845 "parser.cpp"
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
#line 3857 "parser.cpp"
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
#line 3873 "parser.cpp"
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
#line 3891 "parser.cpp"
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
#line 3909 "parser.cpp"
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
#line 3927 "parser.cpp"
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
#line 3950 "parser.cpp"
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
#line 3996 "parser.cpp"
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
#line 4042 "parser.cpp"
    break;

  case 115: /* select_statement: select_without_paren  */
#line 1122 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4050 "parser.cpp"
    break;

  case 116: /* select_statement: select_with_paren  */
#line 1125 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4058 "parser.cpp"
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
#line 4072 "parser.cpp"
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
#line 4086 "parser.cpp"
    break;

  case 119: /* select_with_paren: '(' select_without_paren ')'  */
#line 1147 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4094 "parser.cpp"
    break;

  case 120: /* select_with_paren: '(' select_with_paren ')'  */
#line 1150 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4102 "parser.cpp"
    break;

  case 121: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1154 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4111 "parser.cpp"
    break;

  case 122: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1159 "parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 4122 "parser.cpp"
    break;

  case 123: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1166 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4130 "parser.cpp"
    break;

  case 124: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1169 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4138 "parser.cpp"
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
#line 4158 "parser.cpp"
    break;

  case 126: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1190 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4166 "parser.cpp"
    break;

  case 127: /* order_by_clause: %empty  */
#line 1193 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4174 "parser.cpp"
    break;

  case 128: /* order_by_expr_list: order_by_expr  */
#line 1197 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4183 "parser.cpp"
    break;

  case 129: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1201 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4192 "parser.cpp"
    break;

  case 130: /* order_by_expr: expr order_by_type  */
#line 1206 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4202 "parser.cpp"
    break;

  case 131: /* order_by_type: ASC  */
#line 1212 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4210 "parser.cpp"
    break;

  case 132: /* order_by_type: DESC  */
#line 1215 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4218 "parser.cpp"
    break;

  case 133: /* order_by_type: %empty  */
#line 1218 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4226 "parser.cpp"
    break;

  case 134: /* limit_expr: LIMIT expr  */
#line 1222 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4234 "parser.cpp"
    break;

  case 135: /* limit_expr: %empty  */
#line 1226 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4240 "parser.cpp"
    break;

  case 136: /* offset_expr: OFFSET expr  */
#line 1228 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4248 "parser.cpp"
    break;

  case 137: /* offset_expr: %empty  */
#line 1232 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4254 "parser.cpp"
    break;

  case 138: /* distinct: DISTINCT  */
#line 1234 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4262 "parser.cpp"
    break;

  case 139: /* distinct: %empty  */
#line 1237 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4270 "parser.cpp"
    break;

  case 140: /* from_clause: FROM table_reference  */
#line 1241 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4278 "parser.cpp"
    break;

  case 141: /* from_clause: %empty  */
#line 1244 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4286 "parser.cpp"
    break;

  case 142: /* search_clause: SEARCH sub_search_array  */
#line 1248 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4296 "parser.cpp"
    break;

  case 143: /* search_clause: %empty  */
#line 1253 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4304 "parser.cpp"
    break;

  case 144: /* where_clause: WHERE expr  */
#line 1257 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4312 "parser.cpp"
    break;

  case 145: /* where_clause: %empty  */
#line 1260 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4320 "parser.cpp"
    break;

  case 146: /* having_clause: HAVING expr  */
#line 1264 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4328 "parser.cpp"
    break;

  case 147: /* having_clause: %empty  */
#line 1267 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4336 "parser.cpp"
    break;

  case 148: /* group_by_clause: GROUP BY expr_array  */
#line 1271 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4344 "parser.cpp"
    break;

  case 149: /* group_by_clause: %empty  */
#line 1274 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4352 "parser.cpp"
    break;

  case 150: /* set_operator: UNION  */
#line 1278 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4360 "parser.cpp"
    break;

  case 151: /* set_operator: UNION ALL  */
#line 1281 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4368 "parser.cpp"
    break;

  case 152: /* set_operator: INTERSECT  */
#line 1284 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4376 "parser.cpp"
    break;

  case 153: /* set_operator: EXCEPT  */
#line 1287 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4384 "parser.cpp"
    break;

  case 154: /* table_reference: table_reference_unit  */
#line 1295 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4392 "parser.cpp"
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
#line 4410 "parser.cpp"
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
#line 4428 "parser.cpp"
    break;

  case 159: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1329 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4439 "parser.cpp"
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
#line 4451 "parser.cpp"
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
#line 4465 "parser.cpp"
    break;

  case 162: /* table_alias: AS IDENTIFIER  */
#line 1356 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4475 "parser.cpp"
    break;

  case 163: /* table_alias: IDENTIFIER  */
#line 1361 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4485 "parser.cpp"
    break;

  case 164: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1366 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4496 "parser.cpp"
    break;

  case 165: /* table_alias: %empty  */
#line 1372 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4504 "parser.cpp"
    break;

  case 166: /* with_clause: WITH with_expr_list  */
#line 1379 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4512 "parser.cpp"
    break;

  case 167: /* with_clause: %empty  */
#line 1382 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4520 "parser.cpp"
    break;

  case 168: /* with_expr_list: with_expr  */
#line 1386 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4529 "parser.cpp"
    break;

  case 169: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1389 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4538 "parser.cpp"
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
#line 4550 "parser.cpp"
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
#line 4562 "parser.cpp"
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
#line 4575 "parser.cpp"
    break;

  case 173: /* join_type: INNER  */
#line 1427 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4583 "parser.cpp"
    break;

  case 174: /* join_type: LEFT  */
#line 1430 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4591 "parser.cpp"
    break;

  case 175: /* join_type: RIGHT  */
#line 1433 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4599 "parser.cpp"
    break;

  case 176: /* join_type: OUTER  */
#line 1436 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4607 "parser.cpp"
    break;

  case 177: /* join_type: FULL  */
#line 1439 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4615 "parser.cpp"
    break;

  case 178: /* join_type: CROSS  */
#line 1442 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4623 "parser.cpp"
    break;

  case 179: /* join_type: %empty  */
#line 1445 "parser.y"
                {
}
#line 4630 "parser.cpp"
    break;

  case 180: /* show_statement: SHOW DATABASES  */
#line 1451 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4639 "parser.cpp"
    break;

  case 181: /* show_statement: SHOW TABLES  */
#line 1455 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4648 "parser.cpp"
    break;

  case 182: /* show_statement: SHOW VIEWS  */
#line 1459 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4657 "parser.cpp"
    break;

  case 183: /* show_statement: SHOW CONFIGS  */
#line 1463 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4666 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW PROFILES  */
#line 1467 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4675 "parser.cpp"
    break;

  case 185: /* show_statement: DESCRIBE table_name  */
#line 1471 "parser.y"
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
#line 4691 "parser.cpp"
    break;

  case 186: /* show_statement: DESCRIBE table_name SEGMENTS  */
#line 1482 "parser.y"
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
#line 4707 "parser.cpp"
    break;

  case 187: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE  */
#line 1493 "parser.y"
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
#line 4724 "parser.cpp"
    break;

  case 188: /* show_statement: DESCRIBE table_name SEGMENT LONG_VALUE BLOCK LONG_VALUE  */
#line 1505 "parser.y"
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
#line 4742 "parser.cpp"
    break;

  case 189: /* show_statement: DESCRIBE INDEX table_name  */
#line 1518 "parser.y"
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
#line 4758 "parser.cpp"
    break;

  case 190: /* flush_statement: FLUSH DATA  */
#line 1533 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4767 "parser.cpp"
    break;

  case 191: /* flush_statement: FLUSH LOG  */
#line 1537 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4776 "parser.cpp"
    break;

  case 192: /* flush_statement: FLUSH BUFFER  */
#line 1541 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4785 "parser.cpp"
    break;

  case 193: /* command_statement: USE IDENTIFIER  */
#line 1549 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4796 "parser.cpp"
    break;

  case 194: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1555 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4806 "parser.cpp"
    break;

  case 195: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1560 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4817 "parser.cpp"
    break;

  case 196: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1566 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4828 "parser.cpp"
    break;

  case 197: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1572 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4841 "parser.cpp"
    break;

  case 198: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1580 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4852 "parser.cpp"
    break;

  case 199: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1586 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4863 "parser.cpp"
    break;

  case 200: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1592 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4874 "parser.cpp"
    break;

  case 201: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1598 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4885 "parser.cpp"
    break;

  case 202: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1604 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4898 "parser.cpp"
    break;

  case 203: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1612 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4909 "parser.cpp"
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1618 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4920 "parser.cpp"
    break;

  case 205: /* expr_array: expr_alias  */
#line 1629 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4929 "parser.cpp"
    break;

  case 206: /* expr_array: expr_array ',' expr_alias  */
#line 1633 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4938 "parser.cpp"
    break;

  case 207: /* expr_array_list: '(' expr_array ')'  */
#line 1638 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 4947 "parser.cpp"
    break;

  case 208: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1642 "parser.y"
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
#line 4967 "parser.cpp"
    break;

  case 209: /* expr_alias: expr AS IDENTIFIER  */
#line 1669 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4978 "parser.cpp"
    break;

  case 210: /* expr_alias: expr  */
#line 1675 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4986 "parser.cpp"
    break;

  case 216: /* operand: '(' expr ')'  */
#line 1685 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4994 "parser.cpp"
    break;

  case 217: /* operand: '(' select_without_paren ')'  */
#line 1688 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5005 "parser.cpp"
    break;

  case 218: /* operand: constant_expr  */
#line 1694 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 5013 "parser.cpp"
    break;

  case 227: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ')'  */
#line 1706 "parser.y"
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
#line 5158 "parser.cpp"
    break;

  case 228: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1847 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = *(yyvsp[-3].str_value);
    match_expr->matching_text_ = *(yyvsp[-1].str_value);
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5171 "parser.cpp"
    break;

  case 229: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1855 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->fields_ = *(yyvsp[-5].str_value);
    match_expr->matching_text_ = *(yyvsp[-3].str_value);
    match_expr->options_text_ = *(yyvsp[-1].str_value);
    free((yyvsp[-5].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5186 "parser.cpp"
    break;

  case 230: /* query_expr: QUERY '(' STRING ')'  */
#line 1866 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = *(yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5197 "parser.cpp"
    break;

  case 231: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1872 "parser.y"
                                  {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->matching_text_ = *(yyvsp[-3].str_value);
    match_expr->options_text_ = *(yyvsp[-1].str_value);
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = match_expr;
}
#line 5210 "parser.cpp"
    break;

  case 232: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1881 "parser.y"
                                               {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::string((yyvsp[-3].str_value));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5223 "parser.cpp"
    break;

  case 233: /* sub_search_array: knn_expr  */
#line 1891 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5232 "parser.cpp"
    break;

  case 234: /* sub_search_array: match_expr  */
#line 1895 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5241 "parser.cpp"
    break;

  case 235: /* sub_search_array: query_expr  */
#line 1899 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5250 "parser.cpp"
    break;

  case 236: /* sub_search_array: fusion_expr  */
#line 1903 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5259 "parser.cpp"
    break;

  case 237: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 1907 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5268 "parser.cpp"
    break;

  case 238: /* sub_search_array: sub_search_array ',' match_expr  */
#line 1911 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5277 "parser.cpp"
    break;

  case 239: /* sub_search_array: sub_search_array ',' query_expr  */
#line 1915 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5286 "parser.cpp"
    break;

  case 240: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 1919 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5295 "parser.cpp"
    break;

  case 241: /* function_expr: IDENTIFIER '(' ')'  */
#line 1924 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5308 "parser.cpp"
    break;

  case 242: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1932 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5321 "parser.cpp"
    break;

  case 243: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1940 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5335 "parser.cpp"
    break;

  case 244: /* function_expr: operand IS NOT NULLABLE  */
#line 1949 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5347 "parser.cpp"
    break;

  case 245: /* function_expr: operand IS NULLABLE  */
#line 1956 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5359 "parser.cpp"
    break;

  case 246: /* function_expr: NOT operand  */
#line 1963 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5371 "parser.cpp"
    break;

  case 247: /* function_expr: '-' operand  */
#line 1970 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5383 "parser.cpp"
    break;

  case 248: /* function_expr: '+' operand  */
#line 1977 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5395 "parser.cpp"
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
#line 5408 "parser.cpp"
    break;

  case 250: /* function_expr: operand '+' operand  */
#line 1992 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5421 "parser.cpp"
    break;

  case 251: /* function_expr: operand '*' operand  */
#line 2000 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5434 "parser.cpp"
    break;

  case 252: /* function_expr: operand '/' operand  */
#line 2008 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5447 "parser.cpp"
    break;

  case 253: /* function_expr: operand '%' operand  */
#line 2016 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5460 "parser.cpp"
    break;

  case 254: /* function_expr: operand '=' operand  */
#line 2024 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5473 "parser.cpp"
    break;

  case 255: /* function_expr: operand EQUAL operand  */
#line 2032 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5486 "parser.cpp"
    break;

  case 256: /* function_expr: operand NOT_EQ operand  */
#line 2040 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5499 "parser.cpp"
    break;

  case 257: /* function_expr: operand '<' operand  */
#line 2048 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5512 "parser.cpp"
    break;

  case 258: /* function_expr: operand '>' operand  */
#line 2056 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5525 "parser.cpp"
    break;

  case 259: /* function_expr: operand LESS_EQ operand  */
#line 2064 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5538 "parser.cpp"
    break;

  case 260: /* function_expr: operand GREATER_EQ operand  */
#line 2072 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5551 "parser.cpp"
    break;

  case 261: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2080 "parser.y"
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
#line 5586 "parser.cpp"
    break;

  case 262: /* function_expr: operand LIKE operand  */
#line 2110 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5599 "parser.cpp"
    break;

  case 263: /* function_expr: operand NOT LIKE operand  */
#line 2118 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5612 "parser.cpp"
    break;

  case 264: /* conjunction_expr: expr AND expr  */
#line 2127 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5625 "parser.cpp"
    break;

  case 265: /* conjunction_expr: expr OR expr  */
#line 2135 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5638 "parser.cpp"
    break;

  case 266: /* between_expr: operand BETWEEN operand AND operand  */
#line 2144 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5650 "parser.cpp"
    break;

  case 267: /* in_expr: operand IN '(' expr_array ')'  */
#line 2152 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5661 "parser.cpp"
    break;

  case 268: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2158 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5672 "parser.cpp"
    break;

  case 269: /* case_expr: CASE expr case_check_array END  */
#line 2165 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5683 "parser.cpp"
    break;

  case 270: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2171 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5695 "parser.cpp"
    break;

  case 271: /* case_expr: CASE case_check_array END  */
#line 2178 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5705 "parser.cpp"
    break;

  case 272: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2183 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5716 "parser.cpp"
    break;

  case 273: /* case_check_array: WHEN expr THEN expr  */
#line 2190 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5728 "parser.cpp"
    break;

  case 274: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2197 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5740 "parser.cpp"
    break;

  case 275: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2205 "parser.y"
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
#line 5768 "parser.cpp"
    break;

  case 276: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2229 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5779 "parser.cpp"
    break;

  case 277: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2235 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5790 "parser.cpp"
    break;

  case 278: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2241 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5802 "parser.cpp"
    break;

  case 279: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2248 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5814 "parser.cpp"
    break;

  case 280: /* column_expr: IDENTIFIER  */
#line 2256 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5826 "parser.cpp"
    break;

  case 281: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2263 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5838 "parser.cpp"
    break;

  case 282: /* column_expr: '*'  */
#line 2270 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5848 "parser.cpp"
    break;

  case 283: /* column_expr: column_expr '.' '*'  */
#line 2275 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5862 "parser.cpp"
    break;

  case 284: /* constant_expr: STRING  */
#line 2285 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5872 "parser.cpp"
    break;

  case 285: /* constant_expr: TRUE  */
#line 2290 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 5882 "parser.cpp"
    break;

  case 286: /* constant_expr: FALSE  */
#line 2295 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 5892 "parser.cpp"
    break;

  case 287: /* constant_expr: DOUBLE_VALUE  */
#line 2300 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5902 "parser.cpp"
    break;

  case 288: /* constant_expr: LONG_VALUE  */
#line 2305 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5912 "parser.cpp"
    break;

  case 289: /* constant_expr: DATE STRING  */
#line 2310 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5922 "parser.cpp"
    break;

  case 290: /* constant_expr: INTERVAL interval_expr  */
#line 2315 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5930 "parser.cpp"
    break;

  case 291: /* constant_expr: interval_expr  */
#line 2318 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5938 "parser.cpp"
    break;

  case 292: /* array_expr: long_array_expr  */
#line 2330 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5946 "parser.cpp"
    break;

  case 293: /* array_expr: double_array_expr  */
#line 2333 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5954 "parser.cpp"
    break;

  case 294: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2337 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5962 "parser.cpp"
    break;

  case 295: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2341 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5972 "parser.cpp"
    break;

  case 296: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2346 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 5981 "parser.cpp"
    break;

  case 297: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2351 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5989 "parser.cpp"
    break;

  case 298: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2355 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5999 "parser.cpp"
    break;

  case 299: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2360 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 6008 "parser.cpp"
    break;

  case 300: /* interval_expr: LONG_VALUE SECONDS  */
#line 2365 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6019 "parser.cpp"
    break;

  case 301: /* interval_expr: LONG_VALUE SECOND  */
#line 2371 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6030 "parser.cpp"
    break;

  case 302: /* interval_expr: LONG_VALUE MINUTES  */
#line 2377 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6041 "parser.cpp"
    break;

  case 303: /* interval_expr: LONG_VALUE MINUTE  */
#line 2383 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6052 "parser.cpp"
    break;

  case 304: /* interval_expr: LONG_VALUE HOURS  */
#line 2389 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6063 "parser.cpp"
    break;

  case 305: /* interval_expr: LONG_VALUE HOUR  */
#line 2395 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6074 "parser.cpp"
    break;

  case 306: /* interval_expr: LONG_VALUE DAYS  */
#line 2401 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6085 "parser.cpp"
    break;

  case 307: /* interval_expr: LONG_VALUE DAY  */
#line 2407 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6096 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE MONTHS  */
#line 2413 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6107 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE MONTH  */
#line 2419 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6118 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE YEARS  */
#line 2425 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6129 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE YEAR  */
#line 2431 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6140 "parser.cpp"
    break;

  case 312: /* copy_option_list: copy_option  */
#line 2442 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6149 "parser.cpp"
    break;

  case 313: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2446 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6158 "parser.cpp"
    break;

  case 314: /* copy_option: FORMAT IDENTIFIER  */
#line 2451 "parser.y"
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
#line 6182 "parser.cpp"
    break;

  case 315: /* copy_option: DELIMITER STRING  */
#line 2470 "parser.y"
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
#line 6197 "parser.cpp"
    break;

  case 316: /* copy_option: HEADER  */
#line 2480 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6207 "parser.cpp"
    break;

  case 317: /* file_path: STRING  */
#line 2486 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6215 "parser.cpp"
    break;

  case 318: /* if_exists: IF EXISTS  */
#line 2490 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6221 "parser.cpp"
    break;

  case 319: /* if_exists: %empty  */
#line 2491 "parser.y"
  { (yyval.bool_value) = false; }
#line 6227 "parser.cpp"
    break;

  case 320: /* if_not_exists: IF NOT EXISTS  */
#line 2493 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6233 "parser.cpp"
    break;

  case 321: /* if_not_exists: %empty  */
#line 2494 "parser.y"
  { (yyval.bool_value) = false; }
#line 6239 "parser.cpp"
    break;

  case 324: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2509 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6252 "parser.cpp"
    break;

  case 325: /* if_not_exists_info: %empty  */
#line 2517 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6260 "parser.cpp"
    break;

  case 326: /* with_index_param_list: WITH '(' index_param_list ')'  */
#line 2522 "parser.y"
                                                      {
    (yyval.with_index_param_list_t) = std::move((yyvsp[-1].index_param_list_t));
}
#line 6268 "parser.cpp"
    break;

  case 327: /* with_index_param_list: %empty  */
#line 2525 "parser.y"
  {
    (yyval.with_index_param_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6276 "parser.cpp"
    break;

  case 328: /* index_param_list: index_param  */
#line 2529 "parser.y"
                               {
    (yyval.index_param_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_param_list_t)->push_back((yyvsp[0].index_param_t));
}
#line 6285 "parser.cpp"
    break;

  case 329: /* index_param_list: index_param_list ',' index_param  */
#line 2533 "parser.y"
                                   {
    (yyvsp[-2].index_param_list_t)->push_back((yyvsp[0].index_param_t));
    (yyval.index_param_list_t) = (yyvsp[-2].index_param_list_t);
}
#line 6294 "parser.cpp"
    break;

  case 330: /* index_param: IDENTIFIER  */
#line 2538 "parser.y"
                         {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6304 "parser.cpp"
    break;

  case 331: /* index_param: IDENTIFIER '=' IDENTIFIER  */
#line 2543 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6317 "parser.cpp"
    break;

  case 332: /* index_param: IDENTIFIER '=' LONG_VALUE  */
#line 2551 "parser.y"
                            {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6329 "parser.cpp"
    break;

  case 333: /* index_param: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2558 "parser.y"
                              {
    (yyval.index_param_t) = new infinity::InitParameter();
    (yyval.index_param_t)->param_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_param_t)->param_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6341 "parser.cpp"
    break;

  case 334: /* index_info_list: '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2569 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hlvq") == 0) {
        index_type = infinity::IndexType::kHnswLVQ;
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
#line 6396 "parser.cpp"
    break;

  case 335: /* index_info_list: index_info_list '(' identifier_array ')' USING IDENTIFIER with_index_param_list  */
#line 2619 "parser.y"
                                                                                  {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    infinity::IndexType index_type = infinity::IndexType::kInvalid;
    if(strcmp((yyvsp[-1].str_value), "fulltext") == 0) {
        index_type = infinity::IndexType::kIRSFullText;
    } else if (strcmp((yyvsp[-1].str_value), "hlvq") == 0) {
        index_type = infinity::IndexType::kHnswLVQ;
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
#line 6452 "parser.cpp"
    break;


#line 6456 "parser.cpp"

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

#line 2671 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}

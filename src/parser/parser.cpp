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
  YYSYMBOL_GROUP = 44,                     /* GROUP  */
  YYSYMBOL_BY = 45,                        /* BY  */
  YYSYMBOL_HAVING = 46,                    /* HAVING  */
  YYSYMBOL_AS = 47,                        /* AS  */
  YYSYMBOL_NATURAL = 48,                   /* NATURAL  */
  YYSYMBOL_JOIN = 49,                      /* JOIN  */
  YYSYMBOL_LEFT = 50,                      /* LEFT  */
  YYSYMBOL_RIGHT = 51,                     /* RIGHT  */
  YYSYMBOL_OUTER = 52,                     /* OUTER  */
  YYSYMBOL_FULL = 53,                      /* FULL  */
  YYSYMBOL_ON = 54,                        /* ON  */
  YYSYMBOL_INNER = 55,                     /* INNER  */
  YYSYMBOL_CROSS = 56,                     /* CROSS  */
  YYSYMBOL_DISTINCT = 57,                  /* DISTINCT  */
  YYSYMBOL_WHERE = 58,                     /* WHERE  */
  YYSYMBOL_ORDER = 59,                     /* ORDER  */
  YYSYMBOL_LIMIT = 60,                     /* LIMIT  */
  YYSYMBOL_OFFSET = 61,                    /* OFFSET  */
  YYSYMBOL_ASC = 62,                       /* ASC  */
  YYSYMBOL_DESC = 63,                      /* DESC  */
  YYSYMBOL_IF = 64,                        /* IF  */
  YYSYMBOL_NOT = 65,                       /* NOT  */
  YYSYMBOL_EXISTS = 66,                    /* EXISTS  */
  YYSYMBOL_IN = 67,                        /* IN  */
  YYSYMBOL_FROM = 68,                      /* FROM  */
  YYSYMBOL_TO = 69,                        /* TO  */
  YYSYMBOL_WITH = 70,                      /* WITH  */
  YYSYMBOL_DELIMITER = 71,                 /* DELIMITER  */
  YYSYMBOL_FORMAT = 72,                    /* FORMAT  */
  YYSYMBOL_HEADER = 73,                    /* HEADER  */
  YYSYMBOL_CAST = 74,                      /* CAST  */
  YYSYMBOL_END = 75,                       /* END  */
  YYSYMBOL_CASE = 76,                      /* CASE  */
  YYSYMBOL_ELSE = 77,                      /* ELSE  */
  YYSYMBOL_THEN = 78,                      /* THEN  */
  YYSYMBOL_WHEN = 79,                      /* WHEN  */
  YYSYMBOL_BOOLEAN = 80,                   /* BOOLEAN  */
  YYSYMBOL_INTEGER = 81,                   /* INTEGER  */
  YYSYMBOL_INT = 82,                       /* INT  */
  YYSYMBOL_TINYINT = 83,                   /* TINYINT  */
  YYSYMBOL_SMALLINT = 84,                  /* SMALLINT  */
  YYSYMBOL_BIGINT = 85,                    /* BIGINT  */
  YYSYMBOL_HUGEINT = 86,                   /* HUGEINT  */
  YYSYMBOL_CHAR = 87,                      /* CHAR  */
  YYSYMBOL_VARCHAR = 88,                   /* VARCHAR  */
  YYSYMBOL_FLOAT = 89,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 90,                    /* DOUBLE  */
  YYSYMBOL_REAL = 91,                      /* REAL  */
  YYSYMBOL_DECIMAL = 92,                   /* DECIMAL  */
  YYSYMBOL_DATE = 93,                      /* DATE  */
  YYSYMBOL_TIME = 94,                      /* TIME  */
  YYSYMBOL_DATETIME = 95,                  /* DATETIME  */
  YYSYMBOL_TIMESTAMP = 96,                 /* TIMESTAMP  */
  YYSYMBOL_UUID = 97,                      /* UUID  */
  YYSYMBOL_POINT = 98,                     /* POINT  */
  YYSYMBOL_LINE = 99,                      /* LINE  */
  YYSYMBOL_LSEG = 100,                     /* LSEG  */
  YYSYMBOL_BOX = 101,                      /* BOX  */
  YYSYMBOL_PATH = 102,                     /* PATH  */
  YYSYMBOL_POLYGON = 103,                  /* POLYGON  */
  YYSYMBOL_CIRCLE = 104,                   /* CIRCLE  */
  YYSYMBOL_BLOB = 105,                     /* BLOB  */
  YYSYMBOL_BITMAP = 106,                   /* BITMAP  */
  YYSYMBOL_EMBEDDING = 107,                /* EMBEDDING  */
  YYSYMBOL_VECTOR = 108,                   /* VECTOR  */
  YYSYMBOL_BIT = 109,                      /* BIT  */
  YYSYMBOL_PRIMARY = 110,                  /* PRIMARY  */
  YYSYMBOL_KEY = 111,                      /* KEY  */
  YYSYMBOL_UNIQUE = 112,                   /* UNIQUE  */
  YYSYMBOL_NULLABLE = 113,                 /* NULLABLE  */
  YYSYMBOL_IS = 114,                       /* IS  */
  YYSYMBOL_TRUE = 115,                     /* TRUE  */
  YYSYMBOL_FALSE = 116,                    /* FALSE  */
  YYSYMBOL_INTERVAL = 117,                 /* INTERVAL  */
  YYSYMBOL_SECOND = 118,                   /* SECOND  */
  YYSYMBOL_SECONDS = 119,                  /* SECONDS  */
  YYSYMBOL_MINUTE = 120,                   /* MINUTE  */
  YYSYMBOL_MINUTES = 121,                  /* MINUTES  */
  YYSYMBOL_HOUR = 122,                     /* HOUR  */
  YYSYMBOL_HOURS = 123,                    /* HOURS  */
  YYSYMBOL_DAY = 124,                      /* DAY  */
  YYSYMBOL_DAYS = 125,                     /* DAYS  */
  YYSYMBOL_MONTH = 126,                    /* MONTH  */
  YYSYMBOL_MONTHS = 127,                   /* MONTHS  */
  YYSYMBOL_YEAR = 128,                     /* YEAR  */
  YYSYMBOL_YEARS = 129,                    /* YEARS  */
  YYSYMBOL_EQUAL = 130,                    /* EQUAL  */
  YYSYMBOL_NOT_EQ = 131,                   /* NOT_EQ  */
  YYSYMBOL_LESS_EQ = 132,                  /* LESS_EQ  */
  YYSYMBOL_GREATER_EQ = 133,               /* GREATER_EQ  */
  YYSYMBOL_BETWEEN = 134,                  /* BETWEEN  */
  YYSYMBOL_AND = 135,                      /* AND  */
  YYSYMBOL_OR = 136,                       /* OR  */
  YYSYMBOL_EXTRACT = 137,                  /* EXTRACT  */
  YYSYMBOL_LIKE = 138,                     /* LIKE  */
  YYSYMBOL_DATA = 139,                     /* DATA  */
  YYSYMBOL_LOG = 140,                      /* LOG  */
  YYSYMBOL_BUFFER = 141,                   /* BUFFER  */
  YYSYMBOL_KNN = 142,                      /* KNN  */
  YYSYMBOL_USING = 143,                    /* USING  */
  YYSYMBOL_SESSION = 144,                  /* SESSION  */
  YYSYMBOL_GLOBAL = 145,                   /* GLOBAL  */
  YYSYMBOL_OFF = 146,                      /* OFF  */
  YYSYMBOL_EXPORT = 147,                   /* EXPORT  */
  YYSYMBOL_PROFILE = 148,                  /* PROFILE  */
  YYSYMBOL_CONFIGS = 149,                  /* CONFIGS  */
  YYSYMBOL_PROFILES = 150,                 /* PROFILES  */
  YYSYMBOL_SEARCH = 151,                   /* SEARCH  */
  YYSYMBOL_MATCH = 152,                    /* MATCH  */
  YYSYMBOL_QUERY = 153,                    /* QUERY  */
  YYSYMBOL_FUSION = 154,                   /* FUSION  */
  YYSYMBOL_NUMBER = 155,                   /* NUMBER  */
  YYSYMBOL_156_ = 156,                     /* '='  */
  YYSYMBOL_157_ = 157,                     /* '<'  */
  YYSYMBOL_158_ = 158,                     /* '>'  */
  YYSYMBOL_159_ = 159,                     /* '+'  */
  YYSYMBOL_160_ = 160,                     /* '-'  */
  YYSYMBOL_161_ = 161,                     /* '*'  */
  YYSYMBOL_162_ = 162,                     /* '/'  */
  YYSYMBOL_163_ = 163,                     /* '%'  */
  YYSYMBOL_164_ = 164,                     /* '['  */
  YYSYMBOL_165_ = 165,                     /* ']'  */
  YYSYMBOL_166_ = 166,                     /* '('  */
  YYSYMBOL_167_ = 167,                     /* ')'  */
  YYSYMBOL_168_ = 168,                     /* '.'  */
  YYSYMBOL_169_ = 169,                     /* ';'  */
  YYSYMBOL_170_ = 170,                     /* ','  */
  YYSYMBOL_YYACCEPT = 171,                 /* $accept  */
  YYSYMBOL_input_pattern = 172,            /* input_pattern  */
  YYSYMBOL_statement_list = 173,           /* statement_list  */
  YYSYMBOL_statement = 174,                /* statement  */
  YYSYMBOL_explainable_statement = 175,    /* explainable_statement  */
  YYSYMBOL_create_statement = 176,         /* create_statement  */
  YYSYMBOL_table_element_array = 177,      /* table_element_array  */
  YYSYMBOL_table_element = 178,            /* table_element  */
  YYSYMBOL_table_column = 179,             /* table_column  */
  YYSYMBOL_column_type = 180,              /* column_type  */
  YYSYMBOL_column_constraints = 181,       /* column_constraints  */
  YYSYMBOL_column_constraint = 182,        /* column_constraint  */
  YYSYMBOL_table_constraint = 183,         /* table_constraint  */
  YYSYMBOL_identifier_array = 184,         /* identifier_array  */
  YYSYMBOL_delete_statement = 185,         /* delete_statement  */
  YYSYMBOL_insert_statement = 186,         /* insert_statement  */
  YYSYMBOL_optional_identifier_array = 187, /* optional_identifier_array  */
  YYSYMBOL_explain_statement = 188,        /* explain_statement  */
  YYSYMBOL_explain_type = 189,             /* explain_type  */
  YYSYMBOL_update_statement = 190,         /* update_statement  */
  YYSYMBOL_update_expr_array = 191,        /* update_expr_array  */
  YYSYMBOL_update_expr = 192,              /* update_expr  */
  YYSYMBOL_drop_statement = 193,           /* drop_statement  */
  YYSYMBOL_copy_statement = 194,           /* copy_statement  */
  YYSYMBOL_select_statement = 195,         /* select_statement  */
  YYSYMBOL_select_with_paren = 196,        /* select_with_paren  */
  YYSYMBOL_select_without_paren = 197,     /* select_without_paren  */
  YYSYMBOL_select_clause_with_modifier = 198, /* select_clause_with_modifier  */
  YYSYMBOL_select_clause_without_modifier_paren = 199, /* select_clause_without_modifier_paren  */
  YYSYMBOL_select_clause_without_modifier = 200, /* select_clause_without_modifier  */
  YYSYMBOL_order_by_clause = 201,          /* order_by_clause  */
  YYSYMBOL_order_by_expr_list = 202,       /* order_by_expr_list  */
  YYSYMBOL_order_by_expr = 203,            /* order_by_expr  */
  YYSYMBOL_order_by_type = 204,            /* order_by_type  */
  YYSYMBOL_limit_expr = 205,               /* limit_expr  */
  YYSYMBOL_offset_expr = 206,              /* offset_expr  */
  YYSYMBOL_distinct = 207,                 /* distinct  */
  YYSYMBOL_from_clause = 208,              /* from_clause  */
  YYSYMBOL_search_clause = 209,            /* search_clause  */
  YYSYMBOL_where_clause = 210,             /* where_clause  */
  YYSYMBOL_having_clause = 211,            /* having_clause  */
  YYSYMBOL_group_by_clause = 212,          /* group_by_clause  */
  YYSYMBOL_set_operator = 213,             /* set_operator  */
  YYSYMBOL_table_reference = 214,          /* table_reference  */
  YYSYMBOL_table_reference_unit = 215,     /* table_reference_unit  */
  YYSYMBOL_table_reference_name = 216,     /* table_reference_name  */
  YYSYMBOL_table_name = 217,               /* table_name  */
  YYSYMBOL_table_alias = 218,              /* table_alias  */
  YYSYMBOL_with_clause = 219,              /* with_clause  */
  YYSYMBOL_with_expr_list = 220,           /* with_expr_list  */
  YYSYMBOL_with_expr = 221,                /* with_expr  */
  YYSYMBOL_join_clause = 222,              /* join_clause  */
  YYSYMBOL_join_type = 223,                /* join_type  */
  YYSYMBOL_show_statement = 224,           /* show_statement  */
  YYSYMBOL_flush_statement = 225,          /* flush_statement  */
  YYSYMBOL_command_statement = 226,        /* command_statement  */
  YYSYMBOL_expr_array = 227,               /* expr_array  */
  YYSYMBOL_expr_array_list = 228,          /* expr_array_list  */
  YYSYMBOL_expr_alias = 229,               /* expr_alias  */
  YYSYMBOL_expr = 230,                     /* expr  */
  YYSYMBOL_operand = 231,                  /* operand  */
  YYSYMBOL_knn_expr = 232,                 /* knn_expr  */
  YYSYMBOL_match_expr = 233,               /* match_expr  */
  YYSYMBOL_query_expr = 234,               /* query_expr  */
  YYSYMBOL_fusion_expr = 235,              /* fusion_expr  */
  YYSYMBOL_sub_search_array = 236,         /* sub_search_array  */
  YYSYMBOL_function_expr = 237,            /* function_expr  */
  YYSYMBOL_conjunction_expr = 238,         /* conjunction_expr  */
  YYSYMBOL_between_expr = 239,             /* between_expr  */
  YYSYMBOL_in_expr = 240,                  /* in_expr  */
  YYSYMBOL_case_expr = 241,                /* case_expr  */
  YYSYMBOL_case_check_array = 242,         /* case_check_array  */
  YYSYMBOL_cast_expr = 243,                /* cast_expr  */
  YYSYMBOL_subquery_expr = 244,            /* subquery_expr  */
  YYSYMBOL_column_expr = 245,              /* column_expr  */
  YYSYMBOL_constant_expr = 246,            /* constant_expr  */
  YYSYMBOL_array_expr = 247,               /* array_expr  */
  YYSYMBOL_long_array_expr = 248,          /* long_array_expr  */
  YYSYMBOL_unclosed_long_array_expr = 249, /* unclosed_long_array_expr  */
  YYSYMBOL_double_array_expr = 250,        /* double_array_expr  */
  YYSYMBOL_unclosed_double_array_expr = 251, /* unclosed_double_array_expr  */
  YYSYMBOL_interval_expr = 252,            /* interval_expr  */
  YYSYMBOL_copy_option_list = 253,         /* copy_option_list  */
  YYSYMBOL_copy_option = 254,              /* copy_option  */
  YYSYMBOL_file_path = 255,                /* file_path  */
  YYSYMBOL_if_exists = 256,                /* if_exists  */
  YYSYMBOL_if_not_exists = 257,            /* if_not_exists  */
  YYSYMBOL_semicolon = 258,                /* semicolon  */
  YYSYMBOL_if_not_exists_info = 259,       /* if_not_exists_info  */
  YYSYMBOL_with_index_para_list = 260,     /* with_index_para_list  */
  YYSYMBOL_index_para_list = 261,          /* index_para_list  */
  YYSYMBOL_index_para = 262                /* index_para  */
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
#define YYLAST   826

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  171
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  92
/* YYNRULES -- Number of rules.  */
#define YYNRULES  334
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  653

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   410


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
       2,     2,     2,     2,     2,     2,     2,   163,     2,     2,
     166,   167,   161,   159,   170,   160,   168,   162,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   169,
     157,   156,   158,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   164,     2,   165,     2,     2,     2,     2,     2,     2,
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
     155
};

#if SQLDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   449,   449,   453,   459,   466,   467,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   478,   479,   480,   481,
     482,   483,   484,   485,   486,   487,   494,   507,   522,   546,
     562,   580,   607,   611,   617,   620,   626,   661,   695,   696,
     697,   698,   699,   700,   701,   702,   703,   704,   705,   706,
     707,   708,   709,   710,   711,   712,   713,   714,   715,   716,
     718,   719,   720,   721,   722,   723,   724,   725,   726,   727,
     728,   729,   730,   731,   732,   733,   734,   735,   736,   737,
     738,   739,   740,   741,   760,   764,   774,   777,   780,   783,
     787,   792,   799,   805,   815,   831,   843,   856,   859,   866,
     872,   875,   878,   881,   884,   887,   890,   893,   900,   913,
     917,   922,   935,   948,   963,   978,   993,  1016,  1057,  1102,
    1105,  1108,  1117,  1127,  1130,  1134,  1139,  1146,  1149,  1154,
    1170,  1173,  1177,  1181,  1186,  1192,  1195,  1198,  1202,  1206,
    1208,  1212,  1214,  1217,  1221,  1224,  1228,  1233,  1237,  1240,
    1244,  1247,  1251,  1254,  1258,  1261,  1264,  1267,  1275,  1278,
    1293,  1293,  1295,  1309,  1318,  1325,  1336,  1341,  1346,  1352,
    1359,  1362,  1366,  1369,  1374,  1386,  1393,  1407,  1410,  1413,
    1416,  1419,  1422,  1425,  1431,  1435,  1439,  1443,  1447,  1451,
    1462,  1477,  1481,  1485,  1493,  1499,  1504,  1510,  1516,  1524,
    1530,  1536,  1542,  1548,  1556,  1562,  1573,  1577,  1582,  1586,
    1613,  1619,  1623,  1624,  1625,  1626,  1627,  1629,  1632,  1638,
    1641,  1642,  1643,  1644,  1645,  1646,  1647,  1648,  1650,  1791,
    1800,  1810,  1817,  1827,  1837,  1841,  1845,  1849,  1853,  1857,
    1861,  1865,  1870,  1878,  1886,  1895,  1902,  1909,  1916,  1923,
    1930,  1938,  1946,  1954,  1962,  1970,  1978,  1986,  1994,  2002,
    2010,  2018,  2026,  2056,  2064,  2073,  2081,  2090,  2098,  2104,
    2111,  2117,  2124,  2129,  2136,  2143,  2151,  2175,  2181,  2187,
    2194,  2202,  2209,  2216,  2221,  2231,  2236,  2241,  2246,  2251,
    2256,  2261,  2264,  2276,  2279,  2283,  2287,  2292,  2297,  2301,
    2306,  2311,  2317,  2323,  2329,  2335,  2341,  2347,  2353,  2359,
    2365,  2371,  2377,  2388,  2392,  2397,  2416,  2422,  2428,  2432,
    2433,  2435,  2436,  2438,  2439,  2451,  2459,  2464,  2467,  2471,
    2475,  2480,  2485,  2493,  2500
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
  "INDEX", "ANALYZE", "VIEWS", "DATABASES", "GROUP", "BY", "HAVING", "AS",
  "NATURAL", "JOIN", "LEFT", "RIGHT", "OUTER", "FULL", "ON", "INNER",
  "CROSS", "DISTINCT", "WHERE", "ORDER", "LIMIT", "OFFSET", "ASC", "DESC",
  "IF", "NOT", "EXISTS", "IN", "FROM", "TO", "WITH", "DELIMITER", "FORMAT",
  "HEADER", "CAST", "END", "CASE", "ELSE", "THEN", "WHEN", "BOOLEAN",
  "INTEGER", "INT", "TINYINT", "SMALLINT", "BIGINT", "HUGEINT", "CHAR",
  "VARCHAR", "FLOAT", "DOUBLE", "REAL", "DECIMAL", "DATE", "TIME",
  "DATETIME", "TIMESTAMP", "UUID", "POINT", "LINE", "LSEG", "BOX", "PATH",
  "POLYGON", "CIRCLE", "BLOB", "BITMAP", "EMBEDDING", "VECTOR", "BIT",
  "PRIMARY", "KEY", "UNIQUE", "NULLABLE", "IS", "TRUE", "FALSE",
  "INTERVAL", "SECOND", "SECONDS", "MINUTE", "MINUTES", "HOUR", "HOURS",
  "DAY", "DAYS", "MONTH", "MONTHS", "YEAR", "YEARS", "EQUAL", "NOT_EQ",
  "LESS_EQ", "GREATER_EQ", "BETWEEN", "AND", "OR", "EXTRACT", "LIKE",
  "DATA", "LOG", "BUFFER", "KNN", "USING", "SESSION", "GLOBAL", "OFF",
  "EXPORT", "PROFILE", "CONFIGS", "PROFILES", "SEARCH", "MATCH", "QUERY",
  "FUSION", "NUMBER", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'['", "']'", "'('", "')'", "'.'", "';'", "','", "$accept",
  "input_pattern", "statement_list", "statement", "explainable_statement",
  "create_statement", "table_element_array", "table_element",
  "table_column", "column_type", "column_constraints", "column_constraint",
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
  "flush_statement", "command_statement", "expr_array", "expr_array_list",
  "expr_alias", "expr", "operand", "knn_expr", "match_expr", "query_expr",
  "fusion_expr", "sub_search_array", "function_expr", "conjunction_expr",
  "between_expr", "in_expr", "case_expr", "case_check_array", "cast_expr",
  "subquery_expr", "column_expr", "constant_expr", "array_expr",
  "long_array_expr", "unclosed_long_array_expr", "double_array_expr",
  "unclosed_double_array_expr", "interval_expr", "copy_option_list",
  "copy_option", "file_path", "if_exists", "if_not_exists", "semicolon",
  "if_not_exists_info", "with_index_para_list", "index_para_list",
  "index_para", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-333)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-327)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     573,   178,    35,   230,    49,     0,    49,    70,   423,    39,
      85,   105,    82,   111,   -31,   -44,   133,    -4,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,   204,  -333,  -333,   127,
    -333,  -333,  -333,   103,   103,   103,   103,    -6,    49,   186,
     186,   186,   186,   186,    93,   258,    49,   153,   278,   279,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,    64,  -333,  -333,
    -333,  -333,  -333,    49,  -333,  -333,  -333,  -333,  -333,   247,
     119,  -333,   289,   132,   138,  -333,    21,  -333,   287,  -333,
    -333,     4,   243,  -333,   263,   286,   343,    49,    49,    49,
     347,   269,   170,   291,   355,    49,    49,    49,   374,   378,
     379,   330,   386,   386,    38,    51,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,   204,  -333,  -333,  -333,  -333,   229,   111,
     386,  -333,  -333,  -333,  -333,     4,  -333,  -333,  -333,   360,
     351,   345,   338,  -333,   -42,  -333,   170,  -333,    49,   407,
      -7,  -333,  -333,  -333,  -333,  -333,   368,  -333,   267,   -34,
    -333,   360,  -333,  -333,   354,   361,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,   127,  -333,  -333,   266,
     270,   288,  -333,  -333,   606,   415,   296,   297,   232,   465,
    -333,  -333,   464,   305,   312,   322,   328,   339,   479,   479,
    -333,   313,   -54,  -333,   -28,   535,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,   336,  -333,  -333,
     360,   360,   425,  -333,   -44,     3,   452,   340,  -333,   140,
     341,  -333,    49,   360,   379,  -333,    97,   349,   350,   342,
    -333,  -333,   126,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,   479,   352,   182,   440,   360,
     360,    58,   124,  -333,   606,  -333,   507,   360,   518,   519,
     520,   104,   104,   358,   -74,     5,   360,   376,   525,   360,
     360,   -38,   363,   -27,   479,   479,   479,   479,   479,   479,
     479,   479,   479,   479,   479,   479,   479,   479,    15,   367,
    -333,    77,    97,   360,  -333,   204,   669,   427,   373,   154,
    -333,  -333,  -333,   -44,   407,  -333,   537,   360,   372,  -333,
      97,  -333,   344,   344,  -333,   360,  -333,   192,   440,   405,
     380,    -8,   -24,   185,  -333,   360,   360,   477,    50,   381,
     200,   384,  -333,  -333,   -44,   388,   391,  -333,    94,  -333,
    -333,    56,   330,  -333,  -333,   411,   382,   479,   313,   436,
    -333,   585,   585,   169,   169,   544,   585,   585,   169,   169,
     104,   104,  -333,  -333,  -333,  -333,  -333,   360,  -333,  -333,
    -333,    97,  -333,  -333,  -333,  -333,  -333,  -333,  -333,   390,
    -333,  -333,  -333,  -333,   412,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,   413,   424,   426,
     431,   139,   435,   407,  -333,     3,   204,   201,  -333,   205,
     437,   546,   568,  -333,   206,  -333,   227,   242,  -333,   406,
    -333,   669,   360,  -333,   360,   -26,    45,   479,   441,   587,
    -333,   600,   602,    -1,     5,   528,  -333,  -333,  -333,  -333,
    -333,  -333,   558,  -333,   605,  -333,  -333,  -333,  -333,  -333,
     442,   566,   313,   585,   444,   244,  -333,   479,  -333,   607,
     608,   609,   611,   271,   318,   505,   508,  -333,  -333,   139,
    -333,   407,   265,  -333,   480,  -333,   360,  -333,  -333,  -333,
     344,  -333,  -333,  -333,   453,    97,     8,  -333,   360,   403,
     234,   454,  -333,    25,  -333,    61,   281,   455,   458,    94,
     391,     5,     5,   460,    56,   582,   583,   461,   282,  -333,
    -333,   182,   463,   320,   467,   468,   466,   471,   472,   474,
     476,   478,   481,   482,   483,   484,   485,   486,   487,   489,
     490,   491,  -333,  -333,  -333,   325,  -333,   634,   326,  -333,
    -333,  -333,    97,  -333,  -333,  -333,   643,  -333,   644,  -333,
     657,  -333,   659,  -333,  -333,  -333,  -333,   610,   407,  -333,
    -333,  -333,  -333,   360,   360,  -333,  -333,  -333,  -333,  -333,
     664,  -333,  -333,   665,   666,   672,   674,   675,   677,   678,
     679,   680,   681,   682,   683,   684,   702,   703,   704,  -333,
     641,  -333,   542,  -333,  -333,   547,   360,   331,   543,    97,
     552,   554,   555,   569,   570,   571,   612,   613,   614,   615,
     616,   617,   618,   619,   620,   621,   622,   574,  -333,   774,
    -333,    97,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
     787,   624,   636,   333,  -333,  -333,   285,  -333,   787,  -333,
    -333,  -333,  -333
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     171,     0,     0,     0,     0,     0,     0,     0,   107,     0,
       0,     0,     0,     0,     0,   171,     0,   324,     3,     5,
      10,    12,    13,    11,     6,     7,     9,   120,   119,     0,
       8,    14,    15,   322,   322,   322,   322,   322,     0,   320,
     320,   320,   320,   320,   164,     0,     0,     0,     0,     0,
     101,   105,   102,   103,   104,   106,   100,   171,   185,   186,
     184,   187,   188,     0,   189,   191,   192,   193,   194,     0,
     170,   172,     0,     0,     0,     1,   171,     2,   154,   156,
     157,     0,   143,   125,   131,     0,     0,     0,     0,     0,
       0,     0,    98,     0,     0,     0,     0,     0,     0,     0,
       0,   149,     0,     0,     0,     0,    99,    16,    21,    23,
      22,    17,    18,    20,    19,    24,    25,   190,     0,     0,
       0,   124,   123,     4,   155,     0,   121,   122,   142,     0,
       0,   139,     0,    26,     0,    27,    98,   325,     0,     0,
     171,   319,   112,   114,   113,   115,     0,   165,     0,   149,
     109,     0,    94,   318,     0,     0,   198,   200,   199,   196,
     197,   203,   205,   204,   201,   202,     0,   173,   195,     0,
       0,   281,   285,   288,   289,     0,     0,     0,     0,     0,
     286,   287,     0,     0,     0,     0,     0,     0,     0,     0,
     283,   171,   145,   206,   211,   212,   224,   225,   226,   227,
     221,   216,   215,   214,   222,   223,   213,   220,   219,   292,
       0,     0,   141,   321,   171,     0,     0,     0,    92,     0,
       0,    96,     0,     0,     0,   108,   148,     0,     0,     0,
     128,   127,     0,   302,   301,   304,   303,   306,   305,   308,
     307,   310,   309,   312,   311,     0,     0,   247,   171,     0,
       0,     0,     0,   290,     0,   291,     0,     0,     0,     0,
       0,   249,   248,     0,     0,     0,     0,   147,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   130,
     132,   137,   138,     0,   126,    29,     0,     0,     0,     0,
      32,    34,    35,   171,     0,    97,     0,     0,    95,   116,
     111,   110,     0,     0,   174,     0,   242,     0,   171,     0,
       0,     0,     0,     0,   272,     0,     0,     0,     0,     0,
       0,     0,   218,   217,   171,   144,   158,   160,   169,   161,
     207,     0,   149,   210,   265,   266,     0,     0,   171,     0,
     246,   256,   257,   260,   261,     0,   263,   255,   258,   259,
     251,   250,   252,   253,   254,   282,   284,     0,   135,   136,
     134,   140,    38,    41,    42,    39,    40,    43,    44,    60,
      62,    45,    47,    46,    65,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,     0,     0,     0,
       0,    36,     0,     0,    28,     0,    30,     0,    93,     0,
       0,     0,     0,   317,     0,   313,     0,     0,   243,     0,
     277,     0,     0,   270,     0,     0,     0,     0,     0,     0,
     231,     0,     0,     0,     0,     0,   178,   179,   180,   181,
     177,   182,     0,   167,     0,   162,   234,   235,   236,   237,
     146,   153,   171,   264,     0,     0,   245,     0,   133,     0,
       0,     0,     0,     0,     0,     0,     0,    87,    88,    37,
      84,     0,     0,    33,     0,   208,     0,   316,   315,   118,
       0,   117,   244,   278,     0,   274,     0,   273,     0,     0,
       0,     0,   293,     0,   294,     0,     0,     0,     0,   169,
     159,     0,     0,   166,     0,     0,   151,     0,     0,   279,
     268,   267,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    89,    86,    85,     0,    91,     0,     0,   314,
     276,   271,   275,   262,   299,   296,     0,   295,     0,   298,
       0,   229,     0,   232,   233,   163,   175,     0,     0,   238,
     239,   240,   241,     0,     0,   129,   280,   269,    61,    64,
       0,    66,    67,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    90,
     328,   209,     0,   297,   300,     0,     0,     0,   152,   150,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,     0,
     230,   176,   168,    63,    71,    72,    69,    70,    73,    74,
      75,    68,    79,    80,    77,    78,    81,    82,    83,    76,
       0,     0,   331,     0,   329,   228,     0,   327,     0,   332,
     334,   333,   330
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -333,  -333,  -333,   717,  -333,   737,  -333,   392,  -333,   375,
    -333,   329,  -333,  -302,   738,   742,   667,  -333,  -333,   743,
    -333,   577,   745,   747,   -50,   790,   -14,   640,   685,    57,
    -333,  -333,   445,  -333,  -333,  -333,  -333,  -333,  -333,  -146,
    -333,  -333,  -333,  -333,   377,  -247,     7,   308,  -333,  -333,
     689,  -333,  -333,   752,   756,   757,  -228,  -333,   549,  -151,
    -124,  -332,  -331,  -326,  -325,  -333,  -333,  -333,  -333,  -333,
    -333,   565,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,   635,   506,   346,   -53,   106,   240,  -333,  -333,  -333,
    -333,   172
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    16,    17,    18,   106,    19,   299,   300,   301,   401,
     469,   470,   302,   219,    20,    21,   140,    22,    57,    23,
     149,   150,    24,    25,    26,    27,    28,    83,   126,    84,
     131,   289,   290,   370,   212,   294,   129,   267,   342,   152,
     565,   506,    81,   335,   336,   337,   338,   445,    29,    70,
      71,   339,   442,    30,    31,    32,   192,   308,   193,   194,
     195,   196,   197,   198,   199,   450,   200,   201,   202,   203,
     204,   252,   205,   206,   207,   208,   491,   492,   493,   494,
     495,   209,   414,   415,   154,    94,    86,    77,    91,   618,
     643,   644
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     226,    74,   407,   225,   317,   214,   296,   113,    44,   446,
     447,    45,    82,    47,   265,   448,   449,    64,   365,   268,
      78,  -323,    79,    80,   151,   220,    13,   251,     1,   346,
       2,     3,     4,     5,     6,     7,     8,     9,   349,   421,
     264,    10,   156,   157,   158,    92,    11,    12,  -326,   487,
     155,   247,    44,   101,   422,   161,   162,   163,    85,   291,
     292,   269,   270,    13,   261,   262,    38,   168,    46,    58,
     117,     1,   310,     2,     3,     4,     5,     6,     7,   409,
       9,    59,    60,   541,    10,    68,   350,   417,    44,    11,
      12,    13,   159,   333,   134,   135,   136,   443,   321,   322,
     347,   472,   143,   144,   145,   164,   328,   269,   270,   269,
     270,   269,   270,   297,    69,   298,   266,    72,   344,   345,
     455,   247,    15,   488,   215,    63,   221,   269,   270,   171,
     172,   173,   174,    75,    13,    82,   224,   250,   127,   368,
     369,   444,   371,   269,   270,   217,    95,    96,    97,    98,
     351,   352,   353,   354,   355,   356,   357,   358,   359,   360,
     361,   362,   363,   364,   295,    76,   499,    85,    14,   535,
     125,   334,   559,   560,   425,   426,   366,   263,   561,   562,
     269,   270,   170,   315,   160,   269,   270,    15,    61,    62,
     547,   175,   176,   269,   270,   548,   451,   165,   184,   324,
     177,   325,   178,   326,   465,    33,    34,    35,   185,   186,
     187,    14,   269,   270,    48,    49,   291,    36,    37,   179,
     428,   102,   103,   453,   508,    78,   549,    79,    80,   309,
      15,   550,   269,   270,   320,   171,   172,   173,   174,   544,
     545,   180,   181,   182,    65,    66,    67,   319,   538,   466,
      93,   467,   468,   406,   556,   557,   597,    39,    40,    41,
     423,    99,   424,   183,   326,   285,   286,   287,   184,    42,
      43,   485,   100,   486,    87,    88,    89,    90,   185,   186,
     187,   104,   105,   273,   433,   188,   189,   190,   649,   119,
     650,   651,   191,   316,   118,   120,   273,   175,   176,   121,
     128,  -327,  -327,   489,   419,   122,   177,   305,   178,   124,
     306,   250,   274,   275,   276,   277,   171,   172,   173,   174,
     279,   404,   130,   138,   405,   179,  -327,  -327,   283,   284,
     285,   286,   287,   511,   454,   598,   139,   542,   280,   281,
     282,   283,   284,   285,   286,   287,   133,   180,   181,   182,
     137,   132,   516,   517,   518,   519,   520,   141,   142,   418,
     521,   522,   266,   171,   172,   173,   174,   430,   474,   183,
     431,   306,   475,   479,   184,   266,   480,   146,   175,   176,
     523,   147,   148,    13,   185,   186,   187,   177,   151,   178,
     153,   188,   189,   190,   481,   166,   210,   480,   191,   524,
     525,   526,   527,   528,   213,   211,   179,   529,   530,   482,
     218,   510,   266,   599,   266,   411,   412,   413,   171,   172,
     173,   174,   222,   223,   227,   175,   176,   531,   180,   181,
     182,   228,   536,   230,   177,   306,   178,   231,   507,   435,
    -183,   436,   437,   438,   439,   621,   440,   441,   551,   567,
     183,   552,   266,   179,   232,   184,    50,    51,    52,    53,
      54,    55,   248,   249,    56,   185,   186,   187,   319,   253,
     254,   256,   188,   189,   190,   180,   181,   182,   257,   191,
     245,   246,   171,   172,   173,   174,   293,   569,   258,   177,
     570,   178,   589,   591,   259,   306,   266,   183,   622,   303,
     647,   306,   184,   648,   288,   260,   304,   307,   179,   314,
      13,   327,   185,   186,   187,   312,   313,   273,   318,   188,
     189,   190,   329,   330,   331,   332,   191,   341,   343,   348,
     180,   181,   182,   274,   275,   276,   277,   367,   402,   403,
     408,   279,   410,   347,   245,   427,   269,   420,   452,   456,
     477,   429,   183,   177,   432,   178,   459,   184,   434,   280,
     281,   282,   283,   284,   285,   286,   287,   185,   186,   187,
     543,   478,   179,   483,   188,   189,   190,   501,   460,   461,
       1,   191,     2,     3,     4,     5,     6,     7,     8,     9,
     462,   496,   463,    10,   180,   181,   182,   464,    11,    12,
     271,   471,   272,   476,   497,   490,   498,   502,   503,   319,
     505,   509,   504,   512,   513,   514,   183,   515,   532,   533,
     540,   184,   553,   537,   546,   554,   558,   563,   566,   564,
     568,   185,   186,   187,   571,   572,   573,   590,   188,   189,
     190,   574,   575,    13,   576,   191,   577,   592,   578,   273,
     593,   579,   580,   581,   582,   583,   584,   585,   273,   586,
     587,   588,   594,   595,   596,   274,   275,   276,   277,   278,
     600,   601,   602,   279,   274,   275,   276,   277,   603,   457,
     604,   605,   279,   606,   607,   608,   609,   610,   611,   612,
     613,   280,   281,   282,   283,   284,   285,   286,   287,   273,
     280,   281,   282,   283,   284,   285,   286,   287,   614,   615,
     616,   617,   619,   266,   620,  -327,  -327,   276,   277,   623,
      14,   624,   625,  -327,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   626,   627,   628,    15,
     640,  -327,   281,   282,   283,   284,   285,   286,   287,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   390,   391,   392,
     393,   394,   395,   396,   397,   398,   399,   400,   641,   629,
     630,   631,   632,   633,   634,   635,   636,   637,   638,   639,
     642,   645,   646,   123,   107,   108,   484,   473,   534,   109,
     110,   311,   111,   216,   112,    73,   229,   555,   167,   114,
     169,   500,   458,   115,   116,   340,   323,   255,     0,   416,
     652,     0,     0,     0,     0,     0,   539
};

static const yytype_int16 yycheck[] =
{
     151,    15,   304,   149,   232,    47,     3,    57,     3,   341,
     341,     4,     8,     6,    68,   341,   341,    10,     3,    47,
      21,     0,    23,    24,    58,    32,    70,   178,     7,    67,
       9,    10,    11,    12,    13,    14,    15,    16,    65,    47,
     191,    20,     4,     5,     6,    38,    25,    26,    54,    75,
     103,   175,     3,    46,    78,     4,     5,     6,    64,   210,
     211,   135,   136,    70,   188,   189,    31,   120,    68,    30,
      63,     7,   223,     9,    10,    11,    12,    13,    14,   307,
      16,    42,    43,    75,    20,     3,   113,   315,     3,    25,
      26,    70,    54,   167,    87,    88,    89,     3,   249,   250,
     138,   403,    95,    96,    97,    54,   257,   135,   136,   135,
     136,   135,   136,   110,     3,   112,   170,   148,   269,   270,
     348,   245,   166,    78,   166,    40,   140,   135,   136,     3,
       4,     5,     6,     0,    70,     8,   170,    79,    81,    62,
      63,    47,   293,   135,   136,   138,    40,    41,    42,    43,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   214,   169,   167,    64,   147,   471,
     166,   166,   504,   504,   325,   326,   161,   191,   504,   504,
     135,   136,   125,    57,   146,   135,   136,   166,   149,   150,
     165,    65,    66,   135,   136,   170,   342,   146,   142,    75,
      74,    77,    76,    79,    65,    27,    28,    29,   152,   153,
     154,   147,   135,   136,   144,   145,   367,    39,    40,    93,
     170,    68,    69,   347,   452,    21,   165,    23,    24,   222,
     166,   170,   135,   136,   248,     3,     4,     5,     6,     5,
       6,   115,   116,   117,   139,   140,   141,    65,   476,   110,
      64,   112,   113,   303,   501,   502,   558,    27,    28,    29,
      75,   168,    77,   137,    79,   161,   162,   163,   142,    39,
      40,   422,    14,   424,    34,    35,    36,    37,   152,   153,
     154,     3,     3,   114,   334,   159,   160,   161,     3,   170,
       5,     6,   166,   167,    47,     6,   114,    65,    66,   167,
      57,   132,   133,   427,   318,   167,    74,   167,    76,    22,
     170,    79,   130,   131,   132,   133,     3,     4,     5,     6,
     138,   167,    59,    54,   170,    93,   157,   158,   159,   160,
     161,   162,   163,   457,   348,   563,   166,   488,   156,   157,
     158,   159,   160,   161,   162,   163,     3,   115,   116,   117,
       3,    65,    81,    82,    83,    84,    85,    66,     3,   167,
      89,    90,   170,     3,     4,     5,     6,   167,   167,   137,
     170,   170,   167,   167,   142,   170,   170,     3,    65,    66,
     109,     3,     3,    70,   152,   153,   154,    74,    58,    76,
       4,   159,   160,   161,   167,   166,    45,   170,   166,    81,
      82,    83,    84,    85,    66,    60,    93,    89,    90,   167,
       3,   167,   170,   564,   170,    71,    72,    73,     3,     4,
       5,     6,    54,   156,    70,    65,    66,   109,   115,   116,
     117,    70,   167,   167,    74,   170,    76,   167,   452,    48,
      49,    50,    51,    52,    53,   596,    55,    56,   167,   167,
     137,   170,   170,    93,   166,   142,    33,    34,    35,    36,
      37,    38,   166,   166,    41,   152,   153,   154,    65,     4,
       6,   166,   159,   160,   161,   115,   116,   117,   166,   166,
      65,    66,     3,     4,     5,     6,    61,   167,   166,    74,
     170,    76,   167,   167,   166,   170,   170,   137,   167,    47,
     167,   170,   142,   170,   168,   166,   166,   166,    93,   167,
      70,     4,   152,   153,   154,   166,   166,   114,   166,   159,
     160,   161,     4,     4,     4,   167,   166,   151,     3,   166,
     115,   116,   117,   130,   131,   132,   133,   170,   111,   166,
       3,   138,   170,   138,    65,    68,   135,   167,   166,   113,
       4,   170,   137,    74,   170,    76,   166,   142,   170,   156,
     157,   158,   159,   160,   161,   162,   163,   152,   153,   154,
     167,     3,    93,   167,   159,   160,   161,    49,   166,   166,
       7,   166,     9,    10,    11,    12,    13,    14,    15,    16,
     166,     4,   166,    20,   115,   116,   117,   166,    25,    26,
      65,   166,    67,   166,     4,   164,     4,    49,     3,    65,
      44,   167,   170,     6,     6,     6,   137,     6,   113,   111,
     167,   142,   167,   143,   170,   167,   166,    45,   167,    46,
     167,   152,   153,   154,   167,   167,   170,     3,   159,   160,
     161,   170,   170,    70,   170,   166,   170,     4,   170,   114,
       6,   170,   170,   170,   170,   170,   170,   170,   114,   170,
     170,   170,     5,     4,    54,   130,   131,   132,   133,   134,
       6,     6,     6,   138,   130,   131,   132,   133,     6,   135,
       6,     6,   138,     6,     6,     6,     6,     6,     6,     6,
       6,   156,   157,   158,   159,   160,   161,   162,   163,   114,
     156,   157,   158,   159,   160,   161,   162,   163,     6,     6,
       6,    70,   170,   170,   167,   130,   131,   132,   133,   167,
     147,   167,   167,   138,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   167,   167,   167,   166,
     166,   156,   157,   158,   159,   160,   161,   162,   163,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,     4,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
       3,   167,   156,    76,    57,    57,   421,   405,   469,    57,
      57,   224,    57,   136,    57,    15,   166,   499,   119,    57,
     125,   434,   367,    57,    57,   266,   251,   182,    -1,   313,
     648,    -1,    -1,    -1,    -1,    -1,   480
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,     9,    10,    11,    12,    13,    14,    15,    16,
      20,    25,    26,    70,   147,   166,   172,   173,   174,   176,
     185,   186,   188,   190,   193,   194,   195,   196,   197,   219,
     224,   225,   226,    27,    28,    29,    39,    40,    31,    27,
      28,    29,    39,    40,     3,   217,    68,   217,   144,   145,
      33,    34,    35,    36,    37,    38,    41,   189,    30,    42,
      43,   149,   150,    40,   217,   139,   140,   141,     3,     3,
     220,   221,   148,   196,   197,     0,   169,   258,    21,    23,
      24,   213,     8,   198,   200,    64,   257,   257,   257,   257,
     257,   259,   217,    64,   256,   256,   256,   256,   256,   168,
      14,   217,    68,    69,     3,     3,   175,   176,   185,   186,
     190,   193,   194,   195,   224,   225,   226,   217,    47,   170,
       6,   167,   167,   174,    22,   166,   199,   200,    57,   207,
      59,   201,    65,     3,   217,   217,   217,     3,    54,   166,
     187,    66,     3,   217,   217,   217,     3,     3,     3,   191,
     192,    58,   210,     4,   255,   255,     4,     5,     6,    54,
     146,     4,     5,     6,    54,   146,   166,   221,   255,   199,
     200,     3,     4,     5,     6,    65,    66,    74,    76,    93,
     115,   116,   117,   137,   142,   152,   153,   154,   159,   160,
     161,   166,   227,   229,   230,   231,   232,   233,   234,   235,
     237,   238,   239,   240,   241,   243,   244,   245,   246,   252,
      45,    60,   205,    66,    47,   166,   187,   217,     3,   184,
      32,   197,    54,   156,   170,   210,   230,    70,    70,   198,
     167,   167,   166,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,    65,    66,   231,   166,   166,
      79,   230,   242,     4,     6,   252,   166,   166,   166,   166,
     166,   231,   231,   197,   230,    68,   170,   208,    47,   135,
     136,    65,    67,   114,   130,   131,   132,   133,   134,   138,
     156,   157,   158,   159,   160,   161,   162,   163,   168,   202,
     203,   230,   230,    61,   206,   195,     3,   110,   112,   177,
     178,   179,   183,    47,   166,   167,   170,   166,   228,   217,
     230,   192,   166,   166,   167,    57,   167,   227,   166,    65,
     197,   230,   230,   242,    75,    77,    79,     4,   230,     4,
       4,     4,   167,   167,   166,   214,   215,   216,   217,   222,
     229,   151,   209,     3,   230,   230,    67,   138,   166,    65,
     113,   231,   231,   231,   231,   231,   231,   231,   231,   231,
     231,   231,   231,   231,   231,     3,   161,   170,    62,    63,
     204,   230,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   180,   111,   166,   167,   170,   195,   184,     3,   227,
     170,    71,    72,    73,   253,   254,   253,   227,   167,   197,
     167,    47,    78,    75,    77,   230,   230,    68,   170,   170,
     167,   170,   170,   195,   170,    48,    50,    51,    52,    53,
      55,    56,   223,     3,    47,   218,   232,   233,   234,   235,
     236,   210,   166,   231,   197,   227,   113,   135,   203,   166,
     166,   166,   166,   166,   166,    65,   110,   112,   113,   181,
     182,   166,   184,   178,   167,   167,   166,     4,     3,   167,
     170,   167,   167,   167,   180,   230,   230,    75,    78,   231,
     164,   247,   248,   249,   250,   251,     4,     4,     4,   167,
     215,    49,    49,     3,   170,    44,   212,   197,   227,   167,
     167,   231,     6,     6,     6,     6,    81,    82,    83,    84,
      85,    89,    90,   109,    81,    82,    83,    84,    85,    89,
      90,   109,   113,   111,   182,   184,   167,   143,   227,   254,
     167,    75,   230,   167,     5,     6,   170,   165,   170,   165,
     170,   167,   170,   167,   167,   218,   216,   216,   166,   232,
     233,   234,   235,    45,    46,   211,   167,   167,   167,   167,
     170,   167,   167,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   167,
       3,   167,     4,     6,     5,     4,    54,   184,   227,   230,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,    70,   260,   170,
     167,   230,   167,   167,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   167,   167,   167,   167,   167,   167,
     166,     4,     3,   261,   262,   167,   156,   167,   170,     3,
       5,     6,   262
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   171,   172,   173,   173,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   176,   176,   176,   176,
     176,   176,   177,   177,   178,   178,   179,   179,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   181,   181,   182,   182,   182,   182,
     183,   183,   184,   184,   185,   186,   186,   187,   187,   188,
     189,   189,   189,   189,   189,   189,   189,   189,   190,   191,
     191,   192,   193,   193,   193,   193,   193,   194,   194,   195,
     195,   195,   195,   196,   196,   197,   198,   199,   199,   200,
     201,   201,   202,   202,   203,   204,   204,   204,   205,   205,
     206,   206,   207,   207,   208,   208,   209,   209,   210,   210,
     211,   211,   212,   212,   213,   213,   213,   213,   214,   214,
     215,   215,   216,   216,   217,   217,   218,   218,   218,   218,
     219,   219,   220,   220,   221,   222,   222,   223,   223,   223,
     223,   223,   223,   223,   224,   224,   224,   224,   224,   224,
     224,   225,   225,   225,   226,   226,   226,   226,   226,   226,
     226,   226,   226,   226,   226,   226,   227,   227,   228,   228,
     229,   229,   230,   230,   230,   230,   230,   231,   231,   231,
     231,   231,   231,   231,   231,   231,   231,   231,   232,   233,
     233,   234,   234,   235,   236,   236,   236,   236,   236,   236,
     236,   236,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   237,   237,   237,   237,   237,
     237,   237,   237,   237,   237,   238,   238,   239,   240,   240,
     241,   241,   241,   241,   242,   242,   243,   244,   244,   244,
     244,   245,   245,   245,   245,   246,   246,   246,   246,   246,
     246,   246,   246,   247,   247,   248,   249,   249,   250,   251,
     251,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,   252,   253,   253,   254,   254,   254,   255,   256,
     256,   257,   257,   258,   258,   259,   259,   260,   260,   261,
     261,   262,   262,   262,   262
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     4,     4,     7,     6,
       7,    11,     1,     3,     1,     1,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     4,     1,     6,     4,     1,     4,     4,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     1,     2,     2,     1,     1,     2,
       5,     4,     1,     3,     4,     6,     5,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     0,     5,     1,
       3,     3,     4,     4,     4,     4,     6,     8,     8,     1,
       1,     3,     3,     3,     3,     2,     4,     3,     3,     8,
       3,     0,     1,     3,     2,     1,     1,     0,     2,     0,
       2,     0,     1,     0,     2,     0,     2,     0,     2,     0,
       2,     0,     3,     0,     1,     2,     1,     1,     1,     3,
       1,     1,     2,     4,     1,     3,     2,     1,     5,     0,
       2,     0,     1,     3,     5,     4,     6,     1,     1,     1,
       1,     1,     1,     0,     2,     2,     2,     2,     2,     2,
       3,     2,     2,     2,     2,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     1,     3,     3,     5,
       3,     1,     1,     1,     1,     1,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,    10,     6,
       8,     4,     6,     6,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     4,     5,     4,     3,     2,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     6,     3,     4,     3,     3,     5,     5,     6,
       4,     6,     3,     5,     4,     5,     6,     4,     5,     5,
       6,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       2,     2,     1,     1,     1,     2,     2,     3,     2,     2,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     1,     3,     2,     2,     1,     1,     2,
       0,     3,     0,     1,     0,     2,     0,     4,     0,     1,
       3,     1,     3,     3,     3
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
#line 284 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1966 "parser.cpp"
        break;

    case YYSYMBOL_STRING: /* STRING  */
#line 284 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 1974 "parser.cpp"
        break;

    case YYSYMBOL_statement_list: /* statement_list  */
#line 198 "parser.y"
            {
    fprintf(stderr, "destroy statement array\n");
    if ((((*yyvaluep).stmt_array)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).stmt_array))) {
            delete ptr;
        }
        delete (((*yyvaluep).stmt_array));
    }
}
#line 1988 "parser.cpp"
        break;

    case YYSYMBOL_table_element_array: /* table_element_array  */
#line 188 "parser.y"
            {
    fprintf(stderr, "destroy table element array\n");
    if ((((*yyvaluep).table_element_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).table_element_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).table_element_array_t));
    }
}
#line 2002 "parser.cpp"
        break;

    case YYSYMBOL_column_constraints: /* column_constraints  */
#line 277 "parser.y"
            {
    fprintf(stderr, "destroy constraints\n");
    if ((((*yyvaluep).column_constraints_t)) != nullptr) {
        delete (((*yyvaluep).column_constraints_t));
    }
}
#line 2013 "parser.cpp"
        break;

    case YYSYMBOL_identifier_array: /* identifier_array  */
#line 288 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2022 "parser.cpp"
        break;

    case YYSYMBOL_optional_identifier_array: /* optional_identifier_array  */
#line 288 "parser.y"
            {
    fprintf(stderr, "destroy identifier array\n");
    delete (((*yyvaluep).identifier_array_t));
}
#line 2031 "parser.cpp"
        break;

    case YYSYMBOL_update_expr_array: /* update_expr_array  */
#line 248 "parser.y"
            {
    fprintf(stderr, "destroy update expr array\n");
    if ((((*yyvaluep).update_expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).update_expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).update_expr_array_t));
    }
}
#line 2045 "parser.cpp"
        break;

    case YYSYMBOL_update_expr: /* update_expr  */
#line 241 "parser.y"
            {
    fprintf(stderr, "destroy update expr\n");
    if(((*yyvaluep).update_expr_t) != nullptr) {
        delete ((*yyvaluep).update_expr_t);
    }
}
#line 2056 "parser.cpp"
        break;

    case YYSYMBOL_select_statement: /* select_statement  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2066 "parser.cpp"
        break;

    case YYSYMBOL_select_with_paren: /* select_with_paren  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2076 "parser.cpp"
        break;

    case YYSYMBOL_select_without_paren: /* select_without_paren  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2086 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_with_modifier: /* select_clause_with_modifier  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2096 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier_paren: /* select_clause_without_modifier_paren  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2106 "parser.cpp"
        break;

    case YYSYMBOL_select_clause_without_modifier: /* select_clause_without_modifier  */
#line 323 "parser.y"
            {
    if(((*yyvaluep).select_stmt) != nullptr) {
        delete ((*yyvaluep).select_stmt);
    }
}
#line 2116 "parser.cpp"
        break;

    case YYSYMBOL_order_by_clause: /* order_by_clause  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2130 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr_list: /* order_by_expr_list  */
#line 231 "parser.y"
            {
    fprintf(stderr, "destroy order by expr list\n");
    if ((((*yyvaluep).order_by_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).order_by_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).order_by_expr_list_t));
    }
}
#line 2144 "parser.cpp"
        break;

    case YYSYMBOL_order_by_expr: /* order_by_expr  */
#line 311 "parser.y"
            {
    fprintf(stderr, "destroy order by expr\n");
    delete ((*yyvaluep).order_by_expr_t)->expr_;
    delete ((*yyvaluep).order_by_expr_t);
}
#line 2154 "parser.cpp"
        break;

    case YYSYMBOL_limit_expr: /* limit_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2162 "parser.cpp"
        break;

    case YYSYMBOL_offset_expr: /* offset_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2170 "parser.cpp"
        break;

    case YYSYMBOL_from_clause: /* from_clause  */
#line 306 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2179 "parser.cpp"
        break;

    case YYSYMBOL_search_clause: /* search_clause  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2187 "parser.cpp"
        break;

    case YYSYMBOL_where_clause: /* where_clause  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2195 "parser.cpp"
        break;

    case YYSYMBOL_having_clause: /* having_clause  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2203 "parser.cpp"
        break;

    case YYSYMBOL_group_by_clause: /* group_by_clause  */
#line 208 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2217 "parser.cpp"
        break;

    case YYSYMBOL_table_reference: /* table_reference  */
#line 306 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2226 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_unit: /* table_reference_unit  */
#line 306 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2235 "parser.cpp"
        break;

    case YYSYMBOL_table_reference_name: /* table_reference_name  */
#line 306 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2244 "parser.cpp"
        break;

    case YYSYMBOL_table_name: /* table_name  */
#line 268 "parser.y"
            {
    fprintf(stderr, "destroy table table_name\n");
    if ((((*yyvaluep).table_name_t)) != nullptr) {
        free(((*yyvaluep).table_name_t)->schema_name_ptr_);
        free(((*yyvaluep).table_name_t)->table_name_ptr_);
        delete (((*yyvaluep).table_name_t));
    }
}
#line 2257 "parser.cpp"
        break;

    case YYSYMBOL_table_alias: /* table_alias  */
#line 301 "parser.y"
            {
    fprintf(stderr, "destroy table alias\n");
    delete (((*yyvaluep).table_alias_t));
}
#line 2266 "parser.cpp"
        break;

    case YYSYMBOL_with_clause: /* with_clause  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2280 "parser.cpp"
        break;

    case YYSYMBOL_with_expr_list: /* with_expr_list  */
#line 258 "parser.y"
            {
    fprintf(stderr, "destroy with expr list\n");
    if ((((*yyvaluep).with_expr_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_expr_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_expr_list_t));
    }
}
#line 2294 "parser.cpp"
        break;

    case YYSYMBOL_with_expr: /* with_expr  */
#line 317 "parser.y"
            {
    fprintf(stderr, "destroy with expr\n");
    delete ((*yyvaluep).with_expr_t)->select_;
    delete ((*yyvaluep).with_expr_t);
}
#line 2304 "parser.cpp"
        break;

    case YYSYMBOL_join_clause: /* join_clause  */
#line 306 "parser.y"
            {
    fprintf(stderr, "destroy table reference\n");
    delete (((*yyvaluep).table_reference_t));
}
#line 2313 "parser.cpp"
        break;

    case YYSYMBOL_expr_array: /* expr_array  */
#line 208 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2327 "parser.cpp"
        break;

    case YYSYMBOL_expr_array_list: /* expr_array_list  */
#line 218 "parser.y"
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
#line 2344 "parser.cpp"
        break;

    case YYSYMBOL_expr_alias: /* expr_alias  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2352 "parser.cpp"
        break;

    case YYSYMBOL_expr: /* expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2360 "parser.cpp"
        break;

    case YYSYMBOL_operand: /* operand  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2368 "parser.cpp"
        break;

    case YYSYMBOL_knn_expr: /* knn_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2376 "parser.cpp"
        break;

    case YYSYMBOL_match_expr: /* match_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2384 "parser.cpp"
        break;

    case YYSYMBOL_query_expr: /* query_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2392 "parser.cpp"
        break;

    case YYSYMBOL_fusion_expr: /* fusion_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2400 "parser.cpp"
        break;

    case YYSYMBOL_sub_search_array: /* sub_search_array  */
#line 208 "parser.y"
            {
    fprintf(stderr, "destroy expression array\n");
    if ((((*yyvaluep).expr_array_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).expr_array_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).expr_array_t));
    }
}
#line 2414 "parser.cpp"
        break;

    case YYSYMBOL_function_expr: /* function_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2422 "parser.cpp"
        break;

    case YYSYMBOL_conjunction_expr: /* conjunction_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2430 "parser.cpp"
        break;

    case YYSYMBOL_between_expr: /* between_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2438 "parser.cpp"
        break;

    case YYSYMBOL_in_expr: /* in_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2446 "parser.cpp"
        break;

    case YYSYMBOL_case_expr: /* case_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2454 "parser.cpp"
        break;

    case YYSYMBOL_case_check_array: /* case_check_array  */
#line 329 "parser.y"
            {
    fprintf(stderr, "destroy case check array\n");
    if(((*yyvaluep).case_check_array_t) != nullptr) {
        for(auto ptr: *(((*yyvaluep).case_check_array_t))) {
            delete ptr;
        }
    }
}
#line 2467 "parser.cpp"
        break;

    case YYSYMBOL_cast_expr: /* cast_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2475 "parser.cpp"
        break;

    case YYSYMBOL_subquery_expr: /* subquery_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2483 "parser.cpp"
        break;

    case YYSYMBOL_column_expr: /* column_expr  */
#line 293 "parser.y"
            {
    delete (((*yyvaluep).expr_t));
}
#line 2491 "parser.cpp"
        break;

    case YYSYMBOL_constant_expr: /* constant_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2499 "parser.cpp"
        break;

    case YYSYMBOL_array_expr: /* array_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2507 "parser.cpp"
        break;

    case YYSYMBOL_long_array_expr: /* long_array_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2515 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_long_array_expr: /* unclosed_long_array_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2523 "parser.cpp"
        break;

    case YYSYMBOL_double_array_expr: /* double_array_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2531 "parser.cpp"
        break;

    case YYSYMBOL_unclosed_double_array_expr: /* unclosed_double_array_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2539 "parser.cpp"
        break;

    case YYSYMBOL_interval_expr: /* interval_expr  */
#line 297 "parser.y"
            {
    delete (((*yyvaluep).const_expr_t));
}
#line 2547 "parser.cpp"
        break;

    case YYSYMBOL_file_path: /* file_path  */
#line 284 "parser.y"
            {
    free(((*yyvaluep).str_value));
}
#line 2555 "parser.cpp"
        break;

    case YYSYMBOL_if_not_exists_info: /* if_not_exists_info  */
#line 181 "parser.y"
            {
    fprintf(stderr, "destroy if not exists info\n");
    if ((((*yyvaluep).if_not_exists_info_t)) != nullptr) {
        delete (((*yyvaluep).if_not_exists_info_t));
    }
}
#line 2566 "parser.cpp"
        break;

    case YYSYMBOL_with_index_para_list: /* with_index_para_list  */
#line 164 "parser.y"
            {
    fprintf(stderr, "destroy create index para list\n");
    if ((((*yyvaluep).with_index_para_list_t)) != nullptr) {
        for (auto ptr : *(((*yyvaluep).with_index_para_list_t))) {
            delete ptr;
        }
        delete (((*yyvaluep).with_index_para_list_t));
    }
}
#line 2580 "parser.cpp"
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

#line 2688 "parser.cpp"

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
#line 449 "parser.y"
                                         {
    result->statements_ptr_ = (yyvsp[-1].stmt_array);
}
#line 2903 "parser.cpp"
    break;

  case 3: /* statement_list: statement  */
#line 453 "parser.y"
                           {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyval.stmt_array) = new std::vector<infinity::BaseStatement*>();
    (yyval.stmt_array)->push_back((yyvsp[0].base_stmt));
}
#line 2914 "parser.cpp"
    break;

  case 4: /* statement_list: statement_list ';' statement  */
#line 459 "parser.y"
                               {
    (yyvsp[0].base_stmt)->stmt_length_ = yylloc.string_length;
    yylloc.string_length = 0;
    (yyvsp[-2].stmt_array)->push_back((yyvsp[0].base_stmt));
    (yyval.stmt_array) = (yyvsp[-2].stmt_array);
}
#line 2925 "parser.cpp"
    break;

  case 5: /* statement: create_statement  */
#line 466 "parser.y"
                             { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2931 "parser.cpp"
    break;

  case 6: /* statement: drop_statement  */
#line 467 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 2937 "parser.cpp"
    break;

  case 7: /* statement: copy_statement  */
#line 468 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 2943 "parser.cpp"
    break;

  case 8: /* statement: show_statement  */
#line 469 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 2949 "parser.cpp"
    break;

  case 9: /* statement: select_statement  */
#line 470 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 2955 "parser.cpp"
    break;

  case 10: /* statement: delete_statement  */
#line 471 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 2961 "parser.cpp"
    break;

  case 11: /* statement: update_statement  */
#line 472 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 2967 "parser.cpp"
    break;

  case 12: /* statement: insert_statement  */
#line 473 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 2973 "parser.cpp"
    break;

  case 13: /* statement: explain_statement  */
#line 474 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].explain_stmt); }
#line 2979 "parser.cpp"
    break;

  case 14: /* statement: flush_statement  */
#line 475 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 2985 "parser.cpp"
    break;

  case 15: /* statement: command_statement  */
#line 476 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 2991 "parser.cpp"
    break;

  case 16: /* explainable_statement: create_statement  */
#line 478 "parser.y"
                                         { (yyval.base_stmt) = (yyvsp[0].create_stmt); }
#line 2997 "parser.cpp"
    break;

  case 17: /* explainable_statement: drop_statement  */
#line 479 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].drop_stmt); }
#line 3003 "parser.cpp"
    break;

  case 18: /* explainable_statement: copy_statement  */
#line 480 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].copy_stmt); }
#line 3009 "parser.cpp"
    break;

  case 19: /* explainable_statement: show_statement  */
#line 481 "parser.y"
                 { (yyval.base_stmt) = (yyvsp[0].show_stmt); }
#line 3015 "parser.cpp"
    break;

  case 20: /* explainable_statement: select_statement  */
#line 482 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].select_stmt); }
#line 3021 "parser.cpp"
    break;

  case 21: /* explainable_statement: delete_statement  */
#line 483 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].delete_stmt); }
#line 3027 "parser.cpp"
    break;

  case 22: /* explainable_statement: update_statement  */
#line 484 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].update_stmt); }
#line 3033 "parser.cpp"
    break;

  case 23: /* explainable_statement: insert_statement  */
#line 485 "parser.y"
                   { (yyval.base_stmt) = (yyvsp[0].insert_stmt); }
#line 3039 "parser.cpp"
    break;

  case 24: /* explainable_statement: flush_statement  */
#line 486 "parser.y"
                  { (yyval.base_stmt) = (yyvsp[0].flush_stmt); }
#line 3045 "parser.cpp"
    break;

  case 25: /* explainable_statement: command_statement  */
#line 487 "parser.y"
                    { (yyval.base_stmt) = (yyvsp[0].command_stmt); }
#line 3051 "parser.cpp"
    break;

  case 26: /* create_statement: CREATE DATABASE if_not_exists IDENTIFIER  */
#line 494 "parser.y"
                                                            {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateSchemaInfo> create_schema_info = std::make_shared<infinity::CreateSchemaInfo>();

    ParserHelper::ToLower((yyvsp[0].str_value));
    create_schema_info->schema_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));

    (yyval.create_stmt)->create_info_ = create_schema_info;
    (yyval.create_stmt)->create_info_->conflict_type_ = (yyvsp[-1].bool_value) ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
}
#line 3067 "parser.cpp"
    break;

  case 27: /* create_statement: CREATE COLLECTION if_not_exists table_name  */
#line 507 "parser.y"
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
#line 3085 "parser.cpp"
    break;

  case 28: /* create_statement: CREATE TABLE if_not_exists table_name '(' table_element_array ')'  */
#line 522 "parser.y"
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
#line 3113 "parser.cpp"
    break;

  case 29: /* create_statement: CREATE TABLE if_not_exists table_name AS select_statement  */
#line 546 "parser.y"
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
#line 3133 "parser.cpp"
    break;

  case 30: /* create_statement: CREATE VIEW if_not_exists table_name optional_identifier_array AS select_statement  */
#line 562 "parser.y"
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
#line 3154 "parser.cpp"
    break;

  case 31: /* create_statement: CREATE INDEX if_not_exists_info ON table_name '(' identifier_array ')' USING IDENTIFIER with_index_para_list  */
#line 580 "parser.y"
                                                                                                               {
    (yyval.create_stmt) = new infinity::CreateStatement();
    std::shared_ptr<infinity::CreateIndexInfo> create_index_info = std::make_shared<infinity::CreateIndexInfo>();
    if((yyvsp[-6].table_name_t)->schema_name_ptr_ != nullptr) {
        create_index_info->schema_name_ = (yyvsp[-6].table_name_t)->schema_name_ptr_;
        free((yyvsp[-6].table_name_t)->schema_name_ptr_);
    }
    create_index_info->table_name_ = (yyvsp[-6].table_name_t)->table_name_ptr_;
    free((yyvsp[-6].table_name_t)->table_name_ptr_);
    delete (yyvsp[-6].table_name_t);

    create_index_info->index_name_ = (yyvsp[-8].if_not_exists_info_t)->info_;

    create_index_info->column_names_ = (yyvsp[-4].identifier_array_t);
    if ((yyvsp[-8].if_not_exists_info_t)->exists_) {
        create_index_info->conflict_type_ = (yyvsp[-8].if_not_exists_info_t)->if_not_exists_ ? infinity::ConflictType::kIgnore : infinity::ConflictType::kError;
    } else {
        create_index_info->conflict_type_ = infinity::ConflictType::kIgnore;
    }
    create_index_info->method_type_ = (yyvsp[-1].str_value);
    free((yyvsp[-1].str_value));
    delete (yyvsp[-8].if_not_exists_info_t);

    create_index_info->index_para_list_ = (yyvsp[0].with_index_para_list_t);
    (yyval.create_stmt)->create_info_ = create_index_info;
}
#line 3185 "parser.cpp"
    break;

  case 32: /* table_element_array: table_element  */
#line 607 "parser.y"
                                    {
    (yyval.table_element_array_t) = new std::vector<infinity::TableElement*>();
    (yyval.table_element_array_t)->push_back((yyvsp[0].table_element_t));
}
#line 3194 "parser.cpp"
    break;

  case 33: /* table_element_array: table_element_array ',' table_element  */
#line 611 "parser.y"
                                        {
    (yyvsp[-2].table_element_array_t)->push_back((yyvsp[0].table_element_t));
    (yyval.table_element_array_t) = (yyvsp[-2].table_element_array_t);
}
#line 3203 "parser.cpp"
    break;

  case 34: /* table_element: table_column  */
#line 617 "parser.y"
                             {
    (yyval.table_element_t) = (yyvsp[0].table_column_t);
}
#line 3211 "parser.cpp"
    break;

  case 35: /* table_element: table_constraint  */
#line 620 "parser.y"
                   {
    (yyval.table_element_t) = (yyvsp[0].table_constraint_t);
}
#line 3219 "parser.cpp"
    break;

  case 36: /* table_column: IDENTIFIER column_type  */
#line 626 "parser.y"
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
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[0].column_type_t).width);
            break;
        }
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
#line 3259 "parser.cpp"
    break;

  case 37: /* table_column: IDENTIFIER column_type column_constraints  */
#line 661 "parser.y"
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
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
#line 3296 "parser.cpp"
    break;

  case 38: /* column_type: BOOLEAN  */
#line 695 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBoolean}; }
#line 3302 "parser.cpp"
    break;

  case 39: /* column_type: TINYINT  */
#line 696 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTinyInt}; }
#line 3308 "parser.cpp"
    break;

  case 40: /* column_type: SMALLINT  */
#line 697 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kSmallInt}; }
#line 3314 "parser.cpp"
    break;

  case 41: /* column_type: INTEGER  */
#line 698 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3320 "parser.cpp"
    break;

  case 42: /* column_type: INT  */
#line 699 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kInteger}; }
#line 3326 "parser.cpp"
    break;

  case 43: /* column_type: BIGINT  */
#line 700 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBigInt}; }
#line 3332 "parser.cpp"
    break;

  case 44: /* column_type: HUGEINT  */
#line 701 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kHugeInt}; }
#line 3338 "parser.cpp"
    break;

  case 45: /* column_type: FLOAT  */
#line 702 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3344 "parser.cpp"
    break;

  case 46: /* column_type: REAL  */
#line 703 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kFloat}; }
#line 3350 "parser.cpp"
    break;

  case 47: /* column_type: DOUBLE  */
#line 704 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDouble}; }
#line 3356 "parser.cpp"
    break;

  case 48: /* column_type: DATE  */
#line 705 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDate}; }
#line 3362 "parser.cpp"
    break;

  case 49: /* column_type: TIME  */
#line 706 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTime}; }
#line 3368 "parser.cpp"
    break;

  case 50: /* column_type: DATETIME  */
#line 707 "parser.y"
           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDateTime}; }
#line 3374 "parser.cpp"
    break;

  case 51: /* column_type: TIMESTAMP  */
#line 708 "parser.y"
            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kTimestamp}; }
#line 3380 "parser.cpp"
    break;

  case 52: /* column_type: UUID  */
#line 709 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kUuid}; }
#line 3386 "parser.cpp"
    break;

  case 53: /* column_type: POINT  */
#line 710 "parser.y"
        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPoint}; }
#line 3392 "parser.cpp"
    break;

  case 54: /* column_type: LINE  */
#line 711 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLine}; }
#line 3398 "parser.cpp"
    break;

  case 55: /* column_type: LSEG  */
#line 712 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kLineSeg}; }
#line 3404 "parser.cpp"
    break;

  case 56: /* column_type: BOX  */
#line 713 "parser.y"
      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBox}; }
#line 3410 "parser.cpp"
    break;

  case 57: /* column_type: PATH  */
#line 714 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPath}; }
#line 3416 "parser.cpp"
    break;

  case 58: /* column_type: POLYGON  */
#line 715 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kPolygon}; }
#line 3422 "parser.cpp"
    break;

  case 59: /* column_type: CIRCLE  */
#line 716 "parser.y"
         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kCircle}; }
#line 3428 "parser.cpp"
    break;

  case 60: /* column_type: CHAR  */
#line 718 "parser.y"
       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3434 "parser.cpp"
    break;

  case 61: /* column_type: CHAR '(' LONG_VALUE ')'  */
#line 719 "parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar, (yyvsp[-1].long_value)}; }
#line 3440 "parser.cpp"
    break;

  case 62: /* column_type: VARCHAR  */
#line 720 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kVarchar}; }
#line 3446 "parser.cpp"
    break;

  case 63: /* column_type: DECIMAL '(' LONG_VALUE ',' LONG_VALUE ')'  */
#line 721 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-3].long_value), (yyvsp[-1].long_value)}; }
#line 3452 "parser.cpp"
    break;

  case 64: /* column_type: DECIMAL '(' LONG_VALUE ')'  */
#line 722 "parser.y"
                             { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, (yyvsp[-1].long_value), 0}; }
#line 3458 "parser.cpp"
    break;

  case 65: /* column_type: DECIMAL  */
#line 723 "parser.y"
          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kDecimal, 0, 0, 0}; }
#line 3464 "parser.cpp"
    break;

  case 66: /* column_type: BLOB '(' LONG_VALUE ')'  */
#line 724 "parser.y"
                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBlob, (yyvsp[-1].long_value)}; }
#line 3470 "parser.cpp"
    break;

  case 67: /* column_type: BITMAP '(' LONG_VALUE ')'  */
#line 725 "parser.y"
                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kBitmap, (yyvsp[-1].long_value)}; }
#line 3476 "parser.cpp"
    break;

  case 68: /* column_type: EMBEDDING '(' BIT ',' LONG_VALUE ')'  */
#line 726 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3482 "parser.cpp"
    break;

  case 69: /* column_type: EMBEDDING '(' TINYINT ',' LONG_VALUE ')'  */
#line 727 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3488 "parser.cpp"
    break;

  case 70: /* column_type: EMBEDDING '(' SMALLINT ',' LONG_VALUE ')'  */
#line 728 "parser.y"
                                            { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3494 "parser.cpp"
    break;

  case 71: /* column_type: EMBEDDING '(' INTEGER ',' LONG_VALUE ')'  */
#line 729 "parser.y"
                                           { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3500 "parser.cpp"
    break;

  case 72: /* column_type: EMBEDDING '(' INT ',' LONG_VALUE ')'  */
#line 730 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3506 "parser.cpp"
    break;

  case 73: /* column_type: EMBEDDING '(' BIGINT ',' LONG_VALUE ')'  */
#line 731 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3512 "parser.cpp"
    break;

  case 74: /* column_type: EMBEDDING '(' FLOAT ',' LONG_VALUE ')'  */
#line 732 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3518 "parser.cpp"
    break;

  case 75: /* column_type: EMBEDDING '(' DOUBLE ',' LONG_VALUE ')'  */
#line 733 "parser.y"
                                          { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3524 "parser.cpp"
    break;

  case 76: /* column_type: VECTOR '(' BIT ',' LONG_VALUE ')'  */
#line 734 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemBit}; }
#line 3530 "parser.cpp"
    break;

  case 77: /* column_type: VECTOR '(' TINYINT ',' LONG_VALUE ')'  */
#line 735 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt8}; }
#line 3536 "parser.cpp"
    break;

  case 78: /* column_type: VECTOR '(' SMALLINT ',' LONG_VALUE ')'  */
#line 736 "parser.y"
                                         { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt16}; }
#line 3542 "parser.cpp"
    break;

  case 79: /* column_type: VECTOR '(' INTEGER ',' LONG_VALUE ')'  */
#line 737 "parser.y"
                                        { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3548 "parser.cpp"
    break;

  case 80: /* column_type: VECTOR '(' INT ',' LONG_VALUE ')'  */
#line 738 "parser.y"
                                    { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt32}; }
#line 3554 "parser.cpp"
    break;

  case 81: /* column_type: VECTOR '(' BIGINT ',' LONG_VALUE ')'  */
#line 739 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemInt64}; }
#line 3560 "parser.cpp"
    break;

  case 82: /* column_type: VECTOR '(' FLOAT ',' LONG_VALUE ')'  */
#line 740 "parser.y"
                                      { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemFloat}; }
#line 3566 "parser.cpp"
    break;

  case 83: /* column_type: VECTOR '(' DOUBLE ',' LONG_VALUE ')'  */
#line 741 "parser.y"
                                       { (yyval.column_type_t) = infinity::ColumnType{infinity::LogicalType::kEmbedding, (yyvsp[-1].long_value), 0, 0, infinity::kElemDouble}; }
#line 3572 "parser.cpp"
    break;

  case 84: /* column_constraints: column_constraint  */
#line 760 "parser.y"
                                       {
    (yyval.column_constraints_t) = new std::unordered_set<infinity::ConstraintType>();
    (yyval.column_constraints_t)->insert((yyvsp[0].column_constraint_t));
}
#line 3581 "parser.cpp"
    break;

  case 85: /* column_constraints: column_constraints column_constraint  */
#line 764 "parser.y"
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

  case 86: /* column_constraint: PRIMARY KEY  */
#line 774 "parser.y"
                                {
    (yyval.column_constraint_t) = infinity::ConstraintType::kPrimaryKey;
}
#line 3603 "parser.cpp"
    break;

  case 87: /* column_constraint: UNIQUE  */
#line 777 "parser.y"
         {
    (yyval.column_constraint_t) = infinity::ConstraintType::kUnique;
}
#line 3611 "parser.cpp"
    break;

  case 88: /* column_constraint: NULLABLE  */
#line 780 "parser.y"
           {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNull;
}
#line 3619 "parser.cpp"
    break;

  case 89: /* column_constraint: NOT NULLABLE  */
#line 783 "parser.y"
               {
    (yyval.column_constraint_t) = infinity::ConstraintType::kNotNull;
}
#line 3627 "parser.cpp"
    break;

  case 90: /* table_constraint: PRIMARY KEY '(' identifier_array ')'  */
#line 787 "parser.y"
                                                        {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kPrimaryKey;
}
#line 3637 "parser.cpp"
    break;

  case 91: /* table_constraint: UNIQUE '(' identifier_array ')'  */
#line 792 "parser.y"
                                  {
    (yyval.table_constraint_t) = new infinity::TableConstraint();
    (yyval.table_constraint_t)->names_ptr_ = (yyvsp[-1].identifier_array_t);
    (yyval.table_constraint_t)->constraint_ = infinity::ConstraintType::kUnique;
}
#line 3647 "parser.cpp"
    break;

  case 92: /* identifier_array: IDENTIFIER  */
#line 799 "parser.y"
                              {
    (yyval.identifier_array_t) = new std::vector<std::string>();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 3658 "parser.cpp"
    break;

  case 93: /* identifier_array: identifier_array ',' IDENTIFIER  */
#line 805 "parser.y"
                                  {
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyvsp[-2].identifier_array_t)->emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.identifier_array_t) = (yyvsp[-2].identifier_array_t);
}
#line 3669 "parser.cpp"
    break;

  case 94: /* delete_statement: DELETE FROM table_name where_clause  */
#line 815 "parser.y"
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

  case 95: /* insert_statement: INSERT INTO table_name optional_identifier_array VALUES expr_array_list  */
#line 831 "parser.y"
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

  case 96: /* insert_statement: INSERT INTO table_name optional_identifier_array select_without_paren  */
#line 843 "parser.y"
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

  case 97: /* optional_identifier_array: '(' identifier_array ')'  */
#line 856 "parser.y"
                                                    {
    (yyval.identifier_array_t) = (yyvsp[-1].identifier_array_t);
}
#line 3728 "parser.cpp"
    break;

  case 98: /* optional_identifier_array: %empty  */
#line 859 "parser.y"
  {
    (yyval.identifier_array_t) = nullptr;
}
#line 3736 "parser.cpp"
    break;

  case 99: /* explain_statement: EXPLAIN explain_type explainable_statement  */
#line 866 "parser.y"
                                                               {
    (yyval.explain_stmt) = new infinity::ExplainStatement();
    (yyval.explain_stmt)->type_ = (yyvsp[-1].explain_type_t);
    (yyval.explain_stmt)->statement_ = (yyvsp[0].base_stmt);
}
#line 3746 "parser.cpp"
    break;

  case 100: /* explain_type: ANALYZE  */
#line 872 "parser.y"
                      {
    (yyval.explain_type_t) = infinity::ExplainType::kAnalyze;
}
#line 3754 "parser.cpp"
    break;

  case 101: /* explain_type: AST  */
#line 875 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kAst;
}
#line 3762 "parser.cpp"
    break;

  case 102: /* explain_type: RAW  */
#line 878 "parser.y"
      {
    (yyval.explain_type_t) = infinity::ExplainType::kUnOpt;
}
#line 3770 "parser.cpp"
    break;

  case 103: /* explain_type: LOGICAL  */
#line 881 "parser.y"
          {
    (yyval.explain_type_t) = infinity::ExplainType::kOpt;
}
#line 3778 "parser.cpp"
    break;

  case 104: /* explain_type: PHYSICAL  */
#line 884 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3786 "parser.cpp"
    break;

  case 105: /* explain_type: PIPELINE  */
#line 887 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kPipeline;
}
#line 3794 "parser.cpp"
    break;

  case 106: /* explain_type: FRAGMENT  */
#line 890 "parser.y"
           {
    (yyval.explain_type_t) = infinity::ExplainType::kFragment;
}
#line 3802 "parser.cpp"
    break;

  case 107: /* explain_type: %empty  */
#line 893 "parser.y"
  {
    (yyval.explain_type_t) = infinity::ExplainType::kPhysical;
}
#line 3810 "parser.cpp"
    break;

  case 108: /* update_statement: UPDATE table_name SET update_expr_array where_clause  */
#line 900 "parser.y"
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

  case 109: /* update_expr_array: update_expr  */
#line 913 "parser.y"
                               {
    (yyval.update_expr_array_t) = new std::vector<infinity::UpdateExpr*>();
    (yyval.update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
}
#line 3836 "parser.cpp"
    break;

  case 110: /* update_expr_array: update_expr_array ',' update_expr  */
#line 917 "parser.y"
                                    {
    (yyvsp[-2].update_expr_array_t)->emplace_back((yyvsp[0].update_expr_t));
    (yyval.update_expr_array_t) = (yyvsp[-2].update_expr_array_t);
}
#line 3845 "parser.cpp"
    break;

  case 111: /* update_expr: IDENTIFIER '=' expr  */
#line 922 "parser.y"
                                  {
    (yyval.update_expr_t) = new infinity::UpdateExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    (yyval.update_expr_t)->column_name = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    (yyval.update_expr_t)->value = (yyvsp[0].expr_t);
}
#line 3857 "parser.cpp"
    break;

  case 112: /* drop_statement: DROP DATABASE if_exists IDENTIFIER  */
#line 935 "parser.y"
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

  case 113: /* drop_statement: DROP COLLECTION if_exists table_name  */
#line 948 "parser.y"
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

  case 114: /* drop_statement: DROP TABLE if_exists table_name  */
#line 963 "parser.y"
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

  case 115: /* drop_statement: DROP VIEW if_exists table_name  */
#line 978 "parser.y"
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

  case 116: /* drop_statement: DROP INDEX if_exists IDENTIFIER ON table_name  */
#line 993 "parser.y"
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

  case 117: /* copy_statement: COPY table_name TO file_path WITH '(' copy_option_list ')'  */
#line 1016 "parser.y"
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

  case 118: /* copy_statement: COPY table_name FROM file_path WITH '(' copy_option_list ')'  */
#line 1057 "parser.y"
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

  case 119: /* select_statement: select_without_paren  */
#line 1102 "parser.y"
                                        {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4050 "parser.cpp"
    break;

  case 120: /* select_statement: select_with_paren  */
#line 1105 "parser.y"
                    {
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4058 "parser.cpp"
    break;

  case 121: /* select_statement: select_statement set_operator select_clause_without_modifier_paren  */
#line 1108 "parser.y"
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

  case 122: /* select_statement: select_statement set_operator select_clause_without_modifier  */
#line 1117 "parser.y"
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

  case 123: /* select_with_paren: '(' select_without_paren ')'  */
#line 1127 "parser.y"
                                                 {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4094 "parser.cpp"
    break;

  case 124: /* select_with_paren: '(' select_with_paren ')'  */
#line 1130 "parser.y"
                            {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4102 "parser.cpp"
    break;

  case 125: /* select_without_paren: with_clause select_clause_with_modifier  */
#line 1134 "parser.y"
                                                              {
    (yyvsp[0].select_stmt)->with_exprs_ = (yyvsp[-1].with_expr_list_t);
    (yyval.select_stmt) = (yyvsp[0].select_stmt);
}
#line 4111 "parser.cpp"
    break;

  case 126: /* select_clause_with_modifier: select_clause_without_modifier order_by_clause limit_expr offset_expr  */
#line 1139 "parser.y"
                                                                                                   {
    (yyvsp[-3].select_stmt)->order_by_list = (yyvsp[-2].order_by_expr_list_t);
    (yyvsp[-3].select_stmt)->limit_expr_ = (yyvsp[-1].expr_t);
    (yyvsp[-3].select_stmt)->offset_expr_ = (yyvsp[0].expr_t);
    (yyval.select_stmt) = (yyvsp[-3].select_stmt);
}
#line 4122 "parser.cpp"
    break;

  case 127: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier ')'  */
#line 1146 "parser.y"
                                                                             {
  (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4130 "parser.cpp"
    break;

  case 128: /* select_clause_without_modifier_paren: '(' select_clause_without_modifier_paren ')'  */
#line 1149 "parser.y"
                                               {
    (yyval.select_stmt) = (yyvsp[-1].select_stmt);
}
#line 4138 "parser.cpp"
    break;

  case 129: /* select_clause_without_modifier: SELECT distinct expr_array from_clause search_clause where_clause group_by_clause having_clause  */
#line 1154 "parser.y"
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

  case 130: /* order_by_clause: ORDER BY order_by_expr_list  */
#line 1170 "parser.y"
                                              {
    (yyval.order_by_expr_list_t) = (yyvsp[0].order_by_expr_list_t);
}
#line 4166 "parser.cpp"
    break;

  case 131: /* order_by_clause: %empty  */
#line 1173 "parser.y"
                       {
    (yyval.order_by_expr_list_t) = nullptr;
}
#line 4174 "parser.cpp"
    break;

  case 132: /* order_by_expr_list: order_by_expr  */
#line 1177 "parser.y"
                                  {
    (yyval.order_by_expr_list_t) = new std::vector<infinity::OrderByExpr*>();
    (yyval.order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
}
#line 4183 "parser.cpp"
    break;

  case 133: /* order_by_expr_list: order_by_expr_list ',' order_by_expr  */
#line 1181 "parser.y"
                                       {
    (yyvsp[-2].order_by_expr_list_t)->emplace_back((yyvsp[0].order_by_expr_t));
    (yyval.order_by_expr_list_t) = (yyvsp[-2].order_by_expr_list_t);
}
#line 4192 "parser.cpp"
    break;

  case 134: /* order_by_expr: expr order_by_type  */
#line 1186 "parser.y"
                                   {
    (yyval.order_by_expr_t) = new infinity::OrderByExpr();
    (yyval.order_by_expr_t)->expr_ = (yyvsp[-1].expr_t);
    (yyval.order_by_expr_t)->type_ = (yyvsp[0].order_by_type_t);
}
#line 4202 "parser.cpp"
    break;

  case 135: /* order_by_type: ASC  */
#line 1192 "parser.y"
                   {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4210 "parser.cpp"
    break;

  case 136: /* order_by_type: DESC  */
#line 1195 "parser.y"
       {
    (yyval.order_by_type_t) = infinity::kDesc;
}
#line 4218 "parser.cpp"
    break;

  case 137: /* order_by_type: %empty  */
#line 1198 "parser.y"
  {
    (yyval.order_by_type_t) = infinity::kAsc;
}
#line 4226 "parser.cpp"
    break;

  case 138: /* limit_expr: LIMIT expr  */
#line 1202 "parser.y"
                       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4234 "parser.cpp"
    break;

  case 139: /* limit_expr: %empty  */
#line 1206 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4240 "parser.cpp"
    break;

  case 140: /* offset_expr: OFFSET expr  */
#line 1208 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4248 "parser.cpp"
    break;

  case 141: /* offset_expr: %empty  */
#line 1212 "parser.y"
{   (yyval.expr_t) = nullptr; }
#line 4254 "parser.cpp"
    break;

  case 142: /* distinct: DISTINCT  */
#line 1214 "parser.y"
                    {
    (yyval.bool_value) = true;
}
#line 4262 "parser.cpp"
    break;

  case 143: /* distinct: %empty  */
#line 1217 "parser.y"
  {
    (yyval.bool_value) = false;
}
#line 4270 "parser.cpp"
    break;

  case 144: /* from_clause: FROM table_reference  */
#line 1221 "parser.y"
                                  {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4278 "parser.cpp"
    break;

  case 145: /* from_clause: %empty  */
#line 1224 "parser.y"
                       {
    (yyval.table_reference_t) = nullptr;
}
#line 4286 "parser.cpp"
    break;

  case 146: /* search_clause: SEARCH sub_search_array  */
#line 1228 "parser.y"
                                       {
    infinity::SearchExpr* search_expr = new infinity::SearchExpr();
    search_expr->SetExprs((yyvsp[0].expr_array_t));
    (yyval.expr_t) = search_expr;
}
#line 4296 "parser.cpp"
    break;

  case 147: /* search_clause: %empty  */
#line 1233 "parser.y"
                         {
    (yyval.expr_t) = nullptr;
}
#line 4304 "parser.cpp"
    break;

  case 148: /* where_clause: WHERE expr  */
#line 1237 "parser.y"
                         {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4312 "parser.cpp"
    break;

  case 149: /* where_clause: %empty  */
#line 1240 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4320 "parser.cpp"
    break;

  case 150: /* having_clause: HAVING expr  */
#line 1244 "parser.y"
                           {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4328 "parser.cpp"
    break;

  case 151: /* having_clause: %empty  */
#line 1247 "parser.y"
                        {
    (yyval.expr_t) = nullptr;
}
#line 4336 "parser.cpp"
    break;

  case 152: /* group_by_clause: GROUP BY expr_array  */
#line 1251 "parser.y"
                                     {
    (yyval.expr_array_t) = (yyvsp[0].expr_array_t);
}
#line 4344 "parser.cpp"
    break;

  case 153: /* group_by_clause: %empty  */
#line 1254 "parser.y"
  {
    (yyval.expr_array_t) = nullptr;
}
#line 4352 "parser.cpp"
    break;

  case 154: /* set_operator: UNION  */
#line 1258 "parser.y"
                     {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnion;
}
#line 4360 "parser.cpp"
    break;

  case 155: /* set_operator: UNION ALL  */
#line 1261 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kUnionAll;
}
#line 4368 "parser.cpp"
    break;

  case 156: /* set_operator: INTERSECT  */
#line 1264 "parser.y"
            {
    (yyval.set_operator_t) = infinity::SetOperatorType::kIntersect;
}
#line 4376 "parser.cpp"
    break;

  case 157: /* set_operator: EXCEPT  */
#line 1267 "parser.y"
         {
    (yyval.set_operator_t) = infinity::SetOperatorType::kExcept;
}
#line 4384 "parser.cpp"
    break;

  case 158: /* table_reference: table_reference_unit  */
#line 1275 "parser.y"
                                       {
    (yyval.table_reference_t) = (yyvsp[0].table_reference_t);
}
#line 4392 "parser.cpp"
    break;

  case 159: /* table_reference: table_reference ',' table_reference_unit  */
#line 1278 "parser.y"
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

  case 162: /* table_reference_name: table_name table_alias  */
#line 1295 "parser.y"
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

  case 163: /* table_reference_name: '(' select_statement ')' table_alias  */
#line 1309 "parser.y"
                                       {
    infinity::SubqueryReference* subquery_reference = new infinity::SubqueryReference();
    subquery_reference->select_statement_ = (yyvsp[-2].select_stmt);
    subquery_reference->alias_ = (yyvsp[0].table_alias_t);
    (yyval.table_reference_t) = subquery_reference;
}
#line 4439 "parser.cpp"
    break;

  case 164: /* table_name: IDENTIFIER  */
#line 1318 "parser.y"
                        {
    if(!result->IsError()) {
        (yyval.table_name_t) = new infinity::TableName();
        ParserHelper::ToLower((yyvsp[0].str_value));
        (yyval.table_name_t)->table_name_ptr_ = (yyvsp[0].str_value);
    }
}
#line 4451 "parser.cpp"
    break;

  case 165: /* table_name: IDENTIFIER '.' IDENTIFIER  */
#line 1325 "parser.y"
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

  case 166: /* table_alias: AS IDENTIFIER  */
#line 1336 "parser.y"
                            {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4475 "parser.cpp"
    break;

  case 167: /* table_alias: IDENTIFIER  */
#line 1341 "parser.y"
             {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[0].str_value);
}
#line 4485 "parser.cpp"
    break;

  case 168: /* table_alias: AS IDENTIFIER '(' identifier_array ')'  */
#line 1346 "parser.y"
                                         {
    (yyval.table_alias_t) = new infinity::TableAlias();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    (yyval.table_alias_t)->alias_ = (yyvsp[-3].str_value);
    (yyval.table_alias_t)->column_alias_array_ = (yyvsp[-1].identifier_array_t);
}
#line 4496 "parser.cpp"
    break;

  case 169: /* table_alias: %empty  */
#line 1352 "parser.y"
  {
    (yyval.table_alias_t) = nullptr;
}
#line 4504 "parser.cpp"
    break;

  case 170: /* with_clause: WITH with_expr_list  */
#line 1359 "parser.y"
                                  {
    (yyval.with_expr_list_t) = (yyvsp[0].with_expr_list_t);
}
#line 4512 "parser.cpp"
    break;

  case 171: /* with_clause: %empty  */
#line 1362 "parser.y"
                          {
    (yyval.with_expr_list_t) = nullptr;
}
#line 4520 "parser.cpp"
    break;

  case 172: /* with_expr_list: with_expr  */
#line 1366 "parser.y"
                          {
    (yyval.with_expr_list_t) = new std::vector<infinity::WithExpr*>();
    (yyval.with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
}
#line 4529 "parser.cpp"
    break;

  case 173: /* with_expr_list: with_expr_list ',' with_expr  */
#line 1369 "parser.y"
                                 {
    (yyvsp[-2].with_expr_list_t)->emplace_back((yyvsp[0].with_expr_t));
    (yyval.with_expr_list_t) = (yyvsp[-2].with_expr_list_t);
}
#line 4538 "parser.cpp"
    break;

  case 174: /* with_expr: IDENTIFIER AS '(' select_clause_with_modifier ')'  */
#line 1374 "parser.y"
                                                             {
    (yyval.with_expr_t) = new infinity::WithExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    (yyval.with_expr_t)->alias_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    (yyval.with_expr_t)->select_ = (yyvsp[-1].select_stmt);
}
#line 4550 "parser.cpp"
    break;

  case 175: /* join_clause: table_reference_unit NATURAL JOIN table_reference_name  */
#line 1386 "parser.y"
                                                                    {
    infinity::JoinReference* join_reference = new infinity::JoinReference();
    join_reference->left_ = (yyvsp[-3].table_reference_t);
    join_reference->right_ = (yyvsp[0].table_reference_t);
    join_reference->join_type_ = infinity::JoinType::kNatural;
    (yyval.table_reference_t) = join_reference;
}
#line 4562 "parser.cpp"
    break;

  case 176: /* join_clause: table_reference_unit join_type JOIN table_reference_name ON expr  */
#line 1393 "parser.y"
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

  case 177: /* join_type: INNER  */
#line 1407 "parser.y"
                  {
    (yyval.join_type_t) = infinity::JoinType::kInner;
}
#line 4583 "parser.cpp"
    break;

  case 178: /* join_type: LEFT  */
#line 1410 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kLeft;
}
#line 4591 "parser.cpp"
    break;

  case 179: /* join_type: RIGHT  */
#line 1413 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kRight;
}
#line 4599 "parser.cpp"
    break;

  case 180: /* join_type: OUTER  */
#line 1416 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4607 "parser.cpp"
    break;

  case 181: /* join_type: FULL  */
#line 1419 "parser.y"
       {
    (yyval.join_type_t) = infinity::JoinType::kFull;
}
#line 4615 "parser.cpp"
    break;

  case 182: /* join_type: CROSS  */
#line 1422 "parser.y"
        {
    (yyval.join_type_t) = infinity::JoinType::kCross;
}
#line 4623 "parser.cpp"
    break;

  case 183: /* join_type: %empty  */
#line 1425 "parser.y"
                {
}
#line 4630 "parser.cpp"
    break;

  case 184: /* show_statement: SHOW DATABASES  */
#line 1431 "parser.y"
                               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kDatabases;
}
#line 4639 "parser.cpp"
    break;

  case 185: /* show_statement: SHOW TABLES  */
#line 1435 "parser.y"
              {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kTables;
}
#line 4648 "parser.cpp"
    break;

  case 186: /* show_statement: SHOW VIEWS  */
#line 1439 "parser.y"
             {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kViews;
}
#line 4657 "parser.cpp"
    break;

  case 187: /* show_statement: SHOW CONFIGS  */
#line 1443 "parser.y"
               {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kConfigs;
}
#line 4666 "parser.cpp"
    break;

  case 188: /* show_statement: SHOW PROFILES  */
#line 1447 "parser.y"
                {
    (yyval.show_stmt) = new infinity::ShowStatement();
    (yyval.show_stmt)->show_type_ = infinity::ShowStmtType::kProfiles;
}
#line 4675 "parser.cpp"
    break;

  case 189: /* show_statement: DESCRIBE table_name  */
#line 1451 "parser.y"
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

  case 190: /* show_statement: DESCRIBE INDEX table_name  */
#line 1462 "parser.y"
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
#line 4707 "parser.cpp"
    break;

  case 191: /* flush_statement: FLUSH DATA  */
#line 1477 "parser.y"
                            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kData;
}
#line 4716 "parser.cpp"
    break;

  case 192: /* flush_statement: FLUSH LOG  */
#line 1481 "parser.y"
            {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kLog;
}
#line 4725 "parser.cpp"
    break;

  case 193: /* flush_statement: FLUSH BUFFER  */
#line 1485 "parser.y"
               {
    (yyval.flush_stmt) = new infinity::FlushStatement();
    (yyval.flush_stmt)->type_ = infinity::FlushType::kBuffer;
}
#line 4734 "parser.cpp"
    break;

  case 194: /* command_statement: USE IDENTIFIER  */
#line 1493 "parser.y"
                                  {
    (yyval.command_stmt) = new infinity::CommandStatement();
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::UseCmd>((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
}
#line 4745 "parser.cpp"
    break;

  case 195: /* command_statement: EXPORT PROFILE LONG_VALUE file_path  */
#line 1499 "parser.y"
                                      {
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::ExportCmd>((yyvsp[0].str_value), infinity::ExportType::kProfileRecord, (yyvsp[-1].long_value));
    free((yyvsp[0].str_value));
}
#line 4755 "parser.cpp"
    break;

  case 196: /* command_statement: SET SESSION IDENTIFIER ON  */
#line 1504 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4766 "parser.cpp"
    break;

  case 197: /* command_statement: SET SESSION IDENTIFIER OFF  */
#line 1510 "parser.y"
                             {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4777 "parser.cpp"
    break;

  case 198: /* command_statement: SET SESSION IDENTIFIER STRING  */
#line 1516 "parser.y"
                                {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4790 "parser.cpp"
    break;

  case 199: /* command_statement: SET SESSION IDENTIFIER LONG_VALUE  */
#line 1524 "parser.y"
                                    {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4801 "parser.cpp"
    break;

  case 200: /* command_statement: SET SESSION IDENTIFIER DOUBLE_VALUE  */
#line 1530 "parser.y"
                                      {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4812 "parser.cpp"
    break;

  case 201: /* command_statement: SET GLOBAL IDENTIFIER ON  */
#line 1536 "parser.y"
                           {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), true);
    free((yyvsp[-1].str_value));
}
#line 4823 "parser.cpp"
    break;

  case 202: /* command_statement: SET GLOBAL IDENTIFIER OFF  */
#line 1542 "parser.y"
                            {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kBool, (yyvsp[-1].str_value), false);
    free((yyvsp[-1].str_value));
}
#line 4834 "parser.cpp"
    break;

  case 203: /* command_statement: SET GLOBAL IDENTIFIER STRING  */
#line 1548 "parser.y"
                               {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kGlobal, infinity::SetVarType::kString, (yyvsp[-1].str_value), (yyvsp[0].str_value));
    free((yyvsp[-1].str_value));
    free((yyvsp[0].str_value));
}
#line 4847 "parser.cpp"
    break;

  case 204: /* command_statement: SET GLOBAL IDENTIFIER LONG_VALUE  */
#line 1556 "parser.y"
                                   {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kInteger, (yyvsp[-1].str_value), (yyvsp[0].long_value));
    free((yyvsp[-1].str_value));
}
#line 4858 "parser.cpp"
    break;

  case 205: /* command_statement: SET GLOBAL IDENTIFIER DOUBLE_VALUE  */
#line 1562 "parser.y"
                                     {
    ParserHelper::ToLower((yyvsp[-1].str_value));
    (yyval.command_stmt) = new infinity::CommandStatement();
    (yyval.command_stmt)->command_info_ = std::make_shared<infinity::SetCmd>(infinity::SetScope::kSession, infinity::SetVarType::kDouble, (yyvsp[-1].str_value), (yyvsp[0].double_value));
    free((yyvsp[-1].str_value));
}
#line 4869 "parser.cpp"
    break;

  case 206: /* expr_array: expr_alias  */
#line 1573 "parser.y"
                        {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 4878 "parser.cpp"
    break;

  case 207: /* expr_array: expr_array ',' expr_alias  */
#line 1577 "parser.y"
                            {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 4887 "parser.cpp"
    break;

  case 208: /* expr_array_list: '(' expr_array ')'  */
#line 1582 "parser.y"
                                     {
    (yyval.expr_array_list_t) = new std::vector<std::vector<infinity::ParsedExpr*>*>();
    (yyval.expr_array_list_t)->push_back((yyvsp[-1].expr_array_t));
}
#line 4896 "parser.cpp"
    break;

  case 209: /* expr_array_list: expr_array_list ',' '(' expr_array ')'  */
#line 1586 "parser.y"
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
#line 4916 "parser.cpp"
    break;

  case 210: /* expr_alias: expr AS IDENTIFIER  */
#line 1613 "parser.y"
                                {
    (yyval.expr_t) = (yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.expr_t)->alias_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 4927 "parser.cpp"
    break;

  case 211: /* expr_alias: expr  */
#line 1619 "parser.y"
       {
    (yyval.expr_t) = (yyvsp[0].expr_t);
}
#line 4935 "parser.cpp"
    break;

  case 217: /* operand: '(' expr ')'  */
#line 1629 "parser.y"
                      {
   (yyval.expr_t) = (yyvsp[-1].expr_t);
}
#line 4943 "parser.cpp"
    break;

  case 218: /* operand: '(' select_without_paren ')'  */
#line 1632 "parser.y"
                               {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kScalar;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 4954 "parser.cpp"
    break;

  case 219: /* operand: constant_expr  */
#line 1638 "parser.y"
                {
    (yyval.expr_t) = (yyvsp[0].const_expr_t);
}
#line 4962 "parser.cpp"
    break;

  case 228: /* knn_expr: KNN '(' expr ',' array_expr ',' STRING ',' STRING ')'  */
#line 1650 "parser.y"
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
#line 5107 "parser.cpp"
    break;

  case 229: /* match_expr: MATCH '(' STRING ',' STRING ')'  */
#line 1791 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    int rc = match_expr->SetFilter((yyvsp[-3].str_value), (yyvsp[-1].str_value));
    free((yyvsp[-3].str_value));
    free((yyvsp[-1].str_value));
    if(rc!=0)
        yyerror(&yyloc, scanner, result, "Invalid match_expr");
    (yyval.expr_t) = match_expr;
}
#line 5121 "parser.cpp"
    break;

  case 230: /* match_expr: MATCH '(' STRING ',' STRING ',' STRING ')'  */
#line 1800 "parser.y"
                                             {
    infinity::MatchExpr* match_expr = new infinity::MatchExpr();
    match_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    int rc = match_expr->SetFilter((yyvsp[-5].str_value), (yyvsp[-3].str_value));
    if(rc!=0)
        yyerror(&yyloc, scanner, result, "Invalid match_expr");
    (yyval.expr_t) = match_expr;
}
#line 5135 "parser.cpp"
    break;

  case 231: /* query_expr: QUERY '(' STRING ')'  */
#line 1810 "parser.y"
                                  {
    infinity::QueryExpr* query_expr = new infinity::QueryExpr();
    int rc = query_expr->SetFilter((yyvsp[-1].str_value));
    if(rc!=0)
        yyerror(&yyloc, scanner, result, "Invalid query_expr");
    (yyval.expr_t) = query_expr;
}
#line 5147 "parser.cpp"
    break;

  case 232: /* query_expr: QUERY '(' STRING ',' STRING ')'  */
#line 1817 "parser.y"
                                  {
    infinity::QueryExpr* query_expr = new infinity::QueryExpr();
    query_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    int rc = query_expr->SetFilter((yyvsp[-3].str_value));
    if(rc!=0)
        yyerror(&yyloc, scanner, result, "Invalid query_expr");
    (yyval.expr_t) = query_expr;
}
#line 5161 "parser.cpp"
    break;

  case 233: /* fusion_expr: FUSION '(' STRING ',' STRING ')'  */
#line 1827 "parser.y"
                                               {
    infinity::FusionExpr* fusion_expr = new infinity::FusionExpr();
    fusion_expr->method_ = std::move(std::string((yyvsp[-3].str_value)));
    free((yyvsp[-3].str_value));
    fusion_expr->SetOptions((yyvsp[-1].str_value));
    free((yyvsp[-1].str_value));
    (yyval.expr_t) = fusion_expr;
}
#line 5174 "parser.cpp"
    break;

  case 234: /* sub_search_array: knn_expr  */
#line 1837 "parser.y"
                            {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5183 "parser.cpp"
    break;

  case 235: /* sub_search_array: match_expr  */
#line 1841 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5192 "parser.cpp"
    break;

  case 236: /* sub_search_array: query_expr  */
#line 1845 "parser.y"
             {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5201 "parser.cpp"
    break;

  case 237: /* sub_search_array: fusion_expr  */
#line 1849 "parser.y"
              {
    (yyval.expr_array_t) = new std::vector<infinity::ParsedExpr*>();
    (yyval.expr_array_t)->emplace_back((yyvsp[0].expr_t));
}
#line 5210 "parser.cpp"
    break;

  case 238: /* sub_search_array: sub_search_array ',' knn_expr  */
#line 1853 "parser.y"
                                {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5219 "parser.cpp"
    break;

  case 239: /* sub_search_array: sub_search_array ',' match_expr  */
#line 1857 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5228 "parser.cpp"
    break;

  case 240: /* sub_search_array: sub_search_array ',' query_expr  */
#line 1861 "parser.y"
                                  {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5237 "parser.cpp"
    break;

  case 241: /* sub_search_array: sub_search_array ',' fusion_expr  */
#line 1865 "parser.y"
                                   {
    (yyvsp[-2].expr_array_t)->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_array_t) = (yyvsp[-2].expr_array_t);
}
#line 5246 "parser.cpp"
    break;

  case 242: /* function_expr: IDENTIFIER '(' ')'  */
#line 1870 "parser.y"
                                   {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-2].str_value));
    func_expr->func_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));
    func_expr->arguments_ = nullptr;
    (yyval.expr_t) = func_expr;
}
#line 5259 "parser.cpp"
    break;

  case 243: /* function_expr: IDENTIFIER '(' expr_array ')'  */
#line 1878 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-3].str_value));
    func_expr->func_name_ = (yyvsp[-3].str_value);
    free((yyvsp[-3].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = func_expr;
}
#line 5272 "parser.cpp"
    break;

  case 244: /* function_expr: IDENTIFIER '(' DISTINCT expr_array ')'  */
#line 1886 "parser.y"
                                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    ParserHelper::ToLower((yyvsp[-4].str_value));
    func_expr->func_name_ = (yyvsp[-4].str_value);
    free((yyvsp[-4].str_value));
    func_expr->arguments_ = (yyvsp[-1].expr_array_t);
    func_expr->distinct_ = true;
    (yyval.expr_t) = func_expr;
}
#line 5286 "parser.cpp"
    break;

  case 245: /* function_expr: operand IS NOT NULLABLE  */
#line 1895 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_not_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5298 "parser.cpp"
    break;

  case 246: /* function_expr: operand IS NULLABLE  */
#line 1902 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "is_null";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5310 "parser.cpp"
    break;

  case 247: /* function_expr: NOT operand  */
#line 1909 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5322 "parser.cpp"
    break;

  case 248: /* function_expr: '-' operand  */
#line 1916 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5334 "parser.cpp"
    break;

  case 249: /* function_expr: '+' operand  */
#line 1923 "parser.y"
              {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5346 "parser.cpp"
    break;

  case 250: /* function_expr: operand '-' operand  */
#line 1930 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "-";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5359 "parser.cpp"
    break;

  case 251: /* function_expr: operand '+' operand  */
#line 1938 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "+";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5372 "parser.cpp"
    break;

  case 252: /* function_expr: operand '*' operand  */
#line 1946 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "*";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5385 "parser.cpp"
    break;

  case 253: /* function_expr: operand '/' operand  */
#line 1954 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "/";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5398 "parser.cpp"
    break;

  case 254: /* function_expr: operand '%' operand  */
#line 1962 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "%";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5411 "parser.cpp"
    break;

  case 255: /* function_expr: operand '=' operand  */
#line 1970 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5424 "parser.cpp"
    break;

  case 256: /* function_expr: operand EQUAL operand  */
#line 1978 "parser.y"
                        {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5437 "parser.cpp"
    break;

  case 257: /* function_expr: operand NOT_EQ operand  */
#line 1986 "parser.y"
                         {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<>";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5450 "parser.cpp"
    break;

  case 258: /* function_expr: operand '<' operand  */
#line 1994 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5463 "parser.cpp"
    break;

  case 259: /* function_expr: operand '>' operand  */
#line 2002 "parser.y"
                      {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5476 "parser.cpp"
    break;

  case 260: /* function_expr: operand LESS_EQ operand  */
#line 2010 "parser.y"
                          {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "<=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5489 "parser.cpp"
    break;

  case 261: /* function_expr: operand GREATER_EQ operand  */
#line 2018 "parser.y"
                             {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = ">=";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5502 "parser.cpp"
    break;

  case 262: /* function_expr: EXTRACT '(' STRING FROM operand ')'  */
#line 2026 "parser.y"
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
#line 5537 "parser.cpp"
    break;

  case 263: /* function_expr: operand LIKE operand  */
#line 2056 "parser.y"
                       {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5550 "parser.cpp"
    break;

  case 264: /* function_expr: operand NOT LIKE operand  */
#line 2064 "parser.y"
                           {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "not_like";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-3].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5563 "parser.cpp"
    break;

  case 265: /* conjunction_expr: expr AND expr  */
#line 2073 "parser.y"
                                {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "and";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5576 "parser.cpp"
    break;

  case 266: /* conjunction_expr: expr OR expr  */
#line 2081 "parser.y"
               {
    infinity::FunctionExpr* func_expr = new infinity::FunctionExpr();
    func_expr->func_name_ = "or";
    func_expr->arguments_ = new std::vector<infinity::ParsedExpr*>();
    func_expr->arguments_->emplace_back((yyvsp[-2].expr_t));
    func_expr->arguments_->emplace_back((yyvsp[0].expr_t));
    (yyval.expr_t) = func_expr;
}
#line 5589 "parser.cpp"
    break;

  case 267: /* between_expr: operand BETWEEN operand AND operand  */
#line 2090 "parser.y"
                                                  {
    infinity::BetweenExpr* between_expr = new infinity::BetweenExpr();
    between_expr->value_ = (yyvsp[-4].expr_t);
    between_expr->lower_bound_ = (yyvsp[-2].expr_t);
    between_expr->upper_bound_ = (yyvsp[0].expr_t);
    (yyval.expr_t) = between_expr;
}
#line 5601 "parser.cpp"
    break;

  case 268: /* in_expr: operand IN '(' expr_array ')'  */
#line 2098 "parser.y"
                                       {
    infinity::InExpr* in_expr = new infinity::InExpr(true);
    in_expr->left_ = (yyvsp[-4].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5612 "parser.cpp"
    break;

  case 269: /* in_expr: operand NOT IN '(' expr_array ')'  */
#line 2104 "parser.y"
                                    {
    infinity::InExpr* in_expr = new infinity::InExpr(false);
    in_expr->left_ = (yyvsp[-5].expr_t);
    in_expr->arguments_ = (yyvsp[-1].expr_array_t);
    (yyval.expr_t) = in_expr;
}
#line 5623 "parser.cpp"
    break;

  case 270: /* case_expr: CASE expr case_check_array END  */
#line 2111 "parser.y"
                                          {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-2].expr_t);
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5634 "parser.cpp"
    break;

  case 271: /* case_expr: CASE expr case_check_array ELSE expr END  */
#line 2117 "parser.y"
                                           {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->expr_ = (yyvsp[-4].expr_t);
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5646 "parser.cpp"
    break;

  case 272: /* case_expr: CASE case_check_array END  */
#line 2124 "parser.y"
                            {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-1].case_check_array_t);
    (yyval.expr_t) = case_expr;
}
#line 5656 "parser.cpp"
    break;

  case 273: /* case_expr: CASE case_check_array ELSE expr END  */
#line 2129 "parser.y"
                                      {
    infinity::CaseExpr* case_expr = new infinity::CaseExpr();
    case_expr->case_check_array_ = (yyvsp[-3].case_check_array_t);
    case_expr->else_expr_ = (yyvsp[-1].expr_t);
    (yyval.expr_t) = case_expr;
}
#line 5667 "parser.cpp"
    break;

  case 274: /* case_check_array: WHEN expr THEN expr  */
#line 2136 "parser.y"
                                      {
    (yyval.case_check_array_t) = new std::vector<infinity::WhenThen*>();
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyval.case_check_array_t)->emplace_back(when_then_ptr);
}
#line 5679 "parser.cpp"
    break;

  case 275: /* case_check_array: case_check_array WHEN expr THEN expr  */
#line 2143 "parser.y"
                                       {
    infinity::WhenThen* when_then_ptr = new infinity::WhenThen();
    when_then_ptr->when_ = (yyvsp[-2].expr_t);
    when_then_ptr->then_ = (yyvsp[0].expr_t);
    (yyvsp[-4].case_check_array_t)->emplace_back(when_then_ptr);
    (yyval.case_check_array_t) = (yyvsp[-4].case_check_array_t);
}
#line 5691 "parser.cpp"
    break;

  case 276: /* cast_expr: CAST '(' expr AS column_type ')'  */
#line 2151 "parser.y"
                                            {
    std::shared_ptr<infinity::TypeInfo> type_info_ptr{nullptr};
    switch((yyvsp[-1].column_type_t).logical_type_) {
        case infinity::LogicalType::kDecimal: {
            type_info_ptr = infinity::DecimalInfo::Make((yyvsp[-1].column_type_t).precision, (yyvsp[-1].column_type_t).scale);
            break;
        }
        case infinity::LogicalType::kBitmap: {
            type_info_ptr = infinity::BitmapInfo::Make((yyvsp[-1].column_type_t).width);
            break;
        }
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
#line 5719 "parser.cpp"
    break;

  case 277: /* subquery_expr: EXISTS '(' select_without_paren ')'  */
#line 2175 "parser.y"
                                                   {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5730 "parser.cpp"
    break;

  case 278: /* subquery_expr: NOT EXISTS '(' select_without_paren ')'  */
#line 2181 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotExists;
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5741 "parser.cpp"
    break;

  case 279: /* subquery_expr: operand IN '(' select_without_paren ')'  */
#line 2187 "parser.y"
                                          {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kIn;
    subquery_expr->left_ = (yyvsp[-4].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5753 "parser.cpp"
    break;

  case 280: /* subquery_expr: operand NOT IN '(' select_without_paren ')'  */
#line 2194 "parser.y"
                                              {
    infinity::SubqueryExpr* subquery_expr = new infinity::SubqueryExpr();
    subquery_expr->subquery_type_ = infinity::SubqueryType::kNotIn;
    subquery_expr->left_ = (yyvsp[-5].expr_t);
    subquery_expr->select_ = (yyvsp[-1].select_stmt);
    (yyval.expr_t) = subquery_expr;
}
#line 5765 "parser.cpp"
    break;

  case 281: /* column_expr: IDENTIFIER  */
#line 2202 "parser.y"
                         {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5777 "parser.cpp"
    break;

  case 282: /* column_expr: column_expr '.' IDENTIFIER  */
#line 2209 "parser.y"
                             {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    ParserHelper::ToLower((yyvsp[0].str_value));
    column_expr->names_.emplace_back((yyvsp[0].str_value));
    free((yyvsp[0].str_value));
    (yyval.expr_t) = column_expr;
}
#line 5789 "parser.cpp"
    break;

  case 283: /* column_expr: '*'  */
#line 2216 "parser.y"
      {
    infinity::ColumnExpr* column_expr = new infinity::ColumnExpr();
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5799 "parser.cpp"
    break;

  case 284: /* column_expr: column_expr '.' '*'  */
#line 2221 "parser.y"
                      {
    infinity::ColumnExpr* column_expr = (infinity::ColumnExpr*)(yyvsp[-2].expr_t);
    if(column_expr->star_) {
        yyerror(&yyloc, scanner, result, "Invalid column expression format");
        YYERROR;
    }
    column_expr->star_ = true;
    (yyval.expr_t) = column_expr;
}
#line 5813 "parser.cpp"
    break;

  case 285: /* constant_expr: STRING  */
#line 2231 "parser.y"
                      {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kString);
    const_expr->str_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5823 "parser.cpp"
    break;

  case 286: /* constant_expr: TRUE  */
#line 2236 "parser.y"
       {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = true;
    (yyval.const_expr_t) = const_expr;
}
#line 5833 "parser.cpp"
    break;

  case 287: /* constant_expr: FALSE  */
#line 2241 "parser.y"
        {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kBoolean);
    const_expr->bool_value_ = false;
    (yyval.const_expr_t) = const_expr;
}
#line 5843 "parser.cpp"
    break;

  case 288: /* constant_expr: DOUBLE_VALUE  */
#line 2246 "parser.y"
               {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDouble);
    const_expr->double_value_ = (yyvsp[0].double_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5853 "parser.cpp"
    break;

  case 289: /* constant_expr: LONG_VALUE  */
#line 2251 "parser.y"
             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInteger);
    const_expr->integer_value_ = (yyvsp[0].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5863 "parser.cpp"
    break;

  case 290: /* constant_expr: DATE STRING  */
#line 2256 "parser.y"
              {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDate);
    const_expr->date_value_ = (yyvsp[0].str_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5873 "parser.cpp"
    break;

  case 291: /* constant_expr: INTERVAL interval_expr  */
#line 2261 "parser.y"
                         {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5881 "parser.cpp"
    break;

  case 292: /* constant_expr: interval_expr  */
#line 2264 "parser.y"
                {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5889 "parser.cpp"
    break;

  case 293: /* array_expr: long_array_expr  */
#line 2276 "parser.y"
                            {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5897 "parser.cpp"
    break;

  case 294: /* array_expr: double_array_expr  */
#line 2279 "parser.y"
                    {
    (yyval.const_expr_t) = (yyvsp[0].const_expr_t);
}
#line 5905 "parser.cpp"
    break;

  case 295: /* long_array_expr: unclosed_long_array_expr ']'  */
#line 2283 "parser.y"
                                              {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5913 "parser.cpp"
    break;

  case 296: /* unclosed_long_array_expr: '[' LONG_VALUE  */
#line 2287 "parser.y"
                                         {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kIntegerArray);
    const_expr->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5923 "parser.cpp"
    break;

  case 297: /* unclosed_long_array_expr: unclosed_long_array_expr ',' LONG_VALUE  */
#line 2292 "parser.y"
                                          {
    (yyvsp[-2].const_expr_t)->long_array_.emplace_back((yyvsp[0].long_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 5932 "parser.cpp"
    break;

  case 298: /* double_array_expr: unclosed_double_array_expr ']'  */
#line 2297 "parser.y"
                                                  {
    (yyval.const_expr_t) = (yyvsp[-1].const_expr_t);
}
#line 5940 "parser.cpp"
    break;

  case 299: /* unclosed_double_array_expr: '[' DOUBLE_VALUE  */
#line 2301 "parser.y"
                                             {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kDoubleArray);
    const_expr->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = const_expr;
}
#line 5950 "parser.cpp"
    break;

  case 300: /* unclosed_double_array_expr: unclosed_double_array_expr ',' DOUBLE_VALUE  */
#line 2306 "parser.y"
                                              {
    (yyvsp[-2].const_expr_t)->double_array_.emplace_back((yyvsp[0].double_value));
    (yyval.const_expr_t) = (yyvsp[-2].const_expr_t);
}
#line 5959 "parser.cpp"
    break;

  case 301: /* interval_expr: LONG_VALUE SECONDS  */
#line 2311 "parser.y"
                                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5970 "parser.cpp"
    break;

  case 302: /* interval_expr: LONG_VALUE SECOND  */
#line 2317 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kSecond;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5981 "parser.cpp"
    break;

  case 303: /* interval_expr: LONG_VALUE MINUTES  */
#line 2323 "parser.y"
                     {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 5992 "parser.cpp"
    break;

  case 304: /* interval_expr: LONG_VALUE MINUTE  */
#line 2329 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMinute;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6003 "parser.cpp"
    break;

  case 305: /* interval_expr: LONG_VALUE HOURS  */
#line 2335 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6014 "parser.cpp"
    break;

  case 306: /* interval_expr: LONG_VALUE HOUR  */
#line 2341 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kHour;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6025 "parser.cpp"
    break;

  case 307: /* interval_expr: LONG_VALUE DAYS  */
#line 2347 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6036 "parser.cpp"
    break;

  case 308: /* interval_expr: LONG_VALUE DAY  */
#line 2353 "parser.y"
                 {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kDay;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6047 "parser.cpp"
    break;

  case 309: /* interval_expr: LONG_VALUE MONTHS  */
#line 2359 "parser.y"
                    {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6058 "parser.cpp"
    break;

  case 310: /* interval_expr: LONG_VALUE MONTH  */
#line 2365 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kMonth;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6069 "parser.cpp"
    break;

  case 311: /* interval_expr: LONG_VALUE YEARS  */
#line 2371 "parser.y"
                   {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6080 "parser.cpp"
    break;

  case 312: /* interval_expr: LONG_VALUE YEAR  */
#line 2377 "parser.y"
                  {
    infinity::ConstantExpr* const_expr = new infinity::ConstantExpr(infinity::LiteralType::kInterval);
    const_expr->interval_type_ = infinity::TimeUnit::kYear;
    const_expr->integer_value_ = (yyvsp[-1].long_value);
    (yyval.const_expr_t) = const_expr;
}
#line 6091 "parser.cpp"
    break;

  case 313: /* copy_option_list: copy_option  */
#line 2388 "parser.y"
                               {
    (yyval.copy_option_array) = new std::vector<infinity::CopyOption*>();
    (yyval.copy_option_array)->push_back((yyvsp[0].copy_option_t));
}
#line 6100 "parser.cpp"
    break;

  case 314: /* copy_option_list: copy_option_list ',' copy_option  */
#line 2392 "parser.y"
                                   {
    (yyvsp[-2].copy_option_array)->push_back((yyvsp[0].copy_option_t));
    (yyval.copy_option_array) = (yyvsp[-2].copy_option_array);
}
#line 6109 "parser.cpp"
    break;

  case 315: /* copy_option: FORMAT IDENTIFIER  */
#line 2397 "parser.y"
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
#line 6133 "parser.cpp"
    break;

  case 316: /* copy_option: DELIMITER STRING  */
#line 2416 "parser.y"
                   {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kDelimiter;
    (yyval.copy_option_t)->delimiter_ = (yyvsp[0].str_value)[0];
    free((yyvsp[0].str_value));
}
#line 6144 "parser.cpp"
    break;

  case 317: /* copy_option: HEADER  */
#line 2422 "parser.y"
         {
    (yyval.copy_option_t) = new infinity::CopyOption();
    (yyval.copy_option_t)->option_type_ = infinity::CopyOptionType::kHeader;
    (yyval.copy_option_t)->header_ = true;
}
#line 6154 "parser.cpp"
    break;

  case 318: /* file_path: STRING  */
#line 2428 "parser.y"
                   {
    (yyval.str_value) = (yyvsp[0].str_value);
}
#line 6162 "parser.cpp"
    break;

  case 319: /* if_exists: IF EXISTS  */
#line 2432 "parser.y"
                     { (yyval.bool_value) = true; }
#line 6168 "parser.cpp"
    break;

  case 320: /* if_exists: %empty  */
#line 2433 "parser.y"
  { (yyval.bool_value) = false; }
#line 6174 "parser.cpp"
    break;

  case 321: /* if_not_exists: IF NOT EXISTS  */
#line 2435 "parser.y"
                              { (yyval.bool_value) = true; }
#line 6180 "parser.cpp"
    break;

  case 322: /* if_not_exists: %empty  */
#line 2436 "parser.y"
  { (yyval.bool_value) = false; }
#line 6186 "parser.cpp"
    break;

  case 325: /* if_not_exists_info: if_not_exists IDENTIFIER  */
#line 2451 "parser.y"
                                              {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
    (yyval.if_not_exists_info_t)->exists_ = true;
    (yyval.if_not_exists_info_t)->if_not_exists_ = (yyvsp[-1].bool_value);
    ParserHelper::ToLower((yyvsp[0].str_value));
    (yyval.if_not_exists_info_t)->info_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6199 "parser.cpp"
    break;

  case 326: /* if_not_exists_info: %empty  */
#line 2459 "parser.y"
  {
    (yyval.if_not_exists_info_t) = new infinity::IfNotExistsInfo();
}
#line 6207 "parser.cpp"
    break;

  case 327: /* with_index_para_list: WITH '(' index_para_list ')'  */
#line 2464 "parser.y"
                                                    {
    (yyval.with_index_para_list_t) = std::move((yyvsp[-1].index_para_list_t));
}
#line 6215 "parser.cpp"
    break;

  case 328: /* with_index_para_list: %empty  */
#line 2467 "parser.y"
  {
    (yyval.with_index_para_list_t) = new std::vector<infinity::InitParameter*>();
}
#line 6223 "parser.cpp"
    break;

  case 329: /* index_para_list: index_para  */
#line 2471 "parser.y"
                             {
    (yyval.index_para_list_t) = new std::vector<infinity::InitParameter*>();
    (yyval.index_para_list_t)->push_back((yyvsp[0].index_para_t));
}
#line 6232 "parser.cpp"
    break;

  case 330: /* index_para_list: index_para_list ',' index_para  */
#line 2475 "parser.y"
                                 {
    (yyvsp[-2].index_para_list_t)->push_back((yyvsp[0].index_para_t));
    (yyval.index_para_list_t) = (yyvsp[-2].index_para_list_t);
}
#line 6241 "parser.cpp"
    break;

  case 331: /* index_para: IDENTIFIER  */
#line 2480 "parser.y"
                        {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6251 "parser.cpp"
    break;

  case 332: /* index_para: IDENTIFIER '=' IDENTIFIER  */
#line 2485 "parser.y"
                            {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = (yyvsp[0].str_value);
    free((yyvsp[0].str_value));
}
#line 6264 "parser.cpp"
    break;

  case 333: /* index_para: IDENTIFIER '=' LONG_VALUE  */
#line 2493 "parser.y"
                            {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = std::to_string((yyvsp[0].long_value));
}
#line 6276 "parser.cpp"
    break;

  case 334: /* index_para: IDENTIFIER '=' DOUBLE_VALUE  */
#line 2500 "parser.y"
                              {
    (yyval.index_para_t) = new infinity::InitParameter();
    (yyval.index_para_t)->para_name_ = (yyvsp[-2].str_value);
    free((yyvsp[-2].str_value));

    (yyval.index_para_t)->para_value_ = std::to_string((yyvsp[0].double_value));
}
#line 6288 "parser.cpp"
    break;


#line 6292 "parser.cpp"

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

#line 2508 "parser.y"


void
yyerror(YYLTYPE * llocp, void* lexer, infinity::ParserResult* result, const char* msg) {
    if(result->IsError()) return ;

    result->error_message_ = std::string(msg) + ", " + std::to_string(llocp->first_column);
	fprintf(stderr, "Error: %s, %d:%d\n", msg, llocp->first_line, llocp->first_column);
}
